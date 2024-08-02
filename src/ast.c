#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

AST_NODE* newnode(int type, AST_NODE* left, AST_NODE* right) {
    AST_NODE* node = calloc(1, sizeof(AST_NODE));

    node->type = type;
    node->left = left;
    node->right = right;

    return node;
}


AST_NODE* newleaf(int type, void* value) {
    AST_NODE* node = calloc(1, sizeof(AST_NODE));

    node->type = type;
    node->left = NULL;
    node->right = NULL;

    switch(type) {
        case NULL_N   : node->value.i64 = 0; break;
        case INT64_N  : node->value.i64 = *((int64_t*) value); break;
        case DOUBLE_N : node->value.f64 = *((double*) value); break;
        case STRING_N : node->value.str = (char*) value; break;
        case IDENT_N  : node->value.name = (char*) value; break;
        default       : fprintf(stderr, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, type, type); break;
    }

    return node;
}


/* parse() is defined in jsonexpr.l */
