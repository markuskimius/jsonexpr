#ifndef NODE_H_
#define NODE_H_

#include <inttypes.h>


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
};


/* ***************************************************************************
* TYPES
*/

typedef struct NODE {
    int type;
    struct NODE* left;
    struct NODE* right;
    union {
        int64_t i;
        double f;
        char* s;
    } value;
} NODE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* newnode(int type, NODE* left, NODE* right);
NODE* newleaf(int type, void* value);


#endif /* NODE_H_ */
