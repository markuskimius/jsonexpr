#ifndef JE_MAP_H_
#define JE_MAP_H_


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

JE_MAP* je_newmap();
JE_MAP* je_dupmap(JE_MAP* map);
void je_freemap(JE_MAP* map);

void je_mapset(JE_MAP* map, const char* key, JE_VAL* value);
void je_mapunset(JE_MAP* map, const char* key);
void je_mapclear(JE_MAP* map);
JE_VAL* je_mapget(JE_MAP* map, const char* key);

int je_mapcmp(JE_MAP* map1, JE_MAP* map2);
JE_MAP* je_mapnext(JE_MAP* map);
char* je_mapastr(JE_MAP* map);

char* je_mapkey(JE_MAP* map);
JE_VAL* je_mapval(JE_MAP* map);


#endif /* JE_MAP_H_ */
