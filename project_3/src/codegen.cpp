#include <iostream>

#include <llvm/IR/Module.h>

#include "LLVMUtils.h"
#include "codegen.h"

void codeGen(std::unique_ptr<llvm::Module> &module) {
    for (auto &func : *module) {
        for (auto &bb : func) {
            auto regAlloc = RegisterAllocator(&bb);
            regAlloc.gen();
        }
    }
}

RegisterAllocator::RegisterAllocator(const llvm::BasicBlock *bb) {
    basicBlock = bb;
    initializeMembers();
}

RegisterAllocator::~RegisterAllocator() {}

void RegisterAllocator::gen() { generateTables(); }

void RegisterAllocator::generateTables() {
    indexTable = genIndexTable(*basicBlock);
    tableInit(*basicBlock, indexTable, intervalTable, registerTable);

    // TODO remove
    printTables();
}

void RegisterAllocator::printTables() {
    std::cout << "\nindexTable:\n" << indexTable << "\n";
    std::cout << "\nintervalTable:\n" << intervalTable << "\n";
    std::cout << "\nregisterTable:\n" << registerTable << "\n";
}

void RegisterAllocator::initializeMembers() {
    indexTable = std::make_shared<IndexTable>();
    intervalTable = std::make_shared<IntervalTable>();
    registerTable = std::make_shared<RegisterTable>();
}
