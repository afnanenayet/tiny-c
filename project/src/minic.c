/**
 * \file minicc.c
 * \author Afnan Enayet
 *
 * Main entry point for executable
 */

#include <stdio.h>

#include "ast.h"
#include "parser.h"

/**
 * \brief Return whether the AST is semantically valid.
 *
 * Walk through the tree and make sure that variables are declared before they
 * are used, and that variables aren't declared multiple times.
 *
 * \param[in] tree the root node of the AST
 * \return whether the AST is semantically valid
 */
bool semantic_check(ast_node_t *tree) { return false; }

int main(void) {
    // create ast root
    ast_root = create_node_seq();
    yyparse();
    return 0;
}
