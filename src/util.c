#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* GLOBALS
*/

char utf8buf[8];


/* ***************************************************************************
* SCALAR
*/

/**
* Convert a unicode value to utf8 character string sequence.  The returned
* sequence is allocated globally and overwritten on subsequent calls to the
* function.
*/
char* utf8str(uint32_t c) {
    char* cp = utf8buf;

    if     (c <   0x0080) { *cp++ = c; }
    else if(c <   0x0800) { *cp++ = (c >>  6) | 0xc0;                                                                      *cp++ = (c & 0x3f) | 0x80; }
    else if(c <  0x10000) { *cp++ = (c >> 12) | 0xe0;                                    *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else if(c < 0x110000) { *cp++ = (c >> 18) | 0xf0; *cp++ = ((c >> 12) & 0x3f) | 0x80; *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else                  { *cp++ = 0xf4; *cp++ = 0x8f; *cp++ = 0xbf; *cp++ = 0xbf; }

    *cp = '\0';

    return utf8buf;
}


char* astri64(int64_t src) {
    char* dest = calloc(1, IBUFSIZE);

    snprintf(dest, IBUFSIZE, "%lld", (unsigned long long) src);

    return dest;
}


char* astrf64(double src) {
    char* dest = calloc(1, DBUFSIZE);

    snprintf(dest, DBUFSIZE, "%lf", src);

    return dest;
}


/**
* Like strcat, but dest is assumed to be reallocatable.
*/
char* astrcat(char* dest, const char* src) {
    size_t dlen = strlen(dest);
    size_t slen = strlen(src);

    dest = realloc(dest, dlen+slen+1);
    snprintf(dest+dlen, slen+1, "%s", src);

    return dest;
}


/**
* Like asprintf, but append to dest and return new memory.
*/
char* casprintf(char* dest, const char* format, ...) {
    size_t dlen = strlen(dest);
    size_t slen = 0;
    va_list ap;
    char* src;

    /* source */
    va_start(ap, format);
    vasprintf(&src, format, ap);
    va_end(ap);

    /* append */
    slen = strlen(src);
    dest = realloc(dest, dlen+slen+1);
    snprintf(dest+dlen, slen+1, "%s", src);

    /* free */
    free(src);

    return dest;
}


char* astrencode(const char* src) {
    char* dest = calloc(1, strlen(src) + 3);
    const char* cp = src;

    /* Opening quote */
    dest = astrcat(dest, "\"");

    /* Decode each character */
    while(*cp) {
        char buf[] = { *cp, '\0' };

        switch(*cp) {
            case '"':
            case '\\':
                dest = astrcat(dest, "\\");
                dest = astrcat(dest, buf);
                break;

            case '\b' : buf[0] = 'b'; dest = astrcat(dest, "\\"); dest = astrcat(dest, buf); break;
            case '\f' : buf[0] = 'f'; dest = astrcat(dest, "\\"); dest = astrcat(dest, buf); break;
            case '\n' : buf[0] = 'n'; dest = astrcat(dest, "\\"); dest = astrcat(dest, buf); break;
            case '\r' : buf[0] = 'r'; dest = astrcat(dest, "\\"); dest = astrcat(dest, buf); break;
            case '\t' : buf[0] = 't'; dest = astrcat(dest, "\\"); dest = astrcat(dest, buf); break;
            default   : dest = astrcat(dest, buf); break;
        }

        cp++;
    }

    /* Closing quote */
    dest = astrcat(dest, "\"");

    return dest;
}


char* textat(YYLTYPE* loc) {
    size_t length = loc->last_pos - loc->first_pos + 1;
    char* code = *loc->codeptr;
    char* buf = malloc(length);;

    snprintf(buf, length, "%s", code + loc->first_pos);

    return buf;
}
