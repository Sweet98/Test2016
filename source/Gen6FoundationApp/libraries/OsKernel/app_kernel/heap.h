/*
 * heap.h
 * Copyright: 1997 Advanced RISC Machines Limited. All rights reserved.
 */

/*
 * $RCSfile: app_kernel/heap.h $
 * $Revision: 1.2 $
 * $Date: 2006/02/22 11:16:42EST $
 */

#ifndef HEAP_H
#define HEAP_H
#ifndef HEAPTYPE
#define HEAPTYPE 1
#endif

#if HEAPTYPE==1
#ifndef _HEAP1A_H
#  include "heap1a.h"
#endif
#define Heap_Descriptor         Heap1a_Descriptor
#define Heap_Initialise         __Heap1a_Initialise
#define Heap_InitMemory         __Heap1a_InitMemory
#define Heap_ProvideMemory      __Heap1a_ProvideMemory
#define Heap_Alloc              __Heap1a_Alloc
#define Heap_Free               __Heap1a_Free
#define Heap_Realloc            __Heap1a_Realloc
#define Heap_Stats              __Heap1a_Stats
#define Heap_TraceAlloc         __Heap1a_TraceAlloc
#elif HEAPTYPE==2
#ifndef _HEAP1B_H
#  include "heap1b.h"
#endif
#define Heap_Descriptor         Heap1b_Descriptor
#define Heap_Initialise         __Heap1b_Initialise
#define Heap_InitMemory         __Heap1b_InitMemory
#define Heap_ProvideMemory      __Heap1b_ProvideMemory
#define Heap_Alloc              __Heap1b_Alloc
#define Heap_Free               __Heap1b_Free
#define Heap_Realloc            __Heap1b_Realloc
#define Heap_Stats              __Heap1b_Stats
#define Heap_TraceAlloc         __Heap1b_TraceAlloc
#elif HEAPTYPE==3
#ifndef _HEAP1c_H
#  include "heap1c.h"
#endif
#define Heap_Descriptor         Heap1c_Descriptor
#define Heap_Initialise         __Heap1c_Initialise
#define Heap_InitMemory         __Heap1c_InitMemory
#define Heap_ProvideMemory      __Heap1c_ProvideMemory
#define Heap_Alloc              __Heap1c_Alloc
#define Heap_Free               __Heap1c_Free
#define Heap_Realloc            __Heap1c_Realloc
#define Heap_Stats              __Heap1c_Stats
#define Heap_TraceAlloc         __Heap1c_TraceAlloc
#elif HEAPTYPE==4
#ifndef _HEAP2_H
#  include "heap2.h"
#endif
#define Heap_Descriptor         Heap2_Descriptor
#define Heap_Initialise         __Heap2_Initialise
#define Heap_InitMemory         __Heap2_ProvideMemory
#define Heap_ProvideMemory      __Heap2_ProvideMemory
#define Heap_Alloc              __Heap2_Alloc
#define Heap_Free               __Heap2_Free
#define Heap_Realloc            __Heap2_Realloc
#define Heap_Stats              __Heap2_Stats
#define Heap_TraceAlloc         __Heap2_TraceAlloc
#elif HEAPTYPE==5
#ifndef _HEAP3_H
#  include "heap3.h"
#endif
#define Heap_Descriptor         Heap3_Descriptor
#define Heap_Initialise         __Heap3_Initialise
#define Heap_InitMemory        __Heap3_ProvideMemory
#define Heap_ProvideMemory      __Heap3_ProvideMemory
#define Heap_Alloc              __Heap3_Alloc
#define Heap_Free               __Heap3_Free
#define Heap_Realloc            __Heap3_Realloc
#define Heap_Stats              __Heap3_Stats
#define Heap_TraceAlloc         __Heap3_TraceAlloc
#elif HEAPTYPE==6
#ifndef _HEAP4_H
#  include "heap4.h"
#endif
#define Heap_Descriptor         Heap4_Descriptor
#define Heap_Initialise         __Heap4_Initialise
#define Heap_InitMemory        __Heap4_ProvideMemory
#define Heap_ProvideMemory      __Heap4_ProvideMemory
#define Heap_Alloc              __Heap4_Alloc
#define Heap_Free               __Heap4_Free
#define Heap_Realloc            __Heap4_Realloc
#define Heap_Stats              __Heap4_Stats
#define Heap_TraceAlloc         __Heap4_TraceAlloc
#elif HEAPTYPE==7
#ifndef _HEAP5_H
#  include "heap5.h"
#endif
#define Heap_Descriptor         Heap5_Descriptor
#define Heap_Initialise         __Heap5_Initialise
#define Heap_InitMemory         __Heap5_ProvideMemory
#define Heap_ProvideMemory      __Heap5_ProvideMemory
#define Heap_Alloc              __Heap5_Alloc
#define Heap_Free               __Heap5_Free
#define Heap_Realloc            __Heap5_Realloc
#define Heap_Stats              __Heap5_Stats
#define Heap_TraceAlloc         __Heap5_TraceAlloc
#elif HEAPTYPE==8
#ifndef _HEAP6_H
#  include "heap6.h"
#endif
#define Heap_Descriptor         Heap6_Descriptor
#define Heap_Initialise         __Heap6_Initialise
#define Heap_InitMemory         __Heap6_ProvideMemory
#define Heap_ProvideMemory      __Heap6_ProvideMemory
#define Heap_Alloc              __Heap6_Alloc
#define Heap_Free               __Heap6_Free
#define Heap_Realloc            __Heap6_Realloc
#define Heap_Stats              __Heap6_Stats
#define Heap_TraceAlloc         __Heap6_TraceAlloc
#elif HEAPTYPE==9
#ifndef _HEAP7_H
#  include "heap7.h"
#endif
#define Heap_Descriptor         Heap7_Descriptor
#define Heap_Initialise         __Heap7_Initialise
#define Heap_InitMemory         __Heap7_ProvideMemory
#define Heap_ProvideMemory      __Heap7_ProvideMemory
#define Heap_Alloc              __Heap7_Alloc
#define Heap_Free               __Heap7_Free
#define Heap_Realloc            __Heap7_Realloc
#define Heap_Stats              __Heap7_Stats
#define Heap_TraceAlloc         __Heap7_TraceAlloc
#endif

/* End of file heap.h */

#endif // HEAP_H


