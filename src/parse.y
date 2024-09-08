%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "parse.h"
#include "error.h"
#include "val.h"

extern int yylex();

void yyerror(const char* s) {
    ParseError(&yylloc, s);
}
%}

%code requires {
#define YYLTYPE_IS_DECLARED

typedef struct YYLTYPE {
    char** codeptr;
    size_t first_pos;
    size_t first_line;
    size_t first_column;
    size_t last_pos;
    size_t last_line;
    size_t last_column;
} YYLTYPE;

#define YYLLOC_DEFAULT(Current, Rhs, N) do {                    \
    if(N) {                                                     \
        (Current).codeptr      = YYRHSLOC(Rhs, 1).codeptr;      \
        (Current).first_pos    = YYRHSLOC(Rhs, 1).first_pos;    \
        (Current).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
        (Current).first_column = YYRHSLOC(Rhs, 1).first_column; \
        (Current).last_pos     = YYRHSLOC(Rhs, N).last_pos;     \
        (Current).last_line    = YYRHSLOC(Rhs, N).last_line;    \
        (Current).last_column  = YYRHSLOC(Rhs, N).last_column;  \
    }                                                           \
    else {                                                      \
        (Current).codeptr      = YYRHSLOC(Rhs, 0).codeptr;      \
        (Current).first_pos    = YYRHSLOC(Rhs, 0).first_pos;    \
        (Current).first_line   = YYRHSLOC(Rhs, 0).first_line;   \
        (Current).first_column = YYRHSLOC(Rhs, 0).first_column; \
        (Current).last_pos     = YYRHSLOC(Rhs, 0).last_pos;     \
        (Current).last_line    = YYRHSLOC(Rhs, 0).last_line;    \
        (Current).last_column  = YYRHSLOC(Rhs, 0).last_column;  \
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
    struct NODE* node;
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

start       :                           { yyast = $$ = newinode(NULL_N, 0, &yylloc);        }
            | expr                      { yyast = $$ = $1;                                  }
            ;

expr        : NULL_T                    { $$ = newinode(NULL_N, 0, &@$);                    }
            | BOOL_T                    { $$ = newinode(BOOL_N, $1, &@$);                   }
            | INT_T                     { $$ = newinode(INT_N, $1, &@$);                    }
            | FLOAT_T                   { $$ = newfnode(FLOAT_N, $1, &@$);                  }
            | STRING_T                  { $$ = newsnode(STRING_N, $1, &@$);                 }
            | expr ';'                  { $$ = $1;                                          }
            | '(' expr ')'              { $$ = $2;                                          }
            | '[' ']'                   { $$ = newnode(  ARRAY_N, NULL, NULL, NULL, &@$);   }
            | '[' list ']'              { $$ = newnode(  ARRAY_N,   $2, NULL, NULL, &@$);   }
            | '{' '}'                   { $$ = newnode( OBJECT_N, NULL, NULL, NULL, &@$);   }
            | '{' members '}'           { $$ = newnode( OBJECT_N,   $2, NULL, NULL, &@$);   }
            | symbol                    { $$ = newnode( SYMBOL_N,   $1, NULL, NULL, &@$);   }
            | symbol '(' ')'            { $$ = newnode(   CALL_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | symbol '(' list ')'       { $$ = newnode(   CALL_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol '=' expr           { $$ = newnode(      '=',   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol INC_T              { $$ = newnode(POSTINC_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | symbol DEC_T              { $$ = newnode(POSTDEC_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | INC_T symbol %prec PRE_P  { $$ = newnode( PREINC_N,   newnode(SYMBOL_N,$2,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | DEC_T symbol %prec PRE_P  { $$ = newnode( PREDEC_N,   newnode(SYMBOL_N,$2,NULL,NULL,&@$), NULL, NULL, &@$);   }
            | '+' expr %prec UNI_P      { $$ = newnode(  UPLUS_N,   $2, NULL, NULL, &@$);   }
            | '-' expr %prec UNI_P      { $$ = newnode( UMINUS_N,   $2, NULL, NULL, &@$);   }
            | '!' expr                  { $$ = newnode(      '!',   $2, NULL, NULL, &@$);   }
            | '~' expr                  { $$ = newnode(      '~',   $2, NULL, NULL, &@$);   }
            | expr '+' expr             { $$ = newnode(      '+',   $1,   $3, NULL, &@$);   }
            | expr '-' expr             { $$ = newnode(      '-',   $1,   $3, NULL, &@$);   }
            | expr '*' expr             { $$ = newnode(      '*',   $1,   $3, NULL, &@$);   }
            | expr '/' expr             { $$ = newnode(      '/',   $1,   $3, NULL, &@$);   }
            | expr '%' expr             { $$ = newnode(      '%',   $1,   $3, NULL, &@$);   }
            | expr '|' expr             { $$ = newnode(      '|',   $1,   $3, NULL, &@$);   }
            | expr '&' expr             { $$ = newnode(      '&',   $1,   $3, NULL, &@$);   }
            | expr '^' expr             { $$ = newnode(      '^',   $1,   $3, NULL, &@$);   }
            | expr EQ_T expr            { $$ = newnode(     EQ_N,   $1,   $3, NULL, &@$);   }
            | expr NE_T expr            { $$ = newnode(     NE_N,   $1,   $3, NULL, &@$);   }
            | expr LT_T expr            { $$ = newnode(     LT_N,   $1,   $3, NULL, &@$);   }
            | expr LE_T expr            { $$ = newnode(     LE_N,   $1,   $3, NULL, &@$);   }
            | expr GT_T expr            { $$ = newnode(     GT_N,   $1,   $3, NULL, &@$);   }
            | expr GE_T expr            { $$ = newnode(     GE_N,   $1,   $3, NULL, &@$);   }
            | expr OR_T expr            { $$ = newnode(     OR_N,   $1,   $3, NULL, &@$);   }
            | expr AND_T expr           { $$ = newnode(    AND_N,   $1,   $3, NULL, &@$);   }
            | expr POW_T expr           { $$ = newnode(    POW_N,   $1,   $3, NULL, &@$);   }
            | expr SHL_T expr           { $$ = newnode(    SHL_N,   $1,   $3, NULL, &@$);   }
            | expr ASR_T expr           { $$ = newnode(    ASR_N,   $1,   $3, NULL, &@$);   }
            | expr SHR_T expr           { $$ = newnode(    SHR_N,   $1,   $3, NULL, &@$);   }
            | symbol PLEQ_T expr        { $$ = newnode(   PLEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol MIEQ_T expr        { $$ = newnode(   MIEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol TIEQ_T expr        { $$ = newnode(   TIEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol DIEQ_T expr        { $$ = newnode(   DIEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol MOEQ_T expr        { $$ = newnode(   MOEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol SHLEQ_T expr       { $$ = newnode(  SHLEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol ASREQ_T expr       { $$ = newnode(  ASREQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol SHREQ_T expr       { $$ = newnode(  SHREQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol ANDEQ_T expr       { $$ = newnode(  ANDEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol XOREQ_T expr       { $$ = newnode(  XOREQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol OREQ_T expr        { $$ = newnode(   OREQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | symbol POWEQ_T expr       { $$ = newnode(  POWEQ_N,   newnode(SYMBOL_N,$1,NULL,NULL,&@$),   $3, NULL, &@$);   }
            | expr '?' expr ':' expr    { $$ = newnode(      '?',   $1,   $3,   $5, &@$);   }
            | expr ';' expr             { $$ = newnode(      ';',   $1,   $3, NULL, &@$);   }
            ;

list        : expr                      { $$ = newnode(      ',',   $1, NULL, NULL, &@$);   }
            | expr ','                  { $$ = newnode(      ',',   $1, NULL, NULL, &@$);   }
            | expr ',' list             { $$ = newnode(      ',',   $1,   $3, NULL, &@$);   }
            ;

pair        : expr ':' expr             { $$ = newnode(      ':',   $1,   $3, NULL, &@$);   }
            ;

members     : pair                      { $$ = newnode(      ',',   $1, NULL, NULL, &@$);   }
            | pair ','                  { $$ = newnode(      ',',   $1, NULL, NULL, &@$);   }
            | pair ',' members          { $$ = newnode(      ',',   $1,   $3, NULL, &@$);   }
            ;

symbol      : IDENT_T                   { $$ = newsnode(IDENT_N, $1, &@$);                  }
            | symbol '[' expr ']'       { $$ = newnode(      '[',   $1,   $3, NULL, &@$);   }
            | symbol '.' symbol         { $$ = newnode(      '.',   $1,   $3, NULL, &@$);   }
            ;

%%
