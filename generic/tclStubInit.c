/* 
 * tclStubInit.c --
 *
 *	This file contains the initializers for the Tcl stub vectors.
 *
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclStubInit.c,v 1.2.4.3 1999/03/10 06:41:52 stanton Exp $
 */

#include "tclInt.h"
#include "tclPort.h"

/*
 * Remove macros that will interfere with the definitions below.
 */

#undef Tcl_Alloc
#undef Tcl_Free
#undef Tcl_Realloc
#undef Tcl_NewBooleanObj
#undef Tcl_NewByteArrayObj
#undef Tcl_NewDoubleObj
#undef Tcl_NewIntObj
#undef Tcl_NewListObj
#undef Tcl_NewLongObj
#undef Tcl_NewObj
#undef Tcl_NewStringObj
#undef Tcl_DumpActiveMemory
#undef Tcl_ValidateAllMemory

/*
 * WARNING: The contents of this file is automatically generated by the
 * tools/genStubs.tcl script. Any modifications to the function declarations
 * below should be made in the generic/tcl.decls script.
 */

/* !BEGIN!: Do not edit below this line. */

static TclStubHooks tclStubHooks;

TclStubs tclStubs = {
    TCL_STUB_MAGIC,
    &tclStubHooks,
    Tcl_PkgProvideEx, /* 0 */
    Tcl_PkgRequireEx, /* 1 */
    panic, /* 2 */
    Tcl_Alloc, /* 3 */
    Tcl_Free, /* 4 */
    Tcl_Realloc, /* 5 */
    Tcl_DbCkalloc, /* 6 */
    Tcl_DbCkfree, /* 7 */
    Tcl_DbCkrealloc, /* 8 */
#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
    Tcl_CreateFileHandler, /* 9 */
#endif /* UNIX */
#ifdef __WIN32__
    NULL, /* 9 */
#endif /* __WIN32__ */
#ifdef MAC_TCL
    NULL, /* 9 */
#endif /* MAC_TCL */
#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
    Tcl_DeleteFileHandler, /* 10 */
#endif /* UNIX */
#ifdef __WIN32__
    NULL, /* 10 */
#endif /* __WIN32__ */
#ifdef MAC_TCL
    NULL, /* 10 */
#endif /* MAC_TCL */
    Tcl_SetTimer, /* 11 */
    Tcl_Sleep, /* 12 */
    Tcl_WaitForEvent, /* 13 */
    Tcl_AppendAllObjTypes, /* 14 */
    Tcl_AppendStringsToObj, /* 15 */
    Tcl_AppendToObj, /* 16 */
    Tcl_ConcatObj, /* 17 */
    Tcl_ConvertToType, /* 18 */
    Tcl_DbDecrRefCount, /* 19 */
    Tcl_DbIncrRefCount, /* 20 */
    Tcl_DbIsShared, /* 21 */
    Tcl_DbNewBooleanObj, /* 22 */
    Tcl_DbNewByteArrayObj, /* 23 */
    Tcl_DbNewDoubleObj, /* 24 */
    Tcl_DbNewListObj, /* 25 */
    Tcl_DbNewLongObj, /* 26 */
    Tcl_DbNewObj, /* 27 */
    Tcl_DbNewStringObj, /* 28 */
    Tcl_DuplicateObj, /* 29 */
    TclFreeObj, /* 30 */
    Tcl_GetBoolean, /* 31 */
    Tcl_GetBooleanFromObj, /* 32 */
    Tcl_GetByteArrayFromObj, /* 33 */
    Tcl_GetDouble, /* 34 */
    Tcl_GetDoubleFromObj, /* 35 */
    Tcl_GetIndexFromObj, /* 36 */
    Tcl_GetInt, /* 37 */
    Tcl_GetIntFromObj, /* 38 */
    Tcl_GetLongFromObj, /* 39 */
    Tcl_GetObjType, /* 40 */
    Tcl_GetStringFromObj, /* 41 */
    Tcl_InvalidateStringRep, /* 42 */
    Tcl_ListObjAppendList, /* 43 */
    Tcl_ListObjAppendElement, /* 44 */
    Tcl_ListObjGetElements, /* 45 */
    Tcl_ListObjIndex, /* 46 */
    Tcl_ListObjLength, /* 47 */
    Tcl_ListObjReplace, /* 48 */
    Tcl_NewBooleanObj, /* 49 */
    Tcl_NewByteArrayObj, /* 50 */
    Tcl_NewDoubleObj, /* 51 */
    Tcl_NewIntObj, /* 52 */
    Tcl_NewListObj, /* 53 */
    Tcl_NewLongObj, /* 54 */
    Tcl_NewObj, /* 55 */
    Tcl_NewStringObj, /* 56 */
    Tcl_SetBooleanObj, /* 57 */
    Tcl_SetByteArrayLength, /* 58 */
    Tcl_SetByteArrayObj, /* 59 */
    Tcl_SetDoubleObj, /* 60 */
    Tcl_SetIntObj, /* 61 */
    Tcl_SetListObj, /* 62 */
    Tcl_SetLongObj, /* 63 */
    Tcl_SetObjLength, /* 64 */
    Tcl_SetStringObj, /* 65 */
    Tcl_AddErrorInfo, /* 66 */
    Tcl_AddObjErrorInfo, /* 67 */
    Tcl_AllowExceptions, /* 68 */
    Tcl_AppendElement, /* 69 */
    Tcl_AppendResult, /* 70 */
    Tcl_AsyncCreate, /* 71 */
    Tcl_AsyncDelete, /* 72 */
    Tcl_AsyncInvoke, /* 73 */
    Tcl_AsyncMark, /* 74 */
    Tcl_AsyncReady, /* 75 */
    Tcl_BackgroundError, /* 76 */
    Tcl_Backslash, /* 77 */
    Tcl_BadChannelOption, /* 78 */
    Tcl_CallWhenDeleted, /* 79 */
    Tcl_CancelIdleCall, /* 80 */
    Tcl_Close, /* 81 */
    Tcl_CommandComplete, /* 82 */
    Tcl_Concat, /* 83 */
    Tcl_ConvertElement, /* 84 */
    Tcl_ConvertCountedElement, /* 85 */
    Tcl_CreateAlias, /* 86 */
    Tcl_CreateAliasObj, /* 87 */
    Tcl_CreateChannel, /* 88 */
    Tcl_CreateChannelHandler, /* 89 */
    Tcl_CreateCloseHandler, /* 90 */
    Tcl_CreateCommand, /* 91 */
    Tcl_CreateEventSource, /* 92 */
    Tcl_CreateExitHandler, /* 93 */
    Tcl_CreateInterp, /* 94 */
    Tcl_CreateMathFunc, /* 95 */
    Tcl_CreateObjCommand, /* 96 */
    Tcl_CreateSlave, /* 97 */
    Tcl_CreateTimerHandler, /* 98 */
    Tcl_CreateTrace, /* 99 */
    Tcl_DeleteAssocData, /* 100 */
    Tcl_DeleteChannelHandler, /* 101 */
    Tcl_DeleteCloseHandler, /* 102 */
    Tcl_DeleteCommand, /* 103 */
    Tcl_DeleteCommandFromToken, /* 104 */
    Tcl_DeleteEvents, /* 105 */
    Tcl_DeleteEventSource, /* 106 */
    Tcl_DeleteExitHandler, /* 107 */
    Tcl_DeleteHashEntry, /* 108 */
    Tcl_DeleteHashTable, /* 109 */
    Tcl_DeleteInterp, /* 110 */
    Tcl_DetachPids, /* 111 */
    Tcl_DeleteTimerHandler, /* 112 */
    Tcl_DeleteTrace, /* 113 */
    Tcl_DontCallWhenDeleted, /* 114 */
    Tcl_DoOneEvent, /* 115 */
    Tcl_DoWhenIdle, /* 116 */
    Tcl_DStringAppend, /* 117 */
    Tcl_DStringAppendElement, /* 118 */
    Tcl_DStringEndSublist, /* 119 */
    Tcl_DStringFree, /* 120 */
    Tcl_DStringGetResult, /* 121 */
    Tcl_DStringInit, /* 122 */
    Tcl_DStringResult, /* 123 */
    Tcl_DStringSetLength, /* 124 */
    Tcl_DStringStartSublist, /* 125 */
    Tcl_Eof, /* 126 */
    Tcl_ErrnoId, /* 127 */
    Tcl_ErrnoMsg, /* 128 */
    Tcl_Eval, /* 129 */
    Tcl_EvalFile, /* 130 */
    Tcl_EvalObj, /* 131 */
    Tcl_EventuallyFree, /* 132 */
    Tcl_Exit, /* 133 */
    Tcl_ExposeCommand, /* 134 */
    Tcl_ExprBoolean, /* 135 */
    Tcl_ExprBooleanObj, /* 136 */
    Tcl_ExprDouble, /* 137 */
    Tcl_ExprDoubleObj, /* 138 */
    Tcl_ExprLong, /* 139 */
    Tcl_ExprLongObj, /* 140 */
    Tcl_ExprObj, /* 141 */
    Tcl_ExprString, /* 142 */
    Tcl_Finalize, /* 143 */
    Tcl_FindExecutable, /* 144 */
    Tcl_FirstHashEntry, /* 145 */
    Tcl_Flush, /* 146 */
    Tcl_FreeResult, /* 147 */
    Tcl_GetAlias, /* 148 */
    Tcl_GetAliasObj, /* 149 */
    Tcl_GetAssocData, /* 150 */
    Tcl_GetChannel, /* 151 */
    Tcl_GetChannelBufferSize, /* 152 */
    Tcl_GetChannelHandle, /* 153 */
    Tcl_GetChannelInstanceData, /* 154 */
    Tcl_GetChannelMode, /* 155 */
    Tcl_GetChannelName, /* 156 */
    Tcl_GetChannelOption, /* 157 */
    Tcl_GetChannelType, /* 158 */
    Tcl_GetCommandInfo, /* 159 */
    Tcl_GetCommandName, /* 160 */
    Tcl_GetErrno, /* 161 */
    Tcl_GetHostName, /* 162 */
    Tcl_GetInterpPath, /* 163 */
    Tcl_GetMaster, /* 164 */
    Tcl_GetNameOfExecutable, /* 165 */
    Tcl_GetObjResult, /* 166 */
#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
    Tcl_GetOpenFile, /* 167 */
#endif /* UNIX */
#ifdef __WIN32__
    NULL, /* 167 */
#endif /* __WIN32__ */
#ifdef MAC_TCL
    NULL, /* 167 */
#endif /* MAC_TCL */
    Tcl_GetPathType, /* 168 */
    Tcl_Gets, /* 169 */
    Tcl_GetsObj, /* 170 */
    Tcl_GetServiceMode, /* 171 */
    Tcl_GetSlave, /* 172 */
    Tcl_GetStdChannel, /* 173 */
    Tcl_GetStringResult, /* 174 */
    Tcl_GetVar, /* 175 */
    Tcl_GetVar2, /* 176 */
    Tcl_GlobalEval, /* 177 */
    Tcl_GlobalEvalObj, /* 178 */
    Tcl_HideCommand, /* 179 */
    Tcl_Init, /* 180 */
    Tcl_InitHashTable, /* 181 */
    Tcl_InputBlocked, /* 182 */
    Tcl_InputBuffered, /* 183 */
    Tcl_InterpDeleted, /* 184 */
    Tcl_IsSafe, /* 185 */
    Tcl_JoinPath, /* 186 */
    Tcl_LinkVar, /* 187 */
    NULL, /* 188 */
    Tcl_MakeFileChannel, /* 189 */
    Tcl_MakeSafe, /* 190 */
    Tcl_MakeTcpClientChannel, /* 191 */
    Tcl_Merge, /* 192 */
    Tcl_NextHashEntry, /* 193 */
    Tcl_NotifyChannel, /* 194 */
    Tcl_ObjGetVar2, /* 195 */
    Tcl_ObjSetVar2, /* 196 */
    Tcl_OpenCommandChannel, /* 197 */
    Tcl_OpenFileChannel, /* 198 */
    Tcl_OpenTcpClient, /* 199 */
    Tcl_OpenTcpServer, /* 200 */
    Tcl_Preserve, /* 201 */
    Tcl_PrintDouble, /* 202 */
    Tcl_PutEnv, /* 203 */
    Tcl_PosixError, /* 204 */
    Tcl_QueueEvent, /* 205 */
    Tcl_Read, /* 206 */
    Tcl_ReapDetachedProcs, /* 207 */
    Tcl_RecordAndEval, /* 208 */
    Tcl_RecordAndEvalObj, /* 209 */
    Tcl_RegisterChannel, /* 210 */
    Tcl_RegisterObjType, /* 211 */
    Tcl_RegExpCompile, /* 212 */
    Tcl_RegExpExec, /* 213 */
    Tcl_RegExpMatch, /* 214 */
    Tcl_RegExpRange, /* 215 */
    Tcl_Release, /* 216 */
    Tcl_ResetResult, /* 217 */
    Tcl_ScanElement, /* 218 */
    Tcl_ScanCountedElement, /* 219 */
    Tcl_Seek, /* 220 */
    Tcl_ServiceAll, /* 221 */
    Tcl_ServiceEvent, /* 222 */
    Tcl_SetAssocData, /* 223 */
    Tcl_SetChannelBufferSize, /* 224 */
    Tcl_SetChannelOption, /* 225 */
    Tcl_SetCommandInfo, /* 226 */
    Tcl_SetErrno, /* 227 */
    Tcl_SetErrorCode, /* 228 */
    Tcl_SetMaxBlockTime, /* 229 */
    Tcl_SetPanicProc, /* 230 */
    Tcl_SetRecursionLimit, /* 231 */
    Tcl_SetResult, /* 232 */
    Tcl_SetServiceMode, /* 233 */
    Tcl_SetObjErrorCode, /* 234 */
    Tcl_SetObjResult, /* 235 */
    Tcl_SetStdChannel, /* 236 */
    Tcl_SetVar, /* 237 */
    Tcl_SetVar2, /* 238 */
    Tcl_SignalId, /* 239 */
    Tcl_SignalMsg, /* 240 */
    Tcl_SourceRCFile, /* 241 */
    Tcl_SplitList, /* 242 */
    Tcl_SplitPath, /* 243 */
    Tcl_StaticPackage, /* 244 */
    Tcl_StringMatch, /* 245 */
    Tcl_Tell, /* 246 */
    Tcl_TraceVar, /* 247 */
    Tcl_TraceVar2, /* 248 */
    Tcl_TranslateFileName, /* 249 */
    Tcl_Ungets, /* 250 */
    Tcl_UnlinkVar, /* 251 */
    Tcl_UnregisterChannel, /* 252 */
    Tcl_UnsetVar, /* 253 */
    Tcl_UnsetVar2, /* 254 */
    Tcl_UntraceVar, /* 255 */
    Tcl_UntraceVar2, /* 256 */
    Tcl_UpdateLinkedVar, /* 257 */
    Tcl_UpVar, /* 258 */
    Tcl_UpVar2, /* 259 */
    Tcl_VarEval, /* 260 */
    Tcl_VarTraceInfo, /* 261 */
    Tcl_VarTraceInfo2, /* 262 */
    Tcl_Write, /* 263 */
    Tcl_WrongNumArgs, /* 264 */
    Tcl_DumpActiveMemory, /* 265 */
    Tcl_ValidateAllMemory, /* 266 */
    Tcl_AppendResultVA, /* 267 */
    Tcl_AppendStringsToObjVA, /* 268 */
    Tcl_HashStats, /* 269 */
    Tcl_ParseVar, /* 270 */
    Tcl_PkgPresent, /* 271 */
    Tcl_PkgPresentEx, /* 272 */
    Tcl_PkgProvide, /* 273 */
    Tcl_PkgRequire, /* 274 */
    Tcl_SetErrorCodeVA, /* 275 */
    Tcl_VarEvalVA, /* 276 */
    Tcl_WaitPid, /* 277 */
    panicVA, /* 278 */
    Tcl_AppendObjToObj, /* 279 */
    Tcl_AlertNotifier, /* 280 */
    Tcl_CreateEncoding, /* 281 */
    Tcl_CreateThreadExitHandler, /* 282 */
    Tcl_DeleteThreadExitHandler, /* 283 */
    Tcl_DiscardResult, /* 284 */
    Tcl_EvalEx, /* 285 */
    Tcl_EvalObjv, /* 286 */
    Tcl_EvalObjEx, /* 287 */
    Tcl_ExitThread, /* 288 */
    Tcl_ExternalToUtf, /* 289 */
    Tcl_ExternalToUtfDString, /* 290 */
    Tcl_FinalizeThread, /* 291 */
    Tcl_FinalizeNotifier, /* 292 */
    Tcl_FreeEncoding, /* 293 */
    Tcl_GetCurrentThread, /* 294 */
    Tcl_GetEncoding, /* 295 */
    Tcl_GetEncodingName, /* 296 */
    Tcl_GetEncodingNames, /* 297 */
    Tcl_GetIndexFromObjStruct, /* 298 */
    Tcl_GetThreadData, /* 299 */
    Tcl_GetVar2Ex, /* 300 */
    Tcl_InitNotifier, /* 301 */
    Tcl_MutexLock, /* 302 */
    Tcl_MutexUnlock, /* 303 */
    Tcl_ConditionNotify, /* 304 */
    Tcl_ConditionWait, /* 305 */
    Tcl_NumUtfChars, /* 306 */
    Tcl_ReadChars, /* 307 */
    Tcl_RestoreResult, /* 308 */
    Tcl_SaveResult, /* 309 */
    Tcl_SetSystemEncoding, /* 310 */
    Tcl_SetVar2Ex, /* 311 */
    Tcl_ThreadAlert, /* 312 */
    Tcl_ThreadQueueEvent, /* 313 */
    Tcl_UniCharAtIndex, /* 314 */
    Tcl_UniCharToLower, /* 315 */
    Tcl_UniCharToTitle, /* 316 */
    Tcl_UniCharToUpper, /* 317 */
    Tcl_UniCharToUtf, /* 318 */
    Tcl_UtfAtIndex, /* 319 */
    Tcl_UtfCharComplete, /* 320 */
    Tcl_UtfBackslash, /* 321 */
    Tcl_UtfFindFirst, /* 322 */
    Tcl_UtfFindLast, /* 323 */
    Tcl_UtfNext, /* 324 */
    Tcl_UtfPrev, /* 325 */
    Tcl_UtfToExternal, /* 326 */
    Tcl_UtfToExternalDString, /* 327 */
    Tcl_UtfToLower, /* 328 */
    Tcl_UtfToTitle, /* 329 */
    Tcl_UtfToUniChar, /* 330 */
    Tcl_UtfToUpper, /* 331 */
    Tcl_WriteChars, /* 332 */
    Tcl_WriteObj, /* 333 */
};

TclStubs *tclStubsPtr = &tclStubs;

TclIntStubs tclIntStubs = {
    TCL_STUB_MAGIC,
    NULL,
    TclAccess, /* 0 */
    TclAccessDeleteProc, /* 1 */
    TclAccessInsertProc, /* 2 */
    TclAllocateFreeObjects, /* 3 */
    NULL, /* 4 */
    TclCleanupChildren, /* 5 */
    TclCleanupCommand, /* 6 */
    TclCopyAndCollapse, /* 7 */
    TclCopyChannel, /* 8 */
    TclCreatePipeline, /* 9 */
    TclCreateProc, /* 10 */
    TclDeleteCompiledLocalVars, /* 11 */
    TclDeleteVars, /* 12 */
    TclDoGlob, /* 13 */
    TclDumpMemoryInfo, /* 14 */
    NULL, /* 15 */
    TclExprFloatError, /* 16 */
    TclFileAttrsCmd, /* 17 */
    TclFileCopyCmd, /* 18 */
    TclFileDeleteCmd, /* 19 */
    TclFileMakeDirsCmd, /* 20 */
    TclFileRenameCmd, /* 21 */
    TclFindElement, /* 22 */
    TclFindProc, /* 23 */
    TclFormatInt, /* 24 */
    TclFreePackageInfo, /* 25 */
    NULL, /* 26 */
    TclGetDate, /* 27 */
    TclpGetDefaultStdChannel, /* 28 */
    TclGetElementOfIndexedArray, /* 29 */
    NULL, /* 30 */
    TclGetExtension, /* 31 */
    TclGetFrame, /* 32 */
    TclGetInterpProc, /* 33 */
    TclGetIntForIndex, /* 34 */
    TclGetIndexedScalar, /* 35 */
    TclGetLong, /* 36 */
    TclGetLoadedPackages, /* 37 */
    TclGetNamespaceForQualName, /* 38 */
    TclGetObjInterpProc, /* 39 */
    TclGetOpenMode, /* 40 */
    TclGetOriginalCommand, /* 41 */
    TclpGetUserHome, /* 42 */
    TclGlobalInvoke, /* 43 */
    TclGuessPackageName, /* 44 */
    TclHideUnsafeCommands, /* 45 */
    TclInExit, /* 46 */
    TclIncrElementOfIndexedArray, /* 47 */
    TclIncrIndexedScalar, /* 48 */
    TclIncrVar2, /* 49 */
    TclInitCompiledLocals, /* 50 */
    TclInterpInit, /* 51 */
    TclInvoke, /* 52 */
    TclInvokeObjectCommand, /* 53 */
    TclInvokeStringCommand, /* 54 */
    TclIsProc, /* 55 */
    NULL, /* 56 */
    NULL, /* 57 */
    TclLookupVar, /* 58 */
    TclpMatchFiles, /* 59 */
    TclNeedSpace, /* 60 */
    TclNewProcBodyObj, /* 61 */
    TclObjCommandComplete, /* 62 */
    TclObjInterpProc, /* 63 */
    TclObjInvoke, /* 64 */
    TclObjInvokeGlobal, /* 65 */
    TclOpenFileChannelDeleteProc, /* 66 */
    TclOpenFileChannelInsertProc, /* 67 */
    TclpAccess, /* 68 */
    TclpAlloc, /* 69 */
    TclpCopyFile, /* 70 */
    TclpCopyDirectory, /* 71 */
    TclpCreateDirectory, /* 72 */
    TclpDeleteFile, /* 73 */
    TclpFree, /* 74 */
    TclpGetClicks, /* 75 */
    TclpGetSeconds, /* 76 */
    TclpGetTime, /* 77 */
    TclpGetTimeZone, /* 78 */
    TclpListVolumes, /* 79 */
    TclpOpenFileChannel, /* 80 */
    TclpRealloc, /* 81 */
    TclpRemoveDirectory, /* 82 */
    TclpRenameFile, /* 83 */
    NULL, /* 84 */
    NULL, /* 85 */
    NULL, /* 86 */
    NULL, /* 87 */
    TclPrecTraceProc, /* 88 */
    TclPreventAliasLoop, /* 89 */
    NULL, /* 90 */
    TclProcCleanupProc, /* 91 */
    TclProcCompileProc, /* 92 */
    TclProcDeleteProc, /* 93 */
    TclProcInterpProc, /* 94 */
    TclpStat, /* 95 */
    TclRenameCommand, /* 96 */
    TclResetShadowedCmdRefs, /* 97 */
    TclServiceIdle, /* 98 */
    TclSetElementOfIndexedArray, /* 99 */
    TclSetIndexedScalar, /* 100 */
    NULL, /* 101 */
    TclSetupEnv, /* 102 */
    TclSockGetPort, /* 103 */
    TclSockMinimumBuffers, /* 104 */
    TclStat, /* 105 */
    TclStatDeleteProc, /* 106 */
    TclStatInsertProc, /* 107 */
    TclTeardownNamespace, /* 108 */
    TclUpdateReturnInfo, /* 109 */
    NULL, /* 110 */
    Tcl_AddInterpResolvers, /* 111 */
    Tcl_AppendExportList, /* 112 */
    Tcl_CreateNamespace, /* 113 */
    Tcl_DeleteNamespace, /* 114 */
    Tcl_Export, /* 115 */
    Tcl_FindCommand, /* 116 */
    Tcl_FindNamespace, /* 117 */
    Tcl_GetInterpResolvers, /* 118 */
    Tcl_GetNamespaceResolvers, /* 119 */
    Tcl_FindNamespaceVar, /* 120 */
    Tcl_ForgetImport, /* 121 */
    Tcl_GetCommandFromObj, /* 122 */
    Tcl_GetCommandFullName, /* 123 */
    Tcl_GetCurrentNamespace, /* 124 */
    Tcl_GetGlobalNamespace, /* 125 */
    Tcl_GetVariableFullName, /* 126 */
    Tcl_Import, /* 127 */
    Tcl_PopCallFrame, /* 128 */
    Tcl_PushCallFrame, /* 129 */
    Tcl_RemoveInterpResolvers, /* 130 */
    Tcl_SetNamespaceResolvers, /* 131 */
    TclpHasSockets, /* 132 */
    TclpGetDate, /* 133 */
    TclpStrftime, /* 134 */
    TclpCheckStackSpace, /* 135 */
    Tcl_GetString, /* 136 */
    TclpChdir, /* 137 */
    TclGetEnv, /* 138 */
    TclpLoadFile, /* 139 */
    TclLooksLikeInt, /* 140 */
};

TclIntStubs *tclIntStubsPtr = &tclIntStubs;

TclIntPlatStubs tclIntPlatStubs = {
    TCL_STUB_MAGIC,
    NULL,
#if !defined(__WIN32__) && !defined(MAC_TCL) /* UNIX */
    TclGetAndDetachPids, /* 0 */
    TclpCloseFile, /* 1 */
    TclpCreateCommandChannel, /* 2 */
    TclpCreatePipe, /* 3 */
    TclpCreateProcess, /* 4 */
    NULL, /* 5 */
    TclpMakeFile, /* 6 */
    TclpOpenFile, /* 7 */
    TclUnixWaitForFile, /* 8 */
    TclpCreateTempFile, /* 9 */
#endif /* UNIX */
#ifdef __WIN32__
    TclWinConvertError, /* 0 */
    TclWinConvertWSAError, /* 1 */
    TclWinGetServByName, /* 2 */
    TclWinGetSockOpt, /* 3 */
    TclWinGetTclInstance, /* 4 */
    NULL, /* 5 */
    TclWinNToHS, /* 6 */
    TclWinSetSockOpt, /* 7 */
    TclpGetPid, /* 8 */
    TclWinGetPlatformId, /* 9 */
    TclWinSynchSpawn, /* 10 */
    TclGetAndDetachPids, /* 11 */
    TclpCloseFile, /* 12 */
    TclpCreateCommandChannel, /* 13 */
    TclpCreatePipe, /* 14 */
    TclpCreateProcess, /* 15 */
    NULL, /* 16 */
    NULL, /* 17 */
    TclpMakeFile, /* 18 */
    TclpOpenFile, /* 19 */
    TclpCreateTempFile, /* 20 */
    TclpGetTZName, /* 21 */
    TclWinNoBackslash, /* 22 */
    Tcl_WinUtfToTChar, /* 23 */
    Tcl_WinTCharToUtf, /* 24 */
#endif /* __WIN32__ */
#ifdef MAC_TCL
    TclpSysAlloc, /* 0 */
    TclpSysFree, /* 1 */
    TclpSysRealloc, /* 2 */
    TclpExit, /* 3 */
    FSpGetDefaultDir, /* 4 */
    FSpSetDefaultDir, /* 5 */
    FSpFindFolder, /* 6 */
    GetGlobalMouse, /* 7 */
    FSpGetDirectoryID, /* 8 */
    FSpOpenResFileCompat, /* 9 */
    FSpCreateResFileCompat, /* 10 */
    FSpLocationFromPath, /* 11 */
    FSpPathFromLocation, /* 12 */
    TclMacExitHandler, /* 13 */
    TclMacInitExitToShell, /* 14 */
    TclMacInstallExitToShellPatch, /* 15 */
    TclMacOSErrorToPosixError, /* 16 */
    TclMacRemoveTimer, /* 17 */
    TclMacStartTimer, /* 18 */
    TclMacTimerExpired, /* 19 */
    TclMacRegisterResourceFork, /* 20 */
    TclMacUnRegisterResourceFork, /* 21 */
    TclMacCreateEnv, /* 22 */
    TclMacFOpenHack, /* 23 */
    NULL, /* 24 */
    TclMacChmod, /* 25 */
#endif /* MAC_TCL */
};

TclIntPlatStubs *tclIntPlatStubsPtr = &tclIntPlatStubs;

TclPlatStubs tclPlatStubs = {
    TCL_STUB_MAGIC,
    NULL,
#ifdef MAC_TCL
    Tcl_MacSetEventProc, /* 0 */
    Tcl_MacConvertTextResource, /* 1 */
    Tcl_MacEvalResource, /* 2 */
    Tcl_MacFindResource, /* 3 */
    Tcl_GetOSTypeFromObj, /* 4 */
    Tcl_SetOSTypeObj, /* 5 */
    Tcl_NewOSTypeObj, /* 6 */
    strncasecmp, /* 7 */
    strcasecmp, /* 8 */
#endif /* MAC_TCL */
};

TclPlatStubs *tclPlatStubsPtr = &tclPlatStubs;

static TclStubHooks tclStubHooks = {
    &tclPlatStubs,
    &tclIntStubs,
    &tclIntPlatStubs
};


/* !END!: Do not edit above this line. */
