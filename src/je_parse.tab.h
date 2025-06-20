/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_JE_YY_JE_PARSE_TAB_H_INCLUDED
# define YY_JE_YY_JE_PARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef JE_YYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define JE_YYDEBUG 1
#  else
#   define JE_YYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define JE_YYDEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined JE_YYDEBUG */
#if JE_YYDEBUG
extern int JE_yydebug;
#endif
/* "%code requires" blocks.  */
#line 17 "je_parse.y"

#include "je_ast.h"
#include "je_str.h"
#include "je_token.h"
#include "je_value.h"

#define YYLLOC_DEFAULT(Current, Rhs, N) do {                    \
    if(N) {                                                     \
        (Current).first        = YYRHSLOC(Rhs, 1).first;        \
        (Current).last         = YYRHSLOC(Rhs, N).last;         \
    }                                                           \
    else {                                                      \
        (Current).first        = YYRHSLOC(Rhs, 0).first;        \
        (Current).last         = YYRHSLOC(Rhs, 0).last;         \
    }                                                           \
} while(0)

#line 75 "je_parse.tab.h"

/* Token kinds.  */
#ifndef JE_YYTOKENTYPE
# define JE_YYTOKENTYPE
  enum JE_yytokentype
  {
    JE_YYEMPTY = -2,
    JE_YYEOF = 0,                  /* "end of file"  */
    JE_YYerror = 256,              /* error  */
    JE_YYUNDEF = 257,              /* "invalid token"  */
    JE_F64_TT = 258,               /* JE_F64_TT  */
    JE_I64_TT = 259,               /* JE_I64_TT  */
    JE_NUL_TT = 260,               /* JE_NUL_TT  */
    JE_STR_TT = 261,               /* JE_STR_TT  */
    JE_TRU_TT = 262,               /* JE_TRU_TT  */
    JE_IDENT_TT = 263,             /* JE_IDENT_TT  */
    JE_AND = 264,                  /* JE_AND  */
    JE_ASR = 265,                  /* JE_ASR  */
    JE_DEC = 266,                  /* JE_DEC  */
    JE_EQ = 267,                   /* JE_EQ  */
    JE_GE = 268,                   /* JE_GE  */
    JE_INC = 269,                  /* JE_INC  */
    JE_LE = 270,                   /* JE_LE  */
    JE_NE = 271,                   /* JE_NE  */
    JE_OR = 272,                   /* JE_OR  */
    JE_POW = 273,                  /* JE_POW  */
    JE_SHL = 274,                  /* JE_SHL  */
    JE_SHR = 275,                  /* JE_SHR  */
    JE_ANDEQ = 276,                /* JE_ANDEQ  */
    JE_ASREQ = 277,                /* JE_ASREQ  */
    JE_DECEQ = 278,                /* JE_DECEQ  */
    JE_DIVEQ = 279,                /* JE_DIVEQ  */
    JE_INCEQ = 280,                /* JE_INCEQ  */
    JE_MODEQ = 281,                /* JE_MODEQ  */
    JE_OREQ = 282,                 /* JE_OREQ  */
    JE_POWEQ = 283,                /* JE_POWEQ  */
    JE_SHLEQ = 284,                /* JE_SHLEQ  */
    JE_SHREQ = 285,                /* JE_SHREQ  */
    JE_TIMEQ = 286,                /* JE_TIMEQ  */
    JE_XOREQ = 287,                /* JE_XOREQ  */
    JE_UNIP = 288,                 /* JE_UNIP  */
    JE_SUFP = 289                  /* JE_SUFP  */
  };
  typedef enum JE_yytokentype JE_yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined JE_YYSTYPE && ! defined JE_YYSTYPE_IS_DECLARED
union JE_YYSTYPE
{
#line 107 "je_parse.y"

    JE_AST     ast;
    JE_ASTNODE astnode;
    JE_STR     ident;
    JE_TOKEN   token;
    JE_VALUE   value;

#line 134 "je_parse.tab.h"

};
typedef union JE_YYSTYPE JE_YYSTYPE;
# define JE_YYSTYPE_IS_TRIVIAL 1
# define JE_YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined JE_YYLTYPE && ! defined JE_YYLTYPE_IS_DECLARED
typedef struct JE_YYLTYPE JE_YYLTYPE;
struct JE_YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define JE_YYLTYPE_IS_DECLARED 1
# define JE_YYLTYPE_IS_TRIVIAL 1
#endif


extern JE_YYSTYPE JE_yylval;
extern JE_YYLTYPE JE_yylloc;

int JE_yyparse (void);


#endif /* !YY_JE_YY_JE_PARSE_TAB_H_INCLUDED  */
