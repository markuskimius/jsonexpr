#ifndef FUNC_H_
#define FUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VEC VEC;
typedef struct VALUE VALUE;
typedef struct SYM_TABLE SYM_TABLE;


/* ***************************************************************************
* CONSTANTS
*/

#ifndef MAXARGS
#define MAXARGS ((size_t) -1)
#endif


/* ***************************************************************************
* TYPES
*/

typedef VALUE* (*FUNC_HANDLER)(VEC* args, SYM_TABLE* table);

typedef struct FUNC {
    FUNC_HANDLER handler;
    char* name;
    char* sig;
} FUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(FUNC_HANDLER handler, const char* name, const char* spec);
void freefunc(FUNC* func);
char* astrfunc(FUNC* func);
VEC* funcargs(const char* sig, VEC* nodes, SYM_TABLE* table);
VALUE* funcexec(FUNC* func, VEC* nodes, SYM_TABLE* table);


#endif /* FUNC_H_ */
