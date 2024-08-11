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

char* yycode = NULL;
char* yystring = NULL;
size_t yypos = 0;
size_t yycolumn = 1;
YYLTYPE yystart;
NODE* yyast;

#define YY_USER_ACTION                          \
    if(YY_START == INITIAL) {                   \
        yylloc.code = &yycode;                  \
        yylloc.first_pos = yypos;               \
        yylloc.first_line = yylineno;           \
        yylloc.first_column = yycolumn;         \
        yylloc.last_pos = yypos;                \
    }                                           \
    yycode = astrcat(yycode, yytext);           \
    yypos += yyleng;                            \
    yylloc.last_pos = yypos;                    \
    yylloc.last_line = yylineno;                \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;

static void yynextline() {
    yycolumn = 1;
}

%}

%%

[ \r\t]                 ; /* Ignore whitespaces */
[\n]                    { yynextline(); }
"null"                  { yylval.token = newtoken(NULL_T, &yylloc); return NULL_T; }
"true"                  { yylval.token = inttoken(BOOL_T, &yylloc, 1); return BOOL_T; }
"false"                 { yylval.token = inttoken(BOOL_T, &yylloc, 0); return BOOL_T; }
{INT}                   { yylval.token = inttoken(INT_T, &yylloc, strtoll(yytext, NULL, 0)); return INT_T; }
0{OCT}+                 { yylval.token = inttoken(INT_T, &yylloc, strtoll(yytext, NULL, 0)); return INT_T; }
0[xX]{HEX}+             { yylval.token = inttoken(INT_T, &yylloc, strtoll(yytext, NULL, 0)); return INT_T; }
{INT}{FRAC}{EXP}        { yylval.token = dbltoken(FLOAT_T, &yylloc, strtod(yytext, NULL)); return FLOAT_T; }
{INT}{FRAC}             { yylval.token = dbltoken(FLOAT_T, &yylloc, strtod(yytext, NULL)); return FLOAT_T; }
{INT}{EXP}              { yylval.token = dbltoken(FLOAT_T, &yylloc, strtod(yytext, NULL)); return FLOAT_T; }
[_a-zA-Z][_a-zA-Z0-9]*  { yylval.token = strtoken(IDENT_T, &yylloc, yytext); return IDENT_T; }
"{"                     { yylval.loc = yylloc; return yytext[0]; }
"}"                     { yylval.loc = yylloc; return yytext[0]; }
"["                     { yylval.loc = yylloc; return yytext[0]; }
"]"                     { yylval.loc = yylloc; return yytext[0]; }
"("                     { yylval.loc = yylloc; return yytext[0]; }
")"                     { yylval.loc = yylloc; return yytext[0]; }
":"                     { yylval.loc = yylloc; return yytext[0]; }
","                     { yylval.loc = yylloc; return yytext[0]; }
";"                     { yylval.loc = yylloc; return yytext[0]; }
"."                     { yylval.loc = yylloc; return yytext[0]; }
"="                     { yylval.loc = yylloc; return yytext[0]; }
"*"                     { yylval.loc = yylloc; return yytext[0]; }
"/"                     { yylval.loc = yylloc; return yytext[0]; }
"%"                     { yylval.loc = yylloc; return yytext[0]; }
"+"                     { yylval.loc = yylloc; return yytext[0]; }
"-"                     { yylval.loc = yylloc; return yytext[0]; }
"=="                    { yylval.loc = yylloc; return EQ_T; }
"!="                    { yylval.loc = yylloc; return NE_T; }
"<="                    { yylval.loc = yylloc; return LE_T; }
"<"                     { yylval.loc = yylloc; return LT_T; }
">="                    { yylval.loc = yylloc; return GE_T; }
">"                     { yylval.loc = yylloc; return GT_T; }
["]                     { BEGIN(STRING); yylloc = yystart; yystring = calloc(1, YYSTRING_MINSIZE); }
<STRING>[^"\\\n]+       { yystring = astrcat(yystring, yytext); }
<STRING>\n              { yystring = astrcat(yystring, yytext); yynextline(); }
<STRING>\\U{HEX8}       { yystring = astrcat(yystring, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { yystring = astrcat(yystring, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { yystring = astrcat(yystring, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { yystring = astrcat(yystring, utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\[\"]          { yystring = astrcat(yystring, "\""); }
<STRING>\\\\            { yystring = astrcat(yystring, "\\"); }
<STRING>\\[/]           { yystring = astrcat(yystring, "/");  }
<STRING>\\b             { yystring = astrcat(yystring, "\b"); }
<STRING>\\f             { yystring = astrcat(yystring, "\f"); }
<STRING>\\n             { yystring = astrcat(yystring, "\n"); }
<STRING>\\r             { yystring = astrcat(yystring, "\r"); }
<STRING>\\t             { yystring = astrcat(yystring, "\t"); }
<STRING>\\\n            { yynextline(); }
<STRING>\\              { throwtoken(newtoken(BAD_T, &yylloc), "String with bad backslash sequence"); return BAD_T; }
<STRING><<EOF>>         { throwtoken(newtoken(BAD_T, &yylloc), "String missing terminating quote"); return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); yylval.token = astrtoken(STRING_T, &yystart, &yylloc, yystring); return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { throwtoken(newtoken(BAD_T, &yylloc), "Multiline-comment missing comment terminator"); }
.                       { throwtoken(newtoken(BAD_T, &yylloc), "We shouldn't get here"); }

%%

NODE* parse(const char* code) {
    yycode = calloc(1, 80);
    yypos = 0;
    yylineno = 1;
    yycolumn = 1;
//    yydebug = 1;

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
