#ifndef JE_SYMMAP_H_
#define JE_SYMMAP_H_

#include "je_map.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FUNCTIONS
*/

JE_MAP* JE_SymmapCreate(int set);
JE_MAP* JE_SymmapCreateScope(JE_MAP* parent);
void JE_SymmapDestroy(JE_MAP* symmap);

JE_VALUE* JE_SymmapSet(JE_MAP* symmap, JE_STR symbol, JE_VALUE value, int local);
JE_VALUE* JE_SymmapGet(const JE_MAP symmap, const JE_STR symbol);
void JE_SymmapRemove(JE_MAP* symmap, const JE_STR symbol);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_SYMMAP_H_ */
