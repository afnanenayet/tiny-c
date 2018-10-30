/**
 * \file optimizer.c
 * \author Afnan Enayet
 * \brief Contains functions pertaining to optimization of LLVM code
 *
 * This module contains function definitions for functions that provide
 * optimizations for LLVM code. These functions will modify the LLVM model
 * that is passed in.
 *
 * In this file, we implement constant propagation and constant folding.
 */
#include <llvm-c/Core.h>
#include <stdbool.h>

#include "optimizer.h"
#include "llvm_utils.h"

/*** Private function prototypes ***/

/**
 * \brief Perform constant propagation on a module
 */
static void constProp(void);

/**
 * \brief Perform constant folding on a module
 */
static void constFold(void);


// TODO

/*** Private function definitions ***/

// TODO

/*** Public function definitions ***/

// TODO

void optimizeProgram(LLVMModuleRef m) {
    bool success = walkFunctions(m);
}
