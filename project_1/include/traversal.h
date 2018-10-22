/**
 * \file traversal.h
 * \author Afnan Enayet
 *
 * This module provides functions to help traversal the abstract syntax tree
 * easily.
 */

#include "ast.h"
#include "vector.h"

#pragma once

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
vector_t *get_child_nodes(const ast_node_t *node);

/**
 * \brief Delete a tree given the root node
 *
 * This recursively deletes the tree given some root node
 * \param[out] The root node of the tree (or sub-tree) to delete
 */
void delete_tree(ast_node_t *root);
