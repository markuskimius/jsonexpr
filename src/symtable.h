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
    size_t count;
} SYM_TABLE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYM_TABLE* newtable(SYM_TABLE* parent);
SYM_TABLE* duptable(SYM_TABLE* table);
void freetable(SYM_TABLE* table);
void settable(SYM_TABLE* table, const char* name, VALUE* value);
void unsettable(SYM_TABLE* table, const char* name);
VALUE* gettable(SYM_TABLE* table, const char* name);


#endif /* SYMTABLE_H_ */
