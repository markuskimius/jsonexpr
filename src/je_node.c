#include <stdio.h>
#include <string.h>
#include "je_astnode.h"
#include "je_node.h"
#include "je_ntype.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

JE_NODE JE_NodeArray(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeArray;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeArray2(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeArray2;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAsr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAsr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssn(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssn;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeBitwiseAnd(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeBitwiseAnd;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeBitwiseInv(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeBitwiseInv;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeBitwiseOr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeBitwiseOr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeBitwiseXor(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeBitwiseXor;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeCall(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeCall;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeCall2(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeCall2;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeDivby(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeDivby;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeGreater(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeGreater;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeGreaterEq(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeGreaterEq;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeEqual(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeEqual;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeIdent(JE_STR ident, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeIdent;
    node.data.id = ident;
    node.skip = 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeIndex(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeIndex;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeLess(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeLess;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeLessEq(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeLessEq;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeList(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeList;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeList2(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeList2;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeLogicalAnd(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeLogicalAnd;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeLogicalNot(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeLogicalNot;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeLogicalOr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeLogicalOr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeMember(size_t llen, JE_STR right, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeMember;
    node.data.id = right;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeMinus(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeMinus;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeMod(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeMod;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeMtArray(size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeMtArray;
    node.data.next = 0;
    node.skip = 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeMtObject(size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeMtObject;
    node.data.next = 0;
    node.skip = 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeNeg(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeNeg;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeNotEqual(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeNotEqual;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeObject(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeObject;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeObject2(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeObject2;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePair(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePair;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePairlist(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePairlist;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePairlist2(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePairlist2;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeParen(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeParen;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePlus(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePlus;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePos(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePos;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePostDec(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePostDec;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePostInc(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePostInc;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePow(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePow;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePreDec(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePreDec;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodePreInc(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypePreInc;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeShl(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeShl;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeShr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeShr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeStmt(size_t llen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeStmt;
    node.data.next = 0;
    node.skip = llen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeStmt2(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeStmt2;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeTernary(size_t llen, size_t rlen, size_t rrlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeTernary;
    node.data.next = llen + 1;
    node.skip = llen + rlen + rrlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeTernary2(size_t rlen, size_t rrlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeTernary2;
    node.data.next = rlen + 1;
    node.skip = rlen + rrlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeTimes(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeTimes;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeVal(JE_VALUE value, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeValue;
    node.data.val = value;
    node.skip = 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnAsr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnAsr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnBitAnd(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnBitAnd;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnBitOr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnBitOr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnBitXor(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnBitXor;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnDivby(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnDivby;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnMinus(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnMinus;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnMod(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnMod;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnPlus(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnPlus;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnPow(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnPow;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnShl(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnShl;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnShr(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnShr;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}

JE_NODE JE_NodeAssnTimes(size_t llen, size_t rlen, size_t tmi) {
    JE_NODE node;

    node.ntype = &JE_ntypeAssnTimes;
    node.data.next = llen + 1;
    node.skip = llen + rlen + 1;
    node.tmi = tmi;

    return node;
}
