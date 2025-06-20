#ifndef JE_PARSE_H_
#define JE_PARSE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_AST JE_AST;
typedef struct JE_TOKEN JE_TOKEN;


/* ***************************************************************************
* TYPES
*/

#define JE_YYLTYPE_IS_DECLARED

typedef struct JE_YYLTYPE {
    size_t first;
    size_t last;
} JE_YYLTYPE;


/* ***************************************************************************
* GLOBALS
*/

extern JE_AST JE_ast;
extern JE_TOKEN* JE_token;
extern JE_YYLTYPE JE_yylloc;


/* ***************************************************************************
* FUNCTIONS
*/

JE_AST JE_Parse(const char* code);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_PARSE_H_ */
