#ifndef OPS_H_
#define OPS_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;
typedef struct VAL VAL;
typedef struct SYMTBL SYMTBL;
typedef struct VEC VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VAL* op_times(VAL* lvalue, VAL* rvalue);
VAL* op_divby(VAL* lvalue, VAL* rvalue);
VAL* op_mod(VAL* lvalue, VAL* rvalue);
VAL* op_plus(VAL* lvalue, VAL* rvalue);
VAL* op_minus(VAL* lvalue, VAL* rvalue);
VAL* op_bor(VAL* lvalue, VAL* rvalue);
VAL* op_band(VAL* lvalue, VAL* rvalue);
VAL* op_bxor(VAL* lvalue, VAL* rvalue);
VAL* op_bnot(VAL* lvalue);
VAL* op_lnot(VAL* lvalue);
VAL* op_eq(VAL* lvalue, VAL* rvalue);
VAL* op_ne(VAL* lvalue, VAL* rvalue);
VAL* op_lt(VAL* lvalue, VAL* rvalue);
VAL* op_le(VAL* lvalue, VAL* rvalue);
VAL* op_gt(VAL* lvalue, VAL* rvalue);
VAL* op_ge(VAL* lvalue, VAL* rvalue);
VAL* op_uplus(VAL* value);
VAL* op_uminus(VAL* value);
VAL* op_shl(VAL* lvalue, VAL* rvalue);
VAL* op_asr(VAL* lvalue, VAL* rvalue);
VAL* op_shr(VAL* lvalue, VAL* rvalue);
VAL* op_pow(VAL* lvalue, VAL* rvalue);
VAL* op_lor(NODE* left, NODE* right, SYMTBL* table);
VAL* op_land(NODE* left, NODE* right, SYMTBL* table);
VAL* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYMTBL* table);


#endif /* OPS_H_ */
