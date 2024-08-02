#ifndef SYMTABLE_H_
#define SYMTABLE_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct MAP MAP;
typedef struct VALUE VALUE;


/* ***************************************************************************
* TYPES
*/

typedef struct SYM_TABLE {
    struct MAP* symbols;
    struct SYM_TABLE* parent;
} SYM_TABLE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYM_TABLE* newtable(SYM_TABLE* parent);
VALUE* gettable(SYM_TABLE* table, const char* name);
void settable(SYM_TABLE* table, const char* name, VALUE* value);


#endif /* SYMTABLE_H_ */
