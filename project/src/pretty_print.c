/**
 * \file pretty_print.c
 * \author Afnan Enayet
 *
 * A collection of functions that print the contents of a node in an
 * aesthetically pleasing manner.
 */

#include <stdio.h>

#include "pretty_print.c"
#include "ast.h"

// ---------------------------------------------------------------------------
// Private function prototypes
// ---------------------------------------------------------------------------

/**
 * \brief Print the contents of a `T_VAR` node
 *
 * \param[in] node a T_VAR node
 */
static void node_t_var_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_IF` node
 *
 * \param[in] node a `T_IF` node
 */
static void node_t_if_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_SEQ` node
 *
 * \param[in] node a `T_SEQ` node
 */
static void node_t_seq_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_BEXP` node
 *
 * \param[in] node a `T_BEXP` node
 */
static void node_t_bexp_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_UEXP` node
 *
 * \param[in] node a `T_UEXP` node
 */
static void node_t_uexp_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_LVAL` node
 *
 * \param[in] node a `T_LVAL` node
 */
static void node_t_lval_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_DECL` node
 *
 * \param[in] node a `T_DECL` node
 */
static void node_t_decl_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_CONST` node
 *
 * \param[in] node a `T_CONST` node
 */
static void node_t_const_print(ast_node_t *node);

/**
 * \brief Print the contents of a `T_WHILE` node
 *
 * \param[in] node a `T_WHILE` node
 */
static void node_t_while_print(ast_node_t *node);

// ---------------------------------------------------------------------------
// Private function definitions
// ---------------------------------------------------------------------------

static void node_t_var_print(ast_node_t *node) {
    printf("Node type: T_VAR\n");
}

// ---------------------------------------------------------------------------
// Public function definitions
// ---------------------------------------------------------------------------

void node_print(ast_node_t *node) {
    switch (node->n_type) {
    case T_VAR:
        node_t_var_print(node);
        break;
    case T_IF:
        node_t_if_print(node);
        break;
    case T_SEQ:
        node_t_seq_print(node);
        break;
    case T_BEXP:
        node_t_bexp_print(node);
        break;
    case T_UEXP:
        node_t_uexp_print(node);
        break;
    case T_LVAL:
        node_t_lval_print(node);
        break;
    case T_DECL:
        node_t_decl_print(node);
        break;
    case T_CONST:
        node_t_const_print(node);
        break;
    case T_WHILE:
        node_t_while_print(node);
        break;
    default:
        printf("Unknown or unsupported node type\n");
        break;
    }
}
