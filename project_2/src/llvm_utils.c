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

val_vec_t *computeKillSet(LLVMBasicBlockRef bb) {
    val_vec_t *killSet = malloc(sizeof(val_vec_t));
    vec_init(killSet);
    val_vec_t *storeSet = malloc(sizeof(val_vec_t));

    /// set $S$
    vec_init(storeSet);

    // compute some set S of all the constant store instructions in some
    // given function
    for (LLVMValueRef inst = LLVMGetFirstInstruction(bb); inst;
         inst = LLVMGetNextInstruction(inst)) {
        if (LLVMIsAStoreInst(inst)) {
#ifdef DEBUG
            println("(kill set) Found a store instruction to add to set S");
#endif
            vec_push(storeSet, inst);
        }
    }

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
        vec_foreach(storeSet, val, i) {
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
    // free storeset because it's not used outside the function
    vec_deinit(storeSet);
    free(storeSet);
    return killSet;
}
