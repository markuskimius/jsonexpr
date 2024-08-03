#ifndef EVAL_H_
#define EVAL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;
typedef struct NODE NODE;
typedef struct SYM_TABLE SYM_TABLE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* eval(NODE* node, SYM_TABLE* table);


#endif /* EVAL_H_ */
