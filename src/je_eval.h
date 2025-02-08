#ifndef JE_EVAL_H_
#define JE_EVAL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VAL    JE_VAL;
typedef struct JE_NODE   JE_NODE;
typedef struct JE_SYMTBL JE_SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* JE_EvalByNode(JE_NODE* node, JE_SYMTBL* table);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_EVAL_H_ */
