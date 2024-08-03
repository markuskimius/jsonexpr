%{
#include <stdio.h>
#include <stdlib.h>
#include "node.h"
#include "parse.h"

extern int yylex();
extern char* yytext;
extern const char* yyerrtext;
extern int yylineno;
extern int yyline;
extern int yycolumn;
extern NODE* yyast;

void yyerror(const char* s) {
    const char* errtext = yyerrtext ? yyerrtext : s;

    fprintf(stderr, "%s: %s at line %d, col %d\n", yytext, errtext, yylloc.first_line, yylloc.first_column);
    exit(1);
}
%}

%locations

%token      NULL_T
%token<i>   BOOL_T
%token<i>   INT_T
%token<f>   FLOAT_T
%token<s>   STRING_T
%token<s>   IDENT_T
%token      BAD_T

%start      start
%left       ';'
%left       ','
%left       ':'
%right      '='
%left       '+' '-'
%left       '*' '/' '%'
%left       '{' '[' '(' '.'

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

start       :                       { yyast = $$ = newleaf(NULL_N, NULL);   }
            | expr                  { yyast = $$ = $1;                      }
            ;

expr        : NULL_T                { $$ = newleaf(NULL_N, NULL);           }
            | BOOL_T                { $$ = newleaf(BOOL_N, &$1);            }
            | INT_T                 { $$ = newleaf(INT_N, &$1);             }
            | FLOAT_T               { $$ = newleaf(FLOAT_N, &$1);           }
            | STRING_T              { $$ = newleaf(STRING_N, $1);           }
            | expr ';'              { $$ = $1;                              }
            | '(' expr ')'          { $$ = $2;                              }
            | '[' ']'               { $$ = newnode(ARRAY_N, NULL, NULL);    }
            | '[' list ']'          { $$ = newnode(ARRAY_N, $2, NULL);      }
            | '{' '}'               { $$ = newnode(OBJECT_N, NULL, NULL);   }
            | '{' members '}'       { $$ = newnode(OBJECT_N, $2, NULL);     }
            | symbol                { $$ = newnode(SYMBOL_N, $1, NULL);     }
            | symbol '(' ')'        { $$ = newnode(CALL_N, $1, NULL);       }
            | symbol '(' list ')'   { $$ = newnode(CALL_N, $1, $3);         }
            | symbol '=' expr       { $$ = newnode('=', $1, $3);            }
            | '+' expr              { $$ = newnode(UPLUS_N, $2, NULL);      }
            | '-' expr              { $$ = newnode(UMINUS_N, $2, NULL);     }
            | expr '+' expr         { $$ = newnode('+', $1, $3);            }
            | expr '-' expr         { $$ = newnode('-', $1, $3);            }
            | expr '*' expr         { $$ = newnode('*', $1, $3);            }
            | expr '/' expr         { $$ = newnode('/', $1, $3);            }
            | expr '%' expr         { $$ = newnode('%', $1, $3);            }
            | expr ';' expr         { $$ = newnode(';', $1, $3);            }
            ;

list        : expr                  { $$ = newnode(',', $1, NULL);          }
            | expr ','              { $$ = newnode(',', $1, NULL);          }
            | expr ',' list         { $$ = newnode(',', $1, $3);            }
            ;

pair        : expr ':' expr         { $$ = newnode(':', $1, $3);            }
            ;

members     : pair                  { $$ = newnode(',', $1, NULL);          }
            | pair ','              { $$ = newnode(',', $1, NULL);          }
            | pair ',' members      { $$ = newnode(',', $1, $3);            }
            ;

symbol      : IDENT_T               { $$ = newleaf(IDENT_N, $1);            }
            | symbol '[' expr ']'   { $$ = newnode('[', $1, $3);            }
            | symbol '.' symbol     { $$ = newnode('.', $1, $3);            }
            ;

%%
