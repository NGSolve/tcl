/* 
 * tclWinMtherr.c --
 *
 *	This function provides a default implementation of the
 *	_matherr function for Borland C++.
 *
 * Copyright (c) 1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclWinMtherr.c,v 1.3.26.1 2001/10/22 14:53:22 dkf Exp $
 */

#include "tclWinInt.h"
#include <math.h>


/*
 *----------------------------------------------------------------------
 *
 * _matherr --
 *
 *	This procedure is invoked by Borland C++ when certain
 *	errors occur in mathematical functions.  This procedure
 *	replaces the default implementation which generates pop-up
 *	warnings.
 *
 * Results:
 *	Returns 1 to indicate that we've handled the error
 *	locally.
 *
 * Side effects:
 *	Sets errno based on what's in xPtr.
 *
 *----------------------------------------------------------------------
 */

int
_matherr(xPtr)
    struct exception *xPtr;	/* Describes error that occurred. */
{
    if (!TclMathInProgress()) {
	return 0;
    }
    if ((xPtr->type == DOMAIN)
#ifdef __BORLANDC__
	    || (xPtr->type == TLOSS)
#endif
	    || (xPtr->type == SING)) {
	errno = EDOM;
    } else {
	errno = ERANGE;
    }
    return 1;
}
