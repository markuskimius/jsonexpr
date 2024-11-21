#ifndef JE_THROW_H_
#define JE_THROW_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_YYLTYPE JE_YYLTYPE;


/* ***************************************************************************
* MACROS
*/

#define je_die(args...) _je_die(__FILE__, __FUNCTION__, __LINE__, args)
#define JeLogicError(args...) je_throwx(__FILE__, __FUNCTION__, __LINE__, "JeLogicError", args)
#define JeParseError(args...) je_throw("JeParseError", args)
#define JeRuntimeError(args...) je_throw("JeRuntimeError", args)


/* ***************************************************************************
* GLOBALS
*/

extern char* je_throwText;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _je_die(const char* file, const char* func, size_t line, const char* format, ...);
void je_throw(const char* type, JE_YYLTYPE* loc, const char* format, ...);
void je_throwx(const char* file, const char* func, size_t line, const char* type, JE_YYLTYPE* loc, const char* format, ...);
void je_throwLater(const char* format, ...);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_THROW_H_ */
