//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/thread.c $
// $Revision: 1.50.1.18 $
// $Date: 2010/04/23 11:52:19EDT $
//======================================================================================
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Thread.h"
#include "interrupts.h"
#include "db_chrio.h"
#include "db_sharedtable.h"

#ifdef _DEBUG_THREAD

static FILE *m_fp;
void __cdecl printf(const tchar_t *fmt, ...);
#define TH_TRACE(fmt)			printf(fmt);
	#define TH_TRACE1(fmt, var)	printf(fmt, var);
	#define TH_TRACE2(fmt, var1, var2)	printf(fmt, var1, var2);

#else
	#define TH_TRACE(fmt)
	#define TH_TRACE1(fmt, var)
	#define TH_TRACE2(fmt, var1, var2)
#endif


#ifdef THREAD_TEST

void Click(void);
#endif
#if 0 // use enable disable irqs can call add mailbox from irq
#else
#define GET_MAILBOX(ret) DISABLE_INTERRUPTS(); ret=true;
#define RELEASE_MAILBOX() RESTORE_INTERRUPTS()
#define MAILBOX_LOCK
#define CREATE_MAILBOX_LOCK() (true)
#define DESTROY_MAILBOX_LOCK()
#endif

// low power wait on thread 0 uses doze mose
#include "PowerManagement.h"
#define IDLE_WAIT_LOW_POWER() EnterPowerState(DOZE)

#if PLATFORM_CURRENT == imx25
	#define 	DO_SCHEDULER()	HAZARD_WRITE_FIELD_TO_REG(ASIC_INTFRCL,0x0,0x1,0x1) //Force the Scheduler Interrupt
#endif

extern "C"
{


	typedef void *MT_MT_Mailbox_t;

	typedef struct
				Pcbt
	{
		unsigned int sv_psr;
		unsigned int sv_restartaddr;
		unsigned int sv_r0;
		unsigned int sv_r1;
		unsigned int sv_r2;
		unsigned int sv_r3;
		unsigned int sv_r4;
		unsigned int sv_r5;
		unsigned int sv_r6;
		unsigned int sv_r7;
		unsigned int sv_r8;
		unsigned int sv_r9;
		unsigned int sv_r10;
		unsigned int sv_r11;
		unsigned int sv_r12;
		unsigned int sv_r13;
		unsigned int sv_r14;
		unsigned int  * StackBase;
		unsigned int StackSize;
		void (*ThreadFuncPtr)(void * );
		unsigned int MyOpenMutexsMask;
		unsigned int MyOpenEventsMask;
		unsigned int ChildThreadsMask;
		threadhandle_t MyParent;
	}
	PcbType;

	typedef struct MutexStruct_tag
	{
		volatile unsigned int owned;  // This is set to the ID of the thread (which is 0 based) that owns the mutex.  I believe to tell if thread 0 owns it versus no one owns it, we add 80.  TODO:  This addition results from using a variable for two different things (if it's owned and the ID of the owner).  To make it more readable, consider using a bool/int bOwned and another variable to hold the ID of the owner.
		volatile unsigned int Wait_Q;  // This mask is in "reverse order" of thread IDs (bit 31 corresponds to thread ID 0).
		volatile unsigned int count;
		volatile threadhandle_t CreatedBy;
	}
	MutexStruct_t;



	typedef struct EventStruct_tag
	{
		volatile bool Signaled;
		volatile unsigned int waiting;
		volatile threadhandle_t CreatedBy;
		bool AutoReset;
	}
	EventStruct_t;


	// thread structure
	typedef struct
	{
		volatile threadhandle_t CurrentThread;	// index of curent thread
		volatile unsigned int bitmask;			// ready q priority bitmask.  This mask is in "reverse order" of thread IDs (bit 31 corresponds to thread ID 0).
		PcbType *   Pcbs[MAXTHREADS];				// pointers to process control blocks of threads
		volatile unsigned int TimerWaitQ;
		volatile unsigned int Timers[MAXTHREADS];
	}
	Threadpointers_t;



	/* The following type declaration represents the entire state        */
	/* information for a Mailbox.  This structure is used with all of    */
	/* the Mailbox functions contained in this module.                   */
	typedef struct _tagMailboxHeader_t
	{
		MT_Event_t       Event;
		MAILBOX_LOCK			// this is a macro no ; on purpose
		unsigned int  HeadSlot;
		unsigned int  TailSlot;
		unsigned int  OccupiedSlots;
		unsigned int  NumberSlots;
		unsigned int  SlotSize;
		void         *Slots;
	}
	MailboxHeader_t;


	// exit this driver thread
	extern "C" void MT_DriverThreadExit();
	extern "C" void MT_CloseCurrentThread (void);
	typedef void (* Timerptr_t)(void);
	inline bool IsMutexOpen(MT_Mutex_t Mutex);
	bool IsEventOpen(MT_Event_t Event);




	PcbType DefaultThread={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	Threadpointers_t Threadpointers=
	    {
	        0,0x80000000,
	        {	&DefaultThread,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	          NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	          NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	          NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
	        },
	        0
	    };

	MutexStruct_t MutexPool[MAX_MUTEX];
	EventStruct_t EventPool[MAX_EVENT];
	volatile unsigned int OpenMutexsMask;
	unsigned int OpenEventsMask;
	unsigned int OpenThreadsMask;

	MT_Mutex_t stackmutx =NULL;


	void InitializeTheadSystem(void ** p )
	{
//		RESET_WATCHDOG_TIMER();
		for (int i=0;i<MAXTHREADS;i++)
		{
			Threadpointers.Pcbs[i]=NULL;  // memory leak if thread not closed!!!
			Threadpointers.Timers[i]=0;
		}
		Threadpointers.TimerWaitQ=0;
		Threadpointers.bitmask=0x80000000;
		Threadpointers.TimerWaitQ=0;
		Threadpointers.CurrentThread=0;
		Threadpointers.Pcbs[0]=&DefaultThread;
		unsigned int UsrStackSize = (unsigned int) SYS_STACK - (unsigned int) HEAP_END;
		unsigned int UsrStackBase = (unsigned int) SYS_STACK - UsrStackSize;
		DefaultThread.StackSize=UsrStackSize;
		DefaultThread.StackBase=(unsigned int *)UsrStackBase;

		OpenMutexsMask=0;
		OpenEventsMask=0;
		OpenThreadsMask=0;

		if (p)
			*p=(void *)ServiceThreadTimers;
		if (!stackmutx)
			stackmutx=MT_CreateMutex(false);
		UnLockSheduler();
	}

	void ServiceThreadTimers(void)
	{
		//	if (Threadpointers.TimerWaitQ ==0)
		//	{
		//		return;
		//	}
		unsigned int tmsk=0;
		unsigned int msk=Threadpointers.TimerWaitQ ;
		while (true)
		{
			unsigned int i= __clz(msk);
			if (i>31)
				break;
			msk &=~(0x80000000>>i);
			if (Threadpointers.Timers[i])
			{
				if (--Threadpointers.Timers[i] ==0)
				{
					tmsk |=(0x80000000>>i);
				}
			}
		}
		//	unsigned int * tmrptr =(unsigned int *)&(Threadpointers.Timers[0]);
		//	for (	unsigned int msk=0x80000000;msk;msk>>=1)
		//	{
		//		if (*tmrptr  && *tmrptr!=INFINITE)
		//			if (--(*tmrptr)==0)
		//				tmsk|=msk;
		//		tmrptr++;
		//	}
		DISABLE_INTERRUPTS();
		//tmsk &=Threadpointers.TimerWaitQ;
		if (tmsk)		// if some thread neeeds to run do schedule
		{
			Threadpointers.TimerWaitQ &=(~tmsk);
			Threadpointers.bitmask |=tmsk;
			Threadpointers.bitmask |=0x80000000;
			DO_SCHEDULER();
		}
		RESTORE_INTERRUPTS();
	}




	//threadhandle_t MT_OpenDriverThread(int StackSize,void (*ThreadPtr)(void * ),void * ptr)
	//{
	//	return MT_OpenDriverThread(1, StackSize,ThreadPtr,ptr);
	//}

	threadhandle_t MT_OpenDriverThread(threadhandle_t priority,int StackSize,void (*ThreadPtr)(void * ),void * ptr)
	{
		threadhandle_t i=priority;
		for ( ;i<MAXTHREADS;i++)
			if (Threadpointers.Pcbs[i]==NULL)
				break;
		if (i>=MAXTHREADS)
			return 0;		// no threads left to allocate return 0
		if (NULL == (Threadpointers.Pcbs[i]=(PcbType *)calloc(1,sizeof(PcbType))))
			return 0;				// couldnt allocate process control block return error

		StackSize=(StackSize*8+7)/8; // round stacksize up to even # of words jw stack should be double word aligned
		if (NULL == (Threadpointers.Pcbs[i]->StackBase =(unsigned *) malloc(StackSize)))
		{
			free(Threadpointers.Pcbs[i]);			// we couldnt allocate stack return error
			return 0;
		}
		Threadpointers.Timers[i]=0;
		Threadpointers.Pcbs[i]->StackSize =StackSize;
		Threadpointers.Pcbs[i]->ThreadFuncPtr=ThreadPtr;
		for ( int j=0;j<Threadpointers.Pcbs[i]->StackSize/sizeof(int);j++)
			Threadpointers.Pcbs[i]->StackBase[j]=0xffffffff;
		Threadpointers.Pcbs[i]->sv_r14=(unsigned int)&MT_DriverThreadExit;

		Threadpointers.Pcbs[i]->sv_r13=
		    (unsigned int)(&Threadpointers.Pcbs[i]->StackBase[Threadpointers.Pcbs[i]->StackSize/sizeof(unsigned int) -1]);
		Threadpointers.Pcbs[i]->sv_r13&=~7;		//force double word alginment
		Threadpointers.Pcbs[i]->sv_r0=(unsigned int)ptr;  // r0 gets the pointer to data
		Threadpointers.Pcbs[i]->sv_restartaddr=(unsigned int)ThreadPtr;       //now save the address of the thread
		Threadpointers.Pcbs[i]->sv_psr=0x1f;       //CPSR system mode non thumb mode  irqs fiqs on
		OpenThreadsMask |=0x80000000>>i;
		threadhandle_t myself=Threadpointers.CurrentThread;		//thread creating this new one
		Threadpointers.Pcbs[i]->MyParent=myself;
		Threadpointers.Pcbs[myself]->ChildThreadsMask |= (0x80000000>>i);
		Threadpointers.Pcbs[i]->ChildThreadsMask=0;
		Threadpointers.Pcbs[i]->MyOpenMutexsMask=0;
		Threadpointers.Pcbs[i]->MyOpenEventsMask=0;
		return i;
	}



	extern "C" void MT_CloseCurrentThread (void)
	{
		threadhandle_t myself= Threadpointers.CurrentThread;
		MT_CloseDriverThread(myself);
		//CloseThreadsByBitmask(0x80000000>>myself);
		//	unsigned int chldrn=Threadpointers.Pcbs[myself]->ChildThreadsMask;
	}


	extern "C" void MT_CloseDriverThread (threadhandle_t threadhandle)
	{
		DISABLE_INTERRUPTS();
		/* now the thread is dead, let's clean up the mess */
		if ((threadhandle!=0) && (NULL !=Threadpointers.Pcbs[threadhandle]))
		{
			// need to remove from preempted q  also
			Threadpointers.bitmask    &=~(0x80000000>>threadhandle);	//clear out the entry in bitmask
			Threadpointers.TimerWaitQ &=~(0x80000000>>threadhandle);	//timer waiting Q
			OpenThreadsMask 			  &=~(0x80000000>>threadhandle);
			PcbType * tmp=Threadpointers.Pcbs[threadhandle];
			PcbType * myparent=Threadpointers.Pcbs[tmp->MyParent];
			if (myparent)
				myparent->ChildThreadsMask&=~(0x80000000>>threadhandle);
			Threadpointers.Pcbs[threadhandle]=NULL;
			Threadpointers.Timers[threadhandle]=0;
			free (tmp->StackBase);
			free(tmp);
		}
		RESTORE_INTERRUPTS();
	}


	threadhandle_t MT_GetCurrentThread(void)
	{
		return Threadpointers.CurrentThread;
	}


	bool IsThreadOpen(threadhandle_t hndl)
	{
		return  (hndl <32) &&(Threadpointers.Pcbs[hndl]!=NULL);
	}


	unsigned int GetMutexMask(MT_Mutex_t Mutex)
	{
		unsigned int offset=Mutex-&MutexPool[0];
		return (0x80000000>>offset);

	}

	inline bool IsMutexOpen(MT_Mutex_t Mutex)
	{
		unsigned int offset=Mutex-&MutexPool[0];
		return (offset <32) &&(OpenMutexsMask & (0x80000000>>offset));

	}



	inline bool IsEventOpen(MT_Event_t Event)
	{
		unsigned int offset=Event-&EventPool[0];
		return (offset <32) &&(OpenEventsMask & (0x80000000>>offset));

	}




	void WakeDriverThread(threadhandle_t thread)
	{
		DISABLE_INTERRUPTS();

		if (thread  && IsThreadOpen(thread))
		{
			Threadpointers.bitmask|=0x80000000>>thread;
			DO_SCHEDULER();
		}
		RESTORE_INTERRUPTS();
	}

	void SleepDriverThread(void)
	{
		DISABLE_INTERRUPTS();
		if (Threadpointers.CurrentThread)
		{
			Threadpointers.bitmask &=~(0x80000000>>Threadpointers.CurrentThread);
			DO_SCHEDULER();
		}
		RESTORE_INTERRUPTS();
//		RESET_WATCHDOG_TIMER();
	}


	MT_Mutex_t MT_CreateMutex(bool CreateOwned)
	{
		threadhandle_t myself=Threadpointers.CurrentThread;
		unsigned int temp;
		unsigned int mask =0x80000000;
		MT_Mutex_t Ret=NULL;
		DISABLE_INTERRUPTS();
		for( temp=0;temp<MAX_MUTEX;temp ++)
			if (OpenMutexsMask & mask )
				mask>>=1;
			else
				break;
		if (temp<MAX_MUTEX)
		{
			OpenMutexsMask |= mask;
			Ret= &MutexPool[temp];
			Ret->Wait_Q=0;
			Ret->count=0;
			Ret->CreatedBy=myself;
			Threadpointers.Pcbs[myself]->MyOpenMutexsMask|=mask;
		}
		RESTORE_INTERRUPTS();
		if (CreateOwned && Ret)
			MT_WaitMutex(Ret,INFINITE);
		return Ret;
	}
#define MUTEX_AQUIRED 2
#define MUTEX_TIMEOUT 1
	bool MT_WaitMutex(MT_Mutex_t Mutex, unsigned long Timeout)
	{
		threadhandle_t myself = Threadpointers.CurrentThread;
		if (myself>=MAXTHREADS)
		{
			return false;
		}
		unsigned int mask=0x80000000>>myself;
		Threadpointers.Timers[myself]=Timeout;
		int Done=0;
		do
		{
			if (!IsMutexOpen(Mutex))									// if not created yet return error
			{
				TH_TRACE1("mtx not open %X\r\n",(unsigned int )Mutex);
				return false;
			}
			DISABLE_INTERRUPTS();
			Mutex->Wait_Q &=~(mask);
			Threadpointers.TimerWaitQ &=~(mask);
			if ((Mutex->owned==0)||(Mutex->owned==(myself+0x80U)))	
			{	// Either no one owns the mutex (yet--this thread will in a moment) or it's ours (this thread already owns the mutex)
				Mutex->owned=myself+0x80U;  // +80 so we can tell if thread 0 "owns" the mutex
				Mutex->count++;
				Threadpointers.Timers[myself]=0;
				Done=MUTEX_AQUIRED;
			}																	// maybe we can have it so try
			else
			{	// This is another thread that wants the mutex.
				if (!Threadpointers.Timers[myself])
				{
					Done =MUTEX_TIMEOUT;
				}
				else
				{
					if (Threadpointers.Timers[myself]!=INFINITE)
					{
						Threadpointers.TimerWaitQ |=mask;																		//didnt get it but time left so sleep
					}
					Threadpointers.bitmask &=~(mask);
					Threadpointers.bitmask |=0x80000000;
					Mutex->Wait_Q|=mask;
					if (myself)										// if this is not the idle threa sleep
					{
						DO_SCHEDULER();
					}
					else
					{
						RESET_WATCHDOG_TIMER();	//added for callers in thread zero, since dog is only kicked in thread zero now, and thus we will bark when we sleep that thread
						IDLE_WAIT_LOW_POWER();
					}
				}
			}
			RESTORE_INTERRUPTS();
//			RESET_WATCHDOG_TIMER();
		}
		while (!Done);
		return Done==MUTEX_AQUIRED;
	}



	void MT_ReleaseMutex(MT_Mutex_t Mutex)
	{
		threadhandle_t myself=Threadpointers.CurrentThread;
		DISABLE_INTERRUPTS();
		if (Mutex && (Mutex->owned==(myself+0x80)) && (--Mutex->count)==0)
		{
			Mutex->owned=0;
			Threadpointers.bitmask |=Mutex->Wait_Q;
			Mutex->Wait_Q=0;
			DO_SCHEDULER();
		}
		RESTORE_INTERRUPTS();
	}

	void MT_CloseMutex(MT_Mutex_t Mutex)
	{
		DISABLE_INTERRUPTS();
		if (IsMutexOpen(Mutex))				// wait till we can release
		{
			PcbType * p;
			unsigned int offset=Mutex-&MutexPool[0];
			OpenMutexsMask &=~(0x80000000>>offset);
			if ((p=Threadpointers.Pcbs[Mutex->CreatedBy])!=NULL)
				p->MyOpenMutexsMask &=~(0x80000000>>offset);
			Threadpointers.bitmask |=Mutex->Wait_Q;  // make any thread that is waiting for the mutex ready to run so it can return from MT_WaitMutex() with an error
			Mutex->Wait_Q=0;
			Mutex->owned=0;
			Mutex->count=0;
			DO_SCHEDULER();
		}
		else
		{
			TH_TRACE1("Invalid Mutex %X \r\n",(unsigned int)Mutex);
		}
		RESTORE_INTERRUPTS();
	}

	void MT_Delay(unsigned long MilliSeconds)
	{
		if (MilliSeconds==0)
			return;
		bool done=false;
		threadhandle_t myself=Threadpointers.CurrentThread;
		DISABLE_INTERRUPTS();
		Threadpointers.Timers[myself]=MilliSeconds+1;
		RESTORE_INTERRUPTS();
		do
		{
			DISABLE_INTERRUPTS();
			if	(Threadpointers.Timers[myself]==0 )
			{
				done=true;
				Threadpointers.TimerWaitQ &=~(0x80000000>>myself);
				//theStdInterface.printf("4");
			}
			else
			{
				Threadpointers.TimerWaitQ |=(0x80000000>> myself);
				Threadpointers.bitmask &=~(0x80000000>>myself);
				Threadpointers.bitmask |=0x80000000;
				if (myself)
				{
					DO_SCHEDULER();
				}
				else
				{
					RESET_WATCHDOG_TIMER();	//added for callers in thread zero, since dog is only kicked in thread zero now, and thus we will bark when we sleep that thread
					IDLE_WAIT_LOW_POWER();
				}
			}
			RESTORE_INTERRUPTS();
			RESET_WATCHDOG_TIMER();
		}
		while (!done);
	}





	/* The following function is responsible for creating an actual      */
	/* Event.  The Event is unique in that it only has two states.  These*/
	/* states are Signalled and Non-Signalled.  Functions are provided   */
	/* to allow the setting of the current state and to allow the        */
	/* option of waiting for an Event to become Signalled.  This function*/
	/* accepts as input whether or not the Event is initially Signalled  */
	/* or not.  If this input parameter is TRUE then the state of the    */
	/* Event is Signalled and any MT_WaitEvent() function calls will   */
	/* immediately return.  This function returns a NON-NULL Event       */
	/* Handle if the Event was successfully created, or a NULL Event     */
	/* Handle if the Event was NOT created.  If an Event is successfully */
	/* created, it can only be destroyed by calling the MT_CloseEvent()*/
	/* function (and passing the returned Event Handle).                 */

	MT_Event_t  MT_CreateEvent(bool CreateSignalled)
	{
		return  MT_CreateEvent_Ex(CreateSignalled,false);
	}
	/*
		extended version with auto reset
	*/
	MT_Event_t  MT_CreateEvent_Ex(bool CreateSignalled,bool AutoReset)
	{
		threadhandle_t myself=Threadpointers.CurrentThread;
		unsigned int temp;
		unsigned int mask =0x80000000;
		MT_Event_t Ret=NULL;

		DISABLE_INTERRUPTS();
		for( temp=0;temp<MAX_EVENT;temp ++)
			if (OpenEventsMask & mask )
				mask>>=1;
			else
				break;
		if (temp<MAX_EVENT)
		{
			Ret= &EventPool[temp];
			OpenEventsMask |= mask;
			Threadpointers.Pcbs[myself]->MyOpenEventsMask|=mask;
			Ret->CreatedBy=myself;
			Ret->waiting=0;
			Ret->Signaled=CreateSignalled;
			Ret->AutoReset=AutoReset;
		}
		RESTORE_INTERRUPTS();
		return Ret;
	}

	/* The following function is responsible for waiting for the         */
	/* specified Event to become Signalled.  This function accepts as    */
	/* input the Event Handle to wait for, and the Timeout (specified    */
	/* in Milliseconds) to wait for the Event to become Signalled.  This */
	/* function returns TRUE if the Event was set to the Signalled       */
	/* State (in the Timeout specified) or FALSE if either there was an  */
	/* error OR the Event was not set to the Signalled State in the      */
	/* specified Timeout.  It should be noted that Signalls have a       */
	/* special property in that multiple Threads can be waiting for the  */
	/* Event to become Signalled and ALL calls to MT_WaitEvent() will  */
	/* return TRUE whenever the state of the Event becomes Signalled.    */

	bool  MT_WaitEvent(MT_Event_t Event, unsigned long Timeout)
	{
		int Done =0;
		threadhandle_t myself=Threadpointers.CurrentThread;
		Threadpointers.Timers[myself]=Timeout;
		unsigned int mask=0x80000000>>myself;
		do
		{
			if (!IsEventOpen(Event))
			{
				TH_TRACE1("Event not open %X \r\n",(unsigned int ) Event);
				return false;

			}
			DISABLE_INTERRUPTS();
			Event->waiting &=~(mask);
			Threadpointers.TimerWaitQ &=~(mask);
			if ( Event->Signaled)
			{
				Done =MUTEX_AQUIRED;
				Threadpointers.Timers[myself]=0;
				if (Event->AutoReset)
				{
					Event->Signaled=false;
				}
			}
			else
			{
				if	(Threadpointers.Timers[myself]==0 )
				{
					Done =MUTEX_TIMEOUT;
				}
				else
				{
					if (Threadpointers.Timers[myself]!=INFINITE)
					{
						Threadpointers.TimerWaitQ |=(mask);
					}

					Event->waiting |=(mask);
					Threadpointers.bitmask &=~(mask);
					Threadpointers.bitmask |=0x80000000;
					if (myself)
					{
						DO_SCHEDULER();
					}
					else
					{
						RESET_WATCHDOG_TIMER();	//added for callers in thread zero, since dog is only kicked in thread zero now, and thus we will bark when we sleep that thread
						IDLE_WAIT_LOW_POWER();
					}
				}
			}
			RESTORE_INTERRUPTS();
//			RESET_WATCHDOG_TIMER();
		}
		while (!Done);
		return Done == MUTEX_AQUIRED;
	}



	/* The following function is responsible for waiting for any of the  */
	/* specified Events to become Signalled.  This function accepts as   */
	/* input the number of events to wait for,Event Handles to wait for  */
	/* as an array, and the Timeout (specified in Milliseconds) to wait	*/
	/* for any Event to become Signalled.  This function returns the 		*/
	/* index of the first event in the array given that is set to the 	*/
	/* Signalled State (in the Timeout specified) orWAIT_TIMED_OUT if the*/
	/* Event was not set to the Signalled State in the specified Timeout.*/
	/* This function returns other values <0 if the number of events is 	*/
	/* is not valid or an Event in the list is known to not be open.     */
	/* If there are auto reset events in the list ony the event whose 	*/
	/* index is returned is reset if it is auto reset.							*/
	/* It should be noted that Signalls have a special property in that 	*/
	/* multiple Threads can be waiting for the Event to become Signalled */
	/* and ALL calls to MT_WaitEvent() will return TRUE whenever the 		*/
	/* state of the Event becomes Signalled.note also this assumes that 	*/
	/* lowest priority waiting resets the event									*/

	int MT_WaitMultiEvents(unsigned int NumEvents,MT_Event_t* Events, unsigned long Timeout)
	{
		int Done =MAX_EVENT;
		threadhandle_t myself=Threadpointers.CurrentThread;
		Threadpointers.Timers[myself]=Timeout;
		unsigned int mask=0x80000000>>myself;
		if ((NumEvents==0) || (NumEvents>MAX_EVENT))
			return -2;
		do
		{
			for(int i=0;i<NumEvents;i++)
			{
				if (!IsEventOpen(Events[i]))
				{
					TH_TRACE1("Event not open %X \r\n",(unsigned int ) Event);
					return -2;

				}
			}
			DISABLE_INTERRUPTS();
			Threadpointers.TimerWaitQ &=~(mask);
			int i;
			for(i=0;i<NumEvents;i++)
				Events[i]->waiting &=~(mask);
			for(i=0;i<NumEvents;i++)
			{
				if ( Events[i]->Signaled)
				{
					Done =i;
					if (Events[i]->AutoReset)
					{
						Events[i]->Signaled=false;
					}
					break;
				}

			}
			if (Done<MAX_EVENT)
				Threadpointers.Timers[myself]=0;
			else
				if	(Threadpointers.Timers[myself]==0 )
					Done =WAIT_TIMED_OUT;			//Wait multiple timeout
				else
				{
					for(int i=0;i<NumEvents;i++)
					{
						Events[i]->waiting |=(mask);
					}
					if (Threadpointers.Timers[myself]!=INFINITE)
					{
						Threadpointers.TimerWaitQ |=(mask);
					}
					Threadpointers.bitmask &=~(mask);
					Threadpointers.bitmask |=0x80000000;
					if (myself)
					{
						DO_SCHEDULER();
					}
					else
					{
						RESET_WATCHDOG_TIMER();   //added for callers in thread zero, since dog is only kicked in thread zero now, and thus we will bark when we sleep that thread
						IDLE_WAIT_LOW_POWER();
					}
				}
			RESTORE_INTERRUPTS();
//			RESET_WATCHDOG_TIMER();
		}
		while (Done==MAX_EVENT);
		return Done;
	}


	/* The following function is responsible for changing the state of   */
	/* the specified Event to the Non-Signalled State.  Once the Event   */
	/* is in this State, ALL calls to the MT_WaitEvent() function will */
	/* block until the State of the Event is set to the Signalled State. */
	/* This function accepts as input the Event Handle of the Event to   */
	/* set to the Non-Signalled State.                                   */
	void MT_ResetEvent(MT_Event_t Event)
	{
		DISABLE_INTERRUPTS();
		Event->Signaled=false;
		RESTORE_INTERRUPTS();
	}

	/* The following function is responsible for changing the state of   */
	/* the specified Event to the Signalled State.  Once the Event is in */
	/* this State, ALL calls to the MT_WaitEvent() function will       */
	/* return.  This function accepts as input the Event Handle of the   */
	/* Event to set to the Signalled State.
	*/

	void MT_SetEvent(MT_Event_t Event)
	{

		DISABLE_INTERRUPTS();
		Event->Signaled=true;
		Threadpointers.bitmask|=Event->waiting;
		Event->waiting=0;
		DO_SCHEDULER();
		RESTORE_INTERRUPTS();
	}

	/* The following function is responsible for destroying an Event that*/
	/* was created successfully via a successful call to the             */
	/* MT_CreateEvent() function.  This function accepts as input the  */
	/* Event Handle of the Event to destroy.  Once this function is      */
	/* completed the Event Handle is NO longer valid and CANNOT be       */
	/* used.  Calling this function will cause all outstanding           */
	/* MT_WaitEvent() functions to fail with an error.                 */
	void MT_CloseEvent(MT_Event_t Event)
	{
		PcbType * p;
		DISABLE_INTERRUPTS();
		if (IsEventOpen(Event))
		{
			unsigned int offset=Event-&EventPool[0];
			OpenEventsMask &=~(0x80000000>>offset);
			if ((p=Threadpointers.Pcbs[Event->CreatedBy])!=NULL)
				p->MyOpenEventsMask &=~(0x80000000>>offset);
		}
		else
			TH_TRACE1("Invalid event %X \r\n",(unsigned int) Event);
		RESTORE_INTERRUPTS();
	}


	/* The following function is provided to allow a mechanism to create */
	/* a Mailbox.  A Mailbox is a Data Store that contains slots (all    */
	/* of the same size) that can have data placed into (and retrieved   */
	/* from).  Once Data is placed into a Mailbox (via the               */
	/* MT_AddMailbox() function, it can be retreived by using the      */
	/* MT_WaitMailbox() function.  Data placed into the Mailbox is     */
	/* retrieved in a FIFO method.  This function accepts as input the   */
	/* Maximum Number of Slots that will be present in the Mailbox and   */
	/* the Size of each of the Slots.  This function returns a NON-NULL  */
	/* Mailbox Handle if the Mailbox is successfully created, or a       */
	/* NULL Mailbox Handle if the Mailbox was unable to be created.      */
	MT_Mailbox_t   MT_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize)
	{
		MT_MT_Mailbox_t        ret_val;
		MailboxHeader_t *MailboxHeader;

		/* Before proceding any further we need to make sure that the        */
		/* parameters that were passed to us appear semi-valid.              */
		if((NumberSlots) && (SlotSize))
		{
			/* Parameters appear semi-valid, so now let's allocate enough     */
			/* Memory to hold the Mailbox Header AND enough space to hold     */
			/* all requested Mailbox Slots.                                   */
			if((MailboxHeader = (MailboxHeader_t *)malloc(sizeof(MailboxHeader_t)+(NumberSlots*SlotSize))) != NULL)
			{
				/* Memory successfully allocated so now let's create an        */
				/* Event that will be used to signal when there is Data        */
				/* present in the Mailbox.                                     */
				if((MailboxHeader->Event = MT_CreateEvent(FALSE)) != NULL)
				{
					/* Event created successfully, now let's create a Mutex     */
					/* that will guard access to the Mailbox Slots.             */
					if(CREATE_MAILBOX_LOCK())
					{
						/* Everything successfully created, now let's initialize */
						/* the state of the Mailbox such that it contains NO     */
						/* Data.                                                 */
						MailboxHeader->NumberSlots   = NumberSlots;
						MailboxHeader->SlotSize      = SlotSize;
						MailboxHeader->HeadSlot      = 0;
						MailboxHeader->TailSlot      = 0;
						MailboxHeader->OccupiedSlots = 0;
						MailboxHeader->Slots         = ((unsigned char *)MailboxHeader) + sizeof(MailboxHeader_t);

						/* All finished, return sucess to the caller (the        */
						/* Mailbox Header).                                      */
						ret_val                      = (MT_Mailbox_t)MailboxHeader;
					}
					else
					{
						/* Error creating the Mutex, so let's free the Event     */
						/* we created, Free the Memory for the Mailbox itself    */
						/* and return an error to the caller.                    */
						MT_CloseEvent(MailboxHeader->Event);

						free(MailboxHeader);

						ret_val = NULL;
					}
				}
				else
				{
					/* Error creating the Data Available Event, so let's free   */
					/* the Memory for the Mailbox itself and return an error    */
					/* to the caller.                                           */
					free(MailboxHeader);

					ret_val = NULL;
				}
			}
			else
				ret_val = NULL;
		}
		else
			ret_val = NULL;

		/* Return the result to the caller.                                  */
		return(ret_val);
	}

	/* The following function is provided to allow a means to Add data   */
	/* to the Mailbox (where it can be retrieved via the                 */
	/* MT_WaitMailbox() function.  This function accepts as input the  */
	/* Mailbox Handle of the Mailbox to place the data into and a        */
	/* pointer to a buffer that contains the data to be added.  This     */
	/* pointer *MUST* point to a data buffer that is AT LEAST the Size   */
	/* of the Slots in the Mailbox (specified when the Mailbox was       */
	/* created) and this pointer CANNOT be NULL.  The data that the      */
	/* MailboxData pointer points to is placed into the Mailbox where it */
	/* can be retrieved via the MT_WaitMailbox() function.             */
	/* * NOTE * This function copies from the MailboxData Pointer the    */
	/*          first SlotSize Bytes.  The SlotSize was specified when   */
	/*          the Mailbox was created via a successful call to the     */
	/*          MT_CreateMailbox() function.                           */
	int   MT_AddMailbox(MT_Mailbox_t Mailbox, void *MailboxData)
	{
		int ret_val;

		/* Before proceding any further make sure that the Mailbox Handle    */
		/* and the MailboxData pointer that was specified appears semi-valid.*/
		if((Mailbox) && (MailboxData))
		{
			/* Since we are going to change the Mailbox we need to acquire    */
			/* the Mutex that is guarding access to it.                       */

			bool locked;
			GET_MAILBOX(locked);
			if(locked)
			{
				/* Before adding the data to the Mailbox, make sure that the   */
				/* Mailbox is not already full.                                */
				if(((MailboxHeader_t *)Mailbox)->OccupiedSlots < ((MailboxHeader_t *)Mailbox)->NumberSlots)
				{
					/* Mailbox is NOT full, so add the specified User Data to   */
					/* the next available free Mailbox Slot.                    */
					memcpy(&(((unsigned char *)(((MailboxHeader_t *)Mailbox)->Slots))[((MailboxHeader_t *)Mailbox)->HeadSlot*((MailboxHeader_t *)Mailbox)->SlotSize]), MailboxData, ((MailboxHeader_t *)Mailbox)->SlotSize);

					/* Update the Next available Free Mailbox Slot (taking      */
					/* into account wrapping the pointer).                      */
					if(++(((MailboxHeader_t *)Mailbox)->HeadSlot) == ((MailboxHeader_t *)Mailbox)->NumberSlots)
						((MailboxHeader_t *)Mailbox)->HeadSlot = 0;

					/* Update the Number of occupied slots to signify that there*/
					/* was additional Mailbox Data added to the Mailbox.        */
					((MailboxHeader_t *)Mailbox)->OccupiedSlots++;

					/* Signal the Event that specifies that there is indeed     */
					/* Data present in the Mailbox.                             */
					MT_SetEvent(((MailboxHeader_t *)Mailbox)->Event);

					/* Finally, return success to the caller.                   */
					ret_val = TRUE;
				}
				else
					ret_val = FALSE;

				/* All finished with the Mailbox, so release the Mailbox       */
				/* Mutex that we currently hold.                               */
				RELEASE_MAILBOX();
			}
			else
				ret_val = FALSE;
		}
		else
			ret_val = FALSE;

		/* Return the result to the caller.                                  */
		return(ret_val);
	}

	/* The following function is provided to allow a means to retrieve   */
	/* data from the specified Mailbox.  This function will block until  */
	/* either Data is placed in the Mailbox or an error with the Mailbox */
	/* was detected.  This function accepts as its first parameter a     */
	/* Mailbox Handle that represents the Mailbox to wait for the data   */
	/* with.  This function accepts as its second parameter, a pointer   */
	/* to a data buffer that is AT LEAST the size of a single Slot of    */
	/* the Mailbox (specified when the MT_CreateMailbox() function     */
	/* was called).  The MailboxData parameter CANNOT be NULL.  This     */
	/* function will return TRUE if data was successfully retrieved      */
	/* from the Mailbox or FALSE if there was an error retrieving data   */
	/* from the Mailbox.  If this function returns TRUE then the first   */
	/* SlotSize bytes of the MailboxData pointer will contain the data   */
	/* that was retrieved from the Mailbox.                              */
	/* * NOTE * This function copies to the MailboxData Pointer the      */
	/*          data that is present in the Mailbox Slot (of size        */
	/*          SlotSize).  The SlotSize was specified when the Mailbox  */
	/*          was created via a successful call to the                 */
	/*          MT_CreateMailbox() function.                           */
	int   MT_WaitMailbox(MT_Mailbox_t Mailbox,unsigned long timeout, void *MailboxData)
	{
		int ret_val;

		/* Before proceding any further make sure that the Mailbox Handle    */
		/* and the MailboxData pointer that was specified appears semi-valid.*/
		if((Mailbox) && (MailboxData))
		{
			/* Next, we need to block until there is at least one Mailbox     */
			/* Slot occupied by waiting on the Data Available Event.          */
			if(MT_WaitEvent(((MailboxHeader_t *)Mailbox)->Event, timeout))
			{
				/* Since we are going to update the Mailbox, we need to acquire*/
				/* the Mutex that guards access to the Mailox.                 */
				bool locked;
				GET_MAILBOX(locked);
				if(locked)
				{
					/* Let's double check to make sure that there does indeed   */
					/* exist at least one slot with Mailbox Data present in it. */
					if(((MailboxHeader_t *)Mailbox)->OccupiedSlots)
					{
						/* Flag success to the caller.                           */
						ret_val = TRUE;

						/* Now copy the Data into the Memory Buffer specified    */
						/* by the caller.                                        */
						memcpy(MailboxData, &((((unsigned char *)((MailboxHeader_t *)Mailbox)->Slots))[((MailboxHeader_t *)Mailbox)->TailSlot*((MailboxHeader_t *)Mailbox)->SlotSize]), ((MailboxHeader_t *)Mailbox)->SlotSize);

						/* Now that we've copied the data into the Memory Buffer */
						/* specified by the caller we need to mark the Mailbox   */
						/* Slot as free.                                         */
						if(++(((MailboxHeader_t *)Mailbox)->TailSlot) == ((MailboxHeader_t *)Mailbox)->NumberSlots)
							((MailboxHeader_t *)Mailbox)->TailSlot = 0;

						((MailboxHeader_t *)Mailbox)->OccupiedSlots--;

						/* If there is NO more data available in this Mailbox    */
						/* then we need to flag it as such by Resetting the      */
						/* Mailbox Data available Event.                         */
						if(!((MailboxHeader_t *)Mailbox)->OccupiedSlots)
							MT_ResetEvent(((MailboxHeader_t *)Mailbox)->Event);
					}
					else
					{
						/* Internal error, flag that there is NO Data present    */
						/* in the Mailbox (by resetting the Data Available       */
						/* Event) and return failure to the caller.              */
						MT_ResetEvent(((MailboxHeader_t *)Mailbox)->Event);

						ret_val = FALSE;
					}

					/* All finished with the Mailbox, so release the Mailbox    */
					/* Mutex that we currently hold.                            */
					RELEASE_MAILBOX();
				}
				else
					ret_val = FALSE;
			}
			else
				ret_val = FALSE;
		}
		else
			ret_val = FALSE;

		/* Return the result to the caller.                                  */
		return(ret_val);
	}

	/* The following function is responsible for destroying a Mailbox    */
	/* that was created successfully via a successful call to the        */
	/* MT_CreateMailbox() function.  This function accepts as input    */
	/* the Mailbox Handle of the Mailbox to destroy.  Once this function */
	/* is completed the Mailbox Handle is NO longer valid and CANNOT be  */
	/* used.  Calling this function will cause all outstanding           */
	/* MT_WaitMailbox() functions to fail with an error.               */
	void  MT_DeleteMailbox(MT_Mailbox_t Mailbox)
	{
		/* Before proceding any further make sure that the Mailbox Handle    */
		/* that was specified appears semi-valid.                            */
		if(Mailbox)
		{
			/* Mailbox appears semi-valid, so let's free all Events and       */
			/* Mutexes and finally free the Memory associated with the        */
			/* Mailbox itself.                                                */
			MT_CloseEvent(((MailboxHeader_t *)Mailbox)->Event);
			DESTROY_MAILBOX_LOCK();

			/* All finished cleaning up the Mailbox, simply free all          */
			/* memory that was allocated for the Mailbox.                     */
			free(Mailbox);
		}
	}


	/* MT_SetContext Set the context of a thread to some state it set previously 	*/
	/* first parameter is the thread to set the contest of. 2nd parameter is the	*/
	/* buffer containing the context, this buffer should have been passed to 		*/
	/* MT_GetContext() earlier. If sucsessful MT_SetContext causes the thread		*/
	/* that called MT_GetContext to resume execution from the point that				*/
	/* MT_GetContext returned from																*/
	/* This function returns false indicating a failure if the thread is invalid 	*/
	/* It does not mattch the context saved i,e you cant set one thread to the 	*/
	/* saved by another thread																		*/

	bool  MT_SetContext(threadhandle_t thr,CONTEXTBUF buf)
	{
		bool ret =false;
		DISABLE_INTERRUPTS();
		if ((thr == (threadhandle_t)buf[17] && thr <MAXTHREADS) && (Threadpointers.Pcbs[thr]))
		{
			Threadpointers.Pcbs[thr]->sv_r0=buf[2];
			Threadpointers.Pcbs[thr]->sv_r1=buf[3];
			Threadpointers.Pcbs[thr]->sv_r2=buf[4];
			Threadpointers.Pcbs[thr]->sv_r3=buf[5];
			Threadpointers.Pcbs[thr]->sv_r4=buf[6];
			Threadpointers.Pcbs[thr]->sv_r5=buf[7];
			Threadpointers.Pcbs[thr]->sv_r6=buf[8];
			Threadpointers.Pcbs[thr]->sv_r7=buf[9];
			Threadpointers.Pcbs[thr]->sv_r8=buf[10];
			Threadpointers.Pcbs[thr]->sv_r9=buf[11];
			Threadpointers.Pcbs[thr]->sv_r10=buf[12];
			Threadpointers.Pcbs[thr]->sv_r11=buf[13];
			Threadpointers.Pcbs[thr]->sv_r12=buf[14];
			Threadpointers.Pcbs[thr]->sv_r13=buf[15];
			Threadpointers.Pcbs[thr]->sv_r14=buf[16];
			Threadpointers.Pcbs[thr]->sv_restartaddr=buf[16];
			Threadpointers.Pcbs[thr]->sv_psr=buf[0];
			ret=true;
		}
		RESTORE_INTERRUPTS();
		return ret;
	}

	unsigned int ReturnOpenThreadsAsBitmask(void)
	{
		unsigned int Retmask=0;
		for (threadhandle_t i=0;i<MAXTHREADS;i++)
		{
			Retmask<<=1;
			if (IsThreadOpen(i))
				Retmask |=1;
		}
		return Retmask;
	}


	void CloseThreadsByBitmask(unsigned int Bitmask)
	{

		for (threadhandle_t i=1;i<MAXTHREADS;i++)
		{
			Bitmask<<=1;
			if (Bitmask & 0x80000000)
			{
				MT_CloseDriverThread (i);
			}

		}
	}


	void CloseMutexsByBitmask(unsigned int Bitmask)
	{
		for (int i=0;i<MAX_MUTEX;i++)
		{
			if (Bitmask & 0x80000000)
				MT_CloseMutex(&MutexPool[i]);
			Bitmask<<=1;

		}

	}

	void CloseEventsByBitmask(unsigned int Bitmask)
	{
		for (int i=0;i<MAX_MUTEX;i++)
		{
			if (Bitmask & 0x80000000)
				MT_CloseEvent(&EventPool[i]);
			Bitmask<<=1;

		}

	}
	unsigned int ReturnOpenMutexsAsBitmask(void)
	{
		return OpenMutexsMask;
	}

	unsigned int ReturnOpenEventsAsBitmask(void)
	{
		return OpenMutexsMask;
	}

	// DEBUG and information

	unsigned int ReportDriverThreadUnusedStack(threadhandle_t hndl=0)
	{
		int i;
		for ( i=0;i<Threadpointers.Pcbs[hndl]->StackSize/sizeof(int);i++)
			if (Threadpointers.Pcbs[hndl]->StackBase[i]!=0xffffffff)
				break;
		return (i*sizeof(int));
	}





	void print_thread_info(void)
	{
		int		i;
		int *		p;
		unsigned long		thrdfnc;
		char		tmp[64];

		PrintThreadResourceInfo();
		snprintf(tmp, sizeof(tmp), "Ready Q:%08X  Timer %08X Current: %lX \r\n",
		         Threadpointers.bitmask, Threadpointers.TimerWaitQ, Threadpointers.CurrentThread);
		xputstring(tmp);

		for  (i = 0;  i < MAXTHREADS;  i++)
		{
			if (Threadpointers.Pcbs[i])
			{
				thrdfnc=(unsigned long) (Threadpointers.Pcbs[i]->ThreadFuncPtr);
				snprintf(tmp, sizeof(tmp), "Hndl:%X\tPCB:%X:\tFnc:%lX\r\n",
				         i, (int) (Threadpointers.Pcbs[i]),thrdfnc);
				xputstring(tmp);
				snprintf(tmp, sizeof(tmp), "\tParent:%lX\tChldrn:%08X\r\n",
				         (Threadpointers.Pcbs[i]->MyParent), Threadpointers.Pcbs[i]->ChildThreadsMask);
				xputstring(tmp);
				snprintf(tmp, sizeof(tmp), "\tMutexs:%08X\tEvents:%08X\tTimer %X\r\n",
				         Threadpointers.Pcbs[i]->MyOpenMutexsMask, Threadpointers.Pcbs[i]->MyOpenEventsMask,
				         Threadpointers.Timers[i]);
				xputstring(tmp);

				p = (int *) Threadpointers.Pcbs[i];
				snprintf(tmp, sizeof(tmp), "\t   SR:    %08x   LR:    %08x   R16:   %08x\r\n", p[0], p[1], p[18]);
				xputstring(tmp);
				DumpRegisters((unsigned int *) &p[2], "\t");
				snprintf(tmp, sizeof(tmp), "\tUnused Stack %X\r\n\r\n", ReportDriverThreadUnusedStack(i));
				xputstring(tmp);
			}
		}
	}


	void PrintThreadResourceInfo(void)
	{
		char temp[128];
		sprintf(temp,"Threads :%X  Mutexs:%X Events:%X Threads chk: %X\r\n",
		        OpenThreadsMask,OpenMutexsMask,OpenEventsMask,ReturnOpenThreadsAsBitmask());
		xputstring(temp);
	}


	// thread test functions
#ifdef THREAD_TEST
 #define  TESTPRIORITY 6

	CONTEXTBUF buf;

	void					ThreadFunction(void *);
	void					ThreadFunction2(void *);
	void					ThreadFunction3(void *);
	void					ThreadFunction4(void *);
	void					ThreadFunction5(void *);
	void					ThreadFunction6(void *);
	void 					ThreadFunction7(void *);
	void 					ThreadFunction8(void *);


	void					test_func(unsigned char ch);


	//thread test variables:
	volatile unsigned long int x=0;
	MT_Mutex_t mtx;
	MT_Event_t evnt;
	threadhandle_t hndl;
	threadhandle_t hndl1;
	threadhandle_t hndl2;
	volatile bool k1=false;
	volatile bool k2 = false;


	void test_func(unsigned char ch)
	{
		char buf[100];
		switch (ch)
		{
		case 'w':		//test set/get context
			hndl1=MT_OpenDriverThread(TESTPRIORITY+1,2000,ThreadFunction8,NULL);
			WakeDriverThread(hndl1);
			hndl2=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction7,NULL);
			WakeDriverThread(hndl2);
			xputstring("Leaving test \r\n");
			break;

		case 'x':		// simple mutex test
			mtx =MT_CreateMutex(false);
			hndl=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction,NULL);
			if (hndl)
				WakeDriverThread(hndl);
			hndl=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction2,NULL);
			if (hndl)
				WakeDriverThread(hndl);
			while (!( k1 && k2));
			sprintf(buf,"::%lu\r\n",x);
			xputstring(buf);
			MT_CloseMutex(mtx);
			k1=k2=false;
			break;

		case 'y':		// simple event test
			evnt =MT_CreateEvent(false);
			hndl=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction3,NULL);
			if (hndl)
				WakeDriverThread(hndl);
			hndl=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction4,NULL);
			if (hndl)
				WakeDriverThread(hndl);
			MT_SetEvent(evnt);
			while (!( k1 && k2));
			sprintf(buf,"::%lu\r\n",x);
			xputstring(buf);
			MT_CloseEvent(evnt);
			k1=k2=false;
			break;

		case 'z':
			hndl=MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction5,NULL);
			if (hndl)
				WakeDriverThread(hndl);
			break;

		}
	}


	void ThreadFunction3(void *)
	{
		xputstring("Entered1\r\n");
		MT_WaitEvent(evnt,INFINITE);
		xputstring("Event signalled\r\n");
		k1=true;
	}



	void ThreadFunction4(void *)
	{
		xputstring("Entered2\r\n");
		MT_WaitEvent(evnt,INFINITE);
		xputstring("Event signalled2\r\n");
		k2=true;

	}


	void	ThreadFunction5(void *)
	{
		MT_OpenDriverThread(TESTPRIORITY,2000,ThreadFunction6,NULL);

		xputstring("entered 1\r\n");
		while (true)
		{
			MT_Delay(1500);
			Click();
		}
	}

	void	ThreadFunction6(void *)
	{
		while (true)
		{
			xputstring("entered 2\r\n");
			MT_Delay(1000);
			xputstring("exit 2\r\n");
			Click();
			Click();
		}

	}


	void ThreadFunction2(void *)
	{
		int i;
		xputstring("Entered1\r\n");
		for (i=0; i<100;i++)
		{
			if (MT_WaitMutex(mtx,INFINITE))
			{
				xputstring("1\r\n");
				unsigned long int temp =x;
				MT_Delay(temp);
				x=temp +1;
				MT_ReleaseMutex(mtx);
			}
			else
				xputstring("failed 1\r\n");
			MT_Delay(x/20);

		}
		k1=true;
		xputstring("one done\r\n");

	}


	void ThreadFunction(void *)
	{
		xputstring("Entered2\r\n");

		for (int i=0; i<100;i++)
		{
			if (MT_WaitMutex(mtx,INFINITE))
			{
				xputstring("2\r\n");
				unsigned long int temp = x ;
				MT_Delay(temp&7);
				x=temp + 1;
				MT_ReleaseMutex(mtx);
			}
			else
				xputstring("failed 2\r\n");

			MT_Delay(2*x&7);

		}
		k2=true;
		xputstring("two done\r\n");

	}
	void ThreadFunction7(void *)
	{
		static volatile bool initdone =false;
		xputstring("Entered1\r\n");
		MT_GetContext(buf);
		if (!initdone)
		{
			xputstring(" get context \r\n");
			initdone=true;
			while (true)
				RESET_WATCHDOG_TIMER();
		}
		xputstring("SetContext\r\n");
		MT_Delay(1000);
	}
	void ThreadFunction8(void *)
	{
		xputstring("Entered2\r\n");
		for (int i=10; i>0;i--)
		{
			xputstring ("loop\r\n");
			MT_Delay(1000);
			if (i==5)
				MT_SetContext(hndl2,buf);
		}
		xputstring("Done \r\n");
	}


#else
	void test_func(unsigned char )
{}
#endif
}


