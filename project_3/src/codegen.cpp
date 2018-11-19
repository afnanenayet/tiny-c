#include <cstdio>
#include <iostream>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include "LLVMUtils.h"
#include "codegen.h"

void codeGen(std::unique_ptr<llvm::Module> &module) {
    for (auto &func : *module) {
        auto offsets = genOffsetTable(func);
        for (auto &bb : func) {
            auto regAlloc = RegisterAllocator(&bb, offsets);
            regAlloc.gen();
        }
    }
}

RegisterAllocator::RegisterAllocator(const llvm::BasicBlock *bb,
                                     std::shared_ptr<OffsetTable> &offsets) {
    basicBlock = bb;
    offsetTable = offsets;
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

        // check and see if any of the operands or the current instruction have
        // a result register
        bool registerUsed = false;
        int numOperands = inst.getNumOperands();

        if (resultTable->find(&inst) != resultTable->end())
            registerUsed = true;

        for (int i = 0; i < numOperands; i++) {
            auto operand = static_cast<llvm::Instruction *>(inst.getOperand(i));

            if (resultTable->find(operand) != resultTable->end())
                registerUsed = true;
        }

        // actual code generation below!!!
        //
        // Whittle down the possible candidates for ASM based on the attributes
        // of the LLVM instruction
        // For example, if a register is used, then we are only looking at
        // assembly instructions that involve a register, etc.
        if (registerUsed) {
            if (numOperands == 1) {

            } else if (numOperands == 2) {
                switch (inst.getOpcode()) {
                case llvm::Instruction::Add:
                    break;
                case llvm::Instruction::Sub:
                    break;
                case llvm::Instruction::Mul:
                    break;
                }
            }
        } else {
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
