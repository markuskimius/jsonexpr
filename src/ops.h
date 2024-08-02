#ifndef OPS_H_
#define OPS_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct AST_NODE AST_NODE;
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
VALUE* op_uplus(VALUE* value);
VALUE* op_uminus(VALUE* value);

VEC* evallist(AST_NODE* node, SYM_TABLE* symtable);


#endif /* OPS_H_ */
