#define _GNU_SOURCE
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_parse.h"
#include "je_util.h"
#include "je_token.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _IBUFSIZE   32
#define _DBUFSIZE   320


/* ***************************************************************************
* MACROS
*/

#define _MAX(a,b)   (a>b?a:b)
#define _MIN(a,b)   (a<b?a:b)
#define _WORDSIZE   (sizeof(long))
#define _ROUNDUP(x) ((x + (_WORDSIZE-1)) & -_WORDSIZE)


/* ***************************************************************************
* PRIVATE VARIABLES
*/

static char _utf8buf[8];


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void* JE_Calloc(size_t nmemb, size_t size) {
    void* mem = calloc(nmemb, size);

    if(mem == NULL) {
        perror(__FUNCTION__);
        exit(1);
    }

    return mem;
}

void* JE_Realloc(void *ptr, size_t nmemb, size_t size) {
    void* mem = realloc(ptr, nmemb * _ROUNDUP(size));

    if(mem == NULL) {
        perror(__FUNCTION__);
        exit(1);
    }

    return mem;
}


/* ***************************************************************************
* SCALAR
*/

/**
* Convert a unicode value to utf8 character string sequence.  The returned
* sequence is allocated globally and overwritten on subsequent calls to the
* function.
*/
char* JE_UcharToCstr(uint32_t c) {
    char* cp = _utf8buf;

    if     (c <   0x0080) { *cp++ = c; }
    else if(c <   0x0800) { *cp++ = (c >>  6) | 0xc0;                                                                      *cp++ = (c & 0x3f) | 0x80; }
    else if(c <  0x10000) { *cp++ = (c >> 12) | 0xe0;                                    *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else if(c < 0x110000) { *cp++ = (c >> 18) | 0xf0; *cp++ = ((c >> 12) & 0x3f) | 0x80; *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else                  { *cp++ = 0xf4; *cp++ = 0x8f; *cp++ = 0xbf; *cp++ = 0xbf; }

    *cp = '\0';

    return _utf8buf;
}

char* JE_IntToAstr(int64_t src) {
    char* dest = JE_Calloc(1, _IBUFSIZE);

    snprintf(dest, _IBUFSIZE, "%lld", src);

    return dest;
}

char* JE_FloatToAstr(double src) {
    char* dest = JE_Calloc(1, _DBUFSIZE);

    snprintf(dest, _DBUFSIZE, "%lf", src);

    return dest;
}

/**
* Like strcat, but dest is assumed to be reallocatable.
*/
char* JE_AstrCat(char* dest, const char* src) {
    if(dest) {
        size_t dlen = strlen(dest);
        size_t slen = strlen(src);

        dest = JE_Realloc(dest, 1, dlen+slen+1);
        snprintf(dest+dlen, slen+1, "%s", src);
    }
    else {
        dest = strdup(src);
    }

    return dest;
}

/**
* Like asprintf, but append to dest and return new memory.
*/
char* JE_AstrCatFormat(char* dest, const char* format, ...) {
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
    dest = JE_Realloc(dest, 1, dlen+slen+1);
    snprintf(dest+dlen, slen+1, "%s", src);

    /* free */
    free(src);

    return dest;
}

char* JE_CstrToQstr(const char* src) {
    char* dest = JE_Calloc(1, strlen(src) + 3);
    const char* cp = src;

    /* Opening quote */
    dest = JE_AstrCat(dest, "\"");

    /* Decode each character */
    while(*cp) {
        char buf[] = { *cp, '\0' };

        switch(*cp) {
            case '"':
            case '\\':
                dest = JE_AstrCat(dest, "\\");
                dest = JE_AstrCat(dest, buf);
                break;

            case '\b' : buf[0] = 'b'; dest = JE_AstrCat(dest, "\\"); dest = JE_AstrCat(dest, buf); break;
            case '\f' : buf[0] = 'f'; dest = JE_AstrCat(dest, "\\"); dest = JE_AstrCat(dest, buf); break;
            case '\n' : buf[0] = 'n'; dest = JE_AstrCat(dest, "\\"); dest = JE_AstrCat(dest, buf); break;
            case '\r' : buf[0] = 'r'; dest = JE_AstrCat(dest, "\\"); dest = JE_AstrCat(dest, buf); break;
            case '\t' : buf[0] = 't'; dest = JE_AstrCat(dest, "\\"); dest = JE_AstrCat(dest, buf); break;
            default   : dest = JE_AstrCat(dest, buf); break;
        }

        cp++;
    }

    /* Closing quote */
    dest = JE_AstrCat(dest, "\"");

    return dest;
}

char* JE_LocToAstr(JE_YYLTYPE* loc) {
    return JE_TokenToAstr(loc->first, loc->last->next);
}

char* JE_CstrAcat(const char* s1, const char* s2) {
    JE_LINE_ITER* iter1 = JE_LineIterNew(s1);
    JE_LINE_ITER* iter2 = JE_LineIterNew(s2);
    size_t width1 = JE_CstrGetColumns(s1);
    size_t width2 = JE_CstrGetColumns(s2);
    size_t height1 = JE_CstrGetLines(s1);
    size_t height2 = JE_CstrGetLines(s2);
    char* result = JE_Calloc(1, (width1+width2+1)*(height1+height2)+1);
    char* rend = result;

    while(1) {
        const char* line1 = JE_LineIterNext(iter1);
        const char* line2 = JE_LineIterNext(iter2);

        if(!line1 && !line2) break;
        if(!line1) line1 = "";
        if(!line2) line2 = "";

        rend += snprintf(rend, width1+strlen(line2)+2, "%-*s%s\n", (int)width1, line1, line2);
    }

    /* Remove last newline */
    rend[-1] = '\0';

    return result;
}

size_t JE_CstrGetLines(const char* s) {
    JE_LINE_ITER* iter = JE_LineIterNew(s);
    size_t nlines = 0;

    while(JE_LineIterNext(iter)) nlines++;
    JE_LineIterDelete(iter);

    return nlines;
}

size_t JE_CstrGetColumns(const char* s) {
    JE_LINE_ITER* iter = JE_LineIterNew(s);
    const char* line = NULL;
    size_t maxwidth = 0;

    while((line = JE_LineIterNext(iter))) {
        maxwidth = _MAX(maxwidth, strlen(line));
    }

    JE_LineIterDelete(iter);

    return maxwidth;
}

JE_LINE_ITER* JE_LineIterNew(const char* s) {
    JE_LINE_ITER* iter = JE_Calloc(1, sizeof(JE_LINE_ITER));

    iter->next = s;
    iter->line = NULL;

    return iter;
}

const char* JE_LineIterNext(JE_LINE_ITER* iter) {
    const char* line = NULL;

    if(iter->next) {
        const char* next1 = iter->next;
        const char* next2 = strchr(next1, '\n') ? strchr(next1, '\n') : strchr(next1, '\0');
        size_t len = next2-next1;

        /* Copy text */
        iter->line = JE_Realloc(iter->line, 1, len+1);
        snprintf(iter->line, len+1, "%s", next1);

        /* Setup next */
        if(strchr(next1, '\n')) iter->next = next2+1;
        else iter->next = NULL;

        line = iter->line;
    }

    return line;
}

void JE_LineIterDelete(JE_LINE_ITER* iter) {
    if(iter->line) free(iter->line);

    iter->next = NULL;
    iter->line = NULL;

    free(iter);
}
