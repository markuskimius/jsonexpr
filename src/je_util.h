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

char* je_utf8str(uint32_t c);
char* je_astri64(int64_t src);
char* je_astrf64(double src);
char* je_astrcat(char* dest, const char* src);
char* je_casprintf(char* dest, const char* format, ...);
char* je_astrencode(const char* src);
char* je_atextat(JE_YYLTYPE* loc);
char* je_amcat(const char* s1, const char* s2);
size_t je_nlines(const char* s);
size_t je_maxwidth(const char* s);

JE_LINE_ITER* je_newlineiter(const char* s);
const char* je_nextline(JE_LINE_ITER* iter);
void je_freelineiter(JE_LINE_ITER* iter);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_UTIL_H_ */
