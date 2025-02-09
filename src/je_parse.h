#ifndef JE_PARSE_H_
#define JE_PARSE_H_

#include "je_parse.tab.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_NODE JE_NODE;

extern JE_NODE* JE_ast;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE* JE_Parse(const char* code);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_PARSE_H_ */
