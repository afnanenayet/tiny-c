%{
/**
 * minic.yacc
 *
 * Grammar definition for the subset of C specified in the assignment
 */

#include "ast.h"

%}

%union{
    int val; // all values are integers in this subset of C
    char *str;
    char id;
    ast_node_t *node;
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
%token SEMI
%token MINUS
%token PLUS_TOK
%token MULT_TOK
%token DIV_TOK
%token GEQ_TOK_TOK
%token LEQ_TOK_TOK
%token EQ_TOK
%token NOT_TOK

%type<val> LITERAL
%type<id> VAR value
%type<ast_node_t*> program expression while if_else variable constant seq term
%type<b_op_t> b_op
%type<u_op_t> u_op

%%

program:
       seq { }
       | seq program { }

seq:
   expression { }
   | expression seq { }

expression:
          term b_op term {
            bexpr_n data = {$2, $1, $3};
            $$ = create_node_type_data(T_BEXP, data);
          }
          | u_op term {
            uexpr_n data = {$1, $2};
            $$ = create_node_type_data(T_UEXP, data);
          }

term:
    INT ASSIGN value SEMI {
         var_n data = { $3 };
         $$ = create_node_type_data(T_VAR, data);
    }

constant:
        CONST INT LITERAL SEMI {
            const_n constant = { $3 };
            $$ = create_node_type_data(type_t T_CONST, constant);
        }

value:
     LITERAL { $$ = $1; }

b_op:
    MULT_TOK { $$ = b_op_t MULT_TOK; }
    | DIV_TOK { $$ = b_op_t DIV_TOK; }
    | MINUS { $$ = b_op_t MINUS; }
    | PLUS_TOK { $$ = b_op_t PLUS_TOK; }
    | EQ_TOK { $$ = b_op_t EQ_TOK; }
    | GEQ_TOK_TOK { $$ = b_op_t GEQ_TOK_TOK; }
    | LEQ_TOK_TOK { $$ = b_op_t LEQ_TOK_TOK; }

u_op:
    MINUS { $$ = u_op_t NEG; }
    | NOT_TOK { $$ = u_op_t NOT; }

%%
