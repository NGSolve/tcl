/* 
 * tclMacFile.c --
 *
 *      This file implements the channel drivers for Macintosh
 *	files.  It also comtains Macintosh version of other Tcl
 *	functions that deal with the file system.
 *
 * Copyright (c) 1995-1998 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclMacFile.c,v 1.10 2001/07/31 19:12:07 vincentdarley Exp $
 */

/*
 * Note: This code eventually needs to support async I/O.  In doing this
 * we will need to keep track of all current async I/O.  If exit to shell
 * is called - we shouldn't exit until all asyc I/O completes.
 */

#include "tclInt.h"
#include "tclPort.h"
#include "tclMacInt.h"
#include <Aliases.h>
#include <Errors.h>
#include <Processes.h>
#include <Strings.h>
#include <Types.h>
#include <MoreFiles.h>
#include <MoreFilesExtras.h>
#include <FSpCompat.h>

static OSErr FspLocationFromFsPath _ANSI_ARGS_((Tcl_Obj *pathPtr, FSSpec* specPtr));

OSErr 
FspLocationFromFsPath(pathPtr, specPtr)
    Tcl_Obj *pathPtr;
    FSSpec* specPtr;
{
    char *native = Tcl_FSGetNativePath(pathPtr);
    return FSpLocationFromPath(strlen(native), native, specPtr);
}


/*
 *----------------------------------------------------------------------
 *
 * TclpFindExecutable --
 *
 *	This procedure computes the absolute path name of the current
 *	application, given its argv[0] value.  However, this
 *	implementation doesn't need the argv[0] value.  NULL
 *	may be passed in its place.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The variable tclExecutableName gets filled in with the file
 *	name for the application, if we figured it out.  If we couldn't
 *	figure it out, Tcl_FindExecutable is set to NULL.
 *
 *----------------------------------------------------------------------
 */

char *
TclpFindExecutable(
    CONST char *argv0)		/* The value of the application's argv[0]. */
{
    ProcessSerialNumber psn;
    ProcessInfoRec info;
    Str63 appName;
    FSSpec fileSpec;
    int pathLength;
    Handle pathName = NULL;
    OSErr err;
    Tcl_DString ds;
    
    TclInitSubsystems(argv0);
    
    GetCurrentProcess(&psn);
    info.processInfoLength = sizeof(ProcessInfoRec);
    info.processName = appName;
    info.processAppSpec = &fileSpec;
    GetProcessInformation(&psn, &info);

    if (tclExecutableName != NULL) {
	ckfree(tclExecutableName);
	tclExecutableName = NULL;
    }
    
    err = FSpPathFromLocation(&fileSpec, &pathLength, &pathName);
    HLock(pathName);
    Tcl_ExternalToUtfDString(NULL, *pathName, pathLength, &ds);
    HUnlock(pathName);
    DisposeHandle(pathName);	

    tclExecutableName = (char *) ckalloc((unsigned) 
    	    (Tcl_DStringLength(&ds) + 1));
    strcpy(tclExecutableName, Tcl_DStringValue(&ds));
    Tcl_DStringFree(&ds);
    return tclExecutableName;
}

/*
 *----------------------------------------------------------------------
 *
 * TclpMatchInDirectory --
 *
 *	This routine is used by the globbing code to search a
 *	directory for all files which match a given pattern.
 *
 * Results: 
 *	
 *	The return value is a standard Tcl result indicating whether an
 *	error occurred in globbing.  Errors are left in interp, good
 *	results are lappended to resultPtr (which must be a valid object)
 *
 * Side effects:
 *	None.
 *
 *---------------------------------------------------------------------- */

int
TclpMatchInDirectory(interp, resultPtr, pathPtr, pattern, types)
    Tcl_Interp *interp;		/* Interpreter to receive errors. */
    Tcl_Obj *resultPtr;		/* List object to lappend results. */
    Tcl_Obj *pathPtr;	        /* Contains path to directory to search. */
    char *pattern;		/* Pattern to match against. */
    Tcl_GlobTypeData *types;	/* Object containing list of acceptable types.
				 * May be NULL. In particular the directory
				 * flag is very important. */
{
    char *fname;
    int fnameLen, result = TCL_OK;
    int baseLength;
    CInfoPBRec pb;
    OSErr err;
    FSSpec dirSpec;
    Boolean isDirectory;
    long dirID;
    short itemIndex;
    Str255 fileName;
    Tcl_DString fileString;    
    OSType okType = 0;
    OSType okCreator = 0;
    Tcl_DString dsOrig;
    char *fileName2;

    fileName2 = Tcl_FSGetTranslatedPath(interp, pathPtr);
    if (fileName2 == NULL) {
	return TCL_ERROR;
    }
    Tcl_DStringInit(&dsOrig);
    Tcl_DStringAppend(&dsOrig, fileName2, -1);
    baseLength = Tcl_DStringLength(&dsOrig);

    /*
     * Make sure that the directory part of the name really is a
     * directory.
     */

    Tcl_UtfToExternalDString(NULL, Tcl_DStringValue(&dsOrig),
	    Tcl_DStringLength(&dsOrig), &fileString);

    err = FSpLocationFromPath(Tcl_DStringLength(&fileString), Tcl_DStringValue(&fileString), &dirSpec);
    Tcl_DStringFree(&fileString);
	if (err == noErr)
    err = FSpGetDirectoryID(&dirSpec, &dirID, &isDirectory);
    if ((err != noErr) || !isDirectory) {
    /*
     * Check if we had a relative path (unix style rel path compatibility for glob)
     */
	Tcl_DStringFree(&dsOrig);
    Tcl_DStringAppend(&dsOrig, ":", 1);
    Tcl_DStringAppend(&dsOrig, fileName2, -1);
    baseLength = Tcl_DStringLength(&dsOrig);

    Tcl_UtfToExternalDString(NULL, Tcl_DStringValue(&dsOrig),
	    Tcl_DStringLength(&dsOrig), &fileString);
    
    err = FSpLocationFromPath(Tcl_DStringLength(&fileString), Tcl_DStringValue(&fileString), &dirSpec);
    Tcl_DStringFree(&fileString);
    if (err == noErr)
    err = FSpGetDirectoryID(&dirSpec, &dirID, &isDirectory);
    if ((err != noErr) || !isDirectory) {
		Tcl_DStringFree(&dsOrig);
		return TCL_OK;
	}
    }

    /* Make sure we have a trailing directory delimiter */
    if (Tcl_DStringValue(&dsOrig)[baseLength-1] != ':') {
	Tcl_DStringAppend(&dsOrig, ":", 1);
	baseLength++;
    }
    
    /*
     * Now open the directory for reading and iterate over the contents.
     */

    pb.hFileInfo.ioVRefNum = dirSpec.vRefNum;
    pb.hFileInfo.ioDirID = dirID;
    pb.hFileInfo.ioNamePtr = (StringPtr) fileName;
    pb.hFileInfo.ioFDirIndex = itemIndex = 1;

    if (types != NULL) {
	if (types->macType != NULL) {
	    Tcl_GetOSTypeFromObj(NULL, types->macType, &okType);
	}
	if (types->macCreator != NULL) {
	    Tcl_GetOSTypeFromObj(NULL, types->macCreator, &okCreator);
	}
    }

    while (1) {
	pb.hFileInfo.ioFDirIndex = itemIndex;
	pb.hFileInfo.ioDirID = dirID;
	err = PBGetCatInfoSync(&pb);
	if (err != noErr) {
	    break;
	}

	/*
	 * Now check to see if the file matches.  
	 */
	 
	Tcl_ExternalToUtfDString(NULL, (char *) fileName + 1, fileName[0],
		&fileString);
	if (Tcl_StringMatch(Tcl_DStringValue(&fileString), pattern)) {
	    int typeOk = 1;
	    Tcl_DStringSetLength(&dsOrig, baseLength);
	    Tcl_DStringAppend(&dsOrig, Tcl_DStringValue(&fileString), -1);
	    fname = Tcl_DStringValue(&dsOrig);
	    fnameLen = Tcl_DStringLength(&dsOrig);

	    if (types == NULL) {
		/* If invisible, don't return the file */
		if (pb.hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible) {
		    typeOk = 0;
		}
	    } else {
		if (pb.hFileInfo.ioFlFndrInfo.fdFlags & kIsInvisible) {
		    /* If invisible */
		    if ((types->perm == 0) || 
		      !(types->perm & TCL_GLOB_PERM_HIDDEN)) {
			typeOk = 0;
		    }
		} else {
		    /* Visible */
		    if (types->perm & TCL_GLOB_PERM_HIDDEN) {
			typeOk = 0;
		    }
		}
		if (typeOk == 1 && types->perm != 0) {
		    if (
			((types->perm & TCL_GLOB_PERM_RONLY) &&
				!(pb.hFileInfo.ioFlAttrib & 1)) ||
			((types->perm & TCL_GLOB_PERM_R) &&
				(TclpAccess(fname, R_OK) != 0)) ||
			((types->perm & TCL_GLOB_PERM_W) &&
				(TclpAccess(fname, W_OK) != 0)) ||
			((types->perm & TCL_GLOB_PERM_X) &&
				(TclpAccess(fname, X_OK) != 0))
			) {
			typeOk = 0;
		    }
		}
		if (typeOk == 1 && types->type != 0) {
		    struct stat buf;
		    /*
		     * We must match at least one flag to be listed
		     */
		    typeOk = 0;
		    if (TclpLstat(fname, &buf) >= 0) {
			/*
			 * In order bcdpfls as in 'find -t'
			 */
			if (
			    ((types->type & TCL_GLOB_TYPE_BLOCK) &&
				    S_ISBLK(buf.st_mode)) ||
			    ((types->type & TCL_GLOB_TYPE_CHAR) &&
				    S_ISCHR(buf.st_mode)) ||
			    ((types->type & TCL_GLOB_TYPE_DIR) &&
				    S_ISDIR(buf.st_mode)) ||
			    ((types->type & TCL_GLOB_TYPE_PIPE) &&
				    S_ISFIFO(buf.st_mode)) ||
			    ((types->type & TCL_GLOB_TYPE_FILE) &&
				    S_ISREG(buf.st_mode))
#ifdef S_ISLNK
			    || ((types->type & TCL_GLOB_TYPE_LINK) &&
				    S_ISLNK(buf.st_mode))
#endif
#ifdef S_ISSOCK
			    || ((types->type & TCL_GLOB_TYPE_SOCK) &&
				    S_ISSOCK(buf.st_mode))
#endif
			    ) {
			    typeOk = 1;
			}
		    } else {
			/* Posix error occurred */
		    }
		}
		if (typeOk && (
		    ((okType != 0) && (okType !=
			    pb.hFileInfo.ioFlFndrInfo.fdType)) ||
		    ((okCreator != 0) && (okCreator !=
			    pb.hFileInfo.ioFlFndrInfo.fdCreator)))) {
		    typeOk = 0;
		}
	    } 
	    if (typeOk) {
		if ((fnameLen > 1) && (strchr(fname+1, ':') == NULL)) {
		    Tcl_ListObjAppendElement(interp, resultPtr, 
			    Tcl_NewStringObj(fname+1, fnameLen-1));
		} else {
		    Tcl_ListObjAppendElement(interp, resultPtr, 
			    Tcl_NewStringObj(fname, fnameLen));
		}
	    }
	}
	Tcl_DStringFree(&fileString);
	itemIndex++;
    }

    Tcl_DStringFree(&dsOrig);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * TclpAccess --
 *
 *	This function replaces the library version of access().
 *
 * Results:
 *	See access documentation.
 *
 * Side effects:
 *	See access documentation.
 *
 *----------------------------------------------------------------------
 */

int
TclpAccess(
    CONST char *path,		/* Path of file to access (UTF-8). */
    int mode)			/* Permission setting. */
{
    int ret;
    Tcl_Obj *obj = Tcl_NewStringObj(path,-1);
    Tcl_IncrRefCount(obj);
    ret = TclpObjAccess(obj,mode);
    Tcl_DecrRefCount(obj);
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * TclpChdir --
 *
 *	This function replaces the library version of chdir().
 *
 * Results:
 *	See chdir() documentation.
 *
 * Side effects:
 *	See chdir() documentation.  Also the cache maintained used by 
 *	TclGetCwd() is deallocated and set to NULL.
 *
 *----------------------------------------------------------------------
 */

int
TclpChdir(
    CONST char *dirName)     	/* Path to new working directory (UTF-8). */
{
    int ret;
    Tcl_Obj *obj = Tcl_NewStringObj(dirName,-1);
    Tcl_IncrRefCount(obj);
    ret = TclpObjChdir(obj);
    Tcl_DecrRefCount(obj);
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * TclpGetCwd --
 *
 *	This function replaces the library version of getcwd().
 *
 * Results:
 *	The result is a pointer to a string specifying the current
 *	directory, or NULL if the current directory could not be
 *	determined.  If NULL is returned, an error message is left in the
 *	interp's result.  Storage for the result string is allocated in
 *	bufferPtr; the caller must call Tcl_DStringFree() when the result
 *	is no longer needed.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
TclpGetCwd(
    Tcl_Interp *interp,		/* If non-NULL, used for error reporting. */
    Tcl_DString *bufferPtr)	/* Uninitialized or free DString filled
				 * with name of current directory. */
{
    FSSpec theSpec;
    int length;
    Handle pathHandle = NULL;
    
    if (FSpGetDefaultDir(&theSpec) != noErr) {
 	if (interp != NULL) {
	    Tcl_SetResult(interp, "error getting working directory name",
		    TCL_STATIC);
	}
	return NULL;
    }
    if (FSpPathFromLocation(&theSpec, &length, &pathHandle) != noErr) {
 	if (interp != NULL) {
	     Tcl_SetResult(interp, "error getting working directory name",
		    TCL_STATIC);
	}
	return NULL;
    }
    HLock(pathHandle);
    Tcl_ExternalToUtfDString(NULL, *pathHandle, length, bufferPtr);
    HUnlock(pathHandle);
    DisposeHandle(pathHandle);	

    return Tcl_DStringValue(bufferPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclpReadlink --
 *
 *	This function replaces the library version of readlink().
 *
 * Results:
 *	The result is a pointer to a string specifying the contents
 *	of the symbolic link given by 'path', or NULL if the symbolic
 *	link could not be read.  Storage for the result string is
 *	allocated in bufferPtr; the caller must call Tcl_DStringFree()
 *	when the result is no longer needed.
 *
 * Side effects:
 *	See readlink() documentation.
 *
 *---------------------------------------------------------------------------
 */

char *
TclpReadlink(
    CONST char *path,		/* Path of file to readlink (UTF-8). */
    Tcl_DString *linkPtr)	/* Uninitialized or free DString filled
				 * with contents of link (UTF-8). */
{
    HFileInfo fpb;
    OSErr err;
    FSSpec fileSpec;
    Boolean isDirectory;
    Boolean wasAlias;
    long dirID;
    char fileName[257];
    char *end;
    Handle theString = NULL;
    int pathSize;
    Tcl_DString ds;
    char *native;
    
    native = Tcl_UtfToExternalDString(NULL, path, -1, &ds);

    /*
     * Remove ending colons if they exist.
     */
     
    while ((strlen(native) != 0) && (path[strlen(native) - 1] == ':')) {
	native[strlen(native) - 1] = NULL;
    }

    if (strchr(native, ':') == NULL) {
	strcpy(fileName + 1, native);
	native = NULL;
    } else {
	end = strrchr(native, ':') + 1;
	strcpy(fileName + 1, end);
	*end = NULL;
    }
    fileName[0] = (char) strlen(fileName + 1);
    
    /*
     * Create the file spec for the directory of the file
     * we want to look at.
     */

    if (native != NULL) {
	err = FSpLocationFromPath(strlen(native), native, &fileSpec);
	if (err != noErr) {
	    Tcl_DStringFree(&ds);
	    errno = EINVAL;
	    return NULL;
	}
    } else {
	FSMakeFSSpecCompat(0, 0, NULL, &fileSpec);
    }
    Tcl_DStringFree(&ds);
    
    /*
     * Fill the fpb struct up with info about file or directory.
     */

    FSpGetDirectoryID(&fileSpec, &dirID, &isDirectory);
    fpb.ioVRefNum = fileSpec.vRefNum;
    fpb.ioDirID = dirID;
    fpb.ioNamePtr = (StringPtr) fileName;

    fpb.ioFDirIndex = 0;
    err = PBGetCatInfoSync((CInfoPBPtr)&fpb);
    if (err != noErr) {
	errno = TclMacOSErrorToPosixError(err);
	return NULL;
    } else {
	if (fpb.ioFlAttrib & 0x10) {
	    errno = EINVAL;
	    return NULL;
	} else {
	    if (fpb.ioFlFndrInfo.fdFlags & 0x8000) {
		/*
		 * The file is a link!
		 */
	    } else {
		errno = EINVAL;
		return NULL;
	    }
	}
    }
    
    /*
     * If we are here it's really a link - now find out
     * where it points to.
     */
    err = FSMakeFSSpecCompat(fileSpec.vRefNum, dirID, (StringPtr) fileName, 
    	    &fileSpec);
    if (err == noErr) {
	err = ResolveAliasFile(&fileSpec, true, &isDirectory, &wasAlias);
    }
    if ((err == fnfErr) || wasAlias) {
	err = FSpPathFromLocation(&fileSpec, &pathSize, &theString);
	if (err != noErr) {
	    DisposeHandle(theString);
	    errno = ENAMETOOLONG;
	    return NULL;
	}
    } else {
    	errno = EINVAL;
	return NULL;
    }
    
    Tcl_ExternalToUtfDString(NULL, *theString, pathSize, linkPtr);
    DisposeHandle(theString);
    
    return Tcl_DStringValue(linkPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclpLstat --
 *
 *	This function replaces the library version of lstat().
 *
 * Results:
 *	See stat() documentation.
 *
 * Side effects:
 *	See stat() documentation.
 *
 *----------------------------------------------------------------------
 */

int
TclpLstat(
    CONST char *path,		/* Path of file to stat (in UTF-8). */
    struct stat *bufPtr)	/* Filled with results of stat call. */
{
    /*
     * FIXME: Emulate TclpLstat
     */
     
    return TclpStat(path, bufPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclpStat --
 *
 *	This function replaces the library version of stat().
 *
 * Results:
 *	See stat() documentation.
 *
 * Side effects:
 *	See stat() documentation.
 *
 *----------------------------------------------------------------------
 */

int
TclpStat(
    CONST char *path,		/* Path of file to stat (in UTF-8). */
    struct stat *bufPtr)	/* Filled with results of stat call. */
{
    int ret;
    Tcl_Obj *obj = Tcl_NewStringObj(path,-1);
    Tcl_IncrRefCount(obj);
    ret = TclpObjStat(obj,bufPtr);
    Tcl_DecrRefCount(obj);
    return ret;
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_WaitPid --
 *
 *	Fakes a call to wait pid.
 *
 * Results:
 *	Always returns -1.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_Pid
Tcl_WaitPid(
    Tcl_Pid pid,
    int *statPtr,
    int options)
{
    return (Tcl_Pid) -1;
}

/*
 *----------------------------------------------------------------------
 *
 * TclMacFOpenHack --
 *
 *	This function replaces fopen.  It supports paths with alises.
 *	Note, remember to undefine the fopen macro!
 *
 * Results:
 *	See fopen documentation.
 *
 * Side effects:
 *	See fopen documentation.
 *
 *----------------------------------------------------------------------
 */

#undef fopen
FILE *
TclMacFOpenHack(
    CONST char *path,
    CONST char *mode)
{
    OSErr err;
    FSSpec fileSpec;
    Handle pathString = NULL;
    int size;
    FILE * f;
    
    err = FSpLocationFromPath(strlen(path), (char *) path, &fileSpec);
    if ((err != noErr) && (err != fnfErr)) {
	return NULL;
    }
    err = FSpPathFromLocation(&fileSpec, &size, &pathString);
    if ((err != noErr) && (err != fnfErr)) {
	return NULL;
    }
    
    HLock(pathString);
    f = fopen(*pathString, mode);
    HUnlock(pathString);
    DisposeHandle(pathString);
    return f;
}

/*
 *---------------------------------------------------------------------------
 *
 * TclpGetUserHome --
 *
 *	This function takes the specified user name and finds their
 *	home directory.
 *
 * Results:
 *	The result is a pointer to a string specifying the user's home
 *	directory, or NULL if the user's home directory could not be
 *	determined.  Storage for the result string is allocated in
 *	bufferPtr; the caller must call Tcl_DStringFree() when the result
 *	is no longer needed.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

char *
TclpGetUserHome(name, bufferPtr)
    CONST char *name;		/* User name for desired home directory. */
    Tcl_DString *bufferPtr;	/* Uninitialized or free DString filled
				 * with name of user's home directory. */
{
    return NULL;
}

/*
 *----------------------------------------------------------------------
 *
 * TclMacOSErrorToPosixError --
 *
 *	Given a Macintosh OSErr return the appropiate POSIX error.
 *
 * Results:
 *	A Posix error.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
TclMacOSErrorToPosixError(
    int error)	/* A Macintosh error. */
{
    switch (error) {
	case noErr:
	    return 0;
	case bdNamErr:
	    return ENAMETOOLONG;
	case afpObjectTypeErr:
	    return ENOTDIR;
	case fnfErr:
	case dirNFErr:
	    return ENOENT;
	case dupFNErr:
	    return EEXIST;
	case dirFulErr:
	case dskFulErr:
	    return ENOSPC;
	case fBsyErr:
	    return EBUSY;
	case tmfoErr:
	    return ENFILE;
	case fLckdErr:
	case permErr:
	case afpAccessDenied:
	    return EACCES;
	case wPrErr:
	case vLckdErr:
	    return EROFS;
	case badMovErr:
	    return EINVAL;
	case diffVolErr:
	    return EXDEV;
	default:
	    return EINVAL;
    }
}

int
TclMacChmod(
    char *path, 
    int mode)
{
    HParamBlockRec hpb;
    OSErr err;
    
    c2pstr(path);
    hpb.fileParam.ioNamePtr = (unsigned char *) path;
    hpb.fileParam.ioVRefNum = 0;
    hpb.fileParam.ioDirID = 0;
    
    if (mode & 0200) {
        err = PBHRstFLockSync(&hpb);
    } else {
        err = PBHSetFLockSync(&hpb);
    }
    p2cstr((unsigned char *) path);
    
    if (err != noErr) {
        errno = TclMacOSErrorToPosixError(err);
        return -1;
    }
    
    return 0;
}

int 
TclpObjStat(pathPtr, bufPtr)
    Tcl_Obj *pathPtr;
    struct stat *bufPtr;
{
    HFileInfo fpb;
    HVolumeParam vpb;
    OSErr err;
    FSSpec fileSpec;
    Boolean isDirectory;
    long dirID;
    
    err = FspLocationFromFsPath(pathPtr, &fileSpec);
    
    if (err != noErr) {
	errno = TclMacOSErrorToPosixError(err);
	return -1;
    }
    
    /*
     * Fill the fpb & vpb struct up with info about file or directory.
     */
     
    FSpGetDirectoryID(&fileSpec, &dirID, &isDirectory);
    vpb.ioVRefNum = fpb.ioVRefNum = fileSpec.vRefNum;
    vpb.ioNamePtr = fpb.ioNamePtr = fileSpec.name;
    if (isDirectory) {
	fpb.ioDirID = fileSpec.parID;
    } else {
	fpb.ioDirID = dirID;
    }

    fpb.ioFDirIndex = 0;
    err = PBGetCatInfoSync((CInfoPBPtr)&fpb);
    if (err == noErr) {
	vpb.ioVolIndex = 0;
	err = PBHGetVInfoSync((HParmBlkPtr)&vpb);
	if (err == noErr && bufPtr != NULL) {
	    /* 
	     * Files are always readable by everyone.
	     */
	     
	    bufPtr->st_mode = S_IRUSR | S_IRGRP | S_IROTH;

	    /* 
	     * Use the Volume Info & File Info to fill out stat buf.
	     */
	    if (fpb.ioFlAttrib & 0x10) {
		bufPtr->st_mode |= S_IFDIR;
		bufPtr->st_nlink = 2;
	    } else {
		bufPtr->st_nlink = 1;
		if (fpb.ioFlFndrInfo.fdFlags & 0x8000) {
		    bufPtr->st_mode |= S_IFLNK;
		} else {
		    bufPtr->st_mode |= S_IFREG;
		}
	    }
	    if ((fpb.ioFlAttrib & 0x10) || (fpb.ioFlFndrInfo.fdType == 'APPL')) {
		/*
		 * Directories and applications are executable by everyone.
		 */
		 
		bufPtr->st_mode |= S_IXUSR | S_IXGRP | S_IXOTH;
	    }
	    if ((fpb.ioFlAttrib & 0x01) == 0){
		/* 
		 * If not locked, then everyone has write acces.
		 */
		 
		bufPtr->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
	    }
	    bufPtr->st_ino = fpb.ioDirID;
	    bufPtr->st_dev = fpb.ioVRefNum;
	    bufPtr->st_uid = -1;
	    bufPtr->st_gid = -1;
	    bufPtr->st_rdev = 0;
	    bufPtr->st_size = fpb.ioFlLgLen;
	    bufPtr->st_blksize = vpb.ioVAlBlkSiz;
	    bufPtr->st_blocks = (bufPtr->st_size + bufPtr->st_blksize - 1)
		/ bufPtr->st_blksize;

	    /*
	     * The times returned by the Mac file system are in the
	     * local time zone.  We convert them to GMT so that the
	     * epoch starts from GMT.  This is also consistant with
	     * what is returned from "clock seconds".
	     */

	    bufPtr->st_atime = bufPtr->st_mtime = fpb.ioFlMdDat - TclpGetGMTOffset() + tcl_mac_epoch_offset;
	    bufPtr->st_ctime = fpb.ioFlCrDat - TclpGetGMTOffset() + tcl_mac_epoch_offset;
	}
    }

    if (err != noErr) {
	errno = TclMacOSErrorToPosixError(err);
    }
    
    return (err == noErr ? 0 : -1);
}

Tcl_Obj* 
TclpObjGetCwd(interp)
    Tcl_Interp *interp;
{
    Tcl_DString ds;
    if (TclpGetCwd(interp, &ds) != NULL) {
	Tcl_Obj *cwdPtr = Tcl_NewStringObj(Tcl_DStringValue(&ds), -1);
	Tcl_IncrRefCount(cwdPtr);
	Tcl_DStringFree(&ds);
	return cwdPtr;
    } else {
	return NULL;
    }
}

int 
TclpObjChdir(pathPtr)
    Tcl_Obj *pathPtr;
{
    FSSpec spec;
    OSErr err;
    Boolean isFolder;
    long dirID;

    err = FspLocationFromFsPath(pathPtr, &spec);

    if (err != noErr) {
	errno = ENOENT;
	return -1;
    }
    
    err = FSpGetDirectoryID(&spec, &dirID, &isFolder);
    if (err != noErr) {
	errno = ENOENT;
	return -1;
    }

    if (isFolder != true) {
	errno = ENOTDIR;
	return -1;
    }

    err = FSpSetDefaultDir(&spec);
    if (err != noErr) {
	switch (err) {
	    case afpAccessDenied:
		errno = EACCES;
		break;
	    default:
		errno = ENOENT;
	}
	return -1;
    }

    return 0;
}

int 
TclpObjAccess(pathPtr, mode)
    Tcl_Obj *pathPtr;
    int mode;
{
    HFileInfo fpb;
    HVolumeParam vpb;
    OSErr err;
    FSSpec fileSpec;
    Boolean isDirectory;
    long dirID;
    int full_mode = 0;

    err = FspLocationFromFsPath(pathPtr, &fileSpec);

    if (err != noErr) {
	errno = TclMacOSErrorToPosixError(err);
	return -1;
    }
    
    /*
     * Fill the fpb & vpb struct up with info about file or directory.
     */
    FSpGetDirectoryID(&fileSpec, &dirID, &isDirectory);
    vpb.ioVRefNum = fpb.ioVRefNum = fileSpec.vRefNum;
    vpb.ioNamePtr = fpb.ioNamePtr = fileSpec.name;
    if (isDirectory) {
	fpb.ioDirID = fileSpec.parID;
    } else {
	fpb.ioDirID = dirID;
    }

    fpb.ioFDirIndex = 0;
    err = PBGetCatInfoSync((CInfoPBPtr)&fpb);
    if (err == noErr) {
	vpb.ioVolIndex = 0;
	err = PBHGetVInfoSync((HParmBlkPtr)&vpb);
	if (err == noErr) {
	    /* 
	     * Use the Volume Info & File Info to determine
	     * access information.  If we have got this far
	     * we know the directory is searchable or the file
	     * exists.  (We have F_OK)
	     */

	    /*
	     * Check to see if the volume is hardware or
	     * software locked.  If so we arn't W_OK.
	     */
	    if (mode & W_OK) {
		if ((vpb.ioVAtrb & 0x0080) || (vpb.ioVAtrb & 0x8000)) {
		    errno = EROFS;
		    return -1;
		}
		if (fpb.ioFlAttrib & 0x01) {
		    errno = EACCES;
		    return -1;
		}
	    }
	    
	    /*
	     * Directories are always searchable and executable.  But only 
	     * files of type 'APPL' are executable.
	     */
	    if (!(fpb.ioFlAttrib & 0x10) && (mode & X_OK)
		&& (fpb.ioFlFndrInfo.fdType != 'APPL')) {
		return -1;
	    }
	}
    }

    if (err != noErr) {
	errno = TclMacOSErrorToPosixError(err);
	return -1;
    }
    
    return 0;
}

int 
TclpObjLstat(pathPtr, buf)
    Tcl_Obj *pathPtr;
    struct stat *buf;
{
    return TclpObjStat(pathPtr, buf);
}


/*
 *----------------------------------------------------------------------
 *
 * TclpTempFileName --
 *
 *	This function returns a unique filename.
 *
 * Results:
 *	Returns a valid Tcl_Obj* with refCount 0, or NULL on failure.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj* 
TclpTempFileName()
{
    char fileName[L_tmpnam];
    
    if (tmpnam(fileName) == NULL) {			/* INTL: Native. */
	return NULL;
    }

    return TclpNativeToNormalized((ClientData) fileName);
}

#ifdef S_IFLNK

Tcl_Obj* 
TclpObjReadlink(pathPtr)
    Tcl_Obj *pathPtr;
{
    Tcl_DString ds;
    Tcl_Obj* link = NULL;
    if (TclpReadlink(Tcl_FSGetTranslatedPath(NULL, pathPtr), &ds) != NULL) {
	link = Tcl_NewStringObj(Tcl_DStringValue(&ds), -1);
	Tcl_IncrRefCount(link);
	Tcl_DStringFree(&ds);
    }
    return link;
}

#endif
