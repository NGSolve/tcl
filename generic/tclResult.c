/*
 * tclResult.c --
 *
 *	This file contains code to manage the interpreter result.
 *
 * Copyright (c) 1997 by Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tclInt.h"

/*
 * Indices of the standard return options dictionary keys.
 */

enum returnKeys {
    KEY_CODE,	KEY_ERRORCODE,	KEY_ERRORINFO,	KEY_ERRORLINE,
    KEY_LEVEL,	KEY_OPTIONS,	KEY_ERRORSTACK,	KEY_LAST
};

/*
 * Function prototypes for local functions in this file:
 */

static Tcl_Obj **	GetKeys(void);
static void		ReleaseKeys(ClientData clientData);
static void		ResetObjResult(Interp *iPtr);

/*
 * This structure is used to take a snapshot of the interpreter state in
 * Tcl_SaveInterpState. You can snapshot the state, execute a command, and
 * then back up to the result or the error that was previously in progress.
 */

typedef struct {
    int status;			/* return code status */
    int flags;			/* Each remaining field saves the */
    int returnLevel;		/* corresponding field of the Interp */
    int returnCode;		/* struct. These fields taken together are */
    Tcl_Obj *errorInfo;		/* the "state" of the interp. */
    Tcl_Obj *errorCode;
    Tcl_Obj *returnOpts;
    Tcl_Obj *objResult;
    Tcl_Obj *errorStack;
    int resetErrorStack;
} InterpState;

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SaveInterpState --
 *
 *	Fills a token with a snapshot of the current state of the interpreter.
 *	The snapshot can be restored at any point by TclRestoreInterpState.
 *
 *	The token returned must be eventally passed to one of the routines
 *	TclRestoreInterpState or TclDiscardInterpState, or there will be a
 *	memory leak.
 *
 * Results:
 *	Returns a token representing the interp state.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_InterpState
Tcl_SaveInterpState(
    Tcl_Interp *interp,		/* Interpreter's state to be saved */
    int status)			/* status code for current operation */
{
    Interp *iPtr = (Interp *) interp;
    InterpState *statePtr = ckalloc(sizeof(InterpState));

    statePtr->status = status;
    statePtr->flags = iPtr->flags & ERR_ALREADY_LOGGED;
    statePtr->returnLevel = iPtr->returnLevel;
    statePtr->returnCode = iPtr->returnCode;
    statePtr->errorInfo = iPtr->errorInfo;
    statePtr->errorStack = iPtr->errorStack;
    statePtr->resetErrorStack = iPtr->resetErrorStack;
    if (statePtr->errorInfo) {
	Tcl_IncrRefCount(statePtr->errorInfo);
    }
    statePtr->errorCode = iPtr->errorCode;
    if (statePtr->errorCode) {
	Tcl_IncrRefCount(statePtr->errorCode);
    }
    statePtr->returnOpts = iPtr->returnOpts;
    if (statePtr->returnOpts) {
	Tcl_IncrRefCount(statePtr->returnOpts);
    }
    if (statePtr->errorStack) {
	Tcl_IncrRefCount(statePtr->errorStack);
    }
    statePtr->objResult = Tcl_GetObjResult(interp);
    Tcl_IncrRefCount(statePtr->objResult);
    return (Tcl_InterpState) statePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_RestoreInterpState --
 *
 *	Accepts an interp and a token previously returned by
 *	Tcl_SaveInterpState. Restore the state of the interp to what it was at
 *	the time of the Tcl_SaveInterpState call.
 *
 * Results:
 *	Returns the status value originally passed in to Tcl_SaveInterpState.
 *
 * Side effects:
 *	Restores the interp state and frees memory held by token.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_RestoreInterpState(
    Tcl_Interp *interp,		/* Interpreter's state to be restored. */
    Tcl_InterpState state)	/* Saved interpreter state. */
{
    Interp *iPtr = (Interp *) interp;
    InterpState *statePtr = (InterpState *) state;
    int status = statePtr->status;

    iPtr->flags &= ~ERR_ALREADY_LOGGED;
    iPtr->flags |= (statePtr->flags & ERR_ALREADY_LOGGED);

    iPtr->returnLevel = statePtr->returnLevel;
    iPtr->returnCode = statePtr->returnCode;
    iPtr->resetErrorStack = statePtr->resetErrorStack;
    if (iPtr->errorInfo) {
	Tcl_DecrRefCount(iPtr->errorInfo);
    }
    iPtr->errorInfo = statePtr->errorInfo;
    if (iPtr->errorInfo) {
	Tcl_IncrRefCount(iPtr->errorInfo);
    }
    if (iPtr->errorCode) {
	Tcl_DecrRefCount(iPtr->errorCode);
    }
    iPtr->errorCode = statePtr->errorCode;
    if (iPtr->errorCode) {
	Tcl_IncrRefCount(iPtr->errorCode);
    }
    if (iPtr->errorStack) {
	Tcl_DecrRefCount(iPtr->errorStack);
    }
    iPtr->errorStack = statePtr->errorStack;
    if (iPtr->errorStack) {
	Tcl_IncrRefCount(iPtr->errorStack);
    }
    if (iPtr->returnOpts) {
	Tcl_DecrRefCount(iPtr->returnOpts);
    }
    iPtr->returnOpts = statePtr->returnOpts;
    if (iPtr->returnOpts) {
	Tcl_IncrRefCount(iPtr->returnOpts);
    }
    Tcl_SetObjResult(interp, statePtr->objResult);
    Tcl_DiscardInterpState(state);
    return status;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DiscardInterpState --
 *
 *	Accepts a token previously returned by Tcl_SaveInterpState. Frees the
 *	memory it uses.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Frees memory.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DiscardInterpState(
    Tcl_InterpState state)	/* saved interpreter state */
{
    InterpState *statePtr = (InterpState *) state;

    if (statePtr->errorInfo) {
	Tcl_DecrRefCount(statePtr->errorInfo);
    }
    if (statePtr->errorCode) {
	Tcl_DecrRefCount(statePtr->errorCode);
    }
    if (statePtr->returnOpts) {
	Tcl_DecrRefCount(statePtr->returnOpts);
    }
    if (statePtr->errorStack) {
	Tcl_DecrRefCount(statePtr->errorStack);
    }
    Tcl_DecrRefCount(statePtr->objResult);
    ckfree(statePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetResult --
 *
 *	Arrange for "result" to be the Tcl return value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	interp->result is left pointing either to "result" or to a copy of it.
 *	Also, the object result is reset.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetResult(
    Tcl_Interp *interp,		/* Interpreter with which to associate the
				 * return value. */
    register char *result,	/* Value to be returned. If NULL, the result
				 * is set to an empty string. */
    Tcl_FreeProc *freeProc)	/* Gives information about the string:
				 * TCL_STATIC, TCL_VOLATILE, or the address of
				 * a Tcl_FreeProc such as free. */
{
    Tcl_SetObjResult(interp, Tcl_NewStringObj(result, -1));
    if (result == NULL || freeProc == NULL || freeProc == TCL_VOLATILE) {
	return;
    }
    if (freeProc == TCL_DYNAMIC) {
	ckfree(result);
    } else {
	(*freeProc)(result);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetStringResult --
 *
 *	Returns an interpreter's result value as a string.
 *
 * Results:
 *	The interpreter's result as a string.
 *
 * Side effects:
 *	If the string result is empty, the object result is moved to the
 *	string result, then the object result is reset.
 *
 *----------------------------------------------------------------------
 */

const char *
Tcl_GetStringResult(
    register Tcl_Interp *interp)/* Interpreter whose result to return. */
{
    Interp *iPtr = (Interp *) interp;

    return Tcl_GetString(iPtr->objResultPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetObjResult --
 *
 *	Arrange for objPtr to be an interpreter's result value.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	interp->objResultPtr is left pointing to the object referenced by
 *	objPtr. The object's reference count is incremented since there is now
 *	a new reference to it. The reference count for any old objResultPtr
 *	value is decremented. Also, the string result is reset.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetObjResult(
    Tcl_Interp *interp,		/* Interpreter with which to associate the
				 * return object value. */
    register Tcl_Obj *objPtr)	/* Tcl object to be returned. If NULL, the obj
				 * result is made an empty string object. */
{
    register Interp *iPtr = (Interp *) interp;
    register Tcl_Obj *oldObjResult = iPtr->objResultPtr;

    iPtr->objResultPtr = objPtr;
    Tcl_IncrRefCount(objPtr);	/* since interp result is a reference */

    /*
     * We wait until the end to release the old object result, in case we are
     * setting the result to itself.
     */

    TclDecrRefCount(oldObjResult);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetObjResult --
 *
 *	Returns an interpreter's result value as a Tcl object. The object's
 *	reference count is not modified; the caller must do that if it needs
 *	to hold on to a long-term reference to it.
 *
 * Results:
 *	The interpreter's result as an object.
 *
 * Side effects:
 *	If the interpreter has a non-empty string result, the result object is
 *	either empty or stale because some function set interp->result
 *	directly. If so, the string result is moved to the result object then
 *	the string result is reset.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
Tcl_GetObjResult(
    Tcl_Interp *interp)		/* Interpreter whose result to return. */
{
    register Interp *iPtr = (Interp *) interp;

    return iPtr->objResultPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendResultVA --
 *
 *	Append a variable number of strings onto the interpreter's result.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The result of the interpreter given by the first argument is extended
 *	by the strings in the va_list (up to a terminating NULL argument).
 *
 *	If the string result is non-empty, the object result forced to be a
 *	duplicate of it first. There will be a string result afterwards.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AppendResultVA(
    Tcl_Interp *interp,		/* Interpreter with which to associate the
				 * return value. */
    va_list argList)		/* Variable argument list. */
{
    Tcl_Obj *objPtr = Tcl_GetObjResult(interp);

    if (Tcl_IsShared(objPtr)) {
	objPtr = Tcl_DuplicateObj(objPtr);
    }
    Tcl_AppendStringsToObjVA(objPtr, argList);
    Tcl_SetObjResult(interp, objPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendResult --
 *
 *	Append a variable number of strings onto the interpreter's result.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The result of the interpreter given by the first argument is extended
 *	by the strings given by the second and following arguments (up to a
 *	terminating NULL argument).
 *
 *	If the string result is non-empty, the object result forced to be a
 *	duplicate of it first. There will be a string result afterwards.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AppendResult(
    Tcl_Interp *interp, ...)
{
    va_list argList;

    va_start(argList, interp);
    Tcl_AppendResultVA(interp, argList);
    va_end(argList);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppendElement --
 *
 *	Convert a string to a valid Tcl list element and append it to the
 *	result (which is ostensibly a list).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The result in the interpreter given by the first argument is extended
 *	with a list element converted from string. A separator space is added
 *	before the converted list element unless the current result is empty,
 *	contains the single character "{", or ends in " {".
 *
 *	If the string result is empty, the object result is moved to the
 *	string result, then the object result is reset.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AppendElement(
    Tcl_Interp *interp,		/* Interpreter whose result is to be
				 * extended. */
    const char *element)	/* String to convert to list element and add
				 * to result. */
{
    Interp *iPtr = (Interp *) interp;
    Tcl_Obj *elementPtr = Tcl_NewStringObj(element, -1);
    Tcl_Obj *listPtr = Tcl_NewListObj(1, &elementPtr);
    size_t length;
    const char *bytes;

    if (Tcl_IsShared(iPtr->objResultPtr)) {
	Tcl_SetObjResult(interp, Tcl_DuplicateObj(iPtr->objResultPtr));
    } 
    bytes = Tcl_GetStringFromObj(iPtr->objResultPtr, &length);
    if (TclNeedSpace(bytes, bytes+length)) {
	Tcl_AppendToObj(iPtr->objResultPtr, " ", 1);
    }
    Tcl_AppendObjToObj(iPtr->objResultPtr, listPtr);
    Tcl_DecrRefCount(listPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FreeResult --
 *
 *	This function frees up the memory associated with an interpreter's
 *	result, resetting the interpreter's result object.  Tcl_FreeResult is
 *	most commonly used when a function is about to replace one result
 *	value with another.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Frees the memory associated with interp's result but does not change
 *	any part of the error dictionary (i.e., the errorinfo and errorcode
 *	remain the same).
 *
 *----------------------------------------------------------------------
 */

void
Tcl_FreeResult(
    register Tcl_Interp *interp)/* Interpreter for which to free result. */
{
    register Interp *iPtr = (Interp *) interp;

    ResetObjResult(iPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ResetResult --
 *
 *	This function resets both the interpreter's string and object results.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	It resets the result object to an unshared empty object. It then
 *	restores the interpreter's string result area to its default
 *	initialized state, freeing up any memory that may have been allocated.
 *	It also clears any error information for the interpreter.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_ResetResult(
    register Tcl_Interp *interp)/* Interpreter for which to clear result. */
{
    register Interp *iPtr = (Interp *) interp;

    ResetObjResult(iPtr);
    if (iPtr->errorCode) {
	/* Legacy support */
	if (iPtr->flags & ERR_LEGACY_COPY) {
	    Tcl_ObjSetVar2(interp, iPtr->ecVar, NULL,
		    iPtr->errorCode, TCL_GLOBAL_ONLY);
	}
	Tcl_DecrRefCount(iPtr->errorCode);
	iPtr->errorCode = NULL;
    }
    if (iPtr->errorInfo) {
	/* Legacy support */
	if (iPtr->flags & ERR_LEGACY_COPY) {
	    Tcl_ObjSetVar2(interp, iPtr->eiVar, NULL,
		    iPtr->errorInfo, TCL_GLOBAL_ONLY);
	}
	Tcl_DecrRefCount(iPtr->errorInfo);
	iPtr->errorInfo = NULL;
    }
    iPtr->resetErrorStack = 1;
    iPtr->returnLevel = 1;
    iPtr->returnCode = TCL_OK;
    if (iPtr->returnOpts) {
	Tcl_DecrRefCount(iPtr->returnOpts);
	iPtr->returnOpts = NULL;
    }
    iPtr->flags &= ~(ERR_ALREADY_LOGGED | ERR_LEGACY_COPY);
}

/*
 *----------------------------------------------------------------------
 *
 * ResetObjResult --
 *
 *	Function used to reset an interpreter's Tcl result object.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Resets the interpreter's result object to an unshared empty string
 *	object with ref count one. It does not clear any error information in
 *	the interpreter.
 *
 *----------------------------------------------------------------------
 */

static void
ResetObjResult(
    register Interp *iPtr)	/* Points to the interpreter whose result
				 * object should be reset. */
{
    register Tcl_Obj *objResultPtr = iPtr->objResultPtr;

    if (Tcl_IsShared(objResultPtr)) {
	TclDecrRefCount(objResultPtr);
	TclNewObj(objResultPtr);
	Tcl_IncrRefCount(objResultPtr);
	iPtr->objResultPtr = objResultPtr;
    } else {
	if (objResultPtr->bytes != tclEmptyStringRep) {
	    if (objResultPtr->bytes) {
		ckfree(objResultPtr->bytes);
	    }
	    objResultPtr->bytes = tclEmptyStringRep;
	    objResultPtr->length = 0;
	}
	TclFreeIntRep(objResultPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetErrorCodeVA --
 *
 *	This function is called to record machine-readable information about
 *	an error that is about to be returned.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The errorCode field of the interp is modified to hold all of the
 *	arguments to this function, in a list form with each argument becoming
 *	one element of the list.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetErrorCodeVA(
    Tcl_Interp *interp,		/* Interpreter in which to set errorCode */
    va_list argList)		/* Variable argument list. */
{
    Tcl_Obj *errorObj = Tcl_NewObj();

    /*
     * Scan through the arguments one at a time, appending them to the
     * errorCode field as list elements.
     */

    while (1) {
	char *elem = va_arg(argList, char *);

	if (elem == NULL) {
	    break;
	}
	Tcl_ListObjAppendElement(NULL, errorObj,
                Tcl_NewStringObj(elem, TCL_STRLEN));
    }
    Tcl_SetObjErrorCode(interp, errorObj);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetErrorCode --
 *
 *	This function is called to record machine-readable information about
 *	an error that is about to be returned.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The errorCode field of the interp is modified to hold all of the
 *	arguments to this function, in a list form with each argument becoming
 *	one element of the list.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetErrorCode(
    Tcl_Interp *interp, ...)
{
    va_list argList;

    /*
     * Scan through the arguments one at a time, appending them to the
     * errorCode field as list elements.
     */

    va_start(argList, interp);
    Tcl_SetErrorCodeVA(interp, argList);
    va_end(argList);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetObjErrorCode --
 *
 *	This function is called to record machine-readable information about
 *	an error that is about to be returned. The caller should build a list
 *	object up and pass it to this routine.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The errorCode field of the interp is set to the new value.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetObjErrorCode(
    Tcl_Interp *interp,
    Tcl_Obj *errorObjPtr)
{
    Interp *iPtr = (Interp *) interp;

    if (iPtr->errorCode) {
	Tcl_DecrRefCount(iPtr->errorCode);
    }
    iPtr->errorCode = errorObjPtr;
    Tcl_IncrRefCount(iPtr->errorCode);
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetErrorLine --
 *
 *      Returns the line number associated with the current error.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_GetErrorLine(
    Tcl_Interp *interp)
{
    return ((Interp *) interp)->errorLine;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetErrorLine --
 *
 *      Sets the line number associated with the current error.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetErrorLine(
    Tcl_Interp *interp,
    int value)
{
    ((Interp *) interp)->errorLine = value;
}

/*
 *----------------------------------------------------------------------
 *
 * GetKeys --
 *
 *	Returns a Tcl_Obj * array of the standard keys used in the return
 *	options dictionary.
 *
 *	Broadly sharing one copy of these key values helps with both memory
 *	efficiency and dictionary lookup times.
 *
 * Results:
 *	A Tcl_Obj * array.
 *
 * Side effects:
 *	First time called in a thread, creates the keys (allocating memory)
 *	and arranges for their cleanup at thread exit.
 *
 *----------------------------------------------------------------------
 */

static Tcl_Obj **
GetKeys(void)
{
    static Tcl_ThreadDataKey returnKeysKey;
    Tcl_Obj **keys = Tcl_GetThreadData(&returnKeysKey,
	    (int) (KEY_LAST * sizeof(Tcl_Obj *)));

    if (keys[0] == NULL) {
	/*
	 * First call in this thread, create the keys...
	 */

	int i;

	TclNewLiteralStringObj(keys[KEY_CODE],	    "-code");
	TclNewLiteralStringObj(keys[KEY_ERRORCODE], "-errorcode");
	TclNewLiteralStringObj(keys[KEY_ERRORINFO], "-errorinfo");
	TclNewLiteralStringObj(keys[KEY_ERRORLINE], "-errorline");
	TclNewLiteralStringObj(keys[KEY_ERRORSTACK],"-errorstack");
	TclNewLiteralStringObj(keys[KEY_LEVEL],	    "-level");
	TclNewLiteralStringObj(keys[KEY_OPTIONS],   "-options");

	for (i = KEY_CODE; i < KEY_LAST; i++) {
	    Tcl_IncrRefCount(keys[i]);
	}

	/*
	 * ... and arrange for their clenaup.
	 */

	Tcl_CreateThreadExitHandler(ReleaseKeys, keys);
    }
    return keys;
}

/*
 *----------------------------------------------------------------------
 *
 * ReleaseKeys --
 *
 *	Called as a thread exit handler to cleanup return options dictionary
 *	keys.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Frees memory.
 *
 *----------------------------------------------------------------------
 */

static void
ReleaseKeys(
    ClientData clientData)
{
    Tcl_Obj **keys = clientData;
    int i;

    for (i = KEY_CODE; i < KEY_LAST; i++) {
	Tcl_DecrRefCount(keys[i]);
	keys[i] = NULL;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclProcessReturn --
 *
 *	Does the work of the [return] command based on the code, level, and
 *	returnOpts arguments. Note that the code argument must agree with the
 *	-code entry in returnOpts and the level argument must agree with the
 *	-level entry in returnOpts, as is the case for values returned from
 *	TclMergeReturnOptions.
 *
 * Results:
 *	Returns the return code the [return] command should return.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclProcessReturn(
    Tcl_Interp *interp,
    int code,
    int level,
    Tcl_Obj *returnOpts)
{
    Interp *iPtr = (Interp *) interp;
    Tcl_Obj *valuePtr;
    Tcl_Obj **keys = GetKeys();

    /*
     * Store the merged return options.
     */

    if (iPtr->returnOpts != returnOpts) {
	if (iPtr->returnOpts) {
	    Tcl_DecrRefCount(iPtr->returnOpts);
	}
	iPtr->returnOpts = returnOpts;
	Tcl_IncrRefCount(iPtr->returnOpts);
    }

    if (code == TCL_ERROR) {
	if (iPtr->errorInfo) {
	    Tcl_DecrRefCount(iPtr->errorInfo);
	    iPtr->errorInfo = NULL;
	}
	Tcl_DictObjGet(NULL, iPtr->returnOpts, keys[KEY_ERRORINFO],
                &valuePtr);
	if (valuePtr != NULL) {
	    size_t infoLen;

	    (void) TclGetStringFromObj(valuePtr, &infoLen);
	    if (infoLen) {
		iPtr->errorInfo = valuePtr;
		Tcl_IncrRefCount(iPtr->errorInfo);
		iPtr->flags |= ERR_ALREADY_LOGGED;
	    }
	}
	Tcl_DictObjGet(NULL, iPtr->returnOpts, keys[KEY_ERRORSTACK],
                &valuePtr);
	if (valuePtr != NULL) {
            size_t len, valueObjc;
            Tcl_Obj **valueObjv;

            if (Tcl_IsShared(iPtr->errorStack)) {
                Tcl_Obj *newObj;
                
                newObj = Tcl_DuplicateObj(iPtr->errorStack);
                Tcl_DecrRefCount(iPtr->errorStack);
                Tcl_IncrRefCount(newObj);
                iPtr->errorStack = newObj;
            }

            /*
             * List extraction done after duplication to avoid moving the rug
             * if someone does [return -errorstack [info errorstack]]
             */

            if (Tcl_ListObjGetElements(interp, valuePtr, &valueObjc,
                    &valueObjv) == TCL_ERROR) {
                return TCL_ERROR;
            }
            iPtr->resetErrorStack = 0;
            Tcl_ListObjLength(interp, iPtr->errorStack, &len);

            /*
             * Reset while keeping the list intrep as much as possible.
             */

            Tcl_ListObjReplace(interp, iPtr->errorStack, 0, len, valueObjc,
                    valueObjv);
 	}
	Tcl_DictObjGet(NULL, iPtr->returnOpts, keys[KEY_ERRORCODE],
                &valuePtr);
	if (valuePtr != NULL) {
	    Tcl_SetObjErrorCode(interp, valuePtr);
	} else {
	    Tcl_SetErrorCode(interp, "NONE", NULL);
	}

	Tcl_DictObjGet(NULL, iPtr->returnOpts, keys[KEY_ERRORLINE],
                &valuePtr);
	if (valuePtr != NULL) {
	    TclGetIntFromObj(NULL, valuePtr, &iPtr->errorLine);
	}
    }
    if (level != 0) {
	iPtr->returnLevel = level;
	iPtr->returnCode = code;
	return TCL_RETURN;
    }
    if (code == TCL_ERROR) {
	iPtr->flags |= ERR_LEGACY_COPY;
    }
    return code;
}

/*
 *----------------------------------------------------------------------
 *
 * TclMergeReturnOptions --
 *
 *	Parses, checks, and stores the options to the [return] command.
 *
 * Results:
 *	Returns TCL_ERROR if any of the option values are invalid. Otherwise,
 *	returns TCL_OK, and writes the returnOpts, code, and level values to
 *	the pointers provided.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclMergeReturnOptions(
    Tcl_Interp *interp,		/* Current interpreter. */
    size_t objc,		/* Number of arguments. */
    Tcl_Obj *const objv[],	/* Argument objects. */
    Tcl_Obj **optionsPtrPtr,	/* If not NULL, points to space for a (Tcl_Obj
				 * *) where the pointer to the merged return
				 * options dictionary should be written. */
    int *codePtr,		/* If not NULL, points to space where the
				 * -code value should be written. */
    int *levelPtr)		/* If not NULL, points to space where the
				 * -level value should be written. */
{
    int code = TCL_OK;
    int level = 1;
    Tcl_Obj *valuePtr;
    Tcl_Obj *returnOpts = Tcl_NewObj();
    Tcl_Obj **keys = GetKeys();

    for (;  objc > 1;  objv += 2, objc -= 2) {
	size_t optLen, compareLen;
	const char *opt = TclGetStringFromObj(objv[0], &optLen);
	const char *compare =
		TclGetStringFromObj(keys[KEY_OPTIONS], &compareLen);

	if ((optLen == compareLen) && (memcmp(opt, compare, optLen) == 0)) {
	    Tcl_DictSearch search;
	    int done = 0;
	    Tcl_Obj *keyPtr;
	    Tcl_Obj *dict = objv[1];

	nestedOptions:
	    if (TCL_ERROR == Tcl_DictObjFirst(NULL, dict, &search,
		    &keyPtr, &valuePtr, &done)) {
		/*
		 * Value is not a legal dictionary.
		 */

		Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                        "bad %s value: expected dictionary but got \"%s\"",
                        compare, TclGetString(objv[1])));
		Tcl_SetErrorCode(interp, "TCL", "RESULT", "ILLEGAL_OPTIONS",
			NULL);
		goto error;
	    }

	    while (!done) {
		Tcl_DictObjPut(NULL, returnOpts, keyPtr, valuePtr);
		Tcl_DictObjNext(&search, &keyPtr, &valuePtr, &done);
	    }

	    Tcl_DictObjGet(NULL, returnOpts, keys[KEY_OPTIONS], &valuePtr);
	    if (valuePtr != NULL) {
		dict = valuePtr;
		Tcl_DictObjRemove(NULL, returnOpts, keys[KEY_OPTIONS]);
		goto nestedOptions;
	    }

	} else {
	    Tcl_DictObjPut(NULL, returnOpts, objv[0], objv[1]);
	}
    }

    /*
     * Check for bogus -code value.
     */

    Tcl_DictObjGet(NULL, returnOpts, keys[KEY_CODE], &valuePtr);
    if (valuePtr != NULL) {
	if (TclGetCompletionCodeFromObj(interp, valuePtr,
                &code) == TCL_ERROR) {
	    goto error;
	}
	Tcl_DictObjRemove(NULL, returnOpts, keys[KEY_CODE]);
    }

    /*
     * Check for bogus -level value.
     */

    Tcl_DictObjGet(NULL, returnOpts, keys[KEY_LEVEL], &valuePtr);
    if (valuePtr != NULL) {
	if ((TCL_ERROR == TclGetIntFromObj(NULL, valuePtr, &level))
		|| (level < 0)) {
	    /*
	     * Value is not a legal level.
	     */

	    Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                    "bad -level value: expected non-negative integer but got"
                    " \"%s\"", TclGetString(valuePtr)));
	    Tcl_SetErrorCode(interp, "TCL", "RESULT", "ILLEGAL_LEVEL", NULL);
	    goto error;
	}
	Tcl_DictObjRemove(NULL, returnOpts, keys[KEY_LEVEL]);
    }

    /*
     * Check for bogus -errorcode value.
     */

    Tcl_DictObjGet(NULL, returnOpts, keys[KEY_ERRORCODE], &valuePtr);
    if (valuePtr != NULL) {
	size_t length;

	if (TCL_ERROR == Tcl_ListObjLength(NULL, valuePtr, &length)) {
	    /*
	     * Value is not a list, which is illegal for -errorcode.
	     */

	    Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                    "bad -errorcode value: expected a list but got \"%s\"",
                    TclGetString(valuePtr)));
	    Tcl_SetErrorCode(interp, "TCL", "RESULT", "ILLEGAL_ERRORCODE",
		    NULL);
	    goto error;
	}
    }

    /*
     * Check for bogus -errorstack value.
     */

    Tcl_DictObjGet(NULL, returnOpts, keys[KEY_ERRORSTACK], &valuePtr);
    if (valuePtr != NULL) {
	size_t length;

	if (TCL_ERROR == Tcl_ListObjLength(NULL, valuePtr, &length)) {
	    /*
	     * Value is not a list, which is illegal for -errorstack.
	     */

	    Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                    "bad -errorstack value: expected a list but got \"%s\"",
                    TclGetString(valuePtr)));
	    Tcl_SetErrorCode(interp, "TCL", "RESULT", "NONLIST_ERRORSTACK",
                    NULL);
	    goto error;
	}
        if (length % 2) {
            /*
             * Errorstack must always be an even-sized list
             */

	    Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                    "forbidden odd-sized list for -errorstack: \"%s\"",
		    TclGetString(valuePtr)));
	    Tcl_SetErrorCode(interp, "TCL", "RESULT",
                    "ODDSIZEDLIST_ERRORSTACK", NULL);
	    goto error;
        }
    }

    /*
     * Convert [return -code return -level X] to [return -code ok -level X+1]
     */

    if (code == TCL_RETURN) {
	level++;
	code = TCL_OK;
    }

    if (codePtr != NULL) {
	*codePtr = code;
    }
    if (levelPtr != NULL) {
	*levelPtr = level;
    }

    if (optionsPtrPtr == NULL) {
	/*
	 * Not passing back the options (?!), so clean them up.
	 */

	Tcl_DecrRefCount(returnOpts);
    } else {
	*optionsPtrPtr = returnOpts;
    }
    return TCL_OK;

  error:
    Tcl_DecrRefCount(returnOpts);
    return TCL_ERROR;
}

/*
 *-------------------------------------------------------------------------
 *
 * Tcl_GetReturnOptions --
 *
 *	Packs up the interp state into a dictionary of return options.
 *
 * Results:
 *	A dictionary of return options.
 *
 * Side effects:
 *	None.
 *
 *-------------------------------------------------------------------------
 */

Tcl_Obj *
Tcl_GetReturnOptions(
    Tcl_Interp *interp,
    int result)
{
    Interp *iPtr = (Interp *) interp;
    Tcl_Obj *options;
    Tcl_Obj **keys = GetKeys();

    if (iPtr->returnOpts) {
	options = Tcl_DuplicateObj(iPtr->returnOpts);
    } else {
	options = Tcl_NewObj();
    }

    if (result == TCL_RETURN) {
	Tcl_DictObjPut(NULL, options, keys[KEY_CODE],
		Tcl_NewIntObj(iPtr->returnCode));
	Tcl_DictObjPut(NULL, options, keys[KEY_LEVEL],
		Tcl_NewIntObj(iPtr->returnLevel));
    } else {
	Tcl_DictObjPut(NULL, options, keys[KEY_CODE],
		Tcl_NewIntObj(result));
	Tcl_DictObjPut(NULL, options, keys[KEY_LEVEL],
		Tcl_NewIntObj(0));
    }

    if (result == TCL_ERROR) {
	Tcl_AddErrorInfo(interp, "");
        Tcl_DictObjPut(NULL, options, keys[KEY_ERRORSTACK], iPtr->errorStack);
    }
    if (iPtr->errorCode) {
	Tcl_DictObjPut(NULL, options, keys[KEY_ERRORCODE], iPtr->errorCode);
    }
    if (iPtr->errorInfo) {
	Tcl_DictObjPut(NULL, options, keys[KEY_ERRORINFO], iPtr->errorInfo);
	Tcl_DictObjPut(NULL, options, keys[KEY_ERRORLINE],
		Tcl_NewIntObj(iPtr->errorLine));
    }
    return options;
}

/*
 *-------------------------------------------------------------------------
 *
 * TclNoErrorStack --
 *
 *	Removes the -errorstack entry from an options dict to avoid reference
 *	cycles.
 *
 * Results:
 *	The (unshared) argument options dict, modified in -place.
 *
 *-------------------------------------------------------------------------
 */

Tcl_Obj *
TclNoErrorStack(
    Tcl_Interp *interp,
    Tcl_Obj *options)
{
    Tcl_Obj **keys = GetKeys();
    
    Tcl_DictObjRemove(interp, options, keys[KEY_ERRORSTACK]);
    return options;
}

/*
 *-------------------------------------------------------------------------
 *
 * Tcl_SetReturnOptions --
 *
 *	Accepts an interp and a dictionary of return options, and sets the
 *	return options of the interp to match the dictionary.
 *
 * Results:
 *	A standard status code. Usually TCL_OK, but TCL_ERROR if an invalid
 *	option value was found in the dictionary. If a -level value of 0 is in
 *	the dictionary, then the -code value in the dictionary will be
 *	returned (TCL_OK default).
 *
 * Side effects:
 *	Sets the state of the interp.
 *
 *-------------------------------------------------------------------------
 */

int
Tcl_SetReturnOptions(
    Tcl_Interp *interp,
    Tcl_Obj *options)
{
    int level, code;
    size_t objc;
    Tcl_Obj **objv, *mergedOpts;

    Tcl_IncrRefCount(options);
    if (TCL_ERROR == TclListObjGetElements(interp, options, &objc, &objv)
	    || (objc % 2)) {
	Tcl_SetObjResult(interp, Tcl_ObjPrintf(
                "expected dict but got \"%s\"", TclGetString(options)));
	Tcl_SetErrorCode(interp, "TCL", "RESULT", "ILLEGAL_OPTIONS", NULL);
	code = TCL_ERROR;
    } else if (TCL_ERROR == TclMergeReturnOptions(interp, objc, objv,
	    &mergedOpts, &code, &level)) {
	code = TCL_ERROR;
    } else {
	code = TclProcessReturn(interp, code, level, mergedOpts);
    }

    Tcl_DecrRefCount(options);
    return code;
}

/*
 *-------------------------------------------------------------------------
 *
 * Tcl_TransferResult --
 *
 *	Copy the result (and error information) from one interp to another.
 *	Used when one interp has caused another interp to evaluate a script
 *	and then wants to transfer the results back to itself.
 *
 *	This routine copies the string reps of the result and error
 *	information. It does not simply increment the refcounts of the result
 *	and error information objects themselves. It is not legal to exchange
 *	objects between interps, because an object may be kept alive by one
 *	interp, but have an internal rep that is only valid while some other
 *	interp is alive.
 *
 * Results:
 *	The target interp's result is set to a copy of the source interp's
 *	result. The source's errorInfo field may be transferred to the
 *	target's errorInfo field, and the source's errorCode field may be
 *	transferred to the target's errorCode field.
 *
 * Side effects:
 *	None.
 *
 *-------------------------------------------------------------------------
 */

void
Tcl_TransferResult(
    Tcl_Interp *sourceInterp,	/* Interp whose result and error information
				 * should be moved to the target interp.
				 * After moving result, this interp's result
				 * is reset. */
    int result,			/* TCL_OK if just the result should be copied,
				 * TCL_ERROR if both the result and error
				 * information should be copied. */
    Tcl_Interp *targetInterp)	/* Interp where result and error information
				 * should be stored. If source and target are
				 * the same, nothing is done. */
{
    Interp *tiPtr = (Interp *) targetInterp;
    Interp *siPtr = (Interp *) sourceInterp;

    if (sourceInterp == targetInterp) {
	return;
    }

    if (result == TCL_OK && siPtr->returnOpts == NULL) {
	/*
	 * Special optimization for the common case of normal command return
	 * code and no explicit return options.
	 */

	if (tiPtr->returnOpts) {
	    Tcl_DecrRefCount(tiPtr->returnOpts);
	    tiPtr->returnOpts = NULL;
	}
    } else {
	Tcl_SetReturnOptions(targetInterp,
		Tcl_GetReturnOptions(sourceInterp, result));
	tiPtr->flags &= ~(ERR_ALREADY_LOGGED);
    }
    Tcl_SetObjResult(targetInterp, Tcl_GetObjResult(sourceInterp));
    Tcl_ResetResult(sourceInterp);
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * tab-width: 8
 * indent-tabs-mode: nil
 * End:
 */
