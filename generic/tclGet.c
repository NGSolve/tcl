/*
 * tclGet.c --
 *
 *	This file contains functions to convert strings into other forms, like
 *	integers or floating-point numbers or booleans, doing syntax checking
 *	along the way.
 *
 * Copyright (c) 1990-1993 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclGet.c,v 1.17.8.2 2007/11/13 13:07:42 dgp Exp $
 */

#include "tclInt.h"

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetInt --
 *
 *	Given a string, produce the corresponding integer value.
 *
 * Results:
 *	The return value is normally TCL_OK; in this case *intPtr will be set
 *	to the integer value equivalent to src.  If src is improperly formed
 *	then TCL_ERROR is returned and an error message will be left in the
 *	interp's result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetInt(
    Tcl_Interp *interp,		/* Interpreter to use for error reporting. */
    CONST char *src,		/* String containing a (possibly signed)
				 * integer in a form acceptable to strtoul. */
    int *intPtr)		/* Place to store converted result. */
{
    Tcl_Obj obj;
    int code;

    obj.refCount = 1;
    obj.bytes = (char *) src;
    obj.length = strlen(src);
    obj.typePtr = NULL;

    code = Tcl_GetIntFromObj(interp, &obj, intPtr);
    if (obj.refCount > 1) {
	Tcl_Panic("invalid sharing of Tcl_Obj on C stack");
    }
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetLong --
 *
 *	Given a string, produce the corresponding long integer value. This
 *	routine is a version of Tcl_GetInt but returns a "long" instead of an
 *	"int" (a difference that matters on 64-bit architectures).
 *
 * Results:
 *	The return value is normally TCL_OK; in this case *longPtr will be set
 *	to the long integer value equivalent to src. If src is improperly
 *	formed then TCL_ERROR is returned and an error message will be left in
 *	the interp's result if interp is non-NULL.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGetLong(
    Tcl_Interp *interp,		/* Interpreter used for error reporting if not
				 * NULL. */
    CONST char *src,		/* String containing a (possibly signed) long
				 * integer in a form acceptable to strtoul. */
    long *longPtr)		/* Place to store converted long result. */
{
    Tcl_Obj obj;
    int code;

    obj.refCount = 1;
    obj.bytes = (char *) src;
    obj.length = strlen(src);
    obj.typePtr = NULL;

    code = Tcl_GetLongFromObj(interp, &obj, longPtr);
    if (obj.refCount > 1) {
	Tcl_Panic("invalid sharing of Tcl_Obj on C stack");
    }
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetDouble --
 *
 *	Given a string, produce the corresponding double-precision
 *	floating-point value.
 *
 * Results:
 *	The return value is normally TCL_OK; in this case *doublePtr will be
 *	set to the double-precision value equivalent to src. If src is
 *	improperly formed then TCL_ERROR is returned and an error message will
 *	be left in the interp's result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetDouble(
    Tcl_Interp *interp,		/* Interpreter used for error reporting. */
    CONST char *src,		/* String containing a floating-point number
				 * in a form acceptable to strtod. */
    double *doublePtr)		/* Place to store converted result. */
{
    Tcl_Obj obj;
    int code;

    obj.refCount = 1;
    obj.bytes = (char *) src;
    obj.length = strlen(src);
    obj.typePtr = NULL;

    code = Tcl_GetDoubleFromObj(interp, &obj, doublePtr);
    if (obj.refCount > 1) {
	Tcl_Panic("invalid sharing of Tcl_Obj on C stack");
    }
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetBoolean --
 *
 *	Given a string, return a 0/1 boolean value corresponding to the
 *	string.
 *
 * Results:
 *	The return value is normally TCL_OK; in this case *boolPtr will be set
 *	to the 0/1 value equivalent to src. If src is improperly formed then
 *	TCL_ERROR is returned and an error message will be left in the
 *	interp's result.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetBoolean(
    Tcl_Interp *interp,		/* Interpreter used for error reporting. */
    CONST char *src,		/* String containing a boolean number
				 * specified either as 1/0 or true/false or
				 * yes/no. */
    int *boolPtr)		/* Place to store converted result, which will
				 * be 0 or 1. */
{
    Tcl_Obj obj;
    int code;

    obj.refCount = 1;
    obj.bytes = (char *) src;
    obj.length = strlen(src);
    obj.typePtr = NULL;

    code = Tcl_ConvertToType(interp, &obj, &tclBooleanType);
    if (obj.refCount > 1) {
	Tcl_Panic("invalid sharing of Tcl_Obj on C stack");
    }
    if (code == TCL_OK) {
	*boolPtr = obj.internalRep.longValue;
    }
    return code;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
