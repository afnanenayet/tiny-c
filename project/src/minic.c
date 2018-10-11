/**
 * \file minicc.c
 * \author Afnan Enayet
 *
 * Main entry point for executable
 */

#include <search.h>
#include <stdio.h>

#include "ast.h"
#include "parser.h"
#include "traversal.h"

// ***************************************************************************
// Private function prototypes
// ***************************************************************************

/**
 * \brief A helper function to perform a recursive traversal of the tree
 *
 * \param[in] The input node to recurse on
 */
static bool semantic_check_helper(const ast_node_t *node);

/**
 * \brief Return whether the AST is semantically valid.
 *
 * Walk through the tree and make sure that variables are declared before they
 * are used, and that variables aren't declared multiple times.
 *
 * \param[in] tree the root node of the AST
 * \return whether the AST is semantically valid
 */
static bool semantic_check(const ast_node_t *root);

// ***************************************************************************
// Private function definitions
// ***************************************************************************

static bool semantic_check(const ast_node_t *root) {
    // create the hashtable (search.h)
    hcreate(256);
    return semantic_check_helper(root);
}

static bool semantic_check_helper(const ast_node_t *node) {
    ENTRY e, *ep;
    e.data = NULL; // initialize struct to avoid warning

    vector_t *children = get_child_nodes(node);

    // if the node has children, check them first (doing a DFS)
    if (children != NULL) {
        for (unsigned long i = 0; i < children->n; i++) {
            if (!semantic_check_helper(vector_get(children, i)))
                return false;
        }
    }

    // add declarations to the hash table
    if (node->n_type == T_DECL) {
        e.key = node->data.declaration.id;
        ep = hsearch(e, ENTER);

        // ep is null on errors
        if (ep == NULL) {
            fprintf(stderr, "Entry to hashtable failed\n");
        }
    } else if (node->n_type == T_VAR || node->n_type) {
        // if a value is being assigned to some variable, we must check that
        // it has already been declared
        if (node->n_type == T_VAR)
            e.key = node->data.var.id;
        else
            e.key = node->data.constant.id;
        ep = hsearch(e, FIND);

        // if the element cannot be found, ep will be NULL (this means that
        // the assignment is illegal)
        if (ep == NULL)
            return false;
    }
    return true;
}

int main(void) {
    // create ast root
    ast_root = create_node_seq();

    // generate the AST
    yyparse();

    // perform semantic check
    bool is_semantically_valid = semantic_check(ast_root);

    if (is_semantically_valid)
        printf("Code is semantically valid\n");
    else
        fprintf(stderr, "Code is not semantically valid\n");
    return 0;
}
