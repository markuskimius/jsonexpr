#ifndef AST_H_
#define AST_H_

#include <inttypes.h>


/* ***************************************************************************
* CONSTANTS
*/

enum {
    NULL_N = 256,
    INT64_N,
    DOUBLE_N,
    STRING_N,
    ARRAY_N,
    OBJECT_N,
    LVALUE_N,
    FNCALL_N,
    IDENT_N,
    UPLUS_N,
    UMINUS_N,
};


/* ***************************************************************************
* TYPES
*/

typedef struct AST_NODE {
    int type;
    struct AST_NODE* left;
    struct AST_NODE* right;
    union {
        int64_t i64;
        double f64;
        char* str;
        char* name;
    } value;
} AST_NODE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

AST_NODE* newnode(int type, AST_NODE* left, AST_NODE* right);
AST_NODE* newleaf(int type, void* value);
AST_NODE* parse(const char* code);


#endif /* AST_H_ */
