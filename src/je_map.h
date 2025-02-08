#ifndef JE_MAP_H_
#define JE_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VAL JE_VAL;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_MAP {
    JE_VAL* value;
    char* key;
    struct JE_MAP* next[256];
    struct JE_MAP* prev;
    size_t nchildren;
    size_t length;
    size_t count;
} JE_MAP;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_MAP* JE_MapNew();
JE_MAP* JE_MapDup(JE_MAP* map);
void JE_MapDelete(JE_MAP* map);

void JE_MapSet(JE_MAP* map, const char* key, JE_VAL* value);
void JE_MapUnset(JE_MAP* map, const char* key);
void JE_MapClear(JE_MAP* map);
JE_VAL* JE_MapGet(JE_MAP* map, const char* key);

int JE_MapCmp(JE_MAP* map1, JE_MAP* map2);
JE_MAP* JE_MapNext(JE_MAP* map);
char* JE_MapToAstr(JE_MAP* map);

char* JE_MapKey(JE_MAP* map);
JE_VAL* JE_MapVal(JE_MAP* map);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_MAP_H_ */
