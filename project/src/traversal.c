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
    vector_t *children = vector_new();

    switch (node->n_type) {
    case T_VAR:
        vector_delete(children);
        children = NULL;
        break;
    case T_CONST:
        vector_delete(children);
        children = NULL;
        break;
    case T_IF_ELSE:
        break;
    case T_SEQ:
        break;
    case T_BEXP:
        break;
    case T_UEXP:
        break;
    case T_LVAL:
        vector_delete(children);
        children = NULL;
        break;
    case T_DECL:
        break;
    default:
        vector_delete(children);
        children = NULL;
        break;
    }
    return children;
}
