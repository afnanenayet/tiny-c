/**
 * \file vector.h
 * \author Afnan Enayet
 *
 * Defines the interface for a vector
 */

#include <stdbool.h>
#include <stdlib.h>

#pragma once

/**
 * \brief The vector structure and type.
 *
 * This structure should not be used directly, but rather used in conjunction
 * with the functions provided in the `vector.h` header file.
 *
 * The vector contains pointers to other elements.
 */
typedef struct vector_s {
    /// The number of elements in the vector
    unsigned long n;

    /// The amount of memory that has been allocated to the vector struct
    size_t size;

    /// The internal array containing pointers to the elements
    void **vec;
} vector_t;

/**
 * \brief Create a new vector
 *
 * This function creates and allocates a new vector struct.
 *
 * \return A newly allocated vector
 */
vector_t *vector_new();

/**
 * \brief Delete a vector
 *
 * This function deallocates a vector. It does _not_ deallocate the objects
 * it points to.
 *
 * \param[in] vec The vector to delete
 */
void vector_delete(vector_t *vec);

/**
 * \brief Make a copy of a given vector.
 *
 * Makes a copy of a given vector. This copy must be free'd.
 *
 * \param[in] The old vector to be copied
 * \return A new vector with pointers to all of the elements as the other
 * vector in the same order.
 */
vector_t *vector_copy(vector_t *old);

/**
 * \brief Add a pointer to the vector
 *
 * Adds the given pointer to the vector.
 *
 * \param[out] vec The vector to operate on. This will modify the vector.
 * \param[in] item A pointer to some item to add to the vector
 * \return Whether the operation was successful
 */
bool vector_add(vector_t *vec, void *item);

/**
 * \brief Return the number of elements in the vector
 *
 * \param[in] vec The vector to operate on
 * \return The number of elements in the vector
 */
unsigned long vector_len(const vector_t *vec);

/**
 * \brief Get the element at position `i` in the vector
 *
 * This function provides an access operator in the vector with bounds
 * checking. If `i` is greater than the length of the vector, or generally
 * out of bounds, this function will return `NULL`
 *
 * \param vec The vector to operate on
 * \param i The position to access within the vector, equivalent to vec[i]
 * \return A pointer to the element at `i`, if `i` is out of bounds, then
 *     this returns NULL pointer.
 */
void *vector_get(const vector_t *vec, unsigned long i);
