#ifndef PARSE_H_
#define PARSE_H_

#include "parse.tab.h"


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;

extern char* yytext;
extern const char* yyerrtext;
extern int yylineno;
extern int yycolumn;
extern NODE* yyast;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

NODE* parse(const char* code);


#endif /* PARSE_H_ */
