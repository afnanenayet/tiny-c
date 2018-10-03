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
    ast_node_t *node;
    char *str;
    char id;
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
%token PLUS
%token MULT
%token DIV
%token GEQ
%token LEQ
%token EQ

%type<val> LITERAL value
%type<id> VAR
%type<ast_node_t*> program expression while if_else variable constant if_stmt else_stmt declaration b_exp u_exp

%%

program:
       expression { }
       | expression program { }

expression:
          term b_op term {
            bexpr_n data = {$2};
            $$ = create_node_type_data(T_BEXP, data);
          }
          | u_op term { }

term:
    INT ASSIGN value {
         var_n data = { $3 };
         $$ = create_node_type_data(T_VAR, data);
    }
    | INT ASSIGN VAR { }

value:
     LITERAL { $$ = $1; }
     VAR { $$ = $1 }

b_op:
    MULT { $$ = b_op_t MULT; }
    | DIV { $$ = b_op_t DIV; }
    | MINUS { $$ = b_op_t MINUS; }
    | PLUS { $$ = b_op_t PLUS; }
    | EQ { $$ = b_op_t EQ; }
    | GEQ { $$ = b_op_t GEQ; }
    | LEQ { $$ = b_op_t LEQ; }

u_op:
    MINUS { $$ = u_op_t NEG; }
    NOT { $$ = u_op_t NOT; }

%%
