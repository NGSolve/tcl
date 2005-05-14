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
 * RCS: @(#) $Id: tclIntPlatDecls.h,v 1.27 2005/05/14 20:46:45 das Exp $
 */

#ifndef _TCLINTPLATDECLS
#define _TCLINTPLATDECLS

#undef TCL_STORAGE_CLASS
#ifdef BUILD_tcl
#   define TCL_STORAGE_CLASS DLLEXPORT
#else
#   ifdef USE_TCL_STUBS
#      define TCL_STORAGE_CLASS
#   else
#      define TCL_STORAGE_CLASS DLLIMPORT
#   endif
#endif

/*
 * WARNING: This file is automatically generated by the tools/genStubs.tcl
 * script.  Any modifications to the function declarations below should be made
 * in the generic/tclInt.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

/*
 * Exported function declarations:
 */

#if !defined(__WIN32__) /* UNIX */
#ifndef TclGetAndDetachPids_TCL_DECLARED
#define TclGetAndDetachPids_TCL_DECLARED
/* 0 */
EXTERN void		TclGetAndDetachPids _ANSI_ARGS_((Tcl_Interp * interp, 
				Tcl_Channel chan));
#endif
#ifndef TclpCloseFile_TCL_DECLARED
#define TclpCloseFile_TCL_DECLARED
/* 1 */
EXTERN int		TclpCloseFile _ANSI_ARGS_((TclFile file));
#endif
#ifndef TclpCreateCommandChannel_TCL_DECLARED
#define TclpCreateCommandChannel_TCL_DECLARED
/* 2 */
EXTERN Tcl_Channel	TclpCreateCommandChannel _ANSI_ARGS_((
				TclFile readFile, TclFile writeFile, 
				TclFile errorFile, int numPids, 
				Tcl_Pid * pidPtr));
#endif
#ifndef TclpCreatePipe_TCL_DECLARED
#define TclpCreatePipe_TCL_DECLARED
/* 3 */
EXTERN int		TclpCreatePipe _ANSI_ARGS_((TclFile * readPipe, 
				TclFile * writePipe));
#endif
#ifndef TclpCreateProcess_TCL_DECLARED
#define TclpCreateProcess_TCL_DECLARED
/* 4 */
EXTERN int		TclpCreateProcess _ANSI_ARGS_((Tcl_Interp * interp, 
				int argc, CONST char ** argv, 
				TclFile inputFile, TclFile outputFile, 
				TclFile errorFile, Tcl_Pid * pidPtr));
#endif
/* Slot 5 is reserved */
#ifndef TclpMakeFile_TCL_DECLARED
#define TclpMakeFile_TCL_DECLARED
/* 6 */
EXTERN TclFile		TclpMakeFile _ANSI_ARGS_((Tcl_Channel channel, 
				int direction));
#endif
#ifndef TclpOpenFile_TCL_DECLARED
#define TclpOpenFile_TCL_DECLARED
/* 7 */
EXTERN TclFile		TclpOpenFile _ANSI_ARGS_((CONST char * fname, 
				int mode));
#endif
#ifndef TclUnixWaitForFile_TCL_DECLARED
#define TclUnixWaitForFile_TCL_DECLARED
/* 8 */
EXTERN int		TclUnixWaitForFile _ANSI_ARGS_((int fd, int mask, 
				int timeout));
#endif
#ifndef TclpCreateTempFile_TCL_DECLARED
#define TclpCreateTempFile_TCL_DECLARED
/* 9 */
EXTERN TclFile		TclpCreateTempFile _ANSI_ARGS_((
				CONST char * contents));
#endif
#ifndef TclpReaddir_TCL_DECLARED
#define TclpReaddir_TCL_DECLARED
/* 10 */
EXTERN Tcl_DirEntry *	TclpReaddir _ANSI_ARGS_((DIR * dir));
#endif
#ifndef TclpLocaltime_unix_TCL_DECLARED
#define TclpLocaltime_unix_TCL_DECLARED
/* 11 */
EXTERN struct tm *	TclpLocaltime_unix _ANSI_ARGS_((CONST time_t * clock));
#endif
#ifndef TclpGmtime_unix_TCL_DECLARED
#define TclpGmtime_unix_TCL_DECLARED
/* 12 */
EXTERN struct tm *	TclpGmtime_unix _ANSI_ARGS_((CONST time_t * clock));
#endif
#ifndef TclpInetNtoa_TCL_DECLARED
#define TclpInetNtoa_TCL_DECLARED
/* 13 */
EXTERN char *		TclpInetNtoa _ANSI_ARGS_((struct in_addr addr));
#endif
#ifndef TclUnixCopyFile_TCL_DECLARED
#define TclUnixCopyFile_TCL_DECLARED
/* 14 */
EXTERN int		TclUnixCopyFile _ANSI_ARGS_((CONST char * src, 
				CONST char * dst, 
				CONST Tcl_StatBuf * statBufPtr, 
				int dontCopyAtts));
#endif
#endif /* UNIX */
#ifdef __WIN32__
#ifndef TclWinConvertError_TCL_DECLARED
#define TclWinConvertError_TCL_DECLARED
/* 0 */
EXTERN void		TclWinConvertError _ANSI_ARGS_((DWORD errCode));
#endif
#ifndef TclWinConvertWSAError_TCL_DECLARED
#define TclWinConvertWSAError_TCL_DECLARED
/* 1 */
EXTERN void		TclWinConvertWSAError _ANSI_ARGS_((DWORD errCode));
#endif
#ifndef TclWinGetServByName_TCL_DECLARED
#define TclWinGetServByName_TCL_DECLARED
/* 2 */
EXTERN struct servent *	 TclWinGetServByName _ANSI_ARGS_((CONST char * nm, 
				CONST char * proto));
#endif
#ifndef TclWinGetSockOpt_TCL_DECLARED
#define TclWinGetSockOpt_TCL_DECLARED
/* 3 */
EXTERN int		TclWinGetSockOpt _ANSI_ARGS_((SOCKET s, int level, 
				int optname, char FAR * optval, 
				int FAR * optlen));
#endif
#ifndef TclWinGetTclInstance_TCL_DECLARED
#define TclWinGetTclInstance_TCL_DECLARED
/* 4 */
EXTERN HINSTANCE	TclWinGetTclInstance _ANSI_ARGS_((void));
#endif
/* Slot 5 is reserved */
#ifndef TclWinNToHS_TCL_DECLARED
#define TclWinNToHS_TCL_DECLARED
/* 6 */
EXTERN u_short		TclWinNToHS _ANSI_ARGS_((u_short ns));
#endif
#ifndef TclWinSetSockOpt_TCL_DECLARED
#define TclWinSetSockOpt_TCL_DECLARED
/* 7 */
EXTERN int		TclWinSetSockOpt _ANSI_ARGS_((SOCKET s, int level, 
				int optname, CONST char FAR * optval, 
				int optlen));
#endif
#ifndef TclpGetPid_TCL_DECLARED
#define TclpGetPid_TCL_DECLARED
/* 8 */
EXTERN unsigned long	TclpGetPid _ANSI_ARGS_((Tcl_Pid pid));
#endif
#ifndef TclWinGetPlatformId_TCL_DECLARED
#define TclWinGetPlatformId_TCL_DECLARED
/* 9 */
EXTERN int		TclWinGetPlatformId _ANSI_ARGS_((void));
#endif
/* Slot 10 is reserved */
#ifndef TclGetAndDetachPids_TCL_DECLARED
#define TclGetAndDetachPids_TCL_DECLARED
/* 11 */
EXTERN void		TclGetAndDetachPids _ANSI_ARGS_((Tcl_Interp * interp, 
				Tcl_Channel chan));
#endif
#ifndef TclpCloseFile_TCL_DECLARED
#define TclpCloseFile_TCL_DECLARED
/* 12 */
EXTERN int		TclpCloseFile _ANSI_ARGS_((TclFile file));
#endif
#ifndef TclpCreateCommandChannel_TCL_DECLARED
#define TclpCreateCommandChannel_TCL_DECLARED
/* 13 */
EXTERN Tcl_Channel	TclpCreateCommandChannel _ANSI_ARGS_((
				TclFile readFile, TclFile writeFile, 
				TclFile errorFile, int numPids, 
				Tcl_Pid * pidPtr));
#endif
#ifndef TclpCreatePipe_TCL_DECLARED
#define TclpCreatePipe_TCL_DECLARED
/* 14 */
EXTERN int		TclpCreatePipe _ANSI_ARGS_((TclFile * readPipe, 
				TclFile * writePipe));
#endif
#ifndef TclpCreateProcess_TCL_DECLARED
#define TclpCreateProcess_TCL_DECLARED
/* 15 */
EXTERN int		TclpCreateProcess _ANSI_ARGS_((Tcl_Interp * interp, 
				int argc, CONST char ** argv, 
				TclFile inputFile, TclFile outputFile, 
				TclFile errorFile, Tcl_Pid * pidPtr));
#endif
/* Slot 16 is reserved */
/* Slot 17 is reserved */
#ifndef TclpMakeFile_TCL_DECLARED
#define TclpMakeFile_TCL_DECLARED
/* 18 */
EXTERN TclFile		TclpMakeFile _ANSI_ARGS_((Tcl_Channel channel, 
				int direction));
#endif
#ifndef TclpOpenFile_TCL_DECLARED
#define TclpOpenFile_TCL_DECLARED
/* 19 */
EXTERN TclFile		TclpOpenFile _ANSI_ARGS_((CONST char * fname, 
				int mode));
#endif
#ifndef TclWinAddProcess_TCL_DECLARED
#define TclWinAddProcess_TCL_DECLARED
/* 20 */
EXTERN void		TclWinAddProcess _ANSI_ARGS_((HANDLE hProcess, 
				DWORD id));
#endif
/* Slot 21 is reserved */
#ifndef TclpCreateTempFile_TCL_DECLARED
#define TclpCreateTempFile_TCL_DECLARED
/* 22 */
EXTERN TclFile		TclpCreateTempFile _ANSI_ARGS_((
				CONST char * contents));
#endif
#ifndef TclpGetTZName_TCL_DECLARED
#define TclpGetTZName_TCL_DECLARED
/* 23 */
EXTERN char *		TclpGetTZName _ANSI_ARGS_((int isdst));
#endif
#ifndef TclWinNoBackslash_TCL_DECLARED
#define TclWinNoBackslash_TCL_DECLARED
/* 24 */
EXTERN char *		TclWinNoBackslash _ANSI_ARGS_((char * path));
#endif
/* Slot 25 is reserved */
#ifndef TclWinSetInterfaces_TCL_DECLARED
#define TclWinSetInterfaces_TCL_DECLARED
/* 26 */
EXTERN void		TclWinSetInterfaces _ANSI_ARGS_((int wide));
#endif
#ifndef TclWinFlushDirtyChannels_TCL_DECLARED
#define TclWinFlushDirtyChannels_TCL_DECLARED
/* 27 */
EXTERN void		TclWinFlushDirtyChannels _ANSI_ARGS_((void));
#endif
#ifndef TclWinResetInterfaces_TCL_DECLARED
#define TclWinResetInterfaces_TCL_DECLARED
/* 28 */
EXTERN void		TclWinResetInterfaces _ANSI_ARGS_((void));
#endif
#ifndef TclWinCPUID_TCL_DECLARED
#define TclWinCPUID_TCL_DECLARED
/* 29 */
EXTERN int		TclWinCPUID _ANSI_ARGS_((unsigned int index, 
				unsigned int * regs));
#endif
#endif /* __WIN32__ */
#ifdef MAC_OSX_TCL
#ifndef TclMacOSXGetFileAttribute_TCL_DECLARED
#define TclMacOSXGetFileAttribute_TCL_DECLARED
/* 15 */
EXTERN int		TclMacOSXGetFileAttribute _ANSI_ARGS_((
				Tcl_Interp * interp, int objIndex, 
				Tcl_Obj * fileName, 
				Tcl_Obj ** attributePtrPtr));
#endif
#ifndef TclMacOSXSetFileAttribute_TCL_DECLARED
#define TclMacOSXSetFileAttribute_TCL_DECLARED
/* 16 */
EXTERN int		TclMacOSXSetFileAttribute _ANSI_ARGS_((
				Tcl_Interp * interp, int objIndex, 
				Tcl_Obj * fileName, Tcl_Obj * attributePtr));
#endif
#ifndef TclMacOSXCopyFileAttributes_TCL_DECLARED
#define TclMacOSXCopyFileAttributes_TCL_DECLARED
/* 17 */
EXTERN int		TclMacOSXCopyFileAttributes _ANSI_ARGS_((
				CONST char * src, CONST char * dst, 
				CONST Tcl_StatBuf * statBufPtr));
#endif
#endif /* MAC_OSX_TCL */

typedef struct TclIntPlatStubs {
    int magic;
    struct TclIntPlatStubHooks *hooks;

#if !defined(__WIN32__) /* UNIX */
    void (*tclGetAndDetachPids) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Channel chan)); /* 0 */
    int (*tclpCloseFile) _ANSI_ARGS_((TclFile file)); /* 1 */
    Tcl_Channel (*tclpCreateCommandChannel) _ANSI_ARGS_((TclFile readFile, TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid * pidPtr)); /* 2 */
    int (*tclpCreatePipe) _ANSI_ARGS_((TclFile * readPipe, TclFile * writePipe)); /* 3 */
    int (*tclpCreateProcess) _ANSI_ARGS_((Tcl_Interp * interp, int argc, CONST char ** argv, TclFile inputFile, TclFile outputFile, TclFile errorFile, Tcl_Pid * pidPtr)); /* 4 */
    void *reserved5;
    TclFile (*tclpMakeFile) _ANSI_ARGS_((Tcl_Channel channel, int direction)); /* 6 */
    TclFile (*tclpOpenFile) _ANSI_ARGS_((CONST char * fname, int mode)); /* 7 */
    int (*tclUnixWaitForFile) _ANSI_ARGS_((int fd, int mask, int timeout)); /* 8 */
    TclFile (*tclpCreateTempFile) _ANSI_ARGS_((CONST char * contents)); /* 9 */
    Tcl_DirEntry * (*tclpReaddir) _ANSI_ARGS_((DIR * dir)); /* 10 */
    struct tm * (*tclpLocaltime_unix) _ANSI_ARGS_((CONST time_t * clock)); /* 11 */
    struct tm * (*tclpGmtime_unix) _ANSI_ARGS_((CONST time_t * clock)); /* 12 */
    char * (*tclpInetNtoa) _ANSI_ARGS_((struct in_addr addr)); /* 13 */
    int (*tclUnixCopyFile) _ANSI_ARGS_((CONST char * src, CONST char * dst, CONST Tcl_StatBuf * statBufPtr, int dontCopyAtts)); /* 14 */
#endif /* UNIX */
#ifdef __WIN32__
    void (*tclWinConvertError) _ANSI_ARGS_((DWORD errCode)); /* 0 */
    void (*tclWinConvertWSAError) _ANSI_ARGS_((DWORD errCode)); /* 1 */
    struct servent * (*tclWinGetServByName) _ANSI_ARGS_((CONST char * nm, CONST char * proto)); /* 2 */
    int (*tclWinGetSockOpt) _ANSI_ARGS_((SOCKET s, int level, int optname, char FAR * optval, int FAR * optlen)); /* 3 */
    HINSTANCE (*tclWinGetTclInstance) _ANSI_ARGS_((void)); /* 4 */
    void *reserved5;
    u_short (*tclWinNToHS) _ANSI_ARGS_((u_short ns)); /* 6 */
    int (*tclWinSetSockOpt) _ANSI_ARGS_((SOCKET s, int level, int optname, CONST char FAR * optval, int optlen)); /* 7 */
    unsigned long (*tclpGetPid) _ANSI_ARGS_((Tcl_Pid pid)); /* 8 */
    int (*tclWinGetPlatformId) _ANSI_ARGS_((void)); /* 9 */
    void *reserved10;
    void (*tclGetAndDetachPids) _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Channel chan)); /* 11 */
    int (*tclpCloseFile) _ANSI_ARGS_((TclFile file)); /* 12 */
    Tcl_Channel (*tclpCreateCommandChannel) _ANSI_ARGS_((TclFile readFile, TclFile writeFile, TclFile errorFile, int numPids, Tcl_Pid * pidPtr)); /* 13 */
    int (*tclpCreatePipe) _ANSI_ARGS_((TclFile * readPipe, TclFile * writePipe)); /* 14 */
    int (*tclpCreateProcess) _ANSI_ARGS_((Tcl_Interp * interp, int argc, CONST char ** argv, TclFile inputFile, TclFile outputFile, TclFile errorFile, Tcl_Pid * pidPtr)); /* 15 */
    void *reserved16;
    void *reserved17;
    TclFile (*tclpMakeFile) _ANSI_ARGS_((Tcl_Channel channel, int direction)); /* 18 */
    TclFile (*tclpOpenFile) _ANSI_ARGS_((CONST char * fname, int mode)); /* 19 */
    void (*tclWinAddProcess) _ANSI_ARGS_((HANDLE hProcess, DWORD id)); /* 20 */
    void *reserved21;
    TclFile (*tclpCreateTempFile) _ANSI_ARGS_((CONST char * contents)); /* 22 */
    char * (*tclpGetTZName) _ANSI_ARGS_((int isdst)); /* 23 */
    char * (*tclWinNoBackslash) _ANSI_ARGS_((char * path)); /* 24 */
    void *reserved25;
    void (*tclWinSetInterfaces) _ANSI_ARGS_((int wide)); /* 26 */
    void (*tclWinFlushDirtyChannels) _ANSI_ARGS_((void)); /* 27 */
    void (*tclWinResetInterfaces) _ANSI_ARGS_((void)); /* 28 */
    int (*tclWinCPUID) _ANSI_ARGS_((unsigned int index, unsigned int * regs)); /* 29 */
#endif /* __WIN32__ */
#ifdef MAC_OSX_TCL
    int (*tclMacOSXGetFileAttribute) _ANSI_ARGS_((Tcl_Interp * interp, int objIndex, Tcl_Obj * fileName, Tcl_Obj ** attributePtrPtr)); /* 15 */
    int (*tclMacOSXSetFileAttribute) _ANSI_ARGS_((Tcl_Interp * interp, int objIndex, Tcl_Obj * fileName, Tcl_Obj * attributePtr)); /* 16 */
    int (*tclMacOSXCopyFileAttributes) _ANSI_ARGS_((CONST char * src, CONST char * dst, CONST Tcl_StatBuf * statBufPtr)); /* 17 */
#endif /* MAC_OSX_TCL */
} TclIntPlatStubs;

#ifdef __cplusplus
extern "C" {
#endif
extern TclIntPlatStubs *tclIntPlatStubsPtr;
#ifdef __cplusplus
}
#endif

#if defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS)

/*
 * Inline function declarations:
 */

#if !defined(__WIN32__) /* UNIX */
#ifndef TclGetAndDetachPids
#define TclGetAndDetachPids \
	(tclIntPlatStubsPtr->tclGetAndDetachPids) /* 0 */
#endif
#ifndef TclpCloseFile
#define TclpCloseFile \
	(tclIntPlatStubsPtr->tclpCloseFile) /* 1 */
#endif
#ifndef TclpCreateCommandChannel
#define TclpCreateCommandChannel \
	(tclIntPlatStubsPtr->tclpCreateCommandChannel) /* 2 */
#endif
#ifndef TclpCreatePipe
#define TclpCreatePipe \
	(tclIntPlatStubsPtr->tclpCreatePipe) /* 3 */
#endif
#ifndef TclpCreateProcess
#define TclpCreateProcess \
	(tclIntPlatStubsPtr->tclpCreateProcess) /* 4 */
#endif
/* Slot 5 is reserved */
#ifndef TclpMakeFile
#define TclpMakeFile \
	(tclIntPlatStubsPtr->tclpMakeFile) /* 6 */
#endif
#ifndef TclpOpenFile
#define TclpOpenFile \
	(tclIntPlatStubsPtr->tclpOpenFile) /* 7 */
#endif
#ifndef TclUnixWaitForFile
#define TclUnixWaitForFile \
	(tclIntPlatStubsPtr->tclUnixWaitForFile) /* 8 */
#endif
#ifndef TclpCreateTempFile
#define TclpCreateTempFile \
	(tclIntPlatStubsPtr->tclpCreateTempFile) /* 9 */
#endif
#ifndef TclpReaddir
#define TclpReaddir \
	(tclIntPlatStubsPtr->tclpReaddir) /* 10 */
#endif
#ifndef TclpLocaltime_unix
#define TclpLocaltime_unix \
	(tclIntPlatStubsPtr->tclpLocaltime_unix) /* 11 */
#endif
#ifndef TclpGmtime_unix
#define TclpGmtime_unix \
	(tclIntPlatStubsPtr->tclpGmtime_unix) /* 12 */
#endif
#ifndef TclpInetNtoa
#define TclpInetNtoa \
	(tclIntPlatStubsPtr->tclpInetNtoa) /* 13 */
#endif
#ifndef TclUnixCopyFile
#define TclUnixCopyFile \
	(tclIntPlatStubsPtr->tclUnixCopyFile) /* 14 */
#endif
#endif /* UNIX */
#ifdef __WIN32__
#ifndef TclWinConvertError
#define TclWinConvertError \
	(tclIntPlatStubsPtr->tclWinConvertError) /* 0 */
#endif
#ifndef TclWinConvertWSAError
#define TclWinConvertWSAError \
	(tclIntPlatStubsPtr->tclWinConvertWSAError) /* 1 */
#endif
#ifndef TclWinGetServByName
#define TclWinGetServByName \
	(tclIntPlatStubsPtr->tclWinGetServByName) /* 2 */
#endif
#ifndef TclWinGetSockOpt
#define TclWinGetSockOpt \
	(tclIntPlatStubsPtr->tclWinGetSockOpt) /* 3 */
#endif
#ifndef TclWinGetTclInstance
#define TclWinGetTclInstance \
	(tclIntPlatStubsPtr->tclWinGetTclInstance) /* 4 */
#endif
/* Slot 5 is reserved */
#ifndef TclWinNToHS
#define TclWinNToHS \
	(tclIntPlatStubsPtr->tclWinNToHS) /* 6 */
#endif
#ifndef TclWinSetSockOpt
#define TclWinSetSockOpt \
	(tclIntPlatStubsPtr->tclWinSetSockOpt) /* 7 */
#endif
#ifndef TclpGetPid
#define TclpGetPid \
	(tclIntPlatStubsPtr->tclpGetPid) /* 8 */
#endif
#ifndef TclWinGetPlatformId
#define TclWinGetPlatformId \
	(tclIntPlatStubsPtr->tclWinGetPlatformId) /* 9 */
#endif
/* Slot 10 is reserved */
#ifndef TclGetAndDetachPids
#define TclGetAndDetachPids \
	(tclIntPlatStubsPtr->tclGetAndDetachPids) /* 11 */
#endif
#ifndef TclpCloseFile
#define TclpCloseFile \
	(tclIntPlatStubsPtr->tclpCloseFile) /* 12 */
#endif
#ifndef TclpCreateCommandChannel
#define TclpCreateCommandChannel \
	(tclIntPlatStubsPtr->tclpCreateCommandChannel) /* 13 */
#endif
#ifndef TclpCreatePipe
#define TclpCreatePipe \
	(tclIntPlatStubsPtr->tclpCreatePipe) /* 14 */
#endif
#ifndef TclpCreateProcess
#define TclpCreateProcess \
	(tclIntPlatStubsPtr->tclpCreateProcess) /* 15 */
#endif
/* Slot 16 is reserved */
/* Slot 17 is reserved */
#ifndef TclpMakeFile
#define TclpMakeFile \
	(tclIntPlatStubsPtr->tclpMakeFile) /* 18 */
#endif
#ifndef TclpOpenFile
#define TclpOpenFile \
	(tclIntPlatStubsPtr->tclpOpenFile) /* 19 */
#endif
#ifndef TclWinAddProcess
#define TclWinAddProcess \
	(tclIntPlatStubsPtr->tclWinAddProcess) /* 20 */
#endif
/* Slot 21 is reserved */
#ifndef TclpCreateTempFile
#define TclpCreateTempFile \
	(tclIntPlatStubsPtr->tclpCreateTempFile) /* 22 */
#endif
#ifndef TclpGetTZName
#define TclpGetTZName \
	(tclIntPlatStubsPtr->tclpGetTZName) /* 23 */
#endif
#ifndef TclWinNoBackslash
#define TclWinNoBackslash \
	(tclIntPlatStubsPtr->tclWinNoBackslash) /* 24 */
#endif
/* Slot 25 is reserved */
#ifndef TclWinSetInterfaces
#define TclWinSetInterfaces \
	(tclIntPlatStubsPtr->tclWinSetInterfaces) /* 26 */
#endif
#ifndef TclWinFlushDirtyChannels
#define TclWinFlushDirtyChannels \
	(tclIntPlatStubsPtr->tclWinFlushDirtyChannels) /* 27 */
#endif
#ifndef TclWinResetInterfaces
#define TclWinResetInterfaces \
	(tclIntPlatStubsPtr->tclWinResetInterfaces) /* 28 */
#endif
#ifndef TclWinCPUID
#define TclWinCPUID \
	(tclIntPlatStubsPtr->tclWinCPUID) /* 29 */
#endif
#endif /* __WIN32__ */
#ifdef MAC_OSX_TCL
#ifndef TclMacOSXGetFileAttribute
#define TclMacOSXGetFileAttribute \
	(tclIntPlatStubsPtr->tclMacOSXGetFileAttribute) /* 15 */
#endif
#ifndef TclMacOSXSetFileAttribute
#define TclMacOSXSetFileAttribute \
	(tclIntPlatStubsPtr->tclMacOSXSetFileAttribute) /* 16 */
#endif
#ifndef TclMacOSXCopyFileAttributes
#define TclMacOSXCopyFileAttributes \
	(tclIntPlatStubsPtr->tclMacOSXCopyFileAttributes) /* 17 */
#endif
#endif /* MAC_OSX_TCL */

#endif /* defined(USE_TCL_STUBS) && !defined(USE_TCL_STUB_PROCS) */

/* !END!: Do not edit above this line. */

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif /* _TCLINTPLATDECLS */
