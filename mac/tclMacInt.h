/*
 * tclMacInt.h --
 *
 *	Declarations of Macintosh specific shared variables and procedures.
 *
 * Copyright (c) 1996-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclMacInt.h,v 1.2.4.1 1999/03/04 00:59:55 stanton Exp $
 */

#ifndef _TCLMACINT
#define _TCLMACINT

#ifndef _TCL
#   include "tcl.h"
#endif
#ifndef _TCLMAC
#   include "tclMac.h"
#endif

#include <Events.h>
#include <Files.h>

#pragma export on

/*
 * Defines to control stack behavior
 */

#define TCL_MAC_68K_STACK_GROWTH (256*1024)
#define TCL_MAC_STACK_THRESHOLD 16384

/*
 * This flag is passed to TclMacRegisterResourceFork
 * by a file (usually a library) whose resource fork
 * should not be closed by the resource command.
 */
 
#define TCL_RESOURCE_DONT_CLOSE  2

/*
 * Typedefs used by Macintosh parts of Tcl.
 */
typedef pascal void (*ExitToShellProcPtr)(void);

#include "tclIntPlatDecls.h"

#pragma export reset

#endif /* _TCLMACINT */
