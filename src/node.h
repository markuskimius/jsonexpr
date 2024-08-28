#ifndef NODE_H_
#define NODE_H_

#include <inttypes.h>
#include "parse.h"


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct YYLTYPE YYLTYPE;


/* ***************************************************************************
* CONSTANTS
*/

enum {
    NULL_N = 0x100,
    BOOL_N,
    INT_N,
    FLOAT_N,
    STRING_N,
    ARRAY_N,
    OBJECT_N,
    SYMBOL_N,
    CALL_N,
    IDENT_N,
    UPLUS_N,
    UMINUS_N,
    POW_N,
    EQ_N,
    NE_N,
    LT_N,
    LE_N,
    GT_N,
    GE_N,
    PREINC_N,
    POSTINC_N,
    PREDEC_N,
    POSTDEC_N,
    MAX_N
};


/* ***************************************************************************
* TYPES
*/

typedef struct NODE {
    int type;
    struct NODE* left;
    struct NODE* right;
    struct NODE* righter;

    union {
        int64_t i;
        double f;
        char* s;
    } value;
    struct YYLTYPE loc;
} NODE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right, NODE* righter, YYLTYPE* loc);
NODE* newinode(int type, int64_t i, YYLTYPE* loc);
NODE* newfnode(int type, double f, YYLTYPE* loc);
NODE* newsnode(int type, char* s, YYLTYPE* loc);
char* nodetree(NODE* node);
const char* nodetype(NODE* node);


#endif /* NODE_H_ */
