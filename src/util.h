#ifndef UTIL_H_
#define UTIL_H_

#include <inttypes.h>


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct YYLTYPE YYLTYPE;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

char* utf8str(uint32_t c);
char* astri64(int64_t src);
char* astrf64(double src);
char* astrcat(char* dest, const char* src);
char* casprintf(char* dest, const char* format, ...);
char* astrencode(const char* src);
char* textat(YYLTYPE* loc);


#endif /* UTIL_H_ */
