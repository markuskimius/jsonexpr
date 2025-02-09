#ifndef JE_FUNC_H_
#define JE_FUNC_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VEC    JE_VEC;
typedef struct JE_NODE   JE_NODE;
typedef struct JE_VAL    JE_VAL;
typedef struct JE_SYMTBL JE_SYMTBL;


/* ***************************************************************************
* CONSTANTS
*/

#define JE_MAXARGS ((size_t) -1)

typedef enum {
    JE_BINARY_FT,
    JE_CUSTOM_FT,
} JE_FUNC_T;


/* ***************************************************************************
* TYPES
*/

typedef JE_VAL* (*JE_BINARY_FN)(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc);

typedef struct JE_FUNC {
    JE_FUNC_T type;
    union {
        JE_BINARY_FN bin;
        JE_NODE* cust;
    } handler;

    char* name;
    char* sig;
    JE_SYMTBL* ctx;
    size_t count;
} JE_FUNC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_FUNC* JE_FuncNew(JE_BINARY_FN handler, const char* name, const char* spec);
JE_FUNC* JE_FuncNewUser(JE_NODE* handler, const char* name, const char* sig, JE_SYMTBL* ctx);
JE_FUNC* JE_FuncDup(JE_FUNC* func);
void JE_FuncDelete(JE_FUNC* func);

JE_VEC* JE_FuncParseArgs(const char* sig, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc);
JE_VAL* JE_FuncExec(JE_FUNC* func, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc);
char* JE_FuncToAstr(JE_FUNC* func);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_FUNC_H_ */
