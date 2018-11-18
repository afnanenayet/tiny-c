#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string.h>
#include <unordered_map>

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/ErrorOr.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SourceMgr.h>

#include "LLVMUtils.h"

std::unique_ptr<llvm::Module> loadModule(const std::string &filename,
                                         llvm::LLVMContext &context) {
    llvm::SMDiagnostic diag;
    auto module = llvm::parseIRFile(filename, diag, context);

    if (module == nullptr)
        throw std::runtime_error("Could not parse IR file");
    return module;
}

std::shared_ptr<OffsetTable> genOffsetTable(const llvm::Function &func) {
    auto table = std::make_shared<OffsetTable>();
    int offset = -4;

    // get the entry basic block
    const auto &entryBB = func.getEntryBlock();

    for (const auto &inst : entryBB) {
        if (strcmp(inst.getOpcodeName(), "alloca") == 0) {
            table->insert(std::make_pair(&inst, offset));
            offset -= 4;
        }
    }
    return table;
}

std::shared_ptr<IndexTable> genIndexTable(const llvm::BasicBlock &bb) {
    auto table = std::make_shared<IndexTable>();

    // the counter for the instructions
    unsigned i = 0;

    // iterate through each instruction, only incrementing if the instruction
    // is not an `alloc` instruction
    // Note: the example code does not check to see if an instruction is an
    // `alloca` instruction, so I did not do so here
    for (const auto &inst : bb) {
        table->insert(std::make_pair(&inst, i++));
    }
    return table;
}

std::shared_ptr<IntervalTable>
genIntervalTable(const llvm::BasicBlock &bb,
                 const std::shared_ptr<IndexTable> indexTable) {
    auto table = std::make_shared<IntervalTable>();
    // iterate backwards through the instructions
    // TODO
    return table;
}
