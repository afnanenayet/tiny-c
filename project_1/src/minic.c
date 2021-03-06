/**
 * \file minicc.c
 * \author Afnan Enayet
 *
 * Main entry point for executable
 */

#include <search.h>
#include <stdio.h>
#include <string.h>

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

/**
 * \brief Recusively check whether the AST subtree is semantically valid.
 *
 * This function is a helper function for the `semantic_check` wrapper method.
 * It recursively checks some given node to ensure that all of the node's
 * subtrees are semantically valid.
 *
 * It checks for the following criteria:
 *     - variables are declared once
 *     - variables are assigned once
 *     - variables are declared before assignment
 *
 * \param[in] node The root node of the subtree you want to verify
 * \return Whether the given subtree is semantically valid
 */
static bool semantic_check_helper(const ast_node_t *node);

// ***************************************************************************
// Private function definitions
// ***************************************************************************

static bool semantic_check(const ast_node_t *root) {
    // create the hashtable (search.h), bail if there's an error
    if (hcreate(256) == 0) {
        fprintf(stderr, "Failed to create hashtable\n");
        return false;
    }

    bool is_valid = semantic_check_helper(root);
    hdestroy(); // delete the hashtable
    return is_valid;
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
        // need to duplicate the string because hcreate will free every key
        // in the hash table, which will cause major issues with the AST
        e.key = strdup(node->data.declaration.id);

        // first check if the variable has been declared before
        ep = hsearch(e, FIND);

        if (ep != NULL) {
            fprintf(stderr,
                    "(semantic error) variable or constant `%s` declared "
                    "multiple times\n",
                    e.key);
            return false;
        }
        ep = hsearch(e, ENTER);

        // ep is null on errors
        if (ep == NULL) {
            fprintf(stderr, "Entry to hashtable failed\n");
            return false;
        }
    } else if (node->n_type == T_VAR || node->n_type == T_CONST) {
        // if a value is being assigned to some variable, we must check that
        // it has already been declared
        if (node->n_type == T_VAR)
            e.key = node->data.var.id;
        else
            e.key = node->data.constant.id;
        ep = hsearch(e, FIND);

        // if the element cannot be found, ep will be NULL (this means that
        // the assignment is illegal)
        if (ep == NULL) {
            fprintf(stderr,
                    "(semantic error) variable or constant `%s` was assigned "
                    "before declaration\n",
                    e.key);
            return false;
        } else if (ep->data != NULL) {
            fprintf(stderr,
                    "(semantic error) multiple definitions of variable or "
                    "constant `%s`\n",
                    e.key);
            return false;
        }

        // insert the element back into the hash table, pointing to the
        // assignment node, so that we can ensure variables aren't defined
        // multiple times
        ep->data = (void *)node;

        // ep is only null if there is an error with the hashtable
        if (ep == NULL) {
            fprintf(stderr, "Entry to hashtable failed\n");
            return false;
        }
    }
    return true;
}

int main(void) {
    // create ast root
    ast_root = create_node_seq();

    // generate the AST
    int parse_status = yyparse();

    if (parse_status != 0) {
        return 1;
    }

    // perform semantic check
    bool is_semantically_valid = semantic_check(ast_root);

    // not printing anything for errors because errors are already printed
    if (is_semantically_valid)
        printf("\nCode is semantically valid\n");
    delete_tree(ast_root);

    if (is_semantically_valid)
        return 0;
    return 1;
}
