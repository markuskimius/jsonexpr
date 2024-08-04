#ifndef UFUNC_H_
#define UFUNC_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct NODE NODE;
typedef struct VEC VEC;
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
    NODE* handler;
    char* name;
    char* sig;
} UFUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

UFUNC* newufunc(NODE* handler, const char* name, const char* sig);
void freeufunc(UFUNC* ufunc);
char* astrufunc(UFUNC* ufunc);
VALUE* ufuncexec(UFUNC* ufunc, VEC* args, SYM_TABLE* table);


#endif /* UFUNC_H_ */
