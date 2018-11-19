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
    sortedIntervals = sortIntervalMap(intervalTable);
    resultTable = genResultTable(*basicBlock, registerTable, sortedIntervals);

    // TODO remove
    printTables();
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
