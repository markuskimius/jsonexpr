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
    char** codeptr;
    size_t first_pos;
    size_t first_line;
    size_t first_column;
    size_t last_pos;
    size_t last_line;
    size_t last_column;
} JE_YYLTYPE;

#define JE_YYLLOC_DEFAULT(Current, Rhs, N) do {                    \
    if(N) {                                                        \
        (Current).codeptr      = JE_YYRHSLOC(Rhs, 1).codeptr;      \
        (Current).first_pos    = JE_YYRHSLOC(Rhs, 1).first_pos;    \
        (Current).first_line   = JE_YYRHSLOC(Rhs, 1).first_line;   \
        (Current).first_column = JE_YYRHSLOC(Rhs, 1).first_column; \
        (Current).last_pos     = JE_YYRHSLOC(Rhs, N).last_pos;     \
        (Current).last_line    = JE_YYRHSLOC(Rhs, N).last_line;    \
        (Current).last_column  = JE_YYRHSLOC(Rhs, N).last_column;  \
    }                                                              \
    else {                                                         \
        (Current).codeptr      = JE_YYRHSLOC(Rhs, 0).codeptr;      \
        (Current).first_pos    = JE_YYRHSLOC(Rhs, 0).first_pos;    \
        (Current).first_line   = JE_YYRHSLOC(Rhs, 0).first_line;   \
        (Current).first_column = JE_YYRHSLOC(Rhs, 0).first_column; \
        (Current).last_pos     = JE_YYRHSLOC(Rhs, 0).last_pos;     \
        (Current).last_line    = JE_YYRHSLOC(Rhs, 0).last_line;    \
        (Current).last_column  = JE_YYRHSLOC(Rhs, 0).last_column;  \
    }                                                              \
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
%type<node> symbol

%%

start       :                           { je_ast = $$ = je_newinode(JE_NULL_N, 0, &je_yylloc);       }
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
            | symbol                    { $$ = je_newnode( JE_SYMBOL_N,   $1, NULL, NULL, &@$);   }
            | symbol '(' ')'            { $$ = je_newnode(   JE_CALL_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | symbol '(' list ')'       { $$ = je_newnode(   JE_CALL_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol '=' expr           { $$ = je_newnode(         '=',   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol INC_T              { $$ = je_newnode(JE_POSTINC_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | symbol DEC_T              { $$ = je_newnode(JE_POSTDEC_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | INC_T symbol %prec PRE_P  { $$ = je_newnode( JE_PREINC_N,   je_newnode(JE_SYMBOL_N,$2,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | DEC_T symbol %prec PRE_P  { $$ = je_newnode( JE_PREDEC_N,   je_newnode(JE_SYMBOL_N,$2,NULL,NULL,&@$), NULL, NULL, &@$);   }
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
            | symbol PLEQ_T expr        { $$ = je_newnode(   JE_PLEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol MIEQ_T expr        { $$ = je_newnode(   JE_MIEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol TIEQ_T expr        { $$ = je_newnode(   JE_TIEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol DIEQ_T expr        { $$ = je_newnode(   JE_DIEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol MOEQ_T expr        { $$ = je_newnode(   JE_MOEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol SHLEQ_T expr       { $$ = je_newnode(  JE_SHLEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol ASREQ_T expr       { $$ = je_newnode(  JE_ASREQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol SHREQ_T expr       { $$ = je_newnode(  JE_SHREQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol ANDEQ_T expr       { $$ = je_newnode(  JE_ANDEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol XOREQ_T expr       { $$ = je_newnode(  JE_XOREQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol OREQ_T expr        { $$ = je_newnode(   JE_OREQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol POWEQ_T expr       { $$ = je_newnode(  JE_POWEQ_N,   je_newnode(JE_SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
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

symbol      : IDENT_T                   { $$ = je_newsnode(JE_IDENT_N, $1, &@$);                  }
            | symbol '[' expr ']'       { $$ = je_newnode(         '[',   $1,   $3, NULL, &@$);   }
            | symbol '.' symbol         { $$ = je_newnode(         '.',   $1,   $3, NULL, &@$);   }
            ;

%%
