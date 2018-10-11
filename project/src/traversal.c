/**
 * \file traversal.c
 * \author Afnan Enayet
 *
 * This module provides helper functions to help traverse the AST node.
 * Each AST node type has a different internal data representation, so
 * this module provides an easy interface to help walk through child
 * nodes.
 */

#include "ast.h"
#include "vector.h"

// ----------------------------------------------------------------------------
// Public function definitions
// ----------------------------------------------------------------------------

/**
 * \brief Get a vector with pointers to the child nodes of a particular node.
 *
 * This function is essentially a wrapper for a number of other node types,
 * and it provides a singular interface to get the children of a node.
 *
 * \param[in] node An input node
 * \return A vector with pointers to the child nodes of the given node. If
 * there are no children, this function will return NULL. Any vectors that are
 * returned must be free'd.
 */
vector_t *get_child_nodes(const ast_node_t *node) {
    vector_t *children = NULL;

    switch (node->n_type) {
    case T_IF_ELSE:
        children = vector_new();
        vector_add(children, node->data.if_else.cond);
        vector_add(children, node->data.if_else.if_stmt);
        vector_add(children, node->data.if_else.else_stmt);
        break;
    case T_SEQ:
        children = vector_copy(node->data.sequence.children);
        break;
    case T_BEXP:
        children = vector_new();
        vector_add(children, node->data.b_expr.lhs);
        vector_add(children, node->data.b_expr.rhs);
        break;
    case T_UEXP:
        children = vector_new();
        vector_add(children, node->data.u_expr.expr);
        break;
    case T_WHILE:
        children = vector_new();
        vector_add(children, node->data.while_loop.cond);
        vector_add(children, node->data.while_loop.body);
    case T_FUNC:
        children = vector_new();
        vector_add(children, node->data.func.fn_seq);
    default:
        break;
    }
    return children;
}

/**
 * \brief Delete a tree/subtree given the root node
 *
 * This recursively deletes the tree given some root node
 * \param[out] The root node of the tree (or sub-tree) to delete
 */
void delete_tree(ast_node_t *root) {
    vector_t *children = get_child_nodes(root);

    // recursively delete all children
    if (children != NULL)
        for (unsigned long i = 0; i < children->n; i++)
            delete_tree(vector_get(children, i));

    // delet current node _after_ deleting children
    delete_node(root);
    vector_delete(children);
}
