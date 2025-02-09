#ifndef JE_BUILTIN_H_
#define JE_BUILTIN_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_MAP    JE_MAP;
typedef struct JE_NODE   JE_NODE;
typedef struct JE_SYMTBL JE_SYMTBL;
typedef struct JE_VAL    JE_VAL;
typedef struct JE_VEC    JE_VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP* JE_BuiltinFns();
JE_MAP* JE_BuiltinOps();
JE_VAL* JE_BuiltinOpExec(const char* key, JE_SYMTBL* table, JE_NODE* left, JE_NODE* right, JE_NODE* righter);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_BUILTIN_H_ */
