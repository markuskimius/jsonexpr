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
#include "je_parse.h"
#include "je_error.h"
#include "je_util.h"

#define YYCODE_INITSIZE 80
#define YYSTR_INITSIZE 8

char** yycodeptr = NULL;
size_t yypos = 0;
size_t yycolumn = 1;
char* yystrvalue = NULL;
JE_NODE* yyast = NULL;

#define YY_USER_ACTION                          \
    if(YY_START == INITIAL) {                   \
        yylloc.codeptr = yycodeptr;             \
        yylloc.first_pos = yypos;               \
        yylloc.first_line = yylineno;           \
        yylloc.first_column = yycolumn;         \
    }                                           \
    yypos += yyleng;                            \
    yycolumn += yyleng;                         \
    yylloc.last_pos = yypos;                    \
    yylloc.last_line = yylineno;                \
    yylloc.last_column = yycolumn;              \
    *yycodeptr = je_astrcat(*yycodeptr, yytext);   \

static void yynextline() {
    yycolumn = 1;
}

%}

%%

[ \r\t\xc2\xa0]         ; /* Ignore whitespaces */
[\n]                    { yynextline(); }
"null"                  { return NULL_T; }
"true"                  { yylval.i = 1; return BOOL_T; }
"false"                 { yylval.i = 0; return BOOL_T; }
{INT}                   { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
0{OCT}+                 { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
0[xX]{HEX}+             { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
{INT}{FRAC}{EXP}        { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
{INT}{FRAC}             { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
{INT}{EXP}              { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
[_a-zA-Z][_a-zA-Z0-9]*  { yylval.s = strdup(yytext); return IDENT_T; }
"{"                     { return yytext[0]; }
"}"                     { return yytext[0]; }
"["                     { return yytext[0]; }
"]"                     { return yytext[0]; }
"("                     { return yytext[0]; }
")"                     { return yytext[0]; }
":"                     { return yytext[0]; }
","                     { return yytext[0]; }
";"                     { return yytext[0]; }
"."                     { return yytext[0]; }
"="                     { return yytext[0]; }
"*"                     { return yytext[0]; }
"/"                     { return yytext[0]; }
"%"                     { return yytext[0]; }
"+"                     { return yytext[0]; }
"-"                     { return yytext[0]; }
"?"                     { return yytext[0]; }
"!"                     { return yytext[0]; }
"~"                     { return yytext[0]; }
"|"                     { return yytext[0]; }
"&"                     { return yytext[0]; }
"^"                     { return yytext[0]; }
"**"                    { return POW_T; }
"++"                    { return INC_T; }
"--"                    { return DEC_T; }
"=="                    { return EQ_T; }
"!="                    { return NE_T; }
"<="                    { return LE_T; }
"<"                     { return LT_T; }
">="                    { return GE_T; }
">"                     { return GT_T; }
"||"                    { return OR_T;  }
"&&"                    { return AND_T; }
"<<"                    { return SHL_T; }
">>"                    { return ASR_T; }
">>>"                   { return SHR_T; }
"+="                    { return PLEQ_T; }
"-="                    { return MIEQ_T; }
"*="                    { return TIEQ_T; }
"/="                    { return DIEQ_T; }
"%="                    { return MOEQ_T; }
"<<="                   { return SHLEQ_T; }
">>="                   { return ASREQ_T; }
">>>="                  { return SHREQ_T; }
"&="                    { return ANDEQ_T; }
"^="                    { return XOREQ_T; }
"|="                    { return OREQ_T; }
"**="                   { return POWEQ_T; }
["]                     { BEGIN(STRING); yystrvalue = calloc(1, YYSTR_INITSIZE); }
<STRING>[^"\\\n]+       { yystrvalue = je_astrcat(yystrvalue, yytext); }
<STRING>\n              { yystrvalue = je_astrcat(yystrvalue, yytext); yynextline(); }
<STRING>\\U{HEX8}       { yystrvalue = je_astrcat(yystrvalue, je_utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { yystrvalue = je_astrcat(yystrvalue, je_utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { yystrvalue = je_astrcat(yystrvalue, je_utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { yystrvalue = je_astrcat(yystrvalue, je_utf8str(strtoll(yytext+2, NULL, 16))); }
<STRING>\\[\"]          { yystrvalue = je_astrcat(yystrvalue, "\""); }
<STRING>\\\\            { yystrvalue = je_astrcat(yystrvalue, "\\"); }
<STRING>\\[/]           { yystrvalue = je_astrcat(yystrvalue, "/");  }
<STRING>\\b             { yystrvalue = je_astrcat(yystrvalue, "\b"); }
<STRING>\\f             { yystrvalue = je_astrcat(yystrvalue, "\f"); }
<STRING>\\n             { yystrvalue = je_astrcat(yystrvalue, "\n"); }
<STRING>\\r             { yystrvalue = je_astrcat(yystrvalue, "\r"); }
<STRING>\\t             { yystrvalue = je_astrcat(yystrvalue, "\t"); }
<STRING>\\\n            { yynextline(); }
<STRING>\\              { JeParseError(&yylloc, "String with a bad backslash sequence"); return BAD_T; }
<STRING><<EOF>>         { JeParseError(&yylloc, "String with a missing terminating quote"); return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); yylval.s = yystrvalue; return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { JeParseError(&yylloc, "Multiline-comment with a missing comment terminator"); }
.                       { JeLogicError(&yylloc, "(%d): We shouldn't get here", ((unsigned) yytext[0]) & 0xff); }

%%

JE_NODE* je_parse(const char* code) {
    /* Initialize globals */
    yypos = 0;
    yylineno = 1;
    yycolumn = 1;
    yyast = NULL;
    yystrvalue = NULL;
    yycodeptr = calloc(1, sizeof(char*));
    *yycodeptr = calloc(1, YYCODE_INITSIZE);

    /* Parse from string */
    if(code) {
        YY_BUFFER_STATE bp = yy_scan_string(code);

        yy_switch_to_buffer(bp);
        yyparse();
        yy_flush_buffer(bp);
        yy_delete_buffer(bp);
    }
    /* Parse from stdin */
    else {
        yyin = stdin;

        do {
            yyparse();
        } while(!feof(yyin));
    }

    /* do not free yycodeptr, used by yyast */

    return yyast;
}