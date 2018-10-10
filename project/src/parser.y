%{
/**
 * minic.yacc
 *
 * Grammar definition for the subset of C specified in the assignment
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "ast.h"
#include "vector.h"

// Declare stuff from Flex that Bison needs to know about:
extern int yylex();
extern int yyparse();
void yyerror(const char *str);

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
%token ENDIF
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
%token GEQ_TOK
%token LEQ_TOK
%token EQ_TOK
%token NOT_TOK
%token EOF_TOK
%token NO_ELSE
%token RET
%token MAIN_ID

%type<val> LITERAL
%type<id> VAR
%type<node> program expression while if_else seq term declaration assignment
%type<node> if_stmt else_stmt code_block statement main_func return_stmt
%type<node> function_block /*function*/
%type<binary_op> b_op
%type<unary_op> u_op

%nonassoc ENDIF
%nonassoc ELSE

%%

program:
       seq main_func {
            vector_t *vec = ast_root->data.sequence.children;
            vector_add(vec, $1);
            vector_add(vec, $2);
       }
       | main_func {
            vector_t *vec = ast_root->data.sequence.children;
            vector_add(vec, $1);
       }

code_block:
          S_BRACE seq E_BRACE {
              $$ = $2;
          }
          | S_BRACE E_BRACE {
            // empty code blocks are legal
            $$ = NULL;
          }

/*
function:
        INT VAR S_PAREN E_PAREN function_block {
            node_data_u data;
            data.func = { $2, $5 };
            $$ = create_node_type_data(T_FUNC, data);
        }
*/

function_block:
              S_BRACE seq return_stmt E_BRACE {
                  ast_node_t *seq = $2;
                  vector_t *vec = seq->data.sequence.children;
                  vector_add(vec, $3);
                  $$ = seq;
              }
              | S_BRACE return_stmt E_BRACE {
                  $$ = $2;
              }

main_func:
         INT MAIN_ID S_PAREN E_PAREN function_block {
            node_data_u data;
            data.func = (func_n) { strdup("main"), $5 };
            $$ = create_node_type_data(T_FUNC, data);
         }

return_stmt:
           RET term SEMI {
               $$ = $2;
           }

seq:
   statement {
       ast_node_t *expr = $1;
       ast_node_t *seq = create_node_seq();

       vector_t *vec = seq->data.sequence.children;
       vector_add(vec, expr);
       $$ = seq;
   }
   | code_block {
       ast_node_t *expr = $1;
       ast_node_t *seq = create_node_seq();

       vector_t *vec = seq->data.sequence.children;
       vector_add(vec, expr);
       $$ = seq;
   }
   | seq statement {
       ast_node_t *seq = $1;
       ast_node_t *stmt = $2;

       vector_t *vec = seq->data.sequence.children;
       vector_add(vec, stmt);
       $$ = seq;
   }
   | seq code_block {
       ast_node_t *seq = $1;
       ast_node_t *stmt = $2;

       vector_t *vec = seq->data.sequence.children;
       vector_add(vec, stmt);
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
          | term {
              $$ = $1;
          }

statement:
         assignment SEMI { $$ = $1; }
         | while { $$ = $1; }
         | if_else { $$ = $1; }
         | declaration SEMI { $$ = $1; }

assignment:
          VAR ASSIGN expression {
              node_data_u data;
              data.var = (var_n) {$1, $3};
              ast_node_t *node = create_node_type_data(T_VAR, data);
              $$ = node;
          }

while:
     WHILE S_PAREN expression E_PAREN code_block {
            ast_node_t *condition = $3;
            ast_node_t *body = $5;
            node_data_u data;
            data.while_loop = (while_n) { condition, body };
            $$ = create_node_type_data(T_WHILE, data);
        }
        | WHILE S_PAREN expression E_PAREN statement {
            ast_node_t *condition = $3;
            ast_node_t *body = $5;
            node_data_u data;
            data.while_loop = (while_n) { condition, body };
            $$ = create_node_type_data(T_WHILE, data);
        }


if_else:
       if_stmt %prec ENDIF {
           $$ = $1;
       }
       | if_stmt else_stmt {
           ast_node_t *node = $1;
           node->data.if_else.else_stmt = $2;
           $$ = node;
       }

if_stmt:
       IF S_PAREN expression E_PAREN code_block {
           node_data_u data;
           data.if_else = (if_else_n) { $3, $5, NULL };
           $$ = create_node_type_data(T_IF_ELSE, data);
       }
       | IF S_PAREN expression E_PAREN statement {
           node_data_u data;
           data.if_else = (if_else_n) { $3, $5, NULL };
           $$ = create_node_type_data(T_IF_ELSE, data);
       }

else_stmt:
         ELSE statement {
            $$ = $2;
         }
         | ELSE code_block {
            $$ = $2;
         }

declaration:
           INT VAR {
               node_data_u data;
               data.declaration = (decl_n) { $2, false };
               $$ = create_node_type_data(T_DECL, data);
           }
           | CONST INT VAR {
               node_data_u data;
               data.declaration = (decl_n) { $3, true };
               $$ = create_node_type_data(T_DECL, data);
           }

term:
    VAR {
         node_data_u data;
         data.var = (var_n) { $1, 0 };
         $$ = create_node_type_data(T_VAR, data);
    }
    | LITERAL {
        node_data_u data;
        data.literal = (lval_n) { $1 };
        $$ = create_node_type_data(T_LVAL, data);
    }

b_op:
    MULT_TOK { $$ = MULT; }
    | DIV_TOK { $$ = DIV; }
    | MINUS_TOK { $$ = MINUS; }
    | PLUS_TOK { $$ = PLUS; }
    | EQ_TOK { $$ = EQ; }
    | GEQ_TOK { $$ = GEQ; }
    | LEQ_TOK { $$ = LEQ; }

u_op:
    MINUS_TOK { $$ = NEG; }
    | NOT_TOK { $$ = NOT; }

%%

void yyerror(const char *str) {
    printf("Error: unable to parse string \"%s\"\n", str);
}
