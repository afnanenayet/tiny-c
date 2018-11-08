#include <filesystem>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>
#include <stdexcept>

#include "LLVMUtils.h"

std::unique_ptr<llvm::Module> loadModule(const std::string &filename,
                                         llvm::LLVMContext &context) {
    llvm::SMDiagnostic diag;
    auto module = llvm::parseIRFile(filename, diag, context);

    if (module == nullptr)
        throw std::runtime_error("Could not parse IR file");
    return module;
}
