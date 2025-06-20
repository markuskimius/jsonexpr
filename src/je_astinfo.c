#include <string.h>
#include "je_astinfo.h"
#include "je_token.h"
#include "je_util.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_ASTINFO.
*/
JE_ASTINFO JE_AstInfoCreate() {
    JE_ASTINFO info;

    memset(&info, 0, sizeof(info));

    return info;
}

/**
* Destroy JE_ASTINFO.
*/
void JE_AstInfoDestroy(JE_ASTINFO* info) {
    if(info->tokenmap) JE_Free(info->tokenmap);
    if(info->token) {
        for(size_t i=0; i<info->tlen; i++) JE_TokenDestroy(&info->token[i]);
        JE_Free(info->token);
    }

    memset(info, 0, sizeof(JE_ASTINFO));
}

/**
* Push 1 location to the end of ast.
*
* Once pushed into ast, location may no longer be operated on by the caller.
*/
void JE_AstInfoPushLoc1(JE_ASTINFO* info, JE_YYLTYPE loc1) {
    info->tokenmap = JE_Realloc(info->tokenmap, sizeof(JE_YYLTYPE) * (info->mlen+1));
    info->tokenmap[info->mlen++] = loc1;
}

/**
* Push 2 locations to the end of ast.
*
* Once pushed into ast, locations may no longer be operated on by the caller.
*/
void JE_AstInfoPushLoc2(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    info->tokenmap = JE_Realloc(info->tokenmap, sizeof(JE_YYLTYPE) * (info->mlen+2));
    info->tokenmap[info->mlen++] = loc1;
    info->tokenmap[info->mlen++] = loc2;
}

/**
* Push 3 locations to the end of ast.
*
* Once pushed into ast, locations may no longer be operated on by the caller.
*/
void JE_AstInfoPushLoc3(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    info->tokenmap = JE_Realloc(info->tokenmap, sizeof(JE_YYLTYPE) * (info->mlen+3));
    info->tokenmap[info->mlen++] = loc1;
    info->tokenmap[info->mlen++] = loc2;
    info->tokenmap[info->mlen++] = loc3;
}

/**
* Push 4 locations to the end of ast.
*
* Once pushed into ast, locations may no longer be operated on by the caller.
*/
void JE_AstInfoPushLoc4(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4) {
    info->tokenmap = JE_Realloc(info->tokenmap, sizeof(JE_YYLTYPE) * (info->mlen+4));
    info->tokenmap[info->mlen++] = loc1;
    info->tokenmap[info->mlen++] = loc2;
    info->tokenmap[info->mlen++] = loc3;
    info->tokenmap[info->mlen++] = loc4;
}

/**
* Push 5 locations to the end of ast.
*
* Once pushed into ast, locations may no longer be operated on by the caller.
*/
void JE_AstInfoPushLoc5(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4, JE_YYLTYPE loc5) {
    info->tokenmap = JE_Realloc(info->tokenmap, sizeof(JE_YYLTYPE) * (info->mlen+5));
    info->tokenmap[info->mlen++] = loc1;
    info->tokenmap[info->mlen++] = loc2;
    info->tokenmap[info->mlen++] = loc3;
    info->tokenmap[info->mlen++] = loc4;
    info->tokenmap[info->mlen++] = loc5;
}

/**
* Push token to the end of info.
*
* Once pushed into info, token may no longer be operated on by the caller.  If it
* needs to be modified after the push, modify the returned pointer instead.
*/
JE_TOKEN* JE_AstInfoPushToken(JE_ASTINFO* info, JE_TOKEN token) {
    info->token = JE_Realloc(info->token, sizeof(JE_TOKEN) * (info->tlen+1));
    info->token[info->tlen] = token;

    return &info->token[info->tlen++];
}
