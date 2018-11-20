#include <cstring>
#include <filesystem>
#include <memory>
#include <stdexcept>
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

    if (module == nullptr) {
        throw std::runtime_error("Could not parse IR file!");
    }
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
               const std::shared_ptr<IndexTable> &indexTable,
               const std::shared_ptr<IntervalTable> &intervalTable,
               const std::shared_ptr<RegisterTable> &registers) {
    const auto &instList = bb.getInstList();

    // iterate backwards through each instruction in the basic block
    for (auto it = instList.rbegin(); it != instList.rend(); it++) {
        auto &inst = *it;

        // check each operand:
        // if the operand is non-memory and non-constant, check if it already
        // exists in the interval table
        for (auto user : inst.users()) {
            // ignore constant or memory (alloca instruction)
            if (llvm::isa<llvm::Constant>(user) ||
                llvm::isa<llvm::AllocaInst>(user))
                continue;
            if (const auto opInst = llvm::dyn_cast<llvm::Instruction>(user)) {
                // check if the operand already exists in the liveliness table
                auto searchIt = intervalTable->find(opInst);

                // if it already exists, move on to the next operand
                if (searchIt != intervalTable->end()) {
                    continue;
                }

                // insert the interval into the interval table
                // This will throw an error if the instructions aren't in the
                // index table, but that would be a logic error so that means
                // the indexTable isn't being constructed properly
                // auto interval =
                // std::make_pair(indexTable->find(opInst)->second,
                // indexTable->find(&inst)->second);
                auto interval =
                    std::make_pair(indexTable->find(&inst)->second,
                                   indexTable->find(opInst)->second);
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

std::shared_ptr<SortedIntervalList>
sortIntervalMap(const std::shared_ptr<IntervalTable> &table) {
    auto sortedMap = std::make_shared<SortedIntervalList>();

    // copy each element from the interval table to the sorted interval table
    for (auto &it : *table) {
        sortedMap->push_back(std::make_pair(it.first, it.second));
    }

    // sort the vector, using the length of the interval
    std::sort(sortedMap->begin(), sortedMap->end(),
              [=](const auto &a, const auto &b) -> bool {
                  auto aTuple = a.second;
                  auto bTuple = b.second;
                  auto aLength = std::get<1>(aTuple) - std::get<0>(aTuple);
                  auto bLength = std::get<1>(bTuple) - std::get<0>(bTuple);

                  // returning a > b will sort the vector in descending order
                  // since the default is a < b
                  return std::abs(aLength) > std::abs(bLength);
              });
    return sortedMap;
}

std::shared_ptr<ResultTable>
genResultTable(const llvm::BasicBlock &bb,
               std::shared_ptr<RegisterTable> &registers,
               std::shared_ptr<SortedIntervalList> &liveness) {
    // initialize the empty result table
    auto results = std::make_shared<ResultTable>();

    // If the terminator instruction is a return instruction, take `%eax`
    if (const auto &returnInst =
            llvm::dyn_cast<llvm::ReturnInst>(bb.getTerminator())) {
        const auto &operand = returnInst->getOperand(0);
        auto operandInst = static_cast<llvm::Instruction *>(operand);

        if (operandInst != nullptr) {
            results->insert(std::make_pair(operandInst, eax));

            // find every operand that overlaps with the current interval
            // table and remove `eax` from their list of available registers
            auto overlappingOps = getOverlappingOps(operandInst, liveness);

            for (const auto overlappingInst : *overlappingOps) {
                auto foundInst = registers->find(overlappingInst);

                // skip if we can't find the instruction in the register table
                if (foundInst == registers->end()) {
                    continue;
                }
                foundInst->second.erase(eax);
            }
        }
    }

    // Iterate through the liveness table, taking the longest entries that don't
    // have any entries in the results table. Add the entry to the result table,
    // adding a register to the table, then remove that register from the set of
    // physical registers for any op whose intervals overlap with that
    // instruction.
    //
    // In the instructions, it says to keep repeating until all of the values
    // are loaded into the table, but going through the entire liveness table
    // achieves the same effect because all variables have *some* liveness
    // interval. Once all the variables are loaded, they will be in the results
    // table, which gets caught by the first check, so there's no fear of
    // redundances.
    //
    // Because the liveness entries are sorted in descending order from longest
    // to shortest, this ends up giving precedence to the longer intervals,
    // which also maintains the invariant stated in the instructions.
    for (auto &entry : *liveness) {
        // Check to see if the instruction is already in the results table. If
        // so, move on to the next one.
        if (results->find(entry.first) == results->end())
            continue;

        auto registerEntry = registers->find(entry.first);

        if (registerEntry == registers->end())
            throw std::runtime_error(
                "Liveness table contains operand not found in registers table");

        auto registerSet = registerEntry->second;

        // skip if there are no registers available for the instruction
        if (registerSet.size() == 0) {
            results->insert(std::make_pair(entry.first, nullRegister));
            continue;
        }

        // pick some register to use
        auto selectedRegister = *registerSet.begin();
        results->insert(std::make_pair(entry.first, selectedRegister));

        // get overlapping instructions
        auto overlaps = getOverlappingOps(entry.first, liveness);

        // remove the selected register from the available registers for
        // each overlapping instruction
        for (const auto &overlappingInst : *overlaps) {
            auto registerEntry = registers->find(overlappingInst);

            if (registerEntry == registers->end())
                throw std::runtime_error(
                    "Found instruction from overlapping vector that was not in "
                    "register table");

            registerEntry->second.erase(selectedRegister);
        }
    }
    return results;
}

std::unique_ptr<std::vector<const llvm::Instruction *>>
getOverlappingOps(const llvm::Instruction *inst,
                  const std::shared_ptr<SortedIntervalList> &intervals) {
    // find interval for given instruction
    std::tuple<int, int> targetInterval;
    auto overlapping =
        std::make_unique<std::vector<const llvm::Instruction *>>();

    for (const auto &intervalEntry : *intervals) {
        if (intervalEntry.first == inst) {
            targetInterval = intervalEntry.second;
            break;
        }
    }

    // iterate through the intervals, checking to see if the targetInterval
    // is overlapped by the intervals
    for (const auto &intervalEntry : *intervals) {
        if (intervalEntry.first == inst)
            continue;

        const auto otherInterval = intervalEntry.second;

        if ((std::get<0>(targetInterval) <= std::get<0>(otherInterval) &&
             std::get<0>(otherInterval) <= std::get<1>(targetInterval)) ||
            (std::get<0>(targetInterval) <= std::get<1>(otherInterval) &&
             std::get<1>(otherInterval) <= std::get<1>(targetInterval))) {
            overlapping->push_back(intervalEntry.first);
        }
    }
    return overlapping;
}

bool isArithmeticInst(const llvm::Instruction &inst) {
    auto op = inst.getOpcode();

    // use the enums rather than the actual integer opcode because they can
    // change based on which LLVM version you're using
    return op == llvm::Instruction::Add || op == llvm::Instruction::Sub ||
           op == llvm::Instruction::Mul;
}

std::string registerString(PhysicalRegister reg) {
    switch (reg) {
    case eax:
        return "eax";
    case ebx:
        return "ebx";
    case ecx:
        return "ecx";
    case edx:
        return "edx";
    default:
        return "";
    }
}
