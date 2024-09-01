#ifndef SYMTBL_H_
#define SYMTBL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct MAP MAP;
typedef struct VALUE VALUE;


/* ***************************************************************************
* TYPES
*/

typedef struct SYMTBL {
    struct MAP* symbols;
    struct SYMTBL* parent;
    size_t count;
} SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

SYMTBL* newtable(SYMTBL* parent);
SYMTBL* duptable(SYMTBL* table);
void freetable(SYMTBL* table);

void tableset(SYMTBL* table, const char* name, VALUE* value);
void tableunset(SYMTBL* table, const char* name);
VALUE* tableget(SYMTBL* table, const char* name);


#endif /* SYMTBL_H_ */
