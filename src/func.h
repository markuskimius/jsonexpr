#ifndef FUNC_H_
#define FUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct NODE NODE;
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

typedef VALUE* (*FUNC_HANDLER)(size_t argc, NODE** argv, SYM_TABLE* table);

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
VALUE* funcexec(FUNC* func, size_t argc, NODE** argv, SYM_TABLE* table);


#endif /* FUNC_H_ */
