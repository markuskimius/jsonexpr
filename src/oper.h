#ifndef OPS_H_
#define OPS_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct NODE NODE;
typedef struct VALUE VALUE;
typedef struct SYM_TABLE SYM_TABLE;
typedef struct VEC VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

VALUE* op_times(VALUE* lvalue, VALUE* rvalue);
VALUE* op_divby(VALUE* lvalue, VALUE* rvalue);
VALUE* op_mod(VALUE* lvalue, VALUE* rvalue);
VALUE* op_plus(VALUE* lvalue, VALUE* rvalue);
VALUE* op_minus(VALUE* lvalue, VALUE* rvalue);
VALUE* op_eq(VALUE* lvalue, VALUE* rvalue);
VALUE* op_ne(VALUE* lvalue, VALUE* rvalue);
VALUE* op_lt(VALUE* lvalue, VALUE* rvalue);
VALUE* op_le(VALUE* lvalue, VALUE* rvalue);
VALUE* op_gt(VALUE* lvalue, VALUE* rvalue);
VALUE* op_ge(VALUE* lvalue, VALUE* rvalue);
VALUE* op_lnot(VALUE* value);
VALUE* op_uplus(VALUE* value);
VALUE* op_uminus(VALUE* value);
VALUE* op_pow(VALUE* lvalue, VALUE* rvalue);
VALUE* op_lor(NODE* left, NODE* right, SYM_TABLE* table);
VALUE* op_land(NODE* left, NODE* right, SYM_TABLE* table);
VALUE* op_cond(NODE* test, NODE* iftrue, NODE* iffalse, SYM_TABLE* table);


#endif /* OPS_H_ */
