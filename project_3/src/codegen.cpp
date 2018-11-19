#include <cstdio>
#include <iostream>
#include <optional>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include "LLVMUtils.h"
#include "codegen.h"

void codeGen(std::unique_ptr<llvm::Module> &module) {
    // first, generate the labels for each basic block
    // (TODO)
    auto labels = std::make_shared<LabelTable>();

    for (auto &func : *module) {
        auto offsets = genOffsetTable(func);
        for (auto &bb : func) {
            auto regAlloc = RegisterAllocator(&bb, offsets, labels);
            regAlloc.gen();
        }
    }
}

RegisterAllocator::RegisterAllocator(const llvm::BasicBlock *bb,
                                     std::shared_ptr<OffsetTable> &offsets,
                                     std::shared_ptr<LabelTable> &labels) {
    basicBlock = bb;
    offsetTable = offsets;
    labelTable = labels;
    initializeMembers();
}

RegisterAllocator::~RegisterAllocator() {}

void RegisterAllocator::gen() {
    // generate the metadata necessary to perform code generation
    generateTables();

    for (const auto &inst : *basicBlock) {
        // if an instruction is an alloc instruction, skip it
        if (llvm::isa<llvm::AllocaInst>(inst))
            continue;

        PhysicalRegister resultRegister = nullRegister;

        // check to see if the current instruction has a result register
        auto x = resultTable->find(&inst);

        // if so, store that result register
        if (x != resultTable->end())
            resultRegister = x->second;

        // In these code examples, there are at most two operands, so set them
        // here for convenience. We use std optional because it's available in
        // C++17 and offers more safety than directly dealing with nullptr. It
        // also has much more graceful error saving options.
        std::optional<const llvm::Value *> operand0 = std::nullopt;
        std::optional<const llvm::Value *> operand1 = std::nullopt;

        if (inst.getNumOperands() > 0)
            operand0 = inst.getOperand(0);
        if (inst.getNumOperands() > 1)
            operand1 = inst.getOperand(1);

        // Based on the instruction, determine what kind of assembly
        // instruction to generate, and perform the necessary register
        // twiddling operations, based on whether we need to spill registers
        // or otherwise move memory around
        //
        // The two main conditions we run into are whether the instruction is
        // an arithmetic operation or if it is a branch instruction
        if (isArithmeticInst(inst)) {
            // TODO
        } else if (llvm::isa<llvm::BranchInst>(inst)) {

            // if the instruction is a branch instruction, check whether it's
            // conditional
            auto branchInst = static_cast<const llvm::BranchInst *>(&inst);

            if (!operand0.has_value())
                throw std::runtime_error(
                    "Could not get operand for branch instruction");

            // The destination basic block for the jump
            auto destBB =
                static_cast<const llvm::BasicBlock *>(operand0.value());

            // the label for the section referred to by the basic block
            auto destLabel = labelTable->find(destBB);

            if (destLabel == labelTable->end())
                throw std::runtime_error(
                    "Could not find basic block in label table");

            // If the branch is conditional, then perform whichever compare/jump
            // instruction is necessary for that condition.
            // Otherwise, just dump the jmp instruction.
            if (branchInst->isConditional()) {
                // First, generate the cmp instruction to set up the conditional
                // jump. If the first operand is not a constant, move it to a
                // register.

            } else {
                std::cout << "jmp " << destLabel->second << "\n";
            }
        }
    }
}

void RegisterAllocator::generateTables() {
    indexTable = genIndexTable(*basicBlock);
    tableInit(*basicBlock, indexTable, intervalTable, registerTable);
    sortedIntervals = sortIntervalMap(intervalTable);
    resultTable = genResultTable(*basicBlock, registerTable, sortedIntervals);

    // for debugging purposes
    // printTables();
}

void RegisterAllocator::printTables() {
    std::cout << "\nindexTable:\n";
    printUMap(*indexTable);

    std::cout << "\n\nintervalTable:\n";

    for (const auto &p : *intervalTable) {
        std::cout << "\t" << p.first << " : ( " << std::get<0>(p.second) << ", "
                  << std::get<1>(p.second) << ")\n";
    }

    std::cout << "\n\nregisterTable:\n";
    for (const auto &p : *registerTable) {
        std::cout << "\t" << p.first << " : ";
        std::cout << "(";

        for (const auto &reg : p.second) {
            std::cout << static_cast<int>(reg) << ", ";
        }
        std::cout << ")\n";
    }
    std::cout << std::endl;

    std::cout << "\n\nsortedIntervals:\n";

    for (const auto &entry : *sortedIntervals) {
        std::cout << "\t" << entry.first << " : (" << std::get<0>(entry.second)
                  << ", " << std::get<1>(entry.second) << ")\n";
    }

    std::cout << "\n\nresultTable:\n";
    printUMap(*resultTable);
}

void RegisterAllocator::initializeMembers() {
    indexTable = std::make_shared<IndexTable>();
    intervalTable = std::make_shared<IntervalTable>();
    registerTable = std::make_shared<RegisterTable>();
}
