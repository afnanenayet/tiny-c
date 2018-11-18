/*!
 * \file LLVMUtils.h
 * \author Afnan Enayet
 *
 * \brief Utilities to work with LLVM IR
 *
 * Convenience/helper functions that help parse and interact with LLVM IR in
 * the context of global register allocation
 */
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>

#include <llvm/IR/Instruction.h>
#include <llvm/IRReader/IRReader.h>

#pragma once

/*** typedefs ***/

//! The types of physical registers available
typedef enum {
    eax,
    ebx,
    ecx,
    edx,
} PhysicalRegister;

//! A set of physical registers
typedef std::set<PhysicalRegister> RegisterSet;

//! An interval from one instruction index to another
typedef std::tuple<int, int> Interval;

//! A mapping from an instruction to its byte offset
typedef std::unordered_map<const llvm::Instruction *, int> OffsetTable;

//! A mapping from an instruction to its index
typedef std::unordered_map<const llvm::Instruction *, unsigned int> IndexTable;

/*!
 * \brief A table from instructions to their liveliness intervals
 *
 * A mapping from an LLVM instruction to its liveness interval (a tuple of two
 * integers that represent a variable from when it's live to when it dies)
 */
typedef std::unordered_map<const llvm::Instruction *, Interval> IntervalTable;

//! An ordered map of instructions to liveness intervals
typedef std::map<const llvm::Instruction *, Interval> SortedIntervalTable;

/*!
 * \brief Temporary register to physical register table.
 *
 * A mapping from an temporary instruction/register to the corresponding
 * available physical registers.
 */
typedef std::unordered_map<const llvm::Instruction *,
                           std::set<PhysicalRegister>>
    RegisterTable;

/*** function prototypes ***/

/*!
 * \brief Create an LLVM IR module given the filename of some LLVM IR file.
 *
 * Given the relative path to some LLVM IR file, this function attempts to
 * parse the file using LLVM's utilities.
 *
 * It will throw an exception if there are any errors or if the filepath
 * is invalid. This will not verify the file, however. If the file is
 * malformed but LLVM can still parse it (perhaps the file is not well
 * formed), it is the responsibility of the user to verify that everything
 * is working.
 *
 * \param filename The relative path to the file
 * \param context The MT-safe LLVM context. There should only be one per
 * thread
 * \returns a pointer to an LLVM module
 */
std::unique_ptr<llvm::Module> loadModule(const std::string &filename,
                                         llvm::LLVMContext &context);

/*!
 * \brief Create the instruction offset table for an LLVM function
 *
 * Given some LLVM function, this method will construct the offset table for
 * each instruction in the function. This table will store the offsets for
 * each variable and the return value.
 *
 * These offsets represent the offset that needs to be added to `%ebp` when
 * generating the code.
 *
 * \param func[in] The LLVM function to inspect
 * \returns An offset table
 */
std::shared_ptr<OffsetTable> genOffsetTable(const llvm::Function &func);

/*!
 * \brief Create an index table with indices for each basic block
 *
 * Given some basic block, generate a mapping of instructions to
 * their corresponding index offset
 *
 * \param[in] bb The basic block to inspect
 * \returns An IndexTable with the offsets for each instruction
 */
std::shared_ptr<IndexTable> genIndexTable(const llvm::BasicBlock &bb);

/*!
 * \brief Generate the liveliness and register tables for a basic block
 *
 * Given some basic block, this method will generate the table to determine
 * how long each instruction is "alive" for, and will store that as a tuple.
 * It will also initialize the physical register table for each instruction
 * or op.
 *
 * \param[in] bb The basic block to inspect
 * \param[in] indexTable The populated index table
 * \param[out] intervalTable An allocated shared pointer for the interval table
 * \param[out] registers An allocated shared pointer for the register table
 */
void tableInit(const llvm::BasicBlock &bb,
               const std::shared_ptr<IndexTable>& indexTable,
               const std::shared_ptr<IntervalTable>& intervalTable,
               const std::shared_ptr<RegisterTable>& registers);

/*!
 * \brief Sort a given interval table by the length of its interval
 *
 * Given a hashmap of operands and liveness intervals, this method sorts the
 * hashmap from longest interval to smallest interval.
 *
 * \param[in] table The table to sort
 * \returns An ordered map of operands to their intervals
 */
std::shared_ptr<SortedIntervalTable>
sortIntervalMap(const std::shared_ptr<IntervalTable>& table);
