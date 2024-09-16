#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_node.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define NAMEMAX 16


/* ***************************************************************************
* GLOBALS
*/

static char NODENAMEL[JE_NULL_N][NAMEMAX];

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

JE_NODE* je_newnode(int type, JE_NODE* left, JE_NODE* right, JE_NODE* righter, JE_YYLTYPE* loc) {
    JE_NODE* node = calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->left = left;
    node->right = right;
    node->righter = righter;
    node->loc = *loc;

    return node;
}


JE_NODE* je_newinode(int type, int64_t i, JE_YYLTYPE* loc) {
    JE_NODE* node = calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.i = i;
    node->loc = *loc;

    return node;
}


JE_NODE* je_newfnode(int type, double f, JE_YYLTYPE* loc) {
    JE_NODE* node = calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.f = f;
    node->loc = *loc;

    return node;
}


JE_NODE* je_newsnode(int type, char* s, JE_YYLTYPE* loc) {
    JE_NODE* node = calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.s = s;
    node->loc = *loc;

    return node;
}


void je_freenode(JE_NODE* node) {
    if(node->left) je_freenode(node->left);
    if(node->right) je_freenode(node->right);
    if(node->righter) je_freenode(node->righter);

    if(node->type == JE_IDENT_N || node->type == JE_STRING_N) {
        free(node->value.s);
    }

    node->left = NULL;
    node->right = NULL;
    node->righter = NULL;
    node->value.s = NULL;

    free(node);
}


char* je_nodetree(JE_NODE* node) {
    char* left = NULL;
    char* right = NULL;
    char* value = NULL;
    char* tree = NULL;
    char* cp;
    char* np;

    /* Get the values */
    if(node->left) left = je_nodetree(node->left);
    if(node->right) right = je_nodetree(node->right);
    if(!left && !right) value = je_textat(&node->loc);

    /* This node */
    if(value) asprintf(&tree, "%s(%s) at %p\n", je_nodetype(node), value, node);
    else      asprintf(&tree, "%s at %p\n", je_nodetype(node), node);

    /* Children */
    if(left) {
        for(cp=left, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = je_casprintf(tree,
                "%s %s\n",
                (cp == left ? "+-" : right ? "| " : "  "), cp
            );
        }
    }

    if(right) {
        for(cp=right, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = je_casprintf(tree,
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


const char* je_nodetype(JE_NODE* node) {
    int type = node->type;
    char* name = NULL;

    if(isprint(type)) {
        snprintf(NODENAMEL[type], NAMEMAX, "%c", type);
        name = NODENAMEL[type];
    }
    else if(JE_NULL_N <= type && type < JE_MAX_N) {
        name = NODENAMEH[type-JE_NULL_N];
    }
    else {
        asprintf(&name, "(%x)", type);   /* FIXME - Memory leak */
    }

    return name;
}
