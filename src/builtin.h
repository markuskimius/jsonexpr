#ifndef BUILTIN_H_
#define BUILTIN_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct MAP MAP;
typedef struct NODE NODE;
typedef struct SYMTBL SYMTBL;
typedef struct VAL VAL;
typedef struct VEC VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* binfns();
MAP* binops();
VAL* fnexec(const char* key, SYMTBL* table, VEC* args);
VAL* opexec(const char* key, SYMTBL* table, NODE* left, NODE* right, NODE* righter);


#endif /* BUILTIN_H_ */
