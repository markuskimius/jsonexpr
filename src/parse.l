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
#include "parse.h"
#include "node.h"
#include "util.h"

#define YYSTRING_MINSIZE 80

int yycolumn = 1;
char* yystring = NULL;
const char* yyerrtext = NULL;
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
"null"                  { return NULL_T; }
"true"                  { yylval.i = 1; return BOOL_T; }
"false"                 { yylval.i = 0; return BOOL_T; }
[-+]?{INT}{FRAC}{EXP}   { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
[-+]?{INT}{FRAC}        { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
[-+]?{INT}{EXP}         { yylval.f = strtod(yytext, NULL); return FLOAT_T; }
[-+]?{INT}              { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
[-+]?0{OCT}+            { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
[-+]?0[xX]{HEX}+        { yylval.i = strtoll(yytext, NULL, 0); return INT_T; }
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
["]                     { BEGIN(STRING); yystring[0] = '\0'; }
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
<STRING>\\              { yyerrtext = "String with bad backslash sequence"; return BAD_T; }
<STRING><<EOF>>         { yyerrtext = "String missing terminating quote"; return BAD_T; }
<STRING>\"              { BEGIN(INITIAL); yylval.s = strdup(yystring); return STRING_T; }
"#"                     { BEGIN(COMMENT); }
<COMMENT>.              ;
<COMMENT>\n             { BEGIN(INITIAL); yynextline(); }
"/*"                    { BEGIN(COMMENT2); }
<COMMENT2>.             ;
<COMMENT2>\n            { yynextline(); }
<COMMENT2><<EOF>>       { yyerrtext = "Multiline-comment missing comment terminator"; return BAD_T; }
<COMMENT2>"*/"          { BEGIN(INITIAL); }
.                       { fprintf(stderr, "<%s>\n", yytext); yyerrtext = "We shouldn't get here"; return BAD_T; }

%%

NODE* parse(const char* code) {
    yystring = calloc(1, YYSTRING_MINSIZE);

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

    if(yystring) {
        free(yystring);
        yystring = NULL;
    }

    return yyast;
}
