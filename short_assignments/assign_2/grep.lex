%option yylineno
%option noyywrap
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *search_str = NULL;
char *curr_line = NULL;
%}

%x found

letter [a-zA-Z]
word {letter}+
line (.*)\n

%%

<INITIAL,found>{word} {
    /* If a word matches the string that we are looking for, use the 'found'
     * condition, which will cause the line to be dumped at the end.
     */
    yymore();
    if (strcmp(search_str, yytext) == 0) {
        BEGIN(found);
    }
}

<found>{line} {
    yymore();
    ECHO;
    BEGIN(INITIAL);
}

. { }

\n {}

%%

int main(int argc, char *argv[])
{
    if (argc > 1) {
        unsigned int str_len = sizeof(argv[1]);
        search_str = malloc(str_len + 1);
        strcpy(search_str, argv[1]);
        yylex();
        free(search_str);
        return 0;
    }

    printf("usage: ./a.out [search word]\n");
    return 1;
}
