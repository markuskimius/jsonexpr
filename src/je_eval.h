#ifndef JE_EVAL_H_
#define JE_EVAL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VAL    JE_VAL;
typedef struct JE_NODE   JE_NODE;
typedef struct JE_SYMTBL JE_SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* je_eval(JE_NODE* node, JE_SYMTBL* table);


#endif /* JE_EVAL_H_ */
