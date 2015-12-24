/*===========================================================================
 *  HHPHeap.h -- various handheld utility functions & stubs
 *===========================================================================
 * $RCSfile: app_kernel/hhpheap.h $
 * $Revision: 1.1 $
 * $Date: 2003/11/26 10:24:51EST $
 *
 *
 *===========================================================================*/
#ifndef _INC_HHPHeap
  #define _INC_HHPHeap

/* define the memory free list linked list structure */
typedef struct HHPMemList
{
 	struct HHPMemList*	pNext; 	/* pointer to next block */
 	unsigned long 		Size;   	/* size of this block */
} HHPMemList;

/* Function prototypes */
//void      HHPHeapInit(void);


HHPMemList 						glMemHead;  	/* head of memory free list */

#define	ROUND_UP(s)		((((s)+3)/4)*4)


#endif	/*	_INC_HHPHeap */



