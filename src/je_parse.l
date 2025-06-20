%option noyywrap
%option nodefault
%option yylineno

%x STRING
%x COMMENT
%x COMMENT2

BIN     ([01])
OCT     ([0-7])
HEX     ([0-9a-fA-F])
HEX2    ([0-9a-fA-F][0-9a-fA-F])
HEX4    ([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])
HEX8    ([0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F])

INT     ([1-9][0-9]*|0)
FRAC    ([.][0-9]+)
EXP     ([eE][-+]?[0-9]+)

%{
#include "je_ast.h"
#include "je_chr.h"
#include "je_error.h"
#include "je_parse.h"
#include "je_parse.tab.h"
#include "je_token.h"
#include "je_util.h"
#include "je_value.h"

JE_AST JE_ast;
JE_STR JE_str;
JE_TOKEN* JE_token;
size_t JE_yypos = 0;
size_t JE_yycolumn = 1;

#define YY_USER_ACTION                                                                                      \
    if(YY_START == INITIAL) {                                                                               \
        JE_token = JE_AstInfoPushToken(&JE_ast.info, JE_TokenCreate(JE_yypos, JE_yylineno, JE_yycolumn));   \
                                                                                                            \
        JE_yylloc.first = JE_ast.info.tlen-1;                                                               \
        JE_yylloc.last = JE_ast.info.tlen-1;                                                                \
    }                                                                                                       \
    JE_yypos += JE_yyleng;                                                                                  \
    JE_yycolumn += JE_yyleng;                                                                               \
    JE_token->last_pos = JE_yypos;                                                                          \
    JE_token->last_line = JE_yylineno;                                                                      \
    JE_token->last_column = JE_yycolumn;                                                                    \
    JE_StrPushCstr(&JE_token->text, JE_yytext);                                                             \

#define JE_SYNTAX_ERROR(args...) do {   \
    JE_SyntaxError(args);               \
    JE_ast.isok = 0;                    \
    return 0;                           \
} while(0)

static void JE_yynextline() {
    JE_yycolumn = 1;
}
%}

%%

[ \r\t]+                ;
[\n]                    { JE_yynextline(); }
"null"                  { JE_yylval.value = JE_ValueNul(); return JE_NUL_TT; }
"true"                  { JE_yylval.value = JE_ValueTru(1); return JE_TRU_TT; }
"false"                 { JE_yylval.value = JE_ValueTru(0); return JE_TRU_TT; }
{INT}                   { JE_yylval.value = JE_ValueI64(strtoll(JE_yytext, NULL, 0)); return JE_I64_TT; }
0{OCT}+                 { JE_yylval.value = JE_ValueI64(JE_OctToI64(JE_yytext+1)); return JE_I64_TT; }
0[bB]{BIN}+             { JE_yylval.value = JE_ValueI64(JE_BinToI64(JE_yytext+2)); return JE_I64_TT; }
0[xX]{HEX}+             { JE_yylval.value = JE_ValueI64(JE_HexToI64(JE_yytext+2)); return JE_I64_TT; }
{INT}{FRAC}{EXP}        { JE_yylval.value = JE_ValueF64(strtod(JE_yytext, NULL)); return JE_F64_TT; }
{INT}{FRAC}             { JE_yylval.value = JE_ValueF64(strtod(JE_yytext, NULL)); return JE_F64_TT; }
{INT}{EXP}              { JE_yylval.value = JE_ValueF64(strtod(JE_yytext, NULL)); return JE_F64_TT; }
[_a-zA-Z][_a-zA-Z0-9]*  { JE_yylval.ident = JE_StrCreate(JE_Strdup(JE_yytext)); return JE_IDENT_TT; }
"!"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"!="                    { JE_yylval.token = *JE_token; return JE_NE; }
"%"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"&"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"&&"                    { JE_yylval.token = *JE_token; return JE_AND; }
"("                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
")"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"*"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"**"                    { JE_yylval.token = *JE_token; return JE_POW; }
"+"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"++"                    { JE_yylval.token = *JE_token; return JE_INC; }
","                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"-"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"--"                    { JE_yylval.token = *JE_token; return JE_DEC; }
"."                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"/"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
":"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
";"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"<"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"<<"                    { JE_yylval.token = *JE_token; return JE_SHL; }
"<="                    { JE_yylval.token = *JE_token; return JE_LE; }
"="                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"=="                    { JE_yylval.token = *JE_token; return JE_EQ; }
">"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
">="                    { JE_yylval.token = *JE_token; return JE_GE; }
">>"                    { JE_yylval.token = *JE_token; return JE_ASR; }
">>>"                   { JE_yylval.token = *JE_token; return JE_SHR; }
"?"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"["                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"]"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"^"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"{"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"|"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"||"                    { JE_yylval.token = *JE_token; return JE_OR; }
"}"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }
"~"                     { JE_yylval.token = *JE_token; return JE_yytext[0]; }

"%="                    { JE_yylval.token = *JE_token; return JE_MODEQ; }
"&="                    { JE_yylval.token = *JE_token; return JE_ANDEQ; }
"*="                    { JE_yylval.token = *JE_token; return JE_TIMEQ; }
"**="                   { JE_yylval.token = *JE_token; return JE_POWEQ; }
"+="                    { JE_yylval.token = *JE_token; return JE_INCEQ; }
"-="                    { JE_yylval.token = *JE_token; return JE_DECEQ; }
"/="                    { JE_yylval.token = *JE_token; return JE_DIVEQ; }
"<<="                   { JE_yylval.token = *JE_token; return JE_SHLEQ; }
">>="                   { JE_yylval.token = *JE_token; return JE_ASREQ; }
">>>="                  { JE_yylval.token = *JE_token; return JE_SHREQ; }
"^="                    { JE_yylval.token = *JE_token; return JE_XOREQ; }
"|="                    { JE_yylval.token = *JE_token; return JE_OREQ; }

["]                     { BEGIN(STRING); JE_str = JE_StrCreate(NULL); }
<STRING>[^"\\\n]+       { JE_StrPushCstr(&JE_str, JE_yytext); }
<STRING>\n              { JE_StrPushCstr(&JE_str, JE_yytext); JE_yynextline(); }
<STRING>\\U{HEX8}       { JE_StrPushChr(&JE_str, JE_ChrCreate(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\u{HEX4}       { JE_StrPushChr(&JE_str, JE_ChrCreate(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\x{HEX2}       { JE_StrPushChr(&JE_str, JE_ChrCreate(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\X{HEX2}       { JE_StrPushChr(&JE_str, JE_ChrCreate(strtoll(JE_yytext+2, NULL, 16))); }
<STRING>\\[\"]          { JE_StrPushCstr(&JE_str, "\""); }
<STRING>\\\\            { JE_StrPushCstr(&JE_str, "\\"); }
<STRING>\\[/]           { JE_StrPushCstr(&JE_str, "/");  }
<STRING>\\b             { JE_StrPushCstr(&JE_str, "\b"); }
<STRING>\\f             { JE_StrPushCstr(&JE_str, "\f"); }
<STRING>\\n             { JE_StrPushCstr(&JE_str, "\n"); }
<STRING>\\r             { JE_StrPushCstr(&JE_str, "\r"); }
<STRING>\\t             { JE_StrPushCstr(&JE_str, "\t"); }
<STRING>\\\n            { JE_yynextline(); }
<STRING>\\              { JE_SYNTAX_ERROR(JE_token, JE_token+1, "String with a bad escape sequence"); }
<STRING><<EOF>>         { JE_SYNTAX_ERROR(JE_token, JE_token+1, "String with a missing terminating quote"); }
<STRING>\"              { BEGIN(INITIAL); JE_yylval.value = JE_ValueStr(JE_str); return JE_STR_TT; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); JE_yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { JE_yynextline(); }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
<COMMENT2><<EOF>>       { JE_SYNTAX_ERROR(JE_token, JE_token+1, "Multiline-comment with a missing comment terminator"); }
.                       { JE_SYNTAX_ERROR(JE_token, JE_token+1, "Invalid token `%s` (%d)", JE_yytext, ((unsigned) JE_yytext[0]) & 0xff); }

%%

JE_AST JE_Parse(const char* code) {
    /* Initialize globals */
    JE_ast = JE_AstCreate();
    JE_token = NULL;
    JE_yypos = 0;
    JE_yylineno = 1;
    JE_yycolumn = 1;

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

    return JE_ast;
}
