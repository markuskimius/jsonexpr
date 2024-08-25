#ifndef FUNC_H_
#define FUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VEC VEC;
typedef struct NODE NODE;
typedef struct VALUE VALUE;
typedef struct SYM_TABLE SYM_TABLE;


/* ***************************************************************************
* CONSTANTS
*/

#define MAXARGS ((size_t) -1)

typedef enum {
    BINARY_FT,
    CUSTOM_FT,
} func_t;


/* ***************************************************************************
* TYPES
*/

typedef VALUE* (*BINARY_FN)(VEC* args, SYM_TABLE* table);

typedef struct FUNC {
    func_t type;
    union {
        BINARY_FN bin;
        NODE* cust;
    } handler;

    char* name;
    char* sig;
    SYM_TABLE* ctx;
    size_t count;
} FUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(BINARY_FN handler, const char* name, const char* spec);
FUNC* newcustfunc(NODE* handler, const char* name, const char* sig, SYM_TABLE* ctx);
FUNC* dupfunc(FUNC* func);
void freefunc(FUNC* func);
char* astrfunc(FUNC* func);
VEC* funcargs(const char* sig, VEC* nodes, SYM_TABLE* table);
VALUE* funcexec(FUNC* func, VEC* nodes, SYM_TABLE* table);


#endif /* FUNC_H_ */
