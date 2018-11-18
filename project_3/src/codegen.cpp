#include "codegen.h"
#include "LLVMUtils.h"

RegisterAllocator::RegisterAllocator(const llvm::BasicBlock *bb) {
    basicBlock = bb;
    initializeMembers();
}

RegisterAllocator::~RegisterAllocator() {}

void RegisterAllocator::generateTables() {
    indexTable = genIndexTable(*basicBlock);
    tableInit(*basicBlock, indexTable, intervalTable, registerTable);
}

void RegisterAllocator::initializeMembers() {
    indexTable = std::make_shared<IndexTable>();
    intervalTable = std::make_shared<IntervalTable>();
    registerTable = std::make_shared<RegisterTable>();
}
