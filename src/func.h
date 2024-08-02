#ifndef FUNC_H_
#define FUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct AST_NODE AST_NODE;
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

typedef VALUE* (*FUNC_HANDLER)(size_t argc, AST_NODE** argv, SYM_TABLE* table);

typedef struct FUNC {
    FUNC_HANDLER handler;
    size_t minargs;
    size_t maxargs;
    char* name;
} FUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

FUNC* newfunc(const char* name, FUNC_HANDLER handler, size_t minargs, size_t maxargs);
void freefunc(FUNC* func);
char* astrfunc(FUNC* func);
VALUE* funcexec(FUNC* func, size_t argc, AST_NODE** argv, SYM_TABLE* table);


#endif /* FUNC_H_ */
