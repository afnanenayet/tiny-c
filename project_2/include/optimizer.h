/**
 * \file optimizer.h
 * \author Afnan Enayet
 * \brief Methods to optimize LLVM IR code
 *
 * This module contains function definitions for functions that provide
 * optimizations for LLVM code. These functions will modify the LLVM model
 * that is passed in.
 *
 * In this file, we implement constant propagation and constant folding.
 */
#include <llvm-c/Core.h>

#pragma once

/**
 * \brief Optimize an LLVM program
 *
 * Given some LLVM model, optimize the program in the model until it reaches
 * the fixed point.
 *
 * \param m The module to optimize
 */
void optimize_program(LLVMModuleRef m);
