/************************************************************************/
//
// API for Matrix heap management / reporting routines.
//
//  Date:       3/17/04
//  Tools:      ARM ADS >= 1.1
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/db_Heap.h $
//======================================================================================


#ifndef __DB_HEAP_H
#define __DB_HEAP_H 1


#include	"language.h"



//    Add up the free space found in each free space block on the heap and report the sum to the caller.
ULONG ComputeFreeHeapSpace(void);

// Add up the free space found in each free space block on the heap and report the sum to the host.
bool ShowFreeHeapSpace(void);

// Show free heap space summary.
bool ShowFreeHeapSpaceShortVersion( void );

#ifdef	HEAP_DEBUG
// Show all used heap segments.
void ShowUsedHeapSpace( const char *SortParameter );

// Clear out the heap allocation tracking buffer.  The next call to ShowUsedHeapSpace will now only display heap segments allocated after this
// routine was called, thus allowing the user to limit the number of items that need to be sorted through.
bool ClearUsedHeapSpaceTrackingBuffer( void );
#endif


// Compute the untouched heap space by finding the first non 0xFFFFFFFF int from the end of the heap.
int ComputeUntouchedHeapSpace(void);


// Report untouched heap space to the host.
bool ShowUntouchedHeapSpace(void);


// Return the address of the heap for anyone who may wish to use it as a scratch pad.
UCHAR *HeapAddress(void);


// Create a heap.
void HHPHeapInit( BYTE *StartOfHeap, UINT SizeOfHeap );


// Add another memory block to the heap to increase its' size.
void HHP_AddMemoryToHeap( BYTE *StartOfNewHeapBlock, UINT SizeOfNewHeapBlock );



#endif      /*  __DB_HEAP_H */



