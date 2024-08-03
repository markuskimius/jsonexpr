#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->left = left;
    node->right = right;

    return node;
}


NODE* newleaf(int type, void* value) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->left = NULL;
    node->right = NULL;

    switch(type) {
        case NULL_N   : node->value.i = 0; break;
        case BOOL_N   : node->value.i = *((int64_t*) value); break;
        case INT_N    : node->value.i = *((int64_t*) value); break;
        case FLOAT_N  : node->value.f = *((double*) value); break;
        case STRING_N : node->value.s = (char*) value; break;
        case IDENT_N  : node->value.s = (char*) value; break;
        default       : fprintf(stderr, "%s: Invalid node type: '%c' (%d)\n", __FUNCTION__, type, type); break;
    }

    return node;
}
