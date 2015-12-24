/************************************************************************/
//
// HHP 4480 embedded firmware
//
//  Matrix interface to ARM heap management routines
//  Date:       3/17/04
//  Tools:      ARM ADS >= 1.1
//  Target:     D-ball 4480
//
//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/db_Heap.c $
//======================================================================================



#include	"db_chrio.h"
#include	"db_Heap.h"

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

//#ifdef SIMPLE_IO_ONLY
#if 1	//FIXME_IMX53
#define SendStringToInterface(a,b,c) xputstring(a)
#else
void SendStringToInterface( const char *pucBuffer, int SizeOfBuffer, bool /* DoNotUseIndicators */ );
#endif

					//	available for download was of heap type 1, so this is your only choice.  If this isn't
					//	defined, nothing will be included and the attempt later on to define a heap descriptor
					//	will fail.
#include	"heap.h"		// ARM heap management



extern void __rt_embeddedalloc_init(Heap_Descriptor *, void *, size_t);
static Heap_Descriptor	heapdesc;

extern void Heap_ProvideMemory(Heap_Descriptor *, void *, size_t);

static BYTE		*Heap;
static UINT		HeapSize;


extern	Heap_Descriptor *__rt_heapdescriptor(void)
{
	return(&heapdesc);
}




//
// ComputeFreeHeapSpace
//    Add up the free space found in each free space block on the heap and report the sum to the caller.
//
ULONG ComputeFreeHeapSpace(void) {
   ULONG          FreeSpace;
   Heap1a_FreeBlock    *NextFreeBlock;

	FreeSpace = 0L;
   NextFreeBlock = &(heapdesc.freechain);
   while(NextFreeBlock != NULL) {
      FreeSpace += NextFreeBlock->size;
      NextFreeBlock = NextFreeBlock->next;
   }
   return FreeSpace;
}



//
// ShowFreeHeapSpace
//		Add up the free space found in each free space block on the heap and report the sum to the host.
//		Note that the return type is bool.  This makes the menu command that uses this routine happy.
//
bool ShowFreeHeapSpace(void)
{
	ULONG				NumberOfFreeBlocks;
	ULONG				FreeSpace;
    Heap1a_FreeBlock 	*NextFreeBlock;
	ULONG				BlockNumber;

	char	TempStr[80];


	sprintf(TempStr, "Heap descriptor location: %lX\015\012", ((ULONG) &heapdesc));
	SendStringToInterface(TempStr, strlen( TempStr ), true );
	sprintf(TempStr, "Heap location: %lX\015\012", ((ULONG) Heap));
	SendStringToInterface(TempStr, strlen( TempStr ), true );
	sprintf(TempStr, "Heap size: %u\015\012", HeapSize);
	SendStringToInterface(TempStr, strlen( TempStr ), true );

	FreeSpace = 0L;
	NumberOfFreeBlocks = 0L;
	NextFreeBlock = &(heapdesc.freechain);
	BlockNumber = 1;
	while(NextFreeBlock != NULL)
	{
		sprintf(TempStr, "Block %lu free space = %u at %lX\015\012", BlockNumber, NextFreeBlock->size,
																((ULONG) NextFreeBlock));
		SendStringToInterface(TempStr, strlen( TempStr ), true );
		FreeSpace += NextFreeBlock->size;
		NumberOfFreeBlocks += 1;
		NextFreeBlock = NextFreeBlock->next;
		++BlockNumber;
	}

	sprintf(TempStr, "Total free heap space: %lu    number of free blocks: %lu\015\012", FreeSpace, NumberOfFreeBlocks);
	SendStringToInterface(TempStr, strlen( TempStr ), true );

	return true;
}






//
// ShowFreeHeapSpaceShortVersion
//		Add up the free space found in each free space block on the heap and report the sum to the host
//		and report the largest free block.
//		Note that the return type is bool.  This makes the menu command that uses this routine happy.
//
bool ShowFreeHeapSpaceShortVersion(void)
{
	ULONG				NumberOfFreeBlocks;
	ULONG				FreeSpace;
    Heap1a_FreeBlock 	*NextFreeBlock;
	ULONG				LargestFreeBlock;

	char	TempStr[ 160 ];


	FreeSpace = 0L;
	NumberOfFreeBlocks = 0L;
	NextFreeBlock = &(heapdesc.freechain);
	LargestFreeBlock = 0;
	while(NextFreeBlock != NULL)
	{
		if( LargestFreeBlock < NextFreeBlock->size )
		{
			LargestFreeBlock = NextFreeBlock->size;
		}
		FreeSpace += NextFreeBlock->size;
		NumberOfFreeBlocks += 1;
		NextFreeBlock = NextFreeBlock->next;
	}

	sprintf(TempStr, "Total free heap space: %lu    number of free blocks: %lu    largest free block: %lu\015\012",
				FreeSpace, NumberOfFreeBlocks, LargestFreeBlock );
	SendStringToInterface(TempStr, strlen( TempStr ), true );

	return true;
} // ShowFreeHeapSpaceShortVersion







#ifdef	HEAP_DEBUG
//
// ShowUsedHeapSpace
//		Show all used heap segments.
//
int QSortCompareOfCallerAddress( const void *arg1, const void *arg2 )
{
	HEAP1A_ALLOCATION	*haArg1, *haArg2;

	haArg1 = ( HEAP1A_ALLOCATION * ) arg1;
	haArg2 = ( HEAP1A_ALLOCATION * ) arg2;

	if( haArg1->AddressOfCaller < haArg2->AddressOfCaller )
		return -1;
	else if( haArg1->AddressOfCaller > haArg2->AddressOfCaller )
		return 1;
	else
		return 0;
}

int QSortCompareOfAllocationAddress( const void *arg1, const void *arg2 )
{
	HEAP1A_ALLOCATION	*haArg1, *haArg2;

	haArg1 = ( HEAP1A_ALLOCATION * ) arg1;
	haArg2 = ( HEAP1A_ALLOCATION * ) arg2;

	if( haArg1->AllocatedAddress < haArg2->AllocatedAddress )
		return -1;
	else if( haArg1->AllocatedAddress > haArg2->AllocatedAddress )
		return 1;
	else
		return 0;
}

int QSortCompareOfSize( const void *arg1, const void *arg2 )
{
	HEAP1A_ALLOCATION	*haArg1, *haArg2;

	haArg1 = ( HEAP1A_ALLOCATION * ) arg1;
	haArg2 = ( HEAP1A_ALLOCATION * ) arg2;

	if( haArg1->Size < haArg2->Size )
		return -1;
	else if( haArg1->Size > haArg2->Size )
		return 1;
	else
		return 0;
}

void ShowUsedHeapSpace( const char *SortParameter )
{
	int					i;
	int					NumberOfSortedHeapEntries;
	char				TempStr[80];
	HEAP1A_ALLOCATION	*SortedHeapAllocation;



	SortedHeapAllocation = new HEAP1A_ALLOCATION[ NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES ];

	i = 0;
	NumberOfSortedHeapEntries = 0;
	while( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES )
	{
		if( heapdesc.HEAP1A_Allocation[ i ].Size != 0 )
		{
			SortedHeapAllocation[ NumberOfSortedHeapEntries++ ] = heapdesc.HEAP1A_Allocation[ i ];
		}
		++i;
	}

	if( stricmp( SortParameter, "CALLER" ) == 0 )
	{
		qsort( SortedHeapAllocation, NumberOfSortedHeapEntries, sizeof( HEAP1A_ALLOCATION ), QSortCompareOfCallerAddress );
	}
	else if( stricmp( SortParameter, "LOCATION" ) == 0 )
	{
		qsort( SortedHeapAllocation, NumberOfSortedHeapEntries, sizeof( HEAP1A_ALLOCATION ), QSortCompareOfAllocationAddress );
	}
	else if( stricmp( SortParameter, "SIZE" ) == 0 )
	{
		qsort( SortedHeapAllocation, NumberOfSortedHeapEntries, sizeof( HEAP1A_ALLOCATION ), QSortCompareOfSize );
	}

	for( i = 0;    i < NumberOfSortedHeapEntries;    ++i )
	{
		sprintf( TempStr, "LR = %08X    Address = %08X    Size = %d\r\n", (int) SortedHeapAllocation[ i ].AddressOfCaller,
					(int) SortedHeapAllocation[ i ].AllocatedAddress, SortedHeapAllocation[ i ].Size );
		SendStringToInterface(TempStr, strlen( TempStr ), true );
	}

	delete SortedHeapAllocation;

	return;
} // ShowUsedHeapSpace






//
// ClearUsedHeapSpaceTrackingBuffer
//		Clear out the heap allocation tracking buffer.  The next call to ShowUsedHeapSpace will now only display heap segments allocated after this
//		routine was called, thus allowing the user to limit the number of items that need to be sorted through.
//
//		Note that the return type is bool.  This makes the menu command that uses this routine happy.
//
bool ClearUsedHeapSpaceTrackingBuffer( void )
{
	int		i = 0;
	while( i < NUMBER_OF_HEAP1A_ALLOCATION_ENTRIES )
	{
		heapdesc.HEAP1A_Allocation[ i ].Size = 0;
		++i;
	}

	return true;
} // ClearUsedHeapSpaceTrackingBuffer
#endif




//
// ComputeUntouchedHeapSpace
//		Compute the untouched heap space by finding the first non 0xFFFFFFFF int from the end of the heap.
//
int ComputeUntouchedHeapSpace(void)
{
	int		i;
	int		UntouchedHeapSpace = 0;
	int		*HeapIntPtr = (int *) Heap;

	i = HeapSize / 4 - 1;
	while(( i >= 0) && ( HeapIntPtr[i] == 0xFFFFFFFF ))
	{
		--i;
		UntouchedHeapSpace += 4;
	}

	return( UntouchedHeapSpace );
}


//
// ShowUntouchedHeapSpace
//		Report untouched heap space to the host.
//		Note that the return type is bool.  This makes the menu command that uses this routine happy.
//
bool ShowUntouchedHeapSpace(void)
{
	char	TempStr[80];

	sprintf(TempStr, "Total untouched heap space: %d\015\012", ComputeUntouchedHeapSpace() );
	SendStringToInterface(TempStr, strlen( TempStr ), true );

	return true;
} // ShowUntouchedHeapSpace



//
// Return the address of the heap for anyone who may wish to use it as a scratch pad.
//
UCHAR *HeapAddress(void)
{
	return(Heap);
}




//
//	Create a heap.
//
void HHPHeapInit( BYTE *StartOfHeap, UINT SizeOfHeap )
{
	Heap = StartOfHeap;
	HeapSize = SizeOfHeap;

/*
int	i;
for(i = 0;    i < HeapSize;   ++i)
{
	Heap[i] = 0xFF;
}
*/

	__rt_embeddedalloc_init( &heapdesc, Heap, HeapSize );	// Initialize the heap.

	return;
}




//
//	Add another memory block to the heap to increase its' size.
//
void HHP_AddMemoryToHeap( BYTE *StartOfNewHeapBlock, UINT SizeOfNewHeapBlock )
{
	if( StartOfNewHeapBlock < Heap )
	{
		Heap = StartOfNewHeapBlock;
	}

	HeapSize += SizeOfNewHeapBlock;

	Heap_ProvideMemory( &heapdesc, StartOfNewHeapBlock, SizeOfNewHeapBlock );

	return;
}



