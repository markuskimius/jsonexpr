#ifndef EVAL_H_
#define EVAL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct NODE NODE;
typedef struct SYMTBL SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* eval(NODE* node, SYMTBL* table);


#endif /* EVAL_H_ */
