#ifndef JE_NODE_H_
#define JE_NODE_H_

#include <inttypes.h>
#include "je_parse.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VAL JE_VAL;
typedef struct JE_TOKEN JE_TOKEN;
typedef struct JE_YYLTYPE JE_YYLTYPE;


/* ***************************************************************************
* CONSTANTS
*/

enum {
    JE_NULL_N = 0x100,
    JE_BOOL_N,
    JE_INT_N,
    JE_FLOAT_N,
    JE_STRING_N,
    JE_ARRAY_N,
    JE_OBJECT_N,
    JE_CALL_N,
    JE_IDENT_N,
    JE_UPLUS_N,
    JE_UMINUS_N,
    JE_POW_N,
    JE_EQ_N,
    JE_NE_N,
    JE_LT_N,
    JE_LE_N,
    JE_GT_N,
    JE_GE_N,
    JE_OR_N,
    JE_AND_N,
    JE_SHL_N,
    JE_ASR_N,
    JE_SHR_N,
    JE_PREINC_N,
    JE_POSTINC_N,
    JE_PREDEC_N,
    JE_POSTDEC_N,
    JE_PLEQ_N,
    JE_MIEQ_N,
    JE_TIEQ_N,
    JE_DIEQ_N,
    JE_MOEQ_N,
    JE_SHLEQ_N,
    JE_ASREQ_N,
    JE_SHREQ_N,
    JE_ANDEQ_N,
    JE_XOREQ_N,
    JE_OREQ_N,
    JE_POWEQ_N,
    JE_MAX_N
};


/* ***************************************************************************
* TYPES
*/

typedef struct JE_NODE {
    int type;
    struct JE_NODE* left;
    struct JE_NODE* right;
    struct JE_NODE* righter;
    struct JE_NODE* parent;

    union {
        int64_t i;
        double f;
        char* s;
    } value;

    JE_YYLTYPE loc;
    JE_TOKEN* head;
} JE_NODE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE* je_newnode(int type, JE_NODE* left, JE_NODE* right, JE_NODE* righter, JE_YYLTYPE* loc);
JE_NODE* je_newinode(int type, int64_t i, JE_YYLTYPE* loc);
JE_NODE* je_newfnode(int type, double f, JE_YYLTYPE* loc);
JE_NODE* je_newsnode(int type, char* s, JE_YYLTYPE* loc);
void je_freenode(JE_NODE* node);

JE_NODE* je_nodedetach(JE_NODE* node);
JE_NODE* je_nodeattachto(JE_NODE* dest, JE_NODE* src, int where);

void je_noderemove(JE_NODE* node, int free);
JE_NODE* je_nodereplace(JE_NODE* node, JE_NODE* newnode, int free);

char* je_nodetree(JE_NODE* node);
char* je_nodeastr(JE_NODE* node);
const char* je_nodetype(JE_NODE* node);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_NODE_H_ */
