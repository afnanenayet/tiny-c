%{
/**
 * minic.yacc
 *
 * Grammar definition for the subset of C specified in the assignment
 */

#include <stdbool.h>

#include "ast.h"

extern int yylex();
%}

%union{
    int val; // all values are integers in this subset of C
    char *id;
    ast_node_t *node;
    b_op_t binary_op;
    u_op_t unary_op;
}

%start program

%token INT
%token LITERAL
%token VAR
%token CONST
%token WHILE
%token IF
%token ELSE
%token ELIF
%token ASSIGN
%token S_BRACE
%token E_BRACE
%token S_PAREN
%token E_PAREN
%token SEMI
%token MINUS_TOK
%token PLUS_TOK
%token MULT_TOK
%token DIV_TOK
%token GEQ_TOK_TOK
%token LEQ_TOK_TOK
%token EQ_TOK
%token NOT_TOK
%token EOF

%type<val> LITERAL
%type<id> VAR
%type<node> program expression while if_else seq term declaration assignment code_block statement
%type<node> if_stmt else_stmt
%type<binary_op> b_op
%type<unary_op> u_op

%%

program:
       seq {
            ast_node_t *ast_root = get_root();
            add_child(ast_root, $1);
       }

code_block:
          S_BRACE seq E_BRACE {
              $$ = $2;
          }

seq:
   statement SEMI {
       // `data` is intentionally not initialized, as it is not used.
       // sequences only have children, and we don't need to store any data
       // with them
       node_data_u data;
       ast_node_t *expr = $1;
       ast_node_t *seq = create_node_type_data(T_SEQ, data);
       add_child(seq, expr);
       $$ = seq;
   }
   | seq statement SEMI {
       node_data_u data;
       ast_node_t *seq = $1;
       ast_node_t *expr = $2;
       add_child(seq, expr);
       $$ = seq;
   }
   ;

expression:
          term b_op term {
            node_data_u data;
            data.b_expr = (bexpr_n) {$2, $1, $3};
            $$ = create_node_type_data(T_BEXP, data);
          }
          | u_op term {
            node_data_u data;
            data.u_expr = (uexpr_n) {$1, $2};
            $$ = create_node_type_data(T_UEXP, data);
          }
          ;

statement:
         assignment SEMI { $$ = $1; }
         | while { $$ = $1; }
         | if_else { $$ = $1; }
         | declaration SEMI { $$ = $1; }

assignment:
          VAR ASSIGN LITERAL {
              ast_node_t *node;
              node_data_u data;
              data.var = (var_n) { $1, $3 };
              $$ = create_node_type_data(T_VAR, data);
          }
          | VAR ASSIGN term {
              node_data_u data;
              data.var = (var_n) {$1, 0};
              ast_node_t *node = create_node_type_data(T_VAR, data);
              add_child(node, $3);
              $$ = node;
          }
          ;

while:
        WHILE S_PAREN expression E_PAREN code_block {
        }
        | WHILE S_PAREN expression E_PAREN statement {
        }
     ;


if_else:
       ;

if_stmt:
       ;

else_stmt:
         ;

declaration:
           INT VAR SEMI {
               node_data_u data;
               data.declaration = (decl_n) { $2, false };
               $$ = create_node_type_data(T_DECL, data);
           }
           | CONST INT VAR SEMI {
               node_data_u data;
               data.declaration = (decl_n) { $3, true };
               $$ = create_node_type_data(T_DECL, data);
           }
           ;

term:
    VAR {
         node_data_u data;
         data.var = (var_n) { $1, 0 };
         $$ = create_node_type_data(T_VAR, data);
    }

b_op:
    MULT_TOK { $$ = MULT; }
    | DIV_TOK { $$ = DIV; }
    | MINUS_TOK { $$ = MINUS; }
    | PLUS_TOK { $$ = PLUS; }
    | EQ_TOK { $$ = EQ; }
    | GEQ_TOK_TOK { $$ = GEQ; }
    | LEQ_TOK_TOK { $$ = LEQ; }

u_op:
    MINUS_TOK { $$ = NEG; }
    | NOT_TOK { $$ = NOT; }

%%
