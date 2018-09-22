%{
#include <stdio.h>

unsigned int word_count = 0;
unsigned int line_count = 0;
%}

newline \n
word    ([a-zA-Z0-9_])+

%%

{newline} {line_count++;}
{word}    {word_count++;}

%%

int main()
{
    yylex();
    printf("Number of lines: %d and number of words: %d\n", line_count,
           word_count);
    return 0;
}
