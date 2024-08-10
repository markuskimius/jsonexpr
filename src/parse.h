#ifndef PARSE_H_
#define PARSE_H_

#include "parse.tab.h"


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;

extern NODE* yyast;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* parse(const char* code);


#endif /* PARSE_H_ */
