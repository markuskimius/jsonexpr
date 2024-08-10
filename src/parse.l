%option noyywrap
%option nodefault
%option yylineno

%x STRING
%x COMMENT
%x COMMENT2

OCT     ([0-7])
HEX     ([0-9a-fA-F])
HEX2    ([0-9a-fA-F][0-9a-fA-F])
HEX4    ([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])
HEX8    ([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])

INT     ([1-9][0-9]*|0)
FRAC    ([.][0-9]+)
EXP     ([eE][-+]?[0-9]+)

%{
#include "node.h"
#include "parse.h"
#include "throw.h"
#include "token.h"
#include "util.h"

#define YYSTRING_MINSIZE 8

int yycolumn = 1;
char* yyens = NULL;
char* yydes = NULL;
YYLTYPE yystart;
NODE* yyast;

#define YY_USER_ACTION                  \
    if(YY_START == INITIAL) {           \
        yylloc.first_line = yylineno;   \
        yylloc.first_column = yycolumn; \
    }                                   \
    yylloc.last_line = yylineno;        \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

static void yynextline() {
    yycolumn = 1;
}

%}

%%

[ \r\t]                 ; /* Ignore whitespaces */
[\n]                    { yynextline(); }
"null"                  { yylval.token = newtoken(NULL_T, &yylloc, yytext); return NULL_T; }
"true"                  { yylval.token = inttoken(BOOL_T, &yylloc, yytext, 1); return BOOL_T; }
"false"                 { yylval.token = inttoken(BOOL_T, &yylloc, yytext, 0); return BOOL_T; }
{INT}                   { yylval.token = inttoken(INT_T, &yylloc, yytext, strtoll(yytext, NULL, 0)); return INT_T; }
0{OCT}+                 { yylval.token = inttoken(INT_T, &yylloc, yytext, strtoll(yytext, NULL, 0)); return INT_T; }
0[xX]{HEX}+             { yylval.token = inttoken(INT_T, &yylloc, yytext, strtoll(yytext, NULL, 0)); return INT_T; }
{INT}{FRAC}{EXP}        { yylval.token = dbltoken(FLOAT_T, &yylloc, yytext, strtod(yytext, NULL)); return FLOAT_T; }
{INT}{FRAC}             { yylval.token = dbltoken(FLOAT_T, &yylloc, yytext, strtod(yytext, NULL)); return FLOAT_T; }
{INT}{EXP}              { yylval.token = dbltoken(FLOAT_T, &yylloc, yytext, strtod(yytext, NULL)); return FLOAT_T; }
[_a-zA-Z][_a-zA-Z0-9]*  { yylval.token = strtoken(IDENT_T, &yylloc, yytext, yytext); return IDENT_T; }
"{"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"}"                     { return yytext[0]; }
"["                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"]"                     { return yytext[0]; }
"("                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
")"                     { return yytext[0]; }
":"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
","                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
";"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"."                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"="                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"*"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"/"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"%"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"+"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"-"                     { yylval.token = newtoken(yytext[0], &yylloc, yytext); return yytext[0]; }
"=="                    { yylval.token = newtoken(EQ_T, &yylloc, yytext); return EQ_T; }
"!="                    { yylval.token = newtoken(NE_T, &yylloc, yytext); return NE_T; }
"<="                    { yylval.token = newtoken(LE_T, &yylloc, yytext); return LE_T; }
"<"                     { yylval.token = newtoken(LT_T, &yylloc, yytext); return LT_T; }
">="                    { yylval.token = newtoken(GE_T, &yylloc, yytext); return GE_T; }
">"                     { yylval.token = newtoken(GT_T, &yylloc, yytext); return GT_T; }
["]                     { BEGIN(STRING); yyens = strdup(yytext); yydes = calloc(1, YYSTRING_MINSIZE); }
<STRING>[^"\\\n]+       { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, yytext); }
<STRING>\n              { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, yytext); yynextline(); }
<STRING>\\U{HEX8}       { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\[\"]          { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\""); }
<STRING>\\\\            { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\\"); }
<STRING>\\[/]           { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "/");  }
<STRING>\\b             { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\b"); }
<STRING>\\f             { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\f"); }
<STRING>\\n             { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\n"); }
<STRING>\\r             { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\r"); }
<STRING>\\t             { yyens = astrcat(yyens, yytext); yydes = astrcat(yydes, "\t"); }
<STRING>\\\n            { yyens = astrcat(yyens, yytext); yynextline(); }
<STRING>\\              { throw(newtoken(BAD_T, &yylloc, yytext), "String with bad backslash sequence"); return BAD_T; }
<STRING><<EOF>>         { throw(newtoken(BAD_T, &yylloc, yytext), "String missing terminating quote"); return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); yylval.token = astrtoken(STRING_T, &yystart, &yylloc, yyens, yydes); return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { throw(newtoken(BAD_T, &yylloc, yytext), "Multiline-comment missing comment terminator"); }
.                       { throw(newtoken(BAD_T, &yylloc, yytext), "We shouldn't get here"); }

%%

NODE* parse(const char* code) {
    if(code) {
        YY_BUFFER_STATE bp = yy_scan_string(code);

        yy_switch_to_buffer(bp);
        yyparse();
        yy_flush_buffer(bp);
        yy_delete_buffer(bp);
    }
    else {
        yyin = stdin;

        do {
            yyparse();
        } while(!feof(yyin));
    }

    return yyast;
}
