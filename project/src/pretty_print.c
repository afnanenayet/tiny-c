/**
 * \file pretty_print.c
 * \author Afnan Enayet
 *
 * A collection of functions that print the contents of a node in an
 * aesthetically pleasing manner.
 */

#include <stdio.h>

#include "ast.h"
#include "pretty_print.h"

// ---------------------------------------------------------------------------
// Private function prototypes
// ---------------------------------------------------------------------------

/**
 * \brief Print the contents of a `T_VAR` node
 *
 * \param[in] node a T_VAR node
 */
static void node_t_var_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_IF` node
 *
 * \param[in] node a `T_IF` node
 */
static void node_t_if_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_SEQ` node
 *
 * \param[in] node a `T_SEQ` node
 */
static void node_t_seq_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_BEXP` node
 *
 * \param[in] node a `T_BEXP` node
 */
static void node_t_bexp_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_UEXP` node
 *
 * \param[in] node a `T_UEXP` node
 */
static void node_t_uexp_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_LVAL` node
 *
 * \param[in] node a `T_LVAL` node
 */
static void node_t_lval_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_DECL` node
 *
 * \param[in] node a `T_DECL` node
 */
static void node_t_decl_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_CONST` node
 *
 * \param[in] node a `T_CONST` node
 */
static void node_t_const_print(const ast_node_t *node);

/**
 * \brief Print the contents of a `T_WHILE` node
 *
 * \param[in] node a `T_WHILE` node
 */
static void node_t_while_print(const ast_node_t *node);

// ---------------------------------------------------------------------------
// Private function definitions
// ---------------------------------------------------------------------------

static void node_t_var_print(const ast_node_t *node) {
    printf("Node type: T_VAR\n");
    printf("\t-> id: %s\n", node->data.var.id);
    printf("\t-> value: %p\n", (void *)node->data.var.value);
}

static void node_t_if_print(const ast_node_t *node) {
    printf("Node type: T_IF\n");
    printf("\t-> condition: %p\n", (void *)node->data.if_else.cond);
    printf("\t-> if statement: %p\n", (void *)node->data.if_else.if_stmt);
    printf("\t-> else statement: %p\n", (void *)node->data.if_else.else_stmt);
}

static void node_t_seq_print(const ast_node_t *node) {
    printf("Node type: T_SEQ\n");
    vector_t *children = node->data.sequence.children;

    for (int i = 0; i < vector_len(children); i++) {
        printf("\t->child[%d]: %p\n", i, vector_get(children, i));
    }
}

static void node_t_bexp_print(const ast_node_t *node) {
    printf("Node type: T_BEXP\n");
    bexpr_n data = node->data.b_expr;
    printf("\t-> operation: %d\n", data.op);
    printf("\t-> lhs: %p\n", (void *)data.lhs);
    printf("\t-> rhs: %p\n", (void *)data.rhs);
}

static void node_t_uexp_print(const ast_node_t *node) {
    printf("Node type: T_UEXP\n");
    uexpr_n data = node->data.u_expr;
    printf("\t-> operation: %d\n", data.op);
    printf("\t-> expression: %p\n", (void *)data.expr);
}

static void node_t_lval_print(const ast_node_t *node) {
    printf("Node type: T_LVAL\n");
    printf("\t-> value: %d", node->data.literal.lval);
}

static void node_t_decl_print(const ast_node_t *node) {
    printf("Node type: T_DECL\n");
    printf("\t-> variable name: %s", node->data.declaration.id);
    printf("\t-> id?: %d", node->data.declaration.constant);
}

static void node_t_const_print(const ast_node_t *node) {
    printf("Node type: T_CONST\n");
}

static void node_t_while_print(const ast_node_t *node) {
    printf("Node type: T_WHILE\n");
    printf("\t-> condition: %p\n", (void *)node->data.while_loop.cond);
    printf("\t-> body: %p\n", (void *)node->data.while_loop.body);
}

// ---------------------------------------------------------------------------
// Public function definitions
// ---------------------------------------------------------------------------

void node_print(const ast_node_t *node) {
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
