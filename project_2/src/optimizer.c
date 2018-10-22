/**
 * \file optimizer.c
 *
 * \brief The entry point into the optimizer program
 *
 * This file contains the entry point and the main function for the optimizer.
 * It is as minimal as possible and only calls a function to begin the logic
 * for the actual optimization code.
 */

#include <stdio.h>

/**
 * \brief Main entry point into the program
 *
 * Parse command line arguments and call the main functions for the optimizer
 * program.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Missing input filepath!\n");
        return 1;
    }
    return 0;
}
