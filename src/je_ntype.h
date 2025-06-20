#ifndef JE_NTYPE_H_
#define JE_NTYPE_H_

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATIONS
*/

typedef struct JE_ASTINFO JE_ASTINFO;
typedef struct JE_MAP JE_MAP;
typedef struct JE_NODE JE_NODE;
typedef struct JE_TOKEN JE_TOKEN;
typedef struct JE_VALUE JE_VALUE;


/* ***************************************************************************
* TYPES
*/

typedef enum {
    JE_ARRAY_NT,
    JE_ARRAY2_NT,
    JE_ASR_NT,
    JE_ASSN_NT,
    JE_BITWISEAND_NT,
    JE_BITWISEINV_NT,
    JE_BITWISEOR_NT,
    JE_BITWISEXOR_NT,
    JE_CALL_NT,
    JE_CALL2_NT,
    JE_DIVBY_NT,
    JE_EQUAL_NT,
    JE_GREATER_NT,
    JE_GREATEREQ_NT,
    JE_NOTEQUAL_NT,
    JE_IDENT_NT,
    JE_INDEX_NT,
    JE_LESS_NT,
    JE_LESSEQ_NT,
    JE_LIST_NT,
    JE_LIST2_NT,
    JE_LOGICALAND_NT,
    JE_LOGICALNOT_NT,
    JE_LOGICALOR_NT,
    JE_MEMBER_NT,
    JE_MINUS_NT,
    JE_MOD_NT,
    JE_MTARRAY_NT,
    JE_MTOBJECT_NT,
    JE_NEG_NT,
    JE_OBJECT_NT,
    JE_OBJECT2_NT,
    JE_PAIRLIST_NT,
    JE_PAIRLIST2_NT,
    JE_PAIR_NT,
    JE_PAREN_NT,
    JE_PLUS_NT,
    JE_POSTDEC_NT,
    JE_POSTINC_NT,
    JE_POS_NT,
    JE_POW_NT,
    JE_PREDEC_NT,
    JE_PREINC_NT,
    JE_SHL_NT,
    JE_SHR_NT,
    JE_STMT_NT,
    JE_STMT2_NT,
    JE_TERNARY_NT,
    JE_TERNARY2_NT,
    JE_TIMES_NT,
    JE_VALUE_NT,

    JE_ASSNASR_NT,
    JE_ASSNBITAND_NT,
    JE_ASSNBITOR_NT,
    JE_ASSNBITXOR_NT,
    JE_ASSNDIVBY_NT,
    JE_ASSNMINUS_NT,
    JE_ASSNMOD_NT,
    JE_ASSNPLUS_NT,
    JE_ASSNPOW_NT,
    JE_ASSNSHL_NT,
    JE_ASSNSHR_NT,
    JE_ASSNTIMES_NT,
} JE_NTYPE_T;

typedef struct JE_NTYPE {
    void (*destroy)(JE_NODE*);
    JE_VALUE* (*ref)(const JE_NODE*, int create, JE_MAP*, const JE_ASTINFO*, JE_VALUE* lvalue);
    JE_VALUE (*eval)(const JE_NODE*, JE_MAP*, const JE_ASTINFO*, JE_VALUE* this);
    JE_NTYPE_T (*getType)(const JE_NODE*);
    const char* (*getTypeCstr)(const JE_NODE*);
    JE_TOKEN* (*tokenBegin)(const JE_NODE*, const JE_ASTINFO*);
    JE_TOKEN* (*tokenEnd)(const JE_NODE*, const JE_ASTINFO*);
} JE_NTYPE;


/* ***************************************************************************
* GLOBALS
*/

extern JE_NTYPE JE_ntypeArray;
extern JE_NTYPE JE_ntypeArray2;
extern JE_NTYPE JE_ntypeAsr;
extern JE_NTYPE JE_ntypeAssn;
extern JE_NTYPE JE_ntypeBitwiseAnd;
extern JE_NTYPE JE_ntypeBitwiseInv;
extern JE_NTYPE JE_ntypeBitwiseOr;
extern JE_NTYPE JE_ntypeBitwiseXor;
extern JE_NTYPE JE_ntypeCall;
extern JE_NTYPE JE_ntypeCall2;
extern JE_NTYPE JE_ntypeDivby;
extern JE_NTYPE JE_ntypeEqual;
extern JE_NTYPE JE_ntypeGreater;
extern JE_NTYPE JE_ntypeGreaterEq;
extern JE_NTYPE JE_ntypeIdent;
extern JE_NTYPE JE_ntypeIndex;
extern JE_NTYPE JE_ntypeLess;
extern JE_NTYPE JE_ntypeLessEq;
extern JE_NTYPE JE_ntypeList;
extern JE_NTYPE JE_ntypeList2;
extern JE_NTYPE JE_ntypeLogicalAnd;
extern JE_NTYPE JE_ntypeLogicalNot;
extern JE_NTYPE JE_ntypeLogicalOr;
extern JE_NTYPE JE_ntypeMember;
extern JE_NTYPE JE_ntypeMinus;
extern JE_NTYPE JE_ntypeMod;
extern JE_NTYPE JE_ntypeMtArray;
extern JE_NTYPE JE_ntypeMtObject;
extern JE_NTYPE JE_ntypeNeg;
extern JE_NTYPE JE_ntypeNotEqual;
extern JE_NTYPE JE_ntypeObject;
extern JE_NTYPE JE_ntypeObject2;
extern JE_NTYPE JE_ntypePair;
extern JE_NTYPE JE_ntypePairlist;
extern JE_NTYPE JE_ntypePairlist2;
extern JE_NTYPE JE_ntypeParen;
extern JE_NTYPE JE_ntypePlus;
extern JE_NTYPE JE_ntypePos;
extern JE_NTYPE JE_ntypePostDec;
extern JE_NTYPE JE_ntypePostInc;
extern JE_NTYPE JE_ntypePow;
extern JE_NTYPE JE_ntypePreDec;
extern JE_NTYPE JE_ntypePreInc;
extern JE_NTYPE JE_ntypeShl;
extern JE_NTYPE JE_ntypeShr;
extern JE_NTYPE JE_ntypeStmt;
extern JE_NTYPE JE_ntypeStmt2;
extern JE_NTYPE JE_ntypeTernary;
extern JE_NTYPE JE_ntypeTernary2;
extern JE_NTYPE JE_ntypeTimes;
extern JE_NTYPE JE_ntypeValue;

extern JE_NTYPE JE_ntypeAssnAsr;
extern JE_NTYPE JE_ntypeAssnBitAnd;
extern JE_NTYPE JE_ntypeAssnBitOr;
extern JE_NTYPE JE_ntypeAssnBitXor;
extern JE_NTYPE JE_ntypeAssnDivby;
extern JE_NTYPE JE_ntypeAssnMinus;
extern JE_NTYPE JE_ntypeAssnMod;
extern JE_NTYPE JE_ntypeAssnPlus;
extern JE_NTYPE JE_ntypeAssnPow;
extern JE_NTYPE JE_ntypeAssnShl;
extern JE_NTYPE JE_ntypeAssnShr;
extern JE_NTYPE JE_ntypeAssnTimes;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_NTYPE_H_ */
