#ifndef JE_VEC_H_
#define JE_VEC_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_VALUE JE_VALUE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_VEC {
    JE_VALUE* data;
    size_t length;
} JE_VEC;


/* ***************************************************************************
* FUNCTIONS
*/

JE_VEC JE_VecCreate(size_t capacity);
JE_VEC JE_VecClone(const JE_VEC vec);
JE_VEC JE_VecPlusVec(const JE_VEC x, const JE_VEC y);
int64_t JE_VecCompare(const JE_VEC x, const JE_VEC y);
void JE_VecPlusAssnVec(JE_VEC* x, const JE_VEC y);
void JE_VecDestroy(JE_VEC* vec);

JE_VALUE* JE_VecSet(JE_VEC* vec, size_t i, JE_VALUE val);
JE_VALUE* JE_VecPush(JE_VEC* vec, JE_VALUE val);
JE_VALUE JE_VecPop(JE_VEC* vec);
void JE_VecRemove(JE_VEC* vec, size_t i);

JE_VALUE* JE_VecGet(const JE_VEC vec, size_t i);
size_t JE_VecLength(const JE_VEC vec);
char* JE_VecToCstr(const JE_VEC vec);
char* JE_VecToQstr(const JE_VEC vec);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_VEC_H_ */
