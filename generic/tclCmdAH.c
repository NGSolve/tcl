/*
 * tclCmdAH.c --
 *
 *	This file contains the top-level command routines for most of the Tcl
 *	built-in commands whose names begin with the letters A to H.
 *
 * Copyright (c) 1987-1993 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclCmdAH.c,v 1.57.2.7 2005/08/29 18:38:45 dgp Exp $
 */

#include "tclInt.h"
#include <locale.h>

/*
 * Prototypes for local procedures defined in this file:
 */

static int		CheckAccess(Tcl_Interp *interp, Tcl_Obj *pathPtr,
			    int mode);
static int		GetStatBuf(Tcl_Interp *interp, Tcl_Obj *pathPtr,
			    Tcl_FSStatProc *statProc, Tcl_StatBuf *statPtr);
static char *		GetTypeFromMode(int mode);
static int		StoreStatData(Tcl_Interp *interp, Tcl_Obj *varName,
			    Tcl_StatBuf *statPtr);

/*
 *----------------------------------------------------------------------
 *
 * Tcl_BreakObjCmd --
 *
 *	This procedure is invoked to process the "break" Tcl command. See the
 *	user documentation for details on what it does.
 *
 *	With the bytecode compiler, this procedure is only called when a
 *	command name is computed at runtime, and is "break" or the name to
 *	which "break" was renamed: e.g., "set z break; $z"
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_BreakObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, NULL);
	return TCL_ERROR;
    }
    return TCL_BREAK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CaseObjCmd --
 *
 *	This procedure is invoked to process the "case" Tcl command. See the
 *	user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_CaseObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    register int i;
    int body, result, caseObjc;
    char *stringPtr, *arg;
    Tcl_Obj *CONST *caseObjv;
    Tcl_Obj *armPtr;

    if (objc < 3) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"string ?in? patList body ... ?default body?");
	return TCL_ERROR;
    }

    stringPtr = TclGetString(objv[1]);
    body = -1;

    arg = TclGetString(objv[2]);
    if (strcmp(arg, "in") == 0) {
	i = 3;
    } else {
	i = 2;
    }
    caseObjc = objc - i;
    caseObjv = objv + i;

    /*
     * If all of the pattern/command pairs are lumped into a single argument,
     * split them out again.
     */

    if (caseObjc == 1) {
	Tcl_Obj **newObjv;

	Tcl_ListObjGetElements(interp, caseObjv[0], &caseObjc, &newObjv);
	caseObjv = newObjv;
    }

    for (i = 0;  i < caseObjc;  i += 2) {
	int patObjc, j;
	CONST char **patObjv;
	char *pat;
	unsigned char *p;

	if (i == (caseObjc - 1)) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, "extra case pattern with no body", NULL);
	    return TCL_ERROR;
	}

	/*
	 * Check for special case of single pattern (no list) with no
	 * backslash sequences.
	 */

	pat = TclGetString(caseObjv[i]);
	for (p = (unsigned char *) pat; *p != '\0'; p++) {
	    if (isspace(*p) || (*p == '\\')) {	/* INTL: ISO space, UCHAR */
		break;
	    }
	}
	if (*p == '\0') {
	    if ((*pat == 'd') && (strcmp(pat, "default") == 0)) {
		body = i + 1;
	    }
	    if (Tcl_StringMatch(stringPtr, pat)) {
		body = i + 1;
		goto match;
	    }
	    continue;
	}

	/*
	 * Break up pattern lists, then check each of the patterns in the
	 * list.
	 */

	result = Tcl_SplitList(interp, pat, &patObjc, &patObjv);
	if (result != TCL_OK) {
	    return result;
	}
	for (j = 0; j < patObjc; j++) {
	    if (Tcl_StringMatch(stringPtr, patObjv[j])) {
		body = i + 1;
		break;
	    }
	}
	ckfree((char *) patObjv);
	if (j < patObjc) {
	    break;
	}
    }

  match:
    if (body != -1) {
	armPtr = caseObjv[body - 1];
	result = Tcl_EvalObjEx(interp, caseObjv[body], 0);
	if (result == TCL_ERROR) {
	    char msg[100 + TCL_INTEGER_SPACE];

	    arg = TclGetString(armPtr);
	    sprintf(msg, "\n    (\"%.50s\" arm line %d)", arg,
		    interp->errorLine);
	    Tcl_AddObjErrorInfo(interp, msg, -1);
	}
	return result;
    }

    /*
     * Nothing matched: return nothing.
     */

    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CatchObjCmd --
 *
 *	This object-based procedure is invoked to process the "catch" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_CatchObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    Tcl_Obj *varNamePtr = NULL;
    Tcl_Obj *optionVarNamePtr = NULL;
    int result;

    if ((objc < 2) || (objc > 4)) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"script ?resultVarName? ?optionVarName?");
	return TCL_ERROR;
    }

    if (objc >= 3) {
	varNamePtr = objv[2];
    }
    if (objc == 4) {
	optionVarNamePtr = objv[3];
    }

    result = Tcl_EvalObjEx(interp, objv[1], 0);

    /*
     * We disable catch in interpreters where the limit has been exceeded.
     */

    if (Tcl_LimitExceeded(interp)) {
	char msg[32 + TCL_INTEGER_SPACE];

	sprintf(msg, "\n    (\"catch\" body line %d)", interp->errorLine);
	Tcl_AddErrorInfo(interp, msg);
	return TCL_ERROR;
    }

    if (objc >= 3) {
	if (NULL == Tcl_ObjSetVar2(interp, varNamePtr, NULL,
		Tcl_GetObjResult(interp), 0)) {
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp,
		    "couldn't save command result in variable", NULL);
	    return TCL_ERROR;
	}
    }
    if (objc == 4) {
	Tcl_Obj *options = Tcl_GetReturnOptions(interp, result);
	if (NULL == Tcl_ObjSetVar2(interp, optionVarNamePtr, NULL,
		options, 0)) {
	    Tcl_DecrRefCount(options);
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp,
		    "couldn't save return options in variable", NULL);
	    return TCL_ERROR;
	}
    }

    Tcl_ResetResult(interp);
    Tcl_SetObjResult(interp, Tcl_NewIntObj(result));
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CdObjCmd --
 *
 *	This procedure is invoked to process the "cd" Tcl command. See the
 *	user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_CdObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    Tcl_Obj *dir;
    int result;

    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?dirName?");
	return TCL_ERROR;
    }

    if (objc == 2) {
	dir = objv[1];
    } else {
	dir = Tcl_NewStringObj("~",1);
	Tcl_IncrRefCount(dir);
    }
    if (Tcl_FSConvertToPathType(interp, dir) != TCL_OK) {
	result = TCL_ERROR;
    } else {
	result = Tcl_FSChdir(dir);
	if (result != TCL_OK) {
	    Tcl_AppendResult(interp, "couldn't change working directory to \"",
		    TclGetString(dir), "\": ", Tcl_PosixError(interp), NULL);
	    result = TCL_ERROR;
	}
    }
    if (objc != 2) {
	Tcl_DecrRefCount(dir);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ConcatObjCmd --
 *
 *	This object-based procedure is invoked to process the "concat" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ConcatObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    if (objc >= 2) {
	Tcl_SetObjResult(interp, Tcl_ConcatObj(objc-1, objv+1));
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ContinueObjCmd --
 *
 *	This procedure is invoked to process the "continue" Tcl command. See
 *	the user documentation for details on what it does.
 *
 *	With the bytecode compiler, this procedure is only called when a
 *	command name is computed at runtime, and is "continue" or the name to
 *	which "continue" was renamed: e.g., "set z continue; $z"
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ContinueObjCmd(dummy, interp, objc, objv)
    ClientData dummy;			/* Not used. */
    Tcl_Interp *interp;			/* Current interpreter. */
    int objc;				/* Number of arguments. */
    Tcl_Obj *CONST objv[];		/* Argument objects. */
{
    if (objc != 1) {
	Tcl_WrongNumArgs(interp, 1, objv, NULL);
	return TCL_ERROR;
    }
    return TCL_CONTINUE;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EncodingObjCmd --
 *
 *	This command manipulates encodings.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_EncodingObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int index;

    static CONST char *optionStrings[] = {
	"convertfrom", "convertto", "names", "system",
	NULL
    };
    enum options {
	ENC_CONVERTFROM, ENC_CONVERTTO, ENC_NAMES, ENC_SYSTEM
    };

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], optionStrings, "option", 0,
	    &index) != TCL_OK) {
	return TCL_ERROR;
    }

    switch ((enum options) index) {
    case ENC_CONVERTTO:
    case ENC_CONVERTFROM: {
	Tcl_Obj *data;
	Tcl_DString ds;
	Tcl_Encoding encoding;
	int length;
	char *stringPtr;

	if (objc == 3) {
	    encoding = Tcl_GetEncoding(interp, NULL);
	    data = objv[2];
	} else if (objc == 4) {
	    if (TclGetEncodingFromObj(interp, objv[2], &encoding) != TCL_OK) {
		return TCL_ERROR;
	    }
	    data = objv[3];
	} else {
	    Tcl_WrongNumArgs(interp, 2, objv, "?encoding? data");
	    return TCL_ERROR;
	}

	if ((enum options) index == ENC_CONVERTFROM) {
	    /*
	     * Treat the string as binary data.
	     */

	    stringPtr = (char *) Tcl_GetByteArrayFromObj(data, &length);
	    Tcl_ExternalToUtfDString(encoding, stringPtr, length, &ds);

	    /*
	     * Note that we cannot use Tcl_DStringResult here because it will
	     * truncate the string at the first null byte.
	     */

	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    Tcl_DStringValue(&ds), Tcl_DStringLength(&ds)));
	    Tcl_DStringFree(&ds);
	} else {
	    /*
	     * Store the result as binary data.
	     */

	    stringPtr = Tcl_GetStringFromObj(data, &length);
	    Tcl_UtfToExternalDString(encoding, stringPtr, length, &ds);
	    Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(
		    (unsigned char *) Tcl_DStringValue(&ds),
		    Tcl_DStringLength(&ds)));
	    Tcl_DStringFree(&ds);
	}

	Tcl_FreeEncoding(encoding);
	break;
    }
    case ENC_NAMES:
	if (objc > 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	Tcl_GetEncodingNames(interp);
	break;
    case ENC_SYSTEM:
	if (objc > 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?encoding?");
	    return TCL_ERROR;
	}
	if (objc == 2) {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(
		    Tcl_GetEncodingName(NULL), -1));
	} else {
	    return Tcl_SetSystemEncoding(interp, TclGetString(objv[2]));
	}
	break;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TclEncodingDirsObjCmd --
 *
 *	This command manipulates the encoding search path.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	Can set the encoding search path.
 *
 *----------------------------------------------------------------------
 */

int
TclEncodingDirsObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?dirList?");
	return TCL_ERROR;
    }
    if (objc == 1) {
	Tcl_SetObjResult(interp, TclGetEncodingSearchPath());
	return TCL_OK;
    }
    if (TclSetEncodingSearchPath(objv[1]) == TCL_ERROR) {
	Tcl_AppendResult(interp, "expected directory list but got \"",
		Tcl_GetString(objv[1]), "\"", NULL);
	return TCL_ERROR;
    }
    Tcl_SetObjResult(interp, objv[1]);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ErrorObjCmd --
 *
 *	This procedure is invoked to process the "error" Tcl command. See the
 *	user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ErrorObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    Tcl_Obj *options;

    if ((objc < 2) || (objc > 4)) {
	Tcl_WrongNumArgs(interp, 1, objv, "message ?errorInfo? ?errorCode?");
	return TCL_ERROR;
    }

    options = Tcl_NewStringObj("-code error -level 0", -1);

    if (objc >= 3) {		/* process the optional info argument */
	Tcl_ListObjAppendElement(NULL, options,
		Tcl_NewStringObj("-errorinfo", -1));
	Tcl_ListObjAppendElement(NULL, options, objv[2]);
    }

    if (objc == 4) {		/* process the optional code argument */
	Tcl_ListObjAppendElement(NULL, options,
		Tcl_NewStringObj("-errorcode", -1));
	Tcl_ListObjAppendElement(NULL, options, objv[3]);
    }

    Tcl_SetObjResult(interp, objv[1]);
    return Tcl_SetReturnOptions(interp, options);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_EvalObjCmd --
 *
 *	This object-based procedure is invoked to process the "eval" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_EvalObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int result;
    register Tcl_Obj *objPtr;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "arg ?arg ...?");
	return TCL_ERROR;
    }

    if (objc == 2) {
	result = Tcl_EvalObjEx(interp, objv[1], TCL_EVAL_DIRECT);
    } else {
	/*
	 * More than one argument: concatenate them together with spaces
	 * between, then evaluate the result. Tcl_EvalObjEx will delete the
	 * object when it decrements its refcount after eval'ing it.
	 */

	objPtr = Tcl_ConcatObj(objc-1, objv+1);
	result = Tcl_EvalObjEx(interp, objPtr, TCL_EVAL_DIRECT);
    }
    if (result == TCL_ERROR) {
	char msg[32 + TCL_INTEGER_SPACE];

	sprintf(msg, "\n    (\"eval\" body line %d)", interp->errorLine);
	Tcl_AddObjErrorInfo(interp, msg, -1);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExitObjCmd --
 *
 *	This procedure is invoked to process the "exit" Tcl command. See the
 *	user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ExitObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int value;

    if ((objc != 1) && (objc != 2)) {
	Tcl_WrongNumArgs(interp, 1, objv, "?returnCode?");
	return TCL_ERROR;
    }

    if (objc == 1) {
	value = 0;
    } else if (Tcl_GetIntFromObj(interp, objv[1], &value) != TCL_OK) {
	return TCL_ERROR;
    }
    Tcl_Exit(value);
    /*NOTREACHED*/
    return TCL_OK;			/* Better not ever reach this! */
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ExprObjCmd --
 *
 *	This object-based procedure is invoked to process the "expr" Tcl
 *	command. See the user documentation for details on what it does.
 *
 *	With the bytecode compiler, this procedure is called in two
 *	circumstances: 1) to execute expr commands that are too complicated or
 *	too unsafe to try compiling directly into an inline sequence of
 *	instructions, and 2) to execute commands where the command name is
 *	computed at runtime and is "expr" or the name to which "expr" was
 *	renamed (e.g., "set z expr; $z 2+3")
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ExprObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    register Tcl_Obj *objPtr;
    Tcl_Obj *resultPtr;
    int result;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "arg ?arg ...?");
	return TCL_ERROR;
    }

    objPtr = Tcl_ConcatObj(objc-1, objv+1);
    Tcl_IncrRefCount(objPtr);
    result = Tcl_ExprObj(interp, objPtr, &resultPtr);
    Tcl_DecrRefCount(objPtr);

    if (result == TCL_OK) {
	Tcl_SetObjResult(interp, resultPtr);
	Tcl_DecrRefCount(resultPtr);	/* done with the result object */
    }

    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FileObjCmd --
 *
 *	This procedure is invoked to process the "file" Tcl command. See the
 *	user documentation for details on what it does. PLEASE NOTE THAT THIS
 *	FAILS WITH FILENAMES AND PATHS WITH EMBEDDED NULLS. With the
 *	object-based Tcl_FS APIs, the above NOTE may no longer be true. In any
 *	case this assertion should be tested.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_FileObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int index;

    /*
     * This list of constants should match the fileOption string array below.
     */

    static CONST char *fileOptions[] = {
	"atime",	"attributes",	"channels",	"copy",
	"delete",
	"dirname",	"executable",	"exists",	"extension",
	"isdirectory",	"isfile",	"join",		"link",
	"lstat",	"mtime",	"mkdir",	"nativename",
	"normalize",    "owned",
	"pathtype",	"readable",	"readlink",	"rename",
	"rootname",	"separator",    "size",		"split",
	"stat",		"system",
	"tail",		"type",		"volumes",	"writable",
	(char *) NULL
    };
    enum options {
	FCMD_ATIME,	FCMD_ATTRIBUTES, FCMD_CHANNELS,	FCMD_COPY,
	FCMD_DELETE,
	FCMD_DIRNAME,	FCMD_EXECUTABLE, FCMD_EXISTS,	FCMD_EXTENSION,
	FCMD_ISDIRECTORY, FCMD_ISFILE,	FCMD_JOIN,	FCMD_LINK,
	FCMD_LSTAT,	FCMD_MTIME,	FCMD_MKDIR,	FCMD_NATIVENAME,
	FCMD_NORMALIZE,	FCMD_OWNED,
	FCMD_PATHTYPE,	FCMD_READABLE,	FCMD_READLINK,	FCMD_RENAME,
	FCMD_ROOTNAME,	FCMD_SEPARATOR,	FCMD_SIZE,	FCMD_SPLIT,
	FCMD_STAT,	FCMD_SYSTEM,
	FCMD_TAIL,	FCMD_TYPE,	FCMD_VOLUMES,	FCMD_WRITABLE
    };

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "option ?arg ...?");
	return TCL_ERROR;
    }
    if (Tcl_GetIndexFromObj(interp, objv[1], fileOptions, "option", 0,
	    &index) != TCL_OK) {
	return TCL_ERROR;
    }

    switch ((enum options) index) {
    {
	Tcl_StatBuf buf;
	struct utimbuf tval;

    case FCMD_ATIME:
    case FCMD_MTIME:
	if ((objc < 3) || (objc > 4)) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?time?");
	    return TCL_ERROR;
	}
	if (GetStatBuf(interp, objv[2], Tcl_FSStat, &buf) != TCL_OK) {
	    return TCL_ERROR;
	}
	if (objc == 4) {
	    /*
	     * Need separate variable for reading longs from an object on
	     * 64-bit platforms. [Bug #698146]
	     */

	    long newTime;

	    if (Tcl_GetLongFromObj(interp, objv[3], &newTime) != TCL_OK) {
		return TCL_ERROR;
	    }

	    if (index == FCMD_ATIME) {
		tval.actime = newTime;
		tval.modtime = buf.st_mtime;
	    } else {	/* index == FCMD_MTIME */
		tval.actime = buf.st_atime;
		tval.modtime = newTime;
	    }

	    if (Tcl_FSUtime(objv[2], &tval) != 0) {
		Tcl_AppendResult(interp, "could not set ",
			(index == FCMD_ATIME ? "access" : "modification"),
			" time for file \"", TclGetString(objv[2]), "\": ",
			Tcl_PosixError(interp), (char *) NULL);
		return TCL_ERROR;
	    }

	    /*
	     * Do another stat to ensure that the we return the new recognized
	     * atime - hopefully the same as the one we sent in. However, fs's
	     * like FAT don't even know what atime is.
	     */

	    if (GetStatBuf(interp, objv[2], Tcl_FSStat, &buf) != TCL_OK) {
		return TCL_ERROR;
	    }
	}

	Tcl_SetObjResult(interp, Tcl_NewLongObj((long)
		(index == FCMD_ATIME ? buf.st_atime : buf.st_mtime)));
	return TCL_OK;
    }
    case FCMD_ATTRIBUTES:
	return TclFileAttrsCmd(interp, objc, objv);
    case FCMD_CHANNELS:
	if ((objc < 2) || (objc > 3)) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?pattern?");
	    return TCL_ERROR;
	}
	return Tcl_GetChannelNamesEx(interp,
		((objc == 2) ? NULL : TclGetString(objv[2])));
    case FCMD_COPY:
	return TclFileCopyCmd(interp, objc, objv);
    case FCMD_DELETE:
	return TclFileDeleteCmd(interp, objc, objv);
    case FCMD_DIRNAME: {
	Tcl_Obj *dirPtr;

	if (objc != 3) {
	    goto only3Args;
	}
	dirPtr = TclPathPart(interp, objv[2], TCL_PATH_DIRNAME);
	if (dirPtr == NULL) {
	    return TCL_ERROR;
	} else {
	    Tcl_SetObjResult(interp, dirPtr);
	    Tcl_DecrRefCount(dirPtr);
	    return TCL_OK;
	}
    }
    case FCMD_EXECUTABLE:
	if (objc != 3) {
	    goto only3Args;
	}
	return CheckAccess(interp, objv[2], X_OK);
    case FCMD_EXISTS:
	if (objc != 3) {
	    goto only3Args;
	}
	return CheckAccess(interp, objv[2], F_OK);
    case FCMD_EXTENSION: {
	Tcl_Obj *ext;

	if (objc != 3) {
	    goto only3Args;
	}
	ext = TclPathPart(interp, objv[2], TCL_PATH_EXTENSION);
	if (ext != NULL) {
	    Tcl_SetObjResult(interp, ext);
	    Tcl_DecrRefCount(ext);
	    return TCL_OK;
	} else {
	    return TCL_ERROR;
	}
    }
    {
	int value;
	Tcl_StatBuf buf;

    case FCMD_ISDIRECTORY:
	if (objc != 3) {
	    goto only3Args;
	}
	value = 0;
	if (GetStatBuf(NULL, objv[2], Tcl_FSStat, &buf) == TCL_OK) {
	    value = S_ISDIR(buf.st_mode);
	}
	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(value));
	return TCL_OK;
    case FCMD_ISFILE:
	if (objc != 3) {
	    goto only3Args;
	}
	value = 0;
	if (GetStatBuf(NULL, objv[2], Tcl_FSStat, &buf) == TCL_OK) {
	    value = S_ISREG(buf.st_mode);
	}
	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(value));
	return TCL_OK;
    case FCMD_OWNED:
	if (objc != 3) {
	    goto only3Args;
	}
	value = 0;
	if (GetStatBuf(NULL, objv[2], Tcl_FSStat, &buf) == TCL_OK) {
	    /*
	     * For Windows, there are no user ids associated with a file, so
	     * we always return 1.
	     */

#if defined(__WIN32__)
	    value = 1;
#else
	    value = (geteuid() == buf.st_uid);
#endif
	}
	Tcl_SetObjResult(interp, Tcl_NewBooleanObj(value));
	return TCL_OK;
    }
    case FCMD_JOIN: {
	Tcl_Obj *resObj;

	if (objc < 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?name ...?");
	    return TCL_ERROR;
	}
	resObj = Tcl_FSJoinToPath(NULL, objc - 2, objv + 2);
	Tcl_SetObjResult(interp, resObj);
	return TCL_OK;
    }
    case FCMD_LINK: {
	Tcl_Obj *contents;
	int index;

	if (objc < 3 || objc > 5) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?-linktype? linkname ?target?");
	    return TCL_ERROR;
	}

	/*
	 * Index of the 'source' argument.
	 */

	if (objc == 5) {
	    index = 3;
	} else {
	    index = 2;
	}

	if (objc > 3) {
	    int linkAction;
	    if (objc == 5) {
		/*
		 * We have a '-linktype' argument.
		 */

		static CONST char *linkTypes[] = {
		    "-symbolic", "-hard", NULL
		};
		if (Tcl_GetIndexFromObj(interp, objv[2], linkTypes, "switch",
			0, &linkAction) != TCL_OK) {
		    return TCL_ERROR;
		}
		if (linkAction == 0) {
		    linkAction = TCL_CREATE_SYMBOLIC_LINK;
		} else {
		    linkAction = TCL_CREATE_HARD_LINK;
		}
	    } else {
		linkAction = TCL_CREATE_SYMBOLIC_LINK|TCL_CREATE_HARD_LINK;
	    }
	    if (Tcl_FSConvertToPathType(interp, objv[index]) != TCL_OK) {
		return TCL_ERROR;
	    }

	    /*
	     * Create link from source to target.
	     */

	    contents = Tcl_FSLink(objv[index], objv[index+1], linkAction);
	    if (contents == NULL) {
		/*
		 * We handle three common error cases specially, and for all
		 * other errors, we use the standard posix error message.
		 */

		if (errno == EEXIST) {
		    Tcl_AppendResult(interp, "could not create new link \"",
			    TclGetString(objv[index]),
			    "\": that path already exists", (char *) NULL);
		} else if (errno == ENOENT) {
		    /*
		     * There are two cases here: either the target doesn't
		     * exist, or the directory of the src doesn't exist.
		     */

		    int access;
		    Tcl_Obj *dirPtr = TclPathPart(interp, objv[index],
			    TCL_PATH_DIRNAME);

		    if (dirPtr == NULL) {
			return TCL_ERROR;
		    }
		    access = Tcl_FSAccess(dirPtr, F_OK);
		    Tcl_DecrRefCount(dirPtr);
		    if (access != 0) {
			Tcl_AppendResult(interp,
				"could not create new link \"",
				TclGetString(objv[index]),
				"\": no such file or directory",
				(char *) NULL);
		    } else {
			Tcl_AppendResult(interp,
				"could not create new link \"",
				TclGetString(objv[index]), "\": target \"",
				TclGetString(objv[index+1]),
				"\" doesn't exist", (char *) NULL);
		    }
		} else {
		    Tcl_AppendResult(interp,
			    "could not create new link \"",
			    TclGetString(objv[index]), "\" pointing to \"",
			    TclGetString(objv[index+1]), "\": ",
			    Tcl_PosixError(interp), (char *) NULL);
		}
		return TCL_ERROR;
	    }
	} else {
	    if (Tcl_FSConvertToPathType(interp, objv[index]) != TCL_OK) {
		return TCL_ERROR;
	    }

	    /*
	     * Read link
	     */

	    contents = Tcl_FSLink(objv[index], NULL, 0);
	    if (contents == NULL) {
		Tcl_AppendResult(interp, "could not read link \"",
			TclGetString(objv[index]), "\": ",
			Tcl_PosixError(interp), (char *) NULL);
		return TCL_ERROR;
	    }
	}
	Tcl_SetObjResult(interp, contents);
	if (objc == 3) {
	    /*
	     * If we are reading a link, we need to free this result refCount.
	     * If we are creating a link, this will just be objv[index+1], and
	     * so we don't own it.
	     */

	    Tcl_DecrRefCount(contents);
	}
	return TCL_OK;
    }
    {
	Tcl_StatBuf buf;

    case FCMD_LSTAT:
	if (objc != 4) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name varName");
	    return TCL_ERROR;
	}
	if (GetStatBuf(interp, objv[2], Tcl_FSLstat, &buf) != TCL_OK) {
	    return TCL_ERROR;
	}
	return StoreStatData(interp, objv[3], &buf);
    case FCMD_STAT:
	if (objc != 4) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name varName");
	    return TCL_ERROR;
	}
	if (GetStatBuf(interp, objv[2], Tcl_FSStat, &buf) != TCL_OK) {
	    return TCL_ERROR;
	}
	return StoreStatData(interp, objv[3], &buf);
    case FCMD_SIZE:
	if (objc != 3) {
	    goto only3Args;
	}
	if (GetStatBuf(interp, objv[2], Tcl_FSStat, &buf) != TCL_OK) {
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp,
		Tcl_NewWideIntObj((Tcl_WideInt) buf.st_size));
	return TCL_OK;
    case FCMD_TYPE:
	if (objc != 3) {
	    goto only3Args;
	}
	if (GetStatBuf(interp, objv[2], Tcl_FSLstat, &buf) != TCL_OK) {
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, Tcl_NewStringObj(
		GetTypeFromMode((unsigned short) buf.st_mode), -1));
	return TCL_OK;
    }
    case FCMD_MKDIR:
	if (objc < 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "name ?name ...?");
	    return TCL_ERROR;
	}
	return TclFileMakeDirsCmd(interp, objc, objv);
    case FCMD_NATIVENAME: {
	CONST char *fileName;
	Tcl_DString ds;

	if (objc != 3) {
	    goto only3Args;
	}
	fileName = TclGetString(objv[2]);
	fileName = Tcl_TranslateFileName(interp, fileName, &ds);
	if (fileName == NULL) {
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, Tcl_NewStringObj(fileName,
		Tcl_DStringLength(&ds)));
	Tcl_DStringFree(&ds);
	return TCL_OK;
    }
    case FCMD_NORMALIZE: {
	Tcl_Obj *fileName;

	if (objc != 3) {
	    Tcl_WrongNumArgs(interp, 2, objv, "filename");
	    return TCL_ERROR;
	}

	fileName = Tcl_FSGetNormalizedPath(interp, objv[2]);
	if (fileName == NULL) {
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, fileName);
	return TCL_OK;
    }
    case FCMD_PATHTYPE:
	if (objc != 3) {
	    goto only3Args;
	}
	switch (Tcl_FSGetPathType(objv[2])) {
	case TCL_PATH_ABSOLUTE:
	    Tcl_SetObjResult(interp, Tcl_NewStringObj("absolute", -1));
	    break;
	case TCL_PATH_RELATIVE:
	    Tcl_SetObjResult(interp, Tcl_NewStringObj("relative", -1));
	    break;
	case TCL_PATH_VOLUME_RELATIVE:
	    Tcl_SetObjResult(interp, Tcl_NewStringObj("volumerelative", -1));
	    break;
	}
	return TCL_OK;
    case FCMD_READABLE:
	if (objc != 3) {
	    goto only3Args;
	}
	return CheckAccess(interp, objv[2], R_OK);
    case FCMD_READLINK: {
	Tcl_Obj *contents;

	if (objc != 3) {
	    goto only3Args;
	}

	if (Tcl_FSConvertToPathType(interp, objv[2]) != TCL_OK) {
	    return TCL_ERROR;
	}

	contents = Tcl_FSLink(objv[2], NULL, 0);

	if (contents == NULL) {
	    Tcl_AppendResult(interp, "could not readlink \"",
		    TclGetString(objv[2]), "\": ", Tcl_PosixError(interp),
		    (char *) NULL);
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, contents);
	Tcl_DecrRefCount(contents);
	return TCL_OK;
    }
    case FCMD_RENAME:
	return TclFileRenameCmd(interp, objc, objv);
    case FCMD_ROOTNAME: {
	Tcl_Obj *root;

	if (objc != 3) {
	    goto only3Args;
	}
	root = TclPathPart(interp, objv[2], TCL_PATH_ROOT);
	if (root != NULL) {
	    Tcl_SetObjResult(interp, root);
	    Tcl_DecrRefCount(root);
	    return TCL_OK;
	} else {
	    return TCL_ERROR;
	}
    }
    case FCMD_SEPARATOR:
	if ((objc < 2) || (objc > 3)) {
	    Tcl_WrongNumArgs(interp, 2, objv, "?name?");
	    return TCL_ERROR;
	}
	if (objc == 2) {
	    char *separator = NULL; /* lint */

	    switch (tclPlatform) {
	    case TCL_PLATFORM_UNIX:
		separator = "/";
		break;
	    case TCL_PLATFORM_WINDOWS:
		separator = "\\";
		break;
	    }
	    Tcl_SetObjResult(interp, Tcl_NewStringObj(separator,1));
	} else {
	    Tcl_Obj *separatorObj = Tcl_FSPathSeparator(objv[2]);
	    if (separatorObj != NULL) {
		Tcl_SetObjResult(interp, separatorObj);
	    } else {
		Tcl_SetObjResult(interp,
			Tcl_NewStringObj("Unrecognised path",-1));
		return TCL_ERROR;
	    }
	}
	return TCL_OK;
    case FCMD_SPLIT: {
	Tcl_Obj *res;

	if (objc != 3) {
	    goto only3Args;
	}
	res = Tcl_FSSplitPath(objv[2], NULL);
	if (res == NULL) {
	    if (interp != NULL) {
		Tcl_AppendResult(interp, "could not read \"",
			TclGetString(objv[2]),
			"\": no such file or directory", (char *) NULL);
	    }
	    return TCL_ERROR;
	} else {
	    Tcl_SetObjResult(interp, res);
	    return TCL_OK;
	}
    }
    case FCMD_SYSTEM: {
	Tcl_Obj* fsInfo;

	if (objc != 3) {
	    goto only3Args;
	}
	fsInfo = Tcl_FSFileSystemInfo(objv[2]);
	if (fsInfo != NULL) {
	    Tcl_SetObjResult(interp, fsInfo);
	    return TCL_OK;
	} else {
	    Tcl_SetObjResult(interp, Tcl_NewStringObj("Unrecognised path",-1));
	    return TCL_ERROR;
	}
    }
    case FCMD_TAIL: {
	Tcl_Obj *dirPtr;

	if (objc != 3) {
	    goto only3Args;
	}
	dirPtr = TclPathPart(interp, objv[2], TCL_PATH_TAIL);
	if (dirPtr == NULL) {
	    return TCL_ERROR;
	} else {
	    Tcl_SetObjResult(interp, dirPtr);
	    Tcl_DecrRefCount(dirPtr);
	    return TCL_OK;
	}
    }
    case FCMD_VOLUMES:
	if (objc != 2) {
	    Tcl_WrongNumArgs(interp, 2, objv, NULL);
	    return TCL_ERROR;
	}
	Tcl_SetObjResult(interp, Tcl_FSListVolumes());
	return TCL_OK;
    case FCMD_WRITABLE:
	if (objc != 3) {
	    goto only3Args;
	}
	return CheckAccess(interp, objv[2], W_OK);
    }

  only3Args:
    Tcl_WrongNumArgs(interp, 2, objv, "name");
    return TCL_ERROR;
}

/*
 *---------------------------------------------------------------------------
 *
 * CheckAccess --
 *
 *	Utility procedure used by Tcl_FileObjCmd() to query file attributes
 *	available through the access() system call.
 *
 * Results:
 *	Always returns TCL_OK. Sets interp's result to boolean true or false
 *	depending on whether the file has the specified attribute.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static int
CheckAccess(interp, pathPtr, mode)
    Tcl_Interp *interp;		/* Interp for status return. Must not be
				 * NULL. */
    Tcl_Obj *pathPtr;		/* Name of file to check. */
    int mode;			/* Attribute to check; passed as argument to
				 * access(). */
{
    int value;

    if (Tcl_FSConvertToPathType(interp, pathPtr) != TCL_OK) {
	value = 0;
    } else {
	value = (Tcl_FSAccess(pathPtr, mode) == 0);
    }
    Tcl_SetObjResult(interp, Tcl_NewBooleanObj(value));

    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetStatBuf --
 *
 *	Utility procedure used by Tcl_FileObjCmd() to query file attributes
 *	available through the stat() or lstat() system call.
 *
 * Results:
 *	The return value is TCL_OK if the specified file exists and can be
 *	stat'ed, TCL_ERROR otherwise. If TCL_ERROR is returned, an error
 *	message is left in interp's result. If TCL_OK is returned, *statPtr is
 *	filled with information about the specified file.
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------------
 */

static int
GetStatBuf(interp, pathPtr, statProc, statPtr)
    Tcl_Interp *interp;		/* Interp for error return. May be NULL. */
    Tcl_Obj *pathPtr;		/* Path name to examine. */
    Tcl_FSStatProc *statProc;	/* Either stat() or lstat() depending on
				 * desired behavior. */
    Tcl_StatBuf *statPtr;	/* Filled with info about file obtained by
				 * calling (*statProc)(). */
{
    int status;

    if (Tcl_FSConvertToPathType(interp, pathPtr) != TCL_OK) {
	return TCL_ERROR;
    }

    status = (*statProc)(pathPtr, statPtr);

    if (status < 0) {
	if (interp != NULL) {
	    Tcl_AppendResult(interp, "could not read \"",
		    TclGetString(pathPtr), "\": ",
		    Tcl_PosixError(interp), (char *) NULL);
	}
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * StoreStatData --
 *
 *	This is a utility procedure that breaks out the fields of a "stat"
 *	structure and stores them in textual form into the elements of an
 *	associative array.
 *
 * Results:
 *	Returns a standard Tcl return value. If an error occurs then a message
 *	is left in interp's result.
 *
 * Side effects:
 *	Elements of the associative array given by "varName" are modified.
 *
 *----------------------------------------------------------------------
 */

static int
StoreStatData(interp, varName, statPtr)
    Tcl_Interp *interp;		/* Interpreter for error reports. */
    Tcl_Obj *varName;		/* Name of associative array variable in which
				 * to store stat results. */
    Tcl_StatBuf *statPtr;	/* Pointer to buffer containing stat data to
				 * store in varName. */
{
    Tcl_Obj *field = Tcl_NewObj();
    Tcl_Obj *value;
    register unsigned short mode;

    /*
     * Assume Tcl_ObjSetVar2() does not keep a copy of the field name!
     *
     * Might be a better idea to call Tcl_SetVar2Ex() instead so we don't have
     * to make assumptions that might go wrong later.
     */

#define STORE_ARY(fieldName, object) \
    Tcl_SetStringObj(field, (fieldName), -1); \
    value = (object); \
    if (Tcl_ObjSetVar2(interp,varName,field,value,TCL_LEAVE_ERR_MSG)==NULL) { \
	Tcl_DecrRefCount(field); \
	Tcl_DecrRefCount(value); \
	return TCL_ERROR; \
    }

    Tcl_IncrRefCount(field);

    /*
     * Watch out porters; the inode is meant to be an *unsigned* value, so the
     * cast might fail when there isn't a real arithmentic 'long long' type...
     */

    STORE_ARY("dev",	Tcl_NewLongObj((long)statPtr->st_dev));
    STORE_ARY("ino",	Tcl_NewWideIntObj((Tcl_WideInt)statPtr->st_ino));
    STORE_ARY("nlink",	Tcl_NewLongObj((long)statPtr->st_nlink));
    STORE_ARY("uid",	Tcl_NewLongObj((long)statPtr->st_uid));
    STORE_ARY("gid",	Tcl_NewLongObj((long)statPtr->st_gid));
    STORE_ARY("size",	Tcl_NewWideIntObj((Tcl_WideInt)statPtr->st_size));
#ifdef HAVE_ST_BLOCKS
    STORE_ARY("blocks",	Tcl_NewWideIntObj((Tcl_WideInt)statPtr->st_blocks));
#endif
    STORE_ARY("atime",	Tcl_NewLongObj((long)statPtr->st_atime));
    STORE_ARY("mtime",	Tcl_NewLongObj((long)statPtr->st_mtime));
    STORE_ARY("ctime",	Tcl_NewLongObj((long)statPtr->st_ctime));
    mode = (unsigned short) statPtr->st_mode;
    STORE_ARY("mode",	Tcl_NewIntObj(mode));
    STORE_ARY("type",	Tcl_NewStringObj(GetTypeFromMode(mode), -1));
#undef STORE_ARY

    Tcl_DecrRefCount(field);
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * GetTypeFromMode --
 *
 *	Given a mode word, returns a string identifying the type of a file.
 *
 * Results:
 *	A static text string giving the file type from mode.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static char *
GetTypeFromMode(mode)
    int mode;
{
    if (S_ISREG(mode)) {
	return "file";
    } else if (S_ISDIR(mode)) {
	return "directory";
    } else if (S_ISCHR(mode)) {
	return "characterSpecial";
    } else if (S_ISBLK(mode)) {
	return "blockSpecial";
    } else if (S_ISFIFO(mode)) {
	return "fifo";
#ifdef S_ISLNK
    } else if (S_ISLNK(mode)) {
	return "link";
#endif
#ifdef S_ISSOCK
    } else if (S_ISSOCK(mode)) {
	return "socket";
#endif
    }
    return "unknown";
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ForObjCmd --
 *
 *	This procedure is invoked to process the "for" Tcl command. See the
 *	user documentation for details on what it does.
 *
 *	With the bytecode compiler, this procedure is only called when a
 *	command name is computed at runtime, and is "for" or the name to which
 *	"for" was renamed: e.g.,
 *	"set z for; $z {set i 0} {$i<100} {incr i} {puts $i}"
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ForObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int result, value;

    if (objc != 5) {
	Tcl_WrongNumArgs(interp, 1, objv, "start test next command");
	return TCL_ERROR;
    }

    result = Tcl_EvalObjEx(interp, objv[1], 0);
    if (result != TCL_OK) {
	if (result == TCL_ERROR) {
	    Tcl_AddErrorInfo(interp, "\n    (\"for\" initial command)");
	}
	return result;
    }
    while (1) {
	/*
	 * We need to reset the result before passing it off to
	 * Tcl_ExprBooleanObj. Otherwise, any error message will be appended
	 * to the result of the last evaluation.
	 */

	Tcl_ResetResult(interp);
	result = Tcl_ExprBooleanObj(interp, objv[2], &value);
	if (result != TCL_OK) {
	    return result;
	}
	if (!value) {
	    break;
	}
	result = Tcl_EvalObjEx(interp, objv[4], 0);
	if ((result != TCL_OK) && (result != TCL_CONTINUE)) {
	    if (result == TCL_ERROR) {
		char msg[32 + TCL_INTEGER_SPACE];

		sprintf(msg, "\n    (\"for\" body line %d)",interp->errorLine);
		Tcl_AddErrorInfo(interp, msg);
	    }
	    break;
	}
	result = Tcl_EvalObjEx(interp, objv[3], 0);
	if (result == TCL_BREAK) {
	    break;
	} else if (result != TCL_OK) {
	    if (result == TCL_ERROR) {
		Tcl_AddErrorInfo(interp, "\n    (\"for\" loop-end command)");
	    }
	    return result;
	}
    }
    if (result == TCL_BREAK) {
	result = TCL_OK;
    }
    if (result == TCL_OK) {
	Tcl_ResetResult(interp);
    }
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ForeachObjCmd --
 *
 *	This object-based procedure is invoked to process the "foreach" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ForeachObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    int result = TCL_OK;
    int i;			/* i selects a value list */
    int j, maxj;		/* Number of loop iterations */
    int v;			/* v selects a loop variable */
    int numLists;		/* Count of value lists */
    Tcl_Obj *bodyPtr;

    /*
     * We copy the argument object pointers into a local array to avoid the
     * problem that "objv" might become invalid. It is a pointer into the
     * evaluation stack and that stack might be grown and reallocated if the
     * loop body requires a large amount of stack space.
     */

#define NUM_ARGS 9
    Tcl_Obj *(argObjStorage[NUM_ARGS]);
    Tcl_Obj **argObjv = argObjStorage;

#define STATIC_LIST_SIZE 4
    int indexArray[STATIC_LIST_SIZE];
    int varcListArray[STATIC_LIST_SIZE];
    Tcl_Obj **varvListArray[STATIC_LIST_SIZE];
    int argcListArray[STATIC_LIST_SIZE];
    Tcl_Obj **argvListArray[STATIC_LIST_SIZE];

    int *index = indexArray;		/* Array of value list indices */
    int *varcList = varcListArray;	/* # loop variables per list */
    Tcl_Obj ***varvList = varvListArray;/* Array of var name lists */
    int *argcList = argcListArray;	/* Array of value list sizes */
    Tcl_Obj ***argvList = argvListArray;/* Array of value lists */

    if (objc < 4 || (objc%2 != 0)) {
	Tcl_WrongNumArgs(interp, 1, objv,
		"varList list ?varList list ...? command");
	return TCL_ERROR;
    }

    /*
     * Create the object argument array "argObjv". Make sure argObjv is large
     * enough to hold the objc arguments.
     */

    if (objc > NUM_ARGS) {
	argObjv = (Tcl_Obj **) ckalloc(objc * sizeof(Tcl_Obj *));
    }
    for (i=0 ; i<objc ; i++) {
	argObjv[i] = objv[i];
    }

    /*
     * Manage numList parallel value lists.
     * argvList[i] is a value list counted by argcList[i]l;
     * varvList[i] is the list of variables associated with the value list;
     * varcList[i] is the number of variables associated with the value list;
     * index[i] is the current pointer into the value list argvList[i].
     */

    numLists = (objc-2)/2;
    if (numLists > STATIC_LIST_SIZE) {
	index = (int *) ckalloc(numLists * sizeof(int));
	varcList = (int *) ckalloc(numLists * sizeof(int));
	varvList = (Tcl_Obj ***) ckalloc(numLists * sizeof(Tcl_Obj **));
	argcList = (int *) ckalloc(numLists * sizeof(int));
	argvList = (Tcl_Obj ***) ckalloc(numLists * sizeof(Tcl_Obj **));
    }
    for (i = 0;  i < numLists;  i++) {
	index[i] = 0;
	varcList[i] = 0;
	varvList[i] = (Tcl_Obj **) NULL;
	argcList[i] = 0;
	argvList[i] = (Tcl_Obj **) NULL;
    }

    /*
     * Break up the value lists and variable lists into elements.
     */

    maxj = 0;
    for (i=0 ; i<numLists ; i++) {
	result = Tcl_ListObjGetElements(interp, argObjv[1+i*2],
		&varcList[i], &varvList[i]);
	if (result != TCL_OK) {
	    goto done;
	}
	if (varcList[i] < 1) {
	    Tcl_AppendResult(interp, "foreach varlist is empty", NULL);
	    result = TCL_ERROR;
	    goto done;
	}

	result = Tcl_ListObjGetElements(interp, argObjv[2+i*2],
		&argcList[i], &argvList[i]);
	if (result != TCL_OK) {
	    goto done;
	}

	j = argcList[i] / varcList[i];
	if ((argcList[i] % varcList[i]) != 0) {
	    j++;
	}
	if (j > maxj) {
	    maxj = j;
	}
    }

    /*
     * Iterate maxj times through the lists in parallel. If some value lists
     * run out of values, set loop vars to ""
     */

    bodyPtr = argObjv[objc-1];
    for (j=0 ; j<maxj ; j++) {
	for (i=0 ; i<numLists ; i++) {
	    /*
	     * Refetch the list members; we assume that the sizes are the
	     * same, but the array of elements might be different if the
	     * internal rep of the objects has been lost and recreated (it is
	     * too difficult to accurately tell when this happens, which can
	     * lead to some wierd crashes, like Bug #494348...)
	     */

	    result = Tcl_ListObjGetElements(interp, argObjv[1+i*2],
		    &varcList[i], &varvList[i]);
	    if (result != TCL_OK) {
		Tcl_Panic("Tcl_ForeachObjCmd: could not reconvert variable list %d to a list object\n", i);
	    }
	    result = Tcl_ListObjGetElements(interp, argObjv[2+i*2],
		    &argcList[i], &argvList[i]);
	    if (result != TCL_OK) {
		Tcl_Panic("Tcl_ForeachObjCmd: could not reconvert value list %d to a list object\n", i);
	    }

	    for (v=0 ; v<varcList[i] ; v++) {
		int k = index[i]++;
		Tcl_Obj *valuePtr, *varValuePtr;
		int isEmptyObj = 0;

		if (k < argcList[i]) {
		    valuePtr = argvList[i][k];
		} else {
		    valuePtr = Tcl_NewObj(); /* empty string */
		    isEmptyObj = 1;
		}
		varValuePtr = Tcl_ObjSetVar2(interp, varvList[i][v],
			NULL, valuePtr, 0);
		if (varValuePtr == NULL) {
		    if (isEmptyObj) {
			Tcl_DecrRefCount(valuePtr);
		    }
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, "couldn't set loop variable: \"",
			    TclGetString(varvList[i][v]), "\"", (char *) NULL);
		    result = TCL_ERROR;
		    goto done;
		}
	    }
	}

	result = Tcl_EvalObjEx(interp, bodyPtr, 0);
	if (result != TCL_OK) {
	    if (result == TCL_CONTINUE) {
		result = TCL_OK;
	    } else if (result == TCL_BREAK) {
		result = TCL_OK;
		break;
	    } else if (result == TCL_ERROR) {
		char msg[32 + TCL_INTEGER_SPACE];

		sprintf(msg, "\n    (\"foreach\" body line %d)",
			interp->errorLine);
		Tcl_AddObjErrorInfo(interp, msg, -1);
		break;
	    } else {
		break;
	    }
	}
    }
    if (result == TCL_OK) {
	Tcl_ResetResult(interp);
    }

  done:
    if (numLists > STATIC_LIST_SIZE) {
	ckfree((char *) index);
	ckfree((char *) varcList);
	ckfree((char *) argcList);
	ckfree((char *) varvList);
	ckfree((char *) argvList);
    }
    if (argObjv != argObjStorage) {
	ckfree((char *) argObjv);
    }
    return result;
#undef STATIC_LIST_SIZE
#undef NUM_ARGS
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FormatObjCmd --
 *
 *	This procedure is invoked to process the "format" Tcl command. See
 *	the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_FormatObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    char *format;		/* Used to read characters from the format
				 * string. */
    int formatLen;		/* The length of the format string */
    char *endPtr;		/* Points to the last char in format array */
    char newFormat[43];		/* A new format specifier is generated here. */
    int width;			/* Field width from field specifier, or 0 if
				 * no width given. */
    int precision;		/* Field precision from field specifier, or 0
				 * if no precision given. */
    int size;			/* Number of bytes needed for result of
				 * conversion, based on type of conversion
				 * ("e", "s", etc.), width, and precision. */
    long intValue;		/* Used to hold value to pass to sprintf, if
				 * it's a one-word integer or char value */
    char *ptrValue = NULL;	/* Used to hold value to pass to sprintf, if
				 * it's a one-word value. */
    double doubleValue;		/* Used to hold value to pass to sprintf if
				 * it's a double value. */
    Tcl_WideInt wideValue;	/* Used to hold value to pass to sprintf if
				 * it's a 'long long' value. */
    int whichValue;		/* Indicates which of intValue, ptrValue, or
				 * doubleValue has the value to pass to
				 * sprintf, according to the following
				 * definitions: */
#define INT_VALUE	0
#define CHAR_VALUE	1
#define PTR_VALUE	2
#define DOUBLE_VALUE	3
#define STRING_VALUE	4
#define WIDE_VALUE	5
#define MAX_FLOAT_SIZE	320

    Tcl_Obj *resultPtr;		/* Where result is stored finally. */
    char staticBuf[MAX_FLOAT_SIZE + 1];
				/* A static buffer to copy the format results
				 * into */
    char *dst = staticBuf;	/* The buffer that sprintf writes into each
				 * time the format processes a specifier */
    int dstSize = MAX_FLOAT_SIZE;
				/* The size of the dst buffer */
    int noPercent;		/* Special case for speed: indicates there's
				 * no field specifier, just a string to
				 * copy. */
    int objIndex;		/* Index of argument to substitute next. */
    int gotXpg = 0;		/* Non-zero means that an XPG3 %n$-style
				 * specifier has been seen. */
    int gotSequential = 0;	/* Non-zero means that a regular sequential
				 * (non-XPG3) conversion specifier has been
				 * seen. */
    int useShort;		/* Value to be printed is short (half word). */
    char *end;			/* Used to locate end of numerical fields. */
    int stringLen = 0;		/* Length of string in characters rather than
				 * bytes. Used for %s substitution. */
    int gotMinus;		/* Non-zero indicates that a minus flag has
				 * been seen in the current field. */
    int gotPrecision;		/* Non-zero indicates that a precision has
				 * been set for the current field. */
    int gotZero;		/* Non-zero indicates that a zero flag has
				 * been seen in the current field. */
    int useWide;		/* Value to be printed is Tcl_WideInt. */

    /*
     * This procedure is a bit nasty. The goal is to use sprintf to do most of
     * the dirty work. There are several problems:
     * 1. this procedure can't trust its arguments.
     * 2. we must be able to provide a large enough result area to hold
     *	  whatever's generated. This is hard to estimate.
     * 3. there's no way to move the arguments from objv to the call to
     *	  sprintf in a reasonable way. This is particularly nasty because
     *	  some of the arguments may be two-word values (doubles and
     *	  wide-ints).
     * So, what happens here is to scan the format string one % group at a
     * time, making many individual calls to sprintf.
     */

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "formatString ?arg arg ...?");
	return TCL_ERROR;
    }

    format = Tcl_GetStringFromObj(objv[1], &formatLen);
    endPtr = format + formatLen;
    resultPtr = Tcl_NewObj();
    objIndex = 2;

    while (format < endPtr) {
	register char *newPtr = newFormat;

	width = precision = noPercent = useShort = 0;
	gotZero = gotMinus = gotPrecision = 0;
	useWide = 0;
	whichValue = PTR_VALUE;

	/*
	 * Get rid of any characters before the next field specifier.
	 */

	if (*format != '%') {
	    ptrValue = format;
	    while ((*format != '%') && (format < endPtr)) {
		format++;
	    }
	    size = format - ptrValue;
	    noPercent = 1;
	    goto doField;
	}

	if (format[1] == '%') {
	    ptrValue = format;
	    size = 1;
	    noPercent = 1;
	    format += 2;
	    goto doField;
	}

	/*
	 * Parse off a field specifier, compute how many characters will be
	 * needed to store the result, and substitute for "*" size specifiers.
	 */

	*newPtr = '%';
	newPtr++;
	format++;
	if (isdigit(UCHAR(*format))) { /* INTL: Tcl source. */
	    int tmp;

	    /*
	     * Check for an XPG3-style %n$ specification. Note: there must not
	     * be a mixture of XPG3 specs and non-XPG3 specs in the same
	     * format string.
	     */

	    tmp = strtoul(format, &end, 10);	/* INTL: "C" locale. */
	    if (*end != '$') {
		goto notXpg;
	    }
	    format = end+1;
	    gotXpg = 1;
	    if (gotSequential) {
		goto mixedXPG;
	    }
	    objIndex = tmp+1;
	    if ((objIndex < 2) || (objIndex >= objc)) {
		goto badIndex;
	    }
	    goto xpgCheckDone;
	}

    notXpg:
	gotSequential = 1;
	if (gotXpg) {
	    goto mixedXPG;
	}

    xpgCheckDone:
	while ((*format == '-') || (*format == '#') || (*format == '0')
		|| (*format == ' ') || (*format == '+')) {
	    if (*format == '-') {
		gotMinus = 1;
	    }
	    if (*format == '0') {
		/*
		 * This will be handled by sprintf for numbers, but we need to
		 * do the char/string ones ourselves.
		 */

		gotZero = 1;
	    }
	    *newPtr = *format;
	    newPtr++;
	    format++;
	}
	if (isdigit(UCHAR(*format))) {		/* INTL: Tcl source. */
	    width = strtoul(format, &end, 10);	/* INTL: Tcl source. */
	    format = end;
	} else if (*format == '*') {
	    if (objIndex >= objc) {
		goto badIndex;
	    }
	    if (Tcl_GetIntFromObj(interp,	/* INTL: Tcl source. */
		    objv[objIndex], &width) != TCL_OK) {
		goto fmtError;
	    }
	    if (width < 0) {
		width = -width;
		*newPtr = '-';
		gotMinus = 1;
		newPtr++;
	    }
	    objIndex++;
	    format++;
	}
	if (width > 100000) {
	    /*
	     * Don't allow arbitrarily large widths: could cause core dump
	     * when we try to allocate a zillion bytes of memory below.
	     */

	    width = 100000;
	} else if (width < 0) {
	    width = 0;
	}
	if (width != 0) {
	    TclFormatInt(newPtr, width);	/* INTL: printf format. */
	    while (*newPtr != 0) {
		newPtr++;
	    }
	}
	if (*format == '.') {
	    *newPtr = '.';
	    newPtr++;
	    format++;
	    gotPrecision = 1;
	}
	if (isdigit(UCHAR(*format))) {		/* INTL: Tcl source. */
	    precision = strtoul(format, &end, 10);	/* INTL: "C" locale. */
	    format = end;
	} else if (*format == '*') {
	    if (objIndex >= objc) {
		goto badIndex;
	    }
	    if (Tcl_GetIntFromObj(interp,	/* INTL: Tcl source. */
		    objv[objIndex], &precision) != TCL_OK) {
		goto fmtError;
	    }
	    objIndex++;
	    format++;
	}
	if (gotPrecision) {
	    TclFormatInt(newPtr, precision);	/* INTL: printf format. */
	    while (*newPtr != 0) {
		newPtr++;
	    }
	}
	if (*format == 'l') {
	    useWide = 1;

	    /*
	     * Only add a 'll' modifier for integer values as it makes some
	     * libc's go into spasm otherwise. [Bug #702622]
	     */

	    switch (format[1]) {
	    case 'i':
	    case 'd':
	    case 'o':
	    case 'u':
	    case 'x':
	    case 'X':
		strcpy(newPtr, TCL_LL_MODIFIER);
		newPtr += TCL_LL_MODIFIER_SIZE;
	    }
	    format++;
	} else if (*format == 'h') {
	    useShort = 1;
	    *newPtr = 'h';
	    newPtr++;
	    format++;
	}
	*newPtr = *format;
	newPtr++;
	*newPtr = 0;
	if (objIndex >= objc) {
	    goto badIndex;
	}
	switch (*format) {
	case 'i':
	    newPtr[-1] = 'd';
	case 'd':
	case 'o':
	case 'u':
	case 'x':
	case 'X':
	    if (useWide) {
		if (Tcl_GetWideIntFromObj(interp,	/* INTL: Tcl source. */
			objv[objIndex], &wideValue) != TCL_OK) {
		    goto fmtError;
		}
		whichValue = WIDE_VALUE;
		size = 40 + precision;
		break;
	    }
	    if (Tcl_GetLongFromObj(interp,	/* INTL: Tcl source. */
		    objv[objIndex], &intValue) != TCL_OK) {
		goto fmtError;
	    }
#if (LONG_MAX > INT_MAX)
	    if (!useShort) {
		/*
		 * Add the 'l' for long format type because we are on an LP64
		 * archtecture and we are really going to pass a long argument
		 * to sprintf.
		 *
		 * Do not add this if we're going to pass in a short (i.e. if
		 * we've got an 'h' modifier already in the string); some libc
		 * implementations of sprintf() do not like it at all. [Bug
		 * 1154163]
		 */

		newPtr++;
		*newPtr = 0;
		newPtr[-1] = newPtr[-2];
		newPtr[-2] = 'l';
	    }
#endif /* LONG_MAX > INT_MAX */
	    whichValue = INT_VALUE;
	    size = 40 + precision;
	    break;
	case 's':
	    /*
	     * Compute the length of the string in characters and add any
	     * additional space required by the field width. All of the extra
	     * characters will be spaces, so one byte per character is
	     * adequate.
	     */

	    whichValue = STRING_VALUE;
	    ptrValue = Tcl_GetStringFromObj(objv[objIndex], &size);
	    stringLen = Tcl_NumUtfChars(ptrValue, size);
	    if (gotPrecision && (precision < stringLen)) {
		stringLen = precision;
	    }
	    size = Tcl_UtfAtIndex(ptrValue, stringLen) - ptrValue;
	    if (width > stringLen) {
		size += (width - stringLen);
	    }
	    break;
	case 'c':
	    if (Tcl_GetLongFromObj(interp,	/* INTL: Tcl source. */
		    objv[objIndex], &intValue) != TCL_OK) {
		goto fmtError;
	    }
	    whichValue = CHAR_VALUE;
	    size = width + TCL_UTF_MAX;
	    break;
	case 'e':
	case 'E':
	case 'f':
	case 'g':
	case 'G':
	    if (Tcl_GetDoubleFromObj(interp,	/* INTL: Tcl source. */
		    objv[objIndex], &doubleValue) != TCL_OK) {
		/*TODO: figure out ACCEPT_NAN */
		goto fmtError;
	    }
	    whichValue = DOUBLE_VALUE;
	    size = MAX_FLOAT_SIZE;
	    if (precision > 10) {
		size += precision;
	    }
	    break;
	case 0:
	    Tcl_SetResult(interp,
		    "format string ended in middle of field specifier",
		    TCL_STATIC);
	    goto fmtError;
	default:
	{
	    char buf[40];

	    sprintf(buf, "bad field specifier \"%c\"", *format);
	    Tcl_SetResult(interp, buf, TCL_VOLATILE);
	    goto fmtError;
	}
	}
	objIndex++;
	format++;

	/*
	 * Make sure that there's enough space to hold the formatted result,
	 * then format it.
	 */

    doField:
	if (width > size) {
	    size = width;
	}
	if (noPercent) {
	    Tcl_AppendToObj(resultPtr, ptrValue, size);
	} else {
	    if (size > dstSize) {
		if (dst != staticBuf) {
		    ckfree(dst);
		}
		dst = (char *) ckalloc((unsigned) (size + 1));
		dstSize = size;
	    }
	    switch (whichValue) {
	    case DOUBLE_VALUE:
		sprintf(dst, newFormat, doubleValue); /* INTL: user locale. */
		break;
	    case WIDE_VALUE:
		sprintf(dst, newFormat, wideValue);
		break;
	    case INT_VALUE:
		if (useShort) {
		    sprintf(dst, newFormat, (short) intValue);
		} else {
		    sprintf(dst, newFormat, intValue);
		}
		break;
	    case CHAR_VALUE: {
		char *ptr;
		char padChar = (gotZero ? '0' : ' ');
		ptr = dst;
		if (!gotMinus) {
		    for ( ; --width > 0; ptr++) {
			*ptr = padChar;
		    }
		}
		ptr += Tcl_UniCharToUtf(intValue, ptr);
		for ( ; --width > 0; ptr++) {
		    *ptr = padChar;
		}
		*ptr = '\0';
		break;
	    }
	    case STRING_VALUE: {
		char *ptr;
		char padChar = (gotZero ? '0' : ' ');
		int pad;

		ptr = dst;
		if (width > stringLen) {
		    pad = width - stringLen;
		} else {
		    pad = 0;
		}

		if (!gotMinus) {
		    while (pad > 0) {
			*ptr++ = padChar;
			pad--;
		    }
		}

		size = Tcl_UtfAtIndex(ptrValue, stringLen) - ptrValue;
		if (size) {
		    memcpy(ptr, ptrValue, (size_t) size);
		    ptr += size;
		}
		while (pad > 0) {
		    *ptr++ = padChar;
		    pad--;
		}
		*ptr = '\0';
		break;
	    }
	    default:
		sprintf(dst, newFormat, ptrValue);
		break;
	    }
	    Tcl_AppendToObj(resultPtr, dst, -1);
	}
    }

    Tcl_SetObjResult(interp, resultPtr);
    if (dst != staticBuf) {
	ckfree(dst);
    }
    return TCL_OK;

  mixedXPG:
    Tcl_SetResult(interp,
	    "cannot mix \"%\" and \"%n$\" conversion specifiers", TCL_STATIC);
    goto fmtError;

  badIndex:
    if (gotXpg) {
	Tcl_SetResult(interp,
		"\"%n$\" argument index out of range", TCL_STATIC);
    } else {
	Tcl_SetResult(interp,
		"not enough arguments for all format specifiers", TCL_STATIC);
    }

  fmtError:
    if (dst != staticBuf) {
	ckfree(dst);
    }
    Tcl_DecrRefCount(resultPtr);
    return TCL_ERROR;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
