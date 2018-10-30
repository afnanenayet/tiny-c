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
    vec_init(storeSet);

    // compute some set S of all the constant store instructions in some
    // given function
    for (LLVMValueRef inst = LLVMGetFirstInstruction(bb); inst;
         inst = LLVMGetNextInstruction(inst)) {
        if (LLVMIsAStoreInst(inst)) {
#ifdef DEBUG
            println("(kill set) Found a store instruction");
#endif
            vec_push(storeSet, inst);
        }
    }
    vec_deinit(storeSet);
    free(storeSet);
    return killSet;
}
