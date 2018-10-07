/**
 * minicc.c
 *
 * Main entry point for executable
 */

#include<stdio.h>

#include "ast.h"
#include "parser.h"

int main(void) {
    // create ast root
    ast_root = create_node_type(T_MAIN);
    yyparse();
    printf("%d children\n", ast_root->num_children);
    return 0;
}
