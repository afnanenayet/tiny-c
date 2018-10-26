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

#pragma once

void optimize_program(void);
