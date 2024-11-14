#ifndef JE_UTIL_H_
#define JE_UTIL_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_YYLTYPE JE_YYLTYPE;


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


#endif /* JE_UTIL_H_ */
