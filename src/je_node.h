#ifndef JE_NODE_H_
#define JE_NODE_H_

#include <stddef.h>
#include "je_parse.h"
#include "je_value.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ***************************************************************************
* FORWARD DECLARATION
*/

typedef struct JE_NTYPE JE_NTYPE;


/* ***************************************************************************
* TYPES
*/

typedef struct JE_NODE {
    JE_NTYPE* ntype;

    union {
        JE_STR   id;    /* This node is an identifier */
        JE_VALUE val;   /* This node is a value */
        size_t   next;  /* Offset to the second child node, if any */
    } data;

    size_t skip;        /* Offset to the next sibling node */
    size_t tmi;         /* Token map index */
} JE_NODE;


/* ***************************************************************************
* MACROS
*/

#define JE_NCALL(np,method,...) ((np)->ntype->method((np),##__VA_ARGS__))


/* ***************************************************************************
* FUNCTIONS
*/

JE_NODE JE_NodeArray(size_t llen, size_t tmi);
JE_NODE JE_NodeArray2(size_t llen, size_t tmi);
JE_NODE JE_NodeAsr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssn(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeBitwiseAnd(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeBitwiseInv(size_t llen, size_t tmi);
JE_NODE JE_NodeBitwiseOr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeBitwiseXor(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeCall(size_t llen, size_t tmi);
JE_NODE JE_NodeCall2(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeDivby(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeEqual(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeGreater(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeGreaterEq(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeIdent(JE_STR ident, size_t tmi);
JE_NODE JE_NodeIndex(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeLess(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeLessEq(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeList(size_t llen, size_t tmi);
JE_NODE JE_NodeList2(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeLogicalAnd(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeLogicalNot(size_t llen, size_t tmi);
JE_NODE JE_NodeLogicalOr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeMember(size_t llen, JE_STR right, size_t tmi);
JE_NODE JE_NodeMinus(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeMod(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeMtArray(size_t tmi);
JE_NODE JE_NodeMtObject(size_t tmi);
JE_NODE JE_NodeNeg(size_t llen, size_t tmi);
JE_NODE JE_NodeNotEqual(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeObject(size_t llen, size_t tmi);
JE_NODE JE_NodeObject2(size_t llen, size_t tmi);
JE_NODE JE_NodePair(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodePairlist(size_t llen, size_t tmi);
JE_NODE JE_NodePairlist2(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeParen(size_t llen, size_t tmi);
JE_NODE JE_NodePlus(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodePos(size_t llen, size_t tmi);
JE_NODE JE_NodePostDec(size_t llen, size_t tmi);
JE_NODE JE_NodePostInc(size_t llen, size_t tmi);
JE_NODE JE_NodePow(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodePreDec(size_t llen, size_t tmi);
JE_NODE JE_NodePreInc(size_t llen, size_t tmi);
JE_NODE JE_NodeShl(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeShr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeStmt(size_t llen, size_t tmi);
JE_NODE JE_NodeStmt2(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeTernary(size_t llen, size_t rlen, size_t rrlen, size_t tmi);
JE_NODE JE_NodeTernary2(size_t rlen, size_t rrlen, size_t tmi);
JE_NODE JE_NodeTimes(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeVal(JE_VALUE value, size_t tmi);

JE_NODE JE_NodeAssnAsr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnBitAnd(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnBitOr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnBitXor(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnDivby(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnMinus(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnMod(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnPlus(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnPow(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnShl(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnShr(size_t llen, size_t rlen, size_t tmi);
JE_NODE JE_NodeAssnTimes(size_t llen, size_t rlen, size_t tmi);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JE_NODE_H_ */
