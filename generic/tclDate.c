/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 1

/* Substitute the variable and function names.  */
#define yyparse         TclDateparse
#define yylex           TclDatelex
#define yyerror         TclDateerror
#define yylval          TclDatelval
#define yychar          TclDatechar
#define yydebug         TclDatedebug
#define yynerrs         TclDatenerrs
#define yylloc          TclDatelloc

/* Copy the first part of user declarations.  */


/*
 * tclDate.c --
 *
 *	This file is generated from a yacc grammar defined in the file
 *	tclGetDate.y. It should not be edited directly.
 *
 * Copyright (c) 1992-1995 Karl Lehenbauer and Mark Diekhans.
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */
#include "tclInt.h"

/*
 * Bison generates several labels that happen to be unused. MS Visual C++
 * doesn't like that, and complains. Tell it to shut up.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4102 )
#endif /* _MSC_VER */

/*
 * yyparse will accept a 'struct DateInfo' as its parameter; that's where the
 * parsed fields will be returned.
 */

#include "tclDate.h"

#define YYMALLOC	ckalloc
#define YYFREE(x)	(ckfree((void*) (x)))

#define EPOCH		1970
#define START_OF_TIME	1902
#define END_OF_TIME	2037

/*
 * The offset of tm_year of struct tm returned by localtime, gmtime, etc.
 * Posix requires 1900.
 */

#define TM_YEAR_BASE	1900

#define HOUR(x)		((int) (60 * x))
#define SECSPERDAY	(24L * 60L * 60L)
#define IsLeapYear(x)	((x % 4 == 0) && (x % 100 != 0 || x % 400 == 0))

/*
 * An entry in the lexical lookup table.
 */

typedef struct _TABLE {
    const char *name;
    int type;
    time_t value;
} TABLE;

/*
 * Daylight-savings mode: on, off, or not yet known.
 */

typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;




/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     tAGO = 258,
     tDAY = 259,
     tDAYZONE = 260,
     tID = 261,
     tMERIDIAN = 262,
     tMONTH = 263,
     tMONTH_UNIT = 264,
     tSTARDATE = 265,
     tSEC_UNIT = 266,
     tSNUMBER = 267,
     tUNUMBER = 268,
     tZONE = 269,
     tEPOCH = 270,
     tDST = 271,
     tISOBASE = 272,
     tDAY_UNIT = 273,
     tNEXT = 274
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{


    time_t Number;
    enum _MERIDIAN Meridian;



} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */



/*
 * Prototypes of internal functions.
 */

static int		LookupWord(YYSTYPE* yylvalPtr, char *buff);
 static void		TclDateerror(YYLTYPE* location,
				     DateInfo* info, const char *s);
 static int		TclDatelex(YYSTYPE* yylvalPtr, YYLTYPE* location,
				   DateInfo* info);
MODULE_SCOPE int	yyparse(DateInfo*);




#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   80

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  16
/* YYNRULES -- Number of rules.  */
#define YYNRULES  56
/* YYNRULES -- Number of states.  */
#define YYNSTATES  81

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   274

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    25,    21,    23,    24,    22,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    20,     2,
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    28,    33,    40,    43,    45,    47,
      50,    54,    56,    59,    62,    66,    69,    73,    79,    81,
      87,    93,    96,   101,   104,   106,   110,   113,   117,   121,
     129,   132,   137,   140,   142,   146,   149,   152,   156,   158,
     160,   162,   164,   166,   168,   170,   171
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      27,     0,    -1,    -1,    27,    28,    -1,    29,    -1,    30,
      -1,    32,    -1,    33,    -1,    31,    -1,    36,    -1,    34,
      -1,    35,    -1,    40,    -1,    13,     7,    -1,    13,    20,
      13,    41,    -1,    13,    20,    13,    20,    13,    41,    -1,
      14,    16,    -1,    14,    -1,     5,    -1,    38,    13,    -1,
      14,    38,    13,    -1,     4,    -1,     4,    21,    -1,    13,
       4,    -1,    38,    13,     4,    -1,    19,     4,    -1,    13,
      22,    13,    -1,    13,    22,    13,    22,    13,    -1,    17,
      -1,    13,    23,     8,    23,    13,    -1,    13,    23,    13,
      23,    13,    -1,     8,    13,    -1,     8,    13,    21,    13,
      -1,    13,     8,    -1,    15,    -1,    13,     8,    13,    -1,
      19,     8,    -1,    19,    13,     8,    -1,    17,    14,    17,
      -1,    17,    14,    13,    20,    13,    20,    13,    -1,    17,
      17,    -1,    10,    13,    24,    13,    -1,    37,     3,    -1,
      37,    -1,    38,    13,    39,    -1,    13,    39,    -1,    19,
      39,    -1,    19,    13,    39,    -1,    39,    -1,    23,    -1,
      25,    -1,    11,    -1,    18,    -1,     9,    -1,    13,    -1,
      -1,     7,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   152,   152,   153,   156,   159,   162,   165,   168,   171,
     174,   178,   183,   186,   192,   198,   206,   210,   214,   218,
     222,   228,   232,   236,   240,   244,   250,   254,   259,   264,
     269,   274,   278,   283,   287,   292,   299,   303,   309,   318,
     327,   337,   351,   356,   359,   362,   365,   368,   371,   376,
     379,   384,   388,   392,   398,   416,   419
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tAGO", "tDAY", "tDAYZONE", "tID",
  "tMERIDIAN", "tMONTH", "tMONTH_UNIT", "tSTARDATE", "tSEC_UNIT",
  "tSNUMBER", "tUNUMBER", "tZONE", "tEPOCH", "tDST", "tISOBASE",
  "tDAY_UNIT", "tNEXT", "':'", "','", "'/'", "'-'", "'.'", "'+'",
  "$accept", "spec", "item", "time", "zone", "day", "date", "ordMonth",
  "iso", "trek", "relspec", "relunits", "sign", "unit", "number",
  "o_merid", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
      58,    44,    47,    45,    46,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    26,    27,    27,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    29,    29,    29,    30,    30,    30,    30,
      30,    31,    31,    31,    31,    31,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    33,    33,    34,    34,
      34,    35,    36,    36,    37,    37,    37,    37,    37,    38,
      38,    39,    39,    39,    40,    41,    41
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     4,     6,     2,     1,     1,     2,
       3,     1,     2,     2,     3,     2,     3,     5,     1,     5,
       5,     2,     4,     2,     1,     3,     2,     3,     3,     7,
       2,     4,     2,     1,     3,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,    21,    18,     0,    53,     0,    51,    54,
      17,    34,    28,    52,     0,    49,    50,     3,     4,     5,
       8,     6,     7,    10,    11,     9,    43,     0,    48,    12,
      22,    31,     0,    23,    13,    33,     0,     0,     0,    45,
      16,     0,     0,    40,    25,    36,     0,    46,    42,    19,
       0,     0,    35,    55,    26,     0,     0,    20,     0,    38,
      37,    47,    24,    44,    32,    41,    56,     0,    14,     0,
       0,     0,     0,    55,    27,    29,    30,     0,    15,     0,
      39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    68
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -18
static const yytype_int8 yypact[] =
{
     -18,     2,   -18,   -17,   -18,    -4,   -18,    11,   -18,    24,
      35,   -18,     9,   -18,    30,   -18,   -18,   -18,   -18,   -18,
     -18,   -18,   -18,   -18,   -18,   -18,    26,    17,   -18,   -18,
     -18,    15,    25,   -18,   -18,    42,    44,    48,    -5,   -18,
     -18,    49,     5,   -18,   -18,   -18,    45,   -18,   -18,    41,
      51,    52,   -18,    -6,    46,    43,    47,   -18,    53,   -18,
     -18,   -18,   -18,   -18,   -18,   -18,   -18,    54,   -18,    56,
      58,    59,    61,    68,   -18,   -18,   -18,    57,   -18,    63,
     -18
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,   -18,
     -18,   -18,    69,    -9,   -18,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      39,    66,     2,    55,    30,    47,     3,     4,    56,    31,
       5,     6,     7,     8,    67,     9,    10,    11,    58,    12,
      13,    14,    59,    42,    32,    15,    43,    16,    33,    48,
      49,    34,    35,     6,    44,     8,    50,    61,    45,     6,
      63,     8,    13,    46,    36,    62,    37,    38,    13,    51,
       6,    40,     8,    60,     6,    52,     8,    53,    15,    13,
      16,    54,    57,    13,    64,    65,    70,    73,    69,    74,
      71,    75,    76,    72,    77,    66,    80,    79,     0,    41,
      78
};

static const yytype_int8 yycheck[] =
{
       9,     7,     0,     8,    21,    14,     4,     5,    13,    13,
       8,     9,    10,    11,    20,    13,    14,    15,    13,    17,
      18,    19,    17,    14,    13,    23,    17,    25,     4,     3,
      13,     7,     8,     9,     4,    11,    21,    46,     8,     9,
      49,    11,    18,    13,    20,     4,    22,    23,    18,    24,
       9,    16,    11,     8,     9,    13,    11,    13,    23,    18,
      25,    13,    13,    18,    13,    13,    23,    13,    22,    13,
      23,    13,    13,    20,    13,     7,    13,    20,    -1,    10,
      73
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    27,     0,     4,     5,     8,     9,    10,    11,    13,
      14,    15,    17,    18,    19,    23,    25,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      21,    13,    13,     4,     7,     8,    20,    22,    23,    39,
      16,    38,    14,    17,     4,     8,    13,    39,     3,    13,
      21,    24,    13,    13,    13,     8,    13,    13,    13,    17,
       8,    39,     4,    39,    13,    13,     7,    20,    41,    22,
      23,    23,    20,    13,    13,    13,    13,    13,    41,    20,
      13
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (&yylloc, info, YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, &yylloc, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval, &yylloc, info)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location, info); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, DateInfo* info)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, info)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    DateInfo* info;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
  YYUSE (info);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, DateInfo* info)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp, info)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
    DateInfo* info;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, info);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, DateInfo* info)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule, info)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
    DateInfo* info;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       , info);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule, info); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
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



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, DateInfo* info)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp, info)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
    DateInfo* info;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (info);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (DateInfo* info);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (DateInfo* info)
#else
int
yyparse (info)
    DateInfo* info;
#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Location data for the lookahead symbol.  */
YYLTYPE yylloc;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.
       `yyls': related to locations.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[2];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;

#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 1;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);

	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
	YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:

    {
	    yyHaveTime++;
	;}
    break;

  case 5:

    {
	    yyHaveZone++;
	;}
    break;

  case 6:

    {
	    yyHaveDate++;
	;}
    break;

  case 7:

    {
	    yyHaveOrdinalMonth++;
	;}
    break;

  case 8:

    {
	    yyHaveDay++;
	;}
    break;

  case 9:

    {
	    yyHaveRel++;
	;}
    break;

  case 10:

    {
	    yyHaveTime++;
	    yyHaveDate++;
	;}
    break;

  case 11:

    {
	    yyHaveTime++;
	    yyHaveDate++;
	    yyHaveRel++;
	;}
    break;

  case 13:

    {
	    yyHour = (yyvsp[(1) - (2)].Number);
	    yyMinutes = 0;
	    yySeconds = 0;
	    yyMeridian = (yyvsp[(2) - (2)].Meridian);
	;}
    break;

  case 14:

    {
	    yyHour = (yyvsp[(1) - (4)].Number);
	    yyMinutes = (yyvsp[(3) - (4)].Number);
	    yySeconds = 0;
	    yyMeridian = (yyvsp[(4) - (4)].Meridian);
	;}
    break;

  case 15:

    {
	    yyHour = (yyvsp[(1) - (6)].Number);
	    yyMinutes = (yyvsp[(3) - (6)].Number);
	    yySeconds = (yyvsp[(5) - (6)].Number);
	    yyMeridian = (yyvsp[(6) - (6)].Meridian);
	;}
    break;

  case 16:

    {
	    yyTimezone = (yyvsp[(1) - (2)].Number);
	    yyDSTmode = DSTon;
	;}
    break;

  case 17:

    {
	    yyTimezone = (yyvsp[(1) - (1)].Number);
	    yyDSTmode = DSToff;
	;}
    break;

  case 18:

    {
	    yyTimezone = (yyvsp[(1) - (1)].Number);
	    yyDSTmode = DSTon;
	;}
    break;

  case 19:

    {
	    yyTimezone = -(yyvsp[(1) - (2)].Number)*((yyvsp[(2) - (2)].Number) % 100 + ((yyvsp[(2) - (2)].Number) / 100) * 60);
	    yyDSTmode = DSToff;
	;}
    break;

  case 20:

    {
	    yyTimezone = (yyvsp[(1) - (3)].Number) - (yyvsp[(2) - (3)].Number)*((yyvsp[(3) - (3)].Number) % 100 + ((yyvsp[(3) - (3)].Number) / 100) * 60);
	    yyDSTmode = DSToff;
	;}
    break;

  case 21:

    {
	    yyDayOrdinal = 1;
	    yyDayNumber = (yyvsp[(1) - (1)].Number);
	;}
    break;

  case 22:

    {
	    yyDayOrdinal = 1;
	    yyDayNumber = (yyvsp[(1) - (2)].Number);
	;}
    break;

  case 23:

    {
	    yyDayOrdinal = (yyvsp[(1) - (2)].Number);
	    yyDayNumber = (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 24:

    {
	    yyDayOrdinal = (yyvsp[(1) - (3)].Number) * (yyvsp[(2) - (3)].Number);
	    yyDayNumber = (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 25:

    {
	    yyDayOrdinal = 2;
	    yyDayNumber = (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 26:

    {
	    yyMonth = (yyvsp[(1) - (3)].Number);
	    yyDay = (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 27:

    {
	    yyMonth = (yyvsp[(1) - (5)].Number);
	    yyDay = (yyvsp[(3) - (5)].Number);
	    yyYear = (yyvsp[(5) - (5)].Number);
	;}
    break;

  case 28:

    {
	    yyYear = (yyvsp[(1) - (1)].Number) / 10000;
	    yyMonth = ((yyvsp[(1) - (1)].Number) % 10000)/100;
	    yyDay = (yyvsp[(1) - (1)].Number) % 100;
	;}
    break;

  case 29:

    {
	    yyDay = (yyvsp[(1) - (5)].Number);
	    yyMonth = (yyvsp[(3) - (5)].Number);
	    yyYear = (yyvsp[(5) - (5)].Number);
	;}
    break;

  case 30:

    {
	    yyMonth = (yyvsp[(3) - (5)].Number);
	    yyDay = (yyvsp[(5) - (5)].Number);
	    yyYear = (yyvsp[(1) - (5)].Number);
	;}
    break;

  case 31:

    {
	    yyMonth = (yyvsp[(1) - (2)].Number);
	    yyDay = (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 32:

    {
	    yyMonth = (yyvsp[(1) - (4)].Number);
	    yyDay = (yyvsp[(2) - (4)].Number);
	    yyYear = (yyvsp[(4) - (4)].Number);
	;}
    break;

  case 33:

    {
	    yyMonth = (yyvsp[(2) - (2)].Number);
	    yyDay = (yyvsp[(1) - (2)].Number);
	;}
    break;

  case 34:

    {
	    yyMonth = 1;
	    yyDay = 1;
	    yyYear = EPOCH;
	;}
    break;

  case 35:

    {
	    yyMonth = (yyvsp[(2) - (3)].Number);
	    yyDay = (yyvsp[(1) - (3)].Number);
	    yyYear = (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 36:

    {
	    yyMonthOrdinalIncr = 1;
	    yyMonthOrdinal = (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 37:

    {
	    yyMonthOrdinalIncr = (yyvsp[(2) - (3)].Number);
	    yyMonthOrdinal = (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 38:

    {
	    if ((yyvsp[(2) - (3)].Number) != HOUR( 7)) YYABORT;
	    yyYear = (yyvsp[(1) - (3)].Number) / 10000;
	    yyMonth = ((yyvsp[(1) - (3)].Number) % 10000)/100;
	    yyDay = (yyvsp[(1) - (3)].Number) % 100;
	    yyHour = (yyvsp[(3) - (3)].Number) / 10000;
	    yyMinutes = ((yyvsp[(3) - (3)].Number) % 10000)/100;
	    yySeconds = (yyvsp[(3) - (3)].Number) % 100;
	;}
    break;

  case 39:

    {
	    if ((yyvsp[(2) - (7)].Number) != HOUR( 7)) YYABORT;
	    yyYear = (yyvsp[(1) - (7)].Number) / 10000;
	    yyMonth = ((yyvsp[(1) - (7)].Number) % 10000)/100;
	    yyDay = (yyvsp[(1) - (7)].Number) % 100;
	    yyHour = (yyvsp[(3) - (7)].Number);
	    yyMinutes = (yyvsp[(5) - (7)].Number);
	    yySeconds = (yyvsp[(7) - (7)].Number);
	;}
    break;

  case 40:

    {
	    yyYear = (yyvsp[(1) - (2)].Number) / 10000;
	    yyMonth = ((yyvsp[(1) - (2)].Number) % 10000)/100;
	    yyDay = (yyvsp[(1) - (2)].Number) % 100;
	    yyHour = (yyvsp[(2) - (2)].Number) / 10000;
	    yyMinutes = ((yyvsp[(2) - (2)].Number) % 10000)/100;
	    yySeconds = (yyvsp[(2) - (2)].Number) % 100;
	;}
    break;

  case 41:

    {
	    /*
	     * Offset computed year by -377 so that the returned years will be
	     * in a range accessible with a 32 bit clock seconds value.
	     */

	    yyYear = (yyvsp[(2) - (4)].Number)/1000 + 2323 - 377;
	    yyDay  = 1;
	    yyMonth = 1;
	    yyRelDay += (((yyvsp[(2) - (4)].Number)%1000)*(365 + IsLeapYear(yyYear)))/1000;
	    yyRelSeconds += (yyvsp[(4) - (4)].Number) * 144 * 60;
	;}
    break;

  case 42:

    {
	    yyRelSeconds *= -1;
	    yyRelMonth *= -1;
	    yyRelDay *= -1;
	;}
    break;

  case 44:

    {
	    *yyRelPointer += (yyvsp[(1) - (3)].Number) * (yyvsp[(2) - (3)].Number) * (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 45:

    {
	    *yyRelPointer += (yyvsp[(1) - (2)].Number) * (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 46:

    {
	    *yyRelPointer += (yyvsp[(2) - (2)].Number);
	;}
    break;

  case 47:

    {
	    *yyRelPointer += (yyvsp[(2) - (3)].Number) * (yyvsp[(3) - (3)].Number);
	;}
    break;

  case 48:

    {
	    *yyRelPointer += (yyvsp[(1) - (1)].Number);
	;}
    break;

  case 49:

    {
	    (yyval.Number) = -1;
	;}
    break;

  case 50:

    {
	    (yyval.Number) =  1;
	;}
    break;

  case 51:

    {
	    (yyval.Number) = (yyvsp[(1) - (1)].Number);
	    yyRelPointer = &yyRelSeconds;
	;}
    break;

  case 52:

    {
	    (yyval.Number) = (yyvsp[(1) - (1)].Number);
	    yyRelPointer = &yyRelDay;
	;}
    break;

  case 53:

    {
	    (yyval.Number) = (yyvsp[(1) - (1)].Number);
	    yyRelPointer = &yyRelMonth;
	;}
    break;

  case 54:

    {
	    if (yyHaveTime && yyHaveDate && !yyHaveRel) {
		yyYear = (yyvsp[(1) - (1)].Number);
	    } else {
		yyHaveTime++;
		if (yyDigitCount <= 2) {
		    yyHour = (yyvsp[(1) - (1)].Number);
		    yyMinutes = 0;
		} else {
		    yyHour = (yyvsp[(1) - (1)].Number) / 100;
		    yyMinutes = (yyvsp[(1) - (1)].Number) % 100;
		}
		yySeconds = 0;
		yyMeridian = MER24;
	    }
	;}
    break;

  case 55:

    {
	    (yyval.Meridian) = MER24;
	;}
    break;

  case 56:

    {
	    (yyval.Meridian) = (yyvsp[(1) - (1)].Meridian);
	;}
    break;



      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, info, YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (&yylloc, info, yymsg);
	  }
	else
	  {
	    yyerror (&yylloc, info, YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc, info);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp, info);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, info, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc, info);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp, info);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}




/*
 * Month and day table.
 */

static const TABLE MonthDayTable[] = {
    { "january",	tMONTH,	 1 },
    { "february",	tMONTH,	 2 },
    { "march",		tMONTH,	 3 },
    { "april",		tMONTH,	 4 },
    { "may",		tMONTH,	 5 },
    { "june",		tMONTH,	 6 },
    { "july",		tMONTH,	 7 },
    { "august",		tMONTH,	 8 },
    { "september",	tMONTH,	 9 },
    { "sept",		tMONTH,	 9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 1 },
    { "tuesday",	tDAY, 2 },
    { "tues",		tDAY, 2 },
    { "wednesday",	tDAY, 3 },
    { "wednes",		tDAY, 3 },
    { "thursday",	tDAY, 4 },
    { "thur",		tDAY, 4 },
    { "thurs",		tDAY, 4 },
    { "friday",		tDAY, 5 },
    { "saturday",	tDAY, 6 },
    { NULL, 0, 0 }
};

/*
 * Time units table.
 */

static const TABLE UnitsTable[] = {
    { "year",		tMONTH_UNIT,	12 },
    { "month",		tMONTH_UNIT,	 1 },
    { "fortnight",	tDAY_UNIT,	14 },
    { "week",		tDAY_UNIT,	 7 },
    { "day",		tDAY_UNIT,	 1 },
    { "hour",		tSEC_UNIT, 60 * 60 },
    { "minute",		tSEC_UNIT,	60 },
    { "min",		tSEC_UNIT,	60 },
    { "second",		tSEC_UNIT,	 1 },
    { "sec",		tSEC_UNIT,	 1 },
    { NULL, 0, 0 }
};

/*
 * Assorted relative-time words.
 */

static const TABLE OtherTable[] = {
    { "tomorrow",	tDAY_UNIT,	1 },
    { "yesterday",	tDAY_UNIT,	-1 },
    { "today",		tDAY_UNIT,	0 },
    { "now",		tSEC_UNIT,	0 },
    { "last",		tUNUMBER,	-1 },
    { "this",		tSEC_UNIT,	0 },
    { "next",		tNEXT,		1 },
#if 0
    { "first",		tUNUMBER,	1 },
    { "second",		tUNUMBER,	2 },
    { "third",		tUNUMBER,	3 },
    { "fourth",		tUNUMBER,	4 },
    { "fifth",		tUNUMBER,	5 },
    { "sixth",		tUNUMBER,	6 },
    { "seventh",	tUNUMBER,	7 },
    { "eighth",		tUNUMBER,	8 },
    { "ninth",		tUNUMBER,	9 },
    { "tenth",		tUNUMBER,	10 },
    { "eleventh",	tUNUMBER,	11 },
    { "twelfth",	tUNUMBER,	12 },
#endif
    { "ago",		tAGO,		1 },
    { "epoch",		tEPOCH,		0 },
    { "stardate",	tSTARDATE,	0 },
    { NULL, 0, 0 }
};

/*
 * The timezone table. (Note: This table was modified to not use any floating
 * point constants to work around an SGI compiler bug).
 */

static const TABLE TimezoneTable[] = {
    { "gmt",	tZONE,	   HOUR( 0) },	    /* Greenwich Mean */
    { "ut",	tZONE,	   HOUR( 0) },	    /* Universal (Coordinated) */
    { "utc",	tZONE,	   HOUR( 0) },
    { "uct",	tZONE,	   HOUR( 0) },	    /* Universal Coordinated Time */
    { "wet",	tZONE,	   HOUR( 0) },	    /* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	    /* British Summer */
    { "wat",	tZONE,	   HOUR( 1) },	    /* West Africa */
    { "at",	tZONE,	   HOUR( 2) },	    /* Azores */
#if	0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",	tZONE,	   HOUR( 3) },	    /* Brazil Standard */
    { "gst",	tZONE,	   HOUR( 3) },	    /* Greenland Standard */
#endif
    { "nft",	tZONE,	   HOUR( 7/2) },    /* Newfoundland */
    { "nst",	tZONE,	   HOUR( 7/2) },    /* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR( 7/2) },    /* Newfoundland Daylight */
    { "ast",	tZONE,	   HOUR( 4) },	    /* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	    /* Atlantic Daylight */
    { "est",	tZONE,	   HOUR( 5) },	    /* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	    /* Eastern Daylight */
    { "cst",	tZONE,	   HOUR( 6) },	    /* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	    /* Central Daylight */
    { "mst",	tZONE,	   HOUR( 7) },	    /* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	    /* Mountain Daylight */
    { "pst",	tZONE,	   HOUR( 8) },	    /* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	    /* Pacific Daylight */
    { "yst",	tZONE,	   HOUR( 9) },	    /* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	    /* Yukon Daylight */
    { "hst",	tZONE,	   HOUR(10) },	    /* Hawaii Standard */
    { "hdt",	tDAYZONE,  HOUR(10) },	    /* Hawaii Daylight */
    { "cat",	tZONE,	   HOUR(10) },	    /* Central Alaska */
    { "ahst",	tZONE,	   HOUR(10) },	    /* Alaska-Hawaii Standard */
    { "nt",	tZONE,	   HOUR(11) },	    /* Nome */
    { "idlw",	tZONE,	   HOUR(12) },	    /* International Date Line West */
    { "cet",	tZONE,	  -HOUR( 1) },	    /* Central European */
    { "cest",	tDAYZONE, -HOUR( 1) },	    /* Central European Summer */
    { "met",	tZONE,	  -HOUR( 1) },	    /* Middle European */
    { "mewt",	tZONE,	  -HOUR( 1) },	    /* Middle European Winter */
    { "mest",	tDAYZONE, -HOUR( 1) },	    /* Middle European Summer */
    { "swt",	tZONE,	  -HOUR( 1) },	    /* Swedish Winter */
    { "sst",	tDAYZONE, -HOUR( 1) },	    /* Swedish Summer */
    { "fwt",	tZONE,	  -HOUR( 1) },	    /* French Winter */
    { "fst",	tDAYZONE, -HOUR( 1) },	    /* French Summer */
    { "eet",	tZONE,	  -HOUR( 2) },	    /* Eastern Europe, USSR Zone 1 */
    { "bt",	tZONE,	  -HOUR( 3) },	    /* Baghdad, USSR Zone 2 */
    { "it",	tZONE,	  -HOUR( 7/2) },    /* Iran */
    { "zp4",	tZONE,	  -HOUR( 4) },	    /* USSR Zone 3 */
    { "zp5",	tZONE,	  -HOUR( 5) },	    /* USSR Zone 4 */
    { "ist",	tZONE,	  -HOUR(11/2) },    /* Indian Standard */
    { "zp6",	tZONE,	  -HOUR( 6) },	    /* USSR Zone 5 */
#if	0
    /* For completeness.  NST is also Newfoundland Stanard, nad SST is
     * also Swedish Summer. */
    { "nst",	tZONE,	  -HOUR(13/2) },    /* North Sumatra */
    { "sst",	tZONE,	  -HOUR( 7) },	    /* South Sumatra, USSR Zone 6 */
#endif	/* 0 */
    { "wast",	tZONE,	  -HOUR( 7) },	    /* West Australian Standard */
    { "wadt",	tDAYZONE, -HOUR( 7) },	    /* West Australian Daylight */
    { "jt",	tZONE,	  -HOUR(15/2) },    /* Java (3pm in Cronusland!) */
    { "cct",	tZONE,	  -HOUR( 8) },	    /* China Coast, USSR Zone 7 */
    { "jst",	tZONE,	  -HOUR( 9) },	    /* Japan Standard, USSR Zone 8 */
    { "jdt",	tDAYZONE, -HOUR( 9) },	    /* Japan Daylight */
    { "kst",	tZONE,	  -HOUR( 9) },	    /* Korea Standard */
    { "kdt",	tDAYZONE, -HOUR( 9) },	    /* Korea Daylight */
    { "cast",	tZONE,	  -HOUR(19/2) },    /* Central Australian Standard */
    { "cadt",	tDAYZONE, -HOUR(19/2) },    /* Central Australian Daylight */
    { "east",	tZONE,	  -HOUR(10) },	    /* Eastern Australian Standard */
    { "eadt",	tDAYZONE, -HOUR(10) },	    /* Eastern Australian Daylight */
    { "gst",	tZONE,	  -HOUR(10) },	    /* Guam Standard, USSR Zone 9 */
    { "nzt",	tZONE,	  -HOUR(12) },	    /* New Zealand */
    { "nzst",	tZONE,	  -HOUR(12) },	    /* New Zealand Standard */
    { "nzdt",	tDAYZONE, -HOUR(12) },	    /* New Zealand Daylight */
    { "idle",	tZONE,	  -HOUR(12) },	    /* International Date Line East */
    /* ADDED BY Marco Nijdam */
    { "dst",	tDST,	  HOUR( 0) },	    /* DST on (hour is ignored) */
    /* End ADDED */
    { NULL, 0, 0 }
};

/*
 * Military timezone table.
 */

static const TABLE MilitaryTable[] = {
    { "a",	tZONE,	-HOUR( 1) },
    { "b",	tZONE,	-HOUR( 2) },
    { "c",	tZONE,	-HOUR( 3) },
    { "d",	tZONE,	-HOUR( 4) },
    { "e",	tZONE,	-HOUR( 5) },
    { "f",	tZONE,	-HOUR( 6) },
    { "g",	tZONE,	-HOUR( 7) },
    { "h",	tZONE,	-HOUR( 8) },
    { "i",	tZONE,	-HOUR( 9) },
    { "k",	tZONE,	-HOUR(10) },
    { "l",	tZONE,	-HOUR(11) },
    { "m",	tZONE,	-HOUR(12) },
    { "n",	tZONE,	HOUR(  1) },
    { "o",	tZONE,	HOUR(  2) },
    { "p",	tZONE,	HOUR(  3) },
    { "q",	tZONE,	HOUR(  4) },
    { "r",	tZONE,	HOUR(  5) },
    { "s",	tZONE,	HOUR(  6) },
    { "t",	tZONE,	HOUR(  7) },
    { "u",	tZONE,	HOUR(  8) },
    { "v",	tZONE,	HOUR(  9) },
    { "w",	tZONE,	HOUR( 10) },
    { "x",	tZONE,	HOUR( 11) },
    { "y",	tZONE,	HOUR( 12) },
    { "z",	tZONE,	HOUR( 0) },
    { NULL, 0, 0 }
};

/*
 * Dump error messages in the bit bucket.
 */

static void
TclDateerror(
    YYLTYPE* location,
    DateInfo* infoPtr,
    const char *s)
{
    Tcl_Obj* t;
    Tcl_AppendToObj(infoPtr->messages, infoPtr->separatrix, -1);
    Tcl_AppendToObj(infoPtr->messages, s, -1);
    Tcl_AppendToObj(infoPtr->messages, " (characters ", -1);
    t = Tcl_NewIntObj(location->first_column);
    Tcl_IncrRefCount(t);
    Tcl_AppendObjToObj(infoPtr->messages, t);
    Tcl_DecrRefCount(t);
    Tcl_AppendToObj(infoPtr->messages, "-", -1);
    t = Tcl_NewIntObj(location->last_column);
    Tcl_IncrRefCount(t);
    Tcl_AppendObjToObj(infoPtr->messages, t);
    Tcl_DecrRefCount(t);
    Tcl_AppendToObj(infoPtr->messages, ")", -1);
    infoPtr->separatrix = "\n";
}

int
ToSeconds(
    int Hours,
    int Minutes,
    int Seconds,
    MERIDIAN Meridian)
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 59) {
	return -1;
    }
    switch (Meridian) {
    case MER24:
	if (Hours < 0 || Hours > 23) {
	    return -1;
	}
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERam:
	if (Hours < 1 || Hours > 12) {
	    return -1;
	}
	return ((Hours % 12) * 60L + Minutes) * 60L + Seconds;
    case MERpm:
	if (Hours < 1 || Hours > 12) {
	    return -1;
	}
	return (((Hours % 12) + 12) * 60L + Minutes) * 60L + Seconds;
    }
    return -1;			/* Should never be reached */
}

static int
LookupWord(
    YYSTYPE* yylvalPtr,
    char *buff)
{
    register char *p;
    register char *q;
    register const TABLE *tp;
    int i, abbrev;

    /*
     * Make it lowercase.
     */

    Tcl_UtfToLower(buff);

    if (strcmp(buff, "am") == 0 || strcmp(buff, "a.m.") == 0) {
	yylvalPtr->Meridian = MERam;
	return tMERIDIAN;
    }
    if (strcmp(buff, "pm") == 0 || strcmp(buff, "p.m.") == 0) {
	yylvalPtr->Meridian = MERpm;
	return tMERIDIAN;
    }

    /*
     * See if we have an abbreviation for a month.
     */

    if (strlen(buff) == 3) {
	abbrev = 1;
    } else if (strlen(buff) == 4 && buff[3] == '.') {
	abbrev = 1;
	buff[3] = '\0';
    } else {
	abbrev = 0;
    }

    for (tp = MonthDayTable; tp->name; tp++) {
	if (abbrev) {
	    if (strncmp(buff, tp->name, 3) == 0) {
		yylvalPtr->Number = tp->value;
		return tp->type;
	    }
	} else if (strcmp(buff, tp->name) == 0) {
	    yylvalPtr->Number = tp->value;
	    return tp->type;
	}
    }

    for (tp = TimezoneTable; tp->name; tp++) {
	if (strcmp(buff, tp->name) == 0) {
	    yylvalPtr->Number = tp->value;
	    return tp->type;
	}
    }

    for (tp = UnitsTable; tp->name; tp++) {
	if (strcmp(buff, tp->name) == 0) {
	    yylvalPtr->Number = tp->value;
	    return tp->type;
	}
    }

    /*
     * Strip off any plural and try the units table again.
     */

    i = strlen(buff) - 1;
    if (i > 0 && buff[i] == 's') {
	buff[i] = '\0';
	for (tp = UnitsTable; tp->name; tp++) {
	    if (strcmp(buff, tp->name) == 0) {
		yylvalPtr->Number = tp->value;
		return tp->type;
	    }
	}
    }

    for (tp = OtherTable; tp->name; tp++) {
	if (strcmp(buff, tp->name) == 0) {
	    yylvalPtr->Number = tp->value;
	    return tp->type;
	}
    }

    /*
     * Military timezones.
     */

    if (buff[1] == '\0' && !(*buff & 0x80)
	    && isalpha(UCHAR(*buff))) {			/* INTL: ISO only */
	for (tp = MilitaryTable; tp->name; tp++) {
	    if (strcmp(buff, tp->name) == 0) {
		yylvalPtr->Number = tp->value;
		return tp->type;
	    }
	}
    }

    /*
     * Drop out any periods and try the timezone table again.
     */

    for (i = 0, p = q = buff; *q; q++) {
	if (*q != '.') {
	    *p++ = *q;
	} else {
	    i++;
	}
    }
    *p = '\0';
    if (i) {
	for (tp = TimezoneTable; tp->name; tp++) {
	    if (strcmp(buff, tp->name) == 0) {
		yylvalPtr->Number = tp->value;
		return tp->type;
	    }
	}
    }

    return tID;
}

static int
TclDatelex(
    YYSTYPE* yylvalPtr,
    YYLTYPE* location,
    DateInfo *info)
{
    register char c;
    register char *p;
    char buff[20];
    int Count;

    location->first_column = yyInput - info->dateStart;
    for ( ; ; ) {
	while (isspace(UCHAR(*yyInput))) {
	    yyInput++;
	}

	if (isdigit(UCHAR(c = *yyInput))) { /* INTL: digit */
	    /*
	     * Convert the string into a number; count the number of digits.
	     */

	    Count = 0;
	    for (yylvalPtr->Number = 0;
		    isdigit(UCHAR(c = *yyInput++)); ) {	  /* INTL: digit */
		yylvalPtr->Number = 10 * yylvalPtr->Number + c - '0';
		Count++;
	    }
	    yyInput--;
	    yyDigitCount = Count;

	    /*
	     * A number with 6 or more digits is considered an ISO 8601 base.
	     */

	    if (Count >= 6) {
		location->last_column = yyInput - info->dateStart - 1;
		return tISOBASE;
	    } else {
		location->last_column = yyInput - info->dateStart - 1;
		return tUNUMBER;
	    }
	}
	if (!(c & 0x80) && isalpha(UCHAR(c))) {		  /* INTL: ISO only. */
	    for (p = buff; isalpha(UCHAR(c = *yyInput++)) /* INTL: ISO only. */
		     || c == '.'; ) {
		if (p < &buff[sizeof buff - 1]) {
		    *p++ = c;
		}
	    }
	    *p = '\0';
	    yyInput--;
	    location->last_column = yyInput - info->dateStart - 1;
	    return LookupWord(yylvalPtr, buff);
	}
	if (c != '(') {
	    location->last_column = yyInput - info->dateStart;
	    return *yyInput++;
	}
	Count = 0;
	do {
	    c = *yyInput++;
	    if (c == '\0') {
		location->last_column = yyInput - info->dateStart - 1;
		return c;
	    } else if (c == '(') {
		Count++;
	    } else if (c == ')') {
		Count--;
	    }
	} while (Count > 0);
    }
}

int
TclClockFreeScan(
    Tcl_Interp *interp,		/* Tcl interpreter */
    DateInfo *info)		/* Input and result parameters */
{
    int status;

    /*
     * yyInput = stringToParse;
     *
     * ClockInitDateInfo(info) should be executed to pre-init info;
     */

    yyDSTmode = DSTmaybe;

    info->messages = Tcl_NewObj();
    info->separatrix = "";
    Tcl_IncrRefCount(info->messages);

    info->dateStart = yyInput;
    status = yyparse(info);
    if (status == 1) {
	Tcl_SetObjResult(interp, info->messages);
	Tcl_DecrRefCount(info->messages);
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "PARSE", NULL);
	return TCL_ERROR;
    } else if (status == 2) {
	Tcl_SetObjResult(interp, Tcl_NewStringObj("memory exhausted", -1));
	Tcl_DecrRefCount(info->messages);
	Tcl_SetErrorCode(interp, "TCL", "MEMORY", NULL);
	return TCL_ERROR;
    } else if (status != 0) {
	Tcl_SetObjResult(interp, Tcl_NewStringObj("Unknown status returned "
						  "from date parser. Please "
						  "report this error as a "
						  "bug in Tcl.", -1));
	Tcl_DecrRefCount(info->messages);
	Tcl_SetErrorCode(interp, "TCL", "BUG", NULL);
	return TCL_ERROR;
    }
    Tcl_DecrRefCount(info->messages);

    if (yyHaveDate > 1) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("more than one date in string", -1));
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "MULTIPLE", NULL);
	return TCL_ERROR;
    }
    if (yyHaveTime > 1) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("more than one time of day in string", -1));
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "MULTIPLE", NULL);
	return TCL_ERROR;
    }
    if (yyHaveZone > 1) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("more than one time zone in string", -1));
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "MULTIPLE", NULL);
	return TCL_ERROR;
    }
    if (yyHaveDay > 1) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("more than one weekday in string", -1));
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "MULTIPLE", NULL);
	return TCL_ERROR;
    }
    if (yyHaveOrdinalMonth > 1) {
	Tcl_SetObjResult(interp,
		Tcl_NewStringObj("more than one ordinal month in string", -1));
	Tcl_SetErrorCode(interp, "TCL", "VALUE", "DATE", "MULTIPLE", NULL);
	return TCL_ERROR;
    }

    return TCL_OK;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */

