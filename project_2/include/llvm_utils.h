/**
 * \file llvm_utils.h
 * \brief Utility functions for interacting with LLVM IR files
 *
 * This file contains function definitions for the `llvm_utils` module.
 * This module is a collection of functions meant to make it easier for the
 * user to interact with LLVM IR code, so it's easier to optimize.
 */
#include <llvm-c/Core.h>
#include <stdbool.h>

#include <vec.h>

#pragma once

/*** type definitions ***/

/// A vector of LLVM values
typedef vec_t(LLVMValueRef) val_vec_t;
typedef vec_t(LLVMBasicBlockRef) bb_vec_t;

/*** struct definitions ***/

/// A struct containing a basic block and optimization metadata associated
/// with basic block
struct meta_s {
    /// The basic block that the sets correspond to. This will be referred to
    /// as $B$
    LLVMBasicBlockRef bb;

    /// The gen set, or `gen[B]`
    val_vec_t *genSet;

    /// The kill set, or `kill[B]`
    val_vec_t *killSet;

    /// The in set, or `in[B]`
    val_vec_t *inSet;

    /// The out or `out[B]`
    val_vec_t *outSet;

    /// The predecessors to the basic block
    bb_vec_t *preds;
};

/// A struct containing a basic block and optimization metadata associated
/// with basic block
typedef struct meta_s meta_t;

/// A vector of basic blocks and associated metadata
typedef vec_t(meta_t *) meta_vec_t;

/*** public function prototypes ***/

/**
 * \brief Create an LLVM model from an LLVM IR file
 *
 * Given a path to some LLVM IR file, create an LLVM model using the LLVM API.
 * If there is some error, this function will return NULL and print the error
 * to stderr.
 *
 * \param fp The relative file path to the input LLVM IR file. These are
 * usually suffixed with a `.ll` file extension.
 * \returns An LLVM module on success. On failure, it will return `NULL`.
 */
LLVMModuleRef createLLVMModel(char *fp);

/**
 * \brief Iterate through each function in an LLVM module
 *
 * Given some module, this method loops through each function in the module.
 *
 * \param module The LLVM module to walk through
 * \return Whether any part of the LLVM IR was changed as a result of some
 * optimization routine.
 */
bool walkFunctions(LLVMModuleRef module);

/**
 * \brief Iterate through each basic block in some LLVM function
 *
 * Given some LLVM function, iterate through each basic block in that
 * function.
 *
 * \param function The function to iterate through
 * \return Whether any part of the LLVM IR was changed as a result of some
 * optimization routine.
 */
bool walkBasicblocks(LLVMValueRef function);

/**
 * \brief Compute the "gen" set for a basic block
 *
 * Given some LLVM basic block, this function computes the "gen" set for the
 * basic block with respect to the rest of the basic blocks.
 *
 * Note that this function initializes a new vector that must be deleted
 * later.
 *
 * \param[in] bb The basic block to inspect
 * \returns A newly allocated vector representing a set of LLVM values
 */
val_vec_t *computeGenSet(LLVMBasicBlockRef bb);

/**
 * \brief Compute the "kill" set for a basic block
 *
 * Given some LLVM basic block, this function computes the "kill" set for the
 * basic block with respect to the rest of the basic blocks.
 *
 * Note that this function initializes a new vector that must be deleted
 * later.
 *
 * \param[in] bb The basic block to inspect
 * \param[in] S A set of all of the
 * \returns A newly allocated vector representing a set of LLVM values
 */
val_vec_t *computeKillSet(LLVMBasicBlockRef bb, val_vec_t *S);

/**
 * \brief Compute the set $S$
 *
 * Compute the set $S$ for a function. The set $S$ is defined as all
 * of the instructions in some function that are store instructions that
 * store constant values.
 *
 * \param[in] fn The function to inspect
 * \returns A vector representing the set $S$
 */
val_vec_t *computeS(LLVMValueRef fn);

/**
 * \brief Given some function, compute the optimization metadata for each basic
 * block
 *
 * For a function, this method iterates through every basic block and computes
 * the following sets:
 *   - gen
 *   - kill
 *   - in
 *   - out
 *
 * The method creates a vector of metadata structs that contain a reference to
 * the basic block each set corresponds to.
 *
 * \param[in] fn A LLVM function containing basic blocks
 * \param[in] S a set of all of the constant store instructions in the
 * function
 * \returns A vector of metadata structs
 */
meta_vec_t *computeBlockMData(LLVMValueRef fn, val_vec_t *S);

/**
 * \brief Find the predecessors for a given basic block
 *
 * Given some basic block, this method will compute the precedessors of the
 * basic block in the control flow graph.
 *
 * \param[in] vec An allocated and initialized vector of metadata, containing
 * all of the basic blocks that could be predecessors to the current block to
 * inspect.
 * \param[in] currBlock the metadata corresponding to the basic block to
 * compute the precedessors for. This metadata block *must* be in the vector.
 * \returns A new vector with the precedessors of the basic block
 */
void computePreds(meta_vec_t *vec);

/**
 * \brief Find the metadata block that corresponds to a basic block in a
 * vector.
 *
 * This is a convenienc function that finds the associated metadata structure
 * for some basic block, given a vector struct.
 *
 * \param[in] vec The vector to inspect
 * \param[in] bb The basic block for which we want to find the metadata
 * \returns A reference to the metadata entry in the vector that corresponds to
 * the basic block.
 */
meta_t *vec_find_bb(meta_vec_t *vec, LLVMBasicBlockRef bb);

/**
 * \brief Delete a metadata vector and all data inside
 *
 * Given some metadata vector, this will delete all of the vectors inside the
 * metadata vector.
 *
 * \param[in] vec The vector to delete
 */
void meta_vec_delete(meta_vec_t *vec);
