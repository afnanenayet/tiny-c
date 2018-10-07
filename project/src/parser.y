%{
/**
 * minic.yacc
 *
 * Grammar definition for the subset of C specified in the assignment
 */

#include "ast.h"

extern int yylex();
%}

%union{
    int val; // all values are integers in this subset of C
    char *str;
    char id;
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
%type<id> VAR value
%type<node> program expression while if_else variable constant seq term
%type<binary_op> b_op
%type<unary_op> u_op

%%

program: seq {
            ast_node_t *ast_root = get_root();
            add_child(ast_root, $1);
         }

seq: 
   expression SEMI {
       node_data_u data;
       data.
       $$ = create_node_type_data(T_EXP, data);     
   }
   | seq expression SEMI { }
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
         assignment SEMI { }
         | while SEMI { }
         | if_else SEMI { }
         | declaration SEMI { }

assignment:
          ;

while:
     ;

if_else:
       ;

declaration:
           ;

term:
    INT ASSIGN value SEMI {
        // TODO define term properly
         node_data_u data;
         data.var = (var_n) { $3 };
         $$ = create_node_type_data(T_VAR, data);
    }

constant:
        CONST INT value SEMI {
            node_data_u data;
            data.constant = (const_n) { $3 };
            $$ = create_node_type_data(type_t T_CONST, constant);
        }

value:
     LITERAL { $$ = $1; }

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
