/* 
 * tclPlatStubs.c --
 *
 *	This file contains the wrapper functions for the platform independent
 *	unsupported Tcl API.
 *
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 * All rights reserved.
 *
 * RCS: @(#) $Id: tclPlatStubs.c,v 1.2.2.2 1999/03/10 06:41:52 stanton Exp $
 */

#include "tcl.h"

/*
 * WARNING: This file is automatically generated by the tools/genStubs.tcl
 * script.  Any modifications to the function declarations below should be made
 * in the generic/tclInt.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported stub functions:
 */

#ifdef MAC_TCL
/* Slot 0 */
void
Tcl_MacSetEventProc(procPtr)
    Tcl_MacConvertEventPtr procPtr;
{
    (tclPlatStubsPtr->tcl_MacSetEventProc)(procPtr);
}

/* Slot 1 */
char *
Tcl_MacConvertTextResource(resource)
    Handle resource;
{
    return (tclPlatStubsPtr->tcl_MacConvertTextResource)(resource);
}

/* Slot 2 */
int
Tcl_MacEvalResource(interp, resourceName, resourceNumber, fileName)
    Tcl_Interp * interp;
    char * resourceName;
    int resourceNumber;
    char * fileName;
{
    return (tclPlatStubsPtr->tcl_MacEvalResource)(interp, resourceName, resourceNumber, fileName);
}

/* Slot 3 */
Handle
Tcl_MacFindResource(interp, resourceType, resourceName, resourceNumber, resFileRef, releaseIt)
    Tcl_Interp * interp;
    long resourceType;
    char * resourceName;
    int resourceNumber;
    char * resFileRef;
    int * releaseIt;
{
    return (tclPlatStubsPtr->tcl_MacFindResource)(interp, resourceType, resourceName, resourceNumber, resFileRef, releaseIt);
}

/* Slot 4 */
int
Tcl_GetOSTypeFromObj(interp, objPtr, osTypePtr)
    Tcl_Interp * interp;
    Tcl_Obj * objPtr;
    OSType * osTypePtr;
{
    return (tclPlatStubsPtr->tcl_GetOSTypeFromObj)(interp, objPtr, osTypePtr);
}

/* Slot 5 */
void
Tcl_SetOSTypeObj(objPtr, osType)
    Tcl_Obj * objPtr;
    OSType osType;
{
    (tclPlatStubsPtr->tcl_SetOSTypeObj)(objPtr, osType);
}

/* Slot 6 */
Tcl_Obj *
Tcl_NewOSTypeObj(osType)
    OSType osType;
{
    return (tclPlatStubsPtr->tcl_NewOSTypeObj)(osType);
}

/* Slot 7 */
int
strncasecmp(s1, s2, n)
    CONST char * s1;
    CONST char * s2;
    size_t n;
{
    return (tclPlatStubsPtr->strncasecmp)(s1, s2, n);
}

/* Slot 8 */
int
strcasecmp(s1, s2)
    CONST char * s1;
    CONST char * s2;
{
    return (tclPlatStubsPtr->strcasecmp)(s1, s2);
}

#endif /* MAC_TCL */

/* !END!: Do not edit above this line. */
