line_comment    "//".*
multiline_comment "/*"(\n|.)*"*/"

%%

{multiline_comment} printf("%s", yytext);
{line_comment} printf("%s", yytext);
.   NULL;
