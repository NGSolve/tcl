/*
 * tclOO.h --
 *
 *	This file contains the public API definitions and some of the function
 *	declarations for the object-system (NB: not Tcl_Obj, but ::oo).
 *
 * Copyright (c) 2006-2010 by Donal K. Fellows
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef TCLOO_H_INCLUDED
#define TCLOO_H_INCLUDED
#include "tcl.h"

#ifndef TCLOOAPI
#   if defined(BUILD_tcl) || defined(BUILD_TclOO)
#	define TCLOOAPI MODULE_SCOPE
#   else
#	define TCLOOAPI extern
#	undef USE_TCLOO_STUBS
#	define USE_TCLOO_STUBS 1
#   endif
#endif

extern const char *TclOOInitializeStubs(
	Tcl_Interp *, const char *version);
#define Tcl_OOInitStubs(interp) TclOOInitializeStubs((interp), TCLOO_VERSION)

/*
 * Be careful when it comes to versioning; need to make sure that the
 * standalone TclOO version matches. Also make sure that this matches the
 * version in the files:
 *
 * tests/oo.test
 * tests/ooNext2.test
 * unix/tclooConfig.sh
 * win/tclooConfig.sh
 */

#define TCLOO_VERSION "0.7"
#define TCLOO_PATCHLEVEL TCLOO_VERSION

/*
 * These are opaque types.
 */

typedef struct Tcl_Class_ *Tcl_Class;
typedef struct Tcl_Method_ *Tcl_Method;
typedef struct Tcl_Object_ *Tcl_Object;
typedef struct Tcl_ObjectContext_ *Tcl_ObjectContext;

/*
 * Public datatypes for callbacks and structures used in the TIP#257 (OO)
 * implementation. These are used to implement custom types of method calls
 * and to allow the attachment of arbitrary data to objects and classes.
 */

typedef int (Tcl_MethodCallProc)(ClientData clientData, Tcl_Interp *interp,
	Tcl_ObjectContext objectContext, int objc, Tcl_Obj *const *objv);
typedef void (Tcl_MethodDeleteProc)(ClientData clientData);
typedef int (Tcl_CloneProc)(Tcl_Interp *interp, ClientData oldClientData,
	ClientData *newClientData);
typedef void (Tcl_ObjectMetadataDeleteProc)(ClientData clientData);
typedef int (Tcl_ObjectMapMethodNameProc)(Tcl_Interp *interp,
	Tcl_Object object, Tcl_Class *startClsPtr, Tcl_Obj *methodNameObj);

/*
 * The type of a method implementation. This describes how to call the method
 * implementation, how to delete it (when the object or class is deleted) and
 * how to create a clone of it (when the object or class is copied).
 */

typedef struct {
    int version;		/* Structure version field. Always to be equal
				 * to TCL_OO_METHOD_VERSION_CURRENT in
				 * declarations. */
    const char *name;		/* Name of this type of method, mostly for
				 * debugging purposes. */
    Tcl_MethodCallProc *callProc;
				/* How to invoke this method. */
    Tcl_MethodDeleteProc *deleteProc;
				/* How to delete this method's type-specific
				 * data, or NULL if the type-specific data
				 * does not need deleting. */
    Tcl_CloneProc *cloneProc;	/* How to copy this method's type-specific
				 * data, or NULL if the type-specific data can
				 * be copied directly. */
} Tcl_MethodType;

/*
 * The correct value for the version field of the Tcl_MethodType structure.
 * This allows new versions of the structure to be introduced without breaking
 * binary compatability.
 */

#define TCL_OO_METHOD_VERSION_CURRENT 1

/*
 * The type of some object (or class) metadata. This describes how to delete
 * the metadata (when the object or class is deleted) and how to create a
 * clone of it (when the object or class is copied).
 */

typedef struct {
    int version;		/* Structure version field. Always to be equal
				 * to TCL_OO_METADATA_VERSION_CURRENT in
				 * declarations. */
    const char *name;
    Tcl_ObjectMetadataDeleteProc *deleteProc;
				/* How to delete the metadata. This must not
				 * be NULL. */
    Tcl_CloneProc *cloneProc;	/* How to copy the metadata, or NULL if the
				 * type-specific data can be copied
				 * directly. */
} Tcl_ObjectMetadataType;

/*
 * The correct value for the version field of the Tcl_ObjectMetadataType
 * structure. This allows new versions of the structure to be introduced
 * without breaking binary compatability.
 */

#define TCL_OO_METADATA_VERSION_CURRENT 1

/*
 * Include all the public API, generated from tclOO.decls.
 */

#include "tclOODecls.h"

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
