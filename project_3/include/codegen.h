/*!
 * \file codegen.h
 *
 * \brief Code generation routines and utilities for LLVM IR modules
 *
 * This module contains methods to call routines for code generation and
 * other helper/convenience utilities.
 */

#include <llvm/IR/Instruction.h>

#include "LLVMUtils.h"

#pragma once

/*!
 * \brief A register allocation class for a basic block
 *
 * This class contains routines and has member variables with data pertinent
 * to register allocation. This data and these routines are meant to be run
 * on an individual basic block. It is the callers responsibility to
 * initialize this class and call it on every basic block that they want to
 * allocate.
 */
class RegisterAllocator {
  public:
    /*!
     * \brief Initialize a register allocator
     *
     * Initialize the register allocation class for a given basic block
     *
     * \param[in] bb A pointer to the basic block
     */
    RegisterAllocator(const llvm::BasicBlock *bb);
    ~RegisterAllocator();

  private:
    /*!
     * \brief Run the routines to generate all of the data tables for codegen
     *
     * This function generates all of the tables for a basic block that are
     * necessary for code generation.
     *
     * It generates the following tables:
     *     - the offset table
     *     - the index table
     *     - the result table
     *     - the interval table
     *
     * The description of these tables can be found in the Doxygen
     * documentation of each member variable that corresponds to each table.
     */
    void generateTables();

    /*!
     * \brief Initialize the member variables for the class
     *
     * Intialize member variables of the class that aren't initialized
     * externally so that they can be consumed by other functions.
     */
    void initializeMembers();

    //! The basic block that's being operated on
    const llvm::BasicBlock *basicBlock;

    //! The offset table containing a mapping of instructions to their
    //! memory offsets
    std::shared_ptr<OffsetTable> offsetTable;

    //! A mapping of instructions to their indices
    std::shared_ptr<IndexTable> indexTable;

    //! A mapping of instructions to their available physical registers
    std::shared_ptr<ResultTable> resultTable;

    //! A mapping of instructions for each basic block
    std::shared_ptr<IntervalTable> intervalTable;

    //! A mapping of instructions to their register
    std::shared_ptr<RegisterTable> registerTable;
};
