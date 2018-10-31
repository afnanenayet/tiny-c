/**
 * \file llvm_utils.c
 * \author Afnan Enayet
 * \brief Utility functions for interacting with LLVM IR files
 *
 * This file contains function definitions for the `llvm_utils` module.
 * This module is a collection of functions meant to make it easier for the
 * user to interact with LLVM IR code, so it's easier to optimize.
 */
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "llvm_utils.h"
#include "print_utils.h"
#include "vec.h"

LLVMModuleRef createLLVMModel(char *fp) {
    char *err = 0;
    LLVMMemoryBufferRef ll_f = 0;
    LLVMModuleRef m = 0;

    // try to read a file with LLVM
    LLVMCreateMemoryBufferWithContentsOfFile(fp, &ll_f, &err);

    if (err != NULL) {
        fprintln(stderr, err);
        return NULL;
    }
    // See if LLVM can parse the IR file
    LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

    if (err != NULL) {
        fprintln(stderr, err);
    }
    return m;
}

bool walkBasicblocks(LLVMValueRef function) {
    bool changed = false;
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
         basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
        // TODO run optimization routine here
        val_vec_t *genSet = computeGenSet(basicBlock);
        vec_deinit(genSet); // TODO remove
        free(genSet);
        genSet = NULL;
    }
    return changed;
}

bool walkFunctions(LLVMModuleRef module) {
    bool changed = false;
    for (LLVMValueRef function = LLVMGetFirstFunction(module); function;
         function = LLVMGetNextFunction(function)) {
        // TODO do we need this? Instructions say there will only be one
        // function in the input file? Does it really even matter...?
        // const char* funcName = LLVMGetValueName(function);
        changed = changed || walkBasicblocks(function);
    }
    return changed;
}

val_vec_t *computeGenSet(LLVMBasicBlockRef bb) {
    // initialize the empty set
    // need to malloc because the library assumes the vector is already
    // allocated and doesn't initialize the struct for you
    val_vec_t *genSet = malloc(sizeof(val_vec_t));
    vec_init(genSet);

    // iterate over the instructions in the basic block
    for (LLVMValueRef inst = LLVMGetFirstInstruction(bb); inst;
         inst = LLVMGetNextInstruction(inst)) {
        // if the instruction is a store instruction, store it in the set
        if (LLVMIsAStoreInst(inst)) {
#ifdef DEBUG
            println("(gen set) found store instruction");
#endif
            vec_push(genSet, inst);
        }
        // get memory location of the store instruction
        LLVMValueRef newStoreLoc = LLVMGetOperand(inst, 1);

        // check to see if any of the instructions already stored in the
        // vector reference the same location, if so, remove them
        int i;
        LLVMValueRef val;
        vec_foreach(genSet, val, i) {
            LLVMValueRef oldStoreLoc = LLVMGetOperand(val, 1);

            if (oldStoreLoc == newStoreLoc) {
#ifdef DEBUG
                println("(gen set) redundant location");
#endif
                vec_remove(genSet, val);
            }
        }
    }
    return genSet;
}

val_vec_t *computeKillSet(LLVMBasicBlockRef bb, val_vec_t *S) {
    val_vec_t *killSet = malloc(sizeof(val_vec_t));
    vec_init(killSet);

    // loop through again, for each instruction I, check if it kills an
    // instruction in S and add that instruction to the kill set
    // This method works because all of the instructions are added in
    // order, so if we are looking at a store instruction, and it has a
    // match in the list, every instruction that comes after the current
    // one also follows it in the basic block, so it cannot be killed by
    // the current instruction, hence there is no point looking at any
    // of those instructions.
    for (LLVMValueRef inst = LLVMGetFirstInstruction(bb); inst;
         inst = LLVMGetNextInstruction(inst)) {
        // only need to look at instructions that store constants, otherwise
        // skip to next instruction
        // TODO: figure out if I'm checking the correct operand
        if (!(LLVMIsAStoreInst(inst) &&
              LLVMIsAConstant(LLVMGetOperand(inst, 1))))
            continue;

#ifdef DEBUG
        println("(kill set) found constant store instruction");
#endif

        LLVMValueRef currLoc = LLVMGetOperand(inst, 1);

        int i;            // loop index iterator
        LLVMValueRef val; // loop value iterator
        vec_foreach(S, val, i) {
            LLVMValueRef listLoc = LLVMGetOperand(val, 1);

            // Break once we get to the current instruction because an
            // instruction can't kill itself or other store instructions
            // that come after it. If there is a match on the location,
            // we know that the current instruction kills this previous
            // instruction
            if (inst == val)
                break;
            else if (currLoc == listLoc) {
                vec_push(killSet, val);
#ifdef DEBUG
                println("(kill set) constant store instruction kills previous "
                        "instruction");
#endif
            }
        }
    }
    vec_deinit(S);
    return killSet;
}

val_vec_t *computeS(LLVMValueRef fn) {
    val_vec_t *S = malloc(sizeof(val_vec_t));
    vec_init(S);

    // loop through all of the basic blocks in the function, and loop
    // through each instruction in the basic blocks, adding appropriate
    // instructions to the set
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(fn); basicBlock;
         basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
        for (LLVMValueRef inst = LLVMGetFirstInstruction(basicBlock); inst;
             inst = LLVMGetNextInstruction(inst)) {
            if (LLVMIsAStoreInst(inst) &&
                LLVMIsAConstant(LLVMGetOperand(inst, 1))) {
                vec_push(S, inst);
            }
        }
    }
    return S;
}

meta_vec_t *computeBlockMData(LLVMValueRef fn, val_vec_t *S) {
    meta_vec_t *vec = malloc(sizeof(meta_vec_t));
    vec_init(vec);

    // a buffer for the running block of predecessors (the "in" set)
    val_vec_t preds;
    vec_init(&preds);

    // whether any change was applied
    bool changed = false;

    // initialize the metadata vector with each input set as the null set,
    // the output set as equal to GEN[B], and compute gen/kill for each of the
    // blocks
    for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(fn); basicBlock;
         basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
        meta_t *metadata = malloc(sizeof(meta_t));
        metadata->bb = basicBlock;

        // set gen/kill sets for the bb
        metadata->genSet = computeGenSet(basicBlock);
        metadata->killSet = computeKillSet(basicBlock, S);

        val_vec_t *inSet = malloc(sizeof(val_vec_t));
        vec_init(inSet);
        metadata->inSet = inSet;

        val_vec_t *outSet = malloc(sizeof(val_vec_t));
        vec_init(outSet);

        LLVMValueRef bbIterator;
        int idx;

        // copy gen to out
        vec_foreach(metadata->genSet, bbIterator, idx) {
            vec_push(outSet, bbIterator);
        }
        metadata->outSet = outSet;
        vec_push(vec, metadata);
    }

    // keep applying these changes until we reach a fixed point
    do {
        int idx0;
        meta_t *currMeta;

        // reset the predecessors vector
        vec_deinit(&preds);
        vec_init(&preds);

        // iterate through each basic block
        vec_foreach(vec, currMeta, idx0) {
            // in is the union of OUT from predecessors
            // loop through predecessors and add union of all preds to IN of
            // current bb
            int idx1;
            LLVMValueRef pred;

            vec_foreach(&preds, pred, idx1) {
                int found_idx = -1;

                // add to the predecessors if instruction isn't already there
                vec_find(currMeta->inSet, pred, found_idx);

                if (found_idx == -1)
                    vec_push(currMeta->inSet, pred);
            }

            // $ OUT[B] = GEN[B] \cup (IN[B] - KILL[B])
            // retain old out set for comparison
            // TODO de-allocate this
            val_vec_t *oldOut = currMeta->outSet;

            // IN[B] - KILL[B]
            val_vec_t *b = malloc(sizeof(val_vec_t));
            vec_init(b);

            int idx2;
            LLVMValueRef temp;

            // copy IN[B] to vector `b`
            vec_foreach(currMeta->inSet, temp, idx2) { vec_push(b, temp); }

            // copy KILL[B] to vector b if it's not in B
            // if it is in b, remove the element from b
            vec_foreach(currMeta->killSet, temp, idx2) {
                int found = -1;
                vec_find(b, temp, found);

                if (found == -1) {
                    vec_push(b, temp);
                } else {
                    vec_remove(b, temp);
                }
            }

            // set union between `b` and GEN[B]
            vec_foreach(currMeta->genSet, temp, idx2) {
                int found = -1;
                vec_find(b, temp, found);

                if (found == -1)
                    vec_push(b, temp);
            }
            currMeta->outSet = b;

            // compare the new OUT[B] and the old OUT[B] to see if there
            // were any changes this iteration
            vec_foreach(currMeta->outSet, temp, idx2) {
                int found = -1;
                vec_find(oldOut, temp, found);

                if (found == -1)
                    changed = true;
            }

            // need to compare both ways because a set equality requires us
            // to test whether A is a subset of B and B is a subset of A
            vec_foreach(oldOut, temp, idx2) {
                int found = -1;
                vec_find(currMeta->outSet, temp, found);

                if (found == -1)
                    changed = true;
            }
            vec_deinit(oldOut);
            free(oldOut);

            // add OUT[B] to the predecessors (union set op)
            vec_foreach(currMeta->outSet, temp, idx2) {
                int found = -1;
                vec_find(&preds, temp, found);

                if (found == -1)
                    vec_push(&preds, temp);
            }
        }
    } while (changed);
    return vec;
}
