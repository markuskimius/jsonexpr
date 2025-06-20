#include <stdio.h>
#include <string.h>
#include "je_astinfo.h"
#include "je_astnode.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_util.h"


/* ***************************************************************************
* CONSTANTS
*/

#define _MINLEN 32


/* ***************************************************************************
* PRIVATE FUNCTIONS
*/

/**
* Create a new JE_ASTNODE with an optional initsize.  Set initsize to 0 to
* initialize with a default size.
*/
static JE_ASTNODE _Create(size_t initsize) {
    JE_ASTNODE astnode;

    astnode.node = JE_Malloc(sizeof(JE_NODE) * (initsize < _MINLEN ? _MINLEN : initsize));
    astnode.nlen = 0;

    return astnode;
}

/**
* Create a new JE_ASTNODE with one node.  Once passed to this function, node
* may no longer be operated on by the caller.
*/
static JE_ASTNODE _Create2(JE_NODE node) {
    JE_ASTNODE astnode = _Create(0);

    astnode.node[0] = node;
    astnode.nlen = 1;

    return astnode;
}

/**
* Push nodes from src to dest.  Once pushed, src may no longer be operated on
* by the caller.
*/
static void _Push(JE_ASTNODE* dest, JE_ASTNODE src) {
    dest->node = JE_Realloc(dest->node, sizeof(JE_NODE) * (dest->nlen + src.nlen));
    memcpy(&dest->node[dest->nlen], src.node, sizeof(JE_NODE)*src.nlen);
    dest->nlen += src.nlen;

    src.nlen = 0;
    JE_AstNodeDestroy(&src);
}

/**
* Push two nodes from src1 and src2 to dest.  Once pushed, src1 and src2 may no
* longer be operated on by the caller.
*/
static void _Push2(JE_ASTNODE* dest, JE_ASTNODE src1, JE_ASTNODE src2) {
    dest->node = JE_Realloc(dest->node, sizeof(JE_NODE) * (dest->nlen + src1.nlen + src2.nlen));
    memcpy(&dest->node[dest->nlen], src1.node, sizeof(JE_NODE) * src1.nlen);
    memcpy(&dest->node[dest->nlen+src1.nlen], src2.node, sizeof(JE_NODE) * src2.nlen);
    dest->nlen += src1.nlen + src2.nlen;

    src1.nlen = src2.nlen = 0;
    JE_AstNodeDestroy(&src1);
    JE_AstNodeDestroy(&src2);
}


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_ASTNODE.
*/
JE_ASTNODE JE_AstNodeCreate() {
    return _Create(0);
}

/**
* Destroy JE_ASTNODE.
*/
void JE_AstNodeDestroy(JE_ASTNODE* astnode) {
    if(astnode->node) {
        for(size_t i=0; i<astnode->nlen; i++) JE_NCALL(&astnode->node[i],destroy);
        JE_Free(astnode->node);
    }

    memset(astnode, 0, sizeof(JE_ASTNODE));
}

/**
* Create a new JE_ASTNODE for an array.
*/
JE_ASTNODE JE_AstNodeArray(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeArray(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for an array whose content includes a trailing comma.
*/
JE_ASTNODE JE_AstNodeArray2(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4) {
    JE_NODE node = JE_NodeArray2(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc4(info, loc1, loc2, loc3, loc4);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the >>> operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAsr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAsr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the assignment operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssn(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssn(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the BITWISE AND operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeBitwiseAnd(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeBitwiseAnd(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the BITWISE INVERSE operator whose
* argument is left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeBitwiseInv(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeBitwiseInv(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the BITWISE OR operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeBitwiseOr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeBitwiseOr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the BITWISE XOR operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeBitwiseXor(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeBitwiseXor(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing a function call with no arguments.  Left
* is the function to call.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeCall(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeCall(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing a function call with one or more
* arguments.  Left is the function to call and right is the list of arguments.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeCall2(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4) {
    JE_NODE node = JE_NodeCall2(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc4(info, loc1, loc2, loc3, loc4);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the divide operator whose arguments are
* left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeDivby(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeDivby(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality "==".
*/
JE_ASTNODE JE_AstNodeEqual(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeEqual(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality ">".
*/
JE_ASTNODE JE_AstNodeGreater(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeGreater(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality ">=".
*/
JE_ASTNODE JE_AstNodeGreaterEq(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeGreaterEq(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing one identifier.
*
* Once added into astnode, ident may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeIdent(JE_STR ident, JE_ASTINFO* info, JE_YYLTYPE loc1) {
    JE_NODE node = JE_NodeIdent(ident, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    JE_AstInfoPushLoc1(info, loc1);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the index operator whose arguments are
* left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeIndex(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4) {
    JE_NODE node = JE_NodeIndex(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc4(info, loc1, loc2, loc3, loc4);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality "<".
*/
JE_ASTNODE JE_AstNodeLess(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeLess(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality "<=".
*/
JE_ASTNODE JE_AstNodeLessEq(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeLessEq(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for a list containing one expression.
*/
JE_ASTNODE JE_AstNodeList(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1) {
    JE_NODE node = JE_NodeList(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc1(info, loc1);

    return astnode;
}

/**
* Create a new JE_ASTNODE for a list of 2 or more expressions.
*/
JE_ASTNODE JE_AstNodeList2(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeList2(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the LOGICAL AND operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeLogicalAnd(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeLogicalAnd(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the LOGICAL NOT operator whose argument is
* left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeLogicalNot(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeLogicalNot(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the LOGICAL OR operator whose arguments
* are left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeLogicalOr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeLogicalOr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing a member access lvalue.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeMember(JE_ASTNODE left, JE_STR right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeMember(left.nlen, right, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the minus operator whose arguments are
* left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeMinus(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeMinus(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the modulus operator whose arguments are
* left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeMod(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeMod(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for an empty array.
*/
JE_ASTNODE JE_AstNodeMtArray(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeMtArray(info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE for an empty object.
*/
JE_ASTNODE JE_AstNodeMtObject(JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeMtObject(info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the negation operator whose argument is
* left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeNeg(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeNeg(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE for the inequality "!=".
*/
JE_ASTNODE JE_AstNodeNotEqual(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeNotEqual(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for an object.
*/
JE_ASTNODE JE_AstNodeObject(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeObject(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for an object whose content includes a trailing comma.
*/
JE_ASTNODE JE_AstNodeObject2(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4) {
    JE_NODE node = JE_NodeObject2(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc4(info, loc1, loc2, loc3, loc4);

    return astnode;
}

/**
* Create a new JE_ASTNODE for a pair.
*/
JE_ASTNODE JE_AstNodePair(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodePair(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE for a list containing one pair.
*/
JE_ASTNODE JE_AstNodePairlist(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1) {
    JE_NODE node = JE_NodePairlist(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc1(info, loc1);

    return astnode;
}

/**
* Create a new JE_ASTNODE for a list of 2 or more pairs.
*/
JE_ASTNODE JE_AstNodePairlist2(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodePairlist2(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing an expression surrounded by parenthesis.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeParen(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeParen(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the plus operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodePlus(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodePlus(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the + operator whose argument is left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodePos(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodePos(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the postfix -- operator whose argument is
* left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodePostDec(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodePostDec(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the postfix ++ operator whose argument is
* left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodePostInc(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodePostInc(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the ** operator whose arguments are left and
* right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodePow(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodePow(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the prefix -- operator whose argument is left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodePreDec(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodePreDec(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the prefix ++ operator whose argument is left.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodePreInc(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodePreInc(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the << operator whose arguments are left and
* right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeShl(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeShl(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the >> operator whose arguments are left and
* right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeShr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeShr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing one statement.
*
* Once added into astnode, left may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeStmt(JE_ASTNODE left, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2) {
    JE_NODE node = JE_NodeStmt(left.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push(&astnode, left);
    JE_AstInfoPushLoc2(info, loc1, loc2);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing two statements.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeStmt2(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeStmt2(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the ternary operator with three arguments
* left, right, and righter.
*
* Once added into astnode, left, right, and righter may no longer be operated
* on by the caller.
*/
JE_ASTNODE JE_AstNodeTernary(JE_ASTNODE left, JE_ASTNODE right, JE_ASTNODE righter, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3, JE_YYLTYPE loc4, JE_YYLTYPE loc5) {
    JE_NODE node = JE_NodeTernary(left.nlen, right.nlen, righter.nlen, info->mlen);
    JE_NODE node2 = JE_NodeTernary2(right.nlen, righter.nlen, info->mlen+3);
    JE_ASTNODE astnode = _Create2(node);
    JE_ASTNODE astnode2 = _Create2(node2);

    _Push2(&astnode2, right, righter);
    _Push2(&astnode, left, astnode2);

    JE_AstInfoPushLoc5(info, loc1, loc2, loc3, loc4, loc5);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the times operator whose arguments are
* left and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeTimes(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeTimes(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing one value.
*
* Once added into JE_ASTNODE, value may no longer be operated on by the caller.
*/
JE_ASTNODE JE_AstNodeVal(JE_VALUE value, JE_ASTINFO* info, JE_YYLTYPE loc1) {
    JE_NODE node = JE_NodeVal(value, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    JE_AstInfoPushLoc1(info, loc1);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the ">>=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnAsr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnAsr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "&=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnBitAnd(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnBitAnd(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "|=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnBitOr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnBitOr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "^=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnBitXor(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnBitXor(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "/=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnDivby(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnDivby(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "-=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnMinus(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnMinus(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "%=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnMod(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnMod(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "+=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnPlus(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnPlus(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "**=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnPow(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnPow(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "<<=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnShl(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnShl(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the ">>>=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnShr(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnShr(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}

/**
* Create a new JE_ASTNODE containing the "*=" operator whose arguments are left
* and right.
*
* Once added into astnode, left and right may no longer be operated on by the
* caller.
*/
JE_ASTNODE JE_AstNodeAssnTimes(JE_ASTNODE left, JE_ASTNODE right, JE_ASTINFO* info, JE_YYLTYPE loc1, JE_YYLTYPE loc2, JE_YYLTYPE loc3) {
    JE_NODE node = JE_NodeAssnTimes(left.nlen, right.nlen, info->mlen);
    JE_ASTNODE astnode = _Create2(node);

    _Push2(&astnode, left, right);
    JE_AstInfoPushLoc3(info, loc1, loc2, loc3);

    return astnode;
}
