#ifndef JE_PARSE_H_
#define JE_PARSE_H_

#include "je_parse.tab.h"


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_NODE JE_NODE;

extern JE_NODE* je_ast;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE* je_parse(const char* code);


#endif /* JE_PARSE_H_ */
