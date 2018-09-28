/* ast.h
 * Afnan Enayet, 2018
 *
 * Defines the interface for interacting with the AST.
 */

#pragma once

// A enum that stores the possible types for each AST node.
typedef enum {
  // variable
  T_VAR,

  // constant
  T_CONST,

  // if statement
  T_IF,

  // else statement
  T_ELSE,

  // while loop
  T_WHILE,

  // declaration
  T_DECL,

  // binary expression
  T_BEXP,

  // unary expression
  T_UEXP,

  // sequence
  T_SEQ,
} type_t;

// valid operations for a binary expression
typedef enum {
  PLUS,
  MINUS,
  MULT,
  DIV,
  R_SHIFT,
  L_SHIFT,
} b_op_t;

// The definition for an AST node with a variable number of children.
typedef struct {
  type_t node_type;
  struct ast_node_t *children;
} ast_node_t;

// A variable node
typedef struct {
  // The data being stored. It is mutable.
  int data;
} var_n;

// A const node
typedef struct {
  // The data being stored, it is immutable.
  const int constant;
} const_n;

typedef struct {
  // the if-else condition
  ast_node_t *cond;

  // the if statement
  ast_node_t *if_stmt;

  // the else statement
  ast_node_t *else_stmt;
} if_else_n;

// the root node
typedef struct {
  // main func
  ast_node_t *main_n;
} root;

// the definition of a function
typedef struct {
  // the name of the function
  char *name;

  // the sequence that will be executed
  ast_node_t *fn_seq;
} func_n;

// binary expression
typedef struct {
  // the operation being applied to the binary expression
  b_op_t op;

  // the left hand side of the operation
  ast_node_t *lhs;

  // the right hand side of the operation
  ast_node_t *rhs;
} bexpr_n;
