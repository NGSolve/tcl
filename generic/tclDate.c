/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0

/* If NAME_PREFIX is specified substitute the variables and functions
   names.  */
#define yyparse TclDateparse
#define yylex   TclDatelex
#define yyerror TclDateerror
#define yylval  TclDatelval
#define yychar  TclDatechar
#define yydebug TclDatedebug
#define yynerrs TclDatenerrs


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
     tMINUTE_UNIT = 263,
     tMONTH = 264,
     tMONTH_UNIT = 265,
     tSTARDATE = 266,
     tSEC_UNIT = 267,
     tSNUMBER = 268,
     tUNUMBER = 269,
     tZONE = 270,
     tEPOCH = 271,
     tDST = 272,
     tISOBASE = 273,
     tDAY_UNIT = 274,
     tNEXT = 275
   };
#endif
#define tAGO 258
#define tDAY 259
#define tDAYZONE 260
#define tID 261
#define tMERIDIAN 262
#define tMINUTE_UNIT 263
#define tMONTH 264
#define tMONTH_UNIT 265
#define tSTARDATE 266
#define tSEC_UNIT 267
#define tSNUMBER 268
#define tUNUMBER 269
#define tZONE 270
#define tEPOCH 271
#define tDST 272
#define tISOBASE 273
#define tDAY_UNIT 274
#define tNEXT 275




/* Copy the first part of user declarations.  */
#line 17 "../unix/../generic/tclGetDate.y"

/* 
 * tclDate.c --
 *
 *	This file is generated from a yacc grammar defined in
 *	the file tclGetDate.y.  It should not be edited directly.
 *
 * Copyright (c) 1992-1995 Karl Lehenbauer and Mark Diekhans.
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * SCCSID
 */

/*
 * Bison generates several labels that happen to be unused. MS Visual
 * C++ doesn't like that, and complains.  Tell it to shut up.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4102 )
#endif /* _MSC_VER */

#include "tclInt.h"

#define EPOCH           1970
#define START_OF_TIME   1902
#define END_OF_TIME     2037

/*
 * The offset of tm_year of struct tm returned by localtime, gmtime, etc.
 * I don't know how universal this is; K&R II, the NetBSD manpages, and
 * ../compat/strftime.c all agree that tm_year is the year-1900.  However,
 * some systems may have a different value.  This #define should be the
 * same as in ../compat/strftime.c.
 */
#define TM_YEAR_BASE 1900

#define HOUR(x)         ((int) (60 * x))
#define SECSPERDAY      (24L * 60L * 60L)
#define IsLeapYear(x)   ((x % 4 == 0) && (x % 100 != 0 || x % 400 == 0))

/*
 *  An entry in the lexical lookup table.
 */
typedef struct _TABLE {
    char        *name;
    int         type;
    time_t      value;
} TABLE;


/*
 *  Daylight-savings mode:  on, off, or not yet known.
 */
typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;

/*
 *  Meridian:  am, pm, or 24-hour style.
 */
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;


/*
 *  Global variables.  We could get rid of most of these by using a good
 *  union as the yacc stack.  (This routine was originally written before
 *  yacc had the %union construct.)  Maybe someday; right now we only use
 *  the %union very rarely.
 */

static char     *yyInput;
static DSTMODE  yyDSTmode;
static time_t   yyDayOrdinal;
static time_t   yyDayNumber;
static time_t   yyMonthOrdinal;
static int      yyHaveDate;
static int      yyHaveDay;
static int      yyHaveOrdinalMonth;
static int      yyHaveRel;
static int      yyHaveTime;
static int      yyHaveZone;
static time_t   yyTimezone;
static time_t   yyDay;
static time_t   yyHour;
static time_t   yyMinutes;
static time_t   yyMonth;
static time_t   yySeconds;
static time_t   yyYear;
static MERIDIAN yyMeridian;
static time_t   yyRelMonth;
static time_t   yyRelDay;
static time_t   yyRelSeconds;
static time_t  *yyRelPointer;

/*
 * Prototypes of internal functions.
 */
static void	TclDateerror _ANSI_ARGS_((char *s));
static time_t	ToSeconds _ANSI_ARGS_((time_t Hours, time_t Minutes,
		    time_t Seconds, MERIDIAN Meridian));
static int	Convert _ANSI_ARGS_((time_t Month, time_t Day, time_t Year,
		    time_t Hours, time_t Minutes, time_t Seconds,
		    MERIDIAN Meridia, DSTMODE DSTmode, time_t *TimePtr));
static time_t	DSTcorrect _ANSI_ARGS_((time_t Start, time_t Future));
static time_t	NamedDay _ANSI_ARGS_((time_t Start, time_t DayOrdinal,
		    time_t DayNumber));
static time_t   NamedMonth _ANSI_ARGS_((time_t Start, time_t MonthOrdinal,
                    time_t MonthNumber));
static int	RelativeMonth _ANSI_ARGS_((time_t Start, time_t RelMonth,
		    time_t *TimePtr));
static int	RelativeDay _ANSI_ARGS_((time_t Start, time_t RelDay,
		    time_t *TimePtr));
static int	LookupWord _ANSI_ARGS_((char *buff));
static int	TclDatelex _ANSI_ARGS_((void));




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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 141 "../unix/../generic/tclGetDate.y"
typedef union YYSTYPE {
    time_t              Number;
    enum _MERIDIAN      Meridian;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 253 "../unix/../generic/tclDate.c"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 265 "../unix/../generic/tclDate.c"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   79

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  27
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  16
/* YYNRULES -- Number of rules. */
#define YYNRULES  56
/* YYNRULES -- Number of states. */
#define YYNSTATES  83

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   275

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    26,    23,    22,    25,    24,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    21,     2,
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
      15,    16,    17,    18,    19,    20
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    11,    13,    15,    17,
      19,    21,    23,    25,    28,    33,    39,    46,    54,    57,
      59,    61,    63,    66,    69,    73,    76,    80,    86,    88,
      94,   100,   103,   108,   111,   113,   117,   120,   124,   128,
     136,   139,   144,   147,   149,   153,   156,   159,   163,   165,
     167,   169,   171,   173,   175,   177,   178
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      28,     0,    -1,    -1,    28,    29,    -1,    30,    -1,    31,
      -1,    33,    -1,    34,    -1,    32,    -1,    37,    -1,    35,
      -1,    36,    -1,    41,    -1,    14,     7,    -1,    14,    21,
      14,    42,    -1,    14,    21,    14,    22,    14,    -1,    14,
      21,    14,    21,    14,    42,    -1,    14,    21,    14,    21,
      14,    22,    14,    -1,    15,    17,    -1,    15,    -1,     5,
      -1,     4,    -1,     4,    23,    -1,    14,     4,    -1,    39,
      14,     4,    -1,    20,     4,    -1,    14,    24,    14,    -1,
      14,    24,    14,    24,    14,    -1,    18,    -1,    14,    22,
       9,    22,    14,    -1,    14,    22,    14,    22,    14,    -1,
       9,    14,    -1,     9,    14,    23,    14,    -1,    14,     9,
      -1,    16,    -1,    14,     9,    14,    -1,    20,     9,    -1,
      20,    14,     9,    -1,    18,    15,    18,    -1,    18,    15,
      14,    21,    14,    21,    14,    -1,    18,    18,    -1,    11,
      14,    25,    14,    -1,    38,     3,    -1,    38,    -1,    39,
      14,    40,    -1,    14,    40,    -1,    20,    40,    -1,    20,
      14,    40,    -1,    40,    -1,    22,    -1,    26,    -1,    12,
      -1,    19,    -1,    10,    -1,    14,    -1,    -1,     7,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   157,   157,   158,   161,   164,   167,   170,   173,   176,
     179,   183,   188,   191,   197,   203,   210,   216,   226,   230,
     234,   240,   244,   248,   252,   256,   262,   266,   271,   276,
     281,   286,   290,   295,   299,   304,   311,   315,   321,   330,
     339,   349,   362,   367,   369,   370,   371,   372,   373,   375,
     376,   378,   379,   380,   383,   402,   405
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "tAGO", "tDAY", "tDAYZONE", "tID", 
  "tMERIDIAN", "tMINUTE_UNIT", "tMONTH", "tMONTH_UNIT", "tSTARDATE", 
  "tSEC_UNIT", "tSNUMBER", "tUNUMBER", "tZONE", "tEPOCH", "tDST", 
  "tISOBASE", "tDAY_UNIT", "tNEXT", "':'", "'-'", "','", "'/'", "'.'", 
  "'+'", "$accept", "spec", "item", "time", "zone", "day", "date", 
  "ordMonth", "iso", "trek", "relspec", "relunits", "sign", "unit", 
  "number", "o_merid", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,    58,    45,    44,    47,    46,    43
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    27,    28,    28,    29,    29,    29,    29,    29,    29,
      29,    29,    29,    30,    30,    30,    30,    30,    31,    31,
      31,    32,    32,    32,    32,    32,    33,    33,    33,    33,
      33,    33,    33,    33,    33,    33,    34,    34,    35,    35,
      35,    36,    37,    37,    38,    38,    38,    38,    38,    39,
      39,    40,    40,    40,    41,    42,    42
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     0,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     4,     5,     6,     7,     2,     1,
       1,     1,     2,     2,     3,     2,     3,     5,     1,     5,
       5,     2,     4,     2,     1,     3,     2,     3,     3,     7,
       2,     4,     2,     1,     3,     2,     2,     3,     1,     1,
       1,     1,     1,     1,     1,     0,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       2,     0,     1,    21,    20,     0,    53,     0,    51,    54,
      19,    34,    28,    52,     0,    49,    50,     3,     4,     5,
       8,     6,     7,    10,    11,     9,    43,     0,    48,    12,
      22,    31,     0,    23,    13,    33,     0,     0,     0,    45,
      18,     0,    40,    25,    36,     0,    46,    42,     0,     0,
       0,    35,    55,     0,     0,    26,     0,    38,    37,    47,
      24,    44,    32,    41,    56,     0,     0,    14,     0,     0,
       0,     0,    55,    15,    29,    30,    27,     0,     0,    16,
       0,    17,    39
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,     1,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    67
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -23
static const yysigned_char yypact[] =
{
     -23,     2,   -23,   -22,   -23,    -5,   -23,    -4,   -23,    22,
      -2,   -23,    12,   -23,    38,   -23,   -23,   -23,   -23,   -23,
     -23,   -23,   -23,   -23,   -23,   -23,    30,    11,   -23,   -23,
     -23,    17,    10,   -23,   -23,    35,    40,    -6,    47,   -23,
     -23,    45,   -23,   -23,   -23,    46,   -23,   -23,    41,    48,
      50,   -23,    16,    44,    49,    43,    51,   -23,   -23,   -23,
     -23,   -23,   -23,   -23,   -23,    54,    55,   -23,    56,    59,
      60,    61,    -3,   -23,   -23,   -23,   -23,    57,    62,   -23,
      63,   -23,   -23
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
     -23,   -23,   -23,   -23,   -23,   -23,   -23,   -23,   -23,   -23,
     -23,   -23,   -23,    -9,   -23,     7
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const unsigned char yytable[] =
{
      39,    30,     2,    53,    64,    46,     3,     4,    54,    31,
      32,     5,     6,     7,     8,    40,     9,    10,    11,    78,
      12,    13,    14,    64,    15,    48,    33,    41,    16,    34,
      42,    35,     6,    47,     8,    50,    59,    65,    66,    61,
      49,    13,    43,    36,    37,    60,    38,    44,     6,    51,
       8,     6,    45,     8,    52,    58,     6,    13,     8,    56,
      13,    55,    62,    57,    63,    13,    68,    70,    72,    73,
      74,    69,    71,    75,    76,    77,    81,    82,    80,    79
};

static const unsigned char yycheck[] =
{
       9,    23,     0,     9,     7,    14,     4,     5,    14,    14,
      14,     9,    10,    11,    12,    17,    14,    15,    16,    22,
      18,    19,    20,     7,    22,    14,     4,    15,    26,     7,
      18,     9,    10,     3,    12,    25,    45,    21,    22,    48,
      23,    19,     4,    21,    22,     4,    24,     9,    10,    14,
      12,    10,    14,    12,    14,     9,    10,    19,    12,    14,
      19,    14,    14,    18,    14,    19,    22,    24,    14,    14,
      14,    22,    21,    14,    14,    14,    14,    14,    21,    72
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,    28,     0,     4,     5,     9,    10,    11,    12,    14,
      15,    16,    18,    19,    20,    22,    26,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      23,    14,    14,     4,     7,     9,    21,    22,    24,    40,
      17,    15,    18,     4,     9,    14,    40,     3,    14,    23,
      25,    14,    14,     9,    14,    14,    14,    18,     9,    40,
       4,    40,    14,    14,     7,    21,    22,    42,    22,    22,
      24,    21,    14,    14,    14,    14,    14,    14,    22,    42,
      21,    14,    14
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
#endif

/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 4:
#line 161 "../unix/../generic/tclGetDate.y"
    {
            yyHaveTime++;
        ;}
    break;

  case 5:
#line 164 "../unix/../generic/tclGetDate.y"
    {
            yyHaveZone++;
        ;}
    break;

  case 6:
#line 167 "../unix/../generic/tclGetDate.y"
    {
            yyHaveDate++;
        ;}
    break;

  case 7:
#line 170 "../unix/../generic/tclGetDate.y"
    {
            yyHaveOrdinalMonth++;
        ;}
    break;

  case 8:
#line 173 "../unix/../generic/tclGetDate.y"
    {
            yyHaveDay++;
        ;}
    break;

  case 9:
#line 176 "../unix/../generic/tclGetDate.y"
    {
            yyHaveRel++;
        ;}
    break;

  case 10:
#line 179 "../unix/../generic/tclGetDate.y"
    {
	    yyHaveTime++;
	    yyHaveDate++;
	;}
    break;

  case 11:
#line 183 "../unix/../generic/tclGetDate.y"
    {
	    yyHaveTime++;
	    yyHaveDate++;
	    yyHaveRel++;
        ;}
    break;

  case 13:
#line 191 "../unix/../generic/tclGetDate.y"
    {
            yyHour = yyvsp[-1].Number;
            yyMinutes = 0;
            yySeconds = 0;
            yyMeridian = yyvsp[0].Meridian;
        ;}
    break;

  case 14:
#line 197 "../unix/../generic/tclGetDate.y"
    {
            yyHour = yyvsp[-3].Number;
            yyMinutes = yyvsp[-1].Number;
            yySeconds = 0;
            yyMeridian = yyvsp[0].Meridian;
        ;}
    break;

  case 15:
#line 203 "../unix/../generic/tclGetDate.y"
    {
            yyHour = yyvsp[-4].Number;
            yyMinutes = yyvsp[-2].Number;
            yyMeridian = MER24;
            yyDSTmode = DSToff;
            yyTimezone = (yyvsp[0].Number % 100 + (yyvsp[0].Number / 100) * 60);
        ;}
    break;

  case 16:
#line 210 "../unix/../generic/tclGetDate.y"
    {
            yyHour = yyvsp[-5].Number;
            yyMinutes = yyvsp[-3].Number;
            yySeconds = yyvsp[-1].Number;
            yyMeridian = yyvsp[0].Meridian;
        ;}
    break;

  case 17:
#line 216 "../unix/../generic/tclGetDate.y"
    {
            yyHour = yyvsp[-6].Number;
            yyMinutes = yyvsp[-4].Number;
            yySeconds = yyvsp[-2].Number;
            yyMeridian = MER24;
            yyDSTmode = DSToff;
            yyTimezone = (yyvsp[0].Number % 100 + (yyvsp[0].Number / 100) * 60);
        ;}
    break;

  case 18:
#line 226 "../unix/../generic/tclGetDate.y"
    {
            yyTimezone = yyvsp[-1].Number;
            yyDSTmode = DSTon;
        ;}
    break;

  case 19:
#line 230 "../unix/../generic/tclGetDate.y"
    {
            yyTimezone = yyvsp[0].Number;
            yyDSTmode = DSToff;
        ;}
    break;

  case 20:
#line 234 "../unix/../generic/tclGetDate.y"
    {
            yyTimezone = yyvsp[0].Number;
            yyDSTmode = DSTon;
        ;}
    break;

  case 21:
#line 240 "../unix/../generic/tclGetDate.y"
    {
            yyDayOrdinal = 1;
            yyDayNumber = yyvsp[0].Number;
        ;}
    break;

  case 22:
#line 244 "../unix/../generic/tclGetDate.y"
    {
            yyDayOrdinal = 1;
            yyDayNumber = yyvsp[-1].Number;
        ;}
    break;

  case 23:
#line 248 "../unix/../generic/tclGetDate.y"
    {
            yyDayOrdinal = yyvsp[-1].Number;
            yyDayNumber = yyvsp[0].Number;
        ;}
    break;

  case 24:
#line 252 "../unix/../generic/tclGetDate.y"
    {
            yyDayOrdinal = yyvsp[-2].Number * yyvsp[-1].Number;
            yyDayNumber = yyvsp[0].Number;
        ;}
    break;

  case 25:
#line 256 "../unix/../generic/tclGetDate.y"
    {
            yyDayOrdinal = 2;
            yyDayNumber = yyvsp[0].Number;
        ;}
    break;

  case 26:
#line 262 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-2].Number;
            yyDay = yyvsp[0].Number;
        ;}
    break;

  case 27:
#line 266 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-4].Number;
            yyDay = yyvsp[-2].Number;
            yyYear = yyvsp[0].Number;
        ;}
    break;

  case 28:
#line 271 "../unix/../generic/tclGetDate.y"
    {
	    yyYear = yyvsp[0].Number / 10000;
	    yyMonth = (yyvsp[0].Number % 10000)/100;
	    yyDay = yyvsp[0].Number % 100;
	;}
    break;

  case 29:
#line 276 "../unix/../generic/tclGetDate.y"
    {
	    yyDay = yyvsp[-4].Number;
	    yyMonth = yyvsp[-2].Number;
	    yyYear = yyvsp[0].Number;
	;}
    break;

  case 30:
#line 281 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-2].Number;
            yyDay = yyvsp[0].Number;
            yyYear = yyvsp[-4].Number;
        ;}
    break;

  case 31:
#line 286 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-1].Number;
            yyDay = yyvsp[0].Number;
        ;}
    break;

  case 32:
#line 290 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-3].Number;
            yyDay = yyvsp[-2].Number;
            yyYear = yyvsp[0].Number;
        ;}
    break;

  case 33:
#line 295 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[0].Number;
            yyDay = yyvsp[-1].Number;
        ;}
    break;

  case 34:
#line 299 "../unix/../generic/tclGetDate.y"
    {
	    yyMonth = 1;
	    yyDay = 1;
	    yyYear = EPOCH;
	;}
    break;

  case 35:
#line 304 "../unix/../generic/tclGetDate.y"
    {
            yyMonth = yyvsp[-1].Number;
            yyDay = yyvsp[-2].Number;
            yyYear = yyvsp[0].Number;
        ;}
    break;

  case 36:
#line 311 "../unix/../generic/tclGetDate.y"
    {
	    yyMonthOrdinal = 1;
	    yyMonth = yyvsp[0].Number;
	;}
    break;

  case 37:
#line 315 "../unix/../generic/tclGetDate.y"
    {
	    yyMonthOrdinal = yyvsp[-1].Number;
	    yyMonth = yyvsp[0].Number;
	;}
    break;

  case 38:
#line 321 "../unix/../generic/tclGetDate.y"
    {
            if (yyvsp[-1].Number != HOUR(- 7)) YYABORT;
	    yyYear = yyvsp[-2].Number / 10000;
	    yyMonth = (yyvsp[-2].Number % 10000)/100;
	    yyDay = yyvsp[-2].Number % 100;
	    yyHour = yyvsp[0].Number / 10000;
	    yyMinutes = (yyvsp[0].Number % 10000)/100;
	    yySeconds = yyvsp[0].Number % 100;
        ;}
    break;

  case 39:
#line 330 "../unix/../generic/tclGetDate.y"
    {
            if (yyvsp[-5].Number != HOUR(- 7)) YYABORT;
	    yyYear = yyvsp[-6].Number / 10000;
	    yyMonth = (yyvsp[-6].Number % 10000)/100;
	    yyDay = yyvsp[-6].Number % 100;
	    yyHour = yyvsp[-4].Number;
	    yyMinutes = yyvsp[-2].Number;
	    yySeconds = yyvsp[0].Number;
        ;}
    break;

  case 40:
#line 339 "../unix/../generic/tclGetDate.y"
    {
	    yyYear = yyvsp[-1].Number / 10000;
	    yyMonth = (yyvsp[-1].Number % 10000)/100;
	    yyDay = yyvsp[-1].Number % 100;
	    yyHour = yyvsp[0].Number / 10000;
	    yyMinutes = (yyvsp[0].Number % 10000)/100;
	    yySeconds = yyvsp[0].Number % 100;
        ;}
    break;

  case 41:
#line 349 "../unix/../generic/tclGetDate.y"
    {
            /*
	     * Offset computed year by -377 so that the returned years will
	     * be in a range accessible with a 32 bit clock seconds value
	     */
            yyYear = yyvsp[-2].Number/1000 + 2323 - 377;
            yyDay  = 1;
	    yyMonth = 1;
	    yyRelDay += ((yyvsp[-2].Number%1000)*(365 + IsLeapYear(yyYear)))/1000;
	    yyRelSeconds += yyvsp[0].Number * 144 * 60;
        ;}
    break;

  case 42:
#line 362 "../unix/../generic/tclGetDate.y"
    {
	    yyRelSeconds *= -1;
	    yyRelMonth *= -1;
	    yyRelDay *= -1;
	;}
    break;

  case 44:
#line 369 "../unix/../generic/tclGetDate.y"
    { *yyRelPointer += yyvsp[-2].Number * yyvsp[-1].Number * yyvsp[0].Number; ;}
    break;

  case 45:
#line 370 "../unix/../generic/tclGetDate.y"
    { *yyRelPointer += yyvsp[-1].Number * yyvsp[0].Number; ;}
    break;

  case 46:
#line 371 "../unix/../generic/tclGetDate.y"
    { *yyRelPointer += yyvsp[0].Number; ;}
    break;

  case 47:
#line 372 "../unix/../generic/tclGetDate.y"
    { *yyRelPointer += yyvsp[-1].Number * yyvsp[0].Number; ;}
    break;

  case 48:
#line 373 "../unix/../generic/tclGetDate.y"
    { *yyRelPointer += yyvsp[0].Number; ;}
    break;

  case 49:
#line 375 "../unix/../generic/tclGetDate.y"
    { yyval.Number = -1; ;}
    break;

  case 50:
#line 376 "../unix/../generic/tclGetDate.y"
    { yyval.Number =  1; ;}
    break;

  case 51:
#line 378 "../unix/../generic/tclGetDate.y"
    { yyval.Number = yyvsp[0].Number; yyRelPointer = &yyRelSeconds; ;}
    break;

  case 52:
#line 379 "../unix/../generic/tclGetDate.y"
    { yyval.Number = yyvsp[0].Number; yyRelPointer = &yyRelDay; ;}
    break;

  case 53:
#line 380 "../unix/../generic/tclGetDate.y"
    { yyval.Number = yyvsp[0].Number; yyRelPointer = &yyRelMonth; ;}
    break;

  case 54:
#line 384 "../unix/../generic/tclGetDate.y"
    {
	if (yyHaveTime && yyHaveDate && !yyHaveRel) {
	    yyYear = yyvsp[0].Number;
	} else {
	    yyHaveTime++;
	    if (yyvsp[0].Number < 100) {
		yyHour = yyvsp[0].Number;
		yyMinutes = 0;
	    } else {
		yyHour = yyvsp[0].Number / 100;
		yyMinutes = yyvsp[0].Number % 100;
	    }
	    yySeconds = 0;
	    yyMeridian = MER24;
	}
    ;}
    break;

  case 55:
#line 402 "../unix/../generic/tclGetDate.y"
    {
            yyval.Meridian = MER24;
        ;}
    break;

  case 56:
#line 405 "../unix/../generic/tclGetDate.y"
    {
            yyval.Meridian = yyvsp[0].Meridian;
        ;}
    break;


    }

/* Line 991 of yacc.c.  */
#line 1636 "../unix/../generic/tclDate.c"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  Doesn't work in C++ */
#ifndef __cplusplus
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  __attribute__ ((__unused__))
#endif
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 410 "../unix/../generic/tclGetDate.y"


/*
 * Month and day table.
 */
static TABLE    MonthDayTable[] = {
    { "january",        tMONTH,  1 },
    { "february",       tMONTH,  2 },
    { "march",          tMONTH,  3 },
    { "april",          tMONTH,  4 },
    { "may",            tMONTH,  5 },
    { "june",           tMONTH,  6 },
    { "july",           tMONTH,  7 },
    { "august",         tMONTH,  8 },
    { "september",      tMONTH,  9 },
    { "sept",           tMONTH,  9 },
    { "october",        tMONTH, 10 },
    { "november",       tMONTH, 11 },
    { "december",       tMONTH, 12 },
    { "sunday",         tDAY, 0 },
    { "monday",         tDAY, 1 },
    { "tuesday",        tDAY, 2 },
    { "tues",           tDAY, 2 },
    { "wednesday",      tDAY, 3 },
    { "wednes",         tDAY, 3 },
    { "thursday",       tDAY, 4 },
    { "thur",           tDAY, 4 },
    { "thurs",          tDAY, 4 },
    { "friday",         tDAY, 5 },
    { "saturday",       tDAY, 6 },
    { NULL }
};

/*
 * Time units table.
 */
static TABLE    UnitsTable[] = {
    { "year",           tMONTH_UNIT,    12 },
    { "month",          tMONTH_UNIT,     1 },
    { "fortnight",      tDAY_UNIT,      14 },
    { "week",           tDAY_UNIT,       7 },
    { "day",            tDAY_UNIT,       1 },
    { "hour",           tSEC_UNIT, 60 * 60 },
    { "minute",         tSEC_UNIT,      60 },
    { "min",            tSEC_UNIT,      60 },
    { "second",         tSEC_UNIT,       1 },
    { "sec",            tSEC_UNIT,       1 },
    { NULL }
};

/*
 * Assorted relative-time words.
 */
static TABLE    OtherTable[] = {
    { "tomorrow",       tDAY_UNIT,      1 },
    { "yesterday",      tDAY_UNIT,     -1 },
    { "today",          tDAY_UNIT,      0 },
    { "now",            tSEC_UNIT,      0 },
    { "last",           tUNUMBER,      -1 },
    { "this",           tSEC_UNIT,      0 },
    { "next",           tNEXT,          1 },
#if 0
    { "first",          tUNUMBER,       1 },
    { "second",         tUNUMBER,       2 },
    { "third",          tUNUMBER,       3 },
    { "fourth",         tUNUMBER,       4 },
    { "fifth",          tUNUMBER,       5 },
    { "sixth",          tUNUMBER,       6 },
    { "seventh",        tUNUMBER,       7 },
    { "eighth",         tUNUMBER,       8 },
    { "ninth",          tUNUMBER,       9 },
    { "tenth",          tUNUMBER,       10 },
    { "eleventh",       tUNUMBER,       11 },
    { "twelfth",        tUNUMBER,       12 },
#endif
    { "ago",            tAGO,   1 },
    { "epoch",          tEPOCH,   0 },
    { "stardate",       tSTARDATE, 0},
    { NULL }
};

/*
 * The timezone table.  (Note: This table was modified to not use any floating
 * point constants to work around an SGI compiler bug).
 */
static TABLE    TimezoneTable[] = {
    { "gmt",    tZONE,     HOUR( 0) },      /* Greenwich Mean */
    { "ut",     tZONE,     HOUR( 0) },      /* Universal (Coordinated) */
    { "utc",    tZONE,     HOUR( 0) },
    { "uct",    tZONE,     HOUR( 0) },      /* Universal Coordinated Time */
    { "wet",    tZONE,     HOUR( 0) },      /* Western European */
    { "bst",    tDAYZONE,  HOUR( 0) },      /* British Summer */
    { "wat",    tZONE,     HOUR( 1) },      /* West Africa */
    { "at",     tZONE,     HOUR( 2) },      /* Azores */
#if     0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",    tZONE,     HOUR( 3) },      /* Brazil Standard */
    { "gst",    tZONE,     HOUR( 3) },      /* Greenland Standard */
#endif
    { "nft",    tZONE,     HOUR( 7/2) },    /* Newfoundland */
    { "nst",    tZONE,     HOUR( 7/2) },    /* Newfoundland Standard */
    { "ndt",    tDAYZONE,  HOUR( 7/2) },    /* Newfoundland Daylight */
    { "ast",    tZONE,     HOUR( 4) },      /* Atlantic Standard */
    { "adt",    tDAYZONE,  HOUR( 4) },      /* Atlantic Daylight */
    { "est",    tZONE,     HOUR( 5) },      /* Eastern Standard */
    { "edt",    tDAYZONE,  HOUR( 5) },      /* Eastern Daylight */
    { "cst",    tZONE,     HOUR( 6) },      /* Central Standard */
    { "cdt",    tDAYZONE,  HOUR( 6) },      /* Central Daylight */
    { "mst",    tZONE,     HOUR( 7) },      /* Mountain Standard */
    { "mdt",    tDAYZONE,  HOUR( 7) },      /* Mountain Daylight */
    { "pst",    tZONE,     HOUR( 8) },      /* Pacific Standard */
    { "pdt",    tDAYZONE,  HOUR( 8) },      /* Pacific Daylight */
    { "yst",    tZONE,     HOUR( 9) },      /* Yukon Standard */
    { "ydt",    tDAYZONE,  HOUR( 9) },      /* Yukon Daylight */
    { "hst",    tZONE,     HOUR(10) },      /* Hawaii Standard */
    { "hdt",    tDAYZONE,  HOUR(10) },      /* Hawaii Daylight */
    { "cat",    tZONE,     HOUR(10) },      /* Central Alaska */
    { "ahst",   tZONE,     HOUR(10) },      /* Alaska-Hawaii Standard */
    { "nt",     tZONE,     HOUR(11) },      /* Nome */
    { "idlw",   tZONE,     HOUR(12) },      /* International Date Line West */
    { "cet",    tZONE,    -HOUR( 1) },      /* Central European */
    { "cest",   tDAYZONE, -HOUR( 1) },      /* Central European Summer */
    { "met",    tZONE,    -HOUR( 1) },      /* Middle European */
    { "mewt",   tZONE,    -HOUR( 1) },      /* Middle European Winter */
    { "mest",   tDAYZONE, -HOUR( 1) },      /* Middle European Summer */
    { "swt",    tZONE,    -HOUR( 1) },      /* Swedish Winter */
    { "sst",    tDAYZONE, -HOUR( 1) },      /* Swedish Summer */
    { "fwt",    tZONE,    -HOUR( 1) },      /* French Winter */
    { "fst",    tDAYZONE, -HOUR( 1) },      /* French Summer */
    { "eet",    tZONE,    -HOUR( 2) },      /* Eastern Europe, USSR Zone 1 */
    { "bt",     tZONE,    -HOUR( 3) },      /* Baghdad, USSR Zone 2 */
    { "it",     tZONE,    -HOUR( 7/2) },    /* Iran */
    { "zp4",    tZONE,    -HOUR( 4) },      /* USSR Zone 3 */
    { "zp5",    tZONE,    -HOUR( 5) },      /* USSR Zone 4 */
    { "ist",    tZONE,    -HOUR(11/2) },    /* Indian Standard */
    { "zp6",    tZONE,    -HOUR( 6) },      /* USSR Zone 5 */
#if     0
    /* For completeness.  NST is also Newfoundland Stanard, nad SST is
     * also Swedish Summer. */
    { "nst",    tZONE,    -HOUR(13/2) },    /* North Sumatra */
    { "sst",    tZONE,    -HOUR( 7) },      /* South Sumatra, USSR Zone 6 */
#endif  /* 0 */
    { "wast",   tZONE,    -HOUR( 7) },      /* West Australian Standard */
    { "wadt",   tDAYZONE, -HOUR( 7) },      /* West Australian Daylight */
    { "jt",     tZONE,    -HOUR(15/2) },    /* Java (3pm in Cronusland!) */
    { "cct",    tZONE,    -HOUR( 8) },      /* China Coast, USSR Zone 7 */
    { "jst",    tZONE,    -HOUR( 9) },      /* Japan Standard, USSR Zone 8 */
    { "cast",   tZONE,    -HOUR(19/2) },    /* Central Australian Standard */
    { "cadt",   tDAYZONE, -HOUR(19/2) },    /* Central Australian Daylight */
    { "east",   tZONE,    -HOUR(10) },      /* Eastern Australian Standard */
    { "eadt",   tDAYZONE, -HOUR(10) },      /* Eastern Australian Daylight */
    { "gst",    tZONE,    -HOUR(10) },      /* Guam Standard, USSR Zone 9 */
    { "nzt",    tZONE,    -HOUR(12) },      /* New Zealand */
    { "nzst",   tZONE,    -HOUR(12) },      /* New Zealand Standard */
    { "nzdt",   tDAYZONE, -HOUR(12) },      /* New Zealand Daylight */
    { "idle",   tZONE,    -HOUR(12) },      /* International Date Line East */
    /* ADDED BY Marco Nijdam */
    { "dst",    tDST,     HOUR( 0) },       /* DST on (hour is ignored) */
    /* End ADDED */
    {  NULL  }
};

/*
 * Military timezone table.
 */
static TABLE    MilitaryTable[] = {
    { "a",      tZONE,  HOUR(  1) },
    { "b",      tZONE,  HOUR(  2) },
    { "c",      tZONE,  HOUR(  3) },
    { "d",      tZONE,  HOUR(  4) },
    { "e",      tZONE,  HOUR(  5) },
    { "f",      tZONE,  HOUR(  6) },
    { "g",      tZONE,  HOUR(  7) },
    { "h",      tZONE,  HOUR(  8) },
    { "i",      tZONE,  HOUR(  9) },
    { "k",      tZONE,  HOUR( 10) },
    { "l",      tZONE,  HOUR( 11) },
    { "m",      tZONE,  HOUR( 12) },
    { "n",      tZONE,  HOUR(- 1) },
    { "o",      tZONE,  HOUR(- 2) },
    { "p",      tZONE,  HOUR(- 3) },
    { "q",      tZONE,  HOUR(- 4) },
    { "r",      tZONE,  HOUR(- 5) },
    { "s",      tZONE,  HOUR(- 6) },
    { "t",      tZONE,  HOUR(- 7) },
    { "u",      tZONE,  HOUR(- 8) },
    { "v",      tZONE,  HOUR(- 9) },
    { "w",      tZONE,  HOUR(-10) },
    { "x",      tZONE,  HOUR(-11) },
    { "y",      tZONE,  HOUR(-12) },
    { "z",      tZONE,  HOUR(  0) },
    { NULL }
};


/*
 * Dump error messages in the bit bucket.
 */
static void
TclDateerror(s)
    char  *s;
{
}


static time_t
ToSeconds(Hours, Minutes, Seconds, Meridian)
    time_t      Hours;
    time_t      Minutes;
    time_t      Seconds;
    MERIDIAN    Meridian;
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 59)
        return -1;
    switch (Meridian) {
    case MER24:
        if (Hours < 0 || Hours > 23)
            return -1;
        return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERam:
        if (Hours < 1 || Hours > 12)
            return -1;
        return ((Hours % 12) * 60L + Minutes) * 60L + Seconds;
    case MERpm:
        if (Hours < 1 || Hours > 12)
            return -1;
        return (((Hours % 12) + 12) * 60L + Minutes) * 60L + Seconds;
    }
    return -1;  /* Should never be reached */
}

/*
 *-----------------------------------------------------------------------------
 *
 * Convert --
 *
 *      Convert a {month, day, year, hours, minutes, seconds, meridian, dst}
 *      tuple into a clock seconds value.
 *
 * Results:
 *      0 or -1 indicating success or failure.
 *
 * Side effects:
 *      Fills TimePtr with the computed value.
 *
 *-----------------------------------------------------------------------------
 */
static int
Convert(Month, Day, Year, Hours, Minutes, Seconds, Meridian, DSTmode, TimePtr)
    time_t      Month;
    time_t      Day;
    time_t      Year;
    time_t      Hours;
    time_t      Minutes;
    time_t      Seconds;
    MERIDIAN    Meridian;
    DSTMODE     DSTmode;
    time_t     *TimePtr;
{
    static int  DaysInMonth[12] = {
        31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    time_t tod;
    time_t Julian;
    int i;

    /* Figure out how many days are in February for the given year.
     * Every year divisible by 4 is a leap year.
     * But, every year divisible by 100 is not a leap year.
     * But, every year divisible by 400 is a leap year after all.
     */
    DaysInMonth[1] = IsLeapYear(Year) ? 29 : 28;

    /* Check the inputs for validity */
    if (Month < 1 || Month > 12
	    || Year < START_OF_TIME || Year > END_OF_TIME
	    || Day < 1 || Day > DaysInMonth[(int)--Month])
        return -1;

    /* Start computing the value.  First determine the number of days
     * represented by the date, then multiply by the number of seconds/day.
     */
    for (Julian = Day - 1, i = 0; i < Month; i++)
        Julian += DaysInMonth[i];
    if (Year >= EPOCH) {
        for (i = EPOCH; i < Year; i++)
            Julian += 365 + IsLeapYear(i);
    } else {
        for (i = Year; i < EPOCH; i++)
            Julian -= 365 + IsLeapYear(i);
    }
    Julian *= SECSPERDAY;

    /* Add the timezone offset ?? */
    Julian += yyTimezone * 60L;

    /* Add the number of seconds represented by the time component */
    if ((tod = ToSeconds(Hours, Minutes, Seconds, Meridian)) < 0)
        return -1;
    Julian += tod;

    /* Perform a preliminary DST compensation ?? */
    if (DSTmode == DSTon
     || (DSTmode == DSTmaybe && TclpGetDate(&Julian, 0)->tm_isdst))
        Julian -= 60 * 60;
    *TimePtr = Julian;
    return 0;
}


static time_t
DSTcorrect(Start, Future)
    time_t      Start;
    time_t      Future;
{
    time_t      StartDay;
    time_t      FutureDay;
    StartDay = (TclpGetDate(&Start, 0)->tm_hour + 1) % 24;
    FutureDay = (TclpGetDate(&Future, 0)->tm_hour + 1) % 24;
    return (Future - Start) + (StartDay - FutureDay) * 60L * 60L;
}


static time_t
NamedDay(Start, DayOrdinal, DayNumber)
    time_t      Start;
    time_t      DayOrdinal;
    time_t      DayNumber;
{
    struct tm   *tm;
    time_t      now;

    now = Start;
    tm = TclpGetDate(&now, 0);
    now += SECSPERDAY * ((DayNumber - tm->tm_wday + 7) % 7);
    now += 7 * SECSPERDAY * (DayOrdinal <= 0 ? DayOrdinal : DayOrdinal - 1);
    return DSTcorrect(Start, now);
}

static time_t
NamedMonth(Start, MonthOrdinal, MonthNumber)
    time_t Start;
    time_t MonthOrdinal;
    time_t MonthNumber;
{
    struct tm *tm;
    time_t now;
    int result;
    
    now = Start;
    tm = TclpGetDate(&now, 0);
    /* To compute the next n'th month, we use this alg:
     * add n to year value
     * if currentMonth < requestedMonth decrement year value by 1 (so that
     *  doing next february from january gives us february of the current year)
     * set day to 1, time to 0
     */
    tm->tm_year += MonthOrdinal;
    if (tm->tm_mon < MonthNumber - 1) {
	tm->tm_year--;
    }
    result = Convert(MonthNumber, (time_t) 1, tm->tm_year + TM_YEAR_BASE,
	    (time_t) 0, (time_t) 0, (time_t) 0, MER24, DSTmaybe, &now);
    if (result < 0) {
	return 0;
    }
    return DSTcorrect(Start, now);
}

static int
RelativeMonth(Start, RelMonth, TimePtr)
    time_t Start;
    time_t RelMonth;
    time_t *TimePtr;
{
    struct tm *tm;
    time_t Month;
    time_t Year;
    time_t Julian;
    int result;

    if (RelMonth == 0) {
        *TimePtr = 0;
        return 0;
    }
    tm = TclpGetDate(&Start, 0);
    Month = 12 * (tm->tm_year + TM_YEAR_BASE) + tm->tm_mon + RelMonth;
    Year = Month / 12;
    Month = Month % 12 + 1;
    result = Convert(Month, (time_t) tm->tm_mday, Year,
	    (time_t) tm->tm_hour, (time_t) tm->tm_min, (time_t) tm->tm_sec,
	    MER24, DSTmaybe, &Julian);

    /*
     * The Julian time returned above is behind by one day, if "month" 
     * or "year" is used to specify relative time and the GMT flag is true.
     * This problem occurs only when the current time is closer to
     * midnight, the difference being not more than its time difference
     * with GMT. For example, in US/Pacific time zone, the problem occurs
     * whenever the current time is between midnight to 8:00am or 7:00amDST.
     * See Bug# 413397 for more details and sample script.
     * To resolve this bug, we simply add the number of seconds corresponding
     * to timezone difference with GMT to Julian time, if GMT flag is true.
     */

    if (yyTimezone == 0) {
        Julian += TclpGetTimeZone((unsigned long) Start) * 60L;
    }

    /*
     * The following iteration takes into account the case were we jump
     * into a "short month".  Far example, "one month from Jan 31" will
     * fail because there is no Feb 31.  The code below will reduce the
     * day and try converting the date until we succed or the date equals
     * 28 (which always works unless the date is bad in another way).
     */

    while ((result != 0) && (tm->tm_mday > 28)) {
	tm->tm_mday--;
	result = Convert(Month, (time_t) tm->tm_mday, Year,
		(time_t) tm->tm_hour, (time_t) tm->tm_min, (time_t) tm->tm_sec,
		MER24, DSTmaybe, &Julian);
    }
    if (result != 0) {
	return -1;
    }
    *TimePtr = DSTcorrect(Start, Julian);
    return 0;
}


/*
 *-----------------------------------------------------------------------------
 *
 * RelativeDay --
 *
 *      Given a starting time and a number of days before or after, compute the
 *      DST corrected difference between those dates.
 *
 * Results:
 *     1 or -1 indicating success or failure.
 *
 * Side effects:
 *      Fills TimePtr with the computed value.
 *
 *-----------------------------------------------------------------------------
 */

static int
RelativeDay(Start, RelDay, TimePtr)
    time_t Start;
    time_t RelDay;
    time_t *TimePtr;
{
    time_t new;

    new = Start + (RelDay * 60 * 60 * 24);
    *TimePtr = DSTcorrect(Start, new);
    return 1;
}

static int
LookupWord(buff)
    char                *buff;
{
    register char *p;
    register char *q;
    register TABLE *tp;
    int i;
    int abbrev;

    /*
     * Make it lowercase.
     */

    Tcl_UtfToLower(buff);

    if (strcmp(buff, "am") == 0 || strcmp(buff, "a.m.") == 0) {
        yylval.Meridian = MERam;
        return tMERIDIAN;
    }
    if (strcmp(buff, "pm") == 0 || strcmp(buff, "p.m.") == 0) {
        yylval.Meridian = MERpm;
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
                yylval.Number = tp->value;
                return tp->type;
            }
        } else if (strcmp(buff, tp->name) == 0) {
            yylval.Number = tp->value;
            return tp->type;
        }
    }

    for (tp = TimezoneTable; tp->name; tp++) {
        if (strcmp(buff, tp->name) == 0) {
            yylval.Number = tp->value;
            return tp->type;
        }
    }

    for (tp = UnitsTable; tp->name; tp++) {
        if (strcmp(buff, tp->name) == 0) {
            yylval.Number = tp->value;
            return tp->type;
        }
    }

    /*
     * Strip off any plural and try the units table again.
     */
    i = strlen(buff) - 1;
    if (buff[i] == 's') {
        buff[i] = '\0';
        for (tp = UnitsTable; tp->name; tp++) {
            if (strcmp(buff, tp->name) == 0) {
                yylval.Number = tp->value;
                return tp->type;
            }
	}
    }

    for (tp = OtherTable; tp->name; tp++) {
        if (strcmp(buff, tp->name) == 0) {
            yylval.Number = tp->value;
            return tp->type;
        }
    }

    /*
     * Military timezones.
     */
    if (buff[1] == '\0' && !(*buff & 0x80)
	    && isalpha(UCHAR(*buff))) {	/* INTL: ISO only */
        for (tp = MilitaryTable; tp->name; tp++) {
            if (strcmp(buff, tp->name) == 0) {
                yylval.Number = tp->value;
                return tp->type;
            }
	}
    }

    /*
     * Drop out any periods and try the timezone table again.
     */
    for (i = 0, p = q = buff; *q; q++)
        if (*q != '.') {
            *p++ = *q;
        } else {
            i++;
	}
    *p = '\0';
    if (i) {
        for (tp = TimezoneTable; tp->name; tp++) {
            if (strcmp(buff, tp->name) == 0) {
                yylval.Number = tp->value;
                return tp->type;
            }
	}
    }
    
    return tID;
}


static int
TclDatelex()
{
    register char       c;
    register char       *p;
    char                buff[20];
    int                 Count;

    for ( ; ; ) {
        while (isspace(UCHAR(*yyInput))) {
            yyInput++;
	}

        if (isdigit(UCHAR(c = *yyInput))) { /* INTL: digit */
	    /* convert the string into a number; count the number of digits */
	    Count = 0;
            for (yylval.Number = 0;
		    isdigit(UCHAR(c = *yyInput++)); ) { /* INTL: digit */
                yylval.Number = 10 * yylval.Number + c - '0';
		Count++;
	    }
            yyInput--;
	    /* A number with 6 or more digits is considered an ISO 8601 base */
	    if (Count >= 6) {
		return tISOBASE;
	    } else {
		return tUNUMBER;
	    }
        }
        if (!(c & 0x80) && isalpha(UCHAR(c))) {	/* INTL: ISO only. */
            for (p = buff; isalpha(UCHAR(c = *yyInput++)) /* INTL: ISO only. */
		     || c == '.'; ) {
                if (p < &buff[sizeof buff - 1]) {
                    *p++ = c;
		}
	    }
            *p = '\0';
            yyInput--;
            return LookupWord(buff);
        }
        if (c != '(') {
            return *yyInput++;
	}
        Count = 0;
        do {
            c = *yyInput++;
            if (c == '\0') {
                return c;
	    } else if (c == '(') {
                Count++;
	    } else if (c == ')') {
                Count--;
	    }
        } while (Count > 0);
    }
}

/*
 * Specify zone is of -50000 to force GMT.  (This allows BST to work).
 */

int
TclGetDate(p, now, zone, timePtr)
    char *p;
    unsigned long now;
    long zone;
    unsigned long *timePtr;
{
    struct tm *tm;
    time_t Start;
    time_t Time;
    time_t tod;
    int thisyear;

    yyInput = p;
    /* now has to be cast to a time_t for 64bit compliance */
    Start = now;
    tm = TclpGetDate(&Start, (zone == -50000));
    thisyear = tm->tm_year + TM_YEAR_BASE;
    yyYear = thisyear;
    yyMonth = tm->tm_mon + 1;
    yyDay = tm->tm_mday;
    yyTimezone = zone;
    if (zone == -50000) {
        yyDSTmode = DSToff;  /* assume GMT */
        yyTimezone = 0;
    } else {
        yyDSTmode = DSTmaybe;
    }
    yyHour = 0;
    yyMinutes = 0;
    yySeconds = 0;
    yyMeridian = MER24;
    yyRelSeconds = 0;
    yyRelMonth = 0;
    yyRelDay = 0;
    yyRelPointer = NULL;

    yyHaveDate = 0;
    yyHaveDay = 0;
    yyHaveOrdinalMonth = 0;
    yyHaveRel = 0;
    yyHaveTime = 0;
    yyHaveZone = 0;

    if (yyparse() || yyHaveTime > 1 || yyHaveZone > 1 || yyHaveDate > 1 ||
	    yyHaveDay > 1 || yyHaveOrdinalMonth > 1) {
        return -1;
    }
    
    if (yyHaveDate || yyHaveTime || yyHaveDay) {
	if (yyYear < 0) {
	    yyYear = -yyYear;
	}
	/*
	 * The following line handles years that are specified using
	 * only two digits.  The line of code below implements a policy
	 * defined by the X/Open workgroup on the millinium rollover.
	 * Note: some of those dates may not actually be valid on some
	 * platforms.  The POSIX standard startes that the dates 70-99
	 * shall refer to 1970-1999 and 00-38 shall refer to 2000-2038.
	 * This later definition should work on all platforms.
	 */

	if (yyYear < 100) {
	    if (yyYear >= 69) {
		yyYear += 1900;
	    } else {
		yyYear += 2000;
	    }
	}
	if (Convert(yyMonth, yyDay, yyYear, yyHour, yyMinutes, yySeconds,
		yyMeridian, yyDSTmode, &Start) < 0) {
            return -1;
	}
    } else {
        Start = now;
        if (!yyHaveRel) {
            Start -= ((tm->tm_hour * 60L * 60L) +
		    tm->tm_min * 60L) +	tm->tm_sec;
	}
    }

    Start += yyRelSeconds;
    if (RelativeMonth(Start, yyRelMonth, &Time) < 0) {
        return -1;
    }
    Start += Time;

    if (RelativeDay(Start, yyRelDay, &Time) < 0) {
	return -1;
    }
    Start += Time;
    
    if (yyHaveDay && !yyHaveDate) {
        tod = NamedDay(Start, yyDayOrdinal, yyDayNumber);
        Start += tod;
    }

    if (yyHaveOrdinalMonth) {
	tod = NamedMonth(Start, yyMonthOrdinal, yyMonth);
	Start += tod;
    }
    
    *timePtr = Start;
    return 0;
}


