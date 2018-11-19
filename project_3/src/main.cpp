#include <iostream>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>

#include "LLVMUtils.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Need to supply LLVM IR file!\n";
        return 1;
    }

    // parse module file
    auto context = llvm::LLVMContext();
    auto module = loadModule(argv[1], context);

    // run generation method
    codeGen(module); 
    return 0;
}
