#ifndef JE_UTIL_H_
#define JE_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FUNCTIONS
*/

void* JE_Malloc(size_t size);
void* JE_Calloc(size_t nmemb, size_t size);
void* JE_Realloc(void *ptr, size_t size);
char* JE_Strdup(const char* s);
void JE_Free(void* ptr);

int64_t JE_BinToI64(const char* s);
int64_t JE_OctToI64(const char* s);
int64_t JE_HexToI64(const char* s);

char* JE_F64ToCstr(double f64);
char* JE_F64ToQstr(double f64);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_UTIL_H_ */
