/* 
 * tclGetDate.y --
 *
 *	Contains yacc grammar for parsing date and time strings.
 *	The output of this file should be the file tclDate.c which
 *	is used directly in the Tcl sources.
 *
 * Copyright (c) 1992-1995 Karl Lehenbauer and Mark Diekhans.
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclGetDate.y,v 1.28 2005/11/02 14:51:04 dkf Exp $
 */

%{
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
 */

#include "tclInt.h"

/*
 * Bison generates several labels that happen to be unused. MS Visual
 * C++ doesn't like that, and complains.  Tell it to shut up.
 */

#ifdef _MSC_VER
#pragma warning( disable : 4102 )
#endif /* _MSC_VER */

/*
 * yyparse will accept a 'struct DateInfo' as its parameter;
 * that's where the parsed fields will be returned.
 */

typedef struct DateInfo {

    time_t   dateYear;
    time_t   dateMonth;
    time_t   dateDay;
    int      dateHaveDate;

    time_t   dateHour;
    time_t   dateMinutes;
    time_t   dateSeconds;
    int      dateMeridian;
    int      dateHaveTime;

    time_t   dateTimezone;
    int      dateDSTmode;
    int      dateHaveZone;

    time_t   dateRelMonth;
    time_t   dateRelDay;
    time_t   dateRelSeconds;
    int      dateHaveRel;

    time_t   dateMonthOrdinal;
    int      dateHaveOrdinalMonth;

    time_t   dateDayOrdinal;
    time_t   dateDayNumber;
    int      dateHaveDay;

    char     *dateInput;
    time_t   *dateRelPointer;

    int	     dateDigitCount;

} DateInfo;

#define YYPARSE_PARAM info
#define YYLEX_PARAM info

#define yyDSTmode (((DateInfo*)info)->dateDSTmode)
#define yyDayOrdinal (((DateInfo*)info)->dateDayOrdinal)
#define yyDayNumber (((DateInfo*)info)->dateDayNumber)
#define yyMonthOrdinal (((DateInfo*)info)->dateMonthOrdinal)
#define yyHaveDate (((DateInfo*)info)->dateHaveDate)
#define yyHaveDay (((DateInfo*)info)->dateHaveDay)
#define yyHaveOrdinalMonth (((DateInfo*)info)->dateHaveOrdinalMonth)
#define yyHaveRel (((DateInfo*)info)->dateHaveRel)
#define yyHaveTime (((DateInfo*)info)->dateHaveTime)
#define yyHaveZone (((DateInfo*)info)->dateHaveZone)
#define yyTimezone (((DateInfo*)info)->dateTimezone)
#define yyDay (((DateInfo*)info)->dateDay)
#define yyMonth (((DateInfo*)info)->dateMonth)
#define yyYear (((DateInfo*)info)->dateYear)
#define yyHour (((DateInfo*)info)->dateHour)
#define yyMinutes (((DateInfo*)info)->dateMinutes)
#define yySeconds (((DateInfo*)info)->dateSeconds)
#define yyMeridian (((DateInfo*)info)->dateMeridian)
#define yyRelMonth (((DateInfo*)info)->dateRelMonth)
#define yyRelDay (((DateInfo*)info)->dateRelDay)
#define yyRelSeconds (((DateInfo*)info)->dateRelSeconds)
#define yyRelPointer (((DateInfo*)info)->dateRelPointer)
#define yyInput (((DateInfo*)info)->dateInput)
#define yyDigitCount (((DateInfo*)info)->dateDigitCount)

#define EPOCH           1970
#define START_OF_TIME   1902
#define END_OF_TIME     2037

/*
 * The offset of tm_year of struct tm returned by localtime, gmtime, etc.
 * Posix requires 1900.
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
 * Prototypes of internal functions.
 */
static void	TclDateerror(char *s);
static time_t	ToSeconds(time_t Hours, time_t Minutes,
		    time_t Seconds, MERIDIAN Meridian);
static int	LookupWord(char *buff);
static int	TclDatelex(void* info);


%}

%union {
    time_t              Number;
    enum _MERIDIAN      Meridian;
}

%token  tAGO tDAY tDAYZONE tID tMERIDIAN tMINUTE_UNIT tMONTH tMONTH_UNIT
%token  tSTARDATE tSEC_UNIT tSNUMBER tUNUMBER tZONE tEPOCH tDST tISOBASE
%token  tDAY_UNIT tNEXT

%type   <Number>        tDAY tDAYZONE tMINUTE_UNIT tMONTH tMONTH_UNIT tDST
%type   <Number>        tSEC_UNIT tSNUMBER tUNUMBER tZONE tISOBASE tDAY_UNIT
%type   <Number>        unit sign tNEXT tSTARDATE
%type   <Meridian>      tMERIDIAN o_merid

%%

spec    : /* NULL */
        | spec item
        ;

item    : time {
            yyHaveTime++;
        }
        | zone {
            yyHaveZone++;
        }
        | date {
            yyHaveDate++;
        }
        | ordMonth {
            yyHaveOrdinalMonth++;
        }
        | day {
            yyHaveDay++;
        }
        | relspec {
            yyHaveRel++;
        }
        | iso {
	    yyHaveTime++;
	    yyHaveDate++;
	}
        | trek {
	    yyHaveTime++;
	    yyHaveDate++;
	    yyHaveRel++;
        }
        | number
        ;

time    : tUNUMBER tMERIDIAN {
            yyHour = $1;
            yyMinutes = 0;
            yySeconds = 0;
            yyMeridian = $2;
        }
        | tUNUMBER ':' tUNUMBER o_merid {
            yyHour = $1;
            yyMinutes = $3;
            yySeconds = 0;
            yyMeridian = $4;
        }
        | tUNUMBER ':' tUNUMBER '-' tUNUMBER {
            yyHour = $1;
            yyMinutes = $3;
            yyMeridian = MER24;
            yyDSTmode = DSToff;
            yyTimezone = ($5 % 100 + ($5 / 100) * 60);
	    ++yyHaveZone;
        }
        | tUNUMBER ':' tUNUMBER ':' tUNUMBER o_merid {
            yyHour = $1;
            yyMinutes = $3;
            yySeconds = $5;
            yyMeridian = $6;
        }
        | tUNUMBER ':' tUNUMBER ':' tUNUMBER '-' tUNUMBER {
            yyHour = $1;
            yyMinutes = $3;
            yySeconds = $5;
            yyMeridian = MER24;
            yyDSTmode = DSToff;
            yyTimezone = ($7 % 100 + ($7 / 100) * 60);
	    ++yyHaveZone;
        }
        ;

zone    : tZONE tDST {
            yyTimezone = $1;
            yyDSTmode = DSTon;
        }
        | tZONE {
            yyTimezone = $1;
            yyDSTmode = DSToff;
        }
        | tDAYZONE {
            yyTimezone = $1;
            yyDSTmode = DSTon;
        }
        ;

day     : tDAY {
            yyDayOrdinal = 1;
            yyDayNumber = $1;
        }
        | tDAY ',' {
            yyDayOrdinal = 1;
            yyDayNumber = $1;
        }
        | tUNUMBER tDAY {
            yyDayOrdinal = $1;
            yyDayNumber = $2;
        }
        | sign tUNUMBER tDAY {
            yyDayOrdinal = $1 * $2;
            yyDayNumber = $3;
        }
        | tNEXT tDAY {
            yyDayOrdinal = 2;
            yyDayNumber = $2;
        }
        ;

date    : tUNUMBER '/' tUNUMBER {
            yyMonth = $1;
            yyDay = $3;
        }
        | tUNUMBER '/' tUNUMBER '/' tUNUMBER {
            yyMonth = $1;
            yyDay = $3;
            yyYear = $5;
        }
        | tISOBASE {
	    yyYear = $1 / 10000;
	    yyMonth = ($1 % 10000)/100;
	    yyDay = $1 % 100;
	}
        | tUNUMBER '-' tMONTH '-' tUNUMBER {
	    yyDay = $1;
	    yyMonth = $3;
	    yyYear = $5;
	}
        | tUNUMBER '-' tUNUMBER '-' tUNUMBER {
            yyMonth = $3;
            yyDay = $5;
            yyYear = $1;
        }
        | tMONTH tUNUMBER {
            yyMonth = $1;
            yyDay = $2;
        }
        | tMONTH tUNUMBER ',' tUNUMBER {
            yyMonth = $1;
            yyDay = $2;
            yyYear = $4;
        }
        | tUNUMBER tMONTH {
            yyMonth = $2;
            yyDay = $1;
        }
        | tEPOCH {
	    yyMonth = 1;
	    yyDay = 1;
	    yyYear = EPOCH;
	}
        | tUNUMBER tMONTH tUNUMBER {
            yyMonth = $2;
            yyDay = $1;
            yyYear = $3;
        }
        ;

ordMonth: tNEXT tMONTH {
	    yyMonthOrdinal = 1;
	    yyMonth = $2;
	}
        | tNEXT tUNUMBER tMONTH {
	    yyMonthOrdinal = $2;
	    yyMonth = $3;
	}
        ;

iso     : tISOBASE tZONE tISOBASE {
            if ($2 != HOUR(- 7)) YYABORT;
	    yyYear = $1 / 10000;
	    yyMonth = ($1 % 10000)/100;
	    yyDay = $1 % 100;
	    yyHour = $3 / 10000;
	    yyMinutes = ($3 % 10000)/100;
	    yySeconds = $3 % 100;
        }
        | tISOBASE tZONE tUNUMBER ':' tUNUMBER ':' tUNUMBER {
            if ($2 != HOUR(- 7)) YYABORT;
	    yyYear = $1 / 10000;
	    yyMonth = ($1 % 10000)/100;
	    yyDay = $1 % 100;
	    yyHour = $3;
	    yyMinutes = $5;
	    yySeconds = $7;
        }
	| tISOBASE tISOBASE {
	    yyYear = $1 / 10000;
	    yyMonth = ($1 % 10000)/100;
	    yyDay = $1 % 100;
	    yyHour = $2 / 10000;
	    yyMinutes = ($2 % 10000)/100;
	    yySeconds = $2 % 100;
        }
        ;

trek    : tSTARDATE tUNUMBER '.' tUNUMBER {
            /*
	     * Offset computed year by -377 so that the returned years will
	     * be in a range accessible with a 32 bit clock seconds value
	     */
            yyYear = $2/1000 + 2323 - 377;
            yyDay  = 1;
	    yyMonth = 1;
	    yyRelDay += (($2%1000)*(365 + IsLeapYear(yyYear)))/1000;
	    yyRelSeconds += $4 * 144 * 60;
        }
        ;

relspec : relunits tAGO {
	    yyRelSeconds *= -1;
	    yyRelMonth *= -1;
	    yyRelDay *= -1;
	}
	| relunits
	;
relunits : sign tUNUMBER unit  { *yyRelPointer += $1 * $2 * $3; }
        | tUNUMBER unit        { *yyRelPointer += $1 * $2; }
        | tNEXT unit           { *yyRelPointer += $2; }
        | tNEXT tUNUMBER unit  { *yyRelPointer += $2 * $3; }
        | unit                 { *yyRelPointer += $1; }
        ;
sign    : '-'            { $$ = -1; }
        | '+'            { $$ =  1; }
        ;
unit    : tSEC_UNIT      { $$ = $1; yyRelPointer = &yyRelSeconds; }
        | tDAY_UNIT      { $$ = $1; yyRelPointer = &yyRelDay; }
        | tMONTH_UNIT    { $$ = $1; yyRelPointer = &yyRelMonth; }
        ;

number  : tUNUMBER
    {
	if (yyHaveTime && yyHaveDate && !yyHaveRel) {
	    yyYear = $1;
	} else {
	    yyHaveTime++;
	    if (yyDigitCount <= 2) {
		yyHour = $1;
		yyMinutes = 0;
	    } else {
		yyHour = $1 / 100;
		yyMinutes = $1 % 100;
	    }
	    yySeconds = 0;
	    yyMeridian = MER24;
	}
    }
;

o_merid : /* NULL */ {
            $$ = MER24;
        }
        | tMERIDIAN {
            $$ = $1;
        }
        ;

%%

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
TclDatelex( void* info )
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
	    yyDigitCount = Count;
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

int
TclClockOldscanObjCmd( clientData, interp, objc, objv )
     ClientData clientData;	/* Unused */
     Tcl_Interp* interp;	/* Tcl interpreter */
     int objc;			/* Count of paraneters */
     Tcl_Obj *CONST *objv;	/* Parameters */
{

    Tcl_Obj* result;
    Tcl_Obj* resultElement;
    int yr, mo, da;
    DateInfo dateInfo;
    void* info = (void*) &dateInfo;

    if ( objc != 5 ) {
	Tcl_WrongNumArgs( interp, 1, objv, 
			  "stringToParse baseYear baseMonth baseDay" );
	return TCL_ERROR;
    }

    yyInput = Tcl_GetString( objv[1] );

    yyHaveDate = 0;
    if ( Tcl_GetIntFromObj( interp, objv[2], &yr ) != TCL_OK
	 || Tcl_GetIntFromObj( interp, objv[3], &mo ) != TCL_OK
	 || Tcl_GetIntFromObj( interp, objv[4], &da ) != TCL_OK ) {
	return TCL_ERROR;
    }
    yyYear = yr; yyMonth = mo; yyDay = da;

    yyHaveTime = 0;
    yyHour = 0; yyMinutes = 0; yySeconds = 0; yyMeridian = MER24;

    yyHaveZone = 0;
    yyTimezone = 0; yyDSTmode = DSTmaybe;

    yyHaveOrdinalMonth = 0;
    yyMonthOrdinal = 0;

    yyHaveDay = 0;
    yyDayOrdinal = 0; yyDayNumber = 0;

    yyHaveRel = 0;
    yyRelMonth = 0; yyRelDay = 0; yyRelSeconds = 0; yyRelPointer = NULL;

    if ( yyparse( info ) ) {
	Tcl_SetObjResult( interp, Tcl_NewStringObj( "syntax error", -1 ) );
	return TCL_ERROR;
    }

    if ( yyHaveDate > 1 ) {
	Tcl_SetObjResult
	    ( interp, 
	      Tcl_NewStringObj( "more than one date in string", -1 ) );
	return TCL_ERROR;
    }
    if ( yyHaveTime > 1 ) {
	Tcl_SetObjResult
	    ( interp, 
	      Tcl_NewStringObj( "more than one time of day in string", -1 ) );
	return TCL_ERROR;
    }
    if ( yyHaveZone > 1 ) {
	Tcl_SetObjResult
	    ( interp, 
	      Tcl_NewStringObj( "more than one time zone in string", -1 ) );
	return TCL_ERROR;
    }
    if ( yyHaveDay > 1 ) {
	Tcl_SetObjResult
	    ( interp, 
	      Tcl_NewStringObj( "more than one weekday in string", -1 ) );
	return TCL_ERROR;
    }
    if ( yyHaveOrdinalMonth > 1 ) {
	Tcl_SetObjResult
	    ( interp, 
	      Tcl_NewStringObj( "more than one ordinal month in string", -1 ) );
	return TCL_ERROR;
    }
	
    result = Tcl_NewObj();
    resultElement = Tcl_NewObj();
    if ( yyHaveDate ) {
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyYear ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyMonth ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyDay ) );
    }
    Tcl_ListObjAppendElement( interp, result, resultElement );

    if ( yyHaveTime ) {
	Tcl_ListObjAppendElement( interp, result,
				  Tcl_NewIntObj( ToSeconds( yyHour,
							    yyMinutes,
							    yySeconds,
							    yyMeridian ) ) );
    } else {
	Tcl_ListObjAppendElement( interp, result, Tcl_NewObj() );
    }

    resultElement = Tcl_NewObj();
    if ( yyHaveZone ) {
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( -yyTimezone ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( 1-yyDSTmode ) );
    }
    Tcl_ListObjAppendElement( interp, result, resultElement );

    resultElement = Tcl_NewObj();
    if ( yyHaveRel ) {
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyRelMonth ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyRelDay ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyRelSeconds ) );
    }
    Tcl_ListObjAppendElement( interp, result, resultElement );

    resultElement = Tcl_NewObj();
    if ( yyHaveDay && !yyHaveDate ) {
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyDayOrdinal ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyDayNumber ) );
    }
    Tcl_ListObjAppendElement( interp, result, resultElement );

    resultElement = Tcl_NewObj();
    if ( yyHaveOrdinalMonth ) {
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyMonthOrdinal ) );
	Tcl_ListObjAppendElement( interp, resultElement,
				  Tcl_NewIntObj( yyMonth ) );
    }
    Tcl_ListObjAppendElement( interp, result, resultElement );
	
    Tcl_SetObjResult( interp, result );
    return TCL_OK;
}
