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

#include "llvm_utils.h"
#include "print_utils.h"

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
