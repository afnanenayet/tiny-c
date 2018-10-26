/**
 * \file llvm_utils.c
 * \brief Utility functions for interacting with LLVM IR files
 *
 * This file contains function definitions for the `llvm_utils` module.
 * This module is a collection of functions meant to make it easier for the
 * user to interact with LLVM IR code, so it's easier to optimize.
 */
#include <llvm-c/Core.h>
#include <llvm-c/IRReader.h>
#include <stdio.h>

#include "print_utils.h"

LLVMModuleRef createLLVMModel(char *fp) {
    char *err = 0;

    LLVMMemoryBufferRef ll_f = 0;
    LLVMModuleRef m = 0;

    LLVMCreateMemoryBufferWithContentsOfFile(fp, &ll_f, &err);

    if (err != NULL) {
        fprintln(stderr, err);
        return NULL;
    }

    LLVMParseIRInContext(LLVMGetGlobalContext(), ll_f, &m, &err);

    if (err != NULL) {
        fprintln(stderr, err);
    }

    return m;
}
