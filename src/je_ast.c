#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "je_ast.h"
#include "je_node.h"
#include "je_ntype.h"
#include "je_value.h"


/* ***************************************************************************
* PUBLIC FUNCTIONS
*/

/**
* Create a new JE_AST.
*/
JE_AST JE_AstCreate() {
    JE_AST ast;

    ast.astnode = JE_AstNodeCreate();
    ast.info = JE_AstInfoCreate();
    ast.isok = 1;

    return ast;
}

/**
* Destroy ast.
*/
void JE_AstDestroy(JE_AST* ast) {
    JE_AstNodeDestroy(&ast->astnode);
    JE_AstInfoDestroy(&ast->info);

    memset(ast, 0, sizeof(JE_AST));
}

/**
* Evaluate a JE_AST.
*/
JE_VALUE JE_AstEval(const JE_AST ast, JE_MAP* symmap, size_t inode) {
    JE_VALUE value = JE_ValueNul();

    if(ast.astnode.nlen) {
        assert(inode < ast.astnode.nlen);

        value = JE_NCALL(ast.astnode.node+inode,eval,symmap,&ast.info,NULL);
    }

    return value;
}
