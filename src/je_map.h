#ifndef JE_MAP_H_
#define JE_MAP_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_MAP JE_MAP;
typedef struct JE_STR JE_STR;
typedef struct JE_VALUE JE_VALUE;


/* ***************************************************************************
* TYPES
*/

struct JE_MAP {
    JE_STR* key;
    JE_VALUE* value;
    size_t size;
};


/* ***************************************************************************
* FUNCTIONS
*/

JE_MAP JE_MapCreate(size_t capacity);
JE_MAP JE_MapClone(const JE_MAP map);
JE_MAP JE_MapPlusMap(const JE_MAP x, const JE_MAP y);
int64_t JE_MapCompare(const JE_MAP x, const JE_MAP y);
void JE_MapPlusAssnMap(JE_MAP* x, const JE_MAP y);
void JE_MapDestroy(JE_MAP* map);

JE_VALUE* JE_MapSet(JE_MAP* map, JE_STR key, JE_VALUE value);
JE_VALUE* JE_MapGet(const JE_MAP map, const JE_STR key);
void JE_MapRemove(JE_MAP* map, const JE_STR key);

size_t JE_MapSize(const JE_MAP map);
char* JE_MapToCstr(const JE_MAP map);
char* JE_MapToQstr(const JE_MAP map);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_MAP_H_ */
