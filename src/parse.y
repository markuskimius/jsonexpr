%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "parse.h"
#include "throw.h"
#include "token.h"

extern int yylex();

void yyerror(const char* s) {
    throwtoken(newtoken(BAD_T, &yylloc), s);
}
%}

%code requires {
#define YYLTYPE_IS_DECLARED

typedef struct YYLTYPE {
    char** code;
    size_t first_pos;
    size_t first_line;
    size_t first_column;
    size_t last_pos;
    size_t last_line;
    size_t last_column;
} YYLTYPE;

# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).code         = YYRHSLOC (Rhs, 1).code;              \
          (Current).first_pos    = YYRHSLOC (Rhs, 1).first_pos;         \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_pos     = YYRHSLOC (Rhs, N).last_pos;          \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).code         = (Current).code        =              \
            YYRHSLOC (Rhs, 0).code;                                     \
          (Current).first_pos    = (Current).last_pos    =              \
            YYRHSLOC (Rhs, 0).last_pos;                                 \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
}

%locations

%token<token> NULL_T
%token<token> BOOL_T
%token<token> INT_T
%token<token> FLOAT_T
%token<token> STRING_T
%token<token> IDENT_T
%token<loc>   '('
%token<loc>   ')'
%token<loc>   '+'
%token<loc>   '-'
%token<loc>   '['
%token<loc>   ']'
%token<loc>   '{'
%token<loc>   '}'
%token        BAD_T

%start      start
%left       ';'
%left       ','
%left       ':'
%right      '='
%left       EQ_T NE_T
%left       LT_T LE_T GT_T GE_T
%left       '+' '-'
%left       '*' '/' '%'
%left       '{' '[' '(' '.'

%union {
    struct NODE* node;
    struct TOKEN* token;
    YYLTYPE loc;
}

%type<node> start
%type<node> expr
%type<node> list
%type<node> pair
%type<node> members
%type<node> symbol

%%

start       :                       { yyast = $$ = newleaf(NULL_N, NULL);   }
            | expr                  { yyast = $$ = $1;                      }
            ;

expr        : NULL_T                { $$ = newleaf(NULL_N, $1);     }
            | BOOL_T                { $$ = newleaf(BOOL_N, $1);     }
            | INT_T                 { $$ = newleaf(INT_N, $1);      }
            | FLOAT_T               { $$ = newleaf(FLOAT_N, $1);    }
            | STRING_T              { $$ = newleaf(STRING_N, $1);   }
            | expr ';'              { $$ = $1;                      }
            | '(' expr ')'          { $$ = $2;                      }
            | '[' ']'               { $$ = newnode(   ARRAY_N, NULL, NULL,      &$1,      &$2);   }
            | '[' list ']'          { $$ = newnode(   ARRAY_N,   $2, NULL,      &$1,      &$3);   }
            | '{' '}'               { $$ = newnode(  OBJECT_N, NULL, NULL,      &$1,      &$2);   }
            | '{' members '}'       { $$ = newnode(  OBJECT_N,   $2, NULL,      &$1,      &$3);   }
            | symbol                { $$ = newnode(  SYMBOL_N,   $1, NULL, &$1->loc, &$1->loc);   }
            | symbol '(' ')'        { $$ = newnode(    CALL_N,   $1, NULL, &$1->loc,      &$3);   }
            | symbol '(' list ')'   { $$ = newnode(    CALL_N,   $1,   $3, &$1->loc,      &$4);   }
            | symbol '=' expr       { $$ = newnode(       '=',   $1,   $3, &$1->loc, &$3->loc);   }
            | '+' expr              { $$ = newnode(   UPLUS_N,   $2, NULL,      &$1, &$2->loc);   }
            | '-' expr              { $$ = newnode(  UMINUS_N,   $2, NULL,      &$1, &$2->loc);   }
            | expr '+' expr         { $$ = newnode(       '+',   $1,   $3, &$1->loc, &$3->loc);   }
            | expr '-' expr         { $$ = newnode(       '-',   $1,   $3, &$1->loc, &$3->loc);   }
            | expr '*' expr         { $$ = newnode(       '*',   $1,   $3, &$1->loc, &$3->loc);   }
            | expr '/' expr         { $$ = newnode(       '/',   $1,   $3, &$1->loc, &$3->loc);   }
            | expr '%' expr         { $$ = newnode(       '%',   $1,   $3, &$1->loc, &$3->loc);   }
            | expr EQ_T expr        { $$ = newnode(      EQ_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr NE_T expr        { $$ = newnode(      NE_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr LT_T expr        { $$ = newnode(      LT_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr LE_T expr        { $$ = newnode(      LE_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr GT_T expr        { $$ = newnode(      GT_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr GE_T expr        { $$ = newnode(      GE_N,   $1,   $3, &$1->loc, &$3->loc);   }
            | expr ';' expr         { $$ = newnode(       ';',   $1,   $3, &$1->loc, &$3->loc);   }
            ;

list        : expr                  { $$ = newnode(       ',',   $1, NULL, &$1->loc, &$1->loc);   }
            | expr ','              { $$ = newnode(       ',',   $1, NULL, &$1->loc, &$1->loc);   }
            | expr ',' list         { $$ = newnode(       ',',   $1,   $3, &$1->loc, &$3->loc);   }
            ;

pair        : expr ':' expr         { $$ = newnode(       ':',   $1,   $3, &$1->loc, &$3->loc);   }
            ;

members     : pair                  { $$ = newnode(       ',',   $1, NULL, &$1->loc, &$1->loc);   }
            | pair ','              { $$ = newnode(       ',',   $1, NULL, &$1->loc, &$1->loc);   }
            | pair ',' members      { $$ = newnode(       ',',   $1,   $3, &$1->loc, &$3->loc);   }
            ;

symbol      : IDENT_T               { $$ = newleaf(IDENT_N, $1);    }
            | symbol '[' expr ']'   { $$ = newnode(       '[',   $1,   $3, &$1->loc, &$4);        }
            | symbol '.' symbol     { $$ = newnode(       '.',   $1,   $3, &$1->loc, &$3->loc);   }
            ;

%%
