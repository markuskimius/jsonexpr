#ifndef UFUNC_H_
#define UFUNC_H_

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

typedef struct UFUNC {
    AST_NODE* handler;
    size_t minargs;
    size_t maxargs;
    char* name;
} UFUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

UFUNC* newufunc(const char* name, AST_NODE* handler, size_t minargs, size_t maxargs);
void freeufunc(UFUNC* ufunc);
char* astrufunc(UFUNC* ufunc);
VALUE* ufuncexec(UFUNC* ufunc, size_t argc, AST_NODE** argv, SYM_TABLE* table);


#endif /* UFUNC_H_ */
