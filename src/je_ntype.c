#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "je_ast.h"
#include "je_error.h"
#include "je_map.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_symmap.h"
#include "je_token.h"
#include "je_type.h"
#include "je_util.h"
#include "je_vec.h"


/* ***************************************************************************
* PRIVATE FUNCTIONS - UNROLL
*/

static JE_VEC _UnrollList(const JE_NODE* node, const JE_ASTINFO* info) {
    const JE_NODE* left = node + 1;
    JE_VEC vec = JE_VecCreate(1);

    JE_VecPush(&vec, JE_ValueNod(JE_NodCreate(left, info)));

    return vec;
}

static JE_VEC _UnrollList2(const JE_NODE* node, const JE_ASTINFO* info) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VEC vec;

    switch(JE_NCALL(left,getType)) {
        case JE_LIST_NT  : vec = _UnrollList(left, info); break;
        case JE_LIST2_NT : vec = _UnrollList2(left, info); break;
        default          :
            JE_InternalError(JE_NCALL(left,tokenBegin,info), JE_NCALL(left,tokenEnd,info), "Invalid token type %s: Expected LIST_N or LIST2_N", JE_NCALL(left,getTypeCstr));
            vec = JE_VecCreate(0);
            break;
    }

    JE_VecPush(&vec, JE_ValueNod(JE_NodCreate(right, info)));

    return vec;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ARRAY
*/

static void _ArrayDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ArrayRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ArrayEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_MAP* subscope = JE_SymmapCreateScope(symmap);
    JE_VALUE array = JE_ValueVec(JE_VecCreate(0));

    /* Build the array with the array as THIS */
    JE_MapSet(subscope, JE_StrCreate(JE_Strdup("THIS")), JE_VCALL(&array,clone));
    JE_NCALL(left,eval,subscope,info,&array);

    /* Destroy the subscope */
    JE_SymmapDestroy(subscope);

    return array;
}

static JE_NTYPE_T _ArrayGetType(const JE_NODE* node) {
    return JE_ARRAY_NT;
}

static const char* _ArrayGetTypeCstr(const JE_NODE* node) {
    return "ARRAY_N";
}

static JE_TOKEN* _ArrayTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ArrayTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ARRAY2
*/

static void _Array2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Array2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Array2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    return _ArrayEval(node, symmap, info, this);
}

static JE_NTYPE_T _Array2GetType(const JE_NODE* node) {
    return JE_ARRAY2_NT;
}

static const char* _Array2GetTypeCstr(const JE_NODE* node) {
    return "ARRAY2_N";
}

static JE_TOKEN* _Array2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Array2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+3];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASR
*/

static void _AsrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AsrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AsrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,asr,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _AsrGetType(const JE_NODE* node) {
    return JE_ASR_NT;
}

static const char* _AsrGetTypeCstr(const JE_NODE* node) {
    return "ASR_N";
}

static JE_TOKEN* _AsrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AsrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSN
*/

static void _AssnDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,1,symmap,info, &rlvalue);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result;

    JE_VCALL(lvalue,destroy);           /* Destroy the value lvalue points to */
    *lvalue = rvalue;                   /* Replace the value lvalue points to with rvalue */
    result = JE_VCALL(lvalue,clone);    /* Clone the new lvalue */

    JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */

    return result;
}

static JE_NTYPE_T _AssnGetType(const JE_NODE* node) {
    return JE_ASSN_NT;
}

static const char* _AssnGetTypeCstr(const JE_NODE* node) {
    return "ASSN_N";
}

static JE_TOKEN* _AssnTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - BITWISEAND
*/

static void _BitwiseAndDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _BitwiseAndRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _BitwiseAndEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,and,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _BitwiseAndGetType(const JE_NODE* node) {
    return JE_BITWISEAND_NT;
}

static const char* _BitwiseAndGetTypeCstr(const JE_NODE* node) {
    return "BITWISEAND_N";
}

static JE_TOKEN* _BitwiseAndTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _BitwiseAndTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - BITWISEINV
*/

static void _BitwiseInvDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _BitwiseInvRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _BitwiseInvEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE value = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE negvalue = JE_VCALL(&value,inv);

    JE_VCALL(&value,destroy);

    return negvalue;
}

static JE_NTYPE_T _BitwiseInvGetType(const JE_NODE* node) {
    return JE_BITWISEINV_NT;
}

static const char* _BitwiseInvGetTypeCstr(const JE_NODE* node) {
    return "BITWISEINV_N";
}

static JE_TOKEN* _BitwiseInvTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _BitwiseInvTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - BITWISEOR
*/

static void _BitwiseOrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _BitwiseOrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _BitwiseOrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,or,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _BitwiseOrGetType(const JE_NODE* node) {
    return JE_BITWISEOR_NT;
}

static const char* _BitwiseOrGetTypeCstr(const JE_NODE* node) {
    return "BITWISEOR_N";
}

static JE_TOKEN* _BitwiseOrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _BitwiseOrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - BITWISEXOR
*/

static void _BitwiseXorDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _BitwiseXorRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _BitwiseXorEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,xor,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _BitwiseXorGetType(const JE_NODE* node) {
    return JE_BITWISEXOR_NT;
}

static const char* _BitwiseXorGetTypeCstr(const JE_NODE* node) {
    return "BITWISEXOR_N";
}

static JE_TOKEN* _BitwiseXorTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _BitwiseXorTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - CALL
*/

static void _CallDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _CallRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _CallEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VEC args = JE_VecCreate(0);
    JE_VALUE result = JE_VCALL(&lvalue,call,args,node,symmap,info);

    JE_VecDestroy(&args);
    JE_VCALL(&lvalue,destroy);

    return result;
}

static JE_NTYPE_T _CallGetType(const JE_NODE* node) {
    return JE_CALL_NT;
}

static const char* _CallGetTypeCstr(const JE_NODE* node) {
    return "CALL_N";
}

static JE_TOKEN* _CallTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _CallTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - CALL2
*/

static void _Call2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Call2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Call2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VEC args = right->ntype==&JE_ntypeList ? _UnrollList(right, info) : _UnrollList2(right, info);
    JE_VALUE result = JE_VCALL(&lvalue,call,args,node,symmap,info);

    JE_VecDestroy(&args);
    JE_VCALL(&lvalue,destroy);

    return result;
}

static JE_NTYPE_T _Call2GetType(const JE_NODE* node) {
    return JE_CALL2_NT;
}

static const char* _Call2GetTypeCstr(const JE_NODE* node) {
    return "CALL2_N";
}

static JE_TOKEN* _Call2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Call2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+3];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - DIVBY
*/

static void _DivbyDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _DivbyRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _DivbyEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,divby,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _DivbyGetType(const JE_NODE* node) {
    return JE_DIVBY_NT;
}

static const char* _DivbyGetTypeCstr(const JE_NODE* node) {
    return "DIVBY_N";
}

static JE_TOKEN* _DivbyTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _DivbyTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - EQUAL
*/

static void _EqualDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _EqualRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _EqualEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare == 0);
}

static JE_NTYPE_T _EqualGetType(const JE_NODE* node) {
    return JE_EQUAL_NT;
}

static const char* _EqualGetTypeCstr(const JE_NODE* node) {
    return "EQUAL_N";
}

static JE_TOKEN* _EqualTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _EqualTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - GREATER
*/

static void _GreaterDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _GreaterRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _GreaterEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare > 0);
}

static JE_NTYPE_T _GreaterGetType(const JE_NODE* node) {
    return JE_GREATER_NT;
}

static const char* _GreaterGetTypeCstr(const JE_NODE* node) {
    return "GREATER_N";
}

static JE_TOKEN* _GreaterTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _GreaterTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - GREATEREQ
*/

static void _GreaterEqDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _GreaterEqRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _GreaterEqEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare >= 0);
}

static JE_NTYPE_T _GreaterEqGetType(const JE_NODE* node) {
    return JE_GREATEREQ_NT;
}

static const char* _GreaterEqGetTypeCstr(const JE_NODE* node) {
    return "GREATEREQ_N";
}

static JE_TOKEN* _GreaterEqTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _GreaterEqTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - IDENT
*/

static void _IdentDestroy(JE_NODE* node) {
    JE_StrDestroy(&node->data.id);

    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _IdentRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    JE_VALUE* value = JE_SymmapGet(*symmap, node->data.id);

    if(!value && create) {
        value = JE_SymmapSet(symmap, JE_StrClone(node->data.id), JE_ValueNul(), 0);
    }

    *lvalue = JE_ValueNul();

    return value;
}

static JE_VALUE _IdentEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    JE_VALUE* value = JE_SymmapGet(*symmap, node->data.id);

    if(!value) {
        const JE_TOKEN* begin = JE_NCALL(node,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(node,tokenEnd,info);
        char* qstr = JE_StrToQstr(node->data.id);

        JE_RuntimeError(begin, end, "No such variable named %s in this scope", qstr);
        JE_Free(qstr);

        return JE_ValueNul();
    }

    return JE_VCALL(value,clone);
}

static JE_NTYPE_T _IdentGetType(const JE_NODE* node) {
    return JE_IDENT_NT;
}

static const char* _IdentGetTypeCstr(const JE_NODE* node) {
    return "IDENT_N";
}

static JE_TOKEN* _IdentTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _IdentTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - INDEX
*/

static void _IndexDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _IndexRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE* result = NULL;
    JE_VALUE rvalue;

    *lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    rvalue = JE_NCALL(right,eval,symmap,info,NULL);

    if(lvalue->type==&JE_typeMap && rvalue.type==&JE_typeStr) {
        JE_MAP* map = lvalue->data.map;
        JE_STR str = rvalue.data.str;

        result = JE_MapGet(*map, str);

        if(!result && create) {
            result = JE_MapSet(map, JE_StrClone(str), JE_ValueNul());
        }
    }
    else if(lvalue->type==&JE_typeVec && rvalue.type==&JE_typeI64) {
        JE_VEC* vec = lvalue->data.vec;
        int64_t i64 = rvalue.data.i64;

        if(i64 < 0) {
            const JE_TOKEN* rbegin = JE_NCALL(right,tokenBegin,info);
            const JE_TOKEN* rend = JE_NCALL(right,tokenEnd,info);

            JE_RuntimeError(rbegin, rend, "Array index must be non-negative, got %zu\n", i64);
        }
        else if(i64 < vec->length) {
            result = JE_VecGet(*vec, i64);
        }
        else if(create) {
            result = JE_VecSet(vec, i64, JE_ValueNul());
        }
    }
    else {
        const JE_TOKEN* lbegin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* lend = JE_NCALL(left,tokenEnd,info);
        const char* ltype = JE_VCALL(lvalue,getTypeCstr);

        const JE_TOKEN* rbegin = JE_NCALL(right,tokenBegin,info);
        const JE_TOKEN* rend = JE_NCALL(right,tokenEnd,info);
        const char* rtype = JE_VCALL(&rvalue,getTypeCstr);

        if     (lvalue->type==&JE_typeMap) JE_RuntimeError(rbegin, rend, "STRING expected, got %s\n", rtype);
        else if(lvalue->type==&JE_typeVec) JE_RuntimeError(rbegin, rend, "INTEGER expected, got %s\n", rtype);
        else                              JE_RuntimeError(lbegin, lend, "OBJECT or ARRAY expected, got %s\n", ltype);
    }

    /*
    * We don't destroy "lvalue" here because destroying lvalue can destroy
    * "result" as a consequence.  Instead, we pass lvalue to the caller and let
    * the caller decide *when* to destroy it.  Should the caller choose to keep
    * "result" around, the caller can clone "result" before destroying lvalue;
    * but in any case, the caller must destroy lvalue.
    */

    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_VALUE _IndexEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    JE_VALUE lvalue;
    JE_VALUE* result = _IndexRef(node, 0, symmap, info, &lvalue);
    JE_VALUE clone = result ? JE_VCALL(result,clone) : JE_ValueNul();

    JE_VCALL(&lvalue, destroy);

    return clone;
}

static JE_NTYPE_T _IndexGetType(const JE_NODE* node) {
    return JE_INDEX_NT;
}

static const char* _IndexGetTypeCstr(const JE_NODE* node) {
    return "INDEX_N";
}

static JE_TOKEN* _IndexTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _IndexTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+3];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LESS
*/

static void _LessDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _LessRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _LessEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare < 0);
}

static JE_NTYPE_T _LessGetType(const JE_NODE* node) {
    return JE_LESS_NT;
}

static const char* _LessGetTypeCstr(const JE_NODE* node) {
    return "LESS_N";
}

static JE_TOKEN* _LessTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _LessTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LESSEQ
*/

static void _LessEqDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _LessEqRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _LessEqEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare <= 0);
}

static JE_NTYPE_T _LessEqGetType(const JE_NODE* node) {
    return JE_LESSEQ_NT;
}

static const char* _LessEqGetTypeCstr(const JE_NODE* node) {
    return "LESSEQ_N";
}

static JE_TOKEN* _LessEqTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _LessEqTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LIST
*/

static void _ListDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ListRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ListEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE result = JE_NCALL(left,eval,symmap,info,NULL);

    JE_VecPush(this->data.vec, result);

    return JE_ValueNul();
}

static JE_NTYPE_T _ListGetType(const JE_NODE* node) {
    return JE_LIST_NT;
}

static const char* _ListGetTypeCstr(const JE_NODE* node) {
    return "LIST_N";
}

static JE_TOKEN* _ListTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ListTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LIST2
*/

static void _List2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _List2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _List2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;

    JE_NCALL(left,eval,symmap,info,this);
    JE_VecPush(this->data.vec, JE_NCALL(right,eval,symmap,info,NULL));

    return JE_ValueNul();
}

static JE_NTYPE_T _List2GetType(const JE_NODE* node) {
    return JE_LIST2_NT;
}

static const char* _List2GetTypeCstr(const JE_NODE* node) {
    return "LIST2_N";
}

static JE_TOKEN* _List2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _List2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LOGICALAND
*/

static void _LogicalAndDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _LogicalAndRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _LogicalAndEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE result = JE_NCALL(left,eval,symmap,info,NULL);

    if(JE_VCALL(&result,toTru)) {
        JE_VCALL(&result,destroy);

        result = JE_NCALL(right,eval,symmap,info,NULL);
    }

    return result;
}

static JE_NTYPE_T _LogicalAndGetType(const JE_NODE* node) {
    return JE_LOGICALAND_NT;
}

static const char* _LogicalAndGetTypeCstr(const JE_NODE* node) {
    return "LOGICALAND_N";
}

static JE_TOKEN* _LogicalAndTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _LogicalAndTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LOGICALNOT
*/

static void _LogicalNotDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _LogicalNotRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _LogicalNotEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE value = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE result = JE_ValueTru(!JE_VCALL(&value,toTru));

    JE_VCALL(&value,destroy);

    return result;
}

static JE_NTYPE_T _LogicalNotGetType(const JE_NODE* node) {
    return JE_LOGICALNOT_NT;
}

static const char* _LogicalNotGetTypeCstr(const JE_NODE* node) {
    return "LOGICALNOT_N";
}

static JE_TOKEN* _LogicalNotTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _LogicalNotTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - LOGICALOR
*/

static void _LogicalOrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _LogicalOrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _LogicalOrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE result = JE_NCALL(left,eval,symmap,info,NULL);

    if(!JE_VCALL(&result,toTru)) {
        JE_VCALL(&result,destroy);

        result = JE_NCALL(right,eval,symmap,info,NULL);
    }

    return result;
}

static JE_NTYPE_T _LogicalOrGetType(const JE_NODE* node) {
    return JE_LOGICALOR_NT;
}

static const char* _LogicalOrGetTypeCstr(const JE_NODE* node) {
    return "LOGICALOR_N";
}

static JE_TOKEN* _LogicalOrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _LogicalOrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - MEMBER
*/

static void _MemberDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _MemberRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    const JE_NODE* left = node + 1;
    JE_VALUE* result = NULL;

    *lvalue = JE_NCALL(left,eval,symmap,info,NULL);

    if(lvalue->type == &JE_typeMap) {
        result = JE_MapGet(*lvalue->data.map, node->data.id);

        if(!result && create) {
            result = JE_MapSet(lvalue->data.map, JE_StrClone(node->data.id), JE_ValueNul());
        }
    }
    else {
        const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);
        const char* type = JE_VCALL(lvalue,getTypeCstr);

        JE_RuntimeError(begin, end, "OBJECT expected, got %s\n", type);
    }

    /*
    * We don't destroy "lvalue" here because destroying lvalue can destroy
    * "result" as a consequence.  Instead, we pass lvalue to the caller and let
    * the caller decide *when* to destroy it.  Should the caller choose to keep
    * "result" around, the caller can clone "result" before destroying lvalue;
    * but in any case, the caller must destroy lvalue.
    */

    return result;
}

static JE_VALUE _MemberEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    JE_VALUE lvalue;
    JE_VALUE* result = _MemberRef(node, 0, symmap, info, &lvalue);
    JE_VALUE clone = result ? JE_VCALL(result,clone) : JE_ValueNul();

    JE_VCALL(&lvalue, destroy);

    return clone;
}

static JE_NTYPE_T _MemberGetType(const JE_NODE* node) {
    return JE_MEMBER_NT;
}

static const char* _MemberGetTypeCstr(const JE_NODE* node) {
    return "MEMBER_N";
}

static JE_TOKEN* _MemberTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _MemberTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - MINUS
*/

static void _MinusDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _MinusRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _MinusEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,minus,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _MinusGetType(const JE_NODE* node) {
    return JE_MINUS_NT;
}

static const char* _MinusGetTypeCstr(const JE_NODE* node) {
    return "MINUS_N";
}

static JE_TOKEN* _MinusTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _MinusTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - MOD
*/

static void _ModDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ModRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ModEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,mod,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _ModGetType(const JE_NODE* node) {
    return JE_MINUS_NT;
}

static const char* _ModGetTypeCstr(const JE_NODE* node) {
    return "MINUS_N";
}

static JE_TOKEN* _ModTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ModTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - MTARRAY
*/

static void _MtArrayDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _MtArrayRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _MtArrayEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    JE_VALUE array = JE_ValueVec(JE_VecCreate(0));

    return array;
}

static JE_NTYPE_T _MtArrayGetType(const JE_NODE* node) {
    return JE_MTARRAY_NT;
}

static const char* _MtArrayGetTypeCstr(const JE_NODE* node) {
    return "MTARRAY_N";
}

static JE_TOKEN* _MtArrayTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _MtArrayTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - MTOBJECT
*/

static void _MtObjectDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _MtObjectRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _MtObjectEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    JE_VALUE object = JE_ValueMap(JE_MapCreate(0));

    return object;
}

static JE_NTYPE_T _MtObjectGetType(const JE_NODE* node) {
    return JE_MTOBJECT_NT;
}

static const char* _MtObjectGetTypeCstr(const JE_NODE* node) {
    return "MTOBJECT_N";
}

static JE_TOKEN* _MtObjectTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _MtObjectTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - NEG
*/

static void _NegDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _NegRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _NegEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE value = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE negvalue = JE_VCALL(&value,neg);

    JE_VCALL(&value,destroy);

    return negvalue;
}

static JE_NTYPE_T _NegGetType(const JE_NODE* node) {
    return JE_NEG_NT;
}

static const char* _NegGetTypeCstr(const JE_NODE* node) {
    return "NEG_N";
}

static JE_TOKEN* _NegTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _NegTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - NOTEQUAL
*/

static void _NotEqualDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _NotEqualRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _NotEqualEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    int64_t compare = JE_VCALL(&lvalue,compare,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return JE_ValueTru(compare != 0);
}

static JE_NTYPE_T _NotEqualGetType(const JE_NODE* node) {
    return JE_NOTEQUAL_NT;
}

static const char* _NotEqualGetTypeCstr(const JE_NODE* node) {
    return "NOTEQUAL_N";
}

static JE_TOKEN* _NotEqualTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _NotEqualTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - OBJECT
*/

static void _ObjectDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ObjectRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ObjectEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_MAP* subscope = JE_SymmapCreateScope(symmap);
    JE_VALUE object = JE_ValueMap(JE_MapCreate(0));

    /* Build the object with the object as THIS */
    JE_MapSet(subscope, JE_StrCreate(JE_Strdup("THIS")), JE_VCALL(&object,clone));
    JE_NCALL(left,eval,subscope,info,&object);

    /* Destroy the subscope */
    JE_SymmapDestroy(subscope);

    return object;
}

static JE_NTYPE_T _ObjectGetType(const JE_NODE* node) {
    return JE_OBJECT_NT;
}

static const char* _ObjectGetTypeCstr(const JE_NODE* node) {
    return "OBJECT_N";
}

static JE_TOKEN* _ObjectTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ObjectTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - OBJECT2
*/

static void _Object2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Object2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Object2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    return _ObjectEval(node, symmap, info, this);
}

static JE_NTYPE_T _Object2GetType(const JE_NODE* node) {
    return JE_OBJECT2_NT;
}

static const char* _Object2GetTypeCstr(const JE_NODE* node) {
    return "OBJECT2_N";
}

static JE_TOKEN* _Object2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Object2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+3];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PAIR
*/

static void _PairDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PairRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PairEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,this);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,this);

    if(lvalue.type != &JE_typeStr) {
        const JE_TOKEN* begin = JE_NCALL(node,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(node,tokenEnd,info);

        JE_RuntimeError(begin, end, "OBJECT key must be a string\n");
        return JE_ValueNul();
    }

    assert(this->type == &JE_typeMap);
    JE_MapSet(this->data.map, lvalue.data.str, rvalue);

    return JE_ValueNul();
}

static JE_NTYPE_T _PairGetType(const JE_NODE* node) {
    return JE_PAIR_NT;
}

static const char* _PairGetTypeCstr(const JE_NODE* node) {
    return "PAIR_N";
}

static JE_TOKEN* _PairTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PairTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PAIRLIST
*/

static void _PairlistDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PairlistRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PairlistEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE nul = JE_NCALL(left,eval,symmap,info,this);

    return nul;
}

static JE_NTYPE_T _PairlistGetType(const JE_NODE* node) {
    return JE_PAIRLIST_NT;
}

static const char* _PairlistGetTypeCstr(const JE_NODE* node) {
    return "PAIRLIST_N";
}

static JE_TOKEN* _PairlistTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PairlistTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PAIRLIST2
*/

static void _Pairlist2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Pairlist2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Pairlist2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE nul;

    nul = JE_NCALL(left,eval,symmap,info,this);
    nul = JE_NCALL(right,eval,symmap,info,this);

    return nul;
}

static JE_NTYPE_T _Pairlist2GetType(const JE_NODE* node) {
    return JE_PAIRLIST2_NT;
}

static const char* _Pairlist2GetTypeCstr(const JE_NODE* node) {
    return "PAIRLIST2_N";
}

static JE_TOKEN* _Pairlist2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Pairlist2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PAREN
*/

static void _ParenDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ParenRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ParenEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* stmt = node + 1;
    JE_VALUE result = JE_NCALL(stmt,eval,symmap,info,NULL);

    return result;
}

static JE_NTYPE_T _ParenGetType(const JE_NODE* node) {
    return JE_PAREN_NT;
}

static const char* _ParenGetTypeCstr(const JE_NODE* node) {
    return "PAREN_N";
}

static JE_TOKEN* _ParenTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ParenTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PLUS
*/

static void _PlusDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PlusRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PlusEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,plus,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _PlusGetType(const JE_NODE* node) {
    return JE_PLUS_NT;
}

static const char* _PlusGetTypeCstr(const JE_NODE* node) {
    return "PLUS_N";
}

static JE_TOKEN* _PlusTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PlusTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - POS
*/

static void _PosDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PosRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PosEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE value = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE posvalue = JE_VCALL(&value,pos);

    JE_VCALL(&value,destroy);

    return posvalue;
}

static JE_NTYPE_T _PosGetType(const JE_NODE* node) {
    return JE_POS_NT;
}

static const char* _PosGetTypeCstr(const JE_NODE* node) {
    return "POS_N";
}

static JE_TOKEN* _PosTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PosTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - POSTDEC
*/

static void _PostDecDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PostDecRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PostDecEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info,&rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        switch(JE_VCALL(lvalue,getType)) {
            case JE_I64_T:
            case JE_F64_T: {
                JE_VALUE dec = JE_ValueI64(-1);

                result = JE_VCALL(lvalue,clone);
                JE_VCALL(lvalue,assnplus,dec);

                break;
            }

            default: {
                const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
                const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

                JE_RuntimeError(begin, end, "Cannot post-decrement a %s value", JE_VCALL(lvalue,getTypeCstr));
            }
        }
    }
    else {
        const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

        JE_RuntimeError(begin, end, "Cannot post-decrement, no such lvalue");
    }

    JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */

    return result;
}

static JE_NTYPE_T _PostDecGetType(const JE_NODE* node) {
    return JE_POSTDEC_NT;
}

static const char* _PostDecGetTypeCstr(const JE_NODE* node) {
    return "POSTDEC_N";
}

static JE_TOKEN* _PostDecTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PostDecTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - POSTINC
*/

static void _PostIncDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PostIncRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PostIncEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info,&rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        switch(JE_VCALL(lvalue,getType)) {
            case JE_I64_T:
            case JE_F64_T: {
                JE_VALUE inc = JE_ValueI64(1);

                result = JE_VCALL(lvalue,clone);
                JE_VCALL(lvalue,assnplus,inc);

                break;
            }

            default: {
                const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
                const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

                JE_RuntimeError(begin, end, "Cannot post-increment a %s value", JE_VCALL(lvalue,getTypeCstr));
            }
        }
    }
    else {
        const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

        JE_RuntimeError(begin, end, "Cannot post-increment, no such lvalue");
    }

    JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */

    return result;
}

static JE_NTYPE_T _PostIncGetType(const JE_NODE* node) {
    return JE_POSTINC_NT;
}

static const char* _PostIncGetTypeCstr(const JE_NODE* node) {
    return "POSTINC_N";
}

static JE_TOKEN* _PostIncTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PostIncTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - POW
*/

static void _PowDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PowRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PowEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,pow,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _PowGetType(const JE_NODE* node) {
    return JE_POW_NT;
}

static const char* _PowGetTypeCstr(const JE_NODE* node) {
    return "POW_N";
}

static JE_TOKEN* _PowTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PowTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PREDEC
*/

static void _PreDecDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PreDecRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PreDecEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info,&rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        switch(JE_VCALL(lvalue,getType)) {
            case JE_I64_T:
            case JE_F64_T: {
                JE_VALUE dec = JE_ValueI64(-1);

                JE_VCALL(lvalue,assnplus,dec);
                result = JE_VCALL(lvalue,clone);

                break;
            }

            default: {
                const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
                const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

                JE_RuntimeError(begin, end, "Cannot pre-decrement a %s value", JE_VCALL(lvalue,getTypeCstr));
            }
        }
    }
    else {
        const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

        JE_RuntimeError(begin, end, "Cannot pre-decrement, no such lvalue");
    }

    JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */

    return result;
}

static JE_NTYPE_T _PreDecGetType(const JE_NODE* node) {
    return JE_PREDEC_NT;
}

static const char* _PreDecGetTypeCstr(const JE_NODE* node) {
    return "PREDEC_N";
}

static JE_TOKEN* _PreDecTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PreDecTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - PREINC
*/

static void _PreIncDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _PreIncRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _PreIncEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info,&rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        switch(JE_VCALL(lvalue,getType)) {
            case JE_I64_T:
            case JE_F64_T: {
                JE_VALUE inc = JE_ValueI64(1);

                JE_VCALL(lvalue,assnplus,inc);
                result = JE_VCALL(lvalue,clone);

                break;
            }

            default: {
                const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
                const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

                JE_RuntimeError(begin, end, "Cannot pre-increment a %s value", JE_VCALL(lvalue,getTypeCstr));
            }
        }
    }
    else {
        const JE_TOKEN* begin = JE_NCALL(left,tokenBegin,info);
        const JE_TOKEN* end = JE_NCALL(left,tokenEnd,info);

        JE_RuntimeError(begin, end, "Cannot pre-increment, no such lvalue");
    }

    JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */

    return result;
}

static JE_NTYPE_T _PreIncGetType(const JE_NODE* node) {
    return JE_PREINC_NT;
}

static const char* _PreIncGetTypeCstr(const JE_NODE* node) {
    return "PREINC_N";
}

static JE_TOKEN* _PreIncTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _PreIncTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - SHL
*/

static void _ShlDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ShlRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ShlEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,shl,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _ShlGetType(const JE_NODE* node) {
    return JE_SHL_NT;
}

static const char* _ShlGetTypeCstr(const JE_NODE* node) {
    return "SHL_N";
}

static JE_TOKEN* _ShlTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ShlTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - SHR
*/

static void _ShrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ShrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ShrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,shr,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _ShrGetType(const JE_NODE* node) {
    return JE_SHR_NT;
}

static const char* _ShrGetTypeCstr(const JE_NODE* node) {
    return "SHR_N";
}

static JE_TOKEN* _ShrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ShrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - STMT
*/

static void _StmtDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _StmtRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _StmtEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* stmt = node + 1;
    JE_VALUE result = JE_NCALL(stmt,eval,symmap,info,NULL);

    return result;
}

static JE_NTYPE_T _StmtGetType(const JE_NODE* node) {
    return JE_STMT_NT;
}

static const char* _StmtGetTypeCstr(const JE_NODE* node) {
    return "STMT_N";
}

static JE_TOKEN* _StmtTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _StmtTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+1];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - STMT2
*/

static void _Stmt2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Stmt2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Stmt2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

    JE_VCALL(&lvalue,destroy);

    return rvalue;
}

static JE_NTYPE_T _Stmt2GetType(const JE_NODE* node) {
    return JE_STMT2_NT;
}

static const char* _Stmt2GetTypeCstr(const JE_NODE* node) {
    return "STMT2_N";
}

static JE_TOKEN* _Stmt2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Stmt2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - TERNARY
*/

static void _TernaryDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _TernaryRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _TernaryEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    const JE_NODE* righter = right + right->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,toTru) ? JE_NCALL(right+1,eval,symmap,info,NULL) : JE_NCALL(righter,eval,symmap,info,NULL);

    JE_VCALL(&lvalue,destroy);

    return result;
}

static JE_NTYPE_T _TernaryGetType(const JE_NODE* node) {
    return JE_TERNARY_NT;
}

static const char* _TernaryGetTypeCstr(const JE_NODE* node) {
    return "TERNARY_N";
}

static JE_TOKEN* _TernaryTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _TernaryTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+4];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - TERNARY2
*/

static void _Ternary2Destroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _Ternary2Ref(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _Ternary2Eval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    return JE_ValueNul();
}

static JE_NTYPE_T _Ternary2GetType(const JE_NODE* node) {
    return JE_TERNARY2_NT;
}

static const char* _Ternary2GetTypeCstr(const JE_NODE* node) {
    return "TERNARY2_N";
}

static JE_TOKEN* _Ternary2TokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _Ternary2TokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - TIMES
*/

static void _TimesDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _TimesRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _TimesEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE lvalue = JE_NCALL(left,eval,symmap,info,NULL);
    JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);
    JE_VALUE result = JE_VCALL(&lvalue,times,rvalue);

    JE_VCALL(&lvalue,destroy);
    JE_VCALL(&rvalue,destroy);

    return result;
}

static JE_NTYPE_T _TimesGetType(const JE_NODE* node) {
    return JE_TIMES_NT;
}

static const char* _TimesGetTypeCstr(const JE_NODE* node) {
    return "TIMES_N";
}

static JE_TOKEN* _TimesTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _TimesTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - JE_VALUE
*/

static void _ValueDestroy(JE_NODE* node) {
    JE_VCALL(&node->data.val,destroy);

    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _ValueRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _ValueEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    return JE_VCALL(&node->data.val,clone);
}

static JE_NTYPE_T _ValueGetType(const JE_NODE* node) {
    return JE_VALUE_NT;
}

static const char* _ValueGetTypeCstr(const JE_NODE* node) {
    return "VALUE_N";
}

static JE_TOKEN* _ValueTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _ValueTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNASR
*/

static void _AssnAsrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnAsrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnAsrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnasr,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnAsrGetType(const JE_NODE* node) {
    return JE_ASSNASR_NT;
}

static const char* _AssnAsrGetTypeCstr(const JE_NODE* node) {
    return "ASSNASR_N";
}

static JE_TOKEN* _AssnAsrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnAsrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNBITAND
*/

static void _AssnBitAndDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnBitAndRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnBitAndEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnbitand,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnBitAndGetType(const JE_NODE* node) {
    return JE_ASSNBITAND_NT;
}

static const char* _AssnBitAndGetTypeCstr(const JE_NODE* node) {
    return "ASSNBITAND_N";
}

static JE_TOKEN* _AssnBitAndTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnBitAndTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNBITOR
*/

static void _AssnBitOrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnBitOrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnBitOrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnbitor,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnBitOrGetType(const JE_NODE* node) {
    return JE_ASSNBITOR_NT;
}

static const char* _AssnBitOrGetTypeCstr(const JE_NODE* node) {
    return "ASSNBITOR_N";
}

static JE_TOKEN* _AssnBitOrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnBitOrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNBITXOR
*/

static void _AssnBitXorDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnBitXorRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnBitXorEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnbitxor,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnBitXorGetType(const JE_NODE* node) {
    return JE_ASSNBITXOR_NT;
}

static const char* _AssnBitXorGetTypeCstr(const JE_NODE* node) {
    return "ASSNBITXOR_N";
}

static JE_TOKEN* _AssnBitXorTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnBitXorTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNDIVBY
*/

static void _AssnDivbyDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnDivbyRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnDivbyEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assndivby,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnDivbyGetType(const JE_NODE* node) {
    return JE_ASSNDIVBY_NT;
}

static const char* _AssnDivbyGetTypeCstr(const JE_NODE* node) {
    return "ASSNDIVBY_N";
}

static JE_TOKEN* _AssnDivbyTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnDivbyTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNMINUS
*/

static void _AssnMinusDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnMinusRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnMinusEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnminus,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnMinusGetType(const JE_NODE* node) {
    return JE_ASSNMINUS_NT;
}

static const char* _AssnMinusGetTypeCstr(const JE_NODE* node) {
    return "ASSNMINUS_N";
}

static JE_TOKEN* _AssnMinusTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnMinusTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNMOD
*/

static void _AssnModDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnModRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnModEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnmod,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnModGetType(const JE_NODE* node) {
    return JE_ASSNMOD_NT;
}

static const char* _AssnModGetTypeCstr(const JE_NODE* node) {
    return "ASSNMOD_N";
}

static JE_TOKEN* _AssnModTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnModTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNPLUS
*/

static void _AssnPlusDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnPlusRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnPlusEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnplus,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnPlusGetType(const JE_NODE* node) {
    return JE_ASSNPLUS_NT;
}

static const char* _AssnPlusGetTypeCstr(const JE_NODE* node) {
    return "ASSNPLUS_N";
}

static JE_TOKEN* _AssnPlusTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnPlusTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNPOW
*/

static void _AssnPowDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnPowRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnPowEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnpow,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnPowGetType(const JE_NODE* node) {
    return JE_ASSNPOW_NT;
}

static const char* _AssnPowGetTypeCstr(const JE_NODE* node) {
    return "ASSNPOW_N";
}

static JE_TOKEN* _AssnPowTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnPowTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNSHL
*/

static void _AssnShlDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnShlRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnShlEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnshl,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnShlGetType(const JE_NODE* node) {
    return JE_ASSNSHL_NT;
}

static const char* _AssnShlGetTypeCstr(const JE_NODE* node) {
    return "ASSNSHL_N";
}

static JE_TOKEN* _AssnShlTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnShlTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNSHR
*/

static void _AssnShrDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnShrRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnShrEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assnshr,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnShrGetType(const JE_NODE* node) {
    return JE_ASSNSHR_NT;
}

static const char* _AssnShrGetTypeCstr(const JE_NODE* node) {
    return "ASSNSHR_N";
}

static JE_TOKEN* _AssnShrTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnShrTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* PRIVATE FUNCTIONS - ASSNTIMES
*/

static void _AssnTimesDestroy(JE_NODE* node) {
    memset(node, 0, sizeof(JE_NODE));
}

static JE_VALUE* _AssnTimesRef(const JE_NODE* node, int create, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* lvalue) {
    *lvalue = JE_ValueNul();

    return NULL;
}

static JE_VALUE _AssnTimesEval(const JE_NODE* node, JE_MAP* symmap, const JE_ASTINFO* info, JE_VALUE* this) {
    const JE_NODE* left = node + 1;
    const JE_NODE* right = node + node->data.next;
    JE_VALUE rlvalue;
    JE_VALUE* lvalue = JE_NCALL(left,ref,0,symmap,info, &rlvalue);
    JE_VALUE result = JE_ValueNul();

    if(lvalue) {
        JE_VALUE rvalue = JE_NCALL(right,eval,symmap,info,NULL);

        JE_VCALL(lvalue,assntimes,rvalue);
        result = JE_VCALL(lvalue,clone);

        JE_VCALL(&rlvalue,destroy);         /* Destroy ref's lvalue (always required when calling ref) */
    }

    return result;
}

static JE_NTYPE_T _AssnTimesGetType(const JE_NODE* node) {
    return JE_ASSNTIMES_NT;
}

static const char* _AssnTimesGetTypeCstr(const JE_NODE* node) {
    return "ASSNTIMES_N";
}

static JE_TOKEN* _AssnTimesTokenBegin(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi];
    JE_TOKEN* first = &info->token[loc.first];

    return first;
}

static JE_TOKEN* _AssnTimesTokenEnd(const JE_NODE* node, const JE_ASTINFO* info) {
    JE_YYLTYPE loc = info->tokenmap[node->tmi+2];
    JE_TOKEN* last = &info->token[loc.last];

    return last+1;
}


/* ***************************************************************************
* GLOBALS
*/

JE_NTYPE JE_ntypeArray      = {      _ArrayDestroy,      _ArrayRef,      _ArrayEval,      _ArrayGetType,      _ArrayGetTypeCstr,      _ArrayTokenBegin,      _ArrayTokenEnd };
JE_NTYPE JE_ntypeArray2     = {     _Array2Destroy,     _Array2Ref,     _Array2Eval,     _Array2GetType,     _Array2GetTypeCstr,     _Array2TokenBegin,     _Array2TokenEnd };
JE_NTYPE JE_ntypeAsr        = {        _AsrDestroy,        _AsrRef,        _AsrEval,        _AsrGetType,        _AsrGetTypeCstr,        _AsrTokenBegin,        _AsrTokenEnd };
JE_NTYPE JE_ntypeAssn       = {       _AssnDestroy,       _AssnRef,       _AssnEval,       _AssnGetType,       _AssnGetTypeCstr,       _AssnTokenBegin,       _AssnTokenEnd };
JE_NTYPE JE_ntypeBitwiseAnd = { _BitwiseAndDestroy, _BitwiseAndRef, _BitwiseAndEval, _BitwiseAndGetType, _BitwiseAndGetTypeCstr, _BitwiseAndTokenBegin, _BitwiseAndTokenEnd };
JE_NTYPE JE_ntypeBitwiseInv = { _BitwiseInvDestroy, _BitwiseInvRef, _BitwiseInvEval, _BitwiseInvGetType, _BitwiseInvGetTypeCstr, _BitwiseInvTokenBegin, _BitwiseInvTokenEnd };
JE_NTYPE JE_ntypeBitwiseOr  = {  _BitwiseOrDestroy,  _BitwiseOrRef,  _BitwiseOrEval,  _BitwiseOrGetType,  _BitwiseOrGetTypeCstr,  _BitwiseOrTokenBegin,  _BitwiseOrTokenEnd };
JE_NTYPE JE_ntypeBitwiseXor = { _BitwiseXorDestroy, _BitwiseXorRef, _BitwiseXorEval, _BitwiseXorGetType, _BitwiseXorGetTypeCstr, _BitwiseXorTokenBegin, _BitwiseXorTokenEnd };
JE_NTYPE JE_ntypeCall       = {       _CallDestroy,       _CallRef,       _CallEval,       _CallGetType,       _CallGetTypeCstr,       _CallTokenBegin,       _CallTokenEnd };
JE_NTYPE JE_ntypeCall2      = {      _Call2Destroy,      _Call2Ref,      _Call2Eval,      _Call2GetType,      _Call2GetTypeCstr,      _Call2TokenBegin,      _Call2TokenEnd };
JE_NTYPE JE_ntypeDivby      = {      _DivbyDestroy,      _DivbyRef,      _DivbyEval,      _DivbyGetType,      _DivbyGetTypeCstr,      _DivbyTokenBegin,      _DivbyTokenEnd };
JE_NTYPE JE_ntypeEqual      = {      _EqualDestroy,      _EqualRef,      _EqualEval,      _EqualGetType,      _EqualGetTypeCstr,      _EqualTokenBegin,      _EqualTokenEnd };
JE_NTYPE JE_ntypeGreater    = {    _GreaterDestroy,    _GreaterRef,    _GreaterEval,    _GreaterGetType,    _GreaterGetTypeCstr,    _GreaterTokenBegin,    _GreaterTokenEnd };
JE_NTYPE JE_ntypeGreaterEq  = {  _GreaterEqDestroy,  _GreaterEqRef,  _GreaterEqEval,  _GreaterEqGetType,  _GreaterEqGetTypeCstr,  _GreaterEqTokenBegin,  _GreaterEqTokenEnd };
JE_NTYPE JE_ntypeIdent      = {      _IdentDestroy,      _IdentRef,      _IdentEval,      _IdentGetType,      _IdentGetTypeCstr,      _IdentTokenBegin,      _IdentTokenEnd };
JE_NTYPE JE_ntypeIndex      = {      _IndexDestroy,      _IndexRef,      _IndexEval,      _IndexGetType,      _IndexGetTypeCstr,      _IndexTokenBegin,      _IndexTokenEnd };
JE_NTYPE JE_ntypeLess       = {       _LessDestroy,       _LessRef,       _LessEval,       _LessGetType,       _LessGetTypeCstr,       _LessTokenBegin,       _LessTokenEnd };
JE_NTYPE JE_ntypeLessEq     = {     _LessEqDestroy,     _LessEqRef,     _LessEqEval,     _LessEqGetType,     _LessEqGetTypeCstr,     _LessEqTokenBegin,     _LessEqTokenEnd };
JE_NTYPE JE_ntypeList       = {       _ListDestroy,       _ListRef,       _ListEval,       _ListGetType,       _ListGetTypeCstr,       _ListTokenBegin,       _ListTokenEnd };
JE_NTYPE JE_ntypeList2      = {      _List2Destroy,      _List2Ref,      _List2Eval,      _List2GetType,      _List2GetTypeCstr,      _List2TokenBegin,      _List2TokenEnd };
JE_NTYPE JE_ntypeLogicalAnd = { _LogicalAndDestroy, _LogicalAndRef, _LogicalAndEval, _LogicalAndGetType, _LogicalAndGetTypeCstr, _LogicalAndTokenBegin, _LogicalAndTokenEnd };
JE_NTYPE JE_ntypeLogicalNot = { _LogicalNotDestroy, _LogicalNotRef, _LogicalNotEval, _LogicalNotGetType, _LogicalNotGetTypeCstr, _LogicalNotTokenBegin, _LogicalNotTokenEnd };
JE_NTYPE JE_ntypeLogicalOr  = {  _LogicalOrDestroy,  _LogicalOrRef,  _LogicalOrEval,  _LogicalOrGetType,  _LogicalOrGetTypeCstr,  _LogicalOrTokenBegin,  _LogicalOrTokenEnd };
JE_NTYPE JE_ntypeMember     = {     _MemberDestroy,     _MemberRef,     _MemberEval,     _MemberGetType,     _MemberGetTypeCstr,     _MemberTokenBegin,     _MemberTokenEnd };
JE_NTYPE JE_ntypeMinus      = {      _MinusDestroy,      _MinusRef,      _MinusEval,      _MinusGetType,      _MinusGetTypeCstr,      _MinusTokenBegin,      _MinusTokenEnd };
JE_NTYPE JE_ntypeMod        = {        _ModDestroy,        _ModRef,        _ModEval,        _ModGetType,        _ModGetTypeCstr,        _ModTokenBegin,        _ModTokenEnd };
JE_NTYPE JE_ntypeMtArray    = {    _MtArrayDestroy,    _MtArrayRef,    _MtArrayEval,    _MtArrayGetType,    _MtArrayGetTypeCstr,    _MtArrayTokenBegin,    _MtArrayTokenEnd };
JE_NTYPE JE_ntypeMtObject   = {   _MtObjectDestroy,   _MtObjectRef,   _MtObjectEval,   _MtObjectGetType,   _MtObjectGetTypeCstr,   _MtObjectTokenBegin,   _MtObjectTokenEnd };
JE_NTYPE JE_ntypeNeg        = {        _NegDestroy,        _NegRef,        _NegEval,        _NegGetType,        _NegGetTypeCstr,        _NegTokenBegin,        _NegTokenEnd };
JE_NTYPE JE_ntypeNotEqual   = {   _NotEqualDestroy,   _NotEqualRef,   _NotEqualEval,   _NotEqualGetType,   _NotEqualGetTypeCstr,   _NotEqualTokenBegin,   _NotEqualTokenEnd };
JE_NTYPE JE_ntypeObject     = {     _ObjectDestroy,     _ObjectRef,     _ObjectEval,     _ObjectGetType,     _ObjectGetTypeCstr,     _ObjectTokenBegin,     _ObjectTokenEnd };
JE_NTYPE JE_ntypeObject2    = {    _Object2Destroy,    _Object2Ref,    _Object2Eval,    _Object2GetType,    _Object2GetTypeCstr,    _Object2TokenBegin,    _Object2TokenEnd };
JE_NTYPE JE_ntypePair       = {       _PairDestroy,       _PairRef,       _PairEval,       _PairGetType,       _PairGetTypeCstr,       _PairTokenBegin,       _PairTokenEnd };
JE_NTYPE JE_ntypePairlist   = {   _PairlistDestroy,   _PairlistRef,   _PairlistEval,   _PairlistGetType,   _PairlistGetTypeCstr,   _PairlistTokenBegin,   _PairlistTokenEnd };
JE_NTYPE JE_ntypePairlist2  = {  _Pairlist2Destroy,  _Pairlist2Ref,  _Pairlist2Eval,  _Pairlist2GetType,  _Pairlist2GetTypeCstr,  _Pairlist2TokenBegin,  _Pairlist2TokenEnd };
JE_NTYPE JE_ntypeParen      = {      _ParenDestroy,      _ParenRef,      _ParenEval,      _ParenGetType,      _ParenGetTypeCstr,      _ParenTokenBegin,      _ParenTokenEnd };
JE_NTYPE JE_ntypePlus       = {       _PlusDestroy,       _PlusRef,       _PlusEval,       _PlusGetType,       _PlusGetTypeCstr,       _PlusTokenBegin,       _PlusTokenEnd };
JE_NTYPE JE_ntypePos        = {        _PosDestroy,        _PosRef,        _PosEval,        _PosGetType,        _PosGetTypeCstr,        _PosTokenBegin,        _PosTokenEnd };
JE_NTYPE JE_ntypePostDec    = {    _PostDecDestroy,    _PostDecRef,    _PostDecEval,    _PostDecGetType,    _PostDecGetTypeCstr,    _PostDecTokenBegin,    _PostDecTokenEnd };
JE_NTYPE JE_ntypePostInc    = {    _PostIncDestroy,    _PostIncRef,    _PostIncEval,    _PostIncGetType,    _PostIncGetTypeCstr,    _PostIncTokenBegin,    _PostIncTokenEnd };
JE_NTYPE JE_ntypePow        = {        _PowDestroy,        _PowRef,        _PowEval,        _PowGetType,        _PowGetTypeCstr,        _PowTokenBegin,        _PowTokenEnd };
JE_NTYPE JE_ntypePreDec     = {     _PreDecDestroy,     _PreDecRef,     _PreDecEval,     _PreDecGetType,     _PreDecGetTypeCstr,     _PreDecTokenBegin,     _PreDecTokenEnd };
JE_NTYPE JE_ntypePreInc     = {     _PreIncDestroy,     _PreIncRef,     _PreIncEval,     _PreIncGetType,     _PreIncGetTypeCstr,     _PreIncTokenBegin,     _PreIncTokenEnd };
JE_NTYPE JE_ntypeShl        = {        _ShlDestroy,        _ShlRef,        _ShlEval,        _ShlGetType,        _ShlGetTypeCstr,        _ShlTokenBegin,        _ShlTokenEnd };
JE_NTYPE JE_ntypeShr        = {        _ShrDestroy,        _ShrRef,        _ShrEval,        _ShrGetType,        _ShrGetTypeCstr,        _ShrTokenBegin,        _ShrTokenEnd };
JE_NTYPE JE_ntypeStmt       = {       _StmtDestroy,       _StmtRef,       _StmtEval,       _StmtGetType,       _StmtGetTypeCstr,       _StmtTokenBegin,       _StmtTokenEnd };
JE_NTYPE JE_ntypeStmt2      = {      _Stmt2Destroy,      _Stmt2Ref,      _Stmt2Eval,      _Stmt2GetType,      _Stmt2GetTypeCstr,      _Stmt2TokenBegin,      _Stmt2TokenEnd };
JE_NTYPE JE_ntypeTernary    = {    _TernaryDestroy,    _TernaryRef,    _TernaryEval,    _TernaryGetType,    _TernaryGetTypeCstr,    _TernaryTokenBegin,    _TernaryTokenEnd };
JE_NTYPE JE_ntypeTernary2   = {   _Ternary2Destroy,   _Ternary2Ref,   _Ternary2Eval,   _Ternary2GetType,   _Ternary2GetTypeCstr,   _Ternary2TokenBegin,   _Ternary2TokenEnd };
JE_NTYPE JE_ntypeTimes      = {      _TimesDestroy,      _TimesRef,      _TimesEval,      _TimesGetType,      _TimesGetTypeCstr,      _TimesTokenBegin,      _TimesTokenEnd };
JE_NTYPE JE_ntypeValue      = {      _ValueDestroy,      _ValueRef,      _ValueEval,      _ValueGetType,      _ValueGetTypeCstr,      _ValueTokenBegin,      _ValueTokenEnd };

JE_NTYPE JE_ntypeAssnAsr    = {    _AssnAsrDestroy,    _AssnAsrRef,    _AssnAsrEval,    _AssnAsrGetType,    _AssnAsrGetTypeCstr,    _AssnAsrTokenBegin,    _AssnAsrTokenEnd };
JE_NTYPE JE_ntypeAssnBitAnd = { _AssnBitAndDestroy, _AssnBitAndRef, _AssnBitAndEval, _AssnBitAndGetType, _AssnBitAndGetTypeCstr, _AssnBitAndTokenBegin, _AssnBitAndTokenEnd };
JE_NTYPE JE_ntypeAssnBitOr  = {  _AssnBitOrDestroy,  _AssnBitOrRef,  _AssnBitOrEval,  _AssnBitOrGetType,  _AssnBitOrGetTypeCstr,  _AssnBitOrTokenBegin,  _AssnBitOrTokenEnd };
JE_NTYPE JE_ntypeAssnBitXor = { _AssnBitXorDestroy, _AssnBitXorRef, _AssnBitXorEval, _AssnBitXorGetType, _AssnBitXorGetTypeCstr, _AssnBitXorTokenBegin, _AssnBitXorTokenEnd };
JE_NTYPE JE_ntypeAssnDivby  = {  _AssnDivbyDestroy,  _AssnDivbyRef,  _AssnDivbyEval,  _AssnDivbyGetType,  _AssnDivbyGetTypeCstr,  _AssnDivbyTokenBegin,  _AssnDivbyTokenEnd };
JE_NTYPE JE_ntypeAssnMinus  = {  _AssnMinusDestroy,  _AssnMinusRef,  _AssnMinusEval,  _AssnMinusGetType,  _AssnMinusGetTypeCstr,  _AssnMinusTokenBegin,  _AssnMinusTokenEnd };
JE_NTYPE JE_ntypeAssnMod    = {    _AssnModDestroy,    _AssnModRef,    _AssnModEval,    _AssnModGetType,    _AssnModGetTypeCstr,    _AssnModTokenBegin,    _AssnModTokenEnd };
JE_NTYPE JE_ntypeAssnPlus   = {   _AssnPlusDestroy,   _AssnPlusRef,   _AssnPlusEval,   _AssnPlusGetType,   _AssnPlusGetTypeCstr,   _AssnPlusTokenBegin,   _AssnPlusTokenEnd };
JE_NTYPE JE_ntypeAssnPow    = {    _AssnPowDestroy,    _AssnPowRef,    _AssnPowEval,    _AssnPowGetType,    _AssnPowGetTypeCstr,    _AssnPowTokenBegin,    _AssnPowTokenEnd };
JE_NTYPE JE_ntypeAssnShl    = {    _AssnShlDestroy,    _AssnShlRef,    _AssnShlEval,    _AssnShlGetType,    _AssnShlGetTypeCstr,    _AssnShlTokenBegin,    _AssnShlTokenEnd };
JE_NTYPE JE_ntypeAssnShr    = {    _AssnShrDestroy,    _AssnShrRef,    _AssnShrEval,    _AssnShrGetType,    _AssnShrGetTypeCstr,    _AssnShrTokenBegin,    _AssnShrTokenEnd };
JE_NTYPE JE_ntypeAssnTimes  = {  _AssnTimesDestroy,  _AssnTimesRef,  _AssnTimesEval,  _AssnTimesGetType,  _AssnTimesGetTypeCstr,  _AssnTimesTokenBegin,  _AssnTimesTokenEnd };
