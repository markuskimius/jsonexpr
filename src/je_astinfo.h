#ifndef JE_ASTINFO_H_
#define JE_ASTINFO_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_TOKEN JE_TOKEN;
typedef struct JE_YYLTYPE JE_YYLTYPE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_ASTINFO {
    JE_YYLTYPE* tokenmap;
    size_t mlen;

    JE_TOKEN* token;
    size_t tlen;
} JE_ASTINFO;


/* ***************************************************************************
* FUNCTIONS
*/

JE_ASTINFO JE_AstInfoCreate();
void JE_AstInfoDestroy(JE_ASTINFO* info);

void JE_AstInfoPushLoc1(JE_ASTINFO* info, JE_YYLTYPE loc1);
void JE_AstInfoPushLoc2(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2);
void JE_AstInfoPushLoc3(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3);
void JE_AstInfoPushLoc4(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4);
void JE_AstInfoPushLoc5(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4, JE_YYLTYPE loc5);

JE_TOKEN* JE_AstInfoPushToken(JE_ASTINFO* info, JE_TOKEN token);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_ASTINFO_H_ */
