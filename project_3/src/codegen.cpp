#include <cstdio>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>

#include <llvm/IR/Constants.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>

#include "LLVMUtils.h"
#include "codegen.h"

void codeGen(std::unique_ptr<llvm::Module> &module) {
    auto labels = std::make_shared<LabelTable>();

    for (auto &func : *module) {
        int bbCounter = 0;
        for (auto &bb : func) {
            if (&bb == &func.getEntryBlock()) {
                // the entry basic block's name is the function name followed
                // by a colon
                labels->insert(std::make_pair(&bb, func.getName().str()));
            }
            // create bb label
            std::stringstream ss;
            ss << ".L" << bbCounter++;
            labels->insert(std::make_pair(&bb, ss.str()));
        }
    }

    for (auto &func : *module) {
        auto offsets = genOffsetTable(func);
        printFnDirective(func);

        // print the function directives
        for (auto &bb : func) {
            // print the basic block label
            std::cout << labels->find(&bb)->second << ":\n";

            // deduplicate the load instructions in the basic block
            // loadDedup(bb);

            // generate assembly for the basic block
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

    // so we can iterate over all of the registers easily
    std::vector<PhysicalRegister> allRegisters = {eax, ebx, ecx, edx};

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

        // a list of register that we spilled that will have to be popped later
        std::vector<PhysicalRegister> spilledRegisters;

        // the registers in use
        RegisterSet usedRegisters;

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
            // if there is no register assigned for this instruction, then
            // there is no result for this particular instruction
            auto result = resultTable->find(&inst);

            // check which registers the operands are using
            auto regA = resultTable->find(
                static_cast<const llvm::Instruction *>(operand0.value()));
            auto regB = resultTable->find(
                static_cast<const llvm::Instruction *>(operand1.value()));

            if (regA != resultTable->end())
                usedRegisters.insert(regA->second);

            if (regB != resultTable->end())
                usedRegisters.insert(regB->second);

            // If there is no result, then we need to find some register to
            // use for the instruction destination. Pick a register that isn't
            // being used by an operand and spill it.
            if (result == resultTable->end()) {
                for (auto reg : allRegisters) {
                    if (usedRegisters.find(reg) != usedRegisters.end()) {
                        resultRegister = reg;
                        usedRegisters.insert(reg);
                        spilledRegisters.push_back(reg);

                        // generate instruction to spill the register so it can
                        // be set
                        std::cout << "pushl " << registerString(reg) << "\n";
                        break;
                    }
                }
            }

            // copy the second operand to the result register
            auto secondOpStr = findOp(*operand1.value());
            std::cout << "movl " << registerString(resultRegister) << ", "
                      << secondOpStr << "\n";

            // do the add op
            switch (inst.getOpcode()) {
            case llvm::Instruction::Add:
                std::cout << "addl";
                break;
            case llvm::Instruction::Sub:
                std::cout << "subl";
                break;
            case llvm::Instruction::Mul:
                std::cout << "imull";
                break;
            default:
                std::cout << "unknown_arithmetic_op";
                break;
            }
            std::cout << " " << findOp(*operand0.value()) << ", " << secondOpStr
                      << "\n";

            // pop the stack back into each register
            for (auto reg : usedRegisters) {
                std::cout << "popl " << registerString(reg) << "\n";
            }
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
            auto destLabelStr = destLabel->second;

            // If the branch is conditional, then perform whichever compare/jump
            // instruction is necessary for that condition.
            // Otherwise, just dump the jmp instruction.
            if (branchInst->isConditional()) {
                if (!operand0.has_value() || !operand1.has_value())
                    throw std::runtime_error("Could not find both operands for "
                                             "conditional instruction");

                // First, generate the cmp instruction to set up the conditional
                // jump. If the first operand is not a constant, move it to a
                // register.
                std::cout << "cmpl " << findOp(*operand0.value()) << ", "
                          << findOp(*operand1.value()) << "\n";

                auto condition =
                    static_cast<llvm::CmpInst *>(branchInst->getCondition());

                // Generate the proper jump instruction for the comparison.
                // We are only generating cases that correspond to the jump
                // variants listed in the instructions. We are also only
                // dealing with signed integers, according to the
                // instructions.
                switch (condition->getPredicate()) {
                case llvm::CmpInst::ICMP_EQ:
                    std::cout << "je " << destLabelStr << "\n";
                    break;
                case llvm::CmpInst::ICMP_NE:
                    std::cout << "jne " << destLabelStr << "\n";
                    break;
                case llvm::CmpInst::ICMP_SLE:
                    std::cout << "jle " << destLabelStr << "\n";
                    break;
                case llvm::CmpInst::ICMP_SLT:
                    std::cout << "jl " << destLabelStr << "\n";
                    break;
                case llvm::CmpInst::ICMP_SGE:
                    std::cout << "jge " << destLabelStr << "\n";
                    break;
                case llvm::CmpInst::ICMP_SGT:
                    std::cout << "jg " << destLabelStr << "\n";
                    break;
                default:
                    // silently ignore unsupported comparisons
                    break;
                }
            } else {
                std::cout << "jmp " << destLabelStr << "\n";
            }
        } else if (llvm::isa<llvm::LoadInst>(inst)) {
            auto load = static_cast<const llvm::LoadInst *>(&inst);
            const llvm::Value *valOp = nullptr;
            auto ptrOp = load->getPointerOperand();

            if (operand0.has_value()) {
                valOp = operand0.value();
                std::cout << "movl " << findOp(*ptrOp) << ", " << findOp(*valOp)
                          << "\n";
            }
        } else if (llvm::isa<llvm::StoreInst>(inst)) {
            auto store = static_cast<const llvm::StoreInst *>(&inst);
            auto ptrOp = store->getPointerOperand();
            auto valOp = store->getValueOperand();
            std::cout << "movl " << findOp(*ptrOp) << ", " << findOp(*valOp)
                      << "\n";
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

void RegisterAllocator::printTables() const {
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

std::string RegisterAllocator::findOp(const llvm::Value &inst) const {
    std::stringstream ss;
    // first, check if the instruction is a constant
    if (llvm::isa<llvm::Constant>(inst)) {
        // making the assumption that the value is a signed 32 bit integer
        if (auto val = llvm::dyn_cast<llvm::ConstantInt>(&inst)) {
            ss << "$";
            ss << val->getSExtValue();
        }
    } else if (llvm::isa<llvm::Instruction>(inst)) {
        auto instPtr = static_cast<const llvm::Instruction *>(&inst);

        // first, check to see if the operand has a register
        auto regCandidate = resultTable->find(instPtr);

        if (regCandidate != resultTable->end()) {
            // we have a register!
            switch (regCandidate->second) {
            case eax:
                ss << "%eax";
            case ebx:
                ss << "%ebx";
            case ecx:
                ss << "%ecx";
            case edx:
                ss << "%edx";
            default:
                break;
            }
        } else {
            // otherwise, look in the memory table for the pointer location
            auto offsetCandidate = offsetTable->find(instPtr);

            if (offsetCandidate != offsetTable->end()) {
                ss << "%ebp" << offsetCandidate->second;
            }
        }
    }
    return ss.str();
}

void printFnDirective(const llvm::Function &func) {
    auto name = func.getName();

    std::cout << "\t.globl " << name.str() << "\n"
              << "\t.type " << name.str() << ", @function\n";
}
