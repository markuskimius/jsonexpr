#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_str.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define F64_MAXDECPREC  17  /* maximum decimal precisions representable by f64 */
#define F64_MAXDECLEN   345 /* maximum decimal string length possible by f64 = -0.<324 zeros><17 digits>\0 */


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Like malloc() but guarantees the returned pointer has the requested memory.
* If the memory cannot be allocated, the program exits.
*/
void* JE_Malloc(size_t size) {
    void* ptr = malloc(size);

    if(!ptr && size) {
        perror("JE_Malloc");
        exit(1);
    }

    return ptr;
}

/**
* Like calloc() but guarantees the returned pointer has the requested memory.
* If the memory cannot be allocated, the program exits.
*/
void* JE_Calloc(size_t nmemb, size_t size) {
    void* ptr = calloc(nmemb, size);

    if(!ptr && size) {
        perror("JE_Calloc");
        exit(1);
    }

    return ptr;
}

/**
* Like realloc() but guarantees the returned pointer has the requested memory.
* If the memory cannot be allocated, the program exits.
*/
void* JE_Realloc(void *ptr, size_t size) {
    ptr = realloc(ptr, size);

    if(!ptr && size) {
        perror("JE_Realloc");
        exit(1);
    }

    return ptr;
}

/**
* Like strdup() but guarantees the returned pointer has the requested memory.
* If the memory cannot be allocated, the program exits.
*/
char* JE_Strdup(const char* s) {
    char* dup = strdup(s);

    if(!dup) {
        perror("JE_Strdup");
        exit(1);
    }

    return dup;
}

/**
* Like free() but NUL-terminates the memory before freeing.
*
* Zeroing out the entire allocated memory before freeing helps the program fail
* fast, but we don't actually know the size of the entire memory block here
* and, even if we did, zeroing out the entire memory block could be slow.  So
* we zero out just the first byte of the memory block instead as a low-overhead
* compromise to handle the subset of cases where it could be useful for failing
* fast.
*/
void JE_Free(void* ptr) {
    uint8_t* ptr8 = ptr;

    if(ptr) {
        *ptr8 = 0;
        free(ptr);
    }
}

int64_t JE_BinToI64(const char* s) {
    int64_t i64 = 0;

    while(1) {
        switch(*s++) {
            case '0' : i64 = (i64 << 1) | 0x0; continue;
            case '1' : i64 = (i64 << 1) | 0x1; continue;
            default  : break;
        }

        break;
    }

    return i64;
}

int64_t JE_OctToI64(const char* s) {
    int64_t i64 = 0;

    while(1) {
        switch(*s++) {
            case '0' : i64 = (i64 << 3) | 0x0; continue;
            case '1' : i64 = (i64 << 3) | 0x1; continue;
            case '2' : i64 = (i64 << 3) | 0x2; continue;
            case '3' : i64 = (i64 << 3) | 0x3; continue;
            case '4' : i64 = (i64 << 3) | 0x4; continue;
            case '5' : i64 = (i64 << 3) | 0x5; continue;
            case '6' : i64 = (i64 << 3) | 0x6; continue;
            case '7' : i64 = (i64 << 3) | 0x7; continue;
            default  : break;
        }

        break;
    }

    return i64;
}

int64_t JE_HexToI64(const char* s) {
    int64_t i64 = 0;

    while(1) {
        switch(*s++) {
            case '0' : i64 = (i64 << 4) | 0x0; continue;
            case '1' : i64 = (i64 << 4) | 0x1; continue;
            case '2' : i64 = (i64 << 4) | 0x2; continue;
            case '3' : i64 = (i64 << 4) | 0x3; continue;
            case '4' : i64 = (i64 << 4) | 0x4; continue;
            case '5' : i64 = (i64 << 4) | 0x5; continue;
            case '6' : i64 = (i64 << 4) | 0x6; continue;
            case '7' : i64 = (i64 << 4) | 0x7; continue;
            case '8' : i64 = (i64 << 4) | 0x8; continue;
            case '9' : i64 = (i64 << 4) | 0x9; continue;
            case 'a' : case 'A' : i64 = (i64 << 4) | 0xA; continue;
            case 'b' : case 'B' : i64 = (i64 << 4) | 0xB; continue;
            case 'c' : case 'C' : i64 = (i64 << 4) | 0xC; continue;
            case 'd' : case 'D' : i64 = (i64 << 4) | 0xD; continue;
            case 'e' : case 'E' : i64 = (i64 << 4) | 0xE; continue;
            case 'f' : case 'F' : i64 = (i64 << 4) | 0xF; continue;
            default  : break;
        }

        break;
    }

    return i64;
}

/**
* Convert a double to a C string with full precision.  The returned C string
* must be freed.
*/
char* JE_F64ToCstr(double f64) {
    uint64_t u64 = * (uint64_t*) &f64;

    if     (0x0000000000000000 == u64                             ) return JE_Strdup("0.0");
    else if(0x8000000000000000 == u64                             ) return JE_Strdup("-0.0");
    else if(0x7ff0000000000000 == u64                             ) return JE_Strdup("inf");
    else if(0xfff0000000000000 == u64                             ) return JE_Strdup("-inf");
    else if(0x7ff0000000000001 <= u64 && u64 <= 0x7fffffffffffffff) return JE_Strdup("nan");
    else {
        char* buf = JE_Malloc(F64_MAXDECLEN);
        double e = 1e308;
        int i = 0;  /* next buf index */
        int n = 0;  /* # of significant decimal digits printed */

        /* Sign */
        if(f64 < 0.0) {
            buf[i++] = '-';
            f64 *= -1.0;
        }

        /* Numeral */
        for(int e=308; e>=0; e--) {
            double p = pow(10.0, e);
            int digit = f64 / pow(10.0, e);

            if(digit || n) n++;

            if(n) {
                buf[i++] = digit + '0';

                f64 -= digit * p;
            }
        }

        /* . */
        if(!n) buf[i++] = '0';
        buf[i++] = '.';
        if(!f64 || F64_MAXDECPREC<n || F64_MAXDECLEN-1<i) buf[i++] = '0';

        /* Decimal */
        while(f64 && n<F64_MAXDECPREC && i<F64_MAXDECLEN-1) {
            int digit = (f64 *= 10.0);

            if(digit || n) n++;

            buf[i++] = digit + '0';
            f64 -= digit;
        }

        /* Terminator */
        buf[i] = '\0';

        return buf;
    }
}

/**
* Convert a double to a quoted C string with full precision.  The returned
* quoted C string must be freed.
*/
char* JE_F64ToQstr(double f64) {
    char* qstr = JE_F64ToCstr(f64);

    if(strcmp(qstr, "inf")==0 || strcmp(qstr, "-inf")==0 || strcmp(qstr, "nan")==0) {
        size_t len = strlen(qstr);

        qstr = JE_Realloc(qstr, len+3);
        memmove(qstr+1, qstr, len);

        qstr[0] = '"';
        qstr[len+1] = '"';
        qstr[len+2] = '\0';
    }

    return qstr;
}
