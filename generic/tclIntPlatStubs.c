/* 
 * tclIntPlatStubs.c --
 *
 *	This file contains the wrapper functions for the platform dependent
 *	unsupported Tcl API.
 *
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 * All rights reserved.
 *
 * RCS: @(#) $Id: tclIntPlatStubs.c,v 1.3.2.1 1999/03/10 06:49:19 stanton Exp $
 */

#include "tclInt.h"
#include "tclPort.h"

/*
 * WARNING: This file is automatically generated by the tools/genStubs.tcl
 * script.  Any modifications to the function declarations below should be made
 * in the generic/tclInt.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported stub functions:
 */

#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
/* Slot 0 */
void
TclGetAndDetachPids(interp, chan)
    Tcl_Interp * interp;
    Tcl_Channel chan;
{
    (tclIntPlatStubsPtr->tclGetAndDetachPids)(interp, chan);
}

/* Slot 1 */
int
TclpCloseFile(file)
    TclFile file;
{
    return (tclIntPlatStubsPtr->tclpCloseFile)(file);
}

/* Slot 2 */
Tcl_Channel
TclpCreateCommandChannel(readFile, writeFile, errorFile, numPids, pidPtr)
    TclFile readFile;
    TclFile writeFile;
    TclFile errorFile;
    int numPids;
    Tcl_Pid * pidPtr;
{
    return (tclIntPlatStubsPtr->tclpCreateCommandChannel)(readFile, writeFile, errorFile, numPids, pidPtr);
}

/* Slot 3 */
int
TclpCreatePipe(readPipe, writePipe)
    TclFile * readPipe;
    TclFile * writePipe;
{
    return (tclIntPlatStubsPtr->tclpCreatePipe)(readPipe, writePipe);
}

/* Slot 4 */
int
TclpCreateProcess(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr)
    Tcl_Interp * interp;
    int argc;
    char ** argv;
    TclFile inputFile;
    TclFile outputFile;
    TclFile errorFile;
    Tcl_Pid * pidPtr;
{
    return (tclIntPlatStubsPtr->tclpCreateProcess)(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr);
}

/* Slot 5 is reserved */
/* Slot 6 */
TclFile
TclpMakeFile(channel, direction)
    Tcl_Channel channel;
    int direction;
{
    return (tclIntPlatStubsPtr->tclpMakeFile)(channel, direction);
}

/* Slot 7 */
TclFile
TclpOpenFile(fname, mode)
    CONST char * fname;
    int mode;
{
    return (tclIntPlatStubsPtr->tclpOpenFile)(fname, mode);
}

/* Slot 8 */
int
TclUnixWaitForFile(fd, mask, timeout)
    int fd;
    int mask;
    int timeout;
{
    return (tclIntPlatStubsPtr->tclUnixWaitForFile)(fd, mask, timeout);
}

/* Slot 9 */
TclFile
TclpCreateTempFile(contents)
    CONST char * contents;
{
    return (tclIntPlatStubsPtr->tclpCreateTempFile)(contents);
}

#endif /* UNIX */
#ifdef __WIN32__
/* Slot 0 */
void
TclWinConvertError(errCode)
    DWORD errCode;
{
    (tclIntPlatStubsPtr->tclWinConvertError)(errCode);
}

/* Slot 1 */
void
TclWinConvertWSAError(errCode)
    DWORD errCode;
{
    (tclIntPlatStubsPtr->tclWinConvertWSAError)(errCode);
}

/* Slot 2 */
struct servent *
TclWinGetServByName(nm, proto)
    const char * nm;
    const char * proto;
{
    return (tclIntPlatStubsPtr->tclWinGetServByName)(nm, proto);
}

/* Slot 3 */
int
TclWinGetSockOpt(s, level, optname, optval, optlen)
    SOCKET s;
    int level;
    int optname;
    char FAR * optval;
    int FAR * optlen;
{
    return (tclIntPlatStubsPtr->tclWinGetSockOpt)(s, level, optname, optval, optlen);
}

/* Slot 4 */
HINSTANCE
TclWinGetTclInstance()
{
    return (tclIntPlatStubsPtr->tclWinGetTclInstance)();
}

/* Slot 5 is reserved */
/* Slot 6 */
u_short
TclWinNToHS(ns)
    u_short ns;
{
    return (tclIntPlatStubsPtr->tclWinNToHS)(ns);
}

/* Slot 7 */
int
TclWinSetSockOpt(s, level, optname, optval, optlen)
    SOCKET s;
    int level;
    int optname;
    const char FAR * optval;
    int optlen;
{
    return (tclIntPlatStubsPtr->tclWinSetSockOpt)(s, level, optname, optval, optlen);
}

/* Slot 8 */
unsigned long
TclpGetPid(pid)
    Tcl_Pid pid;
{
    return (tclIntPlatStubsPtr->tclpGetPid)(pid);
}

/* Slot 9 */
int
TclWinGetPlatformId()
{
    return (tclIntPlatStubsPtr->tclWinGetPlatformId)();
}

/* Slot 10 */
int
TclWinSynchSpawn(args, type, trans, pidPtr)
    void * args;
    int type;
    void ** trans;
    Tcl_Pid * pidPtr;
{
    return (tclIntPlatStubsPtr->tclWinSynchSpawn)(args, type, trans, pidPtr);
}

/* Slot 11 */
void
TclGetAndDetachPids(interp, chan)
    Tcl_Interp * interp;
    Tcl_Channel chan;
{
    (tclIntPlatStubsPtr->tclGetAndDetachPids)(interp, chan);
}

/* Slot 12 */
int
TclpCloseFile(file)
    TclFile file;
{
    return (tclIntPlatStubsPtr->tclpCloseFile)(file);
}

/* Slot 13 */
Tcl_Channel
TclpCreateCommandChannel(readFile, writeFile, errorFile, numPids, pidPtr)
    TclFile readFile;
    TclFile writeFile;
    TclFile errorFile;
    int numPids;
    Tcl_Pid * pidPtr;
{
    return (tclIntPlatStubsPtr->tclpCreateCommandChannel)(readFile, writeFile, errorFile, numPids, pidPtr);
}

/* Slot 14 */
int
TclpCreatePipe(readPipe, writePipe)
    TclFile * readPipe;
    TclFile * writePipe;
{
    return (tclIntPlatStubsPtr->tclpCreatePipe)(readPipe, writePipe);
}

/* Slot 15 */
int
TclpCreateProcess(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr)
    Tcl_Interp * interp;
    int argc;
    char ** argv;
    TclFile inputFile;
    TclFile outputFile;
    TclFile errorFile;
    Tcl_Pid * pidPtr;
{
    return (tclIntPlatStubsPtr->tclpCreateProcess)(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr);
}

/* Slot 16 is reserved */
/* Slot 17 is reserved */
/* Slot 18 */
TclFile
TclpMakeFile(channel, direction)
    Tcl_Channel channel;
    int direction;
{
    return (tclIntPlatStubsPtr->tclpMakeFile)(channel, direction);
}

/* Slot 19 */
TclFile
TclpOpenFile(fname, mode)
    CONST char * fname;
    int mode;
{
    return (tclIntPlatStubsPtr->tclpOpenFile)(fname, mode);
}

/* Slot 20 */
TclFile
TclpCreateTempFile(contents)
    CONST char * contents;
{
    return (tclIntPlatStubsPtr->tclpCreateTempFile)(contents);
}

/* Slot 21 */
char *
TclpGetTZName(isdst)
    int isdst;
{
    return (tclIntPlatStubsPtr->tclpGetTZName)(isdst);
}

/* Slot 22 */
char *
TclWinNoBackslash(path)
    char * path;
{
    return (tclIntPlatStubsPtr->tclWinNoBackslash)(path);
}

/* Slot 23 */
TCHAR *
Tcl_WinUtfToTChar(string, len, dsPtr)
    CONST char * string;
    int len;
    Tcl_DString * dsPtr;
{
    return (tclIntPlatStubsPtr->tcl_WinUtfToTChar)(string, len, dsPtr);
}

/* Slot 24 */
char *
Tcl_WinTCharToUtf(string, len, dsPtr)
    CONST TCHAR * string;
    int len;
    Tcl_DString * dsPtr;
{
    return (tclIntPlatStubsPtr->tcl_WinTCharToUtf)(string, len, dsPtr);
}

#endif /* __WIN32__ */
#ifdef MAC_TCL
/* Slot 0 */
VOID *
TclpSysAlloc(size, isBin)
    long size;
    int isBin;
{
    return (tclIntPlatStubsPtr->tclpSysAlloc)(size, isBin);
}

/* Slot 1 */
void
TclpSysFree(ptr)
    VOID * ptr;
{
    (tclIntPlatStubsPtr->tclpSysFree)(ptr);
}

/* Slot 2 */
VOID *
TclpSysRealloc(cp, size)
    VOID * cp;
    unsigned int size;
{
    return (tclIntPlatStubsPtr->tclpSysRealloc)(cp, size);
}

/* Slot 3 */
void
TclpExit(status)
    int status;
{
    (tclIntPlatStubsPtr->tclpExit)(status);
}

/* Slot 4 */
int
FSpGetDefaultDir(theSpec)
    FSSpecPtr theSpec;
{
    return (tclIntPlatStubsPtr->fSpGetDefaultDir)(theSpec);
}

/* Slot 5 */
int
FSpSetDefaultDir(theSpec)
    FSSpecPtr theSpec;
{
    return (tclIntPlatStubsPtr->fSpSetDefaultDir)(theSpec);
}

/* Slot 6 */
OSErr
FSpFindFolder(vRefNum, folderType, createFolder, spec)
    short vRefNum;
    OSType folderType;
    Boolean createFolder;
    FSSpec * spec;
{
    return (tclIntPlatStubsPtr->fSpFindFolder)(vRefNum, folderType, createFolder, spec);
}

/* Slot 7 */
void
GetGlobalMouse(mouse)
    Point * mouse;
{
    (tclIntPlatStubsPtr->getGlobalMouse)(mouse);
}

/* Slot 8 */
pascal OSErr
FSpGetDirectoryID(spec, theDirID, isDirectory)
    const FSSpec * spec;
    long * theDirID;
    Boolean * isDirectory;
{
    return (tclIntPlatStubsPtr->fSpGetDirectoryID)(spec, theDirID, isDirectory);
}

/* Slot 9 */
pascal short
FSpOpenResFileCompat(spec, permission)
    const FSSpec * spec;
    SignedByte permission;
{
    return (tclIntPlatStubsPtr->fSpOpenResFileCompat)(spec, permission);
}

/* Slot 10 */
pascal void
FSpCreateResFileCompat(spec, creator, fileType, scriptTag)
    const FSSpec * spec;
    OSType creator;
    OSType fileType;
    ScriptCode scriptTag;
{
    return (tclIntPlatStubsPtr->fSpCreateResFileCompat)(spec, creator, fileType, scriptTag);
}

/* Slot 11 */
int
FSpLocationFromPath(length, path, theSpec)
    int length;
    CONST char * path;
    FSSpecPtr theSpec;
{
    return (tclIntPlatStubsPtr->fSpLocationFromPath)(length, path, theSpec);
}

/* Slot 12 */
OSErr
FSpPathFromLocation(theSpec, length, fullPath)
    FSSpecPtr theSpec;
    int * length;
    Handle * fullPath;
{
    return (tclIntPlatStubsPtr->fSpPathFromLocation)(theSpec, length, fullPath);
}

/* Slot 13 */
void
TclMacExitHandler()
{
    (tclIntPlatStubsPtr->tclMacExitHandler)();
}

/* Slot 14 */
void
TclMacInitExitToShell(usePatch)
    int usePatch;
{
    (tclIntPlatStubsPtr->tclMacInitExitToShell)(usePatch);
}

/* Slot 15 */
OSErr
TclMacInstallExitToShellPatch(newProc)
    ExitToShellProcPtr newProc;
{
    return (tclIntPlatStubsPtr->tclMacInstallExitToShellPatch)(newProc);
}

/* Slot 16 */
int
TclMacOSErrorToPosixError(error)
    int error;
{
    return (tclIntPlatStubsPtr->tclMacOSErrorToPosixError)(error);
}

/* Slot 17 */
void
TclMacRemoveTimer(timerToken)
    void * timerToken;
{
    (tclIntPlatStubsPtr->tclMacRemoveTimer)(timerToken);
}

/* Slot 18 */
void *
TclMacStartTimer(ms)
    long ms;
{
    return (tclIntPlatStubsPtr->tclMacStartTimer)(ms);
}

/* Slot 19 */
int
TclMacTimerExpired(timerToken)
    void * timerToken;
{
    return (tclIntPlatStubsPtr->tclMacTimerExpired)(timerToken);
}

/* Slot 20 */
int
TclMacRegisterResourceFork(fileRef, tokenPtr, insert)
    short fileRef;
    Tcl_Obj * tokenPtr;
    int insert;
{
    return (tclIntPlatStubsPtr->tclMacRegisterResourceFork)(fileRef, tokenPtr, insert);
}

/* Slot 21 */
short
TclMacUnRegisterResourceFork(tokenPtr, resultPtr)
    char * tokenPtr;
    Tcl_Obj * resultPtr;
{
    return (tclIntPlatStubsPtr->tclMacUnRegisterResourceFork)(tokenPtr, resultPtr);
}

/* Slot 22 */
int
TclMacCreateEnv()
{
    return (tclIntPlatStubsPtr->tclMacCreateEnv)();
}

/* Slot 23 */
FILE *
TclMacFOpenHack(path, mode)
    const char * path;
    const char * mode;
{
    return (tclIntPlatStubsPtr->tclMacFOpenHack)(path, mode);
}

/* Slot 24 is reserved */
/* Slot 25 */
int
TclMacChmod(path, mode)
    char * path;
    int mode;
{
    return (tclIntPlatStubsPtr->tclMacChmod)(path, mode);
}

#endif /* MAC_TCL */

/* !END!: Do not edit above this line. */
