/**
 * \file vector.c
 * \author Afnan Enayet
 *
 * Implementation details for the basic vector type used in the abstract
 * syntax tree.
 */

#include <stdlib.h>

#include "vector.h"

// ---------------------------------------------------------------------------
// Private constants
// ---------------------------------------------------------------------------

/// The default size to allocate to each vector to start with
const size_t DEF_NEW_SIZE = 256;

// ---------------------------------------------------------------------------
// Private function prototypees
// ---------------------------------------------------------------------------

/**
 * \brief Resizes the internal array of the vector
 *
 * This resizes the internal array of a vector so that it has a higher
 * capacity. You cannot resize a vector to have a smaller capacity than
 * it's current number of elements.
 *
 * Because of how realloc works, the original vector will still be valid if
 * the operation fails.
 *
 * \param[out] vec The vector to be internally resized
 * \param[in] new_size The new size to resize the vector to
 * \return Whether the operation was successful
 */
static bool resize_internal_array(vector_t *vec, size_t new_size);

// ---------------------------------------------------------------------------
// Private function definitions
// ---------------------------------------------------------------------------

static bool resize_internal_array(vector_t *vec, size_t new_size) {
    // sanity check -- make sure new_size > n
    if (new_size <= vec->n)
        return false;

    void **new_vec = realloc(vec->vec, sizeof(void *) * new_size);

    // if realloc fails, bail
    if (new_vec == NULL)
        return false;

    vec->vec = new_vec;
    vec->size = new_size;
    return true;
}

// ---------------------------------------------------------------------------
// Public function definitions
// ---------------------------------------------------------------------------

vector_t *vector_new() {
    vector_t *vec = malloc(sizeof(vector_t));
    vec->vec = malloc(sizeof(void *) * DEF_NEW_SIZE);
    return vec;
}

void vector_delete(vector_t *vec) {
    free(vec->vec);
    free(vec);
}

bool vector_add(vector_t *vec, void *item) {
    // if we don't have the capacity to add a new element, increase the memory
    // allocated to the vector
    if (vec->n > (vec->size - 1) && !resize_internal_array(vec, vec->size * 2))
        return false; // operation failed

    // add element, increment element count
    *(vec->vec + vec->n++) = item;
    return true;
}

unsigned long vector_len(const vector_t *vec) { return vec->n; }

void *vector_get(const vector_t *vec, unsigned long i) {
    return *(vec->vec + i);
}
