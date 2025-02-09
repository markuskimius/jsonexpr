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

#define JE_Die(args...) _JE_Die(__FILE__, __FUNCTION__, __LINE__, args)
#define JE_LogicError(args...) JE_Throwx(__FILE__, __FUNCTION__, __LINE__, "JE_LogicError", args)
#define JE_ParseError(args...) JE_Throw("JE_ParseError", args)
#define JE_RuntimeError(args...) JE_Throw("JE_RuntimeError", args)


/* ***************************************************************************
* GLOBALS
*/

extern char* JE_throwText;


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

void _JE_Die(const char* file, const char* func, size_t line, const char* format, ...);
void JE_Throw(const char* type, JE_YYLTYPE* loc, const char* format, ...);
void JE_Throwx(const char* file, const char* func, size_t line, const char* type, JE_YYLTYPE* loc, const char* format, ...);
void JE_ThrowLater(const char* format, ...);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_THROW_H_ */
