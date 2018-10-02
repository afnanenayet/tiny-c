/**
 * ast.c
 *
 * Function implementations to build and construct an abstract syntax tree.
 * These functions can be invoked via bison.
 */

#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"

// Define the global root
ast_node_t *ast_root = create_node();


/** Create/allocate an AST node
 *
 * This initializes a node with no children and no type.
 */
ast_node_t *create_node() {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    node->num_children = 0;

    // arbitrarily allocate space for 256 children
    node->children = malloc(sizeof(ast_node_t *) * 256);
    return node;
}

/** Create/allocate an AST node with the supplied type.
 *
 * This initializes a node with the provided type, and no children.
 */
ast_node_t *create_node_type(type_t type) {
    ast_node_t *node = create_node();
    node->n_type = type;
    return node;
}

/** Create/allocate an AST node with the supplied type and data struct
 *
 * This is a convenience function to initialize a node with a type and some
 * supplied data.
 */
ast_node_t *create_node_type_data(type_t type, node_data_u data) {
    ast_node_t *node = create_node_type(type);
    node->data = data;
    return node;
}

/** Delete a provided AST node
 *
 * Deletes an AST node, given a pointer. This will free the node and its
 * internal structs.
 */
void delete_node(ast_node_t *node) {
    if (node->children != NULL)
        free(node->children);
    free(node);
}

/** Add a child to a node
 *
 * Given a pointer to an AST node, add another node as a child to that node.
 */
void add_child(ast_node_t *node, ast_node_t *child) {
    ast_node_t **array = node->children;

    // add child to the end of the list and increment the number of children
    // currently in the child
    *(array + node->num_children) = child;
    node->num_children++;
}
