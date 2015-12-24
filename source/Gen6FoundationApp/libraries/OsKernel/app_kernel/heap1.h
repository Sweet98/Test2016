/*
 * heap1.h
 * Copyright: 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * $RCSfile: app_kernel/heap1.h $
 * $Revision: 1.8.1.1 $
 * $Date: 2009/05/19 17:29:48EDT $
 */

 /* Code for a type (1) large block heaps
This provides functions which are common across all type (1) heaps, and should
be #included into Heap1X.c at the end.
These should be defined before inclusion:
Heap1_FreeBlock
Heap1_Descriptor
Heap1_ProvideMemory
Heap1_SizeAdjust
Heap1_Alloc
Heap1_Free
Heap1_Realloc
Heap1_Stats

The Heap1 algorithms have in common:
* The heap fragment structure:
        Alloced                 Free
        size                    size
        <user>                  link
                                        <unused>
* Free blocks maintained in an address-ordered singly linked list.
* Blocks are merged into the free list when freed (contiguous blocks
        are merged at this stage).
* The provided blocks are not maintained in a structure of their own -
        their memory is added into the heap by being freed.
* Provided blocks must be:
        A multiple of 4 bytes big
        At least 8 bytes big
        Not starting at address 0, not ending at address 0 and not containing address 0

*/

#ifndef HEAP1_H
#define HEAP1_H

#include "thread.h"
#include "interrupts.h"
#include <string.h>


extern MT_Mutex_t stackmutx;


int LockHeap(void)
{
#ifndef NO_THREADS
	if (!stackmutx)
		return 1;
	return MT_WaitMutex(stackmutx,INFINITE);
#else
	return 1;
#endif
}
void UnLockHeap(void)
{
#ifndef NO_THREADS
	MT_ReleaseMutex(stackmutx);
#endif
}
/* <Heap1_ProvideMemory>
 * Provide memory to the heap
 * 02-06-1997 JSR Written
 */
extern void Heap1_InitMemory(Heap1_Descriptor *h, void *_blk, size_t size)
{
    Heap1_FreeBlock *blk = (Heap1_FreeBlock *)_blk;
		DISABLE_INTERRUPTS();
    /* Turn block into a Heap1 block and free it */
    blk->size = size;
    h->freechain.next = blk;
    blk->next = NULL;
    RESTORE_INTERRUPTS();
}


/* <Heap1_ProvideMemory>
 * Provide memory to the heap
 * 02-06-1997 JSR Written
 */
extern void Heap1_ProvideMemory(Heap1_Descriptor *h, void *_blk, size_t size)
{
    Heap1_FreeBlock *blk = (Heap1_FreeBlock *)_blk;

    /* Turn block into a Heap1 block and free it */
    blk->size = size;
    Heap1_Free(h, &blk->next);
}


/* <Heap1_SizeAdjust>
 * Adjust the block size to be the whole size of of the required allocation block
 * 03-06-1997 JSR Written
 */
static inline size_t Heap1_SizeAdjust(size_t size)
{
    /* Add allocated block overhead and round up to a multiple of 4 bytes */
    size = (size + 2*(sizeof(size_t)) + 7) & ~7;
    if (size < sizeof(Heap1_FreeBlock))
    {
        size = sizeof(Heap1_FreeBlock);
    }

    return size;
}


/* <Heap1_Free>
 * Free some memory back to the heap
 * 03-06-1997 JSR Written
 */
extern void Heap1_Free(Heap1_Descriptor *h, void *_blk)
{
	
    Heap1_FreeBlock *blk = (Heap1_FreeBlock *)((char *)_blk - 2*sizeof(size_t));
    Heap1_FreeBlock *prev;
    Heap1_FreeBlock *rover;
	if (!LockHeap())
    	  	return ;		

    /* Find the blocks before and after blk */
    for (prev = &h->freechain, rover = prev->next;
         rover != NULL && rover < blk;
         prev = rover, rover = prev->next)
        continue;

    /* Check for merging with the block before blk */
    if ((char *)prev + prev->size == (char *)blk)
    {   /* Merge blk into prev */
        prev->size += blk->size;
        blk = prev;
    }
    else
    {   /* Point prev at blk */
        prev->next = blk;
    }

    /* Check for merging with the block after blk */
    if ((char *)blk + blk->size == (char *)rover)
    {   /* Merge rover into blk */

        /* Check if front of lastprev is being merged into rover */
        Heap1_LastPrevCheck(h, rover, blk)

        blk->next = rover->next;
        blk->size += rover->size;
    }
    else
    {
        /* Chain rover after blk */
        blk->next = rover;
    }


#ifdef	HEAP_DEBUG
	int		i = 0;
	while(( h->HEAP1A_Allocation[ i ].AllocatedAddress != _blk ) && ( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES ))
	{
		++i;
	}
	if( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES )
	{
		h->HEAP1A_Allocation[ i ].Size = 0;
	}
 #endif
 UnLockHeap();
}


/* <Heap1_Realloc>
 * Re-allocate the given block, ensuring the newly allocated block has the same
 * min(newsize,oldsize) bytes at its start
 * 03-06-1997 JSR Written
 */
void *Heap1_Realloc(Heap1_Descriptor *h, void *_blk, size_t size)
{
	void * Ret;
 	 if (!LockHeap())
    	  	return NULL;		


    Heap1_FreeBlock *blk = (Heap1_FreeBlock *)((char *)_blk - 2*sizeof(size_t));
    Heap1_FreeBlock *prev;
    Heap1_FreeBlock *rover;
    Heap1_FreeBlock *fragment;

    size = Heap1_SizeAdjust(size);

    if (size <= blk->size)
    {
        /* No change or shrink */
        if (blk->size - size >= sizeof(Heap1_FreeBlock))
        {
            /* Split */
            fragment = (Heap1_FreeBlock *)((char *)blk + size);
            fragment->size = blk->size - size;
            blk->size = size;
            Heap1_Free(h, &fragment->next);
        }
        Ret =&blk->next;
        UnLockHeap();
        return Ret;
    }

    /* Enlarge */

    /* Find the blocks before and after blk */
    for (prev = &h->freechain, rover = prev->next;
         rover != NULL && rover < blk;
         prev = rover, rover = prev->next)
        continue;

    if ((char *)blk + blk->size == (char *)rover && blk->size + rover->size >= size)
    {
        /* A free block of sufficient size follows blk */

        if (blk->size + rover->size - size >= sizeof(Heap1_FreeBlock))
        {
            /* Split */
            fragment = (Heap1_FreeBlock *)((char *)blk + size);
            fragment->next = rover->next;
            fragment->size = blk->size + rover->size - size;
            blk->size = size;
            prev->next = fragment;
        }
        else
        {
            /* Engulf */
            blk->size += rover->size;
            prev->next = rover->next;
        }
        Heap1_PrevSet(h,prev)
        Ret =&blk->next;
        UnLockHeap();
        return Ret;
    }

    /* No free block of sufficient size follows blk, so do an alloc,copy,free sequence */
    rover = (Heap1_FreeBlock *)Heap1_Alloc(h, size - 2*(sizeof(size_t)));
    if (rover != NULL)
    {
        memcpy(rover, &blk->next, blk->size - 2*sizeof(size_t));
        Heap1_Free(h, &blk->next);
    }
    UnLockHeap();
    return rover;
}

  
/* <Heap1_Stats>
 * Print stats on the given heap
 * 03-06-1997 JSR Written
 */
void *Heap1_Stats(int (*dprint)(char const *format, ...), Heap1_Descriptor *h)
{
    Heap1_FreeBlock *rover;
    int sizecount[32];
    int i;
    size_t size;
    char *hwm = NULL;
    size_t totsize = 0;
    int numblocks = 0;

    for (i = 0; i < 32; i++)
    {
        sizecount[i] = 0;
    }

    for (rover = h->freechain.next; rover != NULL; rover = rover->next)
    {
        if ((char *)rover > hwm)
        {
            hwm = (char *)rover;
        }
        numblocks += 1;
        size = rover->size;
        totsize += size;
        for (i = 0; i < 32; i++)
        {
            size >>= 1;
            if (size == 0)
            {
				sizecount[i]++;
                break;
            }
        }
    }

    dprint("%d bytes in %d free blocks (avge size %d)\n", totsize, numblocks, totsize/(numblocks==0?1:numblocks));
    for (i = 0; i < 32; i++)
    {
        if (sizecount[i] != 0)
        {
            dprint("%d blocks 2^%d+1 to 2^%d\n", sizecount[i], i-1, i);
        }
    }

    return hwm;
}

/* End of file heap1.h */

#endif // HEAP1_H


