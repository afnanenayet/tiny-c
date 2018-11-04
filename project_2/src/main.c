/**
 * \file main.c
 * \author Afnan Enayet
 *
 * \brief The entry point into the optimizer program
 *
 * This file contains the entry point and the main function for the optimizer.
 * It is as minimal as possible and only calls a function to begin the logic
 * for the actual optimization code.
 */

#include <stdio.h>

#include "llvm_utils.h"
#include "optimizer.h"
#include "print_utils.h"

/**
 * \brief Main entry point into the program
 *
 * Parse command line arguments and call the main functions for the optimizer
 * program.
 */
int main(int argc, char *argv[]) {
    // bail out if there is no input file
    if (argc < 2) {
        fprintln(stderr, "Missing input filepath");
        return 1;
    }

    // check whether the given file path is valid
    char *fp = argv[1];
    LLVMModuleRef m = createLLVMModel(fp);

    if (m == NULL) {
        fprintln(stderr, "Invalid filepath or file received");
        return 1;
    }
    optimizeProgram(m);

    // print LLVM program to stdout
    LLVMDumpModule(m);

    // deallocate the data structures that were initialized for optimization
    return 0;
}
