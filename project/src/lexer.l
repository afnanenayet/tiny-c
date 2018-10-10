%{
#include "ast.h"
#include "parser.h"

extern int yylex();
%}

%option noyywrap
%option nounput
%option noinput

%%

"while" { return WHILE; }
"int" { return INT; }
"const" { return CONST; }
"{" { return S_BRACE; }
"}" { return E_BRACE; }
";" { return SEMI; }
"return" { return RET; }
"if" { return IF; }
"else" { return ELSE; }
"=" { return ASSIGN; }
"(" { return S_PAREN; }
")" { return E_PAREN; }
"+" { return PLUS_TOK; }
"-" { return MINUS_TOK; }
"*" { return MULT_TOK; }
"/" { return DIV_TOK; }
"<" { return LEQ_TOK; }
">" { return GEQ_TOK; }
"==" { return EQ_TOK; }
"!" { return NOT_TOK; }
"main" { return MAIN_ID; }
"print" { return PRINT_ID; }
\"*.\" { yylval.id = strdup(yytext); return STR; };
\n ;
[ \t]+ ;
[0-9]+ { yylval.val = atoi(yytext); return LITERAL; }
[a-zA-Z0-9_]+ { yylval.id = strdup(yytext); return VAR; }

%%
