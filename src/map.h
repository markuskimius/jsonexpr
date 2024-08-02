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
    const char* key;
    struct MAP* next[256];
    struct MAP* prev;
} MAP;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

MAP* newmap();
void freemap(MAP* map);
char* astrmap(MAP* map);
void setmap(MAP* map, const char* key, VALUE* value);
VALUE* getmap(MAP* map, const char* key);
MAP* nextmap(const MAP* map);


#endif /* MAP_H_ */
