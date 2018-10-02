/* ast.h
 * Afnan Enayet, 2018
 *
 * Defines the interface for interacting with the AST.
 */

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
} type_t;

/**
 * \brief Possible operations for a binary operation
 *
 * Valid operations for a binary operation, defined as an enum for convenience.
 */
typedef enum {
    PLUS,
    MINUS,
    MULT,
    DIV,
    R_SHIFT,
    L_SHIFT,
} b_op_t;

/**
 * \brief Possible operations for a unary operation
 *
 * Valid operations for a unary operation, defined as an enum for convenience.
 */
typedef enum {
    /// increment operator `++`
    INC,

    /// decrement operator `--`
    DEC,

    /// not operator `!`
    NOT,

    /// negation operator `-`
    NEG,

    /// no-op, returns the value `+`
    POS,
} u_op_t;

struct ast_node_s;

/**
 * \brief Data struct for a mutable variable.
 *
 * A data structure for a mutable variable. The data is an `int`.
 */
typedef struct {
    /// The data being stored. It is mutable.
    int data;
} var_n;

/**
 * \brief Data struct for a constant variable
 */
typedef struct {
    /// The data being stored, it is immutable.
    const int constant;
} const_n;

/**
 * \brief Data struct for an if/else construct
 */
typedef struct {
    /// the if-else condition
    struct ast_node_s *cond;

    /// the if statement
    struct ast_node_s *if_stmt;

    /// the else statement
    struct ast_node_s *else_stmt;
} if_else_n;

/**
 * \brief The root node, or global main.
 *
 * The root node is effectively a global main function.
 */
typedef struct {
    /// main func
    struct ast_node_s *main_n;
} root;

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
 * \brief A union containing data for each node type.
 *
 * Each AST node has some data field that contains pertinent information
 * relevant for each node type/operation/symbol.
 */
typedef union node_data {
    var_n var;
    const_n constant;
    if_else_n if_else;
    func_n func;
    bexpr_n b_expr;
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
typedef struct {
    /// An enum declaring the node type
    type_t node_type;

    /// Data relevant for the node type, if applicable.
    node_data_u data;

    /// The pointers to the child nodes of this node
    struct ast_node_s *children;
} ast_node_t;
