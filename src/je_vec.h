#ifndef JE_VEC_H_
#define JE_VEC_H_

#include <inttypes.h>

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

typedef struct JE_VEC {
    JE_VAL** item;
    size_t capacity;
    size_t length;
    size_t count;
} JE_VEC;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_VEC* JE_VecNew();
JE_VEC* JE_VecDup(JE_VEC* vec);
void JE_VecDelete(JE_VEC* vec);

int JE_VecSet(JE_VEC* vec, size_t index, JE_VAL* item);
void JE_VecPop(JE_VEC* vec);
void JE_VecPush(JE_VEC* vec, JE_VAL* item);
void JE_VecUnset(JE_VEC* vec, size_t index);
JE_VAL* JE_VecGet(JE_VEC* vec, size_t index);
JE_VAL* JE_VecBack(JE_VEC* vec);

int JE_VecCmp(JE_VEC* vec1, JE_VEC* vec2);
char* JE_VecToAstr(JE_VEC* vec);
size_t JE_VecLen(JE_VEC* vec);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_VEC_H_ */
