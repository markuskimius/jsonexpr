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

#define IBUFSIZE 32
#define DBUFSIZE 320


/* ***************************************************************************
* MACROS
*/

#define JE_MAX(a,b) (a>b?a:b)
#define JE_MIN(a,b) (a<b?a:b)


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
char* je_utf8str(uint32_t c) {
    char* cp = utf8buf;

    if     (c <   0x0080) { *cp++ = c; }
    else if(c <   0x0800) { *cp++ = (c >>  6) | 0xc0;                                                                      *cp++ = (c & 0x3f) | 0x80; }
    else if(c <  0x10000) { *cp++ = (c >> 12) | 0xe0;                                    *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else if(c < 0x110000) { *cp++ = (c >> 18) | 0xf0; *cp++ = ((c >> 12) & 0x3f) | 0x80; *cp++ = ((c >> 6) & 0x3f) | 0x80; *cp++ = (c & 0x3f) | 0x80; }
    else                  { *cp++ = 0xf4; *cp++ = 0x8f; *cp++ = 0xbf; *cp++ = 0xbf; }

    *cp = '\0';

    return utf8buf;
}


char* je_astri64(int64_t src) {
    char* dest = calloc(1, IBUFSIZE);

    snprintf(dest, IBUFSIZE, "%lld", src);

    return dest;
}


char* je_astrf64(double src) {
    char* dest = calloc(1, DBUFSIZE);

    snprintf(dest, DBUFSIZE, "%lf", src);

    return dest;
}


/**
* Like strcat, but dest is assumed to be reallocatable.
*/
char* je_astrcat(char* dest, const char* src) {
    if(dest) {
        size_t dlen = strlen(dest);
        size_t slen = strlen(src);

        dest = realloc(dest, dlen+slen+1);
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
char* je_casprintf(char* dest, const char* format, ...) {
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


char* je_astrencode(const char* src) {
    char* dest = calloc(1, strlen(src) + 3);
    const char* cp = src;

    /* Opening quote */
    dest = je_astrcat(dest, "\"");

    /* Decode each character */
    while(*cp) {
        char buf[] = { *cp, '\0' };

        switch(*cp) {
            case '"':
            case '\\':
                dest = je_astrcat(dest, "\\");
                dest = je_astrcat(dest, buf);
                break;

            case '\b' : buf[0] = 'b'; dest = je_astrcat(dest, "\\"); dest = je_astrcat(dest, buf); break;
            case '\f' : buf[0] = 'f'; dest = je_astrcat(dest, "\\"); dest = je_astrcat(dest, buf); break;
            case '\n' : buf[0] = 'n'; dest = je_astrcat(dest, "\\"); dest = je_astrcat(dest, buf); break;
            case '\r' : buf[0] = 'r'; dest = je_astrcat(dest, "\\"); dest = je_astrcat(dest, buf); break;
            case '\t' : buf[0] = 't'; dest = je_astrcat(dest, "\\"); dest = je_astrcat(dest, buf); break;
            default   : dest = je_astrcat(dest, buf); break;
        }

        cp++;
    }

    /* Closing quote */
    dest = je_astrcat(dest, "\"");

    return dest;
}


char* je_atextat(JE_YYLTYPE* loc) {
    return je_astrtoken(loc->first, loc->last->next);
}


static void je_reprint(char* where, size_t howmany, const char* what) {
    size_t len = strlen(what);

    for(size_t i=0; i<howmany; i++) {
        snprintf(where, len, "%s", what);
        where += len;
    }
}


char* je_amcat(const char* s1, const char* s2) {
    JE_LINE_ITER* iter1 = je_newlineiter(s1);
    JE_LINE_ITER* iter2 = je_newlineiter(s2);
    size_t width1 = je_maxwidth(s1);
    size_t width2 = je_maxwidth(s2);
    size_t height1 = je_nlines(s1);
    size_t height2 = je_nlines(s2);
    char* result = calloc(1, (width1+width2+1)*(height1+height2)+1);
    char* rend = result;

    while(1) {
        const char* line1 = je_nextline(iter1);
        const char* line2 = je_nextline(iter2);

        if(!line1 && !line2) break;
        if(!line1) line1 = "";
        if(!line2) line2 = "";

        rend += snprintf(rend, width1+strlen(line2)+2, "%-*s%s\n", (int)width1, line1, line2);
    }

    /* Remove last newline */
    rend[-1] = '\0';

    return result;
}


size_t je_nlines(const char* s) {
    JE_LINE_ITER* iter = je_newlineiter(s);
    size_t nlines = 0;

    while(je_nextline(iter)) nlines++;
    je_freelineiter(iter);

    return nlines;
}


size_t je_maxwidth(const char* s) {
    JE_LINE_ITER* iter = je_newlineiter(s);
    const char* line = NULL;
    size_t maxwidth = 0;

    while((line = je_nextline(iter))) {
        maxwidth = JE_MAX(maxwidth, strlen(line));
    }

    je_freelineiter(iter);

    return maxwidth;
}


JE_LINE_ITER* je_newlineiter(const char* s) {
    JE_LINE_ITER* iter = calloc(1, sizeof(JE_LINE_ITER));

    iter->next = s;
    iter->line = NULL;

    return iter;
}


const char* je_nextline(JE_LINE_ITER* iter) {
    const char* line = NULL;

    if(iter->next) {
        const char* next1 = iter->next;
        const char* next2 = strchr(next1, '\n') ? strchr(next1, '\n') : strchr(next1, '\0');
        size_t len = next2-next1;

        /* Copy text */
        iter->line = realloc(iter->line, len+1);
        snprintf(iter->line, len+1, "%s", next1);

        /* Setup next */
        if(strchr(next1, '\n')) iter->next = next2+1;
        else iter->next = NULL;

        line = iter->line;
    }

    return line;
}


void je_freelineiter(JE_LINE_ITER* iter) {
    if(iter->line) free(iter->line);

    iter->next = NULL;
    iter->line = NULL;

    free(iter);
}
