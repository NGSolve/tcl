/*
 * tclUnixPoll.c --
 *
 *	This file contains the implementation of the poll()-based
 *	Unix-specific notifier, which is the lowest-level part of the Tcl
 *	event loop. This file works together with generic/tclNotify.c.
 *
 * Copyright (c) 1995-1997 Sun Microsystems, Inc.
 * Copyright (c) 2009-2010 Donal K. Fellows
 *
 * See the file "license.terms" for information on usage and redistribution of
 * this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tclInt.h"
#include <poll.h>
#include <signal.h>

/*
 * This structure is used to keep track of the notifier info for a registered
 * file.
 */

typedef struct FileHandler {
    int fd;
    int pollIndex;		/* Index into the 'struct pollfd' array. Only
				 * valid in a single thread. */
    int mask;			/* Mask of desired events: TCL_READABLE,
				 * etc. */
    int readyMask;		/* Mask of events that have been seen since
				 * the last time file handlers were invoked
				 * for this file. */
    Tcl_FileProc *proc;		/* Function to call, in the style of
				 * Tcl_CreateFileHandler. */
    ClientData clientData;	/* Argument to pass to proc. */
    struct FileHandler *nextPtr;/* Next in list of all files we care about. */
} FileHandler;

/*
 * The following structure is what is added to the Tcl event queue when file
 * handlers are ready to fire.
 */

typedef struct FileHandlerEvent {
    Tcl_Event header;		/* Information that is standard for all
				 * events. */
    int fd;			/* File descriptor that is ready. Used to find
				 * the FileHandler structure for the file
				 * (can't point directly to the FileHandler
				 * structure because it could go away while
				 * the event is queued). */
} FileHandlerEvent;

/*
 * The following structure contains a set of poll() structures to track
 * readable, writable, and exceptional conditions.
 */

#define DEFAULT_POLL_FDS_SIZE 32
typedef struct {
    nfds_t nfds;
    nfds_t maxNfds;
    struct pollfd *fds;
    struct pollfd defaultFds[DEFAULT_POLL_FDS_SIZE];
} PollData;

/*
 * The following static structure contains the state information for the
 * poll-based implementation of the Tcl notifier. One of these structures is
 * created for each thread that is using the notifier.
 */

typedef struct ThreadSpecificData {
    FileHandler *firstFileHandlerPtr;
				/* Pointer to head of file handler list. */
    PollData pollInfo;
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
    Tcl_Condition waitCV;	/* Any other thread alerts a notifier that an
				 * event is ready to be processed by signaling
				 * this condition variable. */
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
 * The notifier thread spends all its time in poll() waiting for a file
 * descriptor associated with one of the threads on the waitingListPtr list to
 * do something interesting. But if the contents of the waitingListPtr list
 * ever changes, we need to wake up and restart the poll() system call. You
 * can wake up the notifier thread by writing a single byte to the file
 * descriptor defined below. This file descriptor is the input-end of a pipe
 * and the notifier thread is listening for data on the output-end of the same
 * pipe. Hence writing to this file descriptor will cause the poll() system
 * call to return and wake up the notifier thread.
 *
 * You must hold the notifierMutex lock before writing to the pipe.
 */

static int triggerPipe = -1;

/*
 * The notifierMutex locks access to all of the global notifier state.
 */

TCL_DECLARE_MUTEX(notifierMutex)

/*
 * The notifier thread signals the notifierCV when it has finished
 * initializing the triggerPipe and right before the notifier thread
 * terminates.
 */

static Tcl_Condition notifierCV;

/*
 * The pollState bits
 *	POLL_WANT is set by each thread before it waits on its condition
 *		variable. It is checked by the notifier before it does select.
 *	POLL_DONE is set by the notifier if it goes into select after seeing
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

/*
 * Static routines defined in this file.
 */

#ifdef TCL_THREADS
static void		NotifierThreadProc(ClientData clientData);
#endif /* TCL_THREADS */
static int		FileHandlerEventProc(Tcl_Event *evPtr, int flags);
static int		ComparePollFDStructures(const void *, const void *);

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
    ThreadSpecificData *tsdPtr;

    if (tclNotifierHooks.initNotifierProc) {
	return tclNotifierHooks.initNotifierProc();
    }

    tsdPtr = TCL_TSD_INIT(&dataKey);
    tsdPtr->pollInfo.maxNfds = DEFAULT_POLL_FDS_SIZE;
    tsdPtr->pollInfo.fds = tsdPtr->pollInfo.defaultFds;
#ifdef TCL_THREADS
    tsdPtr->pollInfo.nfds = 1; // For the control pipe
    tsdPtr->eventReady = 0;

    /*
     * Start the Notifier thread if necessary.
     */

    Tcl_MutexLock(&notifierMutex);
    if (notifierCount == 0) {
	if (TclpThreadCreate(&notifierThread, NotifierThreadProc, NULL,
		TCL_THREAD_STACK_DEFAULT, TCL_THREAD_JOINABLE) != TCL_OK) {
	    Tcl_Panic("Tcl_InitNotifier: unable to start notifier thread");
	}
    }
    notifierCount++;

    /*
     * Wait for the notifier pipe to be created.
     */

    while (triggerPipe < 0) {
	Tcl_ConditionWait(&notifierCV, &notifierMutex, NULL);
    }

    Tcl_MutexUnlock(&notifierMutex);
#endif /* TCL_THREADS */

    return tsdPtr;
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
#ifdef TCL_THREADS
    ThreadSpecificData *tsdPtr;
#endif

    if (tclNotifierHooks.finalizeNotifierProc) {
	tclNotifierHooks.finalizeNotifierProc(clientData);
	return;
    }

#ifdef TCL_THREADS
    tsdPtr = TCL_TSD_INIT(&dataKey);
    Tcl_MutexLock(&notifierMutex);
    notifierCount--;

    /*
     * If this is the last thread to use the notifier, close the notifier pipe
     * and wait for the background thread to terminate.
     */

    if (notifierCount == 0) {
	int result;

	if (triggerPipe < 0) {
	    Tcl_Panic("Tcl_FinalizeNotifier: notifier pipe not initialized");
	}

	/*
	 * Send "q" message to the notifier thread so that it will terminate.
	 * The notifier will return from its call to select() and notice that
	 * a "q" message has arrived, it will then close its side of the pipe
	 * and terminate its thread. Note the we can not just close the pipe
	 * and check for EOF in the notifier thread because if a background
	 * child process was created with exec, select() would not register
	 * the EOF on the pipe until the child processes had terminated. [Bug:
	 * 4139] [Bug: 1222872]
	 */

	write(triggerPipe, "q", 1);
	close(triggerPipe);
	while(triggerPipe >= 0) {
	    Tcl_ConditionWait(&notifierCV, &notifierMutex, NULL);
	}

	result = Tcl_JoinThread(notifierThread, NULL);
	if (result) {
	    Tcl_Panic("Tcl_FinalizeNotifier: unable to join notifier thread");
	}
    }

    /*
     * Clean up any synchronization objects in the thread local storage.
     */

    Tcl_ConditionFinalize(&tsdPtr->waitCV);

    Tcl_MutexUnlock(&notifierMutex);
#endif
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

	Tcl_MutexLock(&notifierMutex);
	tsdPtr->eventReady = 1;
	Tcl_ConditionNotify(&tsdPtr->waitCV);
	Tcl_MutexUnlock(&notifierMutex);
#endif
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
    }

    /*
     * The interval timer doesn't do anything in this implementation, because
     * the only event loop is via Tcl_DoOneEvent, which passes timeout values
     * to Tcl_WaitForEvent.
     */
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
    }

    /* Does nothing in this implementation. */
}

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
    ThreadSpecificData *tsdPtr;
    FileHandler *filePtr;
    struct pollfd *pollPtr;

    if (tclNotifierHooks.createFileHandlerProc) {
	tclNotifierHooks.createFileHandlerProc(fd, mask, proc, clientData);
	return;
    }
    tsdPtr = TCL_TSD_INIT(&dataKey);

    /*
     * Locate and update in list or splice new record in if it didn't exist.
     */

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
	filePtr->pollIndex = tsdPtr->pollInfo.nfds++;
    }
    filePtr->proc = proc;
    filePtr->clientData = clientData;
    filePtr->mask = mask;

    /*
     * Update the check masks for this file.
     */


    // Grow the array that will be passed into poll()
    if (tsdPtr->pollInfo.nfds > tsdPtr->pollInfo.maxNfds) {
	unsigned newSize = tsdPtr->pollInfo.maxNfds;

	while (tsdPtr->pollInfo.nfds > newSize) {
	    newSize *= 2;
	}

	if (tsdPtr->pollInfo.fds == tsdPtr->pollInfo.defaultFds) {
	    tsdPtr->pollInfo.fds = ckalloc(sizeof(struct pollfd) * newSize);
	    memcpy(tsdPtr->pollInfo.fds, tsdPtr->pollInfo.defaultFds,
		    sizeof(struct pollfd) * tsdPtr->pollInfo.maxNfds);
	} else {
	    tsdPtr->pollInfo.fds = ckrealloc(tsdPtr->pollInfo.fds,
		    sizeof(struct pollfd) * newSize);
	    memset(tsdPtr->pollInfo.fds + tsdPtr->pollInfo.maxNfds, 0,
		    sizeof(struct pollfd)*(newSize-tsdPtr->pollInfo.maxNfds));
	}
	tsdPtr->pollInfo.maxNfds = newSize;
    }

    pollPtr = &tsdPtr->pollInfo.fds[filePtr->pollIndex];

    pollPtr->fd = fd;
    if (mask & TCL_READABLE) {
	pollPtr->events |= POLLIN;
    } else {
	pollPtr->events &= ~POLLIN;
    }
    if (mask & TCL_WRITABLE) {
	pollPtr->events |= POLLOUT;
    } else {
	pollPtr->events &= ~POLLOUT;
    }
    if (mask & TCL_EXCEPTION) {
	pollPtr->events |= POLLERR | POLLHUP;
    } else {
	pollPtr->events &= ~(POLLERR | POLLHUP);
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
    FileHandler *filePtr, *prevPtr;
    ThreadSpecificData *tsdPtr;

    if (tclNotifierHooks.deleteFileHandlerProc) {
	tclNotifierHooks.deleteFileHandlerProc(fd);
	return;
    }
    tsdPtr = TCL_TSD_INIT(&dataKey);

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
     * Remove from the array of pollfd structures, and patch up the array of
     * descriptors too if necessary.
     */

    tsdPtr->pollInfo.nfds--;
    if (filePtr->pollIndex != tsdPtr->pollInfo.nfds) {
	register FileHandler *file2Ptr;

	tsdPtr->pollInfo.fds[filePtr->pollIndex] =
		tsdPtr->pollInfo.fds[tsdPtr->pollInfo.nfds];
	for (file2Ptr = tsdPtr->firstFileHandlerPtr ;;
		file2Ptr = file2Ptr->nextPtr) {
	    if (file2Ptr->pollIndex == tsdPtr->pollInfo.nfds) {
		file2Ptr->pollIndex = filePtr->pollIndex;
		break;
	    }
	}
    }

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
    FileHandler *filePtr;
    FileHandlerEvent *fileEvPtr;
    int mask;
    Tcl_Time vTime;
#ifdef TCL_THREADS
    int waitForFiles;
#else
    /*
     * Impl. notes: timeout & timeoutPtr are used if, and only if threads are
     * not enabled. They are the arguments for the regular select() used when
     * the core is not thread-enabled.
     */

    struct timeval timeout, *timeoutPtr;
    int numFound;
#endif /* TCL_THREADS */
    ThreadSpecificData *tsdPtr;

    if (tclNotifierHooks.waitForEventProc) {
	return tclNotifierHooks.waitForEventProc(timePtr);
    }
    tsdPtr = TCL_TSD_INIT(&dataKey);

    /*
     * Set up the timeout structure. Note that if there are no events to check
     * for, we return with a negative result rather than blocking forever.
     */

    if (timePtr != NULL) {
	/*
	 * TIP #233 (Virtualized Time). Is virtual time in effect? And do we
	 * actually have something to scale? If yes to both then we call the
	 * handler to do this scaling.
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
    } else if (tsdPtr->pollInfo.nfds == 0) {
	/*
	 * If there are no threads, no timeout, and no fds registered, then
	 * there are no events possible and we must avoid deadlock. Note that
	 * this is not entirely correct because there might be a signal that
	 * could interrupt the select call, but we don't handle that case if
	 * we aren't using threads.
	 */

	return -1;
    } else {
	timeoutPtr = NULL;
#endif /* TCL_THREADS */
    }

#ifdef TCL_THREADS
    /*
     * Place this thread on the list of interested threads, signal the
     * notifier thread, and wait for a response or a timeout.
     */

    Tcl_MutexLock(&notifierMutex);

    if (timePtr != NULL && timePtr->sec == 0 && (timePtr->usec == 0
#if defined(__APPLE__) && defined(__LP64__)
	    /*
	     * On 64-bit Darwin, pthread_cond_timedwait() appears to have a
	     * bug that causes it to wait forever when passed an absolute time
	     * which has already been exceeded by the system time; as a
	     * workaround, when given a very brief timeout, just do a poll.
	     * [Bug 1457797]
	     */
	    || timePtr->usec < 10
#endif /* __APPLE__ && __LP64__ */
	    )) {
	/*
	 * Cannot emulate a polling select with a polling condition variable.
	 * Instead, pretend to wait for files and tell the notifier thread
	 * what we are doing. The notifier thread makes sure it goes through
	 * select with its select mask in the same state as ours currently is.
	 * We block until that happens.
	 */

	waitForFiles = 1;
	tsdPtr->pollState = POLL_WANT;
	timePtr = NULL;
    } else {
	waitForFiles = (tsdPtr->pollInfo.nfds > 0);
	tsdPtr->pollState = 0;
    }

    if (waitForFiles) {
	/*
	 * Add the ThreadSpecificData structure of this thread to the list of
	 * ThreadSpecificData structures of all threads that are waiting on
	 * file events.
	 */

	tsdPtr->nextPtr = waitingListPtr;
	if (waitingListPtr) {
	    waitingListPtr->prevPtr = tsdPtr;
	}
	tsdPtr->prevPtr = 0;
	waitingListPtr = tsdPtr;
	tsdPtr->onList = 1;

	write(triggerPipe, "", 1);
    }

    if (!tsdPtr->eventReady) {
	Tcl_ConditionWait(&tsdPtr->waitCV, &notifierMutex, timePtr);
    }
    tsdPtr->eventReady = 0;

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
	write(triggerPipe, "", 1);
    }

#else /* !TCL_THREADS */
    if (timeoutPtr) {
	numFound = poll(tsdPtr->pollInfo.fds, tsdPtr->pollInfo.nfds,
		1000*timeoutPtr->tv_sec + timeoutPtr->tv_usec/1000);
    } else {
	numFound = poll(tsdPtr->pollInfo.fds, tsdPtr->pollInfo.nfds, -1);
    }
#endif /* TCL_THREADS */

    /*
     * Queue all detected file events before returning.
     */

    for (filePtr = tsdPtr->firstFileHandlerPtr; (filePtr != NULL);
	    filePtr = filePtr->nextPtr) {
	register struct pollfd *pollPtr =
		&tsdPtr->pollInfo.fds[filePtr->pollIndex];

	mask = 0;
	if (pollPtr->revents & POLLIN) {
	    mask |= TCL_READABLE;
	}
	if (pollPtr->revents & POLLOUT) {
	    mask |= TCL_WRITABLE;
	}
	if (pollPtr->revents & (POLLERR | POLLHUP)) {
	    mask |= TCL_EXCEPTION;
	}

	if (!mask) {
	    continue;
	}

	/*
	 * Don't bother to queue an event if the mask was previously non-zero
	 * since an event must still be on the queue.
	 */

	if (filePtr->readyMask == 0) {
	    fileEvPtr = ckalloc(sizeof(FileHandlerEvent));
	    fileEvPtr->header.proc = FileHandlerEventProc;
	    fileEvPtr->fd = filePtr->fd;
	    Tcl_QueueEvent((Tcl_Event *) fileEvPtr, TCL_QUEUE_TAIL);
	}
	filePtr->readyMask = mask;
    }
#ifdef TCL_THREADS
    Tcl_MutexUnlock(&notifierMutex);
#endif /* TCL_THREADS */
    return 0;
}

#ifdef TCL_THREADS
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
    int fds[2];
    int i, j, receivePipe, bound;
    long found;
    char buf[2];
    PollData pollData;
    struct pollfd *ptr, *ptr2;

    memset(&pollData, 0, sizeof(PollData));
    pollData.fds = pollData.defaultFds;
    pollData.maxNfds = DEFAULT_POLL_FDS_SIZE;

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

    Tcl_MutexLock(&notifierMutex);
    triggerPipe = fds[1];

    /*
     * Signal any threads that are waiting.
     */

    Tcl_ConditionNotify(&notifierCV);
    Tcl_MutexUnlock(&notifierMutex);

    /*
     * Look for file events and report them to interested threads.
     */

    while (1) {
	/*
	 * Compute the logical OR of the select masks from all the waiting
	 * notifiers.
	 */

	Tcl_MutexLock(&notifierMutex);
	bound = 1;
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    bound += tsdPtr->pollInfo.nfds;
	}
	if (bound > pollData.maxNfds) {
	    if (pollData.fds != pollData.defaultFds) {
		ckfree(pollData.fds);
	    }
	    pollData.fds = ckalloc(bound * sizeof(struct pollfd));
	    pollData.maxNfds = bound;
	}
	ptr = pollData.fds;
	ptr->fd = receivePipe;
	ptr->events = POLLIN;
	ptr++;
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    for (i=0 ; i<tsdPtr->pollInfo.nfds ; i++) {
		ptr2 = tsdPtr->pollInfo.fds;
		for (j=0 ; pollData.fds+j<ptr ; j++) {
		    if (pollData.fds[j].fd == ptr2->fd) {
			pollData.fds[j].events |= ptr2->events;
			goto nextFD;
		    }
		}
		memcpy(ptr++, ptr2, sizeof(struct pollfd));
	    nextFD:
		(void) 0;
	    }
	}
	Tcl_MutexUnlock(&notifierMutex);

	pollData.nfds = ptr - pollData.fds;
	qsort(pollData.fds, pollData.nfds, sizeof(struct pollfd),
		ComparePollFDStructures);

#if 0
	if (tsdPtr->pollState & POLL_WANT) {
	    /*
	     * Here we make sure we go through select() with the same mask
	     * bits that were present when the thread tried to poll.
	     */

	    tsdPtr->pollState |= POLL_DONE;
	}
#endif

	do {
	    for (i=0 ; i<pollData.nfds ; i++) {
		pollData.fds[i].revents = 0;
	    }
	    j = poll(pollData.fds, pollData.nfds, -1);

	    /*
	     * Try again immediately on a recoverable error.
	     */
	} while ((j == -1) && (errno == EAGAIN || errno == EINTR));

	/*
	 * Alert any threads that are waiting on a ready file descriptor.
	 */

	Tcl_MutexLock(&notifierMutex);
	for (tsdPtr = waitingListPtr; tsdPtr; tsdPtr = tsdPtr->nextPtr) {
	    found = 0;
	    for (i=j=0 ; i<tsdPtr->pollInfo.nfds ; i++) {
		if (j >= pollData.nfds) {
		    break;
		}
		while (tsdPtr->pollInfo.fds[i].fd > pollData.fds[j].fd
			&& j < pollData.nfds) {
		    j++;
		}
		if (j >= pollData.nfds) {
		    break;
		}
		if (tsdPtr->pollInfo.fds[i].fd == pollData.fds[j].fd) {
		    tsdPtr->pollInfo.fds[i].revents =
			    tsdPtr->pollInfo.fds[i].events
			    & pollData.fds[j].revents;
		    found = 1;
		}
	    }

	    if (found || (tsdPtr->pollState & POLL_DONE)) {
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
		Tcl_ConditionNotify(&tsdPtr->waitCV);
	    }
	}
	Tcl_MutexUnlock(&notifierMutex);

	/*
	 * Consume the next byte from the notifier pipe if the pipe was
	 * readable. Note that there may be multiple bytes pending, but to
	 * avoid a race condition we only read one at a time.
	 */

	for (i=0 ; i<pollData.nfds ; i++) {
	    if ((pollData.fds[i].fd == receivePipe)
		    && (pollData.fds[i].revents & POLLIN)) {
		i = read(receivePipe, buf, 1);

		if ((i == 0) || ((i == 1) && (buf[0] == 'q'))) {
		    /*
		     * Someone closed the write end of the pipe or sent us a
		     * Quit message [Bug: 4139] and then closed the write end
		     * of the pipe so we need to shut down the notifier
		     * thread.
		     */

		    goto cleanup;
		}
	    }
	}
    }

    /*
     * Clean up the read end of the pipe and signal any threads waiting on
     * termination of the notifier thread.
     */

  cleanup:
    close(receivePipe);
    Tcl_MutexLock(&notifierMutex);
    triggerPipe = -1;
    Tcl_ConditionNotify(&notifierCV);
    Tcl_MutexUnlock(&notifierMutex);

    TclpThreadExit(0);
}

static int
ComparePollFDStructures(
    const void *a,
    const void *b)
{
    return ((struct pollfd *)a)->fd - ((struct pollfd *)b)->fd;
}
#endif /* TCL_THREADS */

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * fill-column: 78
 * End:
 */
