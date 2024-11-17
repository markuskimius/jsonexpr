#ifndef JE_FUNC_H_
#define JE_FUNC_H_

#include <inttypes.h>


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
} je_func_t;


/* ***************************************************************************
* TYPES
*/

typedef JE_VAL* (*JE_BINARY_FN)(JE_VEC* args, JE_SYMTBL* table, JE_YYLTYPE* loc);

typedef struct JE_FUNC {
    je_func_t type;
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

JE_FUNC* je_newfunc(JE_BINARY_FN handler, const char* name, const char* spec);
JE_FUNC* je_newcustfunc(JE_NODE* handler, const char* name, const char* sig, JE_SYMTBL* ctx);
JE_FUNC* je_dupfunc(JE_FUNC* func);
void je_freefunc(JE_FUNC* func);

JE_VEC* je_funcargs(const char* sig, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc);
JE_VAL* je_funcexec(JE_FUNC* func, JE_VEC* nodes, JE_SYMTBL* table, JE_YYLTYPE* loc);
char* je_funcastr(JE_FUNC* func);


#endif /* JE_FUNC_H_ */
