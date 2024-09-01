#ifndef EVAL_H_
#define EVAL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VAL VAL;
typedef struct NODE NODE;
typedef struct SYMTBL SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* eval(NODE* node, SYMTBL* table);


#endif /* EVAL_H_ */
