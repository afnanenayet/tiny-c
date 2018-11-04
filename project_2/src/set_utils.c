#include <stdbool.h>

#include "set_utils.h"
#include "vec.h"

vec_void_t *setUnion(vec_void_t *a, vec_void_t *b) {
    vec_void_t tempSet;
    vec_init(&tempSet);
    int i;
    void *it;

    // add all elements from set a
    vec_foreach(a, it, i) { vec_push(&tempSet, it); }

    // add all elements from set b
    vec_foreach(b, it, i) { vec_push(&tempSet, it); }
    vec_void_t *dedupedSet = setDedup(&tempSet);
    vec_deinit(&tempSet);
    return dedupedSet;
}

vec_void_t *setDedup(vec_void_t *a) {
    vec_void_t *newSet = malloc(sizeof(vec_void_t));
    vec_init(newSet);
    void *it;
    int i = 0;
    int found = -1;

    // Create a new vector, checking to see if each element can be found to
    // avoid adding duplicates
    vec_foreach(a, it, i) {
        vec_find(newSet, it, found);

        if (found == -1)
            vec_push(newSet, it);
    }
    return newSet;
}

bool setEqual(vec_void_t *a, vec_void_t *b) {
    bool equal = true;
    void *it;
    int i;
    int found = -1;

    vec_foreach(a, it, i) {
        vec_find(b, it, found);

        if (found == -1)
            equal = false;
    }

    vec_foreach(b, it, i) {
        vec_find(a, it, found);

        if (found == -1)
            equal = false;
    }
    return equal;
}

vec_void_t *setCopy(vec_void_t *set) {
    vec_void_t temp;
    vec_init(&temp);

    void *data;
    int i;

    vec_foreach(set, data, i) { vec_push(&temp, data); }
    return setDedup(&temp);
}
