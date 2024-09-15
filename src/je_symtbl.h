#ifndef JE_JE_SYMTBL_H_
#define JE_JE_SYMTBL_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_MAP JE_MAP;
typedef struct JE_VAL JE_VAL;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_SYMTBL {
    struct JE_MAP* symbols;
    struct JE_SYMTBL* parent;
    size_t count;
} JE_SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_SYMTBL* je_newtable(JE_SYMTBL* parent);
JE_SYMTBL* je_duptable(JE_SYMTBL* table);
void je_freetable(JE_SYMTBL* table);

void je_tableset(JE_SYMTBL* table, const char* name, JE_VAL* value);
void je_tableunset(JE_SYMTBL* table, const char* name);
JE_VAL* je_tableget(JE_SYMTBL* table, const char* name);


#endif /* JE_JE_SYMTBL_H_ */
