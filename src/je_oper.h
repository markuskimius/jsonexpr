#ifndef JE_OPS_H_
#define JE_OPS_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_NODE JE_NODE;
typedef struct JE_VAL JE_VAL;
typedef struct JE_SYMTBL JE_SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VAL* je_op_times(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_divby(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_mod(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_plus(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_minus(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_bor(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_band(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_bxor(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_bnot(JE_VAL* lvalue);
JE_VAL* je_op_lnot(JE_VAL* lvalue);
JE_VAL* je_op_eq(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_ne(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_lt(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_le(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_gt(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_ge(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_uplus(JE_VAL* value);
JE_VAL* je_op_uminus(JE_VAL* value);
JE_VAL* je_op_shl(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_asr(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_shr(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_pow(JE_VAL* lvalue, JE_VAL* rvalue);
JE_VAL* je_op_lor(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table);
JE_VAL* je_op_land(JE_NODE* left, JE_NODE* right, JE_SYMTBL* table);
JE_VAL* je_op_cond(JE_NODE* test, JE_NODE* iftrue, JE_NODE* iffalse, JE_SYMTBL* table);


#endif /* JE_OPS_H_ */
