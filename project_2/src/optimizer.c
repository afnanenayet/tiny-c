/**
 * \file optimizer.c
 * \author Afnan Enayet
 * \brief Contains functions pertaining to optimization of LLVM code
 *
 * This module contains function definitions for functions that provide
 * optimizations for LLVM code. These functions will modify the LLVM model
 * that is passed in.
 *
 * In this file, we implement constant propagation and constant folding.
 */
#include <llvm-c/Core.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "llvm_utils.h"
#include "optimizer.h"
#include "print_utils.h"
#include "vec.h"

/*** Private function prototypes ***/

/**
 * \brief Perform constant propagation on a module
 *
 * \param[in] fn The function to optimize
 * \param[in] basicBlocks The metadata and basic blocks in the function
 * \returns Whether any optimization was performed
 */
static bool constProp(LLVMValueRef fn, meta_vec_t *basicBlocks);

/**
 * \brief Perform constant folding on a basic block
 *
 * Given some basic block, optimize the basic block if it can be optimized.
 *
 * \param[in] bb The basic block to optimize
 * \returns Whether any optimization was performed
 */
static bool constFold(LLVMBasicBlockRef bb);

/*** Private function definitions ***/

static bool constFold(LLVMBasicBlockRef bb) {
    // to_delete stores all the instructions that need to be deleted
    // We cannot delete instructions when we are actually iterating over a list
    // It won't create wrong code but will miss some instructions
    val_vec_t toDelete;
    vec_init(&toDelete);

    /// Whether any optimization has been performed and changed the code
    bool changed = false;

    for (LLVMValueRef instruction = LLVMGetFirstInstruction(bb); instruction;
         instruction = LLVMGetNextInstruction(instruction)) {

        if (LLVMIsABinaryOperator(instruction)) {

            LLVMValueRef x = LLVMGetOperand(instruction, 0);
            LLVMValueRef y = LLVMGetOperand(instruction, 1);
            if (LLVMIsAConstant(x) && LLVMIsAConstant(y)) {
                LLVMValueRef newInst = NULL;

                switch (LLVMGetInstructionOpcode(instruction)) {
                case LLVMAdd:
                    newInst = LLVMConstAdd(x, y);
                    break;
                case LLVMSub:
                    newInst = LLVMConstSub(x, y);
                    break;
                case LLVMMul:
                    newInst = LLVMConstMul(x, y);
                    break;
                case LLVMSDiv:
                    newInst = LLVMConstSDiv(x, y);
                    break;
                default:
                    break;
                }

                if (newInst) {
                    changed = true;
                    LLVMReplaceAllUsesWith(instruction, newInst);
                    vec_push(&toDelete, instruction);
                }
            }
        }
    }
    LLVMValueRef val;
    int i;
    // delete each instruction that was marked to be deleted
    vec_foreach(&toDelete, val, i) { LLVMInstructionEraseFromParent(val); }
    // deallocate the vector
    vec_deinit(&toDelete);
    return changed;
}

static bool constProp(LLVMValueRef fn, meta_vec_t *basicBlocks) {
    bool changed = false;

    // loop through each basic block, then each instruction in each basic
    // block
    int i;
    meta_t *meta;
    val_vec_t R;
    vec_init(&R);

    // deletion queue
    val_vec_t toDelete;
    vec_init(&toDelete);

    vec_foreach(basicBlocks, meta, i) {
        vec_deinit(&R);
        vec_init(&R);

        // copy IN[B] to R
        LLVMValueRef temp;
        int j;
        vec_foreach(meta->inSet, temp, j) { vec_push(&R, temp); }

#ifdef DEBUG
        printf("(constProp) R initially has %d values\n", R.length);
#endif

        // loop through each instruction in the basic block
        LLVMBasicBlockRef basicBlock = meta->bb;

        for (LLVMValueRef inst = LLVMGetFirstInstruction(basicBlock); inst;
             inst = LLVMGetNextInstruction(inst)) {
            // if I is a constant store instruction, add it to R
            if (LLVMIsAStoreInst(inst) &&
                LLVMIsAConstant(LLVMGetOperand(inst, 0))) {
                vec_push(&R, inst);
#ifdef DEBUG
                println("(constProp) Add constant store instruction to R");
#endif
            } else if (LLVMIsAStoreInst(inst)) {
                // if I is a store instruction, remove everything in R that is
                // killed by the instruction I
                LLVMValueRef tempInst;
                LLVMValueRef addr = LLVMGetOperand(inst, 1);
                vec_foreach(&R, tempInst, j) {
                    LLVMValueRef otherAddr = LLVMGetOperand(tempInst, 1);

                    if (otherAddr == addr) {
                        vec_remove(&R, tempInst);
#ifdef DEBUG
                        println(
                            "(constProp) Removed killed instructions in set R");
#endif
                    }
                }
            } else if (LLVMIsALoadInst(inst)) {
                // If I is a load instruction that loads from the address
                // represented by the variable %t, then find all the store
                // instructions in R that write to addresses represented by %t
                // if they all write the same constant into memory, then replace
                // all of the uses of instruction I by the constant in the store
                // isntructions mark the load instructions for deletion, then
                // delete them later

                // Find all store instructions in R that write to address that
                // the current instruction loads from, and figure out if all
                // of the store instructions are constant store instructions
                LLVMValueRef loadAddr = LLVMGetOperand(inst, 1);
                LLVMValueRef it;
                int j;

                val_vec_t temp;
                vec_init(&temp);

                vec_foreach(&R, it, j) {
                    if (LLVMIsAStoreInst(it) &&
                        (LLVMGetOperand(it, 1) == loadAddr)) {
#ifdef DEBUG
                        println("(constProp) Found store instruction that "
                                "stores to this load address");
#endif
                        vec_push(&temp, it);
                    }
                }

                // bail if no optimizations can be made
                if (R.length < 1) {
                    vec_deinit(&temp);
                    continue;
                }

                // need to make sure all store ops are writing the same constant
                // to memory
                bool allConstStoreInsts = true;
                LLVMValueRef constant = LLVMGetOperand(R.data[0], 0);

                // check and see if everything in temp stores constants and
                // write the same constant value
                vec_foreach(&temp, it, j) {
                    LLVMValueRef storedVal = LLVMGetOperand(it, 0);
                    if (!(LLVMIsAStoreInst(it) && LLVMIsAConstant(storedVal) &&
                          (storedVal == constant))) {
                        allConstStoreInsts = false;
                    }
                }

                // bail out if no optimizations can be made
                if (!allConstStoreInsts) {
                    vec_deinit(&temp);
                    continue;
                }

                // replace all uses of the current load instruction with a
                // store instruction that stores a constant
                LLVMReplaceAllUsesWith(inst, constant);
#ifdef DEBUG
                println(
                    "(constProp) Replacing load instruction(s) with constant");
#endif
                changed = true;
                vec_deinit(&temp);
                vec_push(&toDelete, inst);
            }
        }
    }

    // delete all the instructions marked for deletion in the queue
    LLVMValueRef deleteInst;
    vec_foreach(&toDelete, deleteInst, i) {
        LLVMInstructionEraseFromParent(deleteInst);
    }
    // if anything was deleted, then changes have been made
    // otherwise, we hvae reached a fixed point
    changed = changed || toDelete.length > 0;
    vec_deinit(&toDelete);
    vec_deinit(&R);
    return changed;
}

/*** Public function definitions ***/

void optimizeProgram(LLVMModuleRef m) {
    // get metadata for each basic block
    LLVMValueRef function = LLVMGetFirstFunction(m);
    val_vec_t *S = computeS(function);
    meta_vec_t *metadata = computeBlockMData(function, S);

#ifdef DEBUG
    printf("(optimizeProgram) %d basic blocks in metadata vector\n",
           metadata->length);
#endif

    bool changed = false;

    // the number of passes the optimization routine takes
    unsigned int optimizationPasses = 0;

    // continue optimizing until the fixed-point
    do {
        optimizationPasses++;
#ifdef DEBUG
        printf("(optimizeProgram) Running optimization pass: %d\n",
               optimizationPasses);
#endif
        changed = false;
        changed |= constProp(function, metadata);

        for (LLVMBasicBlockRef basicBlock = LLVMGetFirstBasicBlock(function);
             basicBlock; basicBlock = LLVMGetNextBasicBlock(basicBlock)) {
            changed |= constFold(basicBlock);
        }
    } while (changed);

    // deallocate the data structures that were initialized for optimization
    meta_vec_delete(metadata);

#ifdef DEBUG
    println("(optimizeProgram) Deallocated metadata vector");
#endif

    vec_deinit(S);
    free(S);

#ifdef DEBUG
    println("(optimizeProgram) Deallocated S vector");
#endif
}
