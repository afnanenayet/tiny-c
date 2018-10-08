/**
 * \file pretty_print.h
 * \author Afnan Enayet
 *
 * A collection of functions to print the contents of a node in an
 * aesthetically pleasing manner. This is useful for debugging as well.
 */

#include "ast.h"

#pragma once

/**
 * \brief Print the contents of a node
 *
 * This function prints the contents of a node, and the identification of
 * the node itself, according to rules that have been defined in
 * `pretty_print.c`. It is a convenience function that takes care of
 * formatting for you.
 *
 * \param[in] node A node with type from the abstract syntax tree
 */
void node_print(ast_node_t *node);
