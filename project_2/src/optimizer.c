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
#include <stdlib.h>

#include "llvm_utils.h"
#include "optimizer.h"
#include "vec.h"

/*** Private function prototypes ***/

/**
 * \brief Perform constant propagation on a module
 *
 * \param[in] fn The function to optimize
 * \returns Whether any optimization was performed
 */
static bool constProp(LLVMValueRef fn);

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

static bool constProp(LLVMValueRef fn) {}

/*** Public function definitions ***/

void optimizeProgram(LLVMModuleRef m) { bool success = walkFunctions(m); }
