/**
 * \file ast.h
 * \author Afnan Enayet
 *
 * Defines the interface for interacting with the AST.
 */

#include <stdbool.h>

#include "vector.h"

#pragma once

/**
 * \brief A enum that stores the possible types for each AST node.
 */
typedef enum {
    /// variable
    T_VAR,

    /// constant
    T_CONST,

    /// if statement
    T_IF,

    /// else statement
    T_ELSE,

    /// while loop
    T_WHILE,

    /// declaration
    T_DECL,

    /// binary expression
    T_BEXP,

    /// unary expression
    T_UEXP,

    /// sequence
    T_SEQ,

    /// root node
    T_MAIN,
} type_t;

/**
 * \brief Possible operations for a binary operation
 *
 * Valid operations for a binary operation, defined as an enum for convenience.
 */
typedef enum {
    /// addition operator `-`
    PLUS,

    /// substraction operator `-`
    MINUS,

    /// multiplication operator `*`
    MULT,

    /// division operator `/`
    DIV,

    /// greater than comparative operator `>`
    GEQ,

    /// less than comparative operator `<`
    LEQ,

    /// equality comparison operator `==`
    EQ,
} b_op_t;

/**
 * \brief Possible operations for a unary operation
 *
 * Valid operations for a unary operation, defined as an enum for convenience.
 */
typedef enum {
    /// not operator `!`
    NOT,

    /// negation operator `-`
    NEG,

    /// no-op, returns the value `+`
    POS,
} u_op_t;

// Need to forward-declare
struct ast_node_s;

/**
 * \brief Data struct for a sequence.
 *
 * Contains information for a sequence. It is a list of children.
 * The vector for the children will have to be created using the new
 * vector function provided in `vector.h`.
 */
typedef struct {
    /// A vector of pointer to the child nodes for this sequence
    vector_t *children;
} seq_n;

/**
 * \brief Data struct for a mutable variable.
 *
 * A data structure for a mutable variable. The data is an `int`.
 */
typedef struct {
    /// The identifier for the variable, it is a string.
    char *id;

    /// The data being stored. It is mutable.
    int data;
} var_n;

/**
 * \brief Data struct for a constant variable
 */
typedef struct {
    /// The identifier for the constant, it is a string.
    char *id;

    /// The data being stored, it is immutable.
    int constant;
} const_n;

/**
 * \brief Data struct for an if/else construct
 */
typedef struct {
    /// the if-else condition
    struct ast_node_s *cond;

    /// the if statement, this should be a sequence node
    struct ast_node_s *if_stmt;

    /// the else statement, this should be a sequence node
    struct ast_node_s *else_stmt;
} if_else_n;

/**
 * \brief Data struct for a while construct
 */
typedef struct {
    /// The condition to perpetuate the loop
    struct ast_node_s *cond;

    /// The sequence of actions to take in the body of the loop
    struct ast_node_s *body;
} while_n;

/**
 * \brief Data struct for a function.
 *
 * A function only holds a name and a sequence (no arguments or other
 * fancy things).
 */
typedef struct {
    /// the name of the function
    char *name;

    /// the sequence that will be executed
    struct ast_node_s *fn_seq;
} func_n;

/**
 * \brief Data struct for a variable/constant declaration.
 *
 * The data structure for a declaration. It does not contain a value, but only
 * the information for a variable being declared. In this particular subset of
 * C, a variable cannot be initialized and defined at the same time.
 */
typedef struct {
    /// The variable/constant name of the variable or constant being declared
    char *id;

    /// Whether the declared variable is a constant value. In this case, the
    /// variable can only be assigned a value once, and this value cannot
    /// change.
    bool constant;
} decl_n;

/**
 * \brief Data struct for a binary expression.
 */
typedef struct {
    /// The operation being applied to the binary expression
    b_op_t op;

    /// The left hand side of the operation
    struct ast_node_s *lhs;

    /// The right hand side of the operation
    struct ast_node_s *rhs;
} bexpr_n;

/**
 * \brief Data struct for a unary expression.
 */
typedef struct {
    /// The operation being applied to the unary expression
    u_op_t op;

    /// The expression
    struct ast_node_s *expr;
} uexpr_n;

/**
 * \brief A union containing data for each node type.
 *
 * Each AST node has some data field that contains pertinent information
 * relevant for each node type/operation/symbol.
 */
typedef union node_data {
    seq_n sequence;
    var_n var;
    const_n constant;
    decl_n declaration;
    if_else_n if_else;
    while_n while_loop;
    func_n func;
    bexpr_n b_expr;
    uexpr_n u_expr;
} node_data_u;

/**
 * \brief The definition for an AST node with a variable number of children.
 *
 * This defines the type for an AST node. It consists of a type enum so the
 * parser can discern what kind of data will be in the `data` field.
 * The data field is a union consisting of various structs that contain
 * relevant information for each operation.
 *
 * _Note: be sure to place this definition under all of the node data structs,
 * otherwise it will be significantly more verbose as you will have to forward
 * declare all of the structs in the union._
 */
typedef struct ast_node_s {
    /// An enum declaring the node type
    type_t n_type;

    /// Data relevant for the node type, if applicable.
    node_data_u data;
} ast_node_t;

/// The global root node of the AST. This is an empty _no-op_ node. It
/// represents the `main` function.
extern ast_node_t *ast_root;

// ---------------------------------------------------------------------------
// Function prototypes
// ---------------------------------------------------------------------------

/** Retrieve the root node of the AST
 *
 * If it does not already exist then it will be initialized and allocated.
 * This function returns a pointer to the existing global variable, which
 * can be ignored, as you can just reference `ast_root`.
 *
 * This node will need to be free'd.
 *
 * \return A pointer to the global root node designated ast the root of the
 *     AST
 */
ast_node_t *get_root();

/** Create/allocate an AST node
 *
 * This initializes a node with no children and no type. This node will need
 * to be free'd.
 *
 * \return A newly allocated node
 */
ast_node_t *create_node();

/** Create/allocate an AST node with the supplied type.
 *
 * This initializes a node with the provided type, and no children. This node
 * will need to be free'd.
 *
 * \param[in] type The type information for the node. This will be copied into
 *     the node struct
 * \return A newly allocated node
 */
ast_node_t *create_node_type(type_t type);

/** Create/allocate an AST node with the supplied type and data struct
 *
 * This is a convenience function to initialize a node with a type and some
 * supplied data. This node will need to be free'd.
 *
 * \param[in] type The type information for the node. This will be copied into
 *     the node struct
 * \param[in] data A valid structure with relevant information for the node.
 * \return A newly allocated node
 */
ast_node_t *create_node_type_data(type_t type, node_data_u data);

/** Delete a provided AST node
 *
 * Deletes an AST node, given a pointer. This will free the node and its
 * internal structs.
 *
 * \param[out] node A pointer to a node which will be free'd.
 */
void delete_node(ast_node_t *node);


/**
 * \brief Convenience function to create a sequence AST node
 *
 * Creates the proper structure, allocates the vector, and allocates
 * a node.
 *
 * \return A new node of type `T_SEQ` and an allocated vector
 *     for the node's children
 */
ast_node_t *create_node_seq();
