/*
 * tclOODefineCmds.c --
 *
 *	This file contains the implementation of the ::oo::define command,
 *	part of the object-system core (NB: not Tcl_Obj, but ::oo)
 *
 * Copyright (c) 2006 by Donal K. Fellows
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclOODefineCmds.c,v 1.1.2.22 2006/09/30 22:41:03 dkf Exp $
 */

#include "tclInt.h"
#include "tclOO.h"

static Method *		CloneClassMethod(Tcl_Interp *interp, Class *clsPtr,
			    Method *mPtr, Tcl_Obj *namePtr);
static Method *		CloneObjectMethod(Tcl_Interp *interp, Object *oPtr,
			    Method *mPtr, Tcl_Obj *namePtr);
static Object *		GetDefineCmdContext(Tcl_Interp *interp);

int
TclOODefineObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    CallFrame *framePtr, **framePtrPtr;
    Foundation *fPtr = ((Interp *) interp)->ooFoundation;
    int result;
    Object *oPtr;

    if (objc < 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "objectName arg ?arg ...?");
	return TCL_ERROR;
    }

    oPtr = TclGetObjectFromObj(interp, objv[1]);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }

    /*
     * Make the oo::define namespace the current namespace and evaluate the
     * command(s).
     */

    /* This is needed to satisfy GCC 3.3's strict aliasing rules */
    framePtrPtr = &framePtr;
    result = TclPushStackFrame(interp, (Tcl_CallFrame **) framePtrPtr,
	    (Tcl_Namespace *) fPtr->defineNs, FRAME_IS_OO_DEFINE);
    if (result != TCL_OK) {
	return TCL_ERROR;
    }
    framePtr->ooContextPtr = oPtr;
    framePtr->objc = objc;
    framePtr->objv = objv;	/* Reference counts do not need to be
				 * incremented here. */

    if (objc == 3) {
	result = Tcl_EvalObjEx(interp, objv[2], 0);

	if (result == TCL_ERROR) {
	    int length;
	    const char *objName = Tcl_GetStringFromObj(objv[1], &length);
	    int limit = 200;
	    int overflow = (length > limit);

	    TclFormatToErrorInfo(interp,
		    "\n    (in definition script for object \"%.*s%s\" line %d)",
		    (overflow ? limit : length), objName,
		    (overflow ? "..." : ""), interp->errorLine);
	}
    } else {
	Tcl_Obj *objPtr, *obj2Ptr, **objs;
	Interp *iPtr = (Interp *) interp;
	Tcl_Command cmd;
	int dummy;

	/*
	 * More than one argument: fire them through the ensemble processing
	 * engine so that everything appears to be good and proper in error
	 * messages. Note that we cannot just concatenate and send through
	 * Tcl_EvalObjEx, as that doesn't do ensemble processing, and we
	 * cannot go through Tcl_EvalObjv without the extra work to pre-find
	 * the command, as that finds command names in the wrong namespace at
	 * the moment. Ugly!
	 */

	if (iPtr->ensembleRewrite.sourceObjs == NULL) {
	    iPtr->ensembleRewrite.sourceObjs = objv;
	    iPtr->ensembleRewrite.numRemovedObjs = 3;
	    iPtr->ensembleRewrite.numInsertedObjs = 1;
	} else {
	    int ni = iPtr->ensembleRewrite.numInsertedObjs;
	    if (ni < 3) {
		iPtr->ensembleRewrite.numRemovedObjs += 3 - ni;
	    } else {
		iPtr->ensembleRewrite.numInsertedObjs -= 2;
	    }
	}

	/*
	 * Build the list of arguments using a Tcl_Obj as a workspace. See
	 * comments above for why these contortions are necessary.
	 */

	TclNewObj(objPtr);
	TclNewObj(obj2Ptr);
	cmd = Tcl_FindCommand(interp, TclGetString(objv[2]), fPtr->defineNs,
		TCL_NAMESPACE_ONLY);
	if (cmd == NULL) {
	    /* punt this case! */
	    Tcl_AppendObjToObj(obj2Ptr, objv[2]);
	} else {
	    Tcl_GetCommandFullName(interp, cmd, obj2Ptr);
	}
	Tcl_ListObjAppendElement(NULL, objPtr, obj2Ptr);
	Tcl_ListObjReplace(NULL, objPtr, 1, 0, objc-3, objv+3);
	Tcl_ListObjGetElements(NULL, objPtr, &dummy, &objs);

	result = Tcl_EvalObjv(interp, objc-2, objs, TCL_EVAL_INVOKE);
	TclDecrRefCount(objPtr);
    }

    /*
     * Restore the previous "current" namespace.
     */

    TclPopStackFrame(interp);
    return result;
}

static Object *
GetDefineCmdContext(
    Tcl_Interp *interp)
{
    Interp *iPtr = (Interp *) interp;

    if ((iPtr->framePtr == NULL)
	    || (iPtr->framePtr->isProcCallFrame != FRAME_IS_OO_DEFINE)) {
	Tcl_AppendResult(interp, "this command may only be called from within "
		"the context of the ::oo::define command", NULL);
	return NULL;
    }
    return (Object *) iPtr->framePtr->ooContextPtr;
}

int
TclOODefineConstructorObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr;
    Class *clsPtr;
    int bodyLength;

    if (objc != 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "arguments body");
	return TCL_ERROR;
    }

    /*
     * Extract and validate the context, which is the class that we wish to
     * modify.
     */

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    if (oPtr->classPtr == NULL) {
	Tcl_AppendResult(interp, "only classes may have constructors defined",
		NULL);
	return TCL_ERROR;
    }
    clsPtr = oPtr->classPtr;

    (void) Tcl_GetStringFromObj(objv[2], &bodyLength);
    if (bodyLength > 0) {
	/*
	 * Create the method structure.
	 */

	Method *mPtr;

	mPtr = TclNewProcClassMethod(interp, clsPtr, 1, NULL, objv[1],
		objv[2]);
	if (mPtr == NULL) {
	    return TCL_ERROR;
	}

	/*
	 * Place the method structure in the class record. Note that we might
	 * not immediately delete the constructor as this might be being done
	 * during execution of the constructor itself.
	 */

	TclDeleteMethod(clsPtr->constructorPtr);
	clsPtr->constructorPtr = mPtr;
    } else {
	/*
	 * Delete the constructor method record and set the field in the class
	 * record to NULL.
	 */

	TclDeleteMethod(clsPtr->constructorPtr);
	clsPtr->constructorPtr = NULL;
    }

    return TCL_OK;
}

int
TclOODefineCopyObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr, *o2Ptr;
    FOREACH_HASH_DECLS;
    Method *mPtr;
    Tcl_Obj *keyPtr;
    int i;

    if (objc > 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "?targetName?");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }

    /*
     * Create a new object of the correct class. Note that constructors are
     * not called. Also note that we must resolve the object name ourselves
     * because we do not want to create the object in the current namespace,
     * but rather in the context of the namespace of the caller of the overall
     * [oo::define] command.
     */

    {
	char *name;
	Tcl_DString buffer;

	if (objc == 1) {
	    name = NULL;
	} else {
	    name = TclGetString(objv[1]);
	    Tcl_DStringInit(&buffer);
	    if (name[0]!=':' || name[1]!=':') {
		Interp *iPtr = (Interp *) interp;
		CallFrame *callerFramePtr = iPtr->varFramePtr->callerVarPtr;

		if (callerFramePtr != NULL) {
		    Tcl_DStringAppend(&buffer,
			    callerFramePtr->nsPtr->fullName, -1);
		}
		Tcl_DStringAppend(&buffer, "::", 2);
		Tcl_DStringAppend(&buffer, name, -1);
		name = Tcl_DStringValue(&buffer);
	    }
	}
	o2Ptr = TclOONewInstance(interp, oPtr->selfCls, name, -1, NULL, -1);
	if (name != NULL) {
	    Tcl_DStringFree(&buffer);
	}
    }
    if (o2Ptr == NULL) {
	return TCL_ERROR;
    }

    /*
     * Copy the methods, mixins and filters.
     */

    FOREACH_HASH(keyPtr, mPtr, &oPtr->methods) {
	(void) CloneObjectMethod(interp, o2Ptr, mPtr, keyPtr);
    }
    o2Ptr->mixins.num = oPtr->mixins.num;
    o2Ptr->mixins.list = (Class **) ckalloc(sizeof(Class*) * oPtr->mixins.num);
    memcpy(o2Ptr->mixins.list, oPtr->mixins.list,
	    sizeof(Class *) * oPtr->mixins.num);
    o2Ptr->filters.num = oPtr->filters.num;
    o2Ptr->filters.list = (Tcl_Obj **)
	    ckalloc(sizeof(Tcl_Obj *) * oPtr->filters.num);
    memcpy(o2Ptr->filters.list, oPtr->filters.list,
	    sizeof(Tcl_Obj *) * oPtr->filters.num);
    for (i=0 ; i<o2Ptr->filters.num ; i++) {
	Tcl_IncrRefCount(o2Ptr->filters.list[i]);
    }
    o2Ptr->flags = oPtr->flags & ~ROOT_OBJECT;

    /*
     * Copy the class, if present.
     */

    if (oPtr->classPtr) {
	Class *clsPtr = oPtr->classPtr;
	Class *cls2Ptr = o2Ptr->classPtr;
	Class *superPtr;

	cls2Ptr->flags = clsPtr->flags;
	FOREACH(superPtr, cls2Ptr->superclasses) {
	    TclOORemoveFromSubclasses(cls2Ptr, superPtr);
	}
	if (cls2Ptr->superclasses.num) {
	    cls2Ptr->superclasses.list = (Class **)
		    ckrealloc((char *) cls2Ptr->superclasses.list,
		    sizeof(Class *) * clsPtr->superclasses.num);
	} else {
	    cls2Ptr->superclasses.list = (Class **)
		    ckalloc(sizeof(Class *) * clsPtr->superclasses.num);
	}
	memcpy(cls2Ptr->superclasses.list, clsPtr->superclasses.list,
		sizeof(Class *) * clsPtr->superclasses.num);
	cls2Ptr->superclasses.num = clsPtr->superclasses.num;
	FOREACH(superPtr, cls2Ptr->superclasses) {
	    TclOOAddToSubclasses(cls2Ptr, superPtr);
	}

	cls2Ptr->filters.num = clsPtr->filters.num;
	cls2Ptr->filters.list = (Tcl_Obj **)
		ckalloc(sizeof(Tcl_Obj *) * clsPtr->filters.num);
	memcpy(cls2Ptr->filters.list, clsPtr->filters.list,
		sizeof(Tcl_Obj *) * clsPtr->filters.num);
	for (i=0 ; i<cls2Ptr->filters.num ; i++) {
	    Tcl_IncrRefCount(cls2Ptr->filters.list[i]);
	}

	FOREACH_HASH(keyPtr, mPtr, &clsPtr->classMethods) {
	    (void) CloneClassMethod(interp, cls2Ptr, mPtr, keyPtr);
	}
	if (clsPtr->constructorPtr) {
	    cls2Ptr->constructorPtr = CloneClassMethod(interp, cls2Ptr,
		    clsPtr->constructorPtr, NULL);
	}
	if (clsPtr->destructorPtr) {
	    cls2Ptr->destructorPtr = CloneClassMethod(interp, cls2Ptr,
		    clsPtr->destructorPtr, NULL);
	}
    }

    /*
     * Return the name of the cloned object.
     */

    Tcl_GetCommandFullName(interp, o2Ptr->command, Tcl_GetObjResult(interp));
    return TCL_OK;
}

static Method *
CloneObjectMethod(
    Tcl_Interp *interp,
    Object *oPtr,
    Method *mPtr,
    Tcl_Obj *namePtr)
{
    ClientData newClientData;

    if (mPtr->typePtr == NULL) {
	return (Method *) TclOONewMethod(interp, (Tcl_Object) oPtr, namePtr,
		mPtr->flags & PUBLIC_METHOD, NULL, NULL);
    } else if (mPtr->typePtr->clonePtr &&
	    mPtr->typePtr->clonePtr(mPtr->clientData,&newClientData)==TCL_OK) {
	return (Method *) TclOONewMethod(interp, (Tcl_Object) oPtr, namePtr,
		mPtr->flags & PUBLIC_METHOD, mPtr->typePtr, newClientData);
    } else {
	return NULL;
    }
}

static Method *
CloneClassMethod(
    Tcl_Interp *interp,
    Class *clsPtr,
    Method *mPtr,
    Tcl_Obj *namePtr)
{
    ClientData newClientData;

    if (mPtr->typePtr == NULL) {
	return (Method *) TclOONewClassMethod(interp, (Tcl_Class) clsPtr,
		namePtr, mPtr->flags & PUBLIC_METHOD, NULL, NULL);
    } else if (mPtr->typePtr->clonePtr &&
	    mPtr->typePtr->clonePtr(mPtr->clientData,&newClientData)==TCL_OK) {
	return (Method *) TclOONewClassMethod(interp, (Tcl_Class) clsPtr,
		namePtr, mPtr->flags & PUBLIC_METHOD, mPtr->typePtr,
		newClientData);
    } else {
	return NULL;
    }
}

int
TclOODefineDestructorObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr;
    Class *clsPtr;
    int bodyLength;

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "body");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    if (oPtr->classPtr == NULL) {
	Tcl_AppendResult(interp, "only classes may have destructors defined",
		NULL);
	return TCL_ERROR;
    }
    clsPtr = oPtr->classPtr;

    (void) Tcl_GetStringFromObj(objv[1], &bodyLength);
    if (bodyLength > 0) {
	/*
	 * Create the method structure.
	 */

	Method *mPtr;

	mPtr = TclNewProcClassMethod(interp, clsPtr, 1, NULL, NULL, objv[1]);
	if (mPtr == NULL) {
	    return TCL_ERROR;
	}

	/*
	 * Place the method structure in the class record. Note that we might
	 * not immediately delete the destructor as this might be being done
	 * during execution of the destructor itself.
	 */

	TclDeleteMethod(clsPtr->destructorPtr);
	clsPtr->destructorPtr = mPtr;
    } else {
	/*
	 * Delete the destructor method record and set the field in the class
	 * record to NULL.
	 */

	TclDeleteMethod(clsPtr->destructorPtr);
	clsPtr->destructorPtr = NULL;
    }

    return TCL_OK;
}

int
TclOODefineExportObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    int isSelfExport = (clientData != NULL);
    Object *oPtr;
    Method *mPtr;
    Tcl_HashEntry *hPtr;
    Class *clsPtr;
    int i, isNew;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "name ?name ...?");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    clsPtr = oPtr->classPtr;
    isSelfExport |= (clsPtr == NULL);

    for (i=1 ; i<objc ; i++) {
	if (isSelfExport) {
	    hPtr = Tcl_CreateHashEntry(&oPtr->methods, (char *) objv[i],
		    &isNew);
	} else {
	    hPtr = Tcl_CreateHashEntry(&clsPtr->classMethods, (char *) objv[i],
		    &isNew);
	}

	if (isNew) {
	    mPtr = (Method *) ckalloc(sizeof(Method));
	    memset(mPtr, 0, sizeof(Method));
	    Tcl_SetHashValue(hPtr, mPtr);
	} else {
	    mPtr = Tcl_GetHashValue(hPtr);
	}
	mPtr->flags |= PUBLIC_METHOD;
    }
    if (isSelfExport) {
	oPtr->epoch++;
    } else {
	((Interp *)interp)->ooFoundation->epoch++;
    }
    return TCL_OK;
}

int
TclOODefineFilterObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    int isSelfFilter = (clientData != NULL);
    Object *oPtr;
    int i;

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    isSelfFilter |= (oPtr->classPtr == NULL);

    if (!isSelfFilter) {
	if (oPtr->classPtr->filters.num) {
	    Tcl_Obj *filterObj;

	    FOREACH(filterObj, oPtr->classPtr->filters) {
		TclDecrRefCount(filterObj);
	    }
	}

	if (objc == 1) {
	    // deleting filters
	    ckfree((char *) oPtr->classPtr->filters.list);
	    oPtr->classPtr->filters.list = NULL;
	    oPtr->classPtr->filters.num = 0;
	} else {
	    // creating filters
	    Tcl_Obj **filters;

	    if (oPtr->classPtr->filters.num == 0) {
		filters = (Tcl_Obj **) ckalloc(sizeof(Tcl_Obj *) * (objc-1));
	    } else {
		filters = (Tcl_Obj **) ckrealloc(
			(char *) oPtr->classPtr->filters.list,
			sizeof(Tcl_Obj *) * (objc-1));
	    }
	    for (i=1 ; i<objc ; i++) {
		filters[i-1] = objv[i];
		Tcl_IncrRefCount(objv[i]);
	    }
	    oPtr->classPtr->filters.list = filters;
	    oPtr->classPtr->filters.num = objc-1;
	}
	// may be many objects affected
	((Interp *)interp)->ooFoundation->epoch++;
    } else {
	if (oPtr->filters.num) {
	    Tcl_Obj *filterObj;

	    FOREACH(filterObj, oPtr->filters) {
		TclDecrRefCount(filterObj);
	    }
	}
	if (objc == 1) {
	    // deleting filters
	    ckfree((char *) oPtr->filters.list);
	    oPtr->filters.list = NULL;
	    oPtr->filters.num = 0;
	} else {
	    // creating filters
	    Tcl_Obj **filters;

	    if (oPtr->filters.num == 0) {
		filters = (Tcl_Obj **) ckalloc(sizeof(Tcl_Obj *) * (objc-1));
	    } else {
		filters = (Tcl_Obj **) ckrealloc((char *) oPtr->filters.list,
			sizeof(Tcl_Obj *) * (objc-1));
	    }
	    for (i=1 ; i<objc ; i++) {
		filters[i-1] = objv[i];
		Tcl_IncrRefCount(objv[i]);
	    }
	    oPtr->filters.list = filters;
	    oPtr->filters.num = objc-1;
	}
	oPtr->epoch++; // per-object
    }
    return TCL_OK;
}

int
TclOODefineForwardObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    int isSelfForward = (clientData != NULL);
    Object *oPtr;
    Method *mPtr;
    int isPublic;
    Tcl_Obj *prefixObj;

    if (objc < 3) {
	Tcl_WrongNumArgs(interp, 1, objv, "name cmdName ?arg ...?");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    isSelfForward |= (oPtr->classPtr == NULL);
    isPublic = Tcl_StringMatch(TclGetString(objv[1]), "[a-z]*");

    /*
     * Create the method structure.
     */

    prefixObj = Tcl_NewListObj(objc-2, objv+2);
    if (isSelfForward) {
	mPtr = TclNewForwardMethod(interp, oPtr, isPublic, objv[1], prefixObj);
    } else {
	mPtr = TclNewForwardClassMethod(interp, oPtr->classPtr, isPublic,
		objv[1], prefixObj);
    }
    if (mPtr == NULL) {
	TclDecrRefCount(prefixObj);
	return TCL_ERROR;
    }
    return TCL_OK;
}

int
TclOODefineMethodObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    int isSelfMethod = (clientData != NULL);
    Object *oPtr;
    int bodyLength;

    if (objc != 4) {
	Tcl_WrongNumArgs(interp, 1, objv, "name args body");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    isSelfMethod |= (oPtr->classPtr == NULL);

    (void) Tcl_GetStringFromObj(objv[3], &bodyLength);
    if (bodyLength > 0) {
	/*
	 * Create the method structure.
	 */

	Method *mPtr;
	int isPublic = Tcl_StringMatch(TclGetString(objv[1]), "[a-z]*");

	if (isSelfMethod) {
	    mPtr = TclNewProcMethod(interp, oPtr, isPublic, objv[1], objv[2],
		    objv[3]);
	} else {
	    mPtr = TclNewProcClassMethod(interp, oPtr->classPtr, isPublic,
		    objv[1], objv[2], objv[3]);
	}
	if (mPtr == NULL) {
	    return TCL_ERROR;
	}
    } else {
	/*
	 * Delete the method structure from the appropriate hash table.
	 */

	Tcl_HashEntry *hPtr;

	if (isSelfMethod) {
	    hPtr = Tcl_FindHashEntry(&oPtr->methods, (char *)objv[1]);
	} else {
	    hPtr = Tcl_FindHashEntry(&oPtr->classPtr->classMethods,
		    (char *)objv[1]);
	}
	if (hPtr != NULL) {
	    Method *mPtr = (Method *) Tcl_GetHashValue(hPtr);

	    Tcl_DeleteHashEntry(hPtr);
	    TclDeleteMethod(mPtr);
	}
    }

    return TCL_OK;
}

int
TclOODefineMixinObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    const int objc,
    Tcl_Obj *const *objv)
{
    int isSelfMixin = (clientData != NULL);
    Object *oPtr = GetDefineCmdContext(interp);
    Class *mixinPtr;
    int i;

    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    isSelfMixin |= (oPtr->classPtr == NULL);

    if (!isSelfMixin) {
	Tcl_AppendResult(interp,
		"setting class-wide mixins not yet supported", NULL);
	return TCL_ERROR;
    }

    if (objc == 1) {
	if (oPtr->mixins.num != 0) {
	    FOREACH(mixinPtr, oPtr->mixins) {
		TclOORemoveFromInstances(oPtr, mixinPtr);
	    }
	    ckfree((char *) oPtr->mixins.list);
	    oPtr->mixins.num = 0;
	}
    } else {
	Class **mixins = (Class **) ckalloc(sizeof(Class *) * (objc-1));

	for (i=1 ; i<objc ; i++) {
	    Object *o2Ptr;

	    o2Ptr = TclGetObjectFromObj(interp, objv[i]);
	    if (o2Ptr == NULL) {
	    freeAndError:
		ckfree((char *) mixins);
		return TCL_ERROR;
	    }
	    if (o2Ptr->classPtr == NULL) {
		Tcl_AppendResult(interp, "may only mix in classes; \"",
			TclGetString(objv[i]), "\" is not a class", NULL);
		goto freeAndError;
	    }
	    mixins[i-1] = o2Ptr->classPtr;
	}
	if (oPtr->mixins.num != 0) {
	    FOREACH(mixinPtr, oPtr->mixins) {
		TclOORemoveFromInstances(oPtr, mixinPtr);
	    }
	    ckfree((char *) oPtr->mixins.list);
	}
	oPtr->mixins.num = objc-1;
	oPtr->mixins.list = mixins;
	FOREACH(mixinPtr, oPtr->mixins) {
	    TclOOAddToInstances(oPtr, mixinPtr);
	}
    }
    oPtr->epoch++;
    return TCL_OK;
}

#ifdef SUPPORT_OO_PARAMETERS
// Commented out; not sure whether we want to retain this in the core oo system
int
TclOODefineParameterObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr = GetDefineCmdContext(interp);

    if (oPtr == NULL) {
	return TCL_ERROR;
    }

    /*
     * Must nail down the semantics of this!
     */

    Tcl_AppendResult(interp, "TODO: not yet finished", NULL);
    return TCL_ERROR;
}
#endif

int
TclOODefineSelfClassObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr, *o2Ptr;
    Foundation *fPtr = ((Interp *)interp)->ooFoundation;

    /*
     * Parse the context to get the object to operate on.
     */

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    if (oPtr == fPtr->objectCls->thisPtr) {
	Tcl_AppendResult(interp,
		"may not modify the class of the root object", NULL);
	return TCL_ERROR;
    }
    if (oPtr == fPtr->classCls->thisPtr) {
	Tcl_AppendResult(interp,
		"may not modify the class of the class of classes", NULL);
	return TCL_ERROR;
    }

    /*
     * Parse the argument to get the class to set the object's class to.
     */

    if (objc != 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "className");
	return TCL_ERROR;
    }
    o2Ptr = TclGetObjectFromObj(interp, objv[1]);
    if (o2Ptr == NULL) {
	return TCL_ERROR;
    }
    if (o2Ptr->classPtr == NULL) {
	Tcl_AppendResult(interp, "the class of an object must be a class",
		NULL);
	return TCL_ERROR;
    }

    /*
     * Apply semantic checks. In particular, classes and non-classes are not
     * interchangable (too complicated to do the conversion!) so we must
     * produce an error if any attempt is made to swap from one to the other.
     */

    if ((oPtr->classPtr == NULL) == TclOOIsReachable(fPtr->classCls,
	    o2Ptr->classPtr)) {
	Tcl_AppendResult(interp, "may not change a ",
		(oPtr->classPtr==NULL ? "non-" : ""), "class object into a ",
		(oPtr->classPtr==NULL ? "" : "non-"), "class object", NULL);
	return TCL_ERROR;
    }

    /*
     * Set the object's class.
     */

    if (oPtr->selfCls != o2Ptr->classPtr) {
	TclOORemoveFromInstances(oPtr, oPtr->selfCls);
	oPtr->selfCls = o2Ptr->classPtr;
	TclOOAddToInstances(oPtr, oPtr->selfCls);
	if (oPtr->classPtr != NULL) {
	    fPtr->epoch++;
	} else {
	    oPtr->epoch++;
	}
    }
    return TCL_OK;
}

int
TclOODefineSuperclassObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    Object *oPtr, *o2Ptr;
    Foundation *fPtr = ((Interp *)interp)->ooFoundation;
    Class **superclasses, *superPtr;
    int i, j;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "className ?className ...?");
	return TCL_ERROR;
    }

    /*
     * Get the class to operate on.
     */

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    if (oPtr->classPtr == NULL) {
	Tcl_AppendResult(interp, "only classes may have superclasses defined",
		NULL);
	return TCL_ERROR;
    }
    if (oPtr == fPtr->objectCls->thisPtr) {
	Tcl_AppendResult(interp,
		"may not modify the superclass of the root object", NULL);
	return TCL_ERROR;
    }

    /*
     * Allocate some working space.
     */

    superclasses = (Class **) ckalloc(sizeof(Class *) * (objc-1));

    /*
     * Parse the arguments to get the class to use as superclasses.
     */

    for (i=0 ; i<objc-1 ; i++) {
	o2Ptr = TclGetObjectFromObj(interp, objv[i+1]);
	if (o2Ptr == NULL) {
	    goto failedAfterAlloc;
	}
	if (o2Ptr->classPtr == NULL) {
	    Tcl_AppendResult(interp, "only a class can be a superclass", NULL);
	    goto failedAfterAlloc;
	}
	for (j=0 ; j<i ; j++) {
	    if (superclasses[j] == o2Ptr->classPtr) {
		Tcl_AppendResult(interp,
			"class should only be a direct superclass once", NULL);
		goto failedAfterAlloc;
	    }
	}
	if (TclOOIsReachable(oPtr->classPtr, o2Ptr->classPtr)) {
	    Tcl_AppendResult(interp,
		    "attempt to form circular dependency graph", NULL);
	failedAfterAlloc:
	    ckfree((char *) superclasses);
	    return TCL_ERROR;
	}
	superclasses[i] = o2Ptr->classPtr;
    }

    /*
     * Install the list of superclasses into the class. Note that this also
     * involves splicing the class out of the superclasses' subclass list that
     * it used to be a member of and splicing it into the new superclasses'
     * subclass list.
     */

    if (oPtr->classPtr->superclasses.num != 0) {
	FOREACH(superPtr, oPtr->classPtr->superclasses) {
	    TclOORemoveFromSubclasses(oPtr->classPtr, superPtr);
	}
	ckfree((char *) oPtr->classPtr->superclasses.list);
    }
    oPtr->classPtr->superclasses.list = superclasses;
    oPtr->classPtr->superclasses.num = objc-1;
    FOREACH(superPtr, oPtr->classPtr->superclasses) {
	TclOOAddToSubclasses(oPtr->classPtr, superPtr);
    }
    fPtr->epoch++;

    return TCL_OK;
}

int
TclOODefineUnexportObjCmd(
    ClientData clientData,
    Tcl_Interp *interp,
    int objc,
    Tcl_Obj *const *objv)
{
    int isSelfUnexport = (clientData != NULL);
    Object *oPtr;
    Method *mPtr;
    Tcl_HashEntry *hPtr;
    Class *clsPtr;
    int i, isNew;

    if (objc < 2) {
	Tcl_WrongNumArgs(interp, 1, objv, "name ?name ...?");
	return TCL_ERROR;
    }

    oPtr = GetDefineCmdContext(interp);
    if (oPtr == NULL) {
	return TCL_ERROR;
    }
    clsPtr = oPtr->classPtr;
    isSelfUnexport |= (oPtr->classPtr == NULL);

    for (i=1 ; i<objc ; i++) {
	if (isSelfUnexport) {
	    hPtr = Tcl_CreateHashEntry(&oPtr->methods, (char *) objv[i],
		    &isNew);
	} else {
	    hPtr = Tcl_CreateHashEntry(&clsPtr->classMethods, (char *) objv[i],
		    &isNew);
	}

	if (isNew) {
	    mPtr = (Method *) ckalloc(sizeof(Method));
	    memset(mPtr, 0, sizeof(Method));
	    Tcl_SetHashValue(hPtr, mPtr);
	} else {
	    mPtr = Tcl_GetHashValue(hPtr);
	}
	mPtr->flags &= ~PUBLIC_METHOD;
    }
    if (isSelfUnexport) {
	oPtr->epoch++;
    } else {
	((Interp *)interp)->ooFoundation->epoch++;
    }
    return TCL_OK;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
