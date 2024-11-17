%{
#include <stdio.h>
#include <stdlib.h>
#include "je_node.h"
#include "je_parse.h"
#include "je_error.h"
#include "je_val.h"

extern int yylex();

void yyerror(const char* s) {
    JeParseError(&yylloc, s);
}
%}

%code requires {
#define JE_YYLTYPE_IS_DECLARED

typedef struct JE_YYLTYPE {
    struct JE_TOKEN* first;
    struct JE_TOKEN* last;
} JE_YYLTYPE;

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

%token    NULL_T
%token<i> BOOL_T
%token<i> INT_T
%token<f> FLOAT_T
%token<s> STRING_T
%token<s> IDENT_T
%token    BAD_T

%start    start
%left     ';'
%left     ','
%right    '=' PLEQ_T MIEQ_T TIEQ_T DIEQ_T MOEQ_T SHLEQ_T ASREQ_T SHREQ_T ANDEQ_T XOREQ_T OREQ_T POWEQ_T
%left     ':' '?'
%left     OR_T
%left     AND_T
%left     '|'
%left     '^'
%left     '&'
%left     EQ_T NE_T
%left     LT_T LE_T GT_T GE_T
%left     SHL_T ASR_T SHR_T
%left     '+' '-'
%left     '*' '/' '%'
%right    POW_T
%right    INC_T DEC_T UNI_P '!' '~'
%left     PRE_P '(' '[' '.' '{'

%union {
    struct JE_NODE* node;
    int64_t i;
    double f;
    char* s;
}

%type<node> start
%type<node> expr
%type<node> list
%type<node> pair
%type<node> members

%%

start       :                           { je_ast = $$ = je_newinode(JE_NULL_N, 0, &je_yylloc);    }
            | expr                      { je_ast = $$ = $1;                                       }
            ;

expr        : NULL_T                    { $$ = je_newinode(JE_NULL_N, 0, &@$);                    }
            | BOOL_T                    { $$ = je_newinode(JE_BOOL_N, $1, &@$);                   }
            | INT_T                     { $$ = je_newinode(JE_INT_N, $1, &@$);                    }
            | FLOAT_T                   { $$ = je_newfnode(JE_FLOAT_N, $1, &@$);                  }
            | STRING_T                  { $$ = je_newsnode(JE_STRING_N, $1, &@$);                 }
            | expr ';'                  { $$ = $1;                                                }
            | '(' expr ')'              { $$ = $2;                                                }
            | '[' ']'                   { $$ = je_newnode(  JE_ARRAY_N, NULL, NULL, NULL, &@$);   }
            | '[' list ']'              { $$ = je_newnode(  JE_ARRAY_N,   $2, NULL, NULL, &@$);   }
            | '{' '}'                   { $$ = je_newnode( JE_OBJECT_N, NULL, NULL, NULL, &@$);   }
            | '{' members '}'           { $$ = je_newnode( JE_OBJECT_N,   $2, NULL, NULL, &@$);   }
            | expr '(' ')'              { $$ = je_newnode(   JE_CALL_N,   $1, NULL, NULL, &@$);   }
            | expr '(' list ')'         { $$ = je_newnode(   JE_CALL_N,   $1,   $3, NULL, &@$);   }
            | IDENT_T                   { $$ = je_newsnode(JE_IDENT_N, $1, &@$);                  }
            | expr '.' IDENT_T          { $$ = je_newnode(         '.',   $1, je_newsnode(JE_IDENT_N,$3,&@$), NULL, &@$);   }
            | expr '[' expr ']'         { $$ = je_newnode(         '[',   $1,   $3, NULL, &@$);   }
            | expr '=' expr             { $$ = je_newnode(         '=',   $1,   $3, NULL, &@$);   }
            | expr INC_T                { $$ = je_newnode(JE_POSTINC_N,   $1, NULL, NULL, &@$);   }
            | expr DEC_T                { $$ = je_newnode(JE_POSTDEC_N,   $1, NULL, NULL, &@$);   }
            | INC_T expr %prec PRE_P    { $$ = je_newnode( JE_PREINC_N,   $2, NULL, NULL, &@$);   }
            | DEC_T expr %prec PRE_P    { $$ = je_newnode( JE_PREDEC_N,   $2, NULL, NULL, &@$);   }
            | '+' expr %prec UNI_P      { $$ = je_newnode(  JE_UPLUS_N,   $2, NULL, NULL, &@$);   }
            | '-' expr %prec UNI_P      { $$ = je_newnode( JE_UMINUS_N,   $2, NULL, NULL, &@$);   }
            | '!' expr                  { $$ = je_newnode(         '!',   $2, NULL, NULL, &@$);   }
            | '~' expr                  { $$ = je_newnode(         '~',   $2, NULL, NULL, &@$);   }
            | expr '+' expr             { $$ = je_newnode(         '+',   $1,   $3, NULL, &@$);   }
            | expr '-' expr             { $$ = je_newnode(         '-',   $1,   $3, NULL, &@$);   }
            | expr '*' expr             { $$ = je_newnode(         '*',   $1,   $3, NULL, &@$);   }
            | expr '/' expr             { $$ = je_newnode(         '/',   $1,   $3, NULL, &@$);   }
            | expr '%' expr             { $$ = je_newnode(         '%',   $1,   $3, NULL, &@$);   }
            | expr '|' expr             { $$ = je_newnode(         '|',   $1,   $3, NULL, &@$);   }
            | expr '&' expr             { $$ = je_newnode(         '&',   $1,   $3, NULL, &@$);   }
            | expr '^' expr             { $$ = je_newnode(         '^',   $1,   $3, NULL, &@$);   }
            | expr EQ_T expr            { $$ = je_newnode(     JE_EQ_N,   $1,   $3, NULL, &@$);   }
            | expr NE_T expr            { $$ = je_newnode(     JE_NE_N,   $1,   $3, NULL, &@$);   }
            | expr LT_T expr            { $$ = je_newnode(     JE_LT_N,   $1,   $3, NULL, &@$);   }
            | expr LE_T expr            { $$ = je_newnode(     JE_LE_N,   $1,   $3, NULL, &@$);   }
            | expr GT_T expr            { $$ = je_newnode(     JE_GT_N,   $1,   $3, NULL, &@$);   }
            | expr GE_T expr            { $$ = je_newnode(     JE_GE_N,   $1,   $3, NULL, &@$);   }
            | expr OR_T expr            { $$ = je_newnode(     JE_OR_N,   $1,   $3, NULL, &@$);   }
            | expr AND_T expr           { $$ = je_newnode(    JE_AND_N,   $1,   $3, NULL, &@$);   }
            | expr POW_T expr           { $$ = je_newnode(    JE_POW_N,   $1,   $3, NULL, &@$);   }
            | expr SHL_T expr           { $$ = je_newnode(    JE_SHL_N,   $1,   $3, NULL, &@$);   }
            | expr ASR_T expr           { $$ = je_newnode(    JE_ASR_N,   $1,   $3, NULL, &@$);   }
            | expr SHR_T expr           { $$ = je_newnode(    JE_SHR_N,   $1,   $3, NULL, &@$);   }
            | expr PLEQ_T expr          { $$ = je_newnode(   JE_PLEQ_N,   $1,   $3, NULL, &@$);   }
            | expr MIEQ_T expr          { $$ = je_newnode(   JE_MIEQ_N,   $1,   $3, NULL, &@$);   }
            | expr TIEQ_T expr          { $$ = je_newnode(   JE_TIEQ_N,   $1,   $3, NULL, &@$);   }
            | expr DIEQ_T expr          { $$ = je_newnode(   JE_DIEQ_N,   $1,   $3, NULL, &@$);   }
            | expr MOEQ_T expr          { $$ = je_newnode(   JE_MOEQ_N,   $1,   $3, NULL, &@$);   }
            | expr SHLEQ_T expr         { $$ = je_newnode(  JE_SHLEQ_N,   $1,   $3, NULL, &@$);   }
            | expr ASREQ_T expr         { $$ = je_newnode(  JE_ASREQ_N,   $1,   $3, NULL, &@$);   }
            | expr SHREQ_T expr         { $$ = je_newnode(  JE_SHREQ_N,   $1,   $3, NULL, &@$);   }
            | expr ANDEQ_T expr         { $$ = je_newnode(  JE_ANDEQ_N,   $1,   $3, NULL, &@$);   }
            | expr XOREQ_T expr         { $$ = je_newnode(  JE_XOREQ_N,   $1,   $3, NULL, &@$);   }
            | expr OREQ_T expr          { $$ = je_newnode(   JE_OREQ_N,   $1,   $3, NULL, &@$);   }
            | expr POWEQ_T expr         { $$ = je_newnode(  JE_POWEQ_N,   $1,   $3, NULL, &@$);   }
            | expr '?' expr ':' expr    { $$ = je_newnode(         '?',   $1,   $3,   $5, &@$);   }
            | expr ';' expr             { $$ = je_newnode(         ';',   $1,   $3, NULL, &@$);   }
            ;

list        : expr                      { $$ = je_newnode(         ',',   $1, NULL, NULL, &@$);   }
            | expr ','                  { $$ = je_newnode(         ',',   $1, NULL, NULL, &@$);   }
            | expr ',' list             { $$ = je_newnode(         ',',   $1,   $3, NULL, &@$);   }
            ;

pair        : expr ':' expr             { $$ = je_newnode(         ':',   $1,   $3, NULL, &@$);   }
            ;

members     : pair                      { $$ = je_newnode(         ',',   $1, NULL, NULL, &@$);   }
            | pair ','                  { $$ = je_newnode(         ',',   $1, NULL, NULL, &@$);   }
            | pair ',' members          { $$ = je_newnode(         ',',   $1,   $3, NULL, &@$);   }
            ;

%%
