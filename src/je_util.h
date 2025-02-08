#ifndef JE_UTIL_H_
#define JE_UTIL_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_YYLTYPE JE_YYLTYPE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_LINE_ITER {
    const char* next;
    char* line;
} JE_LINE_ITER;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void* JE_Calloc(size_t nmemb, size_t size);
void* JE_Realloc(void *ptr, size_t nmemb, size_t size);
void JE_Free(void *ptr);

char* JE_UcharToCstr(uint32_t c);
char* JE_IntToAstr(int64_t src);
char* JE_FloatToAstr(double src);
char* JE_AstrCat(char* dest, const char* src);
char* JE_AstrCatFormat(char* dest, const char* format, ...);
char* JE_CstrToQstr(const char* src);
char* JE_LocToAstr(JE_YYLTYPE* loc);
char* JE_CstrAcat(const char* s1, const char* s2);
size_t JE_CstrGetLines(const char* s);
size_t JE_CstrGetColumns(const char* s);

JE_LINE_ITER* JE_LineIterNew(const char* s);
const char* JE_LineIterNext(JE_LINE_ITER* iter);
void JE_LineIterDelete(JE_LINE_ITER* iter);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_UTIL_H_ */
