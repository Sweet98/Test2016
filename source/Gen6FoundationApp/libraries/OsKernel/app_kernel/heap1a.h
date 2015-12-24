/*
 * heap1a.h
 * Copyright: 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * $RCSfile: app_kernel/heap1a.h $
 * $Revision: 1.4.1.2 $
 * $Date: 2009/05/19 17:33:31EDT $
 */

#ifndef HEAP1A_H
#define HEAP1A_H


/* Header for a type (1a) large block heap
This sort of heap maintains a linked-list of free blocks of memory.
Each free block has this format:
<size><link><(size-8) unused bytes (multiple of 4)>
<link>=0 indicates end of list
This list is maintained in memory address order.
*/
#ifdef _HEAP1A_H
#  error "heap1a.h has been included more than once"
#else
#  define _HEAP1A_H
#  include <stddef.h>

typedef struct Heap1a_FreeBlock
{
    size_t size;
    size_t padding;
    struct Heap1a_FreeBlock *next;
} Heap1a_FreeBlock;


#ifdef	HEAP_DEBUG
#define	NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES	4000

typedef struct {
	void	*AddressOfCaller;
	void	*AllocatedAddress;
	size_t	Size;
} HEAP1A_ALLOCATION;
#endif


typedef struct Heap1a_Descriptor
{
    int (*full)(void *, size_t);
    void *fullparam;
    void (*broken)(void *);
    void *brokenparam;

    Heap1a_FreeBlock freechain;
#ifdef	HEAP_DEBUG
	HEAP1A_ALLOCATION	HEAP1A_Allocation[ NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES ];
#endif
} Heap1a_Descriptor;

extern void __Heap1a_Initialise(Heap1a_Descriptor *h, int (*full)(void *, size_t), void *fullparam, void (*broken)(void *), void *brokenparam);
extern void __Heap1a_InitMemory(Heap1a_Descriptor *h, void *blk, size_t size);
extern void __Heap1a_ProvideMemory(Heap1a_Descriptor *h, void *blk, size_t size);
extern void *__Heap1a_Alloc(Heap1a_Descriptor *h, size_t size);
extern void __Heap1a_Free(Heap1a_Descriptor *h, void *blk);
extern void *__Heap1a_Realloc(Heap1a_Descriptor *h, void *_blk, size_t size);
extern void *__Heap1a_Stats(int (*pr)(char const *format, ...), Heap1a_Descriptor *h);
#define __Heap1a_TraceAlloc(h,s,f,l) __Heap1a_Alloc(h,s)

#endif

/* End of file heap1a.h */

#endif // HEAP1A_H


