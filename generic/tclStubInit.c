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
 * RCS: @(#) $Id: tclStubInit.c,v 1.2.2.1 1999/03/05 20:18:06 stanton Exp $
 */

#include "tclInt.h"
#include "tclPort.h"
#include "tclCompile.h"

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
    Tcl_AddErrorInfo, /* 0 */
    Tcl_AddObjErrorInfo, /* 1 */
    Tcl_Alloc, /* 2 */
    Tcl_AllowExceptions, /* 3 */
    Tcl_AppendAllObjTypes, /* 4 */
    Tcl_AppendElement, /* 5 */
    Tcl_AppendResult, /* 6 */
    Tcl_AppendResultVA, /* 7 */
    Tcl_AppendToObj, /* 8 */
    Tcl_AppendStringsToObj, /* 9 */
    Tcl_AppendStringsToObjVA, /* 10 */
    Tcl_AsyncCreate, /* 11 */
    Tcl_AsyncDelete, /* 12 */
    Tcl_AsyncInvoke, /* 13 */
    Tcl_AsyncMark, /* 14 */
    Tcl_AsyncReady, /* 15 */
    Tcl_BackgroundError, /* 16 */
    Tcl_Backslash, /* 17 */
    Tcl_BadChannelOption, /* 18 */
    Tcl_CallWhenDeleted, /* 19 */
    Tcl_CancelIdleCall, /* 20 */
    Tcl_Close, /* 21 */
    Tcl_CommandComplete, /* 22 */
    Tcl_Concat, /* 23 */
    Tcl_ConcatObj, /* 24 */
    Tcl_ConvertCountedElement, /* 25 */
    Tcl_ConvertElement, /* 26 */
    Tcl_ConvertToType, /* 27 */
    Tcl_CreateAlias, /* 28 */
    Tcl_CreateAliasObj, /* 29 */
    Tcl_CreateChannel, /* 30 */
    Tcl_CreateChannelHandler, /* 31 */
    Tcl_CreateCloseHandler, /* 32 */
    Tcl_CreateCommand, /* 33 */
    Tcl_CreateEventSource, /* 34 */
    Tcl_CreateExitHandler, /* 35 */
    Tcl_CreateInterp, /* 36 */
    Tcl_CreateMathFunc, /* 37 */
    Tcl_CreateObjCommand, /* 38 */
    Tcl_CreateSlave, /* 39 */
    Tcl_CreateTimerHandler, /* 40 */
    Tcl_CreateTrace, /* 41 */
    Tcl_DbCkalloc, /* 42 */
    Tcl_DbCkfree, /* 43 */
    Tcl_DbCkrealloc, /* 44 */
    Tcl_DbDecrRefCount, /* 45 */
    Tcl_DbIncrRefCount, /* 46 */
    Tcl_DbIsShared, /* 47 */
    Tcl_DbNewBooleanObj, /* 48 */
    Tcl_DbNewByteArrayObj, /* 49 */
    Tcl_DbNewDoubleObj, /* 50 */
    Tcl_DbNewListObj, /* 51 */
    Tcl_DbNewLongObj, /* 52 */
    Tcl_DbNewObj, /* 53 */
    Tcl_DbNewStringObj, /* 54 */
    Tcl_DeleteAssocData, /* 55 */
    Tcl_DeleteCommand, /* 56 */
    Tcl_DeleteCommandFromToken, /* 57 */
    Tcl_DeleteChannelHandler, /* 58 */
    Tcl_DeleteCloseHandler, /* 59 */
    Tcl_DeleteEvents, /* 60 */
    Tcl_DeleteEventSource, /* 61 */
    Tcl_DeleteExitHandler, /* 62 */
    Tcl_DeleteHashEntry, /* 63 */
    Tcl_DeleteHashTable, /* 64 */
    Tcl_DeleteInterp, /* 65 */
    Tcl_DeleteTimerHandler, /* 66 */
    Tcl_DeleteTrace, /* 67 */
    Tcl_DetachPids, /* 68 */
    Tcl_DontCallWhenDeleted, /* 69 */
    Tcl_DoOneEvent, /* 70 */
    Tcl_DoWhenIdle, /* 71 */
    Tcl_DStringAppend, /* 72 */
    Tcl_DStringAppendElement, /* 73 */
    Tcl_DStringEndSublist, /* 74 */
    Tcl_DStringFree, /* 75 */
    Tcl_DStringGetResult, /* 76 */
    Tcl_DStringInit, /* 77 */
    Tcl_DStringResult, /* 78 */
    Tcl_DStringSetLength, /* 79 */
    Tcl_DStringStartSublist, /* 80 */
    Tcl_DumpActiveMemory, /* 81 */
    Tcl_DuplicateObj, /* 82 */
    Tcl_Eof, /* 83 */
    Tcl_ErrnoId, /* 84 */
    Tcl_ErrnoMsg, /* 85 */
    Tcl_Eval, /* 86 */
    Tcl_EvalFile, /* 87 */
    Tcl_EventuallyFree, /* 88 */
    Tcl_EvalObj, /* 89 */
    Tcl_Exit, /* 90 */
    Tcl_ExposeCommand, /* 91 */
    Tcl_ExprBoolean, /* 92 */
    Tcl_ExprBooleanObj, /* 93 */
    Tcl_ExprDouble, /* 94 */
    Tcl_ExprDoubleObj, /* 95 */
    Tcl_ExprLong, /* 96 */
    Tcl_ExprLongObj, /* 97 */
    Tcl_ExprObj, /* 98 */
    Tcl_ExprString, /* 99 */
    Tcl_Finalize, /* 100 */
    Tcl_FindExecutable, /* 101 */
    Tcl_FirstHashEntry, /* 102 */
    Tcl_Flush, /* 103 */
    Tcl_Free, /* 104 */
    TclFreeObj, /* 105 */
    Tcl_FreeResult, /* 106 */
    Tcl_GetAlias, /* 107 */
    Tcl_GetAliasObj, /* 108 */
    Tcl_GetAssocData, /* 109 */
    Tcl_GetBoolean, /* 110 */
    Tcl_GetBooleanFromObj, /* 111 */
    Tcl_GetByteArrayFromObj, /* 112 */
    Tcl_GetChannel, /* 113 */
    Tcl_GetChannelBufferSize, /* 114 */
    Tcl_GetChannelHandle, /* 115 */
    Tcl_GetChannelInstanceData, /* 116 */
    Tcl_GetChannelMode, /* 117 */
    Tcl_GetChannelName, /* 118 */
    Tcl_GetChannelOption, /* 119 */
    Tcl_GetChannelType, /* 120 */
    Tcl_GetCommandInfo, /* 121 */
    Tcl_GetCommandName, /* 122 */
    Tcl_GetDouble, /* 123 */
    Tcl_GetDoubleFromObj, /* 124 */
    Tcl_GetErrno, /* 125 */
    Tcl_GetHostName, /* 126 */
    Tcl_GetIndexFromObj, /* 127 */
    Tcl_GetInt, /* 128 */
    Tcl_GetInterpPath, /* 129 */
    Tcl_GetIntFromObj, /* 130 */
    Tcl_GetLongFromObj, /* 131 */
    Tcl_GetMaster, /* 132 */
    Tcl_GetNameOfExecutable, /* 133 */
    Tcl_GetObjResult, /* 134 */
    Tcl_GetObjType, /* 135 */
    Tcl_GetPathType, /* 136 */
    Tcl_Gets, /* 137 */
    Tcl_GetsObj, /* 138 */
    Tcl_GetServiceMode, /* 139 */
    Tcl_GetSlave, /* 140 */
    Tcl_GetStdChannel, /* 141 */
    Tcl_GetStringFromObj, /* 142 */
    Tcl_GetStringResult, /* 143 */
    Tcl_GetVar, /* 144 */
    Tcl_GetVar2, /* 145 */
    Tcl_GlobalEval, /* 146 */
    Tcl_GlobalEvalObj, /* 147 */
    Tcl_HashStats, /* 148 */
    Tcl_HideCommand, /* 149 */
    Tcl_Init, /* 150 */
    Tcl_InitHashTable, /* 151 */
    Tcl_InitMemory, /* 152 */
    Tcl_InputBlocked, /* 153 */
    Tcl_InputBuffered, /* 154 */
    Tcl_InterpDeleted, /* 155 */
    Tcl_IsSafe, /* 156 */
    Tcl_InvalidateStringRep, /* 157 */
    Tcl_JoinPath, /* 158 */
    Tcl_LinkVar, /* 159 */
    Tcl_ListObjAppendList, /* 160 */
    Tcl_ListObjAppendElement, /* 161 */
    Tcl_ListObjGetElements, /* 162 */
    Tcl_ListObjIndex, /* 163 */
    Tcl_ListObjLength, /* 164 */
    Tcl_ListObjReplace, /* 165 */
    Tcl_MakeFileChannel, /* 166 */
    Tcl_MakeSafe, /* 167 */
    Tcl_MakeTcpClientChannel, /* 168 */
    Tcl_Merge, /* 169 */
    Tcl_NewBooleanObj, /* 170 */
    Tcl_NewByteArrayObj, /* 171 */
    Tcl_NewDoubleObj, /* 172 */
    Tcl_NewIntObj, /* 173 */
    Tcl_NewListObj, /* 174 */
    Tcl_NewLongObj, /* 175 */
    Tcl_NewObj, /* 176 */
    Tcl_NewStringObj, /* 177 */
    Tcl_NextHashEntry, /* 178 */
    Tcl_NotifyChannel, /* 179 */
    Tcl_ObjGetVar2, /* 180 */
    Tcl_ObjSetVar2, /* 181 */
    Tcl_OpenCommandChannel, /* 182 */
    Tcl_OpenFileChannel, /* 183 */
    Tcl_OpenTcpClient, /* 184 */
    Tcl_OpenTcpServer, /* 185 */
    panic, /* 186 */
    panicVA, /* 187 */
    Tcl_ParseVar, /* 188 */
    Tcl_PkgPresent, /* 189 */
    Tcl_PkgPresentEx, /* 190 */
    Tcl_PkgProvide, /* 191 */
    Tcl_PkgProvideEx, /* 192 */
    Tcl_PkgRequire, /* 193 */
    Tcl_PkgRequireEx, /* 194 */
    Tcl_PosixError, /* 195 */
    Tcl_Preserve, /* 196 */
    Tcl_PrintDouble, /* 197 */
    Tcl_PutEnv, /* 198 */
    Tcl_QueueEvent, /* 199 */
    Tcl_Read, /* 200 */
    Tcl_Realloc, /* 201 */
    Tcl_ReapDetachedProcs, /* 202 */
    Tcl_RecordAndEval, /* 203 */
    Tcl_RecordAndEvalObj, /* 204 */
    Tcl_RegExpCompile, /* 205 */
    Tcl_RegExpExec, /* 206 */
    Tcl_RegExpMatch, /* 207 */
    Tcl_RegExpRange, /* 208 */
    Tcl_RegisterChannel, /* 209 */
    Tcl_RegisterObjType, /* 210 */
    Tcl_Release, /* 211 */
    Tcl_ResetResult, /* 212 */
    Tcl_ScanCountedElement, /* 213 */
    Tcl_ScanElement, /* 214 */
    Tcl_Seek, /* 215 */
    Tcl_ServiceAll, /* 216 */
    Tcl_ServiceEvent, /* 217 */
    Tcl_SetAssocData, /* 218 */
    Tcl_SetBooleanObj, /* 219 */
    Tcl_SetByteArrayLength, /* 220 */
    Tcl_SetByteArrayObj, /* 221 */
    Tcl_SetChannelBufferSize, /* 222 */
    Tcl_SetChannelOption, /* 223 */
    Tcl_SetCommandInfo, /* 224 */
    Tcl_SetDoubleObj, /* 225 */
    Tcl_SetErrno, /* 226 */
    Tcl_SetErrorCode, /* 227 */
    Tcl_SetErrorCodeVA, /* 228 */
    Tcl_SetIntObj, /* 229 */
    Tcl_SetListObj, /* 230 */
    Tcl_SetLongObj, /* 231 */
    Tcl_SetMaxBlockTime, /* 232 */
    Tcl_SetObjErrorCode, /* 233 */
    Tcl_SetObjLength, /* 234 */
    Tcl_SetObjResult, /* 235 */
    Tcl_SetPanicProc, /* 236 */
    Tcl_SetRecursionLimit, /* 237 */
    Tcl_SetResult, /* 238 */
    Tcl_SetServiceMode, /* 239 */
    Tcl_SetStdChannel, /* 240 */
    Tcl_SetStringObj, /* 241 */
    Tcl_SetTimer, /* 242 */
    Tcl_SetVar, /* 243 */
    Tcl_SetVar2, /* 244 */
    Tcl_SignalId, /* 245 */
    Tcl_SignalMsg, /* 246 */
    Tcl_Sleep, /* 247 */
    Tcl_SourceRCFile, /* 248 */
    Tcl_SplitList, /* 249 */
    Tcl_SplitPath, /* 250 */
    Tcl_StaticPackage, /* 251 */
    Tcl_StringMatch, /* 252 */
    Tcl_Tell, /* 253 */
    Tcl_TraceVar, /* 254 */
    Tcl_TraceVar2, /* 255 */
    Tcl_TranslateFileName, /* 256 */
    Tcl_Ungets, /* 257 */
    Tcl_UnlinkVar, /* 258 */
    Tcl_UnregisterChannel, /* 259 */
    Tcl_UnsetVar, /* 260 */
    Tcl_UnsetVar2, /* 261 */
    Tcl_UntraceVar, /* 262 */
    Tcl_UntraceVar2, /* 263 */
    Tcl_UpdateLinkedVar, /* 264 */
    Tcl_UpVar, /* 265 */
    Tcl_UpVar2, /* 266 */
    Tcl_ValidateAllMemory, /* 267 */
    Tcl_VarEval, /* 268 */
    Tcl_VarEvalVA, /* 269 */
    Tcl_VarTraceInfo, /* 270 */
    Tcl_VarTraceInfo2, /* 271 */
    Tcl_WaitForEvent, /* 272 */
    Tcl_WaitPid, /* 273 */
    Tcl_Write, /* 274 */
    Tcl_WrongNumArgs, /* 275 */
};

TclStubs *tclStubsPtr = &tclStubs;

TclCompileStubs tclCompileStubs = {
    TCL_STUB_MAGIC,
    NULL,
    TclCleanupByteCode, /* 0 */
    TclCompileExpr, /* 1 */
    TclCompileQuotes, /* 2 */
    TclCompileString, /* 3 */
    TclCompileDollarVar, /* 4 */
    TclCreateAuxData, /* 5 */
    TclCreateExecEnv, /* 6 */
    TclDeleteExecEnv, /* 7 */
    TclEmitForwardJump, /* 8 */
    TclGetAuxDataType, /* 9 */
    TclGetExceptionRangeForPc, /* 10 */
    TclGetInstructionTable, /* 11 */
    TclExecuteByteCode, /* 12 */
    TclExpandCodeArray, /* 13 */
    TclExpandJumpFixupArray, /* 14 */
    TclFinalizeAuxDataTypeTable, /* 15 */
    TclFixupForwardJump, /* 16 */
    TclFreeCompileEnv, /* 17 */
    TclFreeJumpFixupArray, /* 18 */
    TclInitAuxDataTypeTable, /* 19 */
    TclInitByteCodeObj, /* 20 */
    TclInitCompileEnv, /* 21 */
    TclInitJumpFixupArray, /* 22 */
    TclObjIndexForString, /* 23 */
    TclPrintInstruction, /* 24 */
    TclPrintSource, /* 25 */
    TclRegisterAuxDataType, /* 26 */
};

TclCompileStubs *tclCompileStubsPtr = &tclCompileStubs;

TclIntStubs tclIntStubs = {
    TCL_STUB_MAGIC,
    NULL,
    TclAccess, /* 0 */
    TclAccessDeleteProc, /* 1 */
    TclAccessInsertProc, /* 2 */
    TclAllocateFreeObjects, /* 3 */
    TclChdir, /* 4 */
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
    TclExpandParseValue, /* 15 */
    TclExprFloatError, /* 16 */
    TclFileAttrsCmd, /* 17 */
    TclFileCopyCmd, /* 18 */
    TclFileDeleteCmd, /* 19 */
    TclFileMakeDirsCmd, /* 20 */
    TclFileRenameCmd, /* 21 */
    TclFinalizeCompExecEnv, /* 22 */
    TclFinalizeEnvironment, /* 23 */
    TclFinalizeExecEnv, /* 24 */
    TclFindElement, /* 25 */
    TclFindProc, /* 26 */
    TclFormatInt, /* 27 */
    TclFreePackageInfo, /* 28 */
    TclGetCwd, /* 29 */
    TclGetDate, /* 30 */
    TclGetDefaultStdChannel, /* 31 */
    TclGetElementOfIndexedArray, /* 32 */
    TclGetEnv, /* 33 */
    TclGetExtension, /* 34 */
    TclGetFrame, /* 35 */
    TclGetInterpProc, /* 36 */
    TclGetIntForIndex, /* 37 */
    TclGetIndexedScalar, /* 38 */
    TclGetLong, /* 39 */
    TclGetLoadedPackages, /* 40 */
    TclGetNamespaceForQualName, /* 41 */
    TclGetObjInterpProc, /* 42 */
    TclGetOpenMode, /* 43 */
    TclGetOriginalCommand, /* 44 */
    TclGetUserHome, /* 45 */
    TclGlobalInvoke, /* 46 */
    TclGuessPackageName, /* 47 */
    TclHideUnsafeCommands, /* 48 */
    TclInExit, /* 49 */
    TclIncrElementOfIndexedArray, /* 50 */
    TclIncrIndexedScalar, /* 51 */
    TclIncrVar2, /* 52 */
    TclInitCompiledLocals, /* 53 */
    TclInitNamespaces, /* 54 */
    TclInterpInit, /* 55 */
    TclInvoke, /* 56 */
    TclInvokeObjectCommand, /* 57 */
    TclInvokeStringCommand, /* 58 */
    TclIsProc, /* 59 */
    TclLoadFile, /* 60 */
    TclLooksLikeInt, /* 61 */
    TclLookupVar, /* 62 */
    TclMatchFiles, /* 63 */
    TclNeedSpace, /* 64 */
    TclNewProcBodyObj, /* 65 */
    TclObjCommandComplete, /* 66 */
    TclObjInterpProc, /* 67 */
    TclObjInvoke, /* 68 */
    TclObjInvokeGlobal, /* 69 */
    TclOpenFileChannelDeleteProc, /* 70 */
    TclOpenFileChannelInsertProc, /* 71 */
    TclpAccess, /* 72 */
    TclpAlloc, /* 73 */
    TclpCopyFile, /* 74 */
    TclpCopyDirectory, /* 75 */
    TclpCreateDirectory, /* 76 */
    TclpDeleteFile, /* 77 */
    TclpFree, /* 78 */
    TclpGetClicks, /* 79 */
    TclpGetSeconds, /* 80 */
    TclpGetTime, /* 81 */
    TclpGetTimeZone, /* 82 */
    TclpListVolumes, /* 83 */
    TclpOpenFileChannel, /* 84 */
    TclpRealloc, /* 85 */
    TclpRemoveDirectory, /* 86 */
    TclpRenameFile, /* 87 */
    TclParseBraces, /* 88 */
    TclParseNestedCmd, /* 89 */
    TclParseQuotes, /* 90 */
    TclPlatformInit, /* 91 */
    TclPrecTraceProc, /* 92 */
    TclPreventAliasLoop, /* 93 */
    TclPrintByteCodeObj, /* 94 */
    TclProcCleanupProc, /* 95 */
    TclProcCompileProc, /* 96 */
    TclProcDeleteProc, /* 97 */
    TclProcInterpProc, /* 98 */
    TclpStat, /* 99 */
    TclRenameCommand, /* 100 */
    TclResetShadowedCmdRefs, /* 101 */
    TclServiceIdle, /* 102 */
    TclSetElementOfIndexedArray, /* 103 */
    TclSetIndexedScalar, /* 104 */
    TclSetPreInitScript, /* 105 */
    TclSetupEnv, /* 106 */
    TclSockGetPort, /* 107 */
    TclSockMinimumBuffers, /* 108 */
    TclStat, /* 109 */
    TclStatDeleteProc, /* 110 */
    TclStatInsertProc, /* 111 */
    TclTeardownNamespace, /* 112 */
    TclUpdateReturnInfo, /* 113 */
    TclWordEnd, /* 114 */
    Tcl_AddInterpResolvers, /* 115 */
    Tcl_AppendExportList, /* 116 */
    Tcl_CreateNamespace, /* 117 */
    Tcl_DeleteNamespace, /* 118 */
    Tcl_Export, /* 119 */
    Tcl_FindCommand, /* 120 */
    Tcl_FindNamespace, /* 121 */
    Tcl_GetInterpResolvers, /* 122 */
    Tcl_GetNamespaceResolvers, /* 123 */
    Tcl_FindNamespaceVar, /* 124 */
    Tcl_ForgetImport, /* 125 */
    Tcl_GetCommandFromObj, /* 126 */
    Tcl_GetCommandFullName, /* 127 */
    Tcl_GetCurrentNamespace, /* 128 */
    Tcl_GetGlobalNamespace, /* 129 */
    Tcl_GetVariableFullName, /* 130 */
    Tcl_Import, /* 131 */
    Tcl_PopCallFrame, /* 132 */
    Tcl_PushCallFrame, /* 133 */
    Tcl_RemoveInterpResolvers, /* 134 */
    Tcl_SetNamespaceResolvers, /* 135 */
    TclCompileBreakCmd, /* 136 */
    TclCompileCatchCmd, /* 137 */
    TclCompileContinueCmd, /* 138 */
    TclCompileExprCmd, /* 139 */
    TclCompileForCmd, /* 140 */
    TclCompileForeachCmd, /* 141 */
    TclCompileIfCmd, /* 142 */
    TclCompileIncrCmd, /* 143 */
    TclCompileSetCmd, /* 144 */
    TclCompileWhileCmd, /* 145 */
    TclHasSockets, /* 146 */
    TclpGetDate, /* 147 */
    TclStrftime, /* 148 */
    TclpCheckStackSpace, /* 149 */
};

TclIntStubs *tclIntStubsPtr = &tclIntStubs;

TclIntPlatStubs tclIntPlatStubs = {
    TCL_STUB_MAGIC,
    NULL,
#ifdef __WIN32__
    TclWinConvertError, /* 0 */
    TclWinConvertWSAError, /* 1 */
    TclWinGetServByName, /* 2 */
    TclWinGetSockOpt, /* 3 */
    TclWinGetTclInstance, /* 4 */
    TclWinLoadLibrary, /* 5 */
    TclWinNToHS, /* 6 */
    TclWinSetSockOpt, /* 7 */
    TclpGetPid, /* 8 */
    TclpFinalize, /* 9 */
    TclWinGetPlatformId, /* 10 */
    TclWinInit, /* 11 */
    TclWinSynchSpawn, /* 12 */
    TclGetAndDetachPids, /* 13 */
    TclpCloseFile, /* 14 */
    TclpCreateCommandChannel, /* 15 */
    TclpCreatePipe, /* 16 */
    TclpCreateProcess, /* 17 */
    TclpCreateTempFile, /* 18 */
    TclpGetTZName, /* 19 */
    TclpMakeFile, /* 20 */
    TclpOpenFile, /* 21 */
#endif /* __WIN32__ */
#if !defined(__WIN32__) && !defined(MAC_TCL)
    TclGetAndDetachPids, /* 0 */
    TclpCloseFile, /* 1 */
    TclpCreateCommandChannel, /* 2 */
    TclpCreatePipe, /* 3 */
    TclpCreateProcess, /* 4 */
    TclpCreateTempFile, /* 5 */
    TclpMakeFile, /* 6 */
    TclpOpenFile, /* 7 */
    TclUnixWaitForFile, /* 8 */
#endif /* UNIX */
#ifdef MAC_TCL
    TclpSysAlloc, /* 0 */
    TclpSysFree, /* 1 */
    TclpSysRealloc, /* 2 */
    TclPlatformExit, /* 3 */
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
    TclMacReadlink, /* 24 */
    TclMacChmod, /* 25 */
#endif /* MAC_TCL */
};

TclIntPlatStubs *tclIntPlatStubsPtr = &tclIntPlatStubs;

TclPlatStubs tclPlatStubs = {
    TCL_STUB_MAGIC,
    NULL,
#if !defined(__WIN32__) && !defined(MAC_TCL)
    Tcl_CreateFileHandler, /* 0 */
    Tcl_DeleteFileHandler, /* 1 */
    Tcl_GetOpenFile, /* 2 */
#endif /* UNIX */
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
    &tclIntPlatStubs,
    &tclCompileStubs
};


/* !END!: Do not edit above this line. */
