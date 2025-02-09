#ifndef JE_SYMTBL_H_
#define JE_SYMTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


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
    struct JE_SYMTBL* global;
    struct JE_SYMTBL* builtin;
    size_t count;

    struct JE_VAL* symval;
} JE_SYMTBL;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_SYMTBL* JE_SymtblNew(JE_SYMTBL* parent);
JE_SYMTBL* JE_SymtblDup(JE_SYMTBL* table);
void JE_SymtblDelete(JE_SYMTBL* table);

void JE_SymtblSet(JE_SYMTBL* table, const char* name, JE_VAL* value, int localonly);
void JE_SymtblUnset(JE_SYMTBL* table, const char* name);
void JE_SymtblClear(JE_SYMTBL* table, int localonly);
JE_VAL* JE_SymtblGet(JE_SYMTBL* table, const char* name);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_SYMTBL_H_ */
