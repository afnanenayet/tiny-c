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

// valid operations for a unary operation
typedef enum {
  // increment (++)
  INC,

  // decrement (--)
  DEC,

  // not (!)
  NOT,

  // negative (-)
  NEG,

  // positive (just returns the value of the variable)
  POS,
} u_op_t;

struct ast_node_s;

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
  struct ast_node_s *cond;

  // the if statement
  struct ast_node_s *if_stmt;

  // the else statement
  struct ast_node_s *else_stmt;
} if_else_n;

// the root node
typedef struct {
  // main func
  struct ast_node_s *main_n;
} root;

// the definition of a function
typedef struct {
  // the name of the function
  char *name;

  // the sequence that will be executed
  struct ast_node_s *fn_seq;
} func_n;

// binary expression
typedef struct {
  // the operation being applied to the binary expression
  b_op_t op;

  // the left hand side of the operation
  struct ast_node_s *lhs;

  // the right hand side of the operation
  struct ast_node_s *rhs;
} bexpr_n;

// The definition for an AST node with a variable number of children.
// Note: be sure to place this definition under all of the node data structs
typedef struct {
  type_t node_type;
  struct ast_node_s *children;
} ast_node_t;
