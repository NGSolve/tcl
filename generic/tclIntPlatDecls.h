/*
 * tclIntPlatDecls.h --
 *
 *	This file contains the declarations for all platform dependent
 *	unsupported functions that are exported by the Tcl library.  These
 *	interfaces are not guaranteed to remain the same between
 *	versions.  Use at your own risk.
 *
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 * All rights reserved.
 *
 * RCS: @(#) $Id: tclIntPlatDecls.h,v 1.2.4.2 1999/03/10 05:50:27 stanton Exp $
 */

#ifndef _TCLINTPLATDECLS
#define _TCLINTPLATDECLS

/*
 * WARNING: This file is automatically generated by the tools/genStubs.tcl
 * script.  Any modifications to the function declarations below should be made
 * in the generic/tclInt.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
/* 0 */
EXTERN void		TclGetAndDetachPids _ANSI_ARGS_((Tcl_Interp * interp, 
				Tcl_Channel chan));
/* 1 */
EXTERN int		TclpCloseFile _ANSI_ARGS_((TclFile file));
/* 2 */
EXTERN Tcl_Channel	TclpCreateCommandChannel _ANSI_ARGS_((
				TclFile readFile, TclFile writeFile, 
				TclFile errorFile, int numPids, 
				Tcl_Pid * pidPtr));
/* 3 */
EXTERN int		TclpCreatePipe _ANSI_ARGS_((TclFile * readPipe, 
				TclFile * writePipe));
/* 4 */
EXTERN int		TclpCreateProcess _ANSI_ARGS_((Tcl_Interp * interp, 
				int argc, char ** argv, TclFile inputFile, 
				TclFile outputFile, TclFile errorFile, 
				Tcl_Pid * pidPtr));
/* 5 */
EXTERN TclFile		TclpCreateTempFile _ANSI_ARGS_((char * contents, 
				Tcl_DString * namePtr));
/* 6 */
EXTERN TclFile		TclpMakeFile _ANSI_ARGS_((Tcl_Channel channel, 
				int direction));
/* 7 */
EXTERN TclFile		TclpOpenFile _ANSI_ARGS_((char * fname, int mode));
/* 8 */
EXTERN int		TclUnixWaitForFile _ANSI_ARGS_((int fd, int mask, 
				int timeout));
#endif /* UNIX */
#ifdef __WIN32__
/* 0 */
EXTERN void		TclWinConvertError _ANSI_ARGS_((DWORD errCode));
/* 1 */
EXTERN void		TclWinConvertWSAError _ANSI_ARGS_((DWORD errCode));
/* 2 */
EXTERN struct servent *	 TclWinGetServByName _ANSI_ARGS_((const char * nm, 
				const char * proto));
/* 3 */
EXTERN int		TclWinGetSockOpt _ANSI_ARGS_((SOCKET s, int level, 
				int optname, char FAR * optval, 
				int FAR * optlen));
/* 4 */
EXTERN HINSTANCE	TclWinGetTclInstance _ANSI_ARGS_((void));
/* 5 */
EXTERN HINSTANCE	TclWinLoadLibrary _ANSI_ARGS_((char * name));
/* 6 */
EXTERN u_short		TclWinNToHS _ANSI_ARGS_((u_short ns));
/* 7 */
EXTERN int		TclWinSetSockOpt _ANSI_ARGS_((SOCKET s, int level, 
				int optname, const char FAR * optval, 
				int optlen));
/* 8 */
EXTERN unsigned long	TclpGetPid _ANSI_ARGS_((Tcl_Pid pid));
/* 9 */
EXTERN int		TclWinGetPlatformId _ANSI_ARGS_((void));
/* 10 */
EXTERN int		TclWinSynchSpawn _ANSI_ARGS_((void * args, int type, 
				void ** trans, Tcl_Pid * pidPtr));
/* 11 */
EXTERN void		TclGetAndDetachPids _ANSI_ARGS_((Tcl_Interp * interp, 
				Tcl_Channel chan));
/* 12 */
EXTERN int		TclpCloseFile _ANSI_ARGS_((TclFile file));
/* 13 */
EXTERN Tcl_Channel	TclpCreateCommandChannel _ANSI_ARGS_((
				TclFile readFile, TclFile writeFile, 
				TclFile errorFile, int numPids, 
				Tcl_Pid * pidPtr));
/* 14 */
EXTERN int		TclpCreatePipe _ANSI_ARGS_((TclFile * readPipe, 
				TclFile * writePipe));
/* 15 */
EXTERN int		TclpCreateProcess _ANSI_ARGS_((Tcl_Interp * interp, 
				int argc, char ** argv, TclFile inputFile, 
				TclFile outputFile, TclFile errorFile, 
				Tcl_Pid * pidPtr));
/* 16 */
EXTERN TclFile		TclpCreateTempFile _ANSI_ARGS_((char * contents, 
				Tcl_DString * namePtr));
/* 17 */
EXTERN char *		TclpGetTZName _ANSI_ARGS_((void));
/* 18 */
EXTERN TclFile		TclpMakeFile _ANSI_ARGS_((Tcl_Channel channel, 
				int direction));
/* 19 */
EXTERN TclFile		TclpOpenFile _ANSI_ARGS_((char * fname, int mode));
#endif /* __WIN32__ */
#ifdef MAC_TCL
/* 0 */
EXTERN VOID *		TclpSysAlloc _ANSI_ARGS_((long size, int isBin));
/* 1 */
EXTERN void		TclpSysFree _ANSI_ARGS_((VOID * ptr));
/* 2 */
EXTERN VOID *		TclpSysRealloc _ANSI_ARGS_((VOID * cp, 
				unsigned int size));
/* 3 */
EXTERN void		TclPlatformExit _ANSI_ARGS_((int status));
/* 4 */
EXTERN int		FSpGetDefaultDir _ANSI_ARGS_((FSSpecPtr theSpec));
/* 5 */
EXTERN int		FSpSetDefaultDir _ANSI_ARGS_((FSSpecPtr theSpec));
/* 6 */
EXTERN OSErr		FSpFindFolder _ANSI_ARGS_((short vRefNum, 
				OSType folderType, Boolean createFolder, 
				FSSpec * spec));
/* 7 */
EXTERN void		GetGlobalMouse _ANSI_ARGS_((Point * mouse));
/* 8 */
EXTERN pascal OSErr	FSpGetDirectoryID _ANSI_ARGS_((const FSSpec * spec, 
				long * theDirID, Boolean * isDirectory));
/* 9 */
EXTERN pascal short	FSpOpenResFileCompat _ANSI_ARGS_((
				const FSSpec * spec, SignedByte permission));
/* 10 */
EXTERN pascal void	FSpCreateResFileCompat _ANSI_ARGS_((
				const FSSpec * spec, OSType creator, 
				OSType fileType, ScriptCode scriptTag));
/* 11 */
EXTERN int		FSpLocationFromPath _ANSI_ARGS_((int length, 
				CONST char * path, FSSpecPtr theSpec));
/* 12 */
EXTERN OSErr		FSpPathFromLocation _ANSI_ARGS_((FSSpecPtr theSpec, 
				int * length, Handle * fullPath));
/* 13 */
EXTERN void		TclMacExitHandler _ANSI_ARGS_((void));
/* 14 */
EXTERN void		TclMacInitExitToShell _ANSI_ARGS_((int usePatch));
/* 15 */
EXTERN OSErr		TclMacInstallExitToShellPatch _ANSI_ARGS_((
				ExitToShellProcPtr newProc));
/* 16 */
EXTERN int		TclMacOSErrorToPosixError _ANSI_ARGS_((int error));
/* 17 */
EXTERN void		TclMacRemoveTimer _ANSI_ARGS_((void * timerToken));
/* 18 */
EXTERN void *		TclMacStartTimer _ANSI_ARGS_((long ms));
/* 19 */
EXTERN int		TclMacTimerExpired _ANSI_ARGS_((void * timerToken));
/* 20 */
EXTERN int		TclMacRegisterResourceFork _ANSI_ARGS_((
				short fileRef, Tcl_Obj * tokenPtr, 
				int insert));
/* 21 */
EXTERN short		TclMacUnRegisterResourceFork _ANSI_ARGS_((
				char * tokenPtr, Tcl_Obj * resultPtr));
/* 22 */
EXTERN int		TclMacCreateEnv _ANSI_ARGS_((void));
/* 23 */
EXTERN FILE *		TclMacFOpenHack _ANSI_ARGS_((const char * path, 
				const char * mode));
/* 24 */
EXTERN int		TclMacReadlink _ANSI_ARGS_((char * path, char * buf, 
				int size));
/* 25 */
EXTERN int		TclMacChmod _ANSI_ARGS_((char * path, int mode));
#endif /* MAC_TCL */

typedef struct TclIntPlatStubs {
    int magic;
    struct TclIntPlatStubHooks *hooks;

#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
    void (*tclGetAndDetachPids) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Channel chan)); /* 0 */
    int (*tclpCloseFile) _ANSI_ARGS_((TclFile file)); /* 1 */
    Tcl_Channel (*tclpCreateCommandChannel) _ANSI_ARGS_((TclFile readFile, TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid * pidPtr)); /* 2 */
    int (*tclpCreatePipe) _ANSI_ARGS_((TclFile * readPipe, TclFile * writePipe)); /* 3 */
    int (*tclpCreateProcess) _ANSI_ARGS_((Tcl_Interp * interp, int argc, char ** argv, TclFile inputFile, TclFile outputFile, TclFile errorFile, Tcl_Pid * pidPtr)); /* 4 */
    TclFile (*tclpCreateTempFile) _ANSI_ARGS_((char * contents, Tcl_DString * namePtr)); /* 5 */
    TclFile (*tclpMakeFile) _ANSI_ARGS_((Tcl_Channel channel, int direction)); /* 6 */
    TclFile (*tclpOpenFile) _ANSI_ARGS_((char * fname, int mode)); /* 7 */
    int (*tclUnixWaitForFile) _ANSI_ARGS_((int fd, int mask, int timeout)); /* 8 */
#endif /* UNIX */
#ifdef __WIN32__
    void (*tclWinConvertError) _ANSI_ARGS_((DWORD errCode)); /* 0 */
    void (*tclWinConvertWSAError) _ANSI_ARGS_((DWORD errCode)); /* 1 */
    struct servent * (*tclWinGetServByName) _ANSI_ARGS_((const char * nm, const char * proto)); /* 2 */
    int (*tclWinGetSockOpt) _ANSI_ARGS_((SOCKET s, int level, int optname, char FAR * optval, int FAR * optlen)); /* 3 */
    HINSTANCE (*tclWinGetTclInstance) _ANSI_ARGS_((void)); /* 4 */
    HINSTANCE (*tclWinLoadLibrary) _ANSI_ARGS_((char * name)); /* 5 */
    u_short (*tclWinNToHS) _ANSI_ARGS_((u_short ns)); /* 6 */
    int (*tclWinSetSockOpt) _ANSI_ARGS_((SOCKET s, int level, int optname, const char FAR * optval, int optlen)); /* 7 */
    unsigned long (*tclpGetPid) _ANSI_ARGS_((Tcl_Pid pid)); /* 8 */
    int (*tclWinGetPlatformId) _ANSI_ARGS_((void)); /* 9 */
    int (*tclWinSynchSpawn) _ANSI_ARGS_((void * args, int type, void ** trans, Tcl_Pid * pidPtr)); /* 10 */
    void (*tclGetAndDetachPids) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Channel chan)); /* 11 */
    int (*tclpCloseFile) _ANSI_ARGS_((TclFile file)); /* 12 */
    Tcl_Channel (*tclpCreateCommandChannel) _ANSI_ARGS_((TclFile readFile, TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid * pidPtr)); /* 13 */
    int (*tclpCreatePipe) _ANSI_ARGS_((TclFile * readPipe, TclFile * writePipe)); /* 14 */
    int (*tclpCreateProcess) _ANSI_ARGS_((Tcl_Interp * interp, int argc, char ** argv, TclFile inputFile, TclFile outputFile, TclFile errorFile, Tcl_Pid * pidPtr)); /* 15 */
    TclFile (*tclpCreateTempFile) _ANSI_ARGS_((char * contents, Tcl_DString * namePtr)); /* 16 */
    char * (*tclpGetTZName) _ANSI_ARGS_((void)); /* 17 */
    TclFile (*tclpMakeFile) _ANSI_ARGS_((Tcl_Channel channel, int direction)); /* 18 */
    TclFile (*tclpOpenFile) _ANSI_ARGS_((char * fname, int mode)); /* 19 */
#endif /* __WIN32__ */
#ifdef MAC_TCL
    VOID * (*tclpSysAlloc) _ANSI_ARGS_((long size, int isBin)); /* 0 */
    void (*tclpSysFree) _ANSI_ARGS_((VOID * ptr)); /* 1 */
    VOID * (*tclpSysRealloc) _ANSI_ARGS_((VOID * cp, unsigned int size)); /* 2 */
    void (*tclPlatformExit) _ANSI_ARGS_((int status)); /* 3 */
    int (*fSpGetDefaultDir) _ANSI_ARGS_((FSSpecPtr theSpec)); /* 4 */
    int (*fSpSetDefaultDir) _ANSI_ARGS_((FSSpecPtr theSpec)); /* 5 */
    OSErr (*fSpFindFolder) _ANSI_ARGS_((short vRefNum, OSType folderType, Boolean createFolder, FSSpec * spec)); /* 6 */
    void (*getGlobalMouse) _ANSI_ARGS_((Point * mouse)); /* 7 */
    pascal OSErr (*fSpGetDirectoryID) _ANSI_ARGS_((const FSSpec * spec, long * theDirID, Boolean * isDirectory)); /* 8 */
    pascal short (*fSpOpenResFileCompat) _ANSI_ARGS_((const FSSpec * spec, SignedByte permission)); /* 9 */
    pascal void (*fSpCreateResFileCompat) _ANSI_ARGS_((const FSSpec * spec, OSType creator, OSType fileType, ScriptCode scriptTag)); /* 10 */
    int (*fSpLocationFromPath) _ANSI_ARGS_((int length, CONST char * path, FSSpecPtr theSpec)); /* 11 */
    OSErr (*fSpPathFromLocation) _ANSI_ARGS_((FSSpecPtr theSpec, int * length, Handle * fullPath)); /* 12 */
    void (*tclMacExitHandler) _ANSI_ARGS_((void)); /* 13 */
    void (*tclMacInitExitToShell) _ANSI_ARGS_((int usePatch)); /* 14 */
    OSErr (*tclMacInstallExitToShellPatch) _ANSI_ARGS_((ExitToShellProcPtr newProc)); /* 15 */
    int (*tclMacOSErrorToPosixError) _ANSI_ARGS_((int error)); /* 16 */
    void (*tclMacRemoveTimer) _ANSI_ARGS_((void * timerToken)); /* 17 */
    void * (*tclMacStartTimer) _ANSI_ARGS_((long ms)); /* 18 */
    int (*tclMacTimerExpired) _ANSI_ARGS_((void * timerToken)); /* 19 */
    int (*tclMacRegisterResourceFork) _ANSI_ARGS_((short fileRef, Tcl_Obj * tokenPtr, int insert)); /* 20 */
    short (*tclMacUnRegisterResourceFork) _ANSI_ARGS_((char * tokenPtr, Tcl_Obj * resultPtr)); /* 21 */
    int (*tclMacCreateEnv) _ANSI_ARGS_((void)); /* 22 */
    FILE * (*tclMacFOpenHack) _ANSI_ARGS_((const char * path, const char * mode)); /* 23 */
    int (*tclMacReadlink) _ANSI_ARGS_((char * path, char * buf, int size)); /* 24 */
    int (*tclMacChmod) _ANSI_ARGS_((char * path, int mode)); /* 25 */
#endif /* MAC_TCL */
} TclIntPlatStubs;

extern TclIntPlatStubs *tclIntPlatStubsPtr;

#if defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS)

/*
 * Inline function declarations:
 */

#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
#ifndef TclGetAndDetachPids
#define TclGetAndDetachPids(interp, chan) \
	(tclIntPlatStubsPtr->tclGetAndDetachPids)(interp, chan) /* 0 */
#endif
#ifndef TclpCloseFile
#define TclpCloseFile(file) \
	(tclIntPlatStubsPtr->tclpCloseFile)(file) /* 1 */
#endif
#ifndef TclpCreateCommandChannel
#define TclpCreateCommandChannel(readFile, writeFile, errorFile, numPids, pidPtr) \
	(tclIntPlatStubsPtr->tclpCreateCommandChannel)(readFile, writeFile, errorFile, numPids, pidPtr) /* 2 */
#endif
#ifndef TclpCreatePipe
#define TclpCreatePipe(readPipe, writePipe) \
	(tclIntPlatStubsPtr->tclpCreatePipe)(readPipe, writePipe) /* 3 */
#endif
#ifndef TclpCreateProcess
#define TclpCreateProcess(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr) \
	(tclIntPlatStubsPtr->tclpCreateProcess)(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr) /* 4 */
#endif
#ifndef TclpCreateTempFile
#define TclpCreateTempFile(contents, namePtr) \
	(tclIntPlatStubsPtr->tclpCreateTempFile)(contents, namePtr) /* 5 */
#endif
#ifndef TclpMakeFile
#define TclpMakeFile(channel, direction) \
	(tclIntPlatStubsPtr->tclpMakeFile)(channel, direction) /* 6 */
#endif
#ifndef TclpOpenFile
#define TclpOpenFile(fname, mode) \
	(tclIntPlatStubsPtr->tclpOpenFile)(fname, mode) /* 7 */
#endif
#ifndef TclUnixWaitForFile
#define TclUnixWaitForFile(fd, mask, timeout) \
	(tclIntPlatStubsPtr->tclUnixWaitForFile)(fd, mask, timeout) /* 8 */
#endif
#endif /* UNIX */
#ifdef __WIN32__
#ifndef TclWinConvertError
#define TclWinConvertError(errCode) \
	(tclIntPlatStubsPtr->tclWinConvertError)(errCode) /* 0 */
#endif
#ifndef TclWinConvertWSAError
#define TclWinConvertWSAError(errCode) \
	(tclIntPlatStubsPtr->tclWinConvertWSAError)(errCode) /* 1 */
#endif
#ifndef TclWinGetServByName
#define TclWinGetServByName(nm, proto) \
	(tclIntPlatStubsPtr->tclWinGetServByName)(nm, proto) /* 2 */
#endif
#ifndef TclWinGetSockOpt
#define TclWinGetSockOpt(s, level, optname, optval, optlen) \
	(tclIntPlatStubsPtr->tclWinGetSockOpt)(s, level, optname, optval, optlen) /* 3 */
#endif
#ifndef TclWinGetTclInstance
#define TclWinGetTclInstance() \
	(tclIntPlatStubsPtr->tclWinGetTclInstance)() /* 4 */
#endif
#ifndef TclWinLoadLibrary
#define TclWinLoadLibrary(name) \
	(tclIntPlatStubsPtr->tclWinLoadLibrary)(name) /* 5 */
#endif
#ifndef TclWinNToHS
#define TclWinNToHS(ns) \
	(tclIntPlatStubsPtr->tclWinNToHS)(ns) /* 6 */
#endif
#ifndef TclWinSetSockOpt
#define TclWinSetSockOpt(s, level, optname, optval, optlen) \
	(tclIntPlatStubsPtr->tclWinSetSockOpt)(s, level, optname, optval, optlen) /* 7 */
#endif
#ifndef TclpGetPid
#define TclpGetPid(pid) \
	(tclIntPlatStubsPtr->tclpGetPid)(pid) /* 8 */
#endif
#ifndef TclWinGetPlatformId
#define TclWinGetPlatformId() \
	(tclIntPlatStubsPtr->tclWinGetPlatformId)() /* 9 */
#endif
#ifndef TclWinSynchSpawn
#define TclWinSynchSpawn(args, type, trans, pidPtr) \
	(tclIntPlatStubsPtr->tclWinSynchSpawn)(args, type, trans, pidPtr) /* 10 */
#endif
#ifndef TclGetAndDetachPids
#define TclGetAndDetachPids(interp, chan) \
	(tclIntPlatStubsPtr->tclGetAndDetachPids)(interp, chan) /* 11 */
#endif
#ifndef TclpCloseFile
#define TclpCloseFile(file) \
	(tclIntPlatStubsPtr->tclpCloseFile)(file) /* 12 */
#endif
#ifndef TclpCreateCommandChannel
#define TclpCreateCommandChannel(readFile, writeFile, errorFile, numPids, pidPtr) \
	(tclIntPlatStubsPtr->tclpCreateCommandChannel)(readFile, writeFile, errorFile, numPids, pidPtr) /* 13 */
#endif
#ifndef TclpCreatePipe
#define TclpCreatePipe(readPipe, writePipe) \
	(tclIntPlatStubsPtr->tclpCreatePipe)(readPipe, writePipe) /* 14 */
#endif
#ifndef TclpCreateProcess
#define TclpCreateProcess(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr) \
	(tclIntPlatStubsPtr->tclpCreateProcess)(interp, argc, argv, inputFile, outputFile, errorFile, pidPtr) /* 15 */
#endif
#ifndef TclpCreateTempFile
#define TclpCreateTempFile(contents, namePtr) \
	(tclIntPlatStubsPtr->tclpCreateTempFile)(contents, namePtr) /* 16 */
#endif
#ifndef TclpGetTZName
#define TclpGetTZName() \
	(tclIntPlatStubsPtr->tclpGetTZName)() /* 17 */
#endif
#ifndef TclpMakeFile
#define TclpMakeFile(channel, direction) \
	(tclIntPlatStubsPtr->tclpMakeFile)(channel, direction) /* 18 */
#endif
#ifndef TclpOpenFile
#define TclpOpenFile(fname, mode) \
	(tclIntPlatStubsPtr->tclpOpenFile)(fname, mode) /* 19 */
#endif
#endif /* __WIN32__ */
#ifdef MAC_TCL
#ifndef TclpSysAlloc
#define TclpSysAlloc(size, isBin) \
	(tclIntPlatStubsPtr->tclpSysAlloc)(size, isBin) /* 0 */
#endif
#ifndef TclpSysFree
#define TclpSysFree(ptr) \
	(tclIntPlatStubsPtr->tclpSysFree)(ptr) /* 1 */
#endif
#ifndef TclpSysRealloc
#define TclpSysRealloc(cp, size) \
	(tclIntPlatStubsPtr->tclpSysRealloc)(cp, size) /* 2 */
#endif
#ifndef TclPlatformExit
#define TclPlatformExit(status) \
	(tclIntPlatStubsPtr->tclPlatformExit)(status) /* 3 */
#endif
#ifndef FSpGetDefaultDir
#define FSpGetDefaultDir(theSpec) \
	(tclIntPlatStubsPtr->fSpGetDefaultDir)(theSpec) /* 4 */
#endif
#ifndef FSpSetDefaultDir
#define FSpSetDefaultDir(theSpec) \
	(tclIntPlatStubsPtr->fSpSetDefaultDir)(theSpec) /* 5 */
#endif
#ifndef FSpFindFolder
#define FSpFindFolder(vRefNum, folderType, createFolder, spec) \
	(tclIntPlatStubsPtr->fSpFindFolder)(vRefNum, folderType, createFolder, spec) /* 6 */
#endif
#ifndef GetGlobalMouse
#define GetGlobalMouse(mouse) \
	(tclIntPlatStubsPtr->getGlobalMouse)(mouse) /* 7 */
#endif
#ifndef FSpGetDirectoryID
#define FSpGetDirectoryID(spec, theDirID, isDirectory) \
	(tclIntPlatStubsPtr->fSpGetDirectoryID)(spec, theDirID, isDirectory) /* 8 */
#endif
#ifndef FSpOpenResFileCompat
#define FSpOpenResFileCompat(spec, permission) \
	(tclIntPlatStubsPtr->fSpOpenResFileCompat)(spec, permission) /* 9 */
#endif
#ifndef FSpCreateResFileCompat
#define FSpCreateResFileCompat(spec, creator, fileType, scriptTag) \
	(tclIntPlatStubsPtr->fSpCreateResFileCompat)(spec, creator, fileType, scriptTag) /* 10 */
#endif
#ifndef FSpLocationFromPath
#define FSpLocationFromPath(length, path, theSpec) \
	(tclIntPlatStubsPtr->fSpLocationFromPath)(length, path, theSpec) /* 11 */
#endif
#ifndef FSpPathFromLocation
#define FSpPathFromLocation(theSpec, length, fullPath) \
	(tclIntPlatStubsPtr->fSpPathFromLocation)(theSpec, length, fullPath) /* 12 */
#endif
#ifndef TclMacExitHandler
#define TclMacExitHandler() \
	(tclIntPlatStubsPtr->tclMacExitHandler)() /* 13 */
#endif
#ifndef TclMacInitExitToShell
#define TclMacInitExitToShell(usePatch) \
	(tclIntPlatStubsPtr->tclMacInitExitToShell)(usePatch) /* 14 */
#endif
#ifndef TclMacInstallExitToShellPatch
#define TclMacInstallExitToShellPatch(newProc) \
	(tclIntPlatStubsPtr->tclMacInstallExitToShellPatch)(newProc) /* 15 */
#endif
#ifndef TclMacOSErrorToPosixError
#define TclMacOSErrorToPosixError(error) \
	(tclIntPlatStubsPtr->tclMacOSErrorToPosixError)(error) /* 16 */
#endif
#ifndef TclMacRemoveTimer
#define TclMacRemoveTimer(timerToken) \
	(tclIntPlatStubsPtr->tclMacRemoveTimer)(timerToken) /* 17 */
#endif
#ifndef TclMacStartTimer
#define TclMacStartTimer(ms) \
	(tclIntPlatStubsPtr->tclMacStartTimer)(ms) /* 18 */
#endif
#ifndef TclMacTimerExpired
#define TclMacTimerExpired(timerToken) \
	(tclIntPlatStubsPtr->tclMacTimerExpired)(timerToken) /* 19 */
#endif
#ifndef TclMacRegisterResourceFork
#define TclMacRegisterResourceFork(fileRef, tokenPtr, insert) \
	(tclIntPlatStubsPtr->tclMacRegisterResourceFork)(fileRef, tokenPtr, insert) /* 20 */
#endif
#ifndef TclMacUnRegisterResourceFork
#define TclMacUnRegisterResourceFork(tokenPtr, resultPtr) \
	(tclIntPlatStubsPtr->tclMacUnRegisterResourceFork)(tokenPtr, resultPtr) /* 21 */
#endif
#ifndef TclMacCreateEnv
#define TclMacCreateEnv() \
	(tclIntPlatStubsPtr->tclMacCreateEnv)() /* 22 */
#endif
#ifndef TclMacFOpenHack
#define TclMacFOpenHack(path, mode) \
	(tclIntPlatStubsPtr->tclMacFOpenHack)(path, mode) /* 23 */
#endif
#ifndef TclMacReadlink
#define TclMacReadlink(path, buf, size) \
	(tclIntPlatStubsPtr->tclMacReadlink)(path, buf, size) /* 24 */
#endif
#ifndef TclMacChmod
#define TclMacChmod(path, mode) \
	(tclIntPlatStubsPtr->tclMacChmod)(path, mode) /* 25 */
#endif
#endif /* MAC_TCL */

#endif /* defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS) */

/* !END!: Do not edit above this line. */

#endif /* _TCLINTPLATDECLS */
