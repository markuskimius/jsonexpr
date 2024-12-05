#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_nav.h"
#include "je_node.h"
#include "je_util.h"
#include "je_token.h"


/* ***************************************************************************
* TYPES
*/

typedef struct JE_NAVNODE {
    struct JE_NAVNODE* enter;
    struct JE_NAVNODE* exit;
    struct JE_NAVNODE* prev;
    struct JE_NAVNODE* next;
    struct JE_NODE* node;

    char* textat;
    char* textafter;
    char* textbefore;
} JE_NAVNODE;


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

static JE_NAVNODE* je_newnavnode(JE_NODE* node);
static JE_NAVNODE* je_newstmtnavnode(JE_NODE* node);
static JE_NAVNODE* je_newarrnavnode(JE_NODE* node);
static JE_NAVNODE* je_newobjnavnode(JE_NODE* node);
static JE_NAVNODE* je_newcallnavnode(JE_NODE* node);
static JE_NAVNODE* je_neweqnavnode(JE_NODE* node);

static JE_NAVNODE* je_newarritemnavnode(JE_NODE* node);
static JE_NAVNODE* je_newobjitemnavnode(JE_NODE* node);
static JE_NAVNODE* je_newcallargnavnode(JE_NODE* node);
static JE_NAVNODE* je_newcallargitemnavnode(JE_NODE* node);

static void je_linknext(JE_NAVNODE* node, JE_NAVNODE* next);
static void je_linkenter(JE_NAVNODE* node, JE_NAVNODE* enter);

static JE_NAVNODE* je_newnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = NULL;

    switch(node->type) {
        case ';'         : navnode = je_newstmtnavnode(node); break;
        case JE_ARRAY_N  : navnode = je_newarrnavnode(node); break;
        case JE_OBJECT_N : navnode = je_newobjnavnode(node); break;
        case JE_CALL_N   : navnode = je_newcallnavnode(node); break;
        default          : navnode = je_neweqnavnode(node); break;
    }

    return navnode;
}

static JE_NAVNODE* je_newstmtnavnode(JE_NODE* node) {
    JE_NAVNODE* first = NULL;
    JE_NAVNODE* last = NULL;

    if(node) {
        switch(node->type) {
            case ';' :
                first = last = je_newnavnode(node->left);

                while(last->next) last = last->next;
                if(node->right) je_linknext(last, je_newnavnode(node->right));

                break;
        }
    }

    return first;
}

static JE_NAVNODE* je_newarrnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = calloc(1, sizeof(JE_NAVNODE));

    navnode->node = node;
    je_linkenter(navnode, je_newarritemnavnode(node->left));

    return navnode;
}

static JE_NAVNODE* je_newobjnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = calloc(1, sizeof(JE_NAVNODE));

    navnode->node = node;
    je_linkenter(navnode, je_newobjitemnavnode(node->left));

    return navnode;
}

static JE_NAVNODE* je_newcallnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = calloc(1, sizeof(JE_NAVNODE));

    navnode->node = node;
    if(node->left) je_linkenter(navnode, je_newnavnode(node->left));
    if(node->right) je_linknext(navnode->enter, je_newcallargnavnode(node->right));

    return navnode;
}

static JE_NAVNODE* je_neweqnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = calloc(1, sizeof(JE_NAVNODE));

    navnode->node = node;
    if(node->left) je_linkenter(navnode, je_newnavnode(node->left));
    if(node->right) je_linknext(navnode->enter, je_newnavnode(node->right));
    if(node->righter) je_linknext(navnode->enter->next, je_newnavnode(node->righter));

    return navnode;
}

static JE_NAVNODE* je_newarritemnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = NULL;

    if(node) {
        switch(node->type) {
            case ',' :
                navnode = je_newnavnode(node->left);
                if(node->right) je_linknext(navnode, je_newarritemnavnode(node->right));
                break;
        }
    }

    return navnode;
}

static JE_NAVNODE* je_newobjitemnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = NULL;

    if(node) {
        switch(node->type) {
            case ',' :
                navnode = calloc(1, sizeof(JE_NAVNODE));
                navnode->node = node->left;
                if(node->right) je_linknext(navnode, je_newobjitemnavnode(node->right));
                je_linkenter(navnode, je_newnavnode(node->left->left));
                je_linknext(navnode->enter, je_newnavnode(node->left->right));
                break;
        }
    }

    return navnode;
}

static JE_NAVNODE* je_newcallargnavnode(JE_NODE* node) {
    JE_NAVNODE* first = NULL;
    JE_NAVNODE* last = NULL;

    if(node) {
        switch(node->type) {
            case ',' :
                first = last = je_newcallargitemnavnode(node->left);
                while(last->next) last = last->next;
                if(node->right) je_linknext(last, je_newcallargnavnode(node->right));
                break;
        }
    }

    return first;
}

static JE_NAVNODE* je_newcallargitemnavnode(JE_NODE* node) {
    JE_NAVNODE* navnode = NULL;

    if(node) {
        switch(node->type) {
            case ';' :
                navnode = calloc(1, sizeof(JE_NAVNODE));
                navnode->node = node;
                je_linkenter(navnode, je_newnavnode(node));
                break;

            default  :
                navnode = je_newnavnode(node);
        }
    }

    return navnode;
}

static void je_linknext(JE_NAVNODE* node, JE_NAVNODE* next) {
    if(node) node->next = next;
    if(next) next->prev = node;
    if(next && node) next->exit = node->exit;
}

static void je_linkenter(JE_NAVNODE* node, JE_NAVNODE* enter) {
    if(node) node->enter = enter;
    if(enter) enter->exit = node;

    if(enter && node) {
        JE_NAVNODE* pi = enter;
        JE_NAVNODE* ni = enter;

        while(pi->prev) {
            pi = pi->prev;
            pi->exit = node;
        }

        while(ni->next)  {
            ni = ni->next;
            ni->exit = node;
        }
    }
}

static void je_freenavnode(JE_NAVNODE* navnode, int recurse) {
    if(recurse && navnode->enter) je_freenavnode(navnode->enter, recurse);
    if(recurse && navnode->next) je_freenavnode(navnode->next, recurse);
    if(navnode->textat) free(navnode->textat);
    if(navnode->textafter) free(navnode->textafter);
    if(navnode->textbefore) free(navnode->textbefore);

    navnode->enter = NULL;
    navnode->exit = NULL;
    navnode->prev = NULL;
    navnode->next = NULL;
    navnode->node = NULL;
    navnode->textat = NULL;
    navnode->textafter = NULL;
    navnode->textbefore = NULL;

    free(navnode);
}

static const char* je_navnodetextat(JE_NAVNODE* navnode) {
    JE_TOKEN* first = navnode->node->loc.first;
    JE_TOKEN* last = navnode->node->loc.last;

    if(navnode->textat) free(navnode->textat);

    navnode->textat = je_astrtoken(first, last);

    return navnode->textat;
}

static const char* je_navnodetextafter(JE_NAVNODE* navnode) {
    JE_TOKEN* first = navnode->node->loc.last->next;
    JE_TOKEN* last = first;

    /* Reset */
    if(navnode->textafter) {
        free(navnode->textafter);
        navnode->textafter = NULL;
    }

    /* Only include decorative tokens */
    if(first && (first->flag & JE_DECOR_TF)) {
        while(last->next && (last->next->flag & JE_DECOR_TF)) last = last->next;
        navnode->textafter = je_astrtoken(first, last);
    }

    return navnode->textafter ? navnode->textafter : "";
}

static const char* je_navnodetextbefore(JE_NAVNODE* navnode) {
    JE_TOKEN* last = navnode->node->loc.first->prev;
    JE_TOKEN* first = last;

    /* Reset */
    if(navnode->textbefore) {
        free(navnode->textbefore);
        navnode->textbefore = NULL;
    }

    /* Only include decorative tokens */
    if(last && (last->flag & JE_DECOR_TF)) {
        while(first->prev && (first->prev->flag & JE_DECOR_TF)) first = first->prev;
        navnode->textbefore = je_astrtoken(first, last);
    }

    return navnode->textbefore ? navnode->textbefore : "";
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NAV* je_newnav(JE_NODE* node) {
    JE_NAV* nav = calloc(1, sizeof(JE_NAV));

    nav->root = je_newnavnode(node);
    nav->curr = nav->root;

    return nav;
}


void je_freenav(JE_NAV* nav) {
    je_freenavnode(nav->root, 1);

    nav->root = NULL;
    nav->curr = NULL;

    free(nav);
}


int je_naventer(JE_NAV* nav) {
    int isok = 0;

    if(nav->curr->enter) {
        nav->curr = nav->curr->enter;
        isok = 1;
    }

    return isok;
}


int je_navexit(JE_NAV* nav) {
    int isok = 0;

    if(nav->curr->exit) {
        nav->curr = nav->curr->exit;
        isok = 1;
    }

    return isok;
}


int je_navprev(JE_NAV* nav) {
    int isok = 0;

    if(nav->curr->prev) {
        nav->curr = nav->curr->prev;
        isok = 1;
    }

    return isok;
}


int je_navnext(JE_NAV* nav) {
    int isok = 0;

    if(nav->curr->next) {
        nav->curr = nav->curr->next;
        isok = 1;
    }

    return isok;
}


const char* je_navtype(JE_NAV* nav) {
    return je_nodetype(nav->curr->node);
}


JE_NODE* je_navnode(JE_NAV* nav) {
    return nav->curr->node;
}


JE_NODE* je_navkeynode(JE_NAV* nav) {
    JE_NODE* node = NULL;

    if(nav->curr->node->type == ':') {
        node = nav->curr->node->left;
    }

    return node;
}


JE_NODE* je_navvaluenode(JE_NAV* nav) {
    JE_NODE* node = NULL;

    if(nav->curr->node->type == ':') {
        node = nav->curr->node->right;
    }

    return node;
}


const char* je_navtextat(JE_NAV* nav) {
    return je_navnodetextat(nav->curr);
}


const char* je_navtextafter(JE_NAV* nav) {
    return je_navnodetextafter(nav->curr);
}


const char* je_navtextbefore(JE_NAV* nav) {
    return je_navnodetextbefore(nav->curr);
}


char* je_navnodetree(JE_NAVNODE* navnode) {
    char* value = NULL;
    char* enter = NULL;
    char* next = NULL;
    char* tree = NULL;
    char* cp;
    char* np;

    /* Get the values */
    if(navnode->enter) enter = je_navnodetree(navnode->enter);
    if(navnode->next) next = je_navnodetree(navnode->next);
    // if(!enter && !next) value = je_atextat(&navnode->loc);
    value = je_atextat(&navnode->node->loc);

    /* This navnode */
    asprintf(&tree, "{%s}{%s}{%s}\n", je_nodetype(navnode->node), je_navnodetextbefore(navnode), je_navnodetextafter(navnode));

    if(value) {
        value = je_astrcat(value, "\n");

        for(cp=value, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = je_casprintf(tree,
                "%s%s\n",
                enter ? " | " : "", cp
            );
        }
    }

    if(enter) {
        for(cp=enter, np=strchr(cp,'\n'); np; cp=np+1, np=strchr(cp,'\n')) {
            *np = '\0';

            tree = je_casprintf(tree,
                "%s%s\n",
                (cp == enter ? " +-" : "   "), cp
            );
        }
    }

    if(next) {
        char* tmp1 = tree;
        char* tmp2 = je_amcat(tmp1, " -> ");

        tree = je_amcat(tmp2, next);

        free(tmp1);
        free(tmp2);
    }

    /* Cleanup */
    if(next) free(next);
    if(enter) free(enter);
    if(value) free(value);

    return tree;
}
