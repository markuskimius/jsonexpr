#ifndef MAP_H_
#define MAP_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VAL VAL;


/* ***************************************************************************
* TYPES
*/

typedef struct MAP {
    VAL* value;
    char* key;
    struct MAP* next[256];
    struct MAP* prev;
    size_t nchildren;
    size_t length;
    size_t count;
} MAP;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* newmap();
MAP* dupmap(MAP* map);
void freemap(MAP* map);

void mapset(MAP* map, const char* key, VAL* value);
void mapunset(MAP* map, const char* key);
VAL* mapget(MAP* map, const char* key);

int mapcmp(MAP* map1, MAP* map2);
MAP* mapnext(MAP* map);
char* mapastr(MAP* map);


#endif /* MAP_H_ */
