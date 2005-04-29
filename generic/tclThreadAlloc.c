/*
 * tclThreadAlloc.c --
 *
 *	This is a very fast storage allocator for used with threads (designed
 *	avoid lock contention).  The basic strategy is to allocate memory in
 *	fixed size blocks from block caches.
 *
 * The Initial Developer of the Original Code is America Online, Inc.
 * Portions created by AOL are Copyright (C) 1999 America Online, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * RCS: @(#) $Id: tclThreadAlloc.c,v 1.6.2.6 2005/04/29 22:40:34 dgp Exp $
 */

#include "tclInt.h"
#if defined(TCL_THREADS) && defined(USE_THREAD_ALLOC)

/*
 * If range checking is enabled, an additional byte will be allocated
 * to store the magic number at the end of the requested memory.
 */

#ifndef RCHECK
#ifdef  NDEBUG
#define RCHECK		0
#else
#define RCHECK		1
#endif
#endif

/*
 * The following define the number of Tcl_Obj's to allocate/move
 * at a time and the high water mark to prune a per-thread cache.
 * On a 32 bit system, sizeof(Tcl_Obj) = 24 so 800 * 24 = ~16k.
 */

#define NOBJALLOC	 800
#define NOBJHIGH	1200

/*
 * The following defines the number of buckets in the bucket
 * cache and those block sizes from (1<<4) to (1<<(3+NBUCKETS))
 */

#define NBUCKETS	  11
#define MAXALLOC	  16284

/*
 * The following union stores accounting information for
 * each block including two small magic numbers and
 * a bucket number when in use or a next pointer when
 * free.  The original requested size (not including
 * the Block overhead) is also maintained.
 */

typedef struct Block {
    union {
	struct Block *next;		/* Next in free list. */
	struct {
	    unsigned char magic1;	/* First magic number. */
	    unsigned char bucket;	/* Bucket block allocated from. */
	    unsigned char unused;	/* Padding. */
	    unsigned char magic2;	/* Second magic number. */
	} s;
    } u;
    size_t reqSize;			/* Requested allocation size. */
} Block;
#define nextBlock	u.next
#define sourceBucket	u.s.bucket
#define magicNum1	u.s.magic1
#define magicNum2	u.s.magic2
#define MAGIC		0xEF

/*
 * The following structure defines a bucket of blocks with
 * various accounting and statistics information.
 */

typedef struct Bucket {
    Block *firstPtr;			/* First block available */
    int numFree;			/* Number of blocks available */

    /* All fields below for accounting only */

    int numRemoves;			/* Number of removes from bucket */
    int numInserts;			/* Number of inserts into bucket */
    int numWaits;			/* Number of waits to acquire a lock */
    int numLocks;			/* Number of locks acquired */
    int totalAssigned;			/* Total space assigned to bucket */
} Bucket;

/*
 * The following structure defines a cache of buckets and objs, of
 * which there will be (at most) one per thread.
 */

typedef struct Cache {
    struct Cache *nextPtr;		/* Linked list of cache entries */
    Tcl_ThreadId owner;			/* Which thread's cache is this? */
    Tcl_Obj *firstObjPtr;		/* List of free objects for thread */
    int numObjects;			/* Number of objects for thread */
    int totalAssigned;			/* Total space assigned to thread */
    Bucket buckets[NBUCKETS];		/* The buckets for this thread */
} Cache;

/*
 * The following array specifies various per-bucket limits and locks.
 * The values are statically initialized to avoid calculating them
 * repeatedly.
 */

static struct {
    size_t blockSize;			/* Bucket blocksize. */
    int maxBlocks;			/* Max blocks before move to share. */
    int numMove;			/* Num blocks to move to share. */
    Tcl_Mutex *lockPtr;			/* Share bucket lock. */
} bucketInfo[NBUCKETS] = {
    {   16, 1024, 512, NULL},
    {   32,  512, 256, NULL},
    {   64,  256, 128, NULL},
    {  128,  128,  64, NULL},
    {  256,   64,  32, NULL},
    {  512,   32,  16, NULL},
    { 1024,   16,   8, NULL},
    { 2048,    8,   4, NULL},
    { 4096,    4,   2, NULL},
    { 8192,    2,   1, NULL},
    {16284,    1,   1, NULL},
};

/*
 * Static functions defined in this file.
 */

static void	LockBucket _ANSI_ARGS_((Cache *cachePtr, int bucket));
static void	UnlockBucket _ANSI_ARGS_((Cache *cachePtr, int bucket));
static void	PutBlocks _ANSI_ARGS_((Cache *cachePtr, int bucket,
		    int numMove));
static int	GetBlocks _ANSI_ARGS_((Cache *cachePtr, int bucket));
static Block *	Ptr2Block _ANSI_ARGS_((char *ptr));
static char *	Block2Ptr _ANSI_ARGS_((Block *blockPtr, int bucket,
		    unsigned int reqSize));
static void	MoveObjs _ANSI_ARGS_((Cache *fromPtr, Cache *toPtr,
		    int numMove));

/*
 * Local variables defined in this file and initialized at
 * startup.
 */

static Tcl_Mutex *listLockPtr;
static Tcl_Mutex *objLockPtr;
static Cache sharedCache;
static Cache *sharedPtr = &sharedCache;
static Cache *firstCachePtr = &sharedCache;

/*
 *----------------------------------------------------------------------
 *
 * GetCache ---
 *
 *	Gets per-thread memory cache, allocating it if necessary.
 *
 * Results:
 *	Pointer to cache.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static Cache *
GetCache(void)
{
    Cache *cachePtr;

    /*
     * Check for first-time initialization.
     */

    if (listLockPtr == NULL) {
	Tcl_Mutex *initLockPtr;
	int i;

	initLockPtr = Tcl_GetAllocMutex();
	Tcl_MutexLock(initLockPtr);
	if (listLockPtr == NULL) {
	    listLockPtr = TclpNewAllocMutex();
	    objLockPtr = TclpNewAllocMutex();
	    for (i = 0; i < NBUCKETS; ++i) {
		bucketInfo[i].lockPtr = TclpNewAllocMutex();
	    }
	}
	Tcl_MutexUnlock(initLockPtr);
    }

    /*
     * Get this thread's cache, allocating if necessary.
     */

    cachePtr = TclpGetAllocCache();
    if (cachePtr == NULL) {
	cachePtr = calloc(1, sizeof(Cache));
	if (cachePtr == NULL) {
	    Tcl_Panic("alloc: could not allocate new cache");
	}
	Tcl_MutexLock(listLockPtr);
	cachePtr->nextPtr = firstCachePtr;
	firstCachePtr = cachePtr;
	Tcl_MutexUnlock(listLockPtr);
	cachePtr->owner = Tcl_GetCurrentThread();
	TclpSetAllocCache(cachePtr);
    }
    return cachePtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TclFreeAllocCache --
 *
 *	Flush and delete a cache, removing from list of caches.
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
TclFreeAllocCache(arg)
    void *arg;
{
    Cache *cachePtr = arg;
    Cache **nextPtrPtr;
    register int bucket;

    /*
     * Flush blocks.
     */

    for (bucket = 0; bucket < NBUCKETS; ++bucket) {
	if (cachePtr->buckets[bucket].numFree > 0) {
	    PutBlocks(cachePtr, bucket, cachePtr->buckets[bucket].numFree);
	}
    }

    /*
     * Flush objs.
     */

    if (cachePtr->numObjects > 0) {
	Tcl_MutexLock(objLockPtr);
	MoveObjs(cachePtr, sharedPtr, cachePtr->numObjects);
	Tcl_MutexUnlock(objLockPtr);
    }

    /*
     * Remove from pool list.
     */

    Tcl_MutexLock(listLockPtr);
    nextPtrPtr = &firstCachePtr;
    while (*nextPtrPtr != cachePtr) {
	nextPtrPtr = &(*nextPtrPtr)->nextPtr;
    }
    *nextPtrPtr = cachePtr->nextPtr;
    cachePtr->nextPtr = NULL;
    Tcl_MutexUnlock(listLockPtr);
    free(cachePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * TclpAlloc --
 *
 *	Allocate memory.
 *
 * Results:
 *	Pointer to memory just beyond Block pointer.
 *
 * Side effects:
 *	May allocate more blocks for a bucket.
 *
 *----------------------------------------------------------------------
 */

char *
TclpAlloc(reqSize)
    unsigned int reqSize;
{
    Cache *cachePtr = TclpGetAllocCache();
    Block *blockPtr;
    register int bucket;
    size_t size;

    if (cachePtr == NULL) {
	cachePtr = GetCache();
    }

    /*
     * Increment the requested size to include room for
     * the Block structure.  Call malloc() directly if the
     * required amount is greater than the largest block,
     * otherwise pop the smallest block large enough,
     * allocating more blocks if necessary.
     */

    blockPtr = NULL;
    size = reqSize + sizeof(Block);
#if RCHECK
    ++size;
#endif
    if (size > MAXALLOC) {
	bucket = NBUCKETS;
	blockPtr = malloc(size);
	if (blockPtr != NULL) {
	    cachePtr->totalAssigned += reqSize;
	}
    } else {
	bucket = 0;
	while (bucketInfo[bucket].blockSize < size) {
	    ++bucket;
	}
	if (cachePtr->buckets[bucket].numFree || GetBlocks(cachePtr, bucket)) {
	    blockPtr = cachePtr->buckets[bucket].firstPtr;
	    cachePtr->buckets[bucket].firstPtr = blockPtr->nextBlock;
	    --cachePtr->buckets[bucket].numFree;
	    ++cachePtr->buckets[bucket].numRemoves;
	    cachePtr->buckets[bucket].totalAssigned += reqSize;
	}
    }
    if (blockPtr == NULL) {
	return NULL;
    }
    return Block2Ptr(blockPtr, bucket, reqSize);
}

/*
 *----------------------------------------------------------------------
 *
 * TclpFree --
 *
 *	Return blocks to the thread block cache.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May move blocks to shared cache.
 *
 *----------------------------------------------------------------------
 */

void
TclpFree(ptr)
    char *ptr;
{
    Cache *cachePtr;
    Block *blockPtr;
    int bucket;

    if (ptr == NULL) {
	return;
    }

    cachePtr = TclpGetAllocCache();
    if (cachePtr == NULL) {
	cachePtr = GetCache();
    }

    /*
     * Get the block back from the user pointer and call system free
     * directly for large blocks.  Otherwise, push the block back on
     * the bucket and move blocks to the shared cache if there are now
     * too many free.
     */

    blockPtr = Ptr2Block(ptr);
    bucket = blockPtr->sourceBucket;
    if (bucket == NBUCKETS) {
	cachePtr->totalAssigned -= blockPtr->reqSize;
	free(blockPtr);
	return;
    }
    cachePtr->buckets[bucket].totalAssigned -= blockPtr->reqSize;
    blockPtr->nextBlock = cachePtr->buckets[bucket].firstPtr;
    cachePtr->buckets[bucket].firstPtr = blockPtr;
    ++cachePtr->buckets[bucket].numFree;
    ++cachePtr->buckets[bucket].numInserts;
    if (cachePtr != sharedPtr &&
	    cachePtr->buckets[bucket].numFree > bucketInfo[bucket].maxBlocks) {
	PutBlocks(cachePtr, bucket, bucketInfo[bucket].numMove);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TclpRealloc --
 *
 *	Re-allocate memory to a larger or smaller size.
 *
 * Results:
 *	Pointer to memory just beyond Block pointer.
 *
 * Side effects:
 *	Previous memory, if any, may be freed.
 *
 *----------------------------------------------------------------------
 */

char *
TclpRealloc(ptr, reqSize)
    char *ptr;
    unsigned int reqSize;
{
    Cache *cachePtr = TclpGetAllocCache();
    Block *blockPtr;
    void *new;
    size_t size, min;
    int bucket;

    if (ptr == NULL) {
	return TclpAlloc(reqSize);
    }

    if (cachePtr == NULL) {
	cachePtr = GetCache();
    }

    /*
     * If the block is not a system block and fits in place,
     * simply return the existing pointer.  Otherwise, if the block
     * is a system block and the new size would also require a system
     * block, call realloc() directly.
     */

    blockPtr = Ptr2Block(ptr);
    size = reqSize + sizeof(Block);
#if RCHECK
    ++size;
#endif
    bucket = blockPtr->sourceBucket;
    if (bucket != NBUCKETS) {
	if (bucket > 0) {
	    min = bucketInfo[bucket-1].blockSize;
	} else {
	    min = 0;
	}
	if (size > min && size <= bucketInfo[bucket].blockSize) {
	    cachePtr->buckets[bucket].totalAssigned -= blockPtr->reqSize;
	    cachePtr->buckets[bucket].totalAssigned += reqSize;
	    return Block2Ptr(blockPtr, bucket, reqSize);
	}
    } else if (size > MAXALLOC) {
	cachePtr->totalAssigned -= blockPtr->reqSize;
	cachePtr->totalAssigned += reqSize;
	blockPtr = realloc(blockPtr, size);
	if (blockPtr == NULL) {
	    return NULL;
	}
	return Block2Ptr(blockPtr, NBUCKETS, reqSize);
    }

    /*
     * Finally, perform an expensive malloc/copy/free.
     */

    new = TclpAlloc(reqSize);
    if (new != NULL) {
	if (reqSize > blockPtr->reqSize) {
	    reqSize = blockPtr->reqSize;
	}
	memcpy(new, ptr, reqSize);
	TclpFree(ptr);
    }
    return new;
}

/*
 *----------------------------------------------------------------------
 *
 * TclThreadAllocObj --
 *
 *	Allocate a Tcl_Obj from the per-thread cache.
 *
 * Results:
 *	Pointer to uninitialized Tcl_Obj.
 *
 * Side effects:
 *	May move Tcl_Obj's from shared cached or allocate new Tcl_Obj's
 *	if list is empty.
 *
 *----------------------------------------------------------------------
 */

Tcl_Obj *
TclThreadAllocObj(void)
{
    register Cache *cachePtr = TclpGetAllocCache();
    register int numMove;
    register Tcl_Obj *objPtr;
    Tcl_Obj *newObjsPtr;

    if (cachePtr == NULL) {
	cachePtr = GetCache();
    }

    /*
     * Get this thread's obj list structure and move
     * or allocate new objs if necessary.
     */

    if (cachePtr->numObjects == 0) {
	Tcl_MutexLock(objLockPtr);
	numMove = sharedPtr->numObjects;
	if (numMove > 0) {
	    if (numMove > NOBJALLOC) {
		numMove = NOBJALLOC;
	    }
	    MoveObjs(sharedPtr, cachePtr, numMove);
	}
	Tcl_MutexUnlock(objLockPtr);
	if (cachePtr->numObjects == 0) {
	    cachePtr->numObjects = numMove = NOBJALLOC;
	    newObjsPtr = malloc(sizeof(Tcl_Obj) * numMove);
	    if (newObjsPtr == NULL) {
		Tcl_Panic("alloc: could not allocate %d new objects", numMove);
	    }
	    while (--numMove >= 0) {
		objPtr = &newObjsPtr[numMove];
		objPtr->internalRep.otherValuePtr = cachePtr->firstObjPtr;
		cachePtr->firstObjPtr = objPtr;
	    }
	}
    }

    /*
     * Pop the first object.
     */

    objPtr = cachePtr->firstObjPtr;
    cachePtr->firstObjPtr = objPtr->internalRep.otherValuePtr;
    --cachePtr->numObjects;
    return objPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * TclThreadFreeObj --
 *
 *	Return a free Tcl_Obj to the per-thread cache.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	May move free Tcl_Obj's to shared list upon hitting high
 *	water mark.
 *
 *----------------------------------------------------------------------
 */

void
TclThreadFreeObj(objPtr)
    Tcl_Obj *objPtr;
{
    Cache *cachePtr = TclpGetAllocCache();

    if (cachePtr == NULL) {
	cachePtr = GetCache();
    }

    /*
     * Get this thread's list and push on the free Tcl_Obj.
     */

    objPtr->internalRep.otherValuePtr = cachePtr->firstObjPtr;
    cachePtr->firstObjPtr = objPtr;
    ++cachePtr->numObjects;

    /*
     * If the number of free objects has exceeded the high
     * water mark, move some blocks to the shared list.
     */

    if (cachePtr->numObjects > NOBJHIGH) {
	Tcl_MutexLock(objLockPtr);
	MoveObjs(cachePtr, sharedPtr, NOBJALLOC);
	Tcl_MutexUnlock(objLockPtr);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_GetMemoryInfo --
 *
 *	Return a list-of-lists of memory stats.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	List appended to given dstring.
 *
 *----------------------------------------------------------------------
 */

void
Tcl_GetMemoryInfo(dsPtr)
    Tcl_DString *dsPtr;
{
    Cache *cachePtr;
    char buf[200];
    int n;

    Tcl_MutexLock(listLockPtr);
    cachePtr = firstCachePtr;
    while (cachePtr != NULL) {
	Tcl_DStringStartSublist(dsPtr);
	if (cachePtr == sharedPtr) {
	    Tcl_DStringAppendElement(dsPtr, "shared");
	} else {
	    sprintf(buf, "thread%p", cachePtr->owner);
	    Tcl_DStringAppendElement(dsPtr, buf);
	}
	for (n = 0; n < NBUCKETS; ++n) {
	    sprintf(buf, "%d %d %d %d %d %d %d",
		    (int) bucketInfo[n].blockSize,
		    cachePtr->buckets[n].numFree,
		    cachePtr->buckets[n].numRemoves,
		    cachePtr->buckets[n].numInserts,
		    cachePtr->buckets[n].totalAssigned,
		    cachePtr->buckets[n].numLocks,
		    cachePtr->buckets[n].numWaits);
	    Tcl_DStringAppendElement(dsPtr, buf);
	}
	Tcl_DStringEndSublist(dsPtr);
	cachePtr = cachePtr->nextPtr;
    }
    Tcl_MutexUnlock(listLockPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * MoveObjs --
 *
 *	Move Tcl_Obj's between caches.
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
MoveObjs(fromPtr, toPtr, numMove)
    Cache *fromPtr, *toPtr;
    int numMove;
{
    register Tcl_Obj *objPtr = fromPtr->firstObjPtr;
    Tcl_Obj *fromFirstObjPtr = objPtr;

    toPtr->numObjects += numMove;
    fromPtr->numObjects -= numMove;

    /*
     * Find the last object to be moved; set the next one
     * (the first one not to be moved) as the first object
     * in the 'from' cache.
     */

    while (--numMove) {
	objPtr = objPtr->internalRep.otherValuePtr;
    }
    fromPtr->firstObjPtr = objPtr->internalRep.otherValuePtr;

    /*
     * Move all objects as a block - they are already linked to
     * each other, we just have to update the first and last.
     */

    objPtr->internalRep.otherValuePtr = toPtr->firstObjPtr;
    toPtr->firstObjPtr = fromFirstObjPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * Block2Ptr, Ptr2Block --
 *
 *	Convert between internal blocks and user pointers.
 *
 * Results:
 *	User pointer or internal block.
 *
 * Side effects:
 *	Invalid blocks will abort the server.
 *
 *----------------------------------------------------------------------
 */

static char *
Block2Ptr(blockPtr, bucket, reqSize)
    Block *blockPtr;
    int bucket;
    unsigned int reqSize;
{
    register void *ptr;

    blockPtr->magicNum1 = blockPtr->magicNum2 = MAGIC;
    blockPtr->sourceBucket = bucket;
    blockPtr->reqSize = reqSize;
    ptr = ((void *) (blockPtr + 1));
#if RCHECK
    ((unsigned char *)(ptr))[reqSize] = MAGIC;
#endif
    return (char *) ptr;
}

static Block *
Ptr2Block(ptr)
    char *ptr;
{
    register Block *blockPtr;

    blockPtr = (((Block *) ptr) - 1);
    if (blockPtr->magicNum1 != MAGIC || blockPtr->magicNum2 != MAGIC) {
	Tcl_Panic("alloc: invalid block: %p: %x %x\n",
		blockPtr, blockPtr->magicNum1, blockPtr->magicNum2);
    }
#if RCHECK
    if (((unsigned char *) ptr)[blockPtr->reqSize] != MAGIC) {
	Tcl_Panic("alloc: invalid block: %p: %x %x %x\n",
		blockPtr, blockPtr->magicNum1, blockPtr->magicNum2,
		((unsigned char *) ptr)[blockPtr->reqSize]);
    }
#endif
    return blockPtr;
}

/*
 *----------------------------------------------------------------------
 *
 * LockBucket, UnlockBucket --
 *
 *	Set/unset the lock to access a bucket in the shared cache.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Lock activity and contention are monitored globally and on
 *	a per-cache basis.
 *
 *----------------------------------------------------------------------
 */

static void
LockBucket(cachePtr, bucket)
    Cache *cachePtr;
    int bucket;
{
#if 0
    if (Tcl_MutexTryLock(bucketInfo[bucket].lockPtr) != TCL_OK) {
	Tcl_MutexLock(bucketInfo[bucket].lockPtr);
	++cachePtr->buckets[bucket].numWaits;
	++sharedPtr->buckets[bucket].numWaits;
    }
#else
    Tcl_MutexLock(bucketInfo[bucket].lockPtr);
#endif
    ++cachePtr->buckets[bucket].numLocks;
    ++sharedPtr->buckets[bucket].numLocks;
}

static void
UnlockBucket(cachePtr, bucket)
    Cache *cachePtr;
    int bucket;
{
    Tcl_MutexUnlock(bucketInfo[bucket].lockPtr);
}

/*
 *----------------------------------------------------------------------
 *
 * PutBlocks --
 *
 *	Return unused blocks to the shared cache.
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
PutBlocks(cachePtr, bucket, numMove)
    Cache *cachePtr;
    int bucket, numMove;
{
    register Block *lastPtr, *firstPtr;
    register int n = numMove;

    /*
     * Before acquiring the lock, walk the block list to find
     * the last block to be moved.
     */

    firstPtr = lastPtr = cachePtr->buckets[bucket].firstPtr;
    while (--n > 0) {
	lastPtr = lastPtr->nextBlock;
    }
    cachePtr->buckets[bucket].firstPtr = lastPtr->nextBlock;
    cachePtr->buckets[bucket].numFree -= numMove;

    /*
     * Aquire the lock and place the list of blocks at the front
     * of the shared cache bucket.
     */

    LockBucket(cachePtr, bucket);
    lastPtr->nextBlock = sharedPtr->buckets[bucket].firstPtr;
    sharedPtr->buckets[bucket].firstPtr = firstPtr;
    sharedPtr->buckets[bucket].numFree += numMove;
    UnlockBucket(cachePtr, bucket);
}

/*
 *----------------------------------------------------------------------
 *
 * GetBlocks --
 *
 *	Get more blocks for a bucket.
 *
 * Results:
 *	1 if blocks where allocated, 0 otherwise.
 *
 * Side effects:
 *	Cache may be filled with available blocks.
 *
 *----------------------------------------------------------------------
 */

static int
GetBlocks(cachePtr, bucket)
    Cache *cachePtr;
    int bucket;
{
    register Block *blockPtr;
    register int n;
    register size_t size;

    /*
     * First, atttempt to move blocks from the shared cache.  Note
     * the potentially dirty read of numFree before acquiring the lock
     * which is a slight performance enhancement.  The value is
     * verified after the lock is actually acquired.
     */

    if (cachePtr != sharedPtr && sharedPtr->buckets[bucket].numFree > 0) {
	LockBucket(cachePtr, bucket);
	if (sharedPtr->buckets[bucket].numFree > 0) {

	    /*
	     * Either move the entire list or walk the list to find
	     * the last block to move.
	     */

	    n = bucketInfo[bucket].numMove;
	    if (n >= sharedPtr->buckets[bucket].numFree) {
		cachePtr->buckets[bucket].firstPtr =
			sharedPtr->buckets[bucket].firstPtr;
		cachePtr->buckets[bucket].numFree =
			sharedPtr->buckets[bucket].numFree;
		sharedPtr->buckets[bucket].firstPtr = NULL;
		sharedPtr->buckets[bucket].numFree = 0;
	    } else {
		blockPtr = sharedPtr->buckets[bucket].firstPtr;
		cachePtr->buckets[bucket].firstPtr = blockPtr;
		sharedPtr->buckets[bucket].numFree -= n;
		cachePtr->buckets[bucket].numFree = n;
		while (--n > 0) {
		    blockPtr = blockPtr->nextBlock;
		}
		sharedPtr->buckets[bucket].firstPtr = blockPtr->nextBlock;
		blockPtr->nextBlock = NULL;
	    }
	}
	UnlockBucket(cachePtr, bucket);
    }

    if (cachePtr->buckets[bucket].numFree == 0) {

	/*
	 * If no blocks could be moved from shared, first look for a
	 * larger block in this cache to split up.
	 */

	blockPtr = NULL;
	n = NBUCKETS;
	size = 0; /* lint */
	while (--n > bucket) {
	    if (cachePtr->buckets[n].numFree > 0) {
		size = bucketInfo[n].blockSize;
		blockPtr = cachePtr->buckets[n].firstPtr;
		cachePtr->buckets[n].firstPtr = blockPtr->nextBlock;
		--cachePtr->buckets[n].numFree;
		break;
	    }
	}

	/*
	 * Otherwise, allocate a big new block directly.
	 */

	if (blockPtr == NULL) {
	    size = MAXALLOC;
	    blockPtr = malloc(size);
	    if (blockPtr == NULL) {
		return 0;
	    }
	}

	/*
	 * Split the larger block into smaller blocks for this bucket.
	 */

	n = size / bucketInfo[bucket].blockSize;
	cachePtr->buckets[bucket].numFree = n;
	cachePtr->buckets[bucket].firstPtr = blockPtr;
	while (--n > 0) {
	    blockPtr->nextBlock = (Block *)
		((char *) blockPtr + bucketInfo[bucket].blockSize);
	    blockPtr = blockPtr->nextBlock;
	}
	blockPtr->nextBlock = NULL;
    }
    return 1;
}

/*
 *----------------------------------------------------------------------
 *
 * TclFinalizeThreadAlloc --
 *
 *	This procedure is used to destroy all private resources used in
 *	this file.
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
TclFinalizeThreadAlloc()
{
    int i;
    for (i = 0; i < NBUCKETS; ++i) {
        TclpFreeAllocMutex(bucketInfo[i].lockPtr); 
        bucketInfo[i].lockPtr = NULL;
    }

    TclpFreeAllocMutex(objLockPtr);
    objLockPtr = NULL;

    TclpFreeAllocMutex(listLockPtr);
    listLockPtr = NULL;

    TclpFreeAllocCache(NULL);
}

#else

/*
 *----------------------------------------------------------------------
 *
 * TclFinalizeThreadAlloc --
 *
 *	This procedure is used to destroy all private resources used in
 *	this file.
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
TclFinalizeThreadAlloc()
{
    Tcl_Panic("TclFinalizeThreadAlloc called when threaded memory allocator not in use.");
}

#endif /* TCL_THREADS */
