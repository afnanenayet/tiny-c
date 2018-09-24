%option yylineno noyywrap noinput nounput
%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *search_str = NULL;
int match_length = 0;
%}

%x found

%%

[[:alpha:]]+ {
    yymore();
    /* If a word matches the string that we are looking for, use the 'found'
     * condition, which will cause the line to be dumped at the end.
     */
    if (yyleng >= match_length &&
            strcmp(search_str, yytext + yyleng - match_length) == 0) {
        BEGIN(found);
    }
}

<INITIAL>\n {
    /* reset yytext if no match was found in current line */
}

<found>\n {
    /* If word was found, dump yytext to stdout */
    printf("%s", yytext);
    BEGIN(INITIAL);
}

<found>.*  {
    /* If word was found previously, dump line to stdout */
    printf("%s", yytext);
}

<INITIAL>. {
    /* Store words in buffer in case we need to dump the line to stdout */
    yymore();
}

%%

int main(int argc, char *argv[])
{
    if (argc > 1) {
        search_str = argv[1];
        match_length = strlen(search_str);
        yylex();
        return 0;
    }

    printf("usage: ./a.out [search word]\n");
    return 1;
}
