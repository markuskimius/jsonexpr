/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         JE_YYSTYPE
#define YYLTYPE         JE_YYLTYPE
/* Substitute the variable and function names.  */
#define yyparse         JE_yyparse
#define yylex           JE_yylex
#define yyerror         JE_yyerror
#define yydebug         JE_yydebug
#define yynerrs         JE_yynerrs
#define yylval          JE_yylval
#define yychar          JE_yychar
#define yylloc          JE_yylloc

/* First part of user prologue.  */
#line 1 "je_parse.y"

#include <stdint.h>
#include "je_ast.h"
#include "je_error.h"
#include "je_node.h"
#include "je_parse.h"
#include "je_token.h"

extern int yylex();

void yyerror(const char* s) {
    JE_SyntaxError(JE_token, JE_token+1, "%s", s);
    JE_ast.isok = 0;
}

#line 97 "je_parse.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "je_parse.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_JE_F64_TT = 3,                  /* JE_F64_TT  */
  YYSYMBOL_JE_I64_TT = 4,                  /* JE_I64_TT  */
  YYSYMBOL_JE_NUL_TT = 5,                  /* JE_NUL_TT  */
  YYSYMBOL_JE_STR_TT = 6,                  /* JE_STR_TT  */
  YYSYMBOL_JE_TRU_TT = 7,                  /* JE_TRU_TT  */
  YYSYMBOL_JE_IDENT_TT = 8,                /* JE_IDENT_TT  */
  YYSYMBOL_9_ = 9,                         /* '!'  */
  YYSYMBOL_10_ = 10,                       /* '&'  */
  YYSYMBOL_11_ = 11,                       /* '('  */
  YYSYMBOL_12_ = 12,                       /* '*'  */
  YYSYMBOL_13_ = 13,                       /* '+'  */
  YYSYMBOL_14_ = 14,                       /* ','  */
  YYSYMBOL_15_ = 15,                       /* '-'  */
  YYSYMBOL_16_ = 16,                       /* '.'  */
  YYSYMBOL_17_ = 17,                       /* '/'  */
  YYSYMBOL_18_ = 18,                       /* ':'  */
  YYSYMBOL_19_ = 19,                       /* ';'  */
  YYSYMBOL_20_ = 20,                       /* '<'  */
  YYSYMBOL_21_ = 21,                       /* '='  */
  YYSYMBOL_22_ = 22,                       /* '>'  */
  YYSYMBOL_23_ = 23,                       /* '['  */
  YYSYMBOL_24_ = 24,                       /* '^'  */
  YYSYMBOL_25_ = 25,                       /* '{'  */
  YYSYMBOL_26_ = 26,                       /* '|'  */
  YYSYMBOL_27_ = 27,                       /* '~'  */
  YYSYMBOL_JE_AND = 28,                    /* JE_AND  */
  YYSYMBOL_JE_ASR = 29,                    /* JE_ASR  */
  YYSYMBOL_JE_DEC = 30,                    /* JE_DEC  */
  YYSYMBOL_JE_EQ = 31,                     /* JE_EQ  */
  YYSYMBOL_JE_GE = 32,                     /* JE_GE  */
  YYSYMBOL_JE_INC = 33,                    /* JE_INC  */
  YYSYMBOL_JE_LE = 34,                     /* JE_LE  */
  YYSYMBOL_JE_NE = 35,                     /* JE_NE  */
  YYSYMBOL_JE_OR = 36,                     /* JE_OR  */
  YYSYMBOL_JE_POW = 37,                    /* JE_POW  */
  YYSYMBOL_JE_SHL = 38,                    /* JE_SHL  */
  YYSYMBOL_JE_SHR = 39,                    /* JE_SHR  */
  YYSYMBOL_JE_ANDEQ = 40,                  /* JE_ANDEQ  */
  YYSYMBOL_JE_ASREQ = 41,                  /* JE_ASREQ  */
  YYSYMBOL_JE_DECEQ = 42,                  /* JE_DECEQ  */
  YYSYMBOL_JE_DIVEQ = 43,                  /* JE_DIVEQ  */
  YYSYMBOL_JE_INCEQ = 44,                  /* JE_INCEQ  */
  YYSYMBOL_JE_MODEQ = 45,                  /* JE_MODEQ  */
  YYSYMBOL_JE_OREQ = 46,                   /* JE_OREQ  */
  YYSYMBOL_JE_POWEQ = 47,                  /* JE_POWEQ  */
  YYSYMBOL_JE_SHLEQ = 48,                  /* JE_SHLEQ  */
  YYSYMBOL_JE_SHREQ = 49,                  /* JE_SHREQ  */
  YYSYMBOL_JE_TIMEQ = 50,                  /* JE_TIMEQ  */
  YYSYMBOL_JE_XOREQ = 51,                  /* JE_XOREQ  */
  YYSYMBOL_52_ = 52,                       /* '?'  */
  YYSYMBOL_53_ = 53,                       /* '%'  */
  YYSYMBOL_JE_UNIP = 54,                   /* JE_UNIP  */
  YYSYMBOL_JE_SUFP = 55,                   /* JE_SUFP  */
  YYSYMBOL_56_ = 56,                       /* ')'  */
  YYSYMBOL_57_ = 57,                       /* ']'  */
  YYSYMBOL_58_ = 58,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 59,                  /* $accept  */
  YYSYMBOL_start = 60,                     /* start  */
  YYSYMBOL_expr = 61,                      /* expr  */
  YYSYMBOL_list = 62,                      /* list  */
  YYSYMBOL_pair = 63,                      /* pair  */
  YYSYMBOL_pairlist = 64                   /* pairlist  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined JE_YYLTYPE_IS_TRIVIAL && JE_YYLTYPE_IS_TRIVIAL \
             && defined JE_YYSTYPE_IS_TRIVIAL && JE_YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  34
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   910

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  59
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  6
/* YYNRULES -- Number of rules.  */
#define YYNRULES  70
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  131

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     9,     2,     2,     2,    53,    10,     2,
      11,    56,    12,    13,    14,    15,    16,    17,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,    19,
      20,    21,    22,    52,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    23,     2,    57,    24,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,    26,    58,    27,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    54,    55
};

#if JE_YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   123,   123,   124,   127,   128,   129,   130,   131,   132,
     133,   134,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   190,   193,   194,   197,   200,
     201
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if JE_YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "JE_F64_TT",
  "JE_I64_TT", "JE_NUL_TT", "JE_STR_TT", "JE_TRU_TT", "JE_IDENT_TT", "'!'",
  "'&'", "'('", "'*'", "'+'", "','", "'-'", "'.'", "'/'", "':'", "';'",
  "'<'", "'='", "'>'", "'['", "'^'", "'{'", "'|'", "'~'", "JE_AND",
  "JE_ASR", "JE_DEC", "JE_EQ", "JE_GE", "JE_INC", "JE_LE", "JE_NE",
  "JE_OR", "JE_POW", "JE_SHL", "JE_SHR", "JE_ANDEQ", "JE_ASREQ",
  "JE_DECEQ", "JE_DIVEQ", "JE_INCEQ", "JE_MODEQ", "JE_OREQ", "JE_POWEQ",
  "JE_SHLEQ", "JE_SHREQ", "JE_TIMEQ", "JE_XOREQ", "'?'", "'%'", "JE_UNIP",
  "JE_SUFP", "')'", "']'", "'}'", "$accept", "start", "expr", "list",
  "pair", "pairlist", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-76)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-33)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     864,   -18,   -76,   -76,   -76,   -76,   -76,   -76,   877,   877,
     877,   877,   244,   185,   877,   877,   877,     2,   470,   877,
     211,   379,   211,   211,   -76,   470,    18,   -76,    71,   -76,
      -7,   211,   211,   211,   -76,   877,   302,   877,   877,   877,
      -5,   877,   127,   877,   877,   877,   877,   877,   877,   877,
     877,   -76,   877,   877,   -76,   877,   877,   877,   877,   877,
     877,   877,   877,   877,   877,   877,   877,   877,   877,   877,
     877,   877,   877,   877,   877,   514,   -76,   257,   -76,   877,
     198,   -76,   717,   -76,    -2,   203,   813,   813,   -76,   203,
     514,   775,   514,   775,   331,   688,   656,   624,   804,   746,
     775,   775,   746,   592,   203,   804,   804,   514,   514,   514,
     514,   514,   514,   514,   514,   514,   514,   514,   514,   426,
     203,   -76,   470,   470,   -76,   -76,   877,   -76,   -76,   877,
     558
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,    15,    16,    17,    18,    19,    20,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     3,     0,
       5,     0,     6,     7,     9,    66,     0,    12,     0,    69,
       0,     8,    21,    22,     1,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,     0,     0,    46,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    65,     4,     0,    10,     0,
       0,    13,    24,    25,     0,    27,    28,    29,    30,    31,
      33,    34,    35,    36,     0,    39,    40,    41,    42,    44,
      45,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,     0,
      23,    11,    67,    68,    14,    70,     0,    26,    38,     0,
      37
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -76,     0,   -32,   -75,   -76
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    17,    25,    26,    29,    30
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      18,    19,    34,    88,    84,   125,     0,    80,    20,    21,
      22,    23,   126,    28,    31,    32,    33,     0,     0,    75,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    77,     0,     0,    82,     0,    85,    86,    87,
       0,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    81,    99,   100,   127,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,    78,     0,   122,     0,   123,
      28,    35,    36,    37,    38,     0,    39,    40,    41,    79,
      42,    43,    44,    45,    46,    47,     0,    48,     0,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,     0,   122,   -32,     1,   130,
       2,     3,     4,     5,     6,     7,     8,   -32,     9,   -32,
      10,   -32,    11,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
      12,   -32,    13,   -32,    14,   -32,   -32,    15,   -32,   -32,
      16,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,   -32,
     -32,     0,     0,   -32,   -32,   -32,     1,     0,     2,     3,
       4,     5,     6,     7,     8,     0,     9,     0,    10,     1,
      11,     2,     3,     4,     5,     6,     7,     8,    12,     9,
      13,    10,    14,    11,    36,    15,     0,     0,    16,    40,
       0,    12,    36,    13,     0,    14,    46,    40,    15,     0,
       0,    16,     0,    51,    46,     0,    54,     0,     0,     0,
      58,    51,     0,    27,    54,     1,     0,     2,     3,     4,
       5,     6,     7,     8,     0,     9,   124,    10,     1,    11,
       2,     3,     4,     5,     6,     7,     8,    12,     9,    13,
      10,    14,    11,     0,    15,     0,     0,    16,     0,     0,
      12,     0,    13,     0,    14,     0,     0,    15,     0,     0,
      16,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    24,     0,     1,     0,     2,     3,     4,     5,     6,
       7,     8,     0,     9,   121,    10,     0,    11,     0,     0,
       0,     0,     0,     0,     0,    12,     0,    13,     0,    14,
       0,     0,    15,     0,     0,    16,     0,     0,     0,     0,
       0,    35,    36,    37,    38,     0,    39,    40,    41,     0,
      42,    43,    44,    45,    46,    47,     0,    48,    83,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,     0,     0,     0,   128,    35,
      36,    37,    38,     0,    39,    40,    41,     0,    42,    43,
      44,    45,    46,    47,     0,    48,     0,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,     0,     0,    76,    35,    36,    37,    38,
       0,    39,    40,    41,   129,    42,    43,    44,    45,    46,
      47,     0,    48,     0,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      35,    36,    37,    38,     0,    39,    40,    41,     0,    42,
      43,    44,    45,    46,    47,     0,    48,     0,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    43,    44,    45,    46,    47,     0,
      48,     0,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    43,     0,
      45,    46,    47,     0,    48,     0,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    60,     0,     0,
       0,     0,    35,    36,    37,    38,     0,    39,    40,    41,
      73,    74,    43,     0,    45,    46,    47,     0,    48,     0,
      49,    50,    51,    52,    53,    54,    55,    56,     0,    58,
      59,    60,     0,     0,    35,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    43,    74,    45,    46,    47,     0,
      48,     0,     0,    50,    51,    52,    53,    54,    55,    56,
       0,    58,    59,    60,     0,     0,    35,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    43,    74,    45,    46,
      47,     0,     0,     0,     0,    50,    51,    52,    53,    54,
      55,    56,     0,    58,    59,    60,     0,     0,    35,    36,
      37,    38,     0,    39,    40,    41,     0,     0,    43,    74,
      45,    46,     0,     0,     0,     0,     0,    50,    51,    52,
      53,    54,    55,    56,     0,    58,    59,    60,    36,    37,
      38,     0,    39,    40,    41,     0,     0,    43,     0,    45,
      46,    74,     0,     0,     0,     0,    50,    51,    52,    53,
      54,    55,    56,     0,    58,    59,    60,    36,    37,    38,
       0,    39,    40,    41,     0,     0,    43,     0,    45,    46,
      74,     0,     0,     0,     0,    50,    51,     0,    53,    54,
      55,     0,     0,    58,    59,    60,    36,    37,    38,     0,
      39,    40,    41,     0,     0,     0,     0,     0,    46,    74,
       0,     0,     0,     0,    50,    51,     0,     0,    54,     0,
       0,     0,    58,    59,    60,    36,    37,    38,     0,    39,
      40,    41,     0,     0,    36,    37,     0,    46,    74,    40,
      41,     0,     0,     0,    51,     0,    46,    54,     0,     0,
       0,    58,     0,    51,     0,     0,    54,     0,     0,     0,
      58,     0,     0,     0,     0,     0,     0,    74,     0,     0,
       0,     0,     0,     0,    -2,     1,    74,     2,     3,     4,
       5,     6,     7,     8,     0,     9,     0,    10,     1,    11,
       2,     3,     4,     5,     6,     7,     8,    12,     9,    13,
      10,    14,    11,     0,    15,     0,     0,    16,     0,     0,
      12,     0,    13,     0,    14,     0,     0,    15,     0,     0,
      16
};

static const yytype_int16 yycheck[] =
{
       0,    19,     0,     8,    36,    80,    -1,    14,     8,     9,
      10,    11,    14,    13,    14,    15,    16,    -1,    -1,    19,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    -1,    -1,    35,    -1,    37,    38,    39,
      -1,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    58,    52,    53,    56,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    57,    -1,    77,    -1,    79,
      80,    10,    11,    12,    13,    -1,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    -1,    26,    -1,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,   126,     0,     1,   129,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    56,    57,    58,     1,    -1,     3,     4,
       5,     6,     7,     8,     9,    -1,    11,    -1,    13,     1,
      15,     3,     4,     5,     6,     7,     8,     9,    23,    11,
      25,    13,    27,    15,    11,    30,    -1,    -1,    33,    16,
      -1,    23,    11,    25,    -1,    27,    23,    16,    30,    -1,
      -1,    33,    -1,    30,    23,    -1,    33,    -1,    -1,    -1,
      37,    30,    -1,    58,    33,     1,    -1,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    58,    13,     1,    15,
       3,     4,     5,     6,     7,     8,     9,    23,    11,    25,
      13,    27,    15,    -1,    30,    -1,    -1,    33,    -1,    -1,
      23,    -1,    25,    -1,    27,    -1,    -1,    30,    -1,    -1,
      33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    57,    -1,     1,    -1,     3,     4,     5,     6,     7,
       8,     9,    -1,    11,    57,    13,    -1,    15,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    23,    -1,    25,    -1,    27,
      -1,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,    -1,
      -1,    10,    11,    12,    13,    -1,    15,    16,    17,    -1,
      19,    20,    21,    22,    23,    24,    -1,    26,    56,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    -1,    -1,    -1,    57,    10,
      11,    12,    13,    -1,    15,    16,    17,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    -1,    -1,    56,    10,    11,    12,    13,
      -1,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    -1,    26,    -1,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      10,    11,    12,    13,    -1,    15,    16,    17,    -1,    19,
      20,    21,    22,    23,    24,    -1,    26,    -1,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    10,    11,    12,    13,    -1,    15,
      16,    17,    -1,    -1,    20,    21,    22,    23,    24,    -1,
      26,    -1,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    10,    11,
      12,    13,    -1,    15,    16,    17,    -1,    -1,    20,    -1,
      22,    23,    24,    -1,    26,    -1,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    -1,    -1,
      -1,    -1,    10,    11,    12,    13,    -1,    15,    16,    17,
      52,    53,    20,    -1,    22,    23,    24,    -1,    26,    -1,
      28,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      38,    39,    -1,    -1,    10,    11,    12,    13,    -1,    15,
      16,    17,    -1,    -1,    20,    53,    22,    23,    24,    -1,
      26,    -1,    -1,    29,    30,    31,    32,    33,    34,    35,
      -1,    37,    38,    39,    -1,    -1,    10,    11,    12,    13,
      -1,    15,    16,    17,    -1,    -1,    20,    53,    22,    23,
      24,    -1,    -1,    -1,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    38,    39,    -1,    -1,    10,    11,
      12,    13,    -1,    15,    16,    17,    -1,    -1,    20,    53,
      22,    23,    -1,    -1,    -1,    -1,    -1,    29,    30,    31,
      32,    33,    34,    35,    -1,    37,    38,    39,    11,    12,
      13,    -1,    15,    16,    17,    -1,    -1,    20,    -1,    22,
      23,    53,    -1,    -1,    -1,    -1,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    38,    39,    11,    12,    13,
      -1,    15,    16,    17,    -1,    -1,    20,    -1,    22,    23,
      53,    -1,    -1,    -1,    -1,    29,    30,    -1,    32,    33,
      34,    -1,    -1,    37,    38,    39,    11,    12,    13,    -1,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    23,    53,
      -1,    -1,    -1,    -1,    29,    30,    -1,    -1,    33,    -1,
      -1,    -1,    37,    38,    39,    11,    12,    13,    -1,    15,
      16,    17,    -1,    -1,    11,    12,    -1,    23,    53,    16,
      17,    -1,    -1,    -1,    30,    -1,    23,    33,    -1,    -1,
      -1,    37,    -1,    30,    -1,    -1,    33,    -1,    -1,    -1,
      37,    -1,    -1,    -1,    -1,    -1,    -1,    53,    -1,    -1,
      -1,    -1,    -1,    -1,     0,     1,    53,     3,     4,     5,
       6,     7,     8,     9,    -1,    11,    -1,    13,     1,    15,
       3,     4,     5,     6,     7,     8,     9,    23,    11,    25,
      13,    27,    15,    -1,    30,    -1,    -1,    33,    -1,    -1,
      23,    -1,    25,    -1,    27,    -1,    -1,    30,    -1,    -1,
      33
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     8,     9,    11,
      13,    15,    23,    25,    27,    30,    33,    60,    61,    19,
      61,    61,    61,    61,    57,    61,    62,    58,    61,    63,
      64,    61,    61,    61,     0,    10,    11,    12,    13,    15,
      16,    17,    19,    20,    21,    22,    23,    24,    26,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    61,    56,    14,    57,    18,
      14,    58,    61,    56,    62,    61,    61,    61,     8,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    57,    61,    61,    58,    63,    14,    56,    57,    18,
      61
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    59,    60,    60,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    62,    62,    63,    64,
      64
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     1,     3,     2,     2,     2,     2,     2,
       3,     4,     2,     3,     4,     1,     1,     1,     1,     1,
       1,     2,     2,     3,     3,     3,     4,     3,     3,     3,
       3,     3,     2,     3,     3,     3,     3,     5,     4,     3,
       3,     3,     3,     2,     3,     3,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     3,     3,     1,
       3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = JE_YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == JE_YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use JE_YYerror or JE_YYUNDEF. */
#define YYERRCODE JE_YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if JE_YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined JE_YYLTYPE_IS_TRIVIAL && JE_YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !JE_YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !JE_YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined JE_YYLTYPE_IS_TRIVIAL && JE_YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = JE_YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == JE_YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= JE_YYEOF)
    {
      yychar = JE_YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == JE_YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = JE_YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = JE_YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* start: %empty  */
#line 123 "je_parse.y"
                                        { /* no op */           (yyval.ast) = JE_ast; }
#line 1503 "je_parse.tab.c"
    break;

  case 3: /* start: expr  */
#line 124 "je_parse.y"
                                        { JE_ast.astnode = (yyvsp[0].astnode);  (yyval.ast) = JE_ast; }
#line 1509 "je_parse.tab.c"
    break;

  case 4: /* expr: '(' expr ')'  */
#line 127 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeParen((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1515 "je_parse.tab.c"
    break;

  case 5: /* expr: '!' expr  */
#line 128 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeLogicalNot((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1521 "je_parse.tab.c"
    break;

  case 6: /* expr: '+' expr  */
#line 129 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePos((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1527 "je_parse.tab.c"
    break;

  case 7: /* expr: '-' expr  */
#line 130 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeNeg((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1533 "je_parse.tab.c"
    break;

  case 8: /* expr: '~' expr  */
#line 131 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeBitwiseInv((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1539 "je_parse.tab.c"
    break;

  case 9: /* expr: '[' ']'  */
#line 132 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeMtArray(&JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1545 "je_parse.tab.c"
    break;

  case 10: /* expr: '[' list ']'  */
#line 133 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeArray((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1551 "je_parse.tab.c"
    break;

  case 11: /* expr: '[' list ',' ']'  */
#line 134 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeArray2((yyvsp[-2].astnode), &JE_ast.info, (yylsp[-3]), (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1557 "je_parse.tab.c"
    break;

  case 12: /* expr: '{' '}'  */
#line 135 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeMtObject(&JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1563 "je_parse.tab.c"
    break;

  case 13: /* expr: '{' pairlist '}'  */
#line 136 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeObject((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1569 "je_parse.tab.c"
    break;

  case 14: /* expr: '{' pairlist ',' '}'  */
#line 137 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeObject2((yyvsp[-2].astnode), &JE_ast.info, (yylsp[-3]), (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1575 "je_parse.tab.c"
    break;

  case 15: /* expr: JE_F64_TT  */
#line 138 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeVal((yyvsp[0].value), &JE_ast.info, (yylsp[0])); }
#line 1581 "je_parse.tab.c"
    break;

  case 16: /* expr: JE_I64_TT  */
#line 139 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeVal((yyvsp[0].value), &JE_ast.info, (yylsp[0])); }
#line 1587 "je_parse.tab.c"
    break;

  case 17: /* expr: JE_NUL_TT  */
#line 140 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeVal((yyvsp[0].value), &JE_ast.info, (yylsp[0])); }
#line 1593 "je_parse.tab.c"
    break;

  case 18: /* expr: JE_STR_TT  */
#line 141 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeVal((yyvsp[0].value), &JE_ast.info, (yylsp[0])); }
#line 1599 "je_parse.tab.c"
    break;

  case 19: /* expr: JE_TRU_TT  */
#line 142 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeVal((yyvsp[0].value), &JE_ast.info, (yylsp[0])); }
#line 1605 "je_parse.tab.c"
    break;

  case 20: /* expr: JE_IDENT_TT  */
#line 143 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeIdent((yyvsp[0].ident), &JE_ast.info, (yylsp[0])); }
#line 1611 "je_parse.tab.c"
    break;

  case 21: /* expr: JE_DEC expr  */
#line 144 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePreDec((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1617 "je_parse.tab.c"
    break;

  case 22: /* expr: JE_INC expr  */
#line 145 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePreInc((yyvsp[0].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1623 "je_parse.tab.c"
    break;

  case 23: /* expr: expr '%' expr  */
#line 146 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeMod((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1629 "je_parse.tab.c"
    break;

  case 24: /* expr: expr '&' expr  */
#line 147 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeBitwiseAnd((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1635 "je_parse.tab.c"
    break;

  case 25: /* expr: expr '(' ')'  */
#line 148 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeCall((yyvsp[-2].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1641 "je_parse.tab.c"
    break;

  case 26: /* expr: expr '(' list ')'  */
#line 149 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeCall2((yyvsp[-3].astnode), (yyvsp[-1].astnode), &JE_ast.info, (yylsp[-3]), (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1647 "je_parse.tab.c"
    break;

  case 27: /* expr: expr '*' expr  */
#line 150 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeTimes((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1653 "je_parse.tab.c"
    break;

  case 28: /* expr: expr '+' expr  */
#line 151 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePlus((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1659 "je_parse.tab.c"
    break;

  case 29: /* expr: expr '-' expr  */
#line 152 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeMinus((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1665 "je_parse.tab.c"
    break;

  case 30: /* expr: expr '.' JE_IDENT_TT  */
#line 153 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeMember((yyvsp[-2].astnode), (yyvsp[0].ident), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1671 "je_parse.tab.c"
    break;

  case 31: /* expr: expr '/' expr  */
#line 154 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeDivby((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1677 "je_parse.tab.c"
    break;

  case 32: /* expr: expr ';'  */
#line 155 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeStmt((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1683 "je_parse.tab.c"
    break;

  case 33: /* expr: expr ';' expr  */
#line 156 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeStmt2((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1689 "je_parse.tab.c"
    break;

  case 34: /* expr: expr '<' expr  */
#line 157 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeLess((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1695 "je_parse.tab.c"
    break;

  case 35: /* expr: expr '=' expr  */
#line 158 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssn((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1701 "je_parse.tab.c"
    break;

  case 36: /* expr: expr '>' expr  */
#line 159 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeGreater((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1707 "je_parse.tab.c"
    break;

  case 37: /* expr: expr '?' expr ':' expr  */
#line 160 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeTernary((yyvsp[-4].astnode), (yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-4]), (yylsp[-3]), (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1713 "je_parse.tab.c"
    break;

  case 38: /* expr: expr '[' expr ']'  */
#line 161 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeIndex((yyvsp[-3].astnode), (yyvsp[-1].astnode), &JE_ast.info, (yylsp[-3]), (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1719 "je_parse.tab.c"
    break;

  case 39: /* expr: expr '^' expr  */
#line 162 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeBitwiseXor((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1725 "je_parse.tab.c"
    break;

  case 40: /* expr: expr '|' expr  */
#line 163 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeBitwiseOr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1731 "je_parse.tab.c"
    break;

  case 41: /* expr: expr JE_AND expr  */
#line 164 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeLogicalAnd((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1737 "je_parse.tab.c"
    break;

  case 42: /* expr: expr JE_ASR expr  */
#line 165 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAsr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1743 "je_parse.tab.c"
    break;

  case 43: /* expr: expr JE_DEC  */
#line 166 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePostDec((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1749 "je_parse.tab.c"
    break;

  case 44: /* expr: expr JE_EQ expr  */
#line 167 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeEqual((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1755 "je_parse.tab.c"
    break;

  case 45: /* expr: expr JE_GE expr  */
#line 168 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeGreaterEq((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1761 "je_parse.tab.c"
    break;

  case 46: /* expr: expr JE_INC  */
#line 169 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePostInc((yyvsp[-1].astnode), &JE_ast.info, (yylsp[-1]), (yylsp[0])); }
#line 1767 "je_parse.tab.c"
    break;

  case 47: /* expr: expr JE_LE expr  */
#line 170 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeLessEq((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1773 "je_parse.tab.c"
    break;

  case 48: /* expr: expr JE_NE expr  */
#line 171 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeNotEqual((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1779 "je_parse.tab.c"
    break;

  case 49: /* expr: expr JE_OR expr  */
#line 172 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeLogicalOr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1785 "je_parse.tab.c"
    break;

  case 50: /* expr: expr JE_POW expr  */
#line 173 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePow((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1791 "je_parse.tab.c"
    break;

  case 51: /* expr: expr JE_SHL expr  */
#line 174 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeShl((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1797 "je_parse.tab.c"
    break;

  case 52: /* expr: expr JE_SHR expr  */
#line 175 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeShr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1803 "je_parse.tab.c"
    break;

  case 53: /* expr: expr JE_ANDEQ expr  */
#line 177 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnBitAnd((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1809 "je_parse.tab.c"
    break;

  case 54: /* expr: expr JE_ASREQ expr  */
#line 178 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnAsr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1815 "je_parse.tab.c"
    break;

  case 55: /* expr: expr JE_DECEQ expr  */
#line 179 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnMinus((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1821 "je_parse.tab.c"
    break;

  case 56: /* expr: expr JE_DIVEQ expr  */
#line 180 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnDivby((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1827 "je_parse.tab.c"
    break;

  case 57: /* expr: expr JE_INCEQ expr  */
#line 181 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnPlus((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1833 "je_parse.tab.c"
    break;

  case 58: /* expr: expr JE_MODEQ expr  */
#line 182 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnMod((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1839 "je_parse.tab.c"
    break;

  case 59: /* expr: expr JE_OREQ expr  */
#line 183 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnBitOr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1845 "je_parse.tab.c"
    break;

  case 60: /* expr: expr JE_POWEQ expr  */
#line 184 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnPow((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1851 "je_parse.tab.c"
    break;

  case 61: /* expr: expr JE_SHLEQ expr  */
#line 185 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnShl((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1857 "je_parse.tab.c"
    break;

  case 62: /* expr: expr JE_SHREQ expr  */
#line 186 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnShr((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1863 "je_parse.tab.c"
    break;

  case 63: /* expr: expr JE_TIMEQ expr  */
#line 187 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnTimes((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1869 "je_parse.tab.c"
    break;

  case 64: /* expr: expr JE_XOREQ expr  */
#line 188 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeAssnBitXor((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1875 "je_parse.tab.c"
    break;

  case 65: /* expr: error ';' expr  */
#line 190 "je_parse.y"
                                        { (yyval.astnode) = (yyvsp[0].astnode); }
#line 1881 "je_parse.tab.c"
    break;

  case 66: /* list: expr  */
#line 193 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeList((yyvsp[0].astnode), &JE_ast.info, (yylsp[0]));  }
#line 1887 "je_parse.tab.c"
    break;

  case 67: /* list: list ',' expr  */
#line 194 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodeList2((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1893 "je_parse.tab.c"
    break;

  case 68: /* pair: expr ':' expr  */
#line 197 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePair((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1899 "je_parse.tab.c"
    break;

  case 69: /* pairlist: pair  */
#line 200 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePairlist((yyvsp[0].astnode), &JE_ast.info, (yylsp[0])); }
#line 1905 "je_parse.tab.c"
    break;

  case 70: /* pairlist: pairlist ',' pair  */
#line 201 "je_parse.y"
                                        { (yyval.astnode) = JE_AstNodePairlist2((yyvsp[-2].astnode), (yyvsp[0].astnode), &JE_ast.info, (yylsp[-2]), (yylsp[-1]), (yylsp[0])); }
#line 1911 "je_parse.tab.c"
    break;


#line 1915 "je_parse.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == JE_YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= JE_YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == JE_YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = JE_YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != JE_YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 204 "je_parse.y"

