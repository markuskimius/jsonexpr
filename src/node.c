#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NAMEMAX 16


/* ***************************************************************************
* GLOBALS
*/

static char NODENAMEL[NULL_N][NAMEMAX];

static char* NODENAMEH[] = {
    "NULL_N",
    "BOOL_N",
    "INT_N",
    "FLOAT_N",
    "STRING_N",
    "ARRAY_N",
    "OBJECT_N",
    "SYMBOL_N",
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
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right, NODE* righter, YYLTYPE* loc) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->left = left;
    node->right = right;
    node->righter = righter;
    node->loc = *loc;

    return node;
}


NODE* newinode(int type, int64_t i, YYLTYPE* loc) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->value.i = i;
    node->loc = *loc;

    return node;
}


NODE* newfnode(int type, double f, YYLTYPE* loc) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->value.f = f;
    node->loc = *loc;

    return node;
}


NODE* newsnode(int type, char* s, YYLTYPE* loc) {
    NODE* node = calloc(1, sizeof(NODE));

    node->type = type;
    node->value.s = s;
    node->loc = *loc;

    return node;
}


char* nodetree(NODE* node) {
    char* left = NULL;
    char* right = NULL;
    char* value = NULL;
    char* tree = NULL;
    char* cp;
    char* np;

    /* Get the values */
    if(node->left) left = nodetree(node->left);
    if(node->right) right = nodetree(node->right);
    if(!left && !right) value = textat(&node->loc);

    /* This node */
    if(value) asprintf(&tree, "%s(%s) at %p\n", nodetype(node), value, node);
    else      asprintf(&tree, "%s at %p\n", nodetype(node), node);

    /* Children */
    if(left) {
        for(cp=left, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = casprintf(tree,
                "%s %s\n",
                (cp == left ? "+-" : right ? "| " : "  "), cp
            );
        }
    }

    if(right) {
        for(cp=right, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = casprintf(tree,
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


const char* nodetype(NODE* node) {
    int type = node->type;
    char* name = NULL;

    if(isprint(type)) {
        snprintf(NODENAMEL[type], NAMEMAX, "%c", type);
        name = NODENAMEL[type];
    }
    else if(NULL_N <= type && type < MAX_N) {
        name = NODENAMEH[type-NULL_N];
    }
    else {
        asprintf(&name, "(%x)", type);   /* FIXME - Memory leak */
    }

    return name;
}
