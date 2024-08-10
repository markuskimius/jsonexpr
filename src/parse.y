%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "parse.h"
#include "throw.h"
#include "token.h"

extern int yylex();

void yyerror(const char* s) {
    throw(newtoken(BAD_T, &yylloc, yytext), s);
}
%}

%locations

%token<token> NULL_T
%token<token> BOOL_T
%token<token> INT_T
%token<token> FLOAT_T
%token<token> STRING_T
%token<token> IDENT_T
%token<token> '{'
%token<token> '['
%token<token> '('
%token<token> ':'
%token<token> ','
%token<token> ';'
%token<token> '.'
%token<token> '='
%token<token> '*'
%token<token> '/'
%token<token> '%'
%token<token> '+'
%token<token> '-'
%token<token> EQ_T
%token<token> NE_T
%token<token> LT_T
%token<token> LE_T
%token<token> GT_T
%token<token> GE_T
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
}

%type<node> start
%type<node> expr
%type<node> list
%type<node> pair
%type<node> members
%type<node> symbol

%%

start       :                       { yyast = $$ = newleaf(NULL_N, NULL);           }
            | expr                  { yyast = $$ = $1;                              }
            ;

expr        : NULL_T                { $$ = newleaf(NULL_N, $1);                     }
            | BOOL_T                { $$ = newleaf(BOOL_N, $1);                     }
            | INT_T                 { $$ = newleaf(INT_N, $1);                      }
            | FLOAT_T               { $$ = newleaf(FLOAT_N, $1);                    }
            | STRING_T              { $$ = newleaf(STRING_N, $1);                   }
            | expr ';'              { $$ = $1; freetoken($2);                       }
            | '(' expr ')'          { $$ = $2; freetoken($1);                       }
            | '[' ']'               { $$ = newnode(ARRAY_N, $1, NULL, NULL);        }
            | '[' list ']'          { $$ = newnode(ARRAY_N, $1, $2, NULL);          }
            | '{' '}'               { $$ = newnode(OBJECT_N, $1, NULL, NULL);       }
            | '{' members '}'       { $$ = newnode(OBJECT_N, $1, $2, NULL);         }
            | symbol                { $$ = newnode(SYMBOL_N, $1->token, $1, NULL);  }
            | symbol '(' ')'        { $$ = newnode(CALL_N, $2, $1, NULL);           }
            | symbol '(' list ')'   { $$ = newnode(CALL_N, $2, $1, $3);             }
            | symbol '=' expr       { $$ = newnode('=', $2, $1, $3);                }
            | '+' expr              { $$ = newnode(UPLUS_N, $1, $2, NULL);          }
            | '-' expr              { $$ = newnode(UMINUS_N, $1, $2, NULL);         }
            | expr '+' expr         { $$ = newnode('+', $2, $1, $3);                }
            | expr '-' expr         { $$ = newnode('-', $2, $1, $3);                }
            | expr '*' expr         { $$ = newnode('*', $2, $1, $3);                }
            | expr '/' expr         { $$ = newnode('/', $2, $1, $3);                }
            | expr '%' expr         { $$ = newnode('%', $2, $1, $3);                }
            | expr EQ_T expr        { $$ = newnode(EQ_N, $2, $1, $3);               }
            | expr NE_T expr        { $$ = newnode(NE_N, $2, $1, $3);               }
            | expr LT_T expr        { $$ = newnode(LT_N, $2, $1, $3);               }
            | expr LE_T expr        { $$ = newnode(LE_N, $2, $1, $3);               }
            | expr GT_T expr        { $$ = newnode(GT_N, $2, $1, $3);               }
            | expr GE_T expr        { $$ = newnode(GE_N, $2, $1, $3);               }
            | expr ';' expr         { $$ = newnode(';', $2, $1, $3);                }
            ;

list        : expr                  { $$ = newnode(',', $1->token, $1, NULL);       }
            | expr ','              { $$ = newnode(',', $2, $1, NULL);              }
            | expr ',' list         { $$ = newnode(',', $2, $1, $3);                }
            ;

pair        : expr ':' expr         { $$ = newnode(':', $2, $1, $3);                }
            ;

members     : pair                  { $$ = newnode(',', $1->token, $1, NULL);       }
            | pair ','              { $$ = newnode(',', $2, $1, NULL);              }
            | pair ',' members      { $$ = newnode(',', $2, $1, $3);                }
            ;

symbol      : IDENT_T               { $$ = newleaf(IDENT_N, $1);                    }
            | symbol '[' expr ']'   { $$ = newnode('[', $2, $1, $3);                }
            | symbol '.' symbol     { $$ = newnode('.', $2, $1, $3);                }
            ;

%%
