/* 
 * tclProc.c --
 *
 *	This file contains routines that implement Tcl procedures,
 *	including the "proc" and "uplevel" commands.
 *
 * Copyright (c) 1987-1993 The Regents of the University of California.
 * Copyright (c) 1994-1998 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclProc.c,v 1.71 2004/12/20 21:20:06 msofer Exp $
 */

#include "tclInt.h"
#include "tclCompile.h"

/*
 * Prototypes for static functions in this file
 */

static void	ProcBodyDup _ANSI_ARGS_((Tcl_Obj *srcPtr, Tcl_Obj *dupPtr));
static void	ProcBodyFree _ANSI_ARGS_((Tcl_Obj *objPtr));
static int	ProcessProcResultCode _ANSI_ARGS_((Tcl_Interp *interp,
		    char *procName, int nameLen, int returnCode));
static int	TclCompileNoOp _ANSI_ARGS_((Tcl_Interp *interp,
		    Tcl_Parse *parsePtr, struct CompileEnv *envPtr));

static void     InitCompiledLocals _ANSI_ARGS_((Tcl_Interp *interp,
		    ByteCode *codePtr, CompiledLocal *localPtr,
		    Var *varPtr, Namespace *nsPtr)); 

/*
 * The ProcBodyObjType type
 */

Tcl_ObjType tclProcBodyType = {
    "procbody",			/* name for this type */
    ProcBodyFree,		/* FreeInternalRep procedure */
    ProcBodyDup,		/* DupInternalRep procedure */
    NULL,			/* UpdateString procedure; Tcl_GetString
				 * and Tcl_GetStringFromObj should panic
				 * instead. */
    NULL			/* SetFromAny procedure; Tcl_ConvertToType
				 * should panic instead. */
};

/*
 * The [upvar]/[uplevel] level reference type.  Uses the twoPtrValue
 * field, encoding the type of level reference in ptr1 and the actual
 * parsed out offset in ptr2.
 *
 * Uses the default behaviour throughout, and never disposes of the
 * string rep; it's just a cache type.
 */

Tcl_ObjType tclLevelReferenceType = {
    "levelReference",
    NULL, NULL, NULL, NULL
};

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ProcObjCmd --
 *
 *	This object-based procedure is invoked to process the "proc" Tcl 
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result value.
 *
 * Side effects:
 *	A new procedure gets created.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_ProcObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    register Interp *iPtr = (Interp *) interp;
    Proc *procPtr;
    char *fullName;
    CONST char *procName, *procArgs, *procBody;
    Namespace *nsPtr, *altNsPtr, *cxtNsPtr;
    Tcl_Command cmd;
    Tcl_DString ds;

    if (objc != 4) {
	Tcl_WrongNumArgs(interp, 1, objv, "name args body");
	return TCL_ERROR;
    }

    /*
     * Determine the namespace where the procedure should reside. Unless
     * the command name includes namespace qualifiers, this will be the
     * current namespace.
     */

    fullName = TclGetString(objv[1]);
    TclGetNamespaceForQualName(interp, fullName, (Namespace *) NULL,
	    0, &nsPtr, &altNsPtr, &cxtNsPtr, &procName);

    if (nsPtr == NULL) {
	Tcl_AppendResult(interp, "can't create procedure \"", fullName,
		"\": unknown namespace", (char *) NULL);
	return TCL_ERROR;
    }
    if (procName == NULL) {
	Tcl_AppendResult(interp, "can't create procedure \"", fullName,
		"\": bad procedure name", (char *) NULL);
	return TCL_ERROR;
    }
    if ((nsPtr != iPtr->globalNsPtr)
	    && (procName != NULL) && (procName[0] == ':')) {
	Tcl_AppendResult(interp, "can't create procedure \"", procName,
		"\" in non-global namespace with name starting with \":\"",
		(char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Create the data structure to represent the procedure.
     */
    if (TclCreateProc(interp, nsPtr, procName, objv[2], objv[3],
	    &procPtr) != TCL_OK) {
	return TCL_ERROR;
    }

    /*
     * Now create a command for the procedure. This will initially be in
     * the current namespace unless the procedure's name included namespace
     * qualifiers. To create the new command in the right namespace, we
     * generate a fully qualified name for it.
     */

    Tcl_DStringInit(&ds);
    if (nsPtr != iPtr->globalNsPtr) {
	Tcl_DStringAppend(&ds, nsPtr->fullName, -1);
	Tcl_DStringAppend(&ds, "::", 2);
    }
    Tcl_DStringAppend(&ds, procName, -1);

    cmd = Tcl_CreateObjCommand(interp, Tcl_DStringValue(&ds),
	    TclObjInterpProc, (ClientData) procPtr, TclProcDeleteProc);

    Tcl_DStringFree(&ds);
    /*
     * Now initialize the new procedure's cmdPtr field. This will be used
     * later when the procedure is called to determine what namespace the
     * procedure will run in. This will be different than the current
     * namespace if the proc was renamed into a different namespace.
     */

    procPtr->cmdPtr = (Command *) cmd;

    /*
     * Optimize for no-op procs: if the body is not precompiled (like a TclPro
     * procbody), and the argument list is just "args" and the body is empty,
     * define a compileProc to compile a no-op.
     *
     * Notes: 
     *   - cannot be done for any argument list without having different
     *     compiled/not-compiled behaviour in the "wrong argument #" case, 
     *     or making this code much more complicated. In any case, it doesn't 
     *     seem to make a lot of sense to verify the number of arguments we 
     *     are about to ignore ...
     *   - could be enhanced to handle also non-empty bodies that contain 
     *     only comments; however, parsing the body will slow down the 
     *     compilation of all procs whose argument list is just _args_ */

    if (objv[3]->typePtr == &tclProcBodyType) {
	goto done;
    }

    procArgs = TclGetString(objv[2]);

    while (*procArgs == ' ') {
	procArgs++;
    }

    if ((procArgs[0] == 'a') && (strncmp(procArgs, "args", 4) == 0)) {
	procArgs +=4;
	while(*procArgs != '\0') {
	    if (*procArgs != ' ') {
		goto done;
	    }
	    procArgs++;
	}	

	/* 
	 * The argument list is just "args"; check the body
	 */

	procBody = TclGetString(objv[3]);
	while (*procBody != '\0') {
	    if (!isspace(UCHAR(*procBody))) {
		goto done;
	    }
	    procBody++;
	}	

	/* 
	 * The body is just spaces: link the compileProc
	 */

	((Command *) cmd)->compileProc = TclCompileNoOp;
    }

 done:
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * TclCreateProc --
 *
 *	Creates the data associated with a Tcl procedure definition.
 *	This procedure knows how to handle two types of body objects:
 *	strings and procbody. Strings are the traditional (and common) value
 *	for bodies, procbody are values created by extensions that have
 *	loaded a previously compiled script. 
 *
 * Results:
 *	Returns TCL_OK on success, along with a pointer to a Tcl
 *	procedure definition in procPtrPtr where the cmdPtr field is not
 *	initialised. This definition should be freed by calling
 *	TclProcCleanupProc() when it is no longer needed.  Returns TCL_ERROR if
 *	anything goes wrong. 
 *
 * Side effects:
 *	If anything goes wrong, this procedure returns an error
 *	message in the interpreter.
 *
 *----------------------------------------------------------------------
 */
int
TclCreateProc(interp, nsPtr, procName, argsPtr, bodyPtr, procPtrPtr)
    Tcl_Interp *interp;		/* interpreter containing proc */
    Namespace *nsPtr;		/* namespace containing this proc */
    CONST char *procName;	/* unqualified name of this proc */
    Tcl_Obj *argsPtr;		/* description of arguments */
    Tcl_Obj *bodyPtr;		/* command body */
    Proc **procPtrPtr;		/* returns:  pointer to proc data */
{
    Interp *iPtr = (Interp*)interp;
    CONST char **argArray = NULL;

    register Proc *procPtr;
    int i, length, result, numArgs;
    CONST char *args, *bytes, *p;
    register CompiledLocal *localPtr = NULL;
    Tcl_Obj *defPtr;
    int precompiled = 0;

    if (bodyPtr->typePtr == &tclProcBodyType) {
	/*
	 * Because the body is a TclProProcBody, the actual body is already
	 * compiled, and it is not shared with anyone else, so it's OK not to
	 * unshare it (as a matter of fact, it is bad to unshare it, because
	 * there may be no source code).
	 *
	 * We don't create and initialize a Proc structure for the procedure;
	 * rather, we use what is in the body object. We increment the ref
	 * count of the Proc struct since the command (soon to be created)
	 * will be holding a reference to it.
	 */

	procPtr = (Proc *) bodyPtr->internalRep.otherValuePtr;
	procPtr->iPtr = iPtr;
	procPtr->refCount++;
	precompiled = 1;
    } else {
	/*
	 * If the procedure's body object is shared because its string value is
	 * identical to, e.g., the body of another procedure, we must create a
	 * private copy for this procedure to use. Such sharing of procedure
	 * bodies is rare but can cause problems. A procedure body is compiled
	 * in a context that includes the number of compiler-allocated "slots"
	 * for local variables. Each formal parameter is given a local variable
	 * slot (the "procPtr->numCompiledLocals = numArgs" assignment
	 * below). This means that the same code can not be shared by two
	 * procedures that have a different number of arguments, even if their
	 * bodies are identical. Note that we don't use Tcl_DuplicateObj since
	 * we would not want any bytecode internal representation.
	 */

	if (Tcl_IsShared(bodyPtr)) {
	    bytes = Tcl_GetStringFromObj(bodyPtr, &length);
	    bodyPtr = Tcl_NewStringObj(bytes, length);
	}

	/*
	 * Create and initialize a Proc structure for the procedure. We
	 * increment the ref count of the procedure's body object since there
	 * will be a reference to it in the Proc structure.
	 */

	Tcl_IncrRefCount(bodyPtr);

	procPtr = (Proc *) ckalloc(sizeof(Proc));
	procPtr->iPtr = iPtr;
	procPtr->refCount = 1;
	procPtr->bodyPtr = bodyPtr;
	procPtr->numArgs = 0;	/* actual argument count is set below. */
	procPtr->numCompiledLocals = 0;
	procPtr->firstLocalPtr = NULL;
	procPtr->lastLocalPtr = NULL;
    }

    /*
     * Break up the argument list into argument specifiers, then process
     * each argument specifier.
     * If the body is precompiled, processing is limited to checking that
     * the parsed argument is consistent with the one stored in the
     * Proc.
     * THIS FAILS IF THE ARG LIST OBJECT'S STRING REP CONTAINS NULLS.
     */

    args = Tcl_GetStringFromObj(argsPtr, &length);
    result = Tcl_SplitList(interp, args, &numArgs, &argArray);
    if (result != TCL_OK) {
	goto procError;
    }

    if (precompiled) {
	if (numArgs > procPtr->numArgs) {
	    char buf[40 + TCL_INTEGER_SPACE + TCL_INTEGER_SPACE];
	    sprintf(buf, "%d entries, precompiled header expects %d",
		    numArgs, procPtr->numArgs);
	    Tcl_AppendResult(interp, "procedure \"", procName,
		    "\": arg list contains ", buf, NULL);
	    goto procError;
	}
	localPtr = procPtr->firstLocalPtr;
    } else {
	procPtr->numArgs = numArgs;
	procPtr->numCompiledLocals = numArgs;
    }

    for (i = 0; i < numArgs; i++) {
	int fieldCount, nameLength, valueLength;
	CONST char **fieldValues;

	/*
	 * Now divide the specifier up into name and default.
	 */

	result = Tcl_SplitList(interp, argArray[i], &fieldCount,
		&fieldValues);
	if (result != TCL_OK) {
	    goto procError;
	}
	if (fieldCount > 2) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp,
		    "too many fields in argument specifier \"",
		    argArray[i], "\"", (char *) NULL);
	    goto procError;
	}
	if ((fieldCount == 0) || (*fieldValues[0] == 0)) {
	    ckfree((char *) fieldValues);
	    Tcl_AppendResult(interp, "procedure \"", procName,
		    "\" has argument with no name", (char *) NULL);
	    goto procError;
	}

	nameLength = strlen(fieldValues[0]);
	if (fieldCount == 2) {
	    valueLength = strlen(fieldValues[1]);
	} else {
	    valueLength = 0;
	}

	/*
	 * Check that the formal parameter name is a scalar.
	 */

	p = fieldValues[0];
	while (*p != '\0') {
	    if (*p == '(') {
		CONST char *q = p;
		do {
		    q++;
		} while (*q != '\0');
		q--;
		if (*q == ')') { /* we have an array element */
		    Tcl_AppendResult(interp, "procedure \"", procName,
			    "\" has formal parameter \"", fieldValues[0],
			    "\" that is an array element", (char *) NULL);
		    ckfree((char *) fieldValues);
		    goto procError;
		}
	    } else if ((*p == ':') && (*(p+1) == ':')) {
		Tcl_AppendResult(interp, "procedure \"", procName,
			"\" has formal parameter \"", fieldValues[0],
			"\" that is not a simple name", (char *) NULL);
		ckfree((char *) fieldValues);
		goto procError;
	    }
	    p++;
	}

	if (precompiled) {
	    /*
	     * Compare the parsed argument with the stored one.
	     * For the flags, we and out VAR_UNDEFINED to support bridging
	     * precompiled <= 8.3 code in 8.4 where this is now used as an
	     * optimization indicator.	Yes, this is a hack. -- hobbs
	     */

	    if ((localPtr->nameLength != nameLength)
		    || (strcmp(localPtr->name, fieldValues[0]))
		    || (localPtr->frameIndex != i)
		    || ((localPtr->flags & ~VAR_UNDEFINED)
			    != (VAR_SCALAR | VAR_ARGUMENT))
		    || (localPtr->defValuePtr == NULL && fieldCount == 2)
		    || (localPtr->defValuePtr != NULL && fieldCount != 2)) {
		char buf[40 + TCL_INTEGER_SPACE];

		ckfree((char *) fieldValues);
		sprintf(buf, "%d is inconsistent with precompiled body", i);
		Tcl_AppendResult(interp, "procedure \"", procName,
			"\": formal parameter ", buf, (char *) NULL);
		goto procError;
	    }

	    /*
	     * compare the default value if any
	     */

	    if (localPtr->defValuePtr != NULL) {
		int tmpLength;
		char *tmpPtr = Tcl_GetStringFromObj(localPtr->defValuePtr,
			&tmpLength);
		if ((valueLength != tmpLength) ||
			strncmp(fieldValues[1], tmpPtr, (size_t) tmpLength)) {
		    Tcl_AppendResult(interp, "procedure \"", procName,
			    "\": formal parameter \"", fieldValues[0],
			    "\" has default value inconsistent with precompiled body",
			    (char *) NULL);
		    ckfree((char *) fieldValues);
		    goto procError;
		}
		if ((i == numArgs - 1)
			&& (localPtr->nameLength == 4)
			&& (localPtr->name[0] == 'a')
			&& (strcmp(localPtr->name, "args") == 0)) {
		    localPtr->flags |= VAR_IS_ARGS;
		}
	    }

	    localPtr = localPtr->nextPtr;
	} else {
	    /*
	     * Allocate an entry in the runtime procedure frame's array of
	     * local variables for the argument. 
	     */

	    localPtr = (CompiledLocal *) ckalloc((unsigned) 
		    (sizeof(CompiledLocal) - sizeof(localPtr->name)
			    + nameLength + 1));
	    if (procPtr->firstLocalPtr == NULL) {
		procPtr->firstLocalPtr = procPtr->lastLocalPtr = localPtr;
	    } else {
		procPtr->lastLocalPtr->nextPtr = localPtr;
		procPtr->lastLocalPtr = localPtr;
	    }
	    localPtr->nextPtr = NULL;
	    localPtr->nameLength = nameLength;
	    localPtr->frameIndex = i;
	    localPtr->flags = VAR_SCALAR | VAR_ARGUMENT;
	    localPtr->resolveInfo = NULL;

	    if (fieldCount == 2) {
		localPtr->defValuePtr =
			Tcl_NewStringObj(fieldValues[1], valueLength);
		Tcl_IncrRefCount(localPtr->defValuePtr);
	    } else {
		localPtr->defValuePtr = NULL;
	    }
	    strcpy(localPtr->name, fieldValues[0]);
	    if ((i == numArgs - 1)
		    && (localPtr->nameLength == 4)
		    && (localPtr->name[0] == 'a')
		    && (strcmp(localPtr->name, "args") == 0)) {
		localPtr->flags |= VAR_IS_ARGS;
	    }
	}

	ckfree((char *) fieldValues);
    }

    *procPtrPtr = procPtr;
    ckfree((char *) argArray);
    return TCL_OK;

procError:
    if (precompiled) {
	procPtr->refCount--;
    } else {
	Tcl_DecrRefCount(bodyPtr);
	while (procPtr->firstLocalPtr != NULL) {
	    localPtr = procPtr->firstLocalPtr;
	    procPtr->firstLocalPtr = localPtr->nextPtr;

	    defPtr = localPtr->defValuePtr;
	    if (defPtr != NULL) {
		Tcl_DecrRefCount(defPtr);
	    }

	    ckfree((char *) localPtr);
	}
	ckfree((char *) procPtr);
    }
    if (argArray != NULL) {
	ckfree((char *) argArray);
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetFrame --
 *
 *	Given a description of a procedure frame, such as the first
 *	argument to an "uplevel" or "upvar" command, locate the
 *	call frame for the appropriate level of procedure.
 *
 * Results:
 *	The return value is -1 if an error occurred in finding the frame
 *	(in this case an error message is left in the interp's result).
 *	1 is returned if string was either a number or a number preceded
 *	by "#" and it specified a valid frame.  0 is returned if string
 *	isn't one of the two things above (in this case, the lookup
 *	acts as if string were "1").  The variable pointed to by
 *	framePtrPtr is filled in with the address of the desired frame
 *	(unless an error occurs, in which case it isn't modified).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclGetFrame(interp, name, framePtrPtr)
    Tcl_Interp *interp;		/* Interpreter in which to find frame. */
    CONST char *name;		/* String describing frame. */
    CallFrame **framePtrPtr;	/* Store pointer to frame here (or NULL
				 * if global frame indicated). */
{
    register Interp *iPtr = (Interp *) interp;
    int curLevel, level, result;
    CallFrame *framePtr;

    /*
     * Parse string to figure out which level number to go to.
     */

    result = 1;
    curLevel = (iPtr->varFramePtr == NULL) ? 0 : iPtr->varFramePtr->level;
    if (*name== '#') {
	if (Tcl_GetInt(interp, name+1, &level) != TCL_OK || level < 0) {
	    goto levelError;
	}
    } else if (isdigit(UCHAR(*name))) { /* INTL: digit */
	if (Tcl_GetInt(interp, name, &level) != TCL_OK) {
	    goto levelError;
	}
	level = curLevel - level;
    } else {
	level = curLevel - 1;
	result = 0;
    }

    /* Figure out which frame to use, and return it to the caller */

    if (level == 0) {
	framePtr = NULL;
    } else {
	for (framePtr = iPtr->varFramePtr; framePtr != NULL;
		framePtr = framePtr->callerVarPtr) {
	    if (framePtr->level == level) {
		break;
	    }
	}
	if (framePtr == NULL) {
	    goto levelError;
	}
    }
    *framePtrPtr = framePtr;
    return result;

 levelError:
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "bad level \"", name, "\"", (char *) NULL);
    return -1;
}

/*
 *----------------------------------------------------------------------
 *
 * TclObjGetFrame --
 *
 *	Given a description of a procedure frame, such as the first
 *	argument to an "uplevel" or "upvar" command, locate the
 *	call frame for the appropriate level of procedure.
 *
 * Results:
 *	The return value is -1 if an error occurred in finding the frame
 *	(in this case an error message is left in the interp's result).
 *	1 is returned if objPtr was either a number or a number preceded
 *	by "#" and it specified a valid frame.  0 is returned if objPtr
 *	isn't one of the two things above (in this case, the lookup
 *	acts as if objPtr were "1").  The variable pointed to by
 *	framePtrPtr is filled in with the address of the desired frame
 *	(unless an error occurs, in which case it isn't modified).
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclObjGetFrame(interp, objPtr, framePtrPtr)
    Tcl_Interp *interp;		/* Interpreter in which to find frame. */
    Tcl_Obj *objPtr;		/* Object describing frame. */
    CallFrame **framePtrPtr;	/* Store pointer to frame here (or NULL
				 * if global frame indicated). */
{
    register Interp *iPtr = (Interp *) interp;
    int curLevel, level, result;
    CallFrame *framePtr;
    CONST char *name = TclGetString(objPtr);

    /*
     * Parse object to figure out which level number to go to.
     */

    result = 1;
    curLevel = (iPtr->varFramePtr == NULL) ? 0 : iPtr->varFramePtr->level;
    if (objPtr->typePtr == &tclLevelReferenceType) {
	if ((int) objPtr->internalRep.twoPtrValue.ptr1) {
	    level = curLevel - (int) objPtr->internalRep.twoPtrValue.ptr2;
	} else {
	    level = (int) objPtr->internalRep.twoPtrValue.ptr2;
	}
	if (level < 0) {
	    goto levelError;
	}
    } else if (objPtr->typePtr == &tclIntType ||
	    objPtr->typePtr == &tclWideIntType) {
	if (Tcl_GetIntFromObj(NULL, objPtr, &level) != TCL_OK || level < 0) {
	    goto levelError;
	}
	level = curLevel - level;
    } else {
	if (*name == '#') {
	    if (Tcl_GetInt(interp, name+1, &level) != TCL_OK || level < 0) {
		goto levelError;
	    }
	    /*
	     * Cache for future reference.
	     */
	    TclFreeIntRep(objPtr);
	    objPtr->typePtr = &tclLevelReferenceType;
	    objPtr->internalRep.twoPtrValue.ptr1 = (VOID *) 0;
	    objPtr->internalRep.twoPtrValue.ptr2 = (VOID *) level;
	} else if (isdigit(UCHAR(*name))) { /* INTL: digit */
	    if (Tcl_GetInt(interp, name, &level) != TCL_OK) {
		return -1;
	    }
	    /*
	     * Cache for future reference.
	     */
	    TclFreeIntRep(objPtr);
	    objPtr->typePtr = &tclLevelReferenceType;
	    objPtr->internalRep.twoPtrValue.ptr1 = (VOID *) 1;
	    objPtr->internalRep.twoPtrValue.ptr2 = (VOID *) level;
	    level = curLevel - level;
	} else {
	    /*
	     * Don't cache as the object *isn't* a level reference.
	     */
	    level = curLevel - 1;
	    result = 0;
	}
    }

    /* Figure out which frame to use, and return it to the caller */

    if (level == 0) {
	framePtr = NULL;
    } else {
	for (framePtr = iPtr->varFramePtr; framePtr != NULL;
		framePtr = framePtr->callerVarPtr) {
	    if (framePtr->level == level) {
		break;
	    }
	}
	if (framePtr == NULL) {
	    goto levelError;
	}
    }
    *framePtrPtr = framePtr;
    return result;

levelError:
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, "bad level \"", name, "\"", (char *) NULL);
    return -1;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_UplevelObjCmd --
 *
 *	This object procedure is invoked to process the "uplevel" Tcl
 *	command. See the user documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl object result value.
 *
 * Side effects:
 *	See the user documentation.
 *
 *----------------------------------------------------------------------
 */

	/* ARGSUSED */
int
Tcl_UplevelObjCmd(dummy, interp, objc, objv)
    ClientData dummy;		/* Not used. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int objc;			/* Number of arguments. */
    Tcl_Obj *CONST objv[];	/* Argument objects. */
{
    register Interp *iPtr = (Interp *) interp;
    int result;
    CallFrame *savedVarFramePtr, *framePtr;

    if (objc < 2) {
	uplevelSyntax:
	Tcl_WrongNumArgs(interp, 1, objv, "?level? command ?arg ...?");
	return TCL_ERROR;
    }

    /*
     * Find the level to use for executing the command.
     */

    result = TclObjGetFrame(interp, objv[1], &framePtr);
    if (result == -1) {
	return TCL_ERROR;
    }
    objc -= (result+1);
    if (objc == 0) {
	goto uplevelSyntax;
    }
    objv += (result+1);

    /*
     * Modify the interpreter state to execute in the given frame.
     */

    savedVarFramePtr = iPtr->varFramePtr;
    iPtr->varFramePtr = framePtr;

    /*
     * Execute the residual arguments as a command.
     */

    if (objc == 1) {
	result = Tcl_EvalObjEx(interp, objv[0], TCL_EVAL_DIRECT);
    } else {
	/*
	 * More than one argument: concatenate them together with spaces
	 * between, then evaluate the result.  Tcl_EvalObjEx will delete
	 * the object when it decrements its refcount after eval'ing it.
	 */
	Tcl_Obj *objPtr;

	objPtr = Tcl_ConcatObj(objc, objv);
	result = Tcl_EvalObjEx(interp, objPtr, TCL_EVAL_DIRECT);
    }
    if (result == TCL_ERROR) {
	char msg[32 + TCL_INTEGER_SPACE];
	sprintf(msg, "\n    (\"uplevel\" body line %d)", interp->errorLine);
	Tcl_AddObjErrorInfo(interp, msg, -1);
    }

    /*
     * Restore the variable frame, and return.
     */

    iPtr->varFramePtr = savedVarFramePtr;
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclFindProc --
 *
 *	Given the name of a procedure, return a pointer to the
 *	record describing the procedure. The procedure will be
 *	looked up using the usual rules: first in the current
 *	namespace and then in the global namespace.
 *
 * Results:
 *	NULL is returned if the name doesn't correspond to any
 *	procedure. Otherwise, the return value is a pointer to
 *	the procedure's record. If the name is found but refers
 *	to an imported command that points to a "real" procedure
 *	defined in another namespace, a pointer to that "real"
 *	procedure's structure is returned.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclFindProc(iPtr, procName)
    Interp *iPtr;		/* Interpreter in which to look. */
    CONST char *procName;	/* Name of desired procedure. */
{
    Tcl_Command cmd;
    Tcl_Command origCmd;
    Command *cmdPtr;

    cmd = Tcl_FindCommand((Tcl_Interp *) iPtr, procName,
	    (Tcl_Namespace *) NULL, /*flags*/ 0);
    if (cmd == (Tcl_Command) NULL) {
	return NULL;
    }
    cmdPtr = (Command *) cmd;

    origCmd = TclGetOriginalCommand(cmd);
    if (origCmd != NULL) {
	cmdPtr = (Command *) origCmd;
    }
    if (cmdPtr->objProc != TclObjInterpProc) {
	return NULL;
    }
    return (Proc *) cmdPtr->objClientData;
}

/*
 *----------------------------------------------------------------------
 *
 * TclIsProc --
 *
 *	Tells whether a command is a Tcl procedure or not.
 *
 * Results:
 *	If the given command is actually a Tcl procedure, the
 *	return value is the address of the record describing
 *	the procedure.  Otherwise the return value is 0.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Proc *
TclIsProc(cmdPtr)
    Command *cmdPtr;		/* Command to test. */
{
    Tcl_Command origCmd;

    origCmd = TclGetOriginalCommand((Tcl_Command) cmdPtr);
    if (origCmd != NULL) {
	cmdPtr = (Command *) origCmd;
    }
    if (cmdPtr->objProc == TclObjInterpProc) {
	return (Proc *) cmdPtr->objClientData;
    }
    return (Proc *) 0;
}

/*
 *----------------------------------------------------------------------
 *
 * InitCompiledLocals --
 *
 *	This routine is invoked in order to initialize the compiled
 *	locals table for a new call frame.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May invoke various name resolvers in order to determine which
 *	variables are being referenced at runtime.
 *
 *----------------------------------------------------------------------
 */

static void
InitCompiledLocals(interp, codePtr, localPtr, varPtr, nsPtr)
    Tcl_Interp *interp;		/* Current interpreter. */
    ByteCode *codePtr;
    CompiledLocal *localPtr;
    Var *varPtr;
    Namespace *nsPtr;		/* Pointer to current namespace. */
{
    Interp *iPtr = (Interp*) interp;
    int haveResolvers = (nsPtr->compiledVarResProc || iPtr->resolverPtr);
    CompiledLocal *firstLocalPtr;
    
    if (codePtr->flags & TCL_BYTECODE_RESOLVE_VARS) {
	/*
	 * This is the first run after a recompile, or else the resolver epoch
	 * has changed: update the resolver cache.
	 */

	firstLocalPtr = localPtr;
	for (; localPtr != NULL; localPtr = localPtr->nextPtr) {
	    
	    if (localPtr->resolveInfo) {
		if (localPtr->resolveInfo->deleteProc) {
		    localPtr->resolveInfo->deleteProc(localPtr->resolveInfo);
		} else {
		    ckfree((char*)localPtr->resolveInfo);
		}
		localPtr->resolveInfo = NULL;
	    }
	    localPtr->flags &= ~VAR_RESOLVED;
	    
	    if (haveResolvers &&
		    !(localPtr->flags & (VAR_ARGUMENT|VAR_TEMPORARY))) {
		ResolverScheme *resPtr = iPtr->resolverPtr;
		Tcl_ResolvedVarInfo *vinfo;
		int result;
		
		if (nsPtr->compiledVarResProc) {
		    result = (*nsPtr->compiledVarResProc)(nsPtr->interp,
			    localPtr->name, localPtr->nameLength,
			    (Tcl_Namespace *) nsPtr, &vinfo);
		} else {
		    result = TCL_CONTINUE;
		}
		
		while ((result == TCL_CONTINUE) && resPtr) {
		    if (resPtr->compiledVarResProc) {
			result = (*resPtr->compiledVarResProc)(nsPtr->interp,
				localPtr->name, localPtr->nameLength,
				(Tcl_Namespace *) nsPtr, &vinfo);
		    }
		    resPtr = resPtr->nextPtr;
		}
		if (result == TCL_OK) {
		    localPtr->resolveInfo = vinfo;
		    localPtr->flags |= VAR_RESOLVED;
		}		    
	    }	    
	}
	localPtr = firstLocalPtr;
	codePtr->flags &= ~TCL_BYTECODE_RESOLVE_VARS;
    }

    /*
     * Initialize the array of local variables stored in the call frame.
     * Some variables may have special resolution rules.  In that case,
     * we call their "resolver" procs to get our hands on the variable,
     * and we make the compiled local a link to the real variable.
     */

    if (haveResolvers) {
	Tcl_ResolvedVarInfo *resVarInfo;
	for (; localPtr != NULL; varPtr++, localPtr = localPtr->nextPtr) {
	    varPtr->value.objPtr = NULL;
	    varPtr->name = localPtr->name; /* will be just '\0' if temp var */
	    varPtr->nsPtr = NULL;
	    varPtr->hPtr = NULL;
	    varPtr->refCount = 0;
	    varPtr->tracePtr = NULL;
	    varPtr->searchPtr = NULL;
	    varPtr->flags = localPtr->flags;
    
	    /*
	     * Now invoke the resolvers to determine the exact variables that
	     * should be used.
	     */
	    
	    resVarInfo = localPtr->resolveInfo;
	    if (resVarInfo && resVarInfo->fetchProc) {
		Var *resolvedVarPtr = (Var*) (*resVarInfo->fetchProc)(interp,
			resVarInfo);
		if (resolvedVarPtr) {
		    resolvedVarPtr->refCount++;
		    varPtr->value.linkPtr = resolvedVarPtr;
		    varPtr->flags = VAR_LINK;
		}
	    }
	}
    } else {
	for (; localPtr != NULL; varPtr++, localPtr = localPtr->nextPtr) {
	    varPtr->value.objPtr = NULL;
	    varPtr->name = localPtr->name; /* will be just '\0' if temp var */
	    varPtr->nsPtr = NULL;
	    varPtr->hPtr = NULL;
	    varPtr->refCount = 0;
	    varPtr->tracePtr = NULL;
	    varPtr->searchPtr = NULL;
	    varPtr->flags = localPtr->flags;
	}
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclInitCompiledLocals --
 *
 *	This routine is invoked in order to initialize the compiled
 *	locals table for a new call frame.
 *
 * DEPRECATED: functionality has been inlined elsewhere; this function remains
 * to insure binary compatibility with Itcl.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May invoke various name resolvers in order to determine which
 *	variables are being referenced at runtime.
 *
 *----------------------------------------------------------------------
 */

void
TclInitCompiledLocals(interp, framePtr, nsPtr)
    Tcl_Interp *interp;		/* Current interpreter. */
    CallFrame *framePtr;	/* Call frame to initialize. */
    Namespace *nsPtr;		/* Pointer to current namespace. */
{
    Var *varPtr = framePtr->compiledLocals;
    ByteCode *codePtr = (ByteCode *)
	    framePtr->procPtr->bodyPtr->internalRep.otherValuePtr;
    CompiledLocal *localPtr = framePtr->procPtr->firstLocalPtr;

    InitCompiledLocals(interp, codePtr, localPtr, varPtr, nsPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclObjInterpProc --
 *
 *	When a Tcl procedure gets invoked during bytecode evaluation, this 
 *	object-based routine gets invoked to interpret the procedure.
 *
 * Results:
 *	A standard Tcl object result value.
 *
 * Side effects:
 *	Depends on the commands in the procedure.
 *
 *----------------------------------------------------------------------
 */

int
TclObjInterpProc(clientData, interp, objc, objv)
    ClientData clientData; 	 /* Record describing procedure to be
				  * interpreted. */
    register Tcl_Interp *interp; /* Interpreter in which procedure was
				  * invoked. */
    int objc;			 /* Count of number of arguments to this
				  * procedure. */
    Tcl_Obj *CONST objv[];	 /* Argument value objects. */
{
    register Proc *procPtr = (Proc *) clientData;
    Namespace *nsPtr = procPtr->cmdPtr->nsPtr;
    CallFrame *framePtr, **framePtrPtr;
    register Var *varPtr;
    register CompiledLocal *localPtr;
    char *procName;
    int nameLen, localCt, numArgs, argCt, i, imax, result;
    Var *compiledLocals;

    /*
     * Get the procedure's name.
     */

    procName = Tcl_GetStringFromObj(objv[0], &nameLen);

    /*
     * If necessary, compile the procedure's body. The compiler will
     * allocate frame slots for the procedure's non-argument local
     * variables.  Note that compiling the body might increase
     * procPtr->numCompiledLocals if new local variables are found
     * while compiling.
     */

    result = TclProcCompileProc(interp, procPtr, procPtr->bodyPtr, nsPtr,
	    "body of proc", procName);

    if (result != TCL_OK) {
	return result;
    }


    /*
     * Set up and push a new call frame for the new procedure invocation.
     * This call frame will execute in the proc's namespace, which might
     * be different than the current namespace. The proc's namespace is
     * that of its command, which can change if the command is renamed
     * from one namespace to another.
     */

    framePtrPtr = &framePtr;
    result = TclPushStackFrame(interp, (Tcl_CallFrame **) framePtrPtr,
	    (Tcl_Namespace *) nsPtr, FRAME_IS_PROC);

    if (result != TCL_OK) {
	return result;
    }


    framePtr->objc = objc;
    framePtr->objv = objv;  /* ref counts for args are incremented below */
    framePtr->procPtr = procPtr;

    /*
     * Create the "compiledLocals" array. Make sure it is large enough to
     * hold all the procedure's compiled local variables, including its
     * formal parameters.
     */

    localCt = procPtr->numCompiledLocals;
    compiledLocals = (Var *) TclStackAlloc(interp, localCt*sizeof(Var));
    framePtr->numCompiledLocals = localCt;
    framePtr->compiledLocals = compiledLocals;

    /*
     * Match and assign the call's actual parameters to the procedure's
     * formal arguments. The formal arguments are described by the first
     * numArgs entries in both the Proc structure's local variable list and
     * the call frame's local variable array.
     */

    numArgs = procPtr->numArgs;
    argCt = objc-1; /* set it to the number of args to the proc */
    varPtr = framePtr->compiledLocals;
    localPtr = procPtr->firstLocalPtr;
    if (numArgs == 0) {
	if (argCt) {
	    goto incorrectArgs;
	} else {
	    goto runProc;
	}
    }    
    imax = ((argCt < numArgs - 1)? argCt : (numArgs - 1)); 
    for (i = 1; i <= imax; i++) {
	/*
	 * "Normal" arguments; last formal is special, depends on
	 * it being 'args'.
	 */	
	Tcl_Obj *objPtr = objv[i];
	varPtr->value.objPtr = objPtr;
	Tcl_IncrRefCount(objPtr);  /* local var is a reference */
	varPtr->name = localPtr->name;
	varPtr->nsPtr = NULL;
	varPtr->hPtr = NULL;
	varPtr->refCount = 0;
	varPtr->tracePtr = NULL;
	varPtr->searchPtr = NULL;
	varPtr->flags = localPtr->flags;
	varPtr++;
	localPtr = localPtr->nextPtr;
    }
    for (; i < numArgs; i++) {
	/*
	 * This loop is entered if argCt < (numArgs-1).
	 * Set default values; last formal is special.
	 */
	if (localPtr->defValuePtr != NULL) {
	    Tcl_Obj *objPtr = localPtr->defValuePtr;
	    varPtr->value.objPtr = objPtr;
	    Tcl_IncrRefCount(objPtr);  /* local var is a reference */
	    varPtr->name = localPtr->name;
	    varPtr->nsPtr = NULL;
	    varPtr->hPtr = NULL;
	    varPtr->refCount = 0;
	    varPtr->tracePtr = NULL;
	    varPtr->searchPtr = NULL;
	    varPtr->flags = localPtr->flags;
	    varPtr++;
	    localPtr = localPtr->nextPtr;
	} else {
	    goto incorrectArgs;
	}
    }

    /*
     * When we get here, the last formal argument remains
     * to be defined: localPtr and varPtr point to the last
     * argument to be initialized.
     */

    if (localPtr->flags & VAR_IS_ARGS) {
	Tcl_Obj *listPtr = Tcl_NewListObj(objc-numArgs, &(objv[numArgs]));
	varPtr->value.objPtr = listPtr;
	Tcl_IncrRefCount(listPtr); /* local var is a reference */
    } else if (argCt == numArgs) {
	Tcl_Obj *objPtr = objv[numArgs];
	varPtr->value.objPtr = objPtr;
	Tcl_IncrRefCount(objPtr);  /* local var is a reference */
    } else if ((argCt < numArgs) && (localPtr->defValuePtr != NULL)) {
	Tcl_Obj *objPtr = localPtr->defValuePtr;
	varPtr->value.objPtr = objPtr;
	Tcl_IncrRefCount(objPtr);  /* local var is a reference */
    } else {
	Tcl_Obj **desiredObjs, *argObj;	
	ByteCode *codePtr;		
    incorrectArgs:
	/*
	 * Do initialise all compiled locals, to avoid problems at
	 * DeleteLocalVars. 
	 */
	codePtr = (ByteCode *) procPtr->bodyPtr->internalRep.otherValuePtr;
	InitCompiledLocals(interp, codePtr, localPtr, varPtr, nsPtr);

        /*
	 * Build up desired argument list for Tcl_WrongNumArgs
	 */

	desiredObjs = (Tcl_Obj **)
		ckalloc(sizeof(Tcl_Obj *) * (unsigned)(numArgs+1));
#ifdef AVOID_HACKS_FOR_ITCL
	desiredObjs[0] = objv[0];
#else
	desiredObjs[0] = Tcl_NewListObj(1, objv);
#endif /* AVOID_HACKS_FOR_ITCL */
	localPtr = procPtr->firstLocalPtr;
	for (i=1 ; i<=numArgs ; i++) {
	    TclNewObj(argObj);
	    if (localPtr->defValuePtr != NULL) {
		Tcl_AppendStringsToObj(argObj,
			"?", localPtr->name, "?", (char *) NULL);
	    } else if ((i==numArgs) && !strcmp(localPtr->name, "args")) {
		Tcl_AppendStringsToObj(argObj, "...", (char *) NULL);
	    } else {
		Tcl_AppendStringsToObj(argObj, localPtr->name, (char *) NULL);
	    }
	    desiredObjs[i] = argObj;
	    localPtr = localPtr->nextPtr;
	}

	Tcl_ResetResult(interp);
	Tcl_WrongNumArgs(interp, numArgs+1, desiredObjs, NULL);
	result = TCL_ERROR;

#ifdef AVOID_HACKS_FOR_ITCL
	for (i=1 ; i<=numArgs ; i++) {
	    TclDecrRefCount(desiredObjs[i]);
	}
#else
	for (i=0 ; i<=numArgs ; i++) {
	    TclDecrRefCount(desiredObjs[i]);
	}
#endif /* AVOID_HACKS_FOR_ITCL */
	ckfree((char *) desiredObjs);
	goto procDone;
    }

    varPtr->name = localPtr->name;
    varPtr->nsPtr = NULL;
    varPtr->hPtr = NULL;
    varPtr->refCount = 0;
    varPtr->tracePtr = NULL;
    varPtr->searchPtr = NULL;
    varPtr->flags = localPtr->flags;

    localPtr = localPtr->nextPtr;
    varPtr++;

  runProc:
    /*
     * Initialise and resolve the remaining compiledLocals.
     */

    if (localPtr) {
	ByteCode *codePtr = (ByteCode *) procPtr->bodyPtr->internalRep.otherValuePtr;		
	InitCompiledLocals(interp, codePtr,
		localPtr, varPtr, nsPtr);
    }

    /*
     * Invoke the commands in the procedure's body.
     */

#ifdef TCL_COMPILE_DEBUG
    if (tclTraceExec >= 1) {
	fprintf(stdout, "Calling proc ");
	for (i = 0; i < objc; i++) {
	    TclPrintObject(stdout, objv[i], 15);
	    fprintf(stdout, " ");
	}
	fprintf(stdout, "\n");
	fflush(stdout);
    }
#endif /*TCL_COMPILE_DEBUG*/

    procPtr->refCount++;
    result = TclCompEvalObj(interp, procPtr->bodyPtr);
    procPtr->refCount--;
    if (procPtr->refCount <= 0) {
	TclProcCleanupProc(procPtr);
    }

    if (result != TCL_OK) {
	result = ProcessProcResultCode(interp, procName, nameLen, result);
    }

    /*
     * Pop and free the call frame for this procedure invocation, then
     * free the compiledLocals array if malloc'ed storage was used.
     */

    procDone:
    /*
     * Free the stack-allocated compiled locals and CallFrame. It is important
     * to pop the call frame without freeing it first: the compiledLocals
     * cannot be freed before the frame is popped, as the local variables must
     * be deleted. But the compiledLocals must be freed first, as they were
     * allocated later on the stack.
     */
    
    Tcl_PopCallFrame(interp); /* pop but do not free */
    TclStackFree(interp);     /* free compiledLocals */
    TclStackFree(interp);     /* free CallFrame      */
    return result;
#undef NUM_LOCALS
}

/*
 *----------------------------------------------------------------------
 *
 * TclProcCompileProc --
 *
 *	Called just before a procedure is executed to compile the
 *	body to byte codes.  If the type of the body is not
 *	"byte code" or if the compile conditions have changed
 *	(namespace context, epoch counters, etc.) then the body
 *	is recompiled.  Otherwise, this procedure does nothing.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May change the internal representation of the body object
 *	to compiled code.
 *
 *----------------------------------------------------------------------
 */

int
TclProcCompileProc(interp, procPtr, bodyPtr, nsPtr, description, procName)
    Tcl_Interp *interp;		/* Interpreter containing procedure. */
    Proc *procPtr;		/* Data associated with procedure. */
    Tcl_Obj *bodyPtr;		/* Body of proc. (Usually procPtr->bodyPtr,
 				 * but could be any code fragment compiled
 				 * in the context of this procedure.) */
    Namespace *nsPtr;		/* Namespace containing procedure. */
    CONST char *description;	/* string describing this body of code. */
    CONST char *procName;	/* Name of this procedure. */
{
    Interp *iPtr = (Interp*)interp;
    int result;
    Tcl_CallFrame *framePtr;
    Proc *saveProcPtr;
    ByteCode *codePtr = (ByteCode *) bodyPtr->internalRep.otherValuePtr;

    /*
     * If necessary, compile the procedure's body. The compiler will
     * allocate frame slots for the procedure's non-argument local
     * variables. If the ByteCode already exists, make sure it hasn't been
     * invalidated by someone redefining a core command (this might make the
     * compiled code wrong). Also, if the code was compiled in/for a
     * different interpreter, we recompile it. Note that compiling the body
     * might increase procPtr->numCompiledLocals if new local variables are
     * found while compiling.
     *
     * Precompiled procedure bodies, however, are immutable and therefore
     * they are not recompiled, even if things have changed.
     */

    if (bodyPtr->typePtr == &tclByteCodeType) {
 	if (((Interp *) *codePtr->interpHandle != iPtr)
		|| (codePtr->compileEpoch != iPtr->compileEpoch)
		|| (codePtr->nsPtr != nsPtr)) {
	    if (codePtr->flags & TCL_BYTECODE_PRECOMPILED) {
		if ((Interp *) *codePtr->interpHandle != iPtr) {
		    Tcl_AppendResult(interp,
			    "a precompiled script jumped interps", NULL);
		    return TCL_ERROR;
		}
		codePtr->compileEpoch = iPtr->compileEpoch;
		codePtr->nsPtr = nsPtr;
	    } else {
		bodyPtr->typePtr->freeIntRepProc(bodyPtr);
		bodyPtr->typePtr = (Tcl_ObjType *) NULL;
	    }
 	}
    }
    if (bodyPtr->typePtr != &tclByteCodeType) {
#ifdef TCL_COMPILE_DEBUG
 	if (tclTraceCompile >= 1) {
 	    /*
 	     * Display a line summarizing the top level command we
 	     * are about to compile.
 	     */
	    Tcl_Obj *message = Tcl_NewStringObj("Compiling ", -1);
	    Tcl_IncrRefCount(message);
	    Tcl_AppendStringsToObj(message, description, " \"", NULL);
	    TclAppendLimitedToObj(message, procName, -1, 50, NULL);
 	    fprintf(stdout, "%s\"\n", TclGetString(message));
	    Tcl_DecrRefCount(message);
 	}
#endif

 	/*
 	 * Plug the current procPtr into the interpreter and coerce
 	 * the code body to byte codes.  The interpreter needs to
 	 * know which proc it's compiling so that it can access its
 	 * list of compiled locals.
 	 *
 	 * TRICKY NOTE:  Be careful to push a call frame with the
 	 *   proper namespace context, so that the byte codes are
 	 *   compiled in the appropriate class context.
 	 */

 	saveProcPtr = iPtr->compiledProcPtr;
 	iPtr->compiledProcPtr = procPtr;

 	result = TclPushStackFrame(interp, &framePtr,
		(Tcl_Namespace*)nsPtr, /* isProcCallFrame */ 0);

 	if (result == TCL_OK) {
	    result = tclByteCodeType.setFromAnyProc(interp, bodyPtr);
	    TclPopStackFrame(interp);
	}

 	iPtr->compiledProcPtr = saveProcPtr;

 	if (result != TCL_OK) {
 	    if (result == TCL_ERROR) {
		Tcl_Obj *errorLine = Tcl_NewIntObj(interp->errorLine);
		Tcl_Obj *message =
			Tcl_NewStringObj("\n    (compiling ", -1);
		Tcl_IncrRefCount(message);
		Tcl_AppendStringsToObj(message, description, " \"", NULL);
		TclAppendLimitedToObj(message, procName, -1, 50, NULL);
		Tcl_AppendToObj(message, "\", line ", -1);
		Tcl_AppendObjToObj(message, errorLine);
		Tcl_DecrRefCount(errorLine);
		Tcl_AppendToObj(message, ")", -1);
 		TclAppendObjToErrorInfo(interp, message);
		Tcl_DecrRefCount(message);
	    }
 	    return result;
 	}
    } else if (codePtr->nsEpoch != nsPtr->resolverEpoch) {
	/*
	 * The resolver epoch has changed, but we only need to invalidate
	 * the resolver cache.
	 */

	codePtr->flags |= TCL_BYTECODE_RESOLVE_VARS;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcessProcResultCode --
 *
 *	Procedure called by TclObjInterpProc to process a return code other
 *	than TCL_OK returned by a Tcl procedure.
 *
 * Results:
 *	Depending on the argument return code, the result returned is
 *	another return code and the interpreter's result is set to a value
 *	to supplement that return code.
 *
 * Side effects:
 *	If the result returned is TCL_ERROR, traceback information about
 *	the procedure just executed is appended to the interpreter's
 *	errorInfo field.
 *
 *----------------------------------------------------------------------
 */

static int
ProcessProcResultCode(interp, procName, nameLen, returnCode)
    Tcl_Interp *interp;		/* The interpreter in which the procedure
				 * was called and returned returnCode. */
    char *procName;		/* Name of the procedure. Used for error
				 * messages and trace information. */
    int nameLen;		/* Number of bytes in procedure's name. */
    int returnCode;		/* The unexpected result code. */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_Obj *message, *errorLine;

    if (returnCode == TCL_OK) {
	return TCL_OK;
    }
    if ((returnCode > TCL_CONTINUE) || (returnCode < TCL_OK)) {
	return returnCode;
    }
    if (returnCode == TCL_RETURN) {
	return TclUpdateReturnInfo(iPtr);
    } 
    if (returnCode != TCL_ERROR) {
	Tcl_ResetResult(interp);
	Tcl_AppendResult(interp, "invoked \"",
		((returnCode == TCL_BREAK) ? "break" : "continue"),
		"\" outside of a loop", NULL);
    }
    errorLine = Tcl_NewIntObj(interp->errorLine);
    message = Tcl_NewStringObj("\n    (procedure \"", -1);
    Tcl_IncrRefCount(message);
    TclAppendLimitedToObj(message, procName, nameLen, 60, NULL);
    Tcl_AppendToObj(message, "\" line ", -1);
    Tcl_AppendObjToObj(message, errorLine);
    Tcl_DecrRefCount(errorLine);
    Tcl_AppendToObj(message, ")", -1);
    TclAppendObjToErrorInfo(interp, message);
    Tcl_DecrRefCount(message);
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TclProcDeleteProc --
 *
 *	This procedure is invoked just before a command procedure is
 *	removed from an interpreter.  Its job is to release all the
 *	resources allocated to the procedure.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed, unless the procedure is actively being
 *	executed.  In this case the cleanup is delayed until the
 *	last call to the current procedure completes.
 *
 *----------------------------------------------------------------------
 */

void
TclProcDeleteProc(clientData)
    ClientData clientData;	/* Procedure to be deleted. */
{
    Proc *procPtr = (Proc *) clientData;

    procPtr->refCount--;
    if (procPtr->refCount <= 0) {
	TclProcCleanupProc(procPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclProcCleanupProc --
 *
 *	This procedure does all the real work of freeing up a Proc
 *	structure.  It's called only when the structure's reference
 *	count becomes zero.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory gets freed.
 *
 *----------------------------------------------------------------------
 */

void
TclProcCleanupProc(procPtr)
    register Proc *procPtr;	/* Procedure to be deleted. */
{
    register CompiledLocal *localPtr;
    Tcl_Obj *bodyPtr = procPtr->bodyPtr;
    Tcl_Obj *defPtr;
    Tcl_ResolvedVarInfo *resVarInfo;

    if (bodyPtr != NULL) {
	Tcl_DecrRefCount(bodyPtr);
    }
    for (localPtr = procPtr->firstLocalPtr; localPtr != NULL; ) {
	CompiledLocal *nextPtr = localPtr->nextPtr;

	resVarInfo = localPtr->resolveInfo;
	if (resVarInfo) {
	    if (resVarInfo->deleteProc) {
		(*resVarInfo->deleteProc)(resVarInfo);
	    } else {
		ckfree((char *) resVarInfo);
	    }
	}

	if (localPtr->defValuePtr != NULL) {
	    defPtr = localPtr->defValuePtr;
	    Tcl_DecrRefCount(defPtr);
	}
	ckfree((char *) localPtr);
	localPtr = nextPtr;
    }
    ckfree((char *) procPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclUpdateReturnInfo --
 *
 *	This procedure is called when procedures return, and at other
 *	points where the TCL_RETURN code is used.  It examines the
 *	returnLevel and returnCode to determine the real return status.
 *
 * Results:
 *	The return value is the true completion code to use for
 *	the procedure or script, instead of TCL_RETURN.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclUpdateReturnInfo(iPtr)
    Interp *iPtr;		/* Interpreter for which TCL_RETURN
				 * exception is being processed. */
{
    int code = TCL_RETURN;

    iPtr->returnLevel--;
    if (iPtr->returnLevel < 0) {
	Tcl_Panic("TclUpdateReturnInfo: negative return level");
    }
    if (iPtr->returnLevel == 0) {
	/* Now we've reached the level to return the requested -code */
	return iPtr->returnCode;
    }
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * TclGetObjInterpProc --
 *
 *	Returns a pointer to the TclObjInterpProc procedure; this is
 *	different from the value obtained from the TclObjInterpProc
 *	reference on systems like Windows where import and export
 *	versions of a procedure exported by a DLL exist.
 *
 * Results:
 *	Returns the internal address of the TclObjInterpProc procedure.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

TclObjCmdProcType
TclGetObjInterpProc()
{
    return (TclObjCmdProcType) TclObjInterpProc;
}

/*
 *----------------------------------------------------------------------
 *
 * TclNewProcBodyObj --
 *
 *	Creates a new object, of type "procbody", whose internal
 *	representation is the given Proc struct.  The newly created
 *	object's reference count is 0.
 *
 * Results:
 *	Returns a pointer to a newly allocated Tcl_Obj, 0 on error.
 *
 * Side effects:
 *	The reference count in the ByteCode attached to the Proc is
 *	bumped up by one, since the internal rep stores a pointer to
 *	it.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
TclNewProcBodyObj(procPtr)
    Proc *procPtr;		/* the Proc struct to store as the internal
				 * representation. */
{
    Tcl_Obj *objPtr;

    if (!procPtr) {
	return (Tcl_Obj *) NULL;
    }

    objPtr = Tcl_NewStringObj("", 0);

    if (objPtr) {
	objPtr->typePtr = &tclProcBodyType;
	objPtr->internalRep.otherValuePtr = (VOID *) procPtr;

	procPtr->refCount++;
    }

    return objPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcBodyDup --
 *
 *	Tcl_ObjType's Dup function for the proc body object.
 *	Bumps the reference count on the Proc stored in the internal
 *	representation.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Sets up the object in dupPtr to be a duplicate of the one in srcPtr.
 *
 *----------------------------------------------------------------------
 */

static void
ProcBodyDup(srcPtr, dupPtr)
    Tcl_Obj *srcPtr;		/* object to copy */
    Tcl_Obj *dupPtr;		/* target object for the duplication */
{
    Proc *procPtr = (Proc *) srcPtr->internalRep.otherValuePtr;

    dupPtr->typePtr = &tclProcBodyType;
    dupPtr->internalRep.otherValuePtr = (VOID *) procPtr;
    procPtr->refCount++;
}

/*
 *----------------------------------------------------------------------
 *
 * ProcBodyFree --
 *
 *	Tcl_ObjType's Free function for the proc body object.  The
 *	reference count on its Proc struct is decreased by 1; if the
 *	count reaches 0, the proc is freed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If the reference count on the Proc struct reaches 0, the
 *	struct is freed.
 *
 *----------------------------------------------------------------------
 */

static void
ProcBodyFree(objPtr)
    Tcl_Obj *objPtr;		/* the object to clean up */
{
    Proc *procPtr = (Proc *) objPtr->internalRep.otherValuePtr;
    procPtr->refCount--;
    if (procPtr->refCount <= 0) {
	TclProcCleanupProc(procPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclCompileNoOp --
 *
 *	Procedure called to compile no-op's
 *
 * Results:
 *	The return value is TCL_OK, indicating successful compilation.
 *
 * Side effects:
 *	Instructions are added to envPtr to execute a no-op at runtime.
 *
 *----------------------------------------------------------------------
 */

static int
TclCompileNoOp(interp, parsePtr, envPtr)
    Tcl_Interp *interp;		/* Used for error reporting. */
    Tcl_Parse *parsePtr;	/* Points to a parse structure for the
				 * command created by Tcl_ParseCommand. */
    CompileEnv *envPtr;		/* Holds resulting instructions. */
{
    Tcl_Token *tokenPtr;
    int i;
    int savedStackDepth = envPtr->currStackDepth;

    tokenPtr = parsePtr->tokenPtr;
    for(i = 1; i < parsePtr->numWords; i++) {
	tokenPtr = tokenPtr + tokenPtr->numComponents + 1;
	envPtr->currStackDepth = savedStackDepth;

	if (tokenPtr->type != TCL_TOKEN_SIMPLE_WORD) { 
	    TclCompileTokens(interp, tokenPtr+1, tokenPtr->numComponents,
		    envPtr);
	    TclEmitOpcode(INST_POP, envPtr);
	} 
    }
    envPtr->currStackDepth = savedStackDepth;
    TclEmitPush(TclRegisterLiteral(envPtr, "", 0, /*onHeap*/ 0), envPtr);
    return TCL_OK;
}
