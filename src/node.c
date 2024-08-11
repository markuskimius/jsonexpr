#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "node.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right, YYLTYPE* start, YYLTYPE* end) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->left = left;
    node->right = right;
    node->loc = *start;
    node->loc.last_pos = end->last_pos;
    node->loc.last_line = end->last_line;
    node->loc.last_column = end->last_column;

    return node;
}


NODE* newleaf(int type, TOKEN* token) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->token = token;
    node->loc = token ? token->loc : yylloc;

    return node;
}

const char* nodetext(NODE* node) {
    if(!node->text) {
        YYLTYPE* loc = &node->loc;
        size_t length = loc->last_pos - loc->first_pos + 1;
        char* code = *loc->code;

        node->text = calloc(1, length);
        snprintf(node->text, length, "%s", code + loc->first_pos);
    }

    return node->text;
}
