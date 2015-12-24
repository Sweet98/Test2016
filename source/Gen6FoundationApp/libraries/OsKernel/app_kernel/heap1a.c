/*
 * heap1a.c
 * Copyright: 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * $RCSfile: app_kernel/heap1a.c $
 * $Revision: 1.6 $
 * $Date: 2006/05/04 11:12:08EDT $
 */

/* Code for a type (1a) large block heap

A Heap1a heap is a Heap1 heap which chooses the first block in the free
list which is big enough (first fit).

*/
#ifndef _HEAP1A_H
#  include "heap1a.h"
#endif

#define Heap1_Descriptor        Heap1a_Descriptor   
#define Heap1_FreeBlock         Heap1a_FreeBlock    
#define Heap1_InitMemory        __Heap1a_InitMemory
#define Heap1_ProvideMemory     __Heap1a_ProvideMemory
#define Heap1_SizeAdjust        __Heap1a_SizeAdjust   
#define Heap1_Alloc             __Heap1a_Alloc        
#define Heap1_LastPrevCheck(h,merged,alternate)
#define Heap1_Free              __Heap1a_Free         
#define Heap1_PrevSet(h,newval)
#define Heap1_Realloc           __Heap1a_Realloc      
#define Heap1_Stats             __Heap1a_Stats        

#include "heap1.h"



#ifdef	HEAP_DEBUG
extern void	*HEAP_LinkRegisterValue;
#endif


  /* <Heap1a_Initialise>
 * Initialise the heap
 * 02-06-1997 JSR Written
 */
extern void __Heap1a_Initialise(
    Heap1a_Descriptor *h,
    int (*full)(void *, size_t), void *fullparam,
    void (*broken)(void *), void *brokenparam)
{
    h->full = full;
    h->fullparam = fullparam;
    h->broken = broken;
    h->brokenparam = brokenparam;
    h->freechain.size = 0;  /* Guarantees won't merge with following block */
    h->freechain.next = 0;  /* end of list */

#ifdef	HEAP_DEBUG
	int	i;
	for( i = 0;    i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES;    ++i )
	{
		h->HEAP1A_Allocation[ i ].Size = 0;
	}
#endif
}


/* <Heap1a_Alloc>
 * Allocate some memory from the heap
 * 02-06-1997 JSR Written
 */
extern void *__Heap1a_Alloc(Heap1a_Descriptor *h, size_t size)
{
	 
	void * Ret;
    Heap1a_FreeBlock *prev;
    Heap1a_FreeBlock *rover;
    Heap1a_FreeBlock *fragment;

    size = Heap1_SizeAdjust(size);
    if (!LockHeap())
    	 return NULL;		
    /* Loop around until allocated, or full function returns 0 to indicate no
    extra memory released to heap */
    do
    {	  
        for (prev = &h->freechain, rover = prev->next;
             rover != NULL;
             prev = rover, rover = prev->next)
        {
            if (rover->size >= size)
            {
                /* Block large enough for job */
                if (rover->size >= size + sizeof(Heap1a_FreeBlock))
                {
                    /* Block large enough to split */
                    fragment = (Heap1a_FreeBlock *)((char *)rover + size);
                    fragment->next = rover->next;
                    fragment->size = rover->size - size;
                    prev->next = fragment;
                    rover->size = size;
                }
                else
                {
                    /* Block too small */
                    prev->next = rover->next;
                }

#ifdef	HEAP_DEBUG
				int		i = 0;
				while(( h->HEAP1A_Allocation[ i ].Size != 0 ) && ( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES ))
				{
					++i;
				}
				if( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES )
				{
					h->HEAP1A_Allocation[ i ].AddressOfCaller = HEAP_LinkRegisterValue;
					h->HEAP1A_Allocation[ i ].AllocatedAddress = &rover->next;
					h->HEAP1A_Allocation[ i ].Size = size;
				}
#endif
					Ret =&rover->next;
					UnLockHeap();
            	return Ret;
            }
        }
    } while (h->full != NULL && h->full(h->fullparam, size));
    UnLockHeap();
    return NULL;
}

 /* End of file heap1a.c */



