/* 
 * gettod.c --
 *
 *	This file provides the gettimeofday function on systems
 *	that only have the System V ftime function.
 *
 * Copyright (c) 1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: gettod.c,v 1.2.42.2 2007/04/16 18:35:50 dgp Exp $
 */

#include "tclPort.h"
#include <sys/timeb.h>

#undef timezone

int
gettimeofday(
    struct timeval *tp,
    struct timezone *tz)
{
    struct timeb t;

    ftime(&t);
    tp->tv_sec = t.time;
    tp->tv_usec = t. millitm * 1000;
    return 0;
}

