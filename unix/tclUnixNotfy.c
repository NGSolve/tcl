/*
 * tclUnixNotfy.c --
 *
 *	This file contains the implementation of the Unix-specific notifier,
 *	based on select()/epoll()/kqueue() depending on platform, which is the
 *	lowest-level part of the Tcl event loop. This file works together with
 *	generic/tclNotify.c.
 *
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tclInt.h"
#ifndef HAVE_COREFOUNDATION	/* Darwin/Mac OS X CoreFoundation notifier is
				 * in tclMacOSXNotify.c */
#include <signal.h>
#if defined(HAVE_EPOLL)
#   include <sys/epoll.h>
#elif defined(HAVE_KQUEUE)
#   include <sys/types.h>
#   include <sys/event.h>
#   include <sys/time.h>
#endif

/*
 * This structure is used to keep track of the notifier info for a registered
 * file.
 */

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
struct PlatformEventData;
#undef USE_SELECT_FOR_NOTIFIER
#else
#define USE_SELECT_FOR_NOTIFIER	1
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
typedef struct FileHandler {
    int fd;
    int mask;			/* Mask of desired events: TCL_READABLE,
				 * etc. */
    int readyMask;		/* Mask of events that have been seen since
				 * the last time file handlers were invoked
				 * for this file. */
    Tcl_FileProc *proc;		/* Function to call, in the style of
				 * Tcl_CreateFileHandler. */
    ClientData clientData;	/* Argument to pass to proc. */
    struct FileHandler *nextPtr;/* Next in list of all files we care about. */
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
    struct PlatformEventData *platformEventData;
				/* Pointer associating the platform-specific
				 * event structures with {file,tsd}Ptrs. */
    int platformReadyMask;	/* Platform-specific mask of ready events. */
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
} FileHandler;

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
/*
 * The following structure is associated with a FileHandler through platform-
 * specific event structures (currently either struct epoll_event or kevent)
 * whenever Tcl_CreateFileHandler or Tcl_DeleteFileHandler are called.
 * It contains a FileHandler and a ThreadSpecificData pointer in order to
 * update readyMask and to alert waiting threads.
 */

struct ThreadSpecificData;
struct PlatformEventData {
    FileHandler *filePtr;
    struct ThreadSpecificData *tsdPtr;
};
#else /* !(HAVE_EPOLL || HAVE_KQUEUE) */
/*
 * The following structure contains a set of select() masks to track readable,
 * writable, and exception conditions.
 */

typedef struct {
    fd_set readable;
    fd_set writable;
    fd_set exception;
} SelectMasks;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

/*
 * The following structure is what is added to the Tcl event queue when file
 * handlers are ready to fire.
 */

typedef struct {
    Tcl_Event header;		/* Information that is standard for all
				 * events. */
    int fd;			/* File descriptor that is ready. Used to find
				 * the FileHandler structure for the file
				 * (can't point directly to the FileHandler
				 * structure because it could go away while
				 * the event is queued). */
} FileHandlerEvent;

/*
 * The following static structure contains the state information for the
 * select based implementation of the Tcl notifier. One of these structures is
 * created for each thread that is using the notifier.
 */

typedef struct ThreadSpecificData {
    FileHandler *firstFileHandlerPtr;
				/* Pointer to head of file handler list. */
#ifdef USE_SELECT_FOR_NOTIFIER
    SelectMasks checkMasks;	/* This structure is used to build up the
				 * masks to be used in the next call to
				 * select. Bits are set in response to calls
				 * to Tcl_CreateFileHandler. */
    SelectMasks readyMasks;	/* This array reflects the readable/writable
				 * conditions that were found to exist by the
				 * last call to select. */
    int numFdBits;		/* Number of valid bits in checkMasks (one
				 * more than highest fd for which
				 * Tcl_WatchFile has been called). */
#endif /* USE_SELECT_FOR_NOTIFIER */
#ifdef TCL_THREADS
    int onList;			/* True if it is in this list */
    unsigned int pollState;	/* pollState is used to implement a polling
				 * handshake between each thread and the
				 * notifier thread. Bits defined below. */
    struct ThreadSpecificData *nextPtr, *prevPtr;
				/* All threads that are currently waiting on
				 * an event have their ThreadSpecificData
				 * structure on a doubly-linked listed formed
				 * from these pointers. You must hold the
				 * notifierMutex lock before accessing these
				 * fields. */
#ifdef __CYGWIN__
    void *event;     /* Any other thread alerts a notifier
	 * that an event is ready to be processed
	 * by sending this event. */
    void *hwnd;			/* Messaging window. */
#else /* !__CYGWIN__ */
    pthread_cond_t waitCV;	/* Any other thread alerts a notifier that an
				 * event is ready to be processed by signaling
				 * this condition variable. */
#endif /* __CYGWIN__ */
    int waitCVinitialized;	/* Variable to flag initialization of the structure */
    int eventReady;		/* True if an event is ready to be processed.
				 * Used as condition flag together with waitCV
				 * above. */
#endif /* TCL_THREADS */
} ThreadSpecificData;

static Tcl_ThreadDataKey dataKey;

#ifdef TCL_THREADS
/*
 * The following static indicates the number of threads that have initialized
 * notifiers.
 *
 * You must hold the notifierMutex lock before accessing this variable.
 */

static int notifierCount = 0;

/*
 * The following variable points to the head of a doubly-linked list of
 * ThreadSpecificData structures for all threads that are currently waiting on
 * an event.
 *
 * You must hold the notifierMutex lock before accessing this list.
 */

static ThreadSpecificData *waitingListPtr = NULL;

/*
 * The notifier thread spends all its time in select() waiting for a file
 * descriptor associated with one of the threads on the waitingListPtr list to
 * do something interesting. But if the contents of the waitingListPtr list
 * ever changes, we need to wake up and restart the select() system call. You
 * can wake up the notifier thread by writing a single byte to the file
 * descriptor defined below. This file descriptor is the input-end of a pipe
 * and the notifier thread is listening for data on the output-end of the same
 * pipe. Hence writing to this file descriptor will cause the select() system
 * call to return and wake up the notifier thread.
 *
 * You must hold the notifierMutex lock before writing to the pipe.
 */

static int triggerPipe = -1;

/*
 * The notifierMutex locks access to all of the global notifier state.
 */

static pthread_mutex_t notifierInitMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t notifierMutex     = PTHREAD_MUTEX_INITIALIZER;
/*
 * The following static indicates if the notifier thread is running.
 *
 * You must hold the notifierInitMutex before accessing this variable.
 */

static int notifierThreadRunning = 0;

/*
 * The notifier thread signals the notifierCV when it has finished
 * initializing the triggerPipe and right before the notifier thread
 * terminates.
 */

static pthread_cond_t notifierCV = PTHREAD_COND_INITIALIZER;

/*
 * The pollState bits.
 *
 * POLL_WANT:	Set by each thread before it waits on its condition variable.
 *		It is checked by the notifier before it does select.
 * POLL_DONE:	Set by the notifier if it goes into select after seeing
 *		POLL_WANT. The idea is to ensure it tries a select with the
 *		same bits the initial thread had set.
 */

#define POLL_WANT	0x1
#define POLL_DONE	0x2

/*
 * This is the thread ID of the notifier thread that does select.
 */

static Tcl_ThreadId notifierThread;
#endif /* TCL_THREADS */
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
/*
 * This is the file descriptor for the platform-specific events mechanism
 * (currently either epoll_{ctl,wait} or kevent).
 */

static int eventsFd;

/*
 * This array reflects the readable/writable/error event conditions that
 * were found to exist by the last call to the platform-specific events
 * mechanism (currently either epoll_wait or kevent) in NotifierThreadProc.
 * maxReadyEvents specifies the maximum number of epoll_events in readyEvents.
 */

#if defined(HAVE_EPOLL)
static struct epoll_event *readyEvents;
#elif defined(HAVE_KQUEUE)
static struct kevent *readyEvents;
#endif /* HAVE_EPOLL */
static int maxReadyEvents;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

/*
 * Static routines defined in this file.
 */

#ifdef TCL_THREADS
static void	NotifierThreadProc(ClientData clientData);
#if defined(HAVE_PTHREAD_ATFORK)
static int	atForkInit = 0;
static void	AtForkChild(void);
#endif /* HAVE_PTHREAD_ATFORK */
#endif /* TCL_THREADS */
static int	FileHandlerEventProc(Tcl_Event *evPtr, int flags);

/*
 * Import of Windows API when building threaded with Cygwin.
 */

#if defined(TCL_THREADS) && defined(__CYGWIN__)
typedef struct {
    void *hwnd;
    unsigned int *message;
    int wParam;
    int lParam;
    int time;
    int x;
    int y;
} MSG;

typedef struct {
    unsigned int style;
    void *lpfnWndProc;
    int cbClsExtra;
    int cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    void *lpszMenuName;
    const void *lpszClassName;
} WNDCLASS;

extern void __stdcall	CloseHandle(void *);
extern void *__stdcall	CreateEventW(void *, unsigned char, unsigned char,
			    void *);
extern void * __stdcall	CreateWindowExW(void *, const void *, const void *,
			    DWORD, int, int, int, int, void *, void *, void *, void *);
extern DWORD __stdcall	DefWindowProcW(void *, int, void *, void *);
extern unsigned char __stdcall	DestroyWindow(void *);
extern int __stdcall	DispatchMessageW(const MSG *);
extern unsigned char __stdcall	GetMessageW(MSG *, void *, int, int);
extern void __stdcall	MsgWaitForMultipleObjects(DWORD, void *,
			    unsigned char, DWORD, DWORD);
extern unsigned char __stdcall	PeekMessageW(MSG *, void *, int, int, int);
extern unsigned char __stdcall	PostMessageW(void *, unsigned int, void *,
				    void *);
extern void __stdcall	PostQuitMessage(int);
extern void *__stdcall	RegisterClassW(const WNDCLASS *);
extern unsigned char __stdcall	ResetEvent(void *);
extern unsigned char __stdcall	TranslateMessage(const MSG *);

/*
 * Threaded-cygwin specific constants and functions in this file:
 */

static const WCHAR className[] = L"TclNotifier";
static DWORD __stdcall	NotifierProc(void *hwnd, unsigned int message,
			    void *wParam, void *lParam);
#endif /* TCL_THREADS && __CYGWIN__ */

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
#define PLATFORMEVENTSCONTROL_ADD	0x01
#define PLATFORMEVENTSCONTROL_DEL	0x02
#define PLATFORMEVENTSCONTROL_MOD	0x04
#define PLATFORMEVENTSCONTROL_AUTO_MASK	0x10
static void		PlatformEventsControl(FileHandler *filePtr,
			    ThreadSpecificData *tsdPtr, int op, int mask);
static void		PlatformEventsFinalize(void);
static int		PlatformEventsGet(int numEvent_last,
			    int numReadyEvents, int skipFd, int wantFd,
			    int onList);
static void		PlatformEventsInit(void);
static int		PlatformEventsTranslate(FileHandler *filePtr);
#if defined(HAVE_EPOLL)
static int		PlatformEventsWait(struct epoll_event *events,
			    int numEvents, struct timeval *timePtr);
#elif defined(HAVE_KQUEUE)
static int		PlatformEventsWait(struct kevent *events,
			    int numEvents, struct timeval *timePtr);
#endif /* HAVE_EPOLL */
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

#if TCL_THREADS
/*
 *----------------------------------------------------------------------
 *
 * StartNotifierThread --
 *
 *	Start a notfier thread and wait for the notifier pipe to be created.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Running Thread.
 *
 *----------------------------------------------------------------------
 */

static void
StartNotifierThread(
    const char *proc)
{
    if (!notifierThreadRunning) {
	pthread_mutex_lock(&notifierInitMutex);
	if (!notifierThreadRunning) {
	    if (TclpThreadCreate(&notifierThread, NotifierThreadProc, NULL,
		    TCL_THREAD_STACK_DEFAULT, TCL_THREAD_JOINABLE) != TCL_OK) {
		Tcl_Panic("%s: unable to start notifier thread", proc);
	    }

	    pthread_mutex_lock(&notifierMutex);

	    /*
	     * Wait for the notifier pipe to be created.
	     */

	    while (triggerPipe < 0) {
		pthread_cond_wait(&notifierCV, &notifierMutex);
	    }
	    pthread_mutex_unlock(&notifierMutex);

	    notifierThreadRunning = 1;
	}
	pthread_mutex_unlock(&notifierInitMutex);
    }
}
#endif /* TCL_THREADS */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_InitNotifier --
 *
 *	Initializes the platform specific notifier state.
 *
 * Results:
 *	Returns a handle to the notifier state for this thread.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

ClientData
Tcl_InitNotifier(void)
{
    if (tclNotifierHooks.initNotifierProc) {
	return tclNotifierHooks.initNotifierProc();
    } else {
	ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	PlatformEventsInit();
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
#ifdef TCL_THREADS
	tsdPtr->eventReady = 0;

	/*
	 * Initialize thread specific condition variable for this thread.
	 */
	if (tsdPtr->waitCVinitialized == 0) {
#ifdef __CYGWIN__
	    WNDCLASS class;

	    class.style = 0;
	    class.cbClsExtra = 0;
	    class.cbWndExtra = 0;
	    class.hInstance = TclWinGetTclInstance();
	    class.hbrBackground = NULL;
	    class.lpszMenuName = NULL;
	    class.lpszClassName = className;
	    class.lpfnWndProc = NotifierProc;
	    class.hIcon = NULL;
	    class.hCursor = NULL;

	    RegisterClassW(&class);
	    tsdPtr->hwnd = CreateWindowExW(NULL, class.lpszClassName,
		    class.lpszClassName, 0, 0, 0, 0, 0, NULL, NULL,
		    TclWinGetTclInstance(), NULL);
	    tsdPtr->event = CreateEventW(NULL, 1 /* manual */,
		    0 /* !signaled */, NULL);
#else
	    pthread_cond_init(&tsdPtr->waitCV, NULL);
#endif /* __CYGWIN__ */
	    tsdPtr->waitCVinitialized = 1;
	}

	pthread_mutex_lock(&notifierInitMutex);
#if defined(HAVE_PTHREAD_ATFORK)
	/*
	 * Install pthread_atfork handlers to clean up the notifier in the
	 * child of a fork.
	 */

	if (!atForkInit) {
	    int result = pthread_atfork(NULL, NULL, AtForkChild);

	    if (result) {
		Tcl_Panic("Tcl_InitNotifier: pthread_atfork failed");
	    }
	    atForkInit = 1;
	}
#endif /* HAVE_PTHREAD_ATFORK */

	notifierCount++;

	pthread_mutex_unlock(&notifierInitMutex);

#endif /* TCL_THREADS */
	return tsdPtr;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_FinalizeNotifier --
 *
 *	This function is called to cleanup the notifier state before a thread
 *	is terminated.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May terminate the background notifier thread if this is the last
 *	notifier instance.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_FinalizeNotifier(
    ClientData clientData)		/* Not used. */
{
    if (tclNotifierHooks.finalizeNotifierProc) {
	tclNotifierHooks.finalizeNotifierProc(clientData);
	return;
    } else {
#ifdef TCL_THREADS
	ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

	pthread_mutex_lock(&notifierInitMutex);
	notifierCount--;

	/*
	 * If this is the last thread to use the notifier, close the notifier
	 * pipe and wait for the background thread to terminate.
	 */

	if (notifierCount == 0) {

	    if (triggerPipe != -1) {
		if (write(triggerPipe, "q", 1) != 1) {
		    Tcl_Panic("Tcl_FinalizeNotifier: %s",
			    "unable to write q to triggerPipe");
		}
		close(triggerPipe);
		pthread_mutex_lock(&notifierMutex);
		while(triggerPipe != -1) {
		    pthread_cond_wait(&notifierCV, &notifierMutex);
		}
		pthread_mutex_unlock(&notifierMutex);
		if (notifierThreadRunning) {
		    int result = pthread_join((pthread_t) notifierThread, NULL);

		    if (result) {
			Tcl_Panic("Tcl_FinalizeNotifier: unable to join notifier "
				"thread");
		    }
		    notifierThreadRunning = 0;
		}
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
		PlatformEventsFinalize();
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
	    }
	}

	/*
	 * Clean up any synchronization objects in the thread local storage.
	 */

#ifdef __CYGWIN__
	DestroyWindow(tsdPtr->hwnd);
	CloseHandle(tsdPtr->event);
#else /* __CYGWIN__ */
	pthread_cond_destroy(&tsdPtr->waitCV);
#endif /* __CYGWIN__ */
	tsdPtr->waitCVinitialized = 0;

	pthread_mutex_unlock(&notifierInitMutex);
#endif /* TCL_THREADS */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AlertNotifier --
 *
 *	Wake up the specified notifier from any thread. This routine is called
 *	by the platform independent notifier code whenever the Tcl_ThreadAlert
 *	routine is called. This routine is guaranteed not to be called on a
 *	given notifier after Tcl_FinalizeNotifier is called for that notifier.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Signals the notifier condition variable for the specified notifier.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_AlertNotifier(
    ClientData clientData)
{
    if (tclNotifierHooks.alertNotifierProc) {
	tclNotifierHooks.alertNotifierProc(clientData);
	return;
    } else {
#ifdef TCL_THREADS
	ThreadSpecificData *tsdPtr = clientData;

	pthread_mutex_lock(&notifierMutex);
	tsdPtr->eventReady = 1;

#   ifdef __CYGWIN__
	PostMessageW(tsdPtr->hwnd, 1024, 0, 0);
#   else
	pthread_cond_broadcast(&tsdPtr->waitCV);
#   endif /* __CYGWIN__ */
	pthread_mutex_unlock(&notifierMutex);
#endif /* TCL_THREADS */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_SetTimer --
 *
 *	This function sets the current notifier timer value. This interface is
 *	not implemented in this notifier because we are always running inside
 *	of Tcl_DoOneEvent.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_SetTimer(
    const Tcl_Time *timePtr)		/* Timeout value, may be NULL. */
{
    if (tclNotifierHooks.setTimerProc) {
	tclNotifierHooks.setTimerProc(timePtr);
	return;
    } else {
	/*
	 * The interval timer doesn't do anything in this implementation,
	 * because the only event loop is via Tcl_DoOneEvent, which passes
	 * timeout values to Tcl_WaitForEvent.
	 */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_ServiceModeHook --
 *
 *	This function is invoked whenever the service mode changes.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_ServiceModeHook(
    int mode)			/* Either TCL_SERVICE_ALL, or
				 * TCL_SERVICE_NONE. */
{
    if (tclNotifierHooks.serviceModeHookProc) {
	tclNotifierHooks.serviceModeHookProc(mode);
	return;
    } else if (mode == TCL_SERVICE_ALL) {
#if TCL_THREADS
	StartNotifierThread("Tcl_ServiceModeHook");
#endif
    }
}

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsControl --
 *
 *	This function abstracts adding, modifying, or deleting a file
 *	descriptor and its associated FileHandler and ThreadSpecificData
 *	pointers from an epoll or kqueue fd via either epoll_ctl or kevent.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If successful, a file descriptor is added, deleted, or modified from
 *	the epoll on Linux and kqueue on FreeBSD fd.
 *
 *----------------------------------------------------------------------
 */

void
PlatformEventsControl(
    FileHandler *filePtr,
    ThreadSpecificData *tsdPtr,
    int op,
    int mask)
{
#if defined(HAVE_EPOLL)
    struct epoll_event event;
    int epollOp;
#elif defined(HAVE_KQUEUE)
    int numEvents;
    struct kevent event[2];
    int keventOp;
#endif /* HAVE_EPOLL */
    struct PlatformEventData *platformEventData;

    if (op & PLATFORMEVENTSCONTROL_AUTO_MASK) {
	mask = filePtr->mask;
	op &= ~PLATFORMEVENTSCONTROL_AUTO_MASK;
    }
    if (op == PLATFORMEVENTSCONTROL_ADD) {
	platformEventData = ckalloc(sizeof(*platformEventData));
	platformEventData->filePtr = filePtr;
	platformEventData->tsdPtr = tsdPtr;
	filePtr->platformEventData = platformEventData;
    } else {
	platformEventData = filePtr->platformEventData;
    }
    filePtr->platformReadyMask = 0;

#if defined(HAVE_EPOLL)
    event.events = EPOLLET;
    if (mask & TCL_READABLE) {
	event.events |= EPOLLIN;
    }
    if (mask & TCL_WRITABLE) {
	event.events |= EPOLLOUT;
    }
    if (mask & TCL_EXCEPTION) {
	event.events |= EPOLLERR;
    }
    event.data.ptr = platformEventData;
    if (op == PLATFORMEVENTSCONTROL_ADD) {
	epollOp = EPOLL_CTL_ADD;
    } else if (op == PLATFORMEVENTSCONTROL_DEL) {
	epollOp = EPOLL_CTL_DEL;
    } else if (op == PLATFORMEVENTSCONTROL_MOD) {
	epollOp = EPOLL_CTL_MOD;
    }
    if (epoll_ctl(eventsFd, epollOp, filePtr->fd, &event) == -1) {
	Tcl_Panic("epoll_ctl: %s", strerror(errno));
    }
#elif defined(HAVE_KQUEUE)

    numEvents = 0;
    keventOp = 0;
    if (op == PLATFORMEVENTSCONTROL_ADD) {
	keventOp = EV_ADD;
    } else if (op == PLATFORMEVENTSCONTROL_DEL) {
	keventOp = EV_DELETE;
    } else if (op == PLATFORMEVENTSCONTROL_MOD) {
	keventOp = EV_ADD;
    }
    keventOp |= EV_CLEAR;
    if ((mask & TCL_READABLE) || (mask & TCL_EXCEPTION)) {
	EV_SET(&event[numEvents], filePtr->fd, EVFILT_READ, keventOp, 0, 0,
		platformEventData);
	numEvents++;
    }
    if (mask & TCL_WRITABLE) {
	EV_SET(&event[numEvents], filePtr->fd, EVFILT_WRITE, keventOp, 0, 0,
		platformEventData);
	numEvents++;
    }
    if (kevent(eventsFd, event, numEvents, NULL, 0, NULL) == -1) {
	Tcl_Panic("kevent: %s", strerror(errno));
    }
#endif /* HAVE_EPOLL */
}

/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsFinalize --
 *
 *	This function abstracts closing the epoll on Linux and kqueue on
 *	FreeBSD file descriptor and freeing its associated array of returned
 *	events.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The epoll or kqueue fd is closed if non-zero. The array of returned
 *	events is freed if non_NULL.
 *
 *----------------------------------------------------------------------
 */

void
PlatformEventsFinalize(void)
{
    if (eventsFd > 0) {
	close(eventsFd);
	eventsFd = 0;
    }
    if (readyEvents) {
	ckfree(readyEvents);
	maxReadyEvents = 0;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsGet --
 *
 *	This function abstracts iterating over the array of returned events
 *	since the last call to PlatformEventsWait(). If skipFd and/or wantFd
 *	are non-zero, the specified file descriptor will be skipped or hunted
 *	for respectively. If onList is non-zero, the ThreadSpecificData asso-
 *	ciated with the current event must specify a non-zero onList flag.
 *
 * Results:
 *	Returns -1 if there were no or no eligible events. Returns the index
 *	of the event in the array of returned events in all other cases.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
PlatformEventsGet(
    int numEvent_last,
    int numReadyEvents,
    int skipFd,
    int wantFd,
    int onList)
{
    int numEvent;
    struct PlatformEventData *platformEventData;

    for (numEvent = numEvent_last; numEvent < numReadyEvents; numEvent++) {
#if defined(HAVE_EPOLL)
	platformEventData = readyEvents[numEvent].data.ptr;
#elif defined(HAVE_KQUEUE)
	platformEventData = readyEvents[numEvent].udata;
#endif /* HAVE_EPOLL */
	if (!platformEventData) {
	    continue;
	} else if (!platformEventData->filePtr) {
	    continue;
	} else if (!platformEventData->tsdPtr) {
	    continue;
	} else if (skipFd && (platformEventData->filePtr->fd == skipFd)) {
	    continue;
	} else if (wantFd && (platformEventData->filePtr->fd != wantFd)) {
	    continue;
	} else if (onList && !platformEventData->tsdPtr->onList) {
	    continue;
	} else {
	    return numEvent;
	}
    }
    return -1;
}

/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsInit --
 *	This function abstracts creating an epoll fd on Linux and a kqueue
 *	fd on FreeBSD via epoll_create and kqueue system calls respectively.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The epoll or kqueue fd is created if zero. The array of returned
 *	events is allocated and initialised with space for 128 events if zero.
 *
 *----------------------------------------------------------------------
 */

void
PlatformEventsInit(void)
{
#if defined(HAVE_EPOLL)
    if (eventsFd <= 0) {
	eventsFd = epoll_create1(EPOLL_CLOEXEC);
	if (eventsFd == -1) {
	    Tcl_Panic("epoll_create1: %s", strerror(errno));
	}
    }
#elif defined(HAVE_KQUEUE)
    if (eventsFd <= 0) {
	eventsFd = kqueue();
	if (eventsFd == -1) {
	    Tcl_Panic("kqueue: %s", strerror(errno));
	}
    }
#endif /* HAVE_EPOLL */
    if (!readyEvents) {
	maxReadyEvents = 128;
	readyEvents = ckalloc(maxReadyEvents * sizeof(readyEvents[0]));
    }
}

/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsTranslate --
 *	This function translates the platform-specific mask of returned events
 *	in and specific to a FileHandler to TCL_{READABLE,WRITABLE,EXCEPTION}
 *	bits.
 *
 * Results:
 *	Returns the translated and thus platform-independent mask.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
PlatformEventsTranslate(
    FileHandler *filePtr)
{
    int mask;

    mask = 0;
#if defined(HAVE_EPOLL)
    if (filePtr->platformReadyMask & EPOLLIN) {
	mask |= TCL_READABLE;
    }
    if (filePtr->platformReadyMask & EPOLLOUT) {
	mask |= TCL_WRITABLE;
    }
    if (filePtr->platformReadyMask & EPOLLERR) {
	mask |= TCL_EXCEPTION;
    }
#elif defined(HAVE_KQUEUE)
    if (filePtr->platformReadyMask & EVFILT_READ) {
	mask |= TCL_READABLE;
    }
    if (filePtr->platformReadyMask & EVFILT_WRITE) {
	mask |= TCL_WRITABLE;
    }
#endif /* HAVE_EPOLL */
    return mask;
}

/*
 *----------------------------------------------------------------------
 *
 * PlatformEventsWait --
 *	This function abstracts waiting for I/O events via either epoll_ctl(2)
 *	on Linux or kevent(2) on FreeBSD.
 *
 * Results:
 *	Returns -1 if epoll_ctl(2)/kevent(2) failed. Returns 0 if polling
 *	and if no events became available whilst polling. Returns a pointer
 *	to and the count of all returned events in all other cases.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

int
PlatformEventsWait(
#if defined(HAVE_EPOLL)
    struct epoll_event *events,
#elif defined(HAVE_KQUEUE)
    struct kevent *events,
#endif /* HAVE_EPOLL */
    int numEvents,
    struct timeval *timePtr)
{
    int numFound;
    struct timeval tv0, tv1, tv_delta;
#if defined(HAVE_EPOLL)
    int timeout;
#elif defined(HAVE_KQUEUE)
    struct timespec timeout, *timeoutPtr;
#endif /* HAVE_EPOLL */

    bzero(events, numEvents * sizeof(events[0]));
#if defined(HAVE_EPOLL)
    if (!timePtr) {
	timeout = -1;
    } else if (!timePtr->tv_sec && !timePtr->tv_usec) {
	timeout = 0;
    } else {
	timeout = timePtr->tv_sec;
    }
    gettimeofday(&tv0, NULL);
    numFound = epoll_wait(eventsFd, events, numEvents, timeout);
    gettimeofday(&tv1, NULL);

#elif defined(HAVE_KQUEUE)
    if (!timePtr) {
	timeoutPtr = NULL;
    } else if (!timePtr->tv_sec && !timePtr->tv_usec) {
	timeout.tv_sec = 0;
	timeout.tv_nsec = 0;
	timeoutPtr = &timeout;
    } else {
	timeout.tv_sec = timePtr->tv_sec;
	timeout.tv_nsec = timePtr->tv_usec * 1000;
	timeoutPtr = &timeout;
    }
    gettimeofday(&tv0, NULL);
    numFound = kevent(eventsFd, NULL, 0, events, numEvents, timeoutPtr);
    gettimeofday(&tv1, NULL);
#endif /* HAVE_EPOLL */

    if (timePtr) {
	timersub(&tv1, &tv0, &tv_delta);
	timersub(&tv_delta, timePtr, timePtr);
    }
    if (numFound == -1) {
	bzero(events, numEvents * sizeof(events[0]));
	return -1;
    }
    return numFound;
}
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateFileHandler --
 *
 *	This function registers a file handler with the select notifier.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Creates a new file handler structure.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_CreateFileHandler(
    int fd,			/* Handle of stream to watch. */
    int mask,			/* OR'ed combination of TCL_READABLE,
				 * TCL_WRITABLE, and TCL_EXCEPTION: indicates
				 * conditions under which proc should be
				 * called. */
    Tcl_FileProc *proc,		/* Function to call for each selected
				 * event. */
    ClientData clientData)	/* Arbitrary data to pass to proc. */
{
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
    int is_new;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

    if (tclNotifierHooks.createFileHandlerProc) {
	tclNotifierHooks.createFileHandlerProc(fd, mask, proc, clientData);
	return;
    } else {
	ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);
	FileHandler *filePtr;

	for (filePtr = tsdPtr->firstFileHandlerPtr; filePtr != NULL;
		filePtr = filePtr->nextPtr) {
	    if (filePtr->fd == fd) {
		break;
	    }
	}
	if (filePtr == NULL) {
	    filePtr = ckalloc(sizeof(FileHandler));
	    filePtr->fd = fd;
	    filePtr->readyMask = 0;
	    filePtr->nextPtr = tsdPtr->firstFileHandlerPtr;
	    tsdPtr->firstFileHandlerPtr = filePtr;
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	    is_new = 1;
	} else {
	    is_new = 0;
#endif /* HAVE_EPOLL */
	}
	filePtr->proc = proc;
	filePtr->clientData = clientData;
	filePtr->mask = mask;

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	if (eventsFd) {
	    if (is_new) {
		PlatformEventsControl(filePtr, tsdPtr,
			PLATFORMEVENTSCONTROL_ADD
			| PLATFORMEVENTSCONTROL_AUTO_MASK, 0);
	    } else {
		PlatformEventsControl(filePtr, tsdPtr,
			PLATFORMEVENTSCONTROL_MOD
			| PLATFORMEVENTSCONTROL_AUTO_MASK, 0);
	    }
	}
#else
	/*
	 * Update the check masks for this file.
	 */

	if (mask & TCL_READABLE) {
	    FD_SET(fd, &tsdPtr->checkMasks.readable);
	} else {
	    FD_CLR(fd, &tsdPtr->checkMasks.readable);
	}
	if (mask & TCL_WRITABLE) {
	    FD_SET(fd, &tsdPtr->checkMasks.writable);
	} else {
	    FD_CLR(fd, &tsdPtr->checkMasks.writable);
	}
	if (mask & TCL_EXCEPTION) {
	    FD_SET(fd, &tsdPtr->checkMasks.exception);
	} else {
	    FD_CLR(fd, &tsdPtr->checkMasks.exception);
	}
	if (tsdPtr->numFdBits <= fd) {
	    tsdPtr->numFdBits = fd+1;
	}
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_DeleteFileHandler --
 *
 *	Cancel a previously-arranged callback arrangement for a file.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If a callback was previously registered on file, remove it.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_DeleteFileHandler(
    int fd)			/* Stream id for which to remove callback
				 * function. */
{
    if (tclNotifierHooks.deleteFileHandlerProc) {
	tclNotifierHooks.deleteFileHandlerProc(fd);
	return;
    } else {
	FileHandler *filePtr, *prevPtr;
#ifdef USE_SELECT_FOR_NOTIFIER
	int i;
#endif /* USE_SELECT_FOR_NOTIFIER */
	ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

	/*
	 * Find the entry for the given file (and return if there isn't one).
	 */

	for (prevPtr = NULL, filePtr = tsdPtr->firstFileHandlerPtr; ;
		prevPtr = filePtr, filePtr = filePtr->nextPtr) {
	    if (filePtr == NULL) {
		return;
	    }
	    if (filePtr->fd == fd) {
		break;
	    }
	}

	/*
	 * Update the check masks for this file.
	 */

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	if (eventsFd) {
	    PlatformEventsControl(filePtr, tsdPtr, PLATFORMEVENTSCONTROL_DEL
		    | PLATFORMEVENTSCONTROL_AUTO_MASK, 0);
	}
	if (filePtr->platformEventData) {
	    ckfree(filePtr->platformEventData);
	}
#else
	if (filePtr->mask & TCL_READABLE) {
	    FD_CLR(fd, &tsdPtr->checkMasks.readable);
	}
	if (filePtr->mask & TCL_WRITABLE) {
	    FD_CLR(fd, &tsdPtr->checkMasks.writable);
	}
	if (filePtr->mask & TCL_EXCEPTION) {
	    FD_CLR(fd, &tsdPtr->checkMasks.exception);
	}

	/*
	 * Find current max fd.
	 */

	if (fd+1 == tsdPtr->numFdBits) {
	    int numFdBits = 0;

	    for (i = fd-1; i >= 0; i--) {
		if (FD_ISSET(i, &tsdPtr->checkMasks.readable)
			|| FD_ISSET(i, &tsdPtr->checkMasks.writable)
			|| FD_ISSET(i, &tsdPtr->checkMasks.exception)) {
		    numFdBits = i+1;
		    break;
		}
	    }
	    tsdPtr->numFdBits = numFdBits;
	}
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

	/*
	 * Clean up information in the callback record.
	 */

	if (prevPtr == NULL) {
	    tsdPtr->firstFileHandlerPtr = filePtr->nextPtr;
	} else {
	    prevPtr->nextPtr = filePtr->nextPtr;
	}
	ckfree(filePtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FileHandlerEventProc --
 *
 *	This function is called by Tcl_ServiceEvent when a file event reaches
 *	the front of the event queue. This function is responsible for
 *	actually handling the event by invoking the callback for the file
 *	handler.
 *
 * Results:
 *	Returns 1 if the event was handled, meaning it should be removed from
 *	the queue. Returns 0 if the event was not handled, meaning it should
 *	stay on the queue. The only time the event isn't handled is if the
 *	TCL_FILE_EVENTS flag bit isn't set.
 *
 * Side effects:
 *	Whatever the file handler's callback function does.
 *
 *----------------------------------------------------------------------
 */

static int
FileHandlerEventProc(
    Tcl_Event *evPtr,		/* Event to service. */
    int flags)			/* Flags that indicate what events to handle,
				 * such as TCL_FILE_EVENTS. */
{
    int mask;
    FileHandler *filePtr;
    FileHandlerEvent *fileEvPtr = (FileHandlerEvent *) evPtr;
    ThreadSpecificData *tsdPtr;

    if (!(flags & TCL_FILE_EVENTS)) {
	return 0;
    }

    /*
     * Search through the file handlers to find the one whose handle matches
     * the event. We do this rather than keeping a pointer to the file handler
     * directly in the event, so that the handler can be deleted while the
     * event is queued without leaving a dangling pointer.
     */

    tsdPtr = TCL_TSD_INIT(&dataKey);

    for (filePtr = tsdPtr->firstFileHandlerPtr; filePtr != NULL;
	    filePtr = filePtr->nextPtr) {
	if (filePtr->fd != fileEvPtr->fd) {
	    continue;
	}

	/*
	 * The code is tricky for two reasons:
	 * 1. The file handler's desired events could have changed since the
	 *    time when the event was queued, so AND the ready mask with the
	 *    desired mask.
	 * 2. The file could have been closed and re-opened since the time
	 *    when the event was queued. This is why the ready mask is stored
	 *    in the file handler rather than the queued event: it will be
	 *    zeroed when a new file handler is created for the newly opened
	 *    file.
	 */

	mask = filePtr->readyMask & filePtr->mask;
	filePtr->readyMask = 0;
	if (mask != 0) {
	    filePtr->proc(filePtr->clientData, mask);
	}
	break;
    }
    return 1;
}

#if defined(TCL_THREADS) && defined(__CYGWIN__)

static DWORD __stdcall
NotifierProc(
    void *hwnd,
    unsigned int message,
    void *wParam,
    void *lParam)
{
    ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

    if (message != 1024) {
	return DefWindowProcW(hwnd, message, wParam, lParam);
    }

    /*
     * Process all of the runnable events.
     */

    tsdPtr->eventReady = 1;
    Tcl_ServiceAll();
    return 0;
}
#endif /* TCL_THREADS && __CYGWIN__ */

/*
 *----------------------------------------------------------------------
 *
 * Tcl_WaitForEvent --
 *
 *	This function is called by Tcl_DoOneEvent to wait for new events on
 *	the message queue. If the block time is 0, then Tcl_WaitForEvent just
 *	polls without blocking.
 *
 * Results:
 *	Returns -1 if the select would block forever, otherwise returns 0.
 *
 * Side effects:
 *	Queues file events that are detected by the select.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_WaitForEvent(
    const Tcl_Time *timePtr)		/* Maximum block time, or NULL. */
{
    if (tclNotifierHooks.waitForEventProc) {
	return tclNotifierHooks.waitForEventProc(timePtr);
    } else {
	FileHandler *filePtr;
	int mask;
	Tcl_Time vTime;
#ifdef TCL_THREADS
	int waitForFiles;
#   ifdef __CYGWIN__
	MSG msg;
#   endif /* __CYGWIN__ */
#else
	/*
	 * Impl. notes: timeout & timeoutPtr are used if, and only if threads
	 * are not enabled. They are the arguments for the regular select()
	 * used when the core is not thread-enabled.
	 */

	struct timeval timeout, *timeoutPtr;
	int numFound;
#endif /* TCL_THREADS */
	ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

	/*
	 * Set up the timeout structure. Note that if there are no events to
	 * check for, we return with a negative result rather than blocking
	 * forever.
	 */

	if (timePtr != NULL) {
	    /*
	     * TIP #233 (Virtualized Time). Is virtual time in effect? And do
	     * we actually have something to scale? If yes to both then we
	     * call the handler to do this scaling.
	     */

	    if (timePtr->sec != 0 || timePtr->usec != 0) {
		vTime = *timePtr;
		tclScaleTimeProcPtr(&vTime, tclTimeClientData);
		timePtr = &vTime;
	    }
#ifndef TCL_THREADS
	    timeout.tv_sec = timePtr->sec;
	    timeout.tv_usec = timePtr->usec;
	    timeoutPtr = &timeout;
	} else if (tsdPtr->numFdBits == 0) {
	    /*
	     * If there are no threads, no timeout, and no fds registered,
	     * then there are no events possible and we must avoid deadlock.
	     * Note that this is not entirely correct because there might be a
	     * signal that could interrupt the select call, but we don't
	     * handle that case if we aren't using threads.
	     */

	    return -1;
	} else {
	    timeoutPtr = NULL;
#endif /* !TCL_THREADS */
	}

#ifdef TCL_THREADS
	/*
	 * Start notifier thread and place this thread on the list of
	 * interested threads, signal the notifier thread, and wait for a
	 * response or a timeout.
	 */
	StartNotifierThread("Tcl_WaitForEvent");

	pthread_mutex_lock(&notifierMutex);

	if (timePtr != NULL && timePtr->sec == 0 && (timePtr->usec == 0
#if defined(__APPLE__) && defined(__LP64__)
		/*
		 * On 64-bit Darwin, pthread_cond_timedwait() appears to have
		 * a bug that causes it to wait forever when passed an
		 * absolute time which has already been exceeded by the system
		 * time; as a workaround, when given a very brief timeout,
		 * just do a poll. [Bug 1457797]
		 */
		|| timePtr->usec < 10
#endif /* __APPLE__ && __LP64__ */
		)) {
	    /*
	     * Cannot emulate a polling select with a polling condition
	     * variable. Instead, pretend to wait for files and tell the
	     * notifier thread what we are doing. The notifier thread makes
	     * sure it goes through select with its select mask in the same
	     * state as ours currently is. We block until that happens.
	     */

	    waitForFiles = 1;
	    tsdPtr->pollState = POLL_WANT;
	    timePtr = NULL;
	} else {
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	    waitForFiles = 1;
#else
	    waitForFiles = (tsdPtr->numFdBits > 0);
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
	    tsdPtr->pollState = 0;
	}

	if (waitForFiles) {
	    /*
	     * Add the ThreadSpecificData structure of this thread to the list
	     * of ThreadSpecificData structures of all threads that are
	     * waiting on file events.
	     */

	    tsdPtr->nextPtr = waitingListPtr;
	    if (waitingListPtr) {
		waitingListPtr->prevPtr = tsdPtr;
	    }
	    tsdPtr->prevPtr = 0;
	    waitingListPtr = tsdPtr;
	    tsdPtr->onList = 1;

	    if ((write(triggerPipe, "", 1) == -1) && (errno != EAGAIN)) {
		Tcl_Panic("Tcl_WaitForEvent: %s",
			"unable to write to triggerPipe");
	    }
	}

#ifdef USE_SELECT_FOR_NOTIFIER
	FD_ZERO(&tsdPtr->readyMasks.readable);
	FD_ZERO(&tsdPtr->readyMasks.writable);
	FD_ZERO(&tsdPtr->readyMasks.exception);
#endif /* USE_SELECT_FOR_NOTIFIER */

	if (!tsdPtr->eventReady) {
#ifdef __CYGWIN__
	    if (!PeekMessageW(&msg, NULL, 0, 0, 0)) {
		DWORD timeout;

		if (timePtr) {
		    timeout = timePtr->sec * 1000 + timePtr->usec / 1000;
		} else {
		    timeout = 0xFFFFFFFF;
		}
		pthread_mutex_unlock(&notifierMutex);
		MsgWaitForMultipleObjects(1, &tsdPtr->event, 0, timeout, 1279);
		pthread_mutex_lock(&notifierMutex);
	    }
#else /* !__CYGWIN__ */
	    if (timePtr != NULL) {
	       Tcl_Time now;
	       struct timespec ptime;

	       Tcl_GetTime(&now);
	       ptime.tv_sec = timePtr->sec + now.sec + (timePtr->usec + now.usec) / 1000000;
	       ptime.tv_nsec = 1000 * ((timePtr->usec + now.usec) % 1000000);

	       pthread_cond_timedwait(&tsdPtr->waitCV, &notifierMutex, &ptime);
	    } else {
	       pthread_cond_wait(&tsdPtr->waitCV, &notifierMutex);
	    }
#endif /* __CYGWIN__ */
	}
	tsdPtr->eventReady = 0;

#ifdef __CYGWIN__
	while (PeekMessageW(&msg, NULL, 0, 0, 0)) {
	    /*
	     * Retrieve and dispatch the message.
	     */

	    DWORD result = GetMessageW(&msg, NULL, 0, 0);

	    if (result == 0) {
		PostQuitMessage(msg.wParam);
		/* What to do here? */
	    } else if (result != (DWORD) -1) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	    }
	}
	ResetEvent(tsdPtr->event);
#endif /* __CYGWIN__ */

	if (waitForFiles && tsdPtr->onList) {
	    /*
	     * Remove the ThreadSpecificData structure of this thread from the
	     * waiting list. Alert the notifier thread to recompute its select
	     * masks - skipping this caused a hang when trying to close a pipe
	     * which the notifier thread was still doing a select on.
	     */

	    if (tsdPtr->prevPtr) {
		tsdPtr->prevPtr->nextPtr = tsdPtr->nextPtr;
	    } else {
		waitingListPtr = tsdPtr->nextPtr;
	    }
	    if (tsdPtr->nextPtr) {
		tsdPtr->nextPtr->prevPtr = tsdPtr->prevPtr;
	    }
	    tsdPtr->nextPtr = tsdPtr->prevPtr = NULL;
	    tsdPtr->onList = 0;
	    if ((write(triggerPipe, "", 1) == -1) && (errno != EAGAIN)) {
		Tcl_Panic("Tcl_WaitForEvent: %s",
			"unable to write to triggerPipe");
	    }
	}

#else
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	tsdPtr->numReadyEvents = PlatformEventsWait(readyEvents,
		maxReadyEvents, timeoutPtr);
#else
	tsdPtr->readyMasks = tsdPtr->checkMasks;
	numFound = select(tsdPtr->numFdBits, &tsdPtr->readyMasks.readable,
		&tsdPtr->readyMasks.writable, &tsdPtr->readyMasks.exception,
		timeoutPtr);

	/*
	 * Some systems don't clear the masks after an error, so we have to do
	 * it here.
	 */

	if (numFound == -1) {
	    FD_ZERO(&tsdPtr->readyMasks.readable);
	    FD_ZERO(&tsdPtr->readyMasks.writable);
	    FD_ZERO(&tsdPtr->readyMasks.exception);
	}
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
#endif /* TCL_THREADS */

	/*
	 * Queue all detected file events before returning.
	 */

	for (filePtr = tsdPtr->firstFileHandlerPtr; (filePtr != NULL);
		filePtr = filePtr->nextPtr) {
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	    mask = PlatformEventsTranslate(filePtr);
#else
	    mask = 0;
	    if (FD_ISSET(filePtr->fd, &tsdPtr->readyMasks.readable)) {
		mask |= TCL_READABLE;
	    }
	    if (FD_ISSET(filePtr->fd, &tsdPtr->readyMasks.writable)) {
		mask |= TCL_WRITABLE;
	    }
	    if (FD_ISSET(filePtr->fd, &tsdPtr->readyMasks.exception)) {
		mask |= TCL_EXCEPTION;
	    }
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

	    if (!mask) {
		continue;
	    }

	    /*
	     * Don't bother to queue an event if the mask was previously
	     * non-zero since an event must still be on the queue.
	     */

	    if (filePtr->readyMask == 0) {
		FileHandlerEvent *fileEvPtr =
			ckalloc(sizeof(FileHandlerEvent));

		fileEvPtr->header.proc = FileHandlerEventProc;
		fileEvPtr->fd = filePtr->fd;
		Tcl_QueueEvent((Tcl_Event *) fileEvPtr, TCL_QUEUE_TAIL);
	    }
	    filePtr->readyMask = mask;
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	    filePtr->platformReadyMask = 0;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
	}
#ifdef TCL_THREADS
	pthread_mutex_unlock(&notifierMutex);
#endif /* TCL_THREADS */
	return 0;
    }
}

#ifdef TCL_THREADS
/*
 *----------------------------------------------------------------------
 *
 * AlertSingleThread --
 *
 *	Notify a single thread that is waiting on a file descriptor to become
 *	readable or writable or to have an exception condition.
 *	notifierMutex must be held.
 *
 * Result:
 *	None.
 *
 * Side effects:
 *	The condition variable associated with the thread is broadcasted.
 *
 *----------------------------------------------------------------------
 */

static void
AlertSingleThread(
    ThreadSpecificData *tsdPtr)
{
    tsdPtr->eventReady = 1;
    if (tsdPtr->onList) {
        /*
         * Remove the ThreadSpecificData structure of this thread
         * from the waiting list. This prevents us from
         * continuously spining on select until the other threads
         * runs and services the file event.
         */
    
        if (tsdPtr->prevPtr) {
    	    tsdPtr->prevPtr->nextPtr = tsdPtr->nextPtr;
        } else {
    	    waitingListPtr = tsdPtr->nextPtr;
        }
        if (tsdPtr->nextPtr) {
    	    tsdPtr->nextPtr->prevPtr = tsdPtr->prevPtr;
        }
        tsdPtr->nextPtr = tsdPtr->prevPtr = NULL;
        tsdPtr->onList = 0;
        tsdPtr->pollState = 0;
    }
#ifdef __CYGWIN__
    PostMessageW(tsdPtr->hwnd, 1024, 0, 0);
#else /* __CYGWIN__ */
    pthread_cond_broadcast(&tsdPtr->waitCV);
#endif /* __CYGWIN__ */
}

/*
 *----------------------------------------------------------------------
 *
 * NotifierThreadProc --
 *
 *	This routine is the initial (and only) function executed by the
 *	special notifier thread. Its job is to wait for file descriptors to
 *	become readable or writable or to have an exception condition and then
 *	to notify other threads who are interested in this information by
 *	signalling a condition variable. Other threads can signal this
 *	notifier thread of a change in their interests by writing a single
 *	byte to a special pipe that the notifier thread is monitoring.
 *
 * Result:
 *	None. Once started, this routine never exits. It dies with the overall
 *	process.
 *
 * Side effects:
 *	The trigger pipe used to signal the notifier thread is created when
 *	the notifier thread first starts.
 *
 *----------------------------------------------------------------------
 */

static void
NotifierThreadProc(
    ClientData clientData)	/* Not used. */
{
    ThreadSpecificData *tsdPtr;
#ifdef USE_SELECT_FOR_NOTIFIER
    fd_set readableMask;
    fd_set writableMask;
    fd_set exceptionMask;
#endif /* USE_SELECT_FOR_NOTIFIER */
    int i;
    int fds[2], receivePipe;
    long found;
    struct timeval poll = {0., 0.}, *timePtr;
    char buf[2];
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
    int numReadyEvents, numEvent;
    FileHandler *filePtr_rP;
    struct PlatformEventData *eventDataPtr;
#else
    int numFdBits = 0;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

    if (pipe(fds) != 0) {
	Tcl_Panic("NotifierThreadProc: %s", "could not create trigger pipe");
    }

    receivePipe = fds[0];

    if (TclUnixSetBlockingMode(receivePipe, TCL_MODE_NONBLOCKING) < 0) {
	Tcl_Panic("NotifierThreadProc: %s",
		"could not make receive pipe non blocking");
    }
    if (TclUnixSetBlockingMode(fds[1], TCL_MODE_NONBLOCKING) < 0) {
	Tcl_Panic("NotifierThreadProc: %s",
		"could not make trigger pipe non blocking");
    }
    if (fcntl(receivePipe, F_SETFD, FD_CLOEXEC) < 0) {
	Tcl_Panic("NotifierThreadProc: %s",
		"could not make receive pipe close-on-exec");
    }
    if (fcntl(fds[1], F_SETFD, FD_CLOEXEC) < 0) {
	Tcl_Panic("NotifierThreadProc: %s",
		"could not make trigger pipe close-on-exec");
    }

    /*
     * Install the write end of the pipe into the global variable.
     */

    pthread_mutex_lock(&notifierMutex);
    triggerPipe = fds[1];

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
    /*
     * Set up the epoll/kqueue fd to include the receive pipe.
     */

    filePtr_rP = ckalloc(sizeof(*filePtr_rP));
    filePtr_rP->fd = receivePipe;
    tsdPtr = TCL_TSD_INIT(&dataKey);
    PlatformEventsControl(filePtr_rP, tsdPtr, PLATFORMEVENTSCONTROL_ADD,
	    TCL_READABLE);
    tsdPtr = NULL;
#endif /* HAVE_EPOLL || HAVE_KQUEUE*/

    /*
     * Signal any threads that are waiting.
     */

    pthread_cond_broadcast(&notifierCV);
    pthread_mutex_unlock(&notifierMutex);

    /*
     * Look for file events and report them to interested threads.
     */

    while (1) {
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	pthread_mutex_lock(&notifierMutex);
	timePtr = NULL;
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    if (tsdPtr->pollState & POLL_WANT) {
		/*
		 * Here we make sure we go through select() with the same mask
		 * bits that were present when the thread tried to poll.
		 */

		tsdPtr->pollState |= POLL_DONE;
		timePtr = &poll;
	    }
	}
	pthread_mutex_unlock(&notifierMutex);
#else
	FD_ZERO(&readableMask);
	FD_ZERO(&writableMask);
	FD_ZERO(&exceptionMask);

	/*
	 * Compute the logical OR of the masks from all the waiting
	 * notifiers.
	 */

	pthread_mutex_lock(&notifierMutex);
	timePtr = NULL;
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    for (i = tsdPtr->numFdBits-1; i >= 0; --i) {
		if (FD_ISSET(i, &tsdPtr->checkMasks.readable)) {
		    FD_SET(i, &readableMask);
		}
		if (FD_ISSET(i, &tsdPtr->checkMasks.writable)) {
		    FD_SET(i, &writableMask);
		}
		if (FD_ISSET(i, &tsdPtr->checkMasks.exception)) {
		    FD_SET(i, &exceptionMask);
		}
	    }
	    if (tsdPtr->numFdBits > numFdBits) {
		numFdBits = tsdPtr->numFdBits;
	    }
	    if (tsdPtr->pollState & POLL_WANT) {
		/*
		 * Here we make sure we go through select() with the same mask
		 * bits that were present when the thread tried to poll.
		 */

		tsdPtr->pollState |= POLL_DONE;
		timePtr = &poll;
	    }
	}
	pthread_mutex_unlock(&notifierMutex);
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
#ifdef USE_SELECT_FOR_NOTIFIER
	/*
	 * Set up the mask to include the receive pipe.
	 */

	if (receivePipe >= numFdBits) {
	    numFdBits = receivePipe + 1;
	}
	FD_SET(receivePipe, &readableMask);
#endif /* USE_SELECT_FOR_NOTIFIER */
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	numReadyEvents = PlatformEventsWait(readyEvents, maxReadyEvents,
		timePtr);
	if (numReadyEvents == -1) {
	    /*
	     * Try again immediately on an error.
	     */

	    numReadyEvents = 0;
	    continue;
	}
#else
	if (select(numFdBits, &readableMask, &writableMask, &exceptionMask,
		timePtr) == -1) {
	    /*
	     * Try again immediately on an error.
	     */

	    continue;
	}
#endif /* HAVE_EPOLL || HAVE_KQUEUE*/

	/*
	 * Alert any threads that are waiting on a ready file descriptor.
	 */

	pthread_mutex_lock(&notifierMutex);
	numEvent = 0;
#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    if (tsdPtr->pollState & POLL_DONE) {
		AlertSingleThread(tsdPtr);
	    }
	}
	while (1) {
	    numEvent = PlatformEventsGet(numEvent, numReadyEvents,
		    receivePipe, 0, 1);
	    if (numEvent == -1) {
		break;
	    }
#if defined(HAVE_EPOLL)
	    eventDataPtr = readyEvents[numEvent].data.ptr;
	    eventDataPtr->filePtr->platformReadyMask =
		    readyEvents[numEvent].events;
#elif defined(HAVE_KQUEUE)
	    eventDataPtr = readyEvents[numEvent].udata;
	    if (readyEvents[numEvent].filter == EVFILT_READ) {
		if (readyEvents[numEvent].flags & (EV_EOF | EV_ERROR)) {
		    eventDataPtr->filePtr->platformReadyMask |= EV_ERROR;
		} else {
		    eventDataPtr->filePtr->platformReadyMask
			    |= EVFILT_READ;
		}
	    }
	    if (readyEvents[numEvent].filter == EVFILT_WRITE) {
		if (readyEvents[numEvent].flags & (EV_EOF | EV_ERROR)) {
		    eventDataPtr->filePtr->platformReadyMask |= EV_ERROR;
		} else {
		    eventDataPtr->filePtr->platformReadyMask
			    |= EVFILT_WRITE;
		}
	    }
#endif /* HAVE_EPOLL */
	    numEvent++;
	    found = 1;
	    tsdPtr = eventDataPtr->tsdPtr;
#else
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    found = 0;

	    for (i = tsdPtr->numFdBits-1; i >= 0; --i) {
		if (FD_ISSET(i, &tsdPtr->checkMasks.readable)
			&& FD_ISSET(i, &readableMask)) {
		    FD_SET(i, &tsdPtr->readyMasks.readable);
		    found = 1;
		}
		if (FD_ISSET(i, &tsdPtr->checkMasks.writable)
			&& FD_ISSET(i, &writableMask)) {
		    FD_SET(i, &tsdPtr->readyMasks.writable);
		    found = 1;
		}
		if (FD_ISSET(i, &tsdPtr->checkMasks.exception)
			&& FD_ISSET(i, &exceptionMask)) {
		    FD_SET(i, &tsdPtr->readyMasks.exception);
		    found = 1;
		}
	    }
#endif /* HAVE_EPOLL || HAVE_KQUEUE */

	    if (found || (tsdPtr->pollState & POLL_DONE)) {
		AlertSingleThread(tsdPtr);
	    }
	}
	pthread_mutex_unlock(&notifierMutex);

	/*
	 * Consume the next byte from the notifier pipe if the pipe was
	 * readable. Note that there may be multiple bytes pending, but to
	 * avoid a race condition we only read one at a time.
	 */

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
	found = 0;
	numEvent = 0;
	while (1) {
	    numEvent = PlatformEventsGet(numEvent, numReadyEvents, 0,
		    receivePipe, 0);
	    if (numEvent == -1) {
		break;
	    }
#if defined(HAVE_EPOLL)
	    eventDataPtr = readyEvents[numEvent].data.ptr;
	    eventDataPtr->filePtr->platformReadyMask =
		    readyEvents[numEvent].events;
#elif defined(HAVE_KQUEUE)
	    eventDataPtr = readyEvents[numEvent].udata;
	    eventDataPtr->filePtr->platformReadyMask =
		    readyEvents[numEvent].filter;
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
	    if (eventDataPtr->filePtr->fd == receivePipe) {
		found = 1;
		break;
	    }
	    numEvent++;
	}
#if defined(HAVE_EPOLL)
	if (found && readyEvents[numEvent].events & EPOLLIN)
#elif defined(HAVE_KQUEUE)
	if (found && (readyEvents[numEvent].filter == EVFILT_READ)
		&& !(readyEvents[numEvent].flags & (EV_EOF | EV_ERROR)))
#else
	if (FD_ISSET(receivePipe, &readableMask))
#endif /* HAVE_EPOLL */
	{
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
	    i = read(receivePipe, buf, 1);

	    if ((i == 0) || ((i == 1) && (buf[0] == 'q'))) {
		/*
		 * Someone closed the write end of the pipe or sent us a Quit
		 * message [Bug: 4139] and then closed the write end of the
		 * pipe so we need to shut down the notifier thread.
		 */

		break;
	    }
	}
    }

    /*
     * Clean up the read end of the pipe and signal any threads waiting on
     * termination of the notifier thread.
     */

#if defined(HAVE_EPOLL) || defined(HAVE_KQUEUE)
    PlatformEventsControl(filePtr_rP, tsdPtr, PLATFORMEVENTSCONTROL_DEL,
	    TCL_READABLE);
#endif /* HAVE_EPOLL || HAVE_KQUEUE */
    close(receivePipe);
    pthread_mutex_lock(&notifierMutex);
    triggerPipe = -1;
    pthread_cond_broadcast(&notifierCV);
    pthread_mutex_unlock(&notifierMutex);

    TclpThreadExit(0);
}

#if defined(HAVE_PTHREAD_ATFORK)
/*
 *----------------------------------------------------------------------
 *
 * AtForkChild --
 *
 *	Unlock and reinstall the notifier in the child after a fork.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static void
AtForkChild(void)
{
    if (notifierThreadRunning == 1) {
	pthread_cond_destroy(&notifierCV);
    }
    pthread_mutex_init(&notifierInitMutex, NULL);
    pthread_mutex_init(&notifierMutex, NULL);
    pthread_cond_init(&notifierCV, NULL);

    /*
     * notifierThreadRunning == 1: thread is running, (there might be data in
     *				   notifier lists)
     * atForkInit == 0: InitNotifier was never called
     * notifierCount != 0: unbalanced  InitNotifier() / FinalizeNotifier calls
     * waitingListPtr != 0: there are threads currently waiting for events.
     */

    if (atForkInit == 1) {
	notifierCount = 0;
	if (notifierThreadRunning == 1) {
	    ThreadSpecificData *tsdPtr = TCL_TSD_INIT(&dataKey);

	    notifierThreadRunning = 0;
	    close(triggerPipe);
	    triggerPipe = -1;

	    /*
	     * The waitingListPtr might contain event info from multiple
	     * threads, which are invalid here, so setting it to NULL is not
	     * unreasonable.
	     */

	    waitingListPtr = NULL;

	    /*
	     * The tsdPtr from before the fork is copied as well.  But since
	     * we are paranoic, we don't trust its condvar and reset it.
	     */

#ifdef __CYGWIN__
	    DestroyWindow(tsdPtr->hwnd);
	    tsdPtr->hwnd = CreateWindowExW(NULL, className,
		    className, 0, 0, 0, 0, 0, NULL, NULL,
		    TclWinGetTclInstance(), NULL);
	    ResetEvent(tsdPtr->event);
#else
	    pthread_cond_destroy(&tsdPtr->waitCV);
	    pthread_cond_init(&tsdPtr->waitCV, NULL);
#endif
	    /*
	     * In case, we had multiple threads running before the fork,
	     * make sure, we don't try to reach out to their thread local data.
	     */
	    tsdPtr->nextPtr = tsdPtr->prevPtr = NULL;

	    /*
	     * The list of registered event handlers at fork time is in
	     * tsdPtr->firstFileHandlerPtr;
	     */
	}
    }

    Tcl_InitNotifier();
}
#endif /* HAVE_PTHREAD_ATFORK */

#endif /* TCL_THREADS */

#endif /* !HAVE_COREFOUNDATION */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
