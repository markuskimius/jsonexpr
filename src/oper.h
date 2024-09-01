#ifndef OPS_H_
#define OPS_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;
typedef struct VALUE VALUE;
typedef struct SYMTBL SYMTBL;
typedef struct VEC VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* op_times(VALUE* lvalue, VALUE* rvalue);
VALUE* op_divby(VALUE* lvalue, VALUE* rvalue);
VALUE* op_mod(VALUE* lvalue, VALUE* rvalue);
VALUE* op_plus(VALUE* lvalue, VALUE* rvalue);
VALUE* op_minus(VALUE* lvalue, VALUE* rvalue);
VALUE* op_bor(VALUE* lvalue, VALUE* rvalue);
VALUE* op_band(VALUE* lvalue, VALUE* rvalue);
VALUE* op_bxor(VALUE* lvalue, VALUE* rvalue);
VALUE* op_bnot(VALUE* lvalue);
VALUE* op_lnot(VALUE* lvalue);
VALUE* op_eq(VALUE* lvalue, VALUE* rvalue);
VALUE* op_ne(VALUE* lvalue, VALUE* rvalue);
VALUE* op_lt(VALUE* lvalue, VALUE* rvalue);
VALUE* op_le(VALUE* lvalue, VALUE* rvalue);
VALUE* op_gt(VALUE* lvalue, VALUE* rvalue);
VALUE* op_ge(VALUE* lvalue, VALUE* rvalue);
VALUE* op_uplus(VALUE* value);
VALUE* op_uminus(VALUE* value);
VALUE* op_shl(VALUE* lvalue, VALUE* rvalue);
VALUE* op_asr(VALUE* lvalue, VALUE* rvalue);
VALUE* op_shr(VALUE* lvalue, VALUE* rvalue);
VALUE* op_pow(VALUE* lvalue, VALUE* rvalue);
VALUE* op_lor(NODE* left, NODE* right, SYMTBL* table);
VALUE* op_land(NODE* left, NODE* right, SYMTBL* table);
VALUE* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYMTBL* table);


#endif /* OPS_H_ */
