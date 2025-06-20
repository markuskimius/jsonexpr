#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "je_chr.h"
#include "je_str.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _MINSIZE 32


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_STR, optionally from a C string.  The passed C string, if
* any, must have been dynamically allocated, and may not be operated on after
* it's been passed to this function.  NULL can be passed instead of the C
* string to create a JE_STR with no data.
*
* Unlike a C string, JE_STR may contain '\0', so it can be used to store binary
* data.  However, when interpreting the data as string (e.g., when converting
* the data to/from a C string), it is assumed to be encoded in UTF-8.
*/
JE_STR JE_StrCreate(char* cstr) {
    JE_STR str;

    str.data = cstr ? cstr : JE_Calloc(1, _MINSIZE);
    str.length = cstr ? strlen(cstr) : 0;

    return str;
}

/**
* Return a clone of JE_STR.
*/
JE_STR JE_StrClone(const JE_STR str) {
    JE_STR dup;

    dup.data = str.length < _MINSIZE-1 ? JE_Malloc(_MINSIZE) : JE_Malloc(str.length+1);
    dup.length = str.length;

    if(str.length) memcpy(dup.data, str.data, str.length);
    dup.data[dup.length] = '\0';

    return dup;
}

/**
* Concatenate two JE_STR's, return a new JE_STR.
*/
JE_STR JE_StrPlusStr(const JE_STR x, const JE_STR y) {
    size_t length = x.length + y.length;
    JE_STR str;

    str.data = length < _MINSIZE-1 ? JE_Malloc(_MINSIZE) : JE_Malloc(length+1);

    if(x.length) memcpy(str.data, x.data, x.length);
    if(y.length) memcpy(str.data + x.length, y.data, y.length);

    str.data[length] = '\0';
    str.length = length;

    return str;
}

/**
* Compare x to y.  Return a value less than, equal to, or greater than 0 if
* x is less than, equal to, or greater than y, respectively.
*/
int64_t JE_StrCompare(const JE_STR x, const JE_STR y) {
    size_t length = x.length < y.length ? x.length : y.length;
    uint8_t* xp = x.data;
    uint8_t* yp = y.data;

    for(size_t i=0; i<length; i++) {
        int cmp = *xp++ - *yp++;

        if(cmp) return cmp;
    }

    return (ssize_t)x.length - (ssize_t)y.length;
}

/**
* Push y to the end of x.
*/
void JE_StrPushStr(JE_STR* x, const JE_STR y) {
    size_t length = x->length + y.length;

    x->data = JE_Realloc(x->data, length+1);
    if(y.length) memcpy(x->data + x->length, y.data, y.length);

    x->data[length] = '\0';
    x->length = length;
}

/**
* Push y to the end of x.
*/
void JE_StrPushCstr(JE_STR* x, const char* y) {
    size_t ylength = strlen(y);
    size_t length = x->length + ylength;

    x->data = JE_Realloc(x->data, length+1);
    if(ylength) memcpy(x->data + x->length, y, ylength);

    x->data[length] = '\0';
    x->length = length;
}

/**
* Push y to the end of x.
*/
void JE_StrPushChr(JE_STR* x, const JE_CHR y) {
    size_t length = x->length + y.length;

    x->data = JE_Realloc(x->data, length+1);
    if(y.length) memcpy(x->data + x->length, y.chr, y.length);

    x->data[length] = '\0';
    x->length = length;
}

/**
* Destroy the JE_STR.
*/
void JE_StrDestroy(JE_STR* str) {
    JE_Free(str->data);
    memset(str, 0, sizeof(JE_STR));
}

/**
* Return the UTF-8 code point length of JE_STR.
*/
size_t JE_StrLength(const JE_STR str) {
    uint8_t* cp = str.data;
    size_t count = str.length;
    size_t length = 0;

    while(count--) {
        /*
        * From https://stackoverflow.com/a/32936928/20025913:
        */
        length += (*cp++ & 0xC0) != 0x80;
    }

    return length;
}

/**
* Convert JE_STR to an int64_t.  This is done by assuming JE_STR contain an
* UTF-8 string representation of an integer.
*/
int64_t JE_StrToI64(const JE_STR str) {
    int64_t i64 = 0;

    if(str.data) {
        i64 = atoi((char*) str.data);
    }

    return i64;
}

/**
* Convert JE_STR to a double.  This is done by assuming JE_STR contain an UTF-8
* string representation of a floating point number.
*/
double JE_StrToF64(const JE_STR str) {
    double f64 = 0.0;

    if(str.data) {
        f64 = atof((char*) str.data);
    }

    return f64;
}

/**
* Convert JE_STR to a C string.  The returned C string must be freed.
*/
char* JE_StrToCstr(const JE_STR str) {
    char* cstr = str.length < _MINSIZE-1 ? JE_Malloc(_MINSIZE) : JE_Malloc(str.length+1);

    if(str.length) memcpy(cstr, str.data, str.length);
    cstr[str.length] = '\0';

    return cstr;
}

/**
* Convert JE_STR to a quoted C string.  The returned quoted C string must be
* freed.
*/
char* JE_StrToQstr(const JE_STR str) {
    size_t capacity = str.length < _MINSIZE-1 ? _MINSIZE : str.length+1;
    char* qstr = JE_Malloc(capacity);
    uint8_t* sp = str.data;
    size_t qi = 0;

    if(capacity < qi) capacity*=2;
    qstr[qi++] = '"';

    for(size_t si=0; si<str.length; si++) {
        if(qi+6 > capacity) qstr = JE_Realloc(qstr, capacity*=2);

        switch((char) *sp) {
            case '"'    : qstr[qi++] = '\\'; qstr[qi++] = '"'; break;
            case '\\'   : qstr[qi++] = '\\'; qstr[qi++] = '\\'; break;
            case '\b'   : qstr[qi++] = '\\'; qstr[qi++] = 'b'; break;
            case '\f'   : qstr[qi++] = '\\'; qstr[qi++] = 'f'; break;
            case '\n'   : qstr[qi++] = '\\'; qstr[qi++] = 'n'; break;
            case '\r'   : qstr[qi++] = '\\'; qstr[qi++] = 'r'; break;
            case '\t'   : qstr[qi++] = '\\'; qstr[qi++] = 't'; break;
            case '\0'   : qstr[qi++] = '\\'; qstr[qi++] = 'u'; qstr[qi++] = '0'; qstr[qi++] = '0'; qstr[qi++] = '0'; qstr[qi++] = '0'; break;
            default     : {
                if(*sp < 0x20) {
                    char d1 = ((*sp) >> 4) & 0xf;
                    char d0 = ((*sp)     ) & 0xf;

                    qstr[qi++] = '\\';
                    qstr[qi++] = 'u';
                    qstr[qi++] = '0';
                    qstr[qi++] = '0';
                    qstr[qi++] = (d1 < 10) ? ('0' + d1) : ('A' + d1-10);
                    qstr[qi++] = (d0 < 10) ? ('0' + d0) : ('A' + d0-10);
                }
                else {
                    qstr[qi++] = *sp;
                }
            }
        }

        sp++;
    }

    /* nul terminate the string */
    if(qi+2 > capacity) qstr = JE_Realloc(qstr, capacity+=2);
    qstr[qi++] = '"';
    qstr[qi++] = '\0';

    return qstr;
}
