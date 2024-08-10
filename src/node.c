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


NODE* newleaf(int type, TOKEN* token) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->token = token;

    return node;
}
