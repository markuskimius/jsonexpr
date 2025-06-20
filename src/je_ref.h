#ifndef JE_REF_H_
#define JE_REF_H_

#include "je_fun.h"
#include "je_map.h"
#include "je_vec.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FUNCTIONS
*/

JE_MAP* JE_RefMapAlloc(JE_MAP map);
JE_VEC* JE_RefVecAlloc(JE_VEC vec);
JE_FUN* JE_RefFunAlloc(JE_FUN fun);
JE_MAP* JE_RefMapDup(const JE_MAP* map);
JE_VEC* JE_RefVecDup(const JE_VEC* vec);
JE_FUN* JE_RefFunDup(const JE_FUN* fun);
void JE_RefMapFree(JE_MAP* map);
void JE_RefVecFree(JE_VEC* vec);
void JE_RefFunFree(JE_FUN* fun);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_REF_H_ */
