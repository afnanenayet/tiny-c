/*!
 * \file LLVMUtils.h
 * \author Afnan Enayet
 *
 * \brief Utilities to work with LLVM IR
 *
 * Convenience/helper functions that help parse and interact with LLVM IR in
 * the context of global register allocation
 */
#include <llvm/IRReader/IRReader.h>
#include <string>

#pragma once

/*!
 * \brief Create an LLVM IR module given the filename of some LLVM IR file.
 *
 * Given the relative path to some LLVM IR file, this function attempts to
 * parse the file using LLVM's utilities.
 *
 * It will throw an exception if there are any errors or if the filepath
 * is invalid. This will not verify the file, however. If the file is
 * malformed but LLVM can still parse it (perhaps the file is not well
 * formed), it is the responsibility of the user to verify that everything
 * is working.
 *
 * \param filename The relative path to the file
 * \param context The MT-safe LLVM context. There should only be one per
 * thread
 * \returns a pointer to an LLVM module
 */
std::unique_ptr<llvm::Module> loadModule(const std::string &filename,
                                         llvm::LLVMContext &context);
