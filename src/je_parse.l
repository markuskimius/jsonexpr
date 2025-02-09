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

size_t JE_yypos = 0;
size_t JE_yycolumn = 1;
char* JE_yystrvalue = NULL;
JE_NODE* JE_ast = NULL;
JE_TOKEN* JE_last = NULL;
JE_TOKEN* JE_first = NULL;

#define YY_USER_ACTION                                                      \
    if(YY_START == INITIAL) {                                               \
        JE_last = JE_TokenNew(JE_yypos, JE_yylineno, JE_yycolumn, JE_last); \
        if(!JE_first) JE_first = JE_last;                                   \
                                                                            \
        JE_yylloc.first = JE_last;                                          \
        JE_yylloc.last = JE_last;                                           \
    }                                                                       \
    JE_yypos += JE_yyleng;                                                  \
    JE_yycolumn += JE_yyleng;                                               \
    JE_yylloc.last->text = JE_AstrCat(JE_last->text, JE_yytext);            \
    JE_yylloc.last->last_pos = JE_yypos;                                    \
    JE_yylloc.last->last_line = JE_yylineno;                                \
    JE_yylloc.last->last_column = JE_yycolumn;                              \

static void JE_yynextline() {
    JE_yycolumn = 1;
}

%}

%%

[ \r\t]+                ;
[\n]                    { JE_yynextline(); }
"null"                  { return NULL_T; }
"true"                  { JE_yylval.i = 1; return BOOL_T; }
"false"                 { JE_yylval.i = 0; return BOOL_T; }
{INT}                   { JE_yylval.i = strtoll(JE_yytext, NULL, 0); return INT_T; }
0{OCT}+                 { JE_yylval.i = strtoll(JE_yytext, NULL, 0); return INT_T; }
0[xX]{HEX}+             { JE_yylval.i = strtoll(JE_yytext, NULL, 0); return INT_T; }
{INT}{FRAC}{EXP}        { JE_yylval.f = strtod(JE_yytext, NULL); return FLOAT_T; }
{INT}{FRAC}             { JE_yylval.f = strtod(JE_yytext, NULL); return FLOAT_T; }
{INT}{EXP}              { JE_yylval.f = strtod(JE_yytext, NULL); return FLOAT_T; }
[_a-zA-Z][_a-zA-Z0-9]*  { JE_yylval.s = strdup(JE_yytext); return IDENT_T; }
"{"                     { return JE_yytext[0]; }
"}"                     { return JE_yytext[0]; }
"["                     { return JE_yytext[0]; }
"]"                     { return JE_yytext[0]; }
"("                     { return JE_yytext[0]; }
")"                     { return JE_yytext[0]; }
":"                     { return JE_yytext[0]; }
","                     { return JE_yytext[0]; }
";"                     { return JE_yytext[0]; }
"."                     { return JE_yytext[0]; }
"="                     { return JE_yytext[0]; }
"*"                     { return JE_yytext[0]; }
"/"                     { return JE_yytext[0]; }
"%"                     { return JE_yytext[0]; }
"+"                     { return JE_yytext[0]; }
"-"                     { return JE_yytext[0]; }
"?"                     { return JE_yytext[0]; }
"!"                     { return JE_yytext[0]; }
"~"                     { return JE_yytext[0]; }
"|"                     { return JE_yytext[0]; }
"&"                     { return JE_yytext[0]; }
"^"                     { return JE_yytext[0]; }
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
["]                     { BEGIN(STRING); JE_yystrvalue = JE_Calloc(1, JE_YYSTR_INITSIZE); }
<STRING>[^"\\\n]+       { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_yytext); }
<STRING>\n              { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_yytext); JE_yynextline(); }
<STRING>\\U{HEX8}       { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_UcharToCstr(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_UcharToCstr(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_UcharToCstr(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, JE_UcharToCstr(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\[\"]          { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\""); }
<STRING>\\\\            { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\\"); }
<STRING>\\[/]           { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "/");  }
<STRING>\\b             { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\b"); }
<STRING>\\f             { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\f"); }
<STRING>\\n             { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\n"); }
<STRING>\\r             { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\r"); }
<STRING>\\t             { JE_yystrvalue = JE_AstrCat(JE_yystrvalue, "\t"); }
<STRING>\\\n            { JE_yynextline(); }
<STRING>\\              { JE_ParseError(&JE_yylloc, "String with a bad backslash sequence"); return BAD_T; }
<STRING><<EOF>>         { JE_ParseError(&JE_yylloc, "String with a missing terminating quote"); return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); JE_yylval.s = JE_yystrvalue; return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); JE_yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { JE_yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { JE_ParseError(&JE_yylloc, "Multiline-comment with a missing comment terminator"); }
.                       { JE_LogicError(&JE_yylloc, "(%d): We shouldn't get here", ((unsigned) JE_yytext[0]) & 0xff); }

%%

JE_NODE* JE_Parse(const char* code) {
    /* Initialize globals */
    JE_yypos = 0;
    JE_yylineno = 1;
    JE_yycolumn = 1;
    JE_ast = NULL;
    JE_last = NULL;
    JE_first = NULL;
    JE_yystrvalue = NULL;

    /* Parse from string */
    if(code) {
        YY_BUFFER_STATE bp = JE_yy_scan_string(code);

        JE_yy_switch_to_buffer(bp);
        JE_yyparse();
        JE_yy_flush_buffer(bp);
        JE_yy_delete_buffer(bp);
    }
    /* Parse from stdin */
    else {
        JE_yyin = stdin;

        do {
            JE_yyparse();
        } while(!feof(JE_yyin));
    }

    JE_ast->head = JE_first;

    // printf("%s\n", JE_TokenToAstr(JE_first, NULL));
    // printf("%s\n", JE_NodeTreeAstr(JE_ast));

    return JE_ast;
}
