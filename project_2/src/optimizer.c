/**
 * \file optimizer.c
 * \brief Contains functions pertaining to optimization of LLVM code
 *
 * This function contains function definitions for functions that provide
 * optimizations for LLVM code. These functions will modify the LLVM model
 * that is passed in.
 *
 * In this file, we implement constant propagation and constant folding.
 */

/**
 * \brief Optimize the program given the LLVM module
 *
 * This is essentially a wrapper function for the optimizations that
 * will be applied to the program. It contains a loop that calls each
 * optimization function and keeps looping/optimizing until each there are
 * no more changes applied, at which point we know the program has been
 * fully optimized.
 *
 * Right now, the following optimizations are called with this function:
 *     - constant propagation
 *     - constant folding
 */
void optimize_program(void);
