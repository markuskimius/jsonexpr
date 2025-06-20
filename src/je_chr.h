#ifndef JE_CHR_H_
#define JE_CHR_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* TYPES
*/

typedef struct JE_CHR {
    char chr[5];
    size_t length;
} JE_CHR;


/* ***************************************************************************
* FUNCTIONS
*/

JE_CHR JE_ChrCreate(uint32_t u32);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_CHR_H_ */
