/**
 * \file set_utils.h
 * \author Afnan Enayet
 * \brief Functions to perform set operations
 *
 * This module provides functions to perform mathematical set operations that
 * are relevant to the optimization algorithms, on vectors from the rxi vec
 * library.
 *
 * All of the functions in this module operate on the type `vec_void_t`, which
 * will work with most vector types, provided that they hold pointers to data,
 * and not the data or any primitives directly (as is usual with any `void *`
 * "generic" programming.
 */
#include "vec.h"

#pragma once

/**
 * \brief Return the union of two sets
 *
 * Given two vectors, this will compute the set union of the elements of these
 * new vectors, and return a new vector containing the union of the two vectors.
 * You should note that these types are (void*) vectors, which attempt to make
 * this function generic, but the vector may have to be casted back to the
 * original type.
 *
 * Both vectors that are passed in should have the same type, or at least have
 * elements that are comparable using the equality operator.
 *
 * \param a A vector/set
 * \param b A vector/set
 * \returns A new vector with the union of the elements between `a` and `b`
 */
vec_void_t *setUnion(vec_void_t *a, vec_void_t *b);

/**
 * \brief Deduplicate the elements of a vector
 *
 * This function initializes a vector by deduplicating the elements of a
 * given vector. This will create a new vector with the deduplicated elements.
 * This can be thought of a set constructor that takes an existing vector,
 * much like Python's `set(obj)` method.
 *
 * \param a A vector
 * \returns A new vector with the deduplicated elements of vector `a`
 */
vec_void_t *setDedup(vec_void_t *a);
