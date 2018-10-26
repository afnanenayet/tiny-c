/**
 * \file llvm_utils.h
 * \brief Utility functions for interacting with LLVM IR files
 *
 * This file contains function definitions for the `llvm_utils` module.
 * This module is a collection of functions meant to make it easier for the
 * user to interact with LLVM IR code, so it's easier to optimize.
 */
#include <llvm-c/Core.h>

/**
 * \brief Create an LLVM model from an LLVM IR file
 *
 * Given a path to some LLVM IR file, create an LLVM model using the LLVM API.
 * If there is some error, this function will return NULL and print the error
 * to stderr.
 *
 * \param fp The relative file path to the input LLVM IR file. These are
 * usually suffixed with a `.ll` file extension.
 * \returns An LLVM module on success. On failure, it will return `NULL`.
 */
LLVMModuleRef createLLVMModel(char *fp);
