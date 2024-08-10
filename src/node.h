#ifndef NODE_H_
#define NODE_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct TOKEN TOKEN;


/* ***************************************************************************
* CONSTANTS
*/

enum {
    NULL_N = 256,
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
    EQ_N,
    NE_N,
    LT_N,
    LE_N,
    GT_N,
    GE_N,
};


/* ***************************************************************************
* TYPES
*/

typedef struct NODE {
    int type;
    struct NODE* left;
    struct NODE* right;
    struct TOKEN* token;
} NODE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right);
NODE* newleaf(int type, TOKEN* token);


#endif /* NODE_H_ */
