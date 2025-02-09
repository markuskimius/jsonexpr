#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "je_node.h"
#include "je_util.h"
#include "je_token.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _NAMEMAX 16


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static char _NODENAMEL[JE_NULL_N][_NAMEMAX];

static char* _NODENAMEH[] = {
    "NULL_N",
    "BOOL_N",
    "INT_N",
    "FLOAT_N",
    "STRING_N",
    "ARRAY_N",
    "OBJECT_N",
    "CALL_N",
    "IDENT_N",
    "UPLUS_N",
    "UMINUS_N",
    "POW_N",
    "EQ_N",
    "NE_N",
    "LT_N",
    "LE_N",
    "GT_N",
    "GE_N",
    "OR_N",
    "AND_N",
    "SHL_N",
    "ASR_N",
    "SHR_N",
    "PREINC_N",
    "POSTINC_N",
    "PREDEC_N",
    "POSTDEC_N",
    "PLEQ_N",
    "MIEQ_N",
    "TIEQ_N",
    "DIEQ_N",
    "MOEQ_N",
    "SHLEQ_N",
    "ASREQ_N",
    "SHREQ_N",
    "ANDEQ_N",
    "XOREQ_N",
    "OREQ_N",
    "POWEQ_N",
    NULL
};


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_TOKEN* _blanktoken() {
    JE_TOKEN* token = JE_TokenNew(0, 0, 0, NULL);

    token->text = JE_Calloc(1, sizeof(int64_t));

    return token;
}

static JE_YYLTYPE _blankloc() {
    JE_YYLTYPE loc;

    loc.first = _blanktoken();
    loc.last = loc.first;

    return loc;
}

static void _nodeswap(JE_NODE* node1, JE_NODE* node2) {
    JE_NODE tmp;

    memcpy(&tmp, node1, sizeof(JE_NODE));
    memcpy(node1, node2, sizeof(JE_NODE));
    memcpy(node2, &tmp, sizeof(JE_NODE));
}

static void _nodereloc(JE_NODE* node, JE_TOKEN* head) {
    JE_TOKEN* first = node->loc.first;
    JE_TOKEN* last = node->loc.last;
    JE_TOKEN* first_child = NULL;
    JE_TOKEN* last_child = NULL;

    /* Find first child with a first location */
    if     (node->left && node->left->loc.first) first_child = node->left->loc.first;
    else if(node->right && node->right->loc.first) first_child = node->right->loc.first;
    else if(node->righter && node->righter->loc.first) first_child = node->righter->loc.first;

    /* Find last child with a last location */
    if     (node->righter && node->righter->loc.last) last_child = node->righter->loc.last;
    else if(node->right && node->right->loc.last) last_child = node->right->loc.last;
    else if(node->left && node->left->loc.last) last_child = node->left->loc.last;

    /* Use first child's first location? */
    if(JE_TokenFindHead(first) != head) first = first_child;
    else if(first_child && first_child->first_pos < first->first_pos) first = first_child;

    /* Use last child's last location? */
    if(JE_TokenFindHead(last) != head) last = last_child;
    else if(last_child && last_child->last_pos < last->last_pos) last = last_child;

    node->loc.first = first;
    node->loc.last = last;

    if(node->parent) _nodereloc(node->parent, head);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE* JE_NodeNew(int type, JE_NODE* left, JE_NODE* right, JE_NODE* righter, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->left = left;
    node->right = right;
    node->righter = righter;
    node->loc = loc ? *loc : _blankloc();
    node->head = loc ? NULL : node->loc.first;

    if(left) left->parent = node;
    if(right) right->parent = node;
    if(righter) righter->parent = node;

    return node;
}

JE_NODE* JE_NodeNewInt(int type, int64_t i, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.i = i;
    node->loc = loc ? *loc : _blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}

JE_NODE* JE_NodeNewFloat(int type, double f, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.f = f;
    node->loc = loc ? *loc : _blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}

JE_NODE* JE_NodeNewStr(int type, char* s, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.s = s;
    node->loc = loc ? *loc : _blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}

void JE_NodeDelete(JE_NODE* node) {
    if(node->left) JE_NodeDelete(node->left);
    if(node->right) JE_NodeDelete(node->right);
    if(node->righter) JE_NodeDelete(node->righter);

    if(node->head) JE_TokenDelete(node->head, 1);    /* Free the tokens if this node is the token owner (root node) */

    if(node->type == JE_IDENT_N || node->type == JE_STRING_N) {
        free(node->value.s);
    }

    node->left = NULL;
    node->right = NULL;
    node->righter = NULL;
    node->parent = NULL;
    node->value.s = NULL;
    node->head = NULL;

    free(node);
}

char* JE_NodeTreeAstr(JE_NODE* node) {
    char* left = NULL;
    char* right = NULL;
    char* value = NULL;
    char* tree = NULL;
    char* cp;
    char* np;

    /* Get the values */
    if(node->left) left = JE_NodeTreeAstr(node->left);
    if(node->right) right = JE_NodeTreeAstr(node->right);
    // if(!left && !right) value = JE_LocToAstr(&node->loc);
    value = JE_LocToAstr(&node->loc);

    /* This node */
    asprintf(&tree, "%s at %p w/ p=%p (%zd,%zd,%zd)-(%zd,%zd,%zd)\n", JE_NodeTypeCstr(node), node, node->parent,
        node->loc.first->first_pos,
        node->loc.first->first_line,
        node->loc.first->first_column,
        node->loc.last->last_pos,
        node->loc.last->last_line,
        node->loc.last->last_column
    );

    if(value) {
        value = JE_AstrCat(value, "\n");

        for(cp=value, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = JE_AstrCatFormat(tree,
                "%s%s\n",
                (left || right) ? "| " : "", cp
            );
        }
    }

    /* Children */
    if(left) {
        for(cp=left, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = JE_AstrCatFormat(tree,
                "%s %s\n",
                (cp == left ? "+-" : right ? "| " : "  "), cp
            );
        }
    }

    if(right) {
        for(cp=right, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = JE_AstrCatFormat(tree,
                "%s %s\n",
                (cp == right ? "+-" : "  "), cp
            );
        }
    }

    /* Cleanup */
    if(left) free(left);
    if(right) free(right);
    if(value) free(value);

    return tree;
}

char* JE_NodeAstr(JE_NODE* node) {
    return JE_LocToAstr(&node->loc);
}

const char* JE_NodeTypeCstr(JE_NODE* node) {
    int type = node->type;
    char* name = NULL;

    if(isprint(type)) {
        snprintf(_NODENAMEL[type], _NAMEMAX, "%c", type);
        name = _NODENAMEL[type];
    }
    else if(JE_NULL_N <= type && type < JE_MAX_N) {
        name = _NODENAMEH[type-JE_NULL_N];
    }
    else {
        asprintf(&name, "(%x)", type);   /* FIXME - Memory leak */
    }

    return name;
}
