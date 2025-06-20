#ifndef JE_SYMBOLS_H_
#define JE_SYMBOLS_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* TYPES
*/

#define JE_SYMBOLS_CONST     0x0001
#define JE_SYMBOLS_IO        0x0002
#define JE_SYMBOLS_LINEAR    0x0004
#define JE_SYMBOLS_LOOPABLE  0x0010
#define JE_SYMBOLS_SYSTEM    0x0100

#define JE_SYMBOLS_SAFE      0x00ff
#define JE_SYMBOLS_DEFAULT   0xffff


/* ***************************************************************************
* FUNCTIONS
*/

JE_MAP JE_Symbols(int set);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_SYMBOLS_H_ */
