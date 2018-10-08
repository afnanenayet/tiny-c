/**
 * \file ast.c
 * \author Afnan Enayet
 *
 * Function implementations to build and construct an abstract syntax tree.
 * These functions can be invoked via bison.
 */

#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "vector.h"

// Define the global root (NULL at default to indicate it hasn't been
// initialized)
ast_node_t *ast_root = NULL;

/**
 * \brief Retrieve the root node of the AST
 *
 * If it does not already exist then it will be initialized and allocated.
 * This function returns a pointer to the existing global variable, which
 * can be ignored, as you can just reference `ast_root`.
 */
ast_node_t *get_root() {
    // If root exists, just return a pointer to the root
    if (ast_root != NULL)
        return ast_root;

    ast_root = create_node();
    return ast_root;
}

/**
 * \brief Create/allocate an AST node
 *
 * This initializes a node with no type.
 */
ast_node_t *create_node() {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    return node;
}

/**
 * \brief Create/allocate an AST node with the supplied type.
 *
 * This initializes a node with the provided type
 */
ast_node_t *create_node_type(type_t type) {
    ast_node_t *node = create_node();
    node->n_type = type;
    return node;
}

/**
 * \brief Create/allocate an AST node with the supplied type and data struct
 *
 * This is a convenience function to initialize a node with a type and some
 * supplied data.
 */
ast_node_t *create_node_type_data(type_t type, node_data_u data) {
    ast_node_t *node = create_node_type(type);
    node->data = data;
    return node;
}

/**
 * \brief Delete a provided AST node
 *
 * Deletes an AST node, given a pointer. This will free the node and its
 * internal structs.
 */
void delete_node(ast_node_t *node) {
    if (node->n_type == T_SEQ)
        vector_delete(node->data.sequence.children);
    free(node);
}

/**
 * \brief Convenience function to create a sequence AST node
 *
 * \return A new node of type `T_SEQ` and an allocated vector
 *     for the node's children
 */
ast_node_t *create_node_seq() {
    ast_node_t *node = create_node_type(T_SEQ);
    node_data_u data;
    data.sequence = (seq_n){vector_new()};
    return node;
}

/**
 * \brief Walk through a tree and recreate the program based on the tree
 */
void recreate_program(ast_node_t *tree) {}
