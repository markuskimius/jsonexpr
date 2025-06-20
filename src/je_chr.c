#include "je_chr.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_CHR JE_ChrCreate(uint32_t u32) {
    JE_CHR value;
    char* cp = &value.chr[0];

    if     (u32 <   0x0080) { value.length=1; *cp++ = u32; }
    else if(u32 <   0x0800) { value.length=2; *cp++ = (u32 >>  6) | 0xc0;                                                                          *cp++ = (u32 & 0x3f) | 0x80; }
    else if(u32 <  0x10000) { value.length=3; *cp++ = (u32 >> 12) | 0xe0;                                      *cp++ = ((u32 >> 6) & 0x3f) | 0x80; *cp++ = (u32 & 0x3f) | 0x80; }
    else if(u32 < 0x110000) { value.length=4; *cp++ = (u32 >> 18) | 0xf0; *cp++ = ((u32 >> 12) & 0x3f) | 0x80; *cp++ = ((u32 >> 6) & 0x3f) | 0x80; *cp++ = (u32 & 0x3f) | 0x80; }
    else                    { value.length=4; *cp++ = 0xf4; *cp++ = 0x8f; *cp++ = 0xbf; *cp++ = 0xbf; }

    *cp = '\0';

    return value;
}
