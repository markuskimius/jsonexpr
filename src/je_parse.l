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
#include "je_token.h"
#include "je_error.h"
#include "je_util.h"
#include "je_node.h"

#define JE_YYSTR_INITSIZE 8

size_t je_yypos = 0;
size_t je_yycolumn = 1;
char* je_yystrvalue = NULL;
JE_NODE* je_ast = NULL;
JE_TOKEN* je_last = NULL;
JE_TOKEN* je_first = NULL;

#define YY_USER_ACTION                                                      \
    if(YY_START == INITIAL) {                                               \
        je_last = je_newtoken(je_yypos, je_yylineno, je_yycolumn, je_last); \
        if(!je_first) je_first = je_last;                                   \
                                                                            \
        je_yylloc.first = je_last;                                          \
        je_yylloc.last = je_last;                                           \
    }                                                                       \
    je_yypos += je_yyleng;                                                  \
    je_yycolumn += je_yyleng;                                               \
    je_yylloc.last->text = je_astrcat(je_last->text, je_yytext);            \
    je_yylloc.last->last_pos = je_yypos;                                    \
    je_yylloc.last->last_line = je_yylineno;                                \
    je_yylloc.last->last_column = je_yycolumn;                              \

static void je_yynextline() {
    je_yycolumn = 1;
}

%}

%%

[ \r\t\xc2\xa0]         ; /* Ignore whitespaces */
[\n]                    { je_yynextline(); }
"null"                  { return NULL_T; }
"true"                  { je_yylval.i = 1; return BOOL_T; }
"false"                 { je_yylval.i = 0; return BOOL_T; }
{INT}                   { je_yylval.i = strtoll(je_yytext, NULL, 0); return INT_T; }
0{OCT}+                 { je_yylval.i = strtoll(je_yytext, NULL, 0); return INT_T; }
0[xX]{HEX}+             { je_yylval.i = strtoll(je_yytext, NULL, 0); return INT_T; }
{INT}{FRAC}{EXP}        { je_yylval.f = strtod(je_yytext, NULL); return FLOAT_T; }
{INT}{FRAC}             { je_yylval.f = strtod(je_yytext, NULL); return FLOAT_T; }
{INT}{EXP}              { je_yylval.f = strtod(je_yytext, NULL); return FLOAT_T; }
[_a-zA-Z][_a-zA-Z0-9]*  { je_yylval.s = strdup(je_yytext); return IDENT_T; }
"{"                     { return je_yytext[0]; }
"}"                     { return je_yytext[0]; }
"["                     { return je_yytext[0]; }
"]"                     { return je_yytext[0]; }
"("                     { return je_yytext[0]; }
")"                     { return je_yytext[0]; }
":"                     { return je_yytext[0]; }
","                     { return je_yytext[0]; }
";"                     { return je_yytext[0]; }
"."                     { return je_yytext[0]; }
"="                     { return je_yytext[0]; }
"*"                     { return je_yytext[0]; }
"/"                     { return je_yytext[0]; }
"%"                     { return je_yytext[0]; }
"+"                     { return je_yytext[0]; }
"-"                     { return je_yytext[0]; }
"?"                     { return je_yytext[0]; }
"!"                     { return je_yytext[0]; }
"~"                     { return je_yytext[0]; }
"|"                     { return je_yytext[0]; }
"&"                     { return je_yytext[0]; }
"^"                     { return je_yytext[0]; }
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
["]                     { BEGIN(STRING); je_yystrvalue = calloc(1, JE_YYSTR_INITSIZE); }
<STRING>[^"\\\n]+       { je_yystrvalue = je_astrcat(je_yystrvalue, je_yytext); }
<STRING>\n              { je_yystrvalue = je_astrcat(je_yystrvalue, je_yytext); je_yynextline(); }
<STRING>\\U{HEX8}       { je_yystrvalue = je_astrcat(je_yystrvalue, je_utf8str(strtoll(je_yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { je_yystrvalue = je_astrcat(je_yystrvalue, je_utf8str(strtoll(je_yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { je_yystrvalue = je_astrcat(je_yystrvalue, je_utf8str(strtoll(je_yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { je_yystrvalue = je_astrcat(je_yystrvalue, je_utf8str(strtoll(je_yytext+2, NULL, 16))); }
<STRING>\\[\"]          { je_yystrvalue = je_astrcat(je_yystrvalue, "\""); }
<STRING>\\\\            { je_yystrvalue = je_astrcat(je_yystrvalue, "\\"); }
<STRING>\\[/]           { je_yystrvalue = je_astrcat(je_yystrvalue, "/");  }
<STRING>\\b             { je_yystrvalue = je_astrcat(je_yystrvalue, "\b"); }
<STRING>\\f             { je_yystrvalue = je_astrcat(je_yystrvalue, "\f"); }
<STRING>\\n             { je_yystrvalue = je_astrcat(je_yystrvalue, "\n"); }
<STRING>\\r             { je_yystrvalue = je_astrcat(je_yystrvalue, "\r"); }
<STRING>\\t             { je_yystrvalue = je_astrcat(je_yystrvalue, "\t"); }
<STRING>\\\n            { je_yynextline(); }
<STRING>\\              { JeParseError(&je_yylloc, "String with a bad backslash sequence"); return BAD_T; }
<STRING><<EOF>>         { JeParseError(&je_yylloc, "String with a missing terminating quote"); return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); je_yylval.s = je_yystrvalue; return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); je_yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { je_yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { JeParseError(&je_yylloc, "Multiline-comment with a missing comment terminator"); }
.                       { JeLogicError(&je_yylloc, "(%d): We shouldn't get here", ((unsigned) je_yytext[0]) & 0xff); }

%%

JE_NODE* je_parse(const char* code) {
    /* Initialize globals */
    je_yypos = 0;
    je_yylineno = 1;
    je_yycolumn = 1;
    je_ast = NULL;
    je_last = NULL;
    je_first = NULL;
    je_yystrvalue = NULL;

    /* Parse from string */
    if(code) {
        YY_BUFFER_STATE bp = je_yy_scan_string(code);

        je_yy_switch_to_buffer(bp);
        je_yyparse();
        je_yy_flush_buffer(bp);
        je_yy_delete_buffer(bp);
    }
    /* Parse from stdin */
    else {
        je_yyin = stdin;

        do {
            je_yyparse();
        } while(!feof(je_yyin));
    }

    je_ast->head = je_first;

    // printf("%s\n", je_astrtoken(je_first, NULL));
    // printf("%s\n", je_nodetree(je_ast));

    return je_ast;
}
