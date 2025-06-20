#ifndef JE_FUN_H_
#define JE_FUN_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_ASTINFO JE_ASTINFO;
typedef struct JE_MAP JE_MAP;
typedef struct JE_NODE JE_NODE;
typedef struct JE_VALUE JE_VALUE;
typedef struct JE_VEC JE_VEC;

typedef JE_VALUE (*JE_FUNCALL)(JE_VEC* args, const JE_NODE* code, JE_MAP* symmap, const JE_ASTINFO* info);


/* ***************************************************************************
* TYPES
*/

typedef struct JE_FUN {
    char* name;
    char* argfmt;
    const JE_NODE* code;
    JE_FUNCALL call;
    JE_MAP* symmap;
} JE_FUN;


/* ***************************************************************************
* FUNCTIONS
*/

JE_FUN JE_FunCreate(char* name, JE_FUNCALL call, char* argfmt, const JE_NODE* code, JE_MAP* symmap);
void JE_FunDestroy(JE_FUN* fun);

JE_VALUE JE_FunCall(const JE_FUN fun, const JE_VEC args, const JE_NODE* source, JE_MAP* symmap, const JE_ASTINFO* info);
char* JE_FunToCstr(const JE_FUN fun);
char* JE_FunToQstr(const JE_FUN fun);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_FUN_H_ */
