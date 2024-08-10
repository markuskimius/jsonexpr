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
    YYLTYPE* first = start ? start : left->loc.first_pos <= right->loc.first_pos ? &left->loc : &right->loc;
    YYLTYPE* last = end ? end : left->loc.last_pos >= right->loc.last_pos ? &left->loc : &right->loc;

    node->type = type;
    node->left = left;
    node->right = right;
    node->loc = left->loc;
    node->loc.first_pos = first->first_pos;
    node->loc.first_line = first->first_line;
    node->loc.first_column = first->first_column;
    node->loc.last_pos = last->last_pos;
    node->loc.last_line = last->last_line;
    node->loc.last_column = last->last_column;

    return node;
}


NODE* newleaf(int type, TOKEN* token) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->token = token;
    node->loc = token->loc;

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
