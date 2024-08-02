#ifndef UTIL_H_
#define UTIL_H_

#include <inttypes.h>


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

char* utf8str(uint32_t c);
char* astri64(uint64_t src);
char* astrf64(double src);
char* astrcat(char* dest, const char* src);
char* astrencode(const char* src);


#endif /* UTIL_H_ */
