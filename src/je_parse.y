%{
#include <stdint.h>
#include "je_ast.h"
#include "je_error.h"
#include "je_node.h"
#include "je_parse.h"
#include "je_token.h"

extern int yylex();

void yyerror(const char* s) {
    JE_SyntaxError(JE_token, JE_token+1, "%s", s);
    JE_ast.isok = 0;
}
%}

%code requires {
#include "je_ast.h"
#include "je_str.h"
#include "je_token.h"
#include "je_value.h"

#define YYLLOC_DEFAULT(Current, Rhs, N) do {                    \
    if(N) {                                                     \
        (Current).first        = YYRHSLOC(Rhs, 1).first;        \
        (Current).last         = YYRHSLOC(Rhs, N).last;         \
    }                                                           \
    else {                                                      \
        (Current).first        = YYRHSLOC(Rhs, 0).first;        \
        (Current).last         = YYRHSLOC(Rhs, 0).last;         \
    }                                                           \
} while(0)
}

%locations

%token<value> JE_F64_TT
%token<value> JE_I64_TT
%token<value> JE_NUL_TT
%token<value> JE_STR_TT
%token<value> JE_TRU_TT
%token<ident> JE_IDENT_TT
%token<token> '!'
%token<token> '&'
%token<token> '('
%token<token> '*'
%token<token> '+'
%token<token> ','
%token<token> '-'
%token<token> '.'
%token<token> '/'
%token<token> ':'
%token<token> ';'
%token<token> '<'
%token<token> '='
%token<token> '>'
%token<token> '['
%token<token> '^'
%token<token> '{'
%token<token> '|'
%token<token> '~'
%token<token> JE_AND
%token<token> JE_ASR
%token<token> JE_DEC
%token<token> JE_EQ
%token<token> JE_GE
%token<token> JE_INC
%token<token> JE_LE
%token<token> JE_NE
%token<token> JE_OR
%token<token> JE_POW
%token<token> JE_SHL
%token<token> JE_SHR

%token<token> JE_ANDEQ
%token<token> JE_ASREQ
%token<token> JE_DECEQ
%token<token> JE_DIVEQ
%token<token> JE_INCEQ
%token<token> JE_MODEQ
%token<token> JE_OREQ
%token<token> JE_POWEQ
%token<token> JE_SHLEQ
%token<token> JE_SHREQ
%token<token> JE_TIMEQ
%token<token> JE_XOREQ

%start    start
%left     ';'
%left     ','
%right    '=' JE_ANDEQ JE_ASREQ JE_DECEQ JE_DIVEQ JE_INCEQ JE_MODEQ JE_OREQ JE_POWEQ JE_SHLEQ JE_SHREQ JE_TIMEQ JE_XOREQ
%right    '?' ':'
%left     JE_OR
%left     JE_AND
%left     '|'
%left     '^'
%left     '&'
%left     JE_EQ JE_NE
%left     '<' '>' JE_LE JE_GE
%left     JE_ASR JE_SHL JE_SHR
%left     '+' '-'
%left     '*' '/' '%'
%right    JE_POW
%right    JE_INC JE_DEC JE_UNIP '!' '~'
%left     JE_SUFP '(' '.' '[' '{'

%union {
    JE_AST     ast;
    JE_ASTNODE astnode;
    JE_STR     ident;
    JE_TOKEN   token;
    JE_VALUE   value;
}

%type<ast>     start
%type<astnode> expr
%type<astnode> list
%type<astnode> pair
%type<astnode> pairlist

%%

start       : %empty                    { /* no op */           $$ = JE_ast; }
            | expr                      { JE_ast.astnode = $1;  $$ = JE_ast; }
            ;

expr        : '(' expr ')'              { $$ = JE_AstNodeParen($2, &JE_ast.info, @1, @2, @3); }
            | '!' expr                  { $$ = JE_AstNodeLogicalNot($2, &JE_ast.info, @1, @2); }
            | '+' expr %prec JE_UNIP    { $$ = JE_AstNodePos($2, &JE_ast.info, @1, @2); }
            | '-' expr %prec JE_UNIP    { $$ = JE_AstNodeNeg($2, &JE_ast.info, @1, @2); }
            | '~' expr                  { $$ = JE_AstNodeBitwiseInv($2, &JE_ast.info, @1, @2); }
            | '[' ']'                   { $$ = JE_AstNodeMtArray(&JE_ast.info, @1, @2); }
            | '[' list ']'              { $$ = JE_AstNodeArray($2, &JE_ast.info, @1, @2, @3); }
            | '[' list ',' ']'          { $$ = JE_AstNodeArray2($2, &JE_ast.info, @1, @2, @3, @4); }
            | '{' '}'                   { $$ = JE_AstNodeMtObject(&JE_ast.info, @1, @2); }
            | '{' pairlist '}'          { $$ = JE_AstNodeObject($2, &JE_ast.info, @1, @2, @3); }
            | '{' pairlist ',' '}'      { $$ = JE_AstNodeObject2($2, &JE_ast.info, @1, @2, @3, @4); }
            | JE_F64_TT                 { $$ = JE_AstNodeVal($1, &JE_ast.info, @1); }
            | JE_I64_TT                 { $$ = JE_AstNodeVal($1, &JE_ast.info, @1); }
            | JE_NUL_TT                 { $$ = JE_AstNodeVal($1, &JE_ast.info, @1); }
            | JE_STR_TT                 { $$ = JE_AstNodeVal($1, &JE_ast.info, @1); }
            | JE_TRU_TT                 { $$ = JE_AstNodeVal($1, &JE_ast.info, @1); }
            | JE_IDENT_TT               { $$ = JE_AstNodeIdent($1, &JE_ast.info, @1); }
            | JE_DEC expr               { $$ = JE_AstNodePreDec($2, &JE_ast.info, @1, @2); }
            | JE_INC expr               { $$ = JE_AstNodePreInc($2, &JE_ast.info, @1, @2); }
            | expr '%' expr             { $$ = JE_AstNodeMod($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '&' expr             { $$ = JE_AstNodeBitwiseAnd($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '(' ')'              { $$ = JE_AstNodeCall($1, &JE_ast.info, @1, @2, @3); }
            | expr '(' list ')'         { $$ = JE_AstNodeCall2($1, $3, &JE_ast.info, @1, @2, @3, @4); }
            | expr '*' expr             { $$ = JE_AstNodeTimes($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '+' expr             { $$ = JE_AstNodePlus($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '-' expr             { $$ = JE_AstNodeMinus($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '.' JE_IDENT_TT      { $$ = JE_AstNodeMember($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '/' expr             { $$ = JE_AstNodeDivby($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr ';'                  { $$ = JE_AstNodeStmt($1, &JE_ast.info, @1, @2); }
            | expr ';' expr             { $$ = JE_AstNodeStmt2($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '<' expr             { $$ = JE_AstNodeLess($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '=' expr             { $$ = JE_AstNodeAssn($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '>' expr             { $$ = JE_AstNodeGreater($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '?' expr ':' expr    { $$ = JE_AstNodeTernary($1, $3, $5, &JE_ast.info, @1, @2, @3, @4, @5); }
            | expr '[' expr ']'         { $$ = JE_AstNodeIndex($1, $3, &JE_ast.info, @1, @2, @3, @4); }
            | expr '^' expr             { $$ = JE_AstNodeBitwiseXor($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr '|' expr             { $$ = JE_AstNodeBitwiseOr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_AND expr          { $$ = JE_AstNodeLogicalAnd($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_ASR expr          { $$ = JE_AstNodeAsr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_DEC %prec JE_SUFP { $$ = JE_AstNodePostDec($1, &JE_ast.info, @1, @2); }
            | expr JE_EQ expr           { $$ = JE_AstNodeEqual($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_GE expr           { $$ = JE_AstNodeGreaterEq($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_INC %prec JE_SUFP { $$ = JE_AstNodePostInc($1, &JE_ast.info, @1, @2); }
            | expr JE_LE expr           { $$ = JE_AstNodeLessEq($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_NE expr           { $$ = JE_AstNodeNotEqual($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_OR expr           { $$ = JE_AstNodeLogicalOr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_POW expr          { $$ = JE_AstNodePow($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_SHL expr          { $$ = JE_AstNodeShl($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_SHR expr          { $$ = JE_AstNodeShr($1, $3, &JE_ast.info, @1, @2, @3); }

            | expr JE_ANDEQ expr        { $$ = JE_AstNodeAssnBitAnd($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_ASREQ expr        { $$ = JE_AstNodeAssnAsr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_DECEQ expr        { $$ = JE_AstNodeAssnMinus($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_DIVEQ expr        { $$ = JE_AstNodeAssnDivby($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_INCEQ expr        { $$ = JE_AstNodeAssnPlus($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_MODEQ expr        { $$ = JE_AstNodeAssnMod($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_OREQ  expr        { $$ = JE_AstNodeAssnBitOr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_POWEQ expr        { $$ = JE_AstNodeAssnPow($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_SHLEQ expr        { $$ = JE_AstNodeAssnShl($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_SHREQ expr        { $$ = JE_AstNodeAssnShr($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_TIMEQ expr        { $$ = JE_AstNodeAssnTimes($1, $3, &JE_ast.info, @1, @2, @3); }
            | expr JE_XOREQ expr        { $$ = JE_AstNodeAssnBitXor($1, $3, &JE_ast.info, @1, @2, @3); }

            | error ';' expr            { $$ = $3; }
            ;

list        : expr                      { $$ = JE_AstNodeList($1, &JE_ast.info, @1);  }
            | list ',' expr             { $$ = JE_AstNodeList2($1, $3, &JE_ast.info, @1, @2, @3); }
            ;

pair        : expr ':' expr             { $$ = JE_AstNodePair($1, $3, &JE_ast.info, @1, @2, @3); }
            ;

pairlist    : pair                      { $$ = JE_AstNodePairlist($1, &JE_ast.info, @1); }
            | pairlist ',' pair         { $$ = JE_AstNodePairlist2($1, $3, &JE_ast.info, @1, @2, @3); }
            ;

%%
