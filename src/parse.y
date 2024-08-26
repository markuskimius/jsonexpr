%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "parse.h"
#include "throw.h"
#include "value.h"

extern int yylex();

void yyerror(const char* s) {
    throw(&yylloc, s);
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
%left     ':'
%right    '='
%left     EQ_T NE_T
%left     LT_T LE_T GT_T GE_T
%left     '+' '-'
%left     '*' '/' '%'
%left     '{' '[' '(' '.' INC_T DEC_T

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

start       :                       { yyast = $$ = newinode(NULL_N, 0, &yylloc);            }
            | expr                  { yyast = $$ = $1;                                      }
            ;

expr        : NULL_T                { $$ = newinode(NULL_N, 0, &@$);                        }
            | BOOL_T                { $$ = newinode(BOOL_N, $1, &@$);                       }
            | INT_T                 { $$ = newinode(INT_N, $1, &@$);                        }
            | FLOAT_T               { $$ = newfnode(FLOAT_N, $1, &@$);                      }
            | STRING_T              { $$ = newsnode(STRING_N, $1, &@$);                     }
            | expr ';'              { $$ = $1;                                              }
            | '(' expr ')'          { $$ = $2;                                              }
            | '[' ']'               { $$ = newnode(  ARRAY_N, NULL, NULL, &@$);             }
            | '[' list ']'          { $$ = newnode(  ARRAY_N,   $2, NULL, &@$);             }
            | '{' '}'               { $$ = newnode( OBJECT_N, NULL, NULL, &@$);             }
            | '{' members '}'       { $$ = newnode( OBJECT_N,   $2, NULL, &@$);             }
            | symbol                { $$ = newnode( SYMBOL_N,   $1, NULL, &@$);             }
            | symbol '(' ')'        { $$ = newnode(   CALL_N,   $1, NULL, &@$);             }
            | symbol '(' list ')'   { $$ = newnode(   CALL_N,   $1,   $3, &@$);             }
            | symbol '=' expr       { $$ = newnode(      '=',   $1,   $3, &@$);             }
            | symbol INC_T          { $$ = newnode(POSTINC_N,   $1, NULL, &@$);             }
            | symbol DEC_T          { $$ = newnode(POSTDEC_N,   $1, NULL, &@$);             }
            | INC_T symbol          { $$ = newnode( PREINC_N,   $2, NULL, &@$);             }
            | DEC_T symbol          { $$ = newnode( PREDEC_N,   $2, NULL, &@$);             }
            | '+' expr              { $$ = newnode(  UPLUS_N,   $2, NULL, &@$);             }
            | '-' expr              { $$ = newnode( UMINUS_N,   $2, NULL, &@$);             }
            | expr '+' expr         { $$ = newnode(      '+',   $1,   $3, &@$);             }
            | expr '-' expr         { $$ = newnode(      '-',   $1,   $3, &@$);             }
            | expr '*' expr         { $$ = newnode(      '*',   $1,   $3, &@$);             }
            | expr '/' expr         { $$ = newnode(      '/',   $1,   $3, &@$);             }
            | expr '%' expr         { $$ = newnode(      '%',   $1,   $3, &@$);             }
            | expr EQ_T expr        { $$ = newnode(     EQ_N,   $1,   $3, &@$);             }
            | expr NE_T expr        { $$ = newnode(     NE_N,   $1,   $3, &@$);             }
            | expr LT_T expr        { $$ = newnode(     LT_N,   $1,   $3, &@$);             }
            | expr LE_T expr        { $$ = newnode(     LE_N,   $1,   $3, &@$);             }
            | expr GT_T expr        { $$ = newnode(     GT_N,   $1,   $3, &@$);             }
            | expr GE_T expr        { $$ = newnode(     GE_N,   $1,   $3, &@$);             }
            | expr ';' expr         { $$ = newnode(      ';',   $1,   $3, &@$);             }
            ;

list        : expr                  { $$ = newnode(      ',',   $1, NULL, &@$);             }
            | expr ','              { $$ = newnode(      ',',   $1, NULL, &@$);             }
            | expr ',' list         { $$ = newnode(      ',',   $1,   $3, &@$);             }
            ;

pair        : expr ':' expr         { $$ = newnode(      ':',   $1,   $3, &@$);             }
            ;

members     : pair                  { $$ = newnode(      ',',   $1, NULL, &@$);             }
            | pair ','              { $$ = newnode(      ',',   $1, NULL, &@$);             }
            | pair ',' members      { $$ = newnode(      ',',   $1,   $3, &@$);             }
            ;

symbol      : IDENT_T               { $$ = newsnode(  IDENT_N, $1, &@$);                    }
            | symbol '[' expr ']'   { $$ = newnode(      '[',   $1,   $3, &@$);             }
            | symbol '.' symbol     { $$ = newnode(      '.',   $1,   $3, &@$);             }
            ;

%%
