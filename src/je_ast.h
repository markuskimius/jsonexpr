#ifndef JE_AST_H_
#define JE_AST_H_

#include "je_astnode.h"
#include "je_value.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_ASTINFO JE_ASTINFO;
typedef struct JE_MAP JE_MAP;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_AST {
    JE_ASTNODE astnode;
    JE_ASTINFO info;
    int isok;
} JE_AST;


/* ***************************************************************************
* FUNCTIONS
*/

JE_AST JE_AstCreate();
void JE_AstDestroy(JE_AST* ast);

JE_VALUE JE_AstEval(const JE_AST ast, JE_MAP* symmap, size_t inode);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_AST_H_ */
