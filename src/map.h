#ifndef MAP_H_
#define MAP_H_


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct VALUE VALUE;


/* ***************************************************************************
* TYPES
*/

typedef struct MAP {
    VALUE* value;
    char* key;
    struct MAP* next[256];
    struct MAP* prev;
    size_t nchildren;
    size_t length;
} MAP;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* newmap();
void freemap(MAP* map);
char* astrmap(MAP* map);
void setmap(MAP* map, const char* key, VALUE* value);
void unsetmap(MAP* map, const char* key);
VALUE* getmap(MAP* map, const char* key);
MAP* nextmap(const MAP* map);

int cmpmap(MAP* map1, MAP* map2);


#endif /* MAP_H_ */
