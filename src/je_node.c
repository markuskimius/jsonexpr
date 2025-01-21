#define _GNU_SOURCE
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "je_node.h"
#include "je_util.h"
#include "je_token.h"


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

static JE_TOKEN* je_blanktoken() {
    JE_TOKEN* token = je_newtoken(0, 0, 0, NULL);

    token->text = JE_Calloc(1, sizeof(int64_t));

    return token;
}

static JE_YYLTYPE je_blankloc() {
    JE_YYLTYPE loc;

    loc.first = je_blanktoken();
    loc.last = loc.first;

    return loc;
}

static void je_nodeswap(JE_NODE* node1, JE_NODE* node2) {
    JE_NODE tmp;

    memcpy(&tmp, node1, sizeof(JE_NODE));
    memcpy(node1, node2, sizeof(JE_NODE));
    memcpy(node2, &tmp, sizeof(JE_NODE));
}

static void je_nodereloc(JE_NODE* node, JE_TOKEN* head) {
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
    if(je_tokenhead(first) != head) first = first_child;
    else if(first_child && first_child->first_pos < first->first_pos) first = first_child;

    /* Use last child's last location? */
    if(je_tokenhead(last) != head) last = last_child;
    else if(last_child && last_child->last_pos < last->last_pos) last = last_child;

    node->loc.first = first;
    node->loc.last = last;

    if(node->parent) je_nodereloc(node->parent, head);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE* je_newnode(int type, JE_NODE* left, JE_NODE* right, JE_NODE* righter, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->left = left;
    node->right = right;
    node->righter = righter;
    node->loc = loc ? *loc : je_blankloc();
    node->head = loc ? NULL : node->loc.first;

    if(left) left->parent = node;
    if(right) right->parent = node;
    if(righter) righter->parent = node;

    return node;
}


JE_NODE* je_newinode(int type, int64_t i, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.i = i;
    node->loc = loc ? *loc : je_blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}


JE_NODE* je_newfnode(int type, double f, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.f = f;
    node->loc = loc ? *loc : je_blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}


JE_NODE* je_newsnode(int type, char* s, JE_YYLTYPE* loc) {
    JE_NODE* node = JE_Calloc(1, sizeof(JE_NODE));

    node->type = type;
    node->value.s = s;
    node->loc = loc ? *loc : je_blankloc();
    node->head = loc ? NULL : node->loc.first;

    return node;
}


void je_freenode(JE_NODE* node) {
    if(node->left) je_freenode(node->left);
    if(node->right) je_freenode(node->right);
    if(node->righter) je_freenode(node->righter);

    if(node->head) je_freetoken(node->head, 1);    /* Free the tokens if this node is the token owner (root node) */

    if(node->type == JE_IDENT_N || node->type == JE_STRING_N) {
        JE_Free(node->value.s);
    }

    node->left = NULL;
    node->right = NULL;
    node->righter = NULL;
    node->parent = NULL;
    node->value.s = NULL;
    node->head = NULL;

    JE_Free(node);
}


// JE_NODE* je_nodedetach(JE_NODE* node) {
//     JE_NODE* parent = node->parent;
//     JE_TOKEN* first = node->loc.first;
//     JE_TOKEN* last = node->loc.last;
//     JE_TOKEN* head = je_tokenhead(first);
//
//     /* Detach node */
//     if(parent && parent->left == node) parent->left = NULL;
//     if(parent && parent->right == node) parent->right = NULL;
//     if(parent && parent->righter == node) parent->righter = NULL;
//     node->parent = NULL;
//
//     /* Detach tokens */
//     node->head = je_tokendetach(first, last);
//
//     /* Update locations */
//     je_nodereloc(parent, head);
//
//     return node;
// }
//
//
// JE_NODE* je_nodeattachto(JE_NODE* dest, JE_NODE* src, int where) {
//     JE_NODE* replaced = NULL;
//     JE_TOKEN* dtoken = NULL;
//
//     assert(src->parent == NULL);
//
//     /* Attach node */
//     switch(where) {
//         case 1  :
//             dtoken = dest->loc.first;
//             if(dest->left && dest->left->loc.first == dtoken) dtoken = dtoken->prev;
//             if(dest->left) replaced = je_nodedetach(dest->left);
//
//             dest->left = src;
//             je_tokenattachto(dtoken, src->loc.first);
//             dest->left->loc.first = dtoken;
//
//             break;
//
//         case 2  :
//             dtoken = dest->loc.last;
//             if(dest->right) replaced = je_nodedetach(dest->right);
//
//             dest->right = src;
//             je_tokenattachto(dtoken, src->loc.first);
//
//             break;
//
//         case 3  :
//             dtoken = dest->loc.last;
//             if(dest->righter) replaced = je_nodedetach(dest->righter);
//
//             dest->righter = src;
//             je_tokenattachto(dtoken, src->loc.first);
//
//             break;
//     }
//     src->parent = dest;
//     src->head = NULL;
//
//     /* Update locations */
//     if(replaced) je_nodereloc(dest, je_tokenhead(dest->loc.first));
//
//     return replaced;
// }
//
//
// void je_noderemove(JE_NODE* node, int free) {
//     JE_NODE* detached = je_nodedetach(node);
//
//     if(free) je_freenode(detached);
//
//     // JE_NODE* newnode = NULL;
//
//     // if(node->parent == NULL) {
//     //     newnode = je_newinode(JE_NULL_N, 0, NULL);
//
//     //     /* Create something for the caller to free */
//     //     je_nodeswap(node, newnode);
//     //     je_nodereplace(newnode, NULL, free);
//     // }
//     // else {
//     //     je_nodereplace(node, NULL, free);
//     // }
// }
//
//
// JE_NODE* je_nodereplace(JE_NODE* node, JE_NODE* newnode, int free) {
//     JE_NODE* parent = node->parent;
//
//     if(parent && parent->left == node) parent->left = newnode;
//     if(parent && parent->right == node) parent->right = newnode;
//     if(parent && parent->righter == node) parent->righter = newnode;
//
//     if(newnode && newnode->parent) {
//         if(newnode->parent->left == newnode) newnode->parent->left = NULL;
//         if(newnode->parent->right == newnode) newnode->parent->right = NULL;
//         if(newnode->parent->righter == newnode) newnode->parent->righter = NULL;
//     }
//
//     if(newnode) {
//         newnode->parent = parent;
//         newnode->head = NULL;
//     }
//
//     node->parent = NULL;
//
//     if(free) {
//         je_freenode(node);
//         node = NULL;
//     }
//
//     return node;
// }


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
    // if(!left && !right) value = je_atextat(&node->loc);
    value = je_atextat(&node->loc);

    /* This node */
    asprintf(&tree, "%s at %p w/ p=%p (%zd,%zd,%zd)-(%zd,%zd,%zd)\n", je_nodetype(node), node, node->parent,
        node->loc.first->first_pos,
        node->loc.first->first_line,
        node->loc.first->first_column,
        node->loc.last->last_pos,
        node->loc.last->last_line,
        node->loc.last->last_column
    );

    if(value) {
        value = je_astrcat(value, "\n");

        for(cp=value, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = je_casprintf(tree,
                "%s%s\n",
                (left || right) ? "| " : "", cp
            );
        }
    }

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
    if(left) JE_Free(left);
    if(right) JE_Free(right);
    if(value) JE_Free(value);

    return tree;
}


char* je_nodeastr(JE_NODE* node) {
    return je_atextat(&node->loc);
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
