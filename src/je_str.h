#ifndef JE_STR_H_
#define JE_STR_H_

#include <stddef.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_CHR JE_CHR;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_STR {
    uint8_t* data;
    size_t length;
} JE_STR;


/* ***************************************************************************
* FUNCTIONS
*/

JE_STR JE_StrCreate(char* cstr);
JE_STR JE_StrClone(const JE_STR str);
JE_STR JE_StrPlusStr(const JE_STR x, const JE_STR y);
int64_t JE_StrCompare(const JE_STR x, const JE_STR y);
void JE_StrPushStr(JE_STR* x, const JE_STR y);
void JE_StrPushCstr(JE_STR* x, const char* y);
void JE_StrPushChr(JE_STR* x, const JE_CHR y);
void JE_StrDestroy(JE_STR* str);

size_t JE_StrLength(const JE_STR str);
int64_t JE_StrToI64(const JE_STR str);
double JE_StrToF64(const JE_STR str);
char* JE_StrToCstr(const JE_STR str);
char* JE_StrToQstr(const JE_STR str);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_STR_H_ */
