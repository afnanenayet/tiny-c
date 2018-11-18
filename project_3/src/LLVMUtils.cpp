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
        throw std::runtime_error("Could not parse IR file!");
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

    // iterate through each instruction and add the index to the table
    // Note that the instructions say to skip "alloc" instructions, but
    // the example code doesn't do so, and it seems that we need to use
    // alloc instructions that define variables in the register allocation
    // section
    for (const auto &inst : bb) {
        // ensure that the opcode is NOT an alloc instruction
        table->insert(std::make_pair(&inst, i++));
    }
    return table;
}

void tableInit(const llvm::BasicBlock &bb,
               const std::shared_ptr<IndexTable> indexTable,
               std::shared_ptr<IntervalTable> intervalTable,
               std::shared_ptr<RegisterTable> registers) {
    const auto &instList = bb.getInstList();

    // iterate backwards through each instruction in the basic block
    for (auto it = instList.rbegin(); it != instList.rend(); it++) {
        auto &inst = *it;

        // check each operand:
        // if the operand is non-memory and non-constant, check if it already
        // exists in the interval table
        for (auto user : inst.users()) {
            // ignore constant or memory (alloca instruction)
            if (llvm::dyn_cast<llvm::Constant>(user) != nullptr ||
                llvm::dyn_cast<llvm::AllocaInst>(user) != nullptr) {
                continue;
            } else if (const auto opInst =
                           llvm::dyn_cast<llvm::Instruction>(user)) {
                // check if the operand already exists in the liveliness table
                auto searchIt = intervalTable->find(opInst);

                // if it already exists, move on to the next operand
                if (searchIt != intervalTable->end())
                    continue;

                // insert the interval into the interval table
                auto interval = std::make_pair(indexTable->find(opInst)->second,
                                               indexTable->find(&inst)->second);
                intervalTable->insert(std::make_pair(opInst, interval));

                // insert all of the physical registers for this operand
                auto s = RegisterSet();
                s.insert(eax);
                s.insert(ebx);
                s.insert(ecx);
                s.insert(edx);
                registers->insert(std::make_pair(opInst, s));
            }
        }
    }
}
