%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "ast.h"
#include "map.h"
#include "eval.h"
#include "util.h"
#include "value.h"
#include "vector.h"
#include "jsonexpr.tab.h"

extern int yylex();
extern char* yytext;
extern const char* yyerrtext;
extern int yylineno;
extern int yyline;
extern int yycolumn;
extern AST_NODE* yyast;

void yyerror(const char* s) {
    const char* errtext = yyerrtext ? yyerrtext : s;

    fprintf(stderr, "%s: %s at line %d, col %d\n", yytext, errtext, yylloc.first_line, yylloc.first_column);
    exit(1);
}
%}

%locations

%token<i64>  INT64_T
%token<f64>  DOUBLE_T
%token<str>  STRING_T
%token<name> IDENT_T
%token       UPLUS_T
%token       UMINUS_T
%token       BAD_T

%start       start
%left        ';'
%left        ','
%left        ':'
%right       '='
%left        '+' '-'
%left        '*' '/' '%'
%left        '{' '[' '(' '.'


%union {
    AST_NODE* node;
    int64_t i64;
    double f64;
    char* str;
    char* name;
}

%type<node>   start
%type<node>   expr
%type<node>   list
%type<node>   pairlist
%type<node>   pair
%type<node>   symbol

%%

start       :                       { yyast = $$ = newleaf(NULL_N, NULL);       }
            | expr                  { yyast = $$ = $1;                          }
            ;

expr        : INT64_T               { $$ = newleaf(INT64_N, &$1);               }
            | DOUBLE_T              { $$ = newleaf(DOUBLE_N, &$1);              }
            | STRING_T              { $$ = newleaf(STRING_N, $1);               }
            | expr ';'              { $$ = $1;                                  }
            | '(' expr ')'          { $$ = $2;                                  }
            | '[' ']'               { $$ = newnode(ARRAY_N, NULL, NULL);        }
            | '[' list ']'          { $$ = newnode(ARRAY_N, $2, NULL);          }
            | '{' '}'               { $$ = newnode(OBJECT_N, NULL, NULL);       }
            | '{' pairlist '}'      { $$ = newnode(OBJECT_N, $2, NULL);         }
            | symbol                { $$ = newnode(LVALUE_N, $1, NULL);         }
            | symbol '(' ')'        { $$ = newnode(FNCALL_N, $1, NULL);         }
            | symbol '(' list ')'   { $$ = newnode(FNCALL_N, $1, $3);           }
            | symbol '=' expr       { $$ = newnode('=', $1, $3);                }
            | '+' expr              { $$ = newnode(UPLUS_N, $2, NULL);          }
            | '-' expr              { $$ = newnode(UMINUS_N, $2, NULL);         }
            | expr '+' expr         { $$ = newnode('+', $1, $3);                }
            | expr '-' expr         { $$ = newnode('-', $1, $3);                }
            | expr '*' expr         { $$ = newnode('*', $1, $3);                }
            | expr '/' expr         { $$ = newnode('/', $1, $3);                }
            | expr '%' expr         { $$ = newnode('%', $1, $3);                }
            | expr ';' expr         { $$ = newnode(';', $1, $3);                }
            ;

list        : expr                  { $$ = newnode(',', $1, NULL);              }
            | expr ','              { $$ = newnode(',', $1, NULL);              }
            | expr ',' list         { $$ = newnode(',', $1, $3);                }
            ;

pairlist    : pair                  { $$ = newnode(',', $1, NULL);              }
            | pair ','              { $$ = newnode(',', $1, NULL);              }
            | pair ',' pairlist     { $$ = newnode(',', $1, $3);                }
            ;

pair        : IDENT_T ':' expr      { $$ = newnode(':', newleaf(IDENT_N, $1), $3); }
            ;

symbol      : IDENT_T               { $$ = newleaf(IDENT_N, $1);                }
            | symbol '[' expr ']'   { $$ = newnode('[', $1, $3);                }
            | symbol '.' symbol     { $$ = newnode('.', $1, $3);                }
            ;

%%

int main() {
    AST_NODE* ast = parse(NULL);
    VALUE* result = eval(ast, NULL);
    char* buf = NULL;

    switch(result->type) {
        case NULL_V     : printf("NULL\n"); break;
        case INT64_V    : printf("i64 : %lld\n", result->value.i64); break;
        case DOUBLE_V   : printf("f64 : %lf\n", result->value.f64);  break;
        case STRING_V   : printf("str : %s\n", buf = astrencode(result->value.str)); break;
        case ARRAY_V    : printf("arr : %s\n", buf = astrvec(result->value.vec)); break;
        case OBJECT_V   : printf("obj : %s\n", buf = astrmap(result->value.map)); break;
        case BUILTIN_V  : printf("bfn : %s\n", buf = strdup("FUNCTION")); break;
        case USERFUNC_V : printf("ufn : %s\n", buf = strdup("FUNCTION")); break;
        default         : fprintf(stderr, "Invalid value type: '%c' (%d)\n", result->type, result->type); break;
    }

    if(buf) free(buf);

    return 0;
}
