//======================================================================================
//	Revision information
//======================================================================================
// $RCSfile: app_kernel/thread.h $
// $Revision: 1.31.1.8 $
// $Date: 2010/03/03 19:06:45EST $
//======================================================================================
#ifndef THREAD_H
#define THREAD_H
extern "C"
{

#define	MAXTHREADS	(threadhandle_t)(8*sizeof(unsigned int))
#define NEW_THREAD 1
	/*-----------------5/10/2002 1:07PM-----------------
	 * this file contains prototypes for functions implementing 2 threading systems
	 * a copreative second application thead implemented with  MT_OpenThread,MT_SwapThread
	 * and MT_Exit_Close_Thread
	 * and a preememting thread used by the driver routines  implemented with MT_OpenDriverThread
	 * MT_CloseDriverThread  and MT_DriverThreadSleep
	 *  all extern "C" functions are implemented in file threadasm.s  the preemptive switch is
	 * implemented in db_start.s as part of the normal irq routine
	 * --------------------------------------------------*/
#define IFACE_PRIORITY 8
#define SCAN_PRIORITY 27
#define FASTDECODE_PRIORITY 2
#define ILLCONTROL_PRIORITY 26
#define STACK_CB_PRIORITY 25
#define STACKPRIORITY 16
#define STACK_THREAD_PRIORITY_MASK		((0xff<<((MAXTHREADS-8) - STACKPRIORITY))+ (1<<((MAXTHREADS-1) - STACK_CB_PRIORITY)))
#define MAX_MUTEX 32
#define MAX_EVENT 32
#define WAIT_TIMED_OUT	-1					// wait timout return value for wait multiple events
#define ERR_BAD_ARG	-2						// bad argument(bad number of event return value for wait multiple events
#ifndef WINSIM	                           // a simulated debug environement with MSVC
#define INFINITE 0xffffffff
#endif
	typedef unsigned long threadhandle_t;
	typedef struct EventStruct_tag  * MT_Event_t;
	typedef struct MutexStruct_tag * MT_Mutex_t;
	typedef void *MT_Mailbox_t;

	// Initialize everything the first time
	void InitializeTheadSystem(void **);
	// this function updates timers for sleeping threads and updates the Ready Q (bitmask)
	void ServiceThreadTimers(void);



	//close a driver thread
	void MT_CloseDriverThread (threadhandle_t);

	// put this thread to sleep and run next highest priority
	void SleepDriverThread(void);

	// wake a particular thread up (put it on ready Q]
	void WakeDriverThread(threadhandle_t thread);


	// open a thread with at least priority priority
	threadhandle_t MT_OpenDriverThread(threadhandle_t priority,int StackSize,void (*ThreadPtr)(void * ),void * ptr);
	// put this thread to sleep for MilliSeconds MilliSeconds
	void MT_Delay(unsigned long MilliSeconds);

#ifndef WINSIM	                           // a simulated debug environement with MSVC6
	// return handle of current thread
	threadhandle_t MT_GetCurrentThread(void);
#endif

	// is this handle valid ?
	bool IsThreadOpen(threadhandle_t hndl);








	/* The following function is responsible for creating an actual      */
	/* Mutex (Binary Semaphore).  The Mutex is unique in that if a       */
	/* Thread already owns the Mutex, and it requests the Mutex again    */
	/* it will be granted the Mutex.  This is in Stark contrast to a     */
	/* Semaphore that will block waiting for the second acquisition of   */
	/* the Sempahore.  This function accepts as input whether or not     */
	/* the Mutex is initially Signalled or not.  If this input parameter */
	/* is TRUE then the caller owns the Mutex and any other threads      */
	/* waiting on the Mutex will block.  This function returns a NON-NULL*/
	/* Mutex Handle if the Mutex was successfully created, or a NULL     */
	/* Mutex Handle if the Mutex was NOT created.  If a Mutex is         */
	/* successfully created, it can only be destroyed by calling the     */
	/* MT_CloseMutex() function (and passing the returned Mutex        */
	/* Handle).                                                          */

	// open a mutex
	MT_Mutex_t MT_CreateMutex(bool CreateOwned);

	/* The following function is responsible for waiting for the         */
	/* specified Mutex to become free.  This function accepts as input   */
	/* the Mutex Handle to wait for, and the Timeout (specified in       */
	/* Milliseconds) to wait for the Mutex to become available.  This    */
	/* function returns TRUE if the Mutex was successfully acquired and  */
	/* FALSE if either there was an error OR the Mutex was not           */
	/* acquired in the specified Timeout.  It should be noted that       */
	/* Mutexes have the special property that if the calling Thread      */
	/* already owns the Mutex and it requests access to the Mutex again  */
	/* (by calling this function and specifying the same Mutex Handle)   */
	/* then it will automatically be granted the Mutex.  Once a Mutex    */
	/* has been granted successfully (this function returns TRUE), then  */
	/* the caller MUST call the MT_ReleaseMutex() function.            */
	/* * NOTE * There must exist a corresponding MT_ReleaseMutex()     */
	/*          function call for EVERY successful MT_WaitMutex()      */
	/*          function call or a deadlock will occur in the system !!! */

	bool MT_WaitMutex(MT_Mutex_t Mutex, unsigned long Timeout);

	/* The following function is responsible for releasing a Mutex that  */
	/* was successfully acquired with the MT_WaitMutex() function.     */
	/* This function accepts as input the Mutex that is currently        */
	/* owned.                                                            */
	/* * NOTE * There must exist a corresponding MT_ReleaseMutex()     */
	/*          function call for EVERY successful MT_WaitMutex()      */
	/*          function call or a deadlock will occur in the system !!! */

	void MT_ReleaseMutex(MT_Mutex_t Mutex);

	/* The following function is responsible for destroying a Mutex that */
	/* was created successfully via a successful call to the             */
	/* MT_CreateMutex() function.  This function accepts as input the  */
	/* Mutex Handle of the Mutex to destroy.  Once this function is      */
	/* completed the Mutex Handle is NO longer valid and CANNOT be       */
	/* used.  Calling this function will cause all outstanding           */
	/* MT_WaitMutex() functions to fail with an error.                 */

	void MT_CloseMutex(MT_Mutex_t Mutex);


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

	bool  MT_WaitEvent(MT_Event_t Event, unsigned long Timeout);


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

	int MT_WaitMultiEvents(unsigned int NumEvents,MT_Event_t* Events, unsigned long Timeout);


	/* The following function is responsible for changing the state of   */
	/* the specified Event to the Non-Signalled State.  Once the Event   */
	/* is in this State, ALL calls to the MT_WaitEvent() function will */
	/* block until the State of the Event is set to the Signalled State. */
	/* This function accepts as input the Event Handle of the Event to   */
	/* set to the Non-Signalled State.                                   */

	void MT_ResetEvent(MT_Event_t Event);

	/* The following function is responsible for changing the state of   */
	/* the specified Event to the Signalled State.  Once the Event is in */
	/* this State, ALL calls to the MT_WaitEvent() function will       */
	/* return.  This function accepts as input the Event Handle of the   */
	/* Event to set to the Signalled State.                              */

	void MT_SetEvent(MT_Event_t Event);

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

	MT_Event_t  MT_CreateEvent(bool CreateSignalled);
	/* this version is the same as above but provides auto reset function*/

	MT_Event_t  MT_CreateEvent_Ex(bool CreateSignalled,bool AutoReset);


	/* The following function is responsible for destroying an Event that*/
	/* was created successfully via a successful call to the             */
	/* MT_CreateEvent() function.  This function accepts as input the  */
	/* Event Handle of the Event to destroy.  Once this function is      */
	/* completed the Event Handle is NO longer valid and CANNOT be       */
	/* used.  Calling this function will cause all outstanding           */
	/* MT_WaitEvent() functions to fail with an error.                 */


	void MT_CloseEvent(MT_Event_t Event);



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
	MT_Mailbox_t  MT_CreateMailbox(unsigned int NumberSlots, unsigned int SlotSize);

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
	int  MT_AddMailbox(MT_Mailbox_t Mailbox, void *MailboxData);

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
	int   MT_WaitMailbox(MT_Mailbox_t Mailbox,unsigned long timeout, void *MailboxData);

	/* The following function is responsible for destroying a Mailbox    */
	/* that was created successfully via a successful call to the        */
	/* MT_CreateMailbox() function.  This function accepts as input    */
	/* the Mailbox Handle of the Mailbox to destroy.  Once this function */
	/* is completed the Mailbox Handle is NO longer valid and CANNOT be  */
	/* used.  Calling this function will cause all outstanding           */
	/* MT_WaitMailbox() functions to fail with an error.               */
	void  MT_DeleteMailbox(MT_Mailbox_t Mailbox);




	/* CONTEXTBUF buffer used in MT_GetContext() and MT_SetContext()			*/
	/* below. user must declare a variable of this type (usually global		*/
	/* to use those functions 																*/
	typedef unsigned int CONTEXTBUF[20];


	/* This function saves the context opf the current thread in  a buffer 	*/
	/* supplied as the first parameter to the function this buffer can then */
	/*be used to resume execution of this thread at the return point of 		*/
	/* this function																			*/

	void MT_GetContext(CONTEXTBUF buf);

	/* MT_SetContext Set the context of a thread to some state it set previously 	*/
	/* first parameter is the thread to set the contest of. 2nd parameter is the	*/
	/* buffer containing the context, this buffer should have been passed to 		*/
	/* MT_GetContext() earlier. If sucsessful MT_SetContext causes the thread		*/
	/* that called MT_GetContext to resume execution from the point that				*/
	/* MT_GetContext returned from																*/
	/* This function returns false indicating a failure if the thread is invalid 	*/
	/* It does not mattch the context saved i,e you cant set one thread to the 	*/
	/* saved by another thread																		*/

	bool MT_SetContext(threadhandle_t thr,CONTEXTBUF buf);


	// print wordy thead infomation
	void print_thread_info(void);
	// print summary information ie how many threads /mutexs etc are being used
	void PrintThreadResourceInfo(void);


	// debug only not official
	//unsigned int ReportDriverThreadUnusedStack(void);
	unsigned int ReportDriverThreadUnusedStack(threadhandle_t);
	unsigned int ReturnOpenThreadsAsBitmask(void);
	unsigned int ReturnOpenMutexsAsBitmask(void);
	unsigned int ReturnOpenEventsAsBitmask(void);

	void CloseThreadsByBitmask(unsigned int Bitmask);
	void CloseEventsByBitmask(unsigned int Bitmask);
	void CloseMutexsByBitmask(unsigned int Bitmask);
}
#endif
