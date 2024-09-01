#ifndef FUNC_H_
#define FUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VEC VEC;
typedef struct NODE NODE;
typedef struct VAL VAL;
typedef struct SYMTBL SYMTBL;


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

typedef VAL* (*BINARY_FN)(VEC* args, SYMTBL* table);

typedef struct FUNC {
    func_t type;
    union {
        BINARY_FN bin;
        NODE* cust;
    } handler;

    char* name;
    char* sig;
    SYMTBL* ctx;
    size_t count;
} FUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(BINARY_FN handler, const char* name, const char* spec);
FUNC* newcustfunc(NODE* handler, const char* name, const char* sig, SYMTBL* ctx);
FUNC* dupfunc(FUNC* func);
void freefunc(FUNC* func);

VEC* funcargs(const char* sig, VEC* nodes, SYMTBL* table);
VAL* funcexec(FUNC* func, VEC* nodes, SYMTBL* table);
char* funcastr(FUNC* func);


#endif /* FUNC_H_ */
