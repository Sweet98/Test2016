/*
 * heapalloc.c
 * Copyright: 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * $RCSfile: app_kernel/heapalloc.c $
 * $Revision: 1.4 $
 * $Date: 2006/02/24 12:02:09EST $
 */

#include <stdlib.h>

					// Necessary for heap.h to include the correct heap1a information.  The only software
					//	available for download was of heap type 1, so this is your only choice.  If this isn't
					//	defined, nothing will be included and the attempt later on to define a heap descriptor
					//	will fail.
#include "heap.h"

#define heapDescPtr __rt_heapdescriptor()
extern Heap_Descriptor *__rt_heapdescriptor(void);
//extern Heap_Descriptor *__rt_embeddedalloc_init(void *base, size_t size);
extern void __rt_embeddedalloc_init(Heap_Descriptor *, void *base, size_t size);





#ifdef	HEAP_DEBUG
void	*HEAP_LinkRegisterValue = 0;


extern void *operator new(size_t n)
{
	size_t	NewSize = n;
	__asm
	{
		mov r0,&HEAP_LinkRegisterValue
		str	lr,[r0]
	}
	
	void *p = malloc(NewSize == 0 ? 1 : n);
	
	HEAP_LinkRegisterValue = 0;
	
    return p;
}



extern void operator delete(void *p)
{
	free( p );
}
#endif



//extern Heap_Descriptor *__rt_embeddedalloc_init(void *base, size_t size)
//{
//    if (size <= sizeof(Heap_Descriptor))
//        return NULL;
//    else
//    {   Heap_Descriptor *hd = (Heap_Descriptor *)base;
//        Heap_Initialise(hd, NULL, NULL, NULL, NULL);
//        Heap_InitMemory(hd, hd+1, size - sizeof(Heap_Descriptor));
//        return hd;
//    }
//}
extern void __rt_embeddedalloc_init(Heap_Descriptor *HeapDesc, void *base, size_t size)
{
	Heap_Initialise(HeapDesc, NULL, NULL, NULL, NULL);
    Heap_InitMemory(HeapDesc, base, size);
    return;
}


/* <malloc>
 * Allocate the given number of bytes - return 0 if fails
 */
extern  "C" void *malloc(size_t size)
{
	size_t	NewSize = size;
	
#ifdef	HEAP_DEBUG
	if( HEAP_LinkRegisterValue == 0 )
	{
		__asm
		{
			mov r0,&HEAP_LinkRegisterValue
			str	lr,[r0]
		}
	}
#endif
	
    void	*p = ( NewSize != 0 ) ? Heap_Alloc(heapDescPtr, NewSize)
                     : NULL;

#ifdef	HEAP_DEBUG                     
	HEAP_LinkRegisterValue = 0;
#endif
	
    return p;
}


/* <free>
 * Free the given block of bytes
 */
extern  "C" void free(void *p)
{
    if (p != NULL)
    {
        Heap_Free(heapDescPtr, p);
    }
}


extern  "C" void *realloc(void *p, size_t size)
{
    return p == NULL ? Heap_Alloc(heapDescPtr, size)
                     : Heap_Realloc(heapDescPtr, p, size);
}


#include <string.h>

/* <calloc>
 * Alloc a zero-filled block count*size bytes big
 */
extern  "C" void *calloc(size_t count, size_t size)
{
    void *r;
    /*
     * This code computes the 64 bit product of count * size to 
     * verify that the product really is in range.
     */
    unsigned m1 = (count >> 16) * (size & 0xffff);
    unsigned m2 = (count & 0xffff) * (size >> 16);
    unsigned len = (count & 0xffff) * (size & 0xffff);

    if ((m1 | m2) >> 16)
        return NULL;
    m1 += m2;
    if (m1 + (len >> 16) >= 0x10000)
        return NULL;
    len += m1 << 16;
    
    r = malloc(len);
    if (r != NULL)
        memset(r, 0, len);
    return r;
}

extern void __heapstats(int (*dprint)(char const *format, ...))
{
    Heap_Stats(dprint, heapDescPtr);
}


/* End of file heapalloc.c */



