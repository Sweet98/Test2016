;/************************************************************************/
;//
;// HIM 44xx embedded firmware
;//
;//  Op Code Startup -- Threads and Interupts --
;//  Date:       7/21/08
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1, Metaware, RVCT Build 674
;//  Target:     Dragonball Development Board
;//
;//======================================================================================
;//	Revision information
;//======================================================================================
;// $RCSfile: app_kernel/threads_interupts.s $
;// $Revision: 1.17 $
;// $Date: 2011/04/19 10:45:25EDT $
;//======================================================================================

	REQUIRE8
	PRESERVE8


	INCLUDE db_hdw.s    ; definitions of dragonball registers, etc
	INCLUDE mmumacro.s


	AREA threads_interupts, CODE, READONLY

;DOG DEBUG added to MAKEFILE. UNCOMMENT IT THERE.
;GBLL	WDOGDEBUG
;WDOGDEBUG	SETL	{TRUE}


	IMPORT	Reset_Handler
	IMPORT	register_save
	IMPORT	savedpsr
	IMPORT	UndefinedHandler
	IMPORT  C_SWI_Handler
	IMPORT  PrefetchHandler
	IMPORT  AbortHandler
	IMPORT	Threadpointers
	IMPORT	UserIsrVectors
	IMPORT  MT_CloseCurrentThread

	IF :DEF: WDOGDEBUG
	IMPORT	WDogAddr
	ENDIF ;:DEF: WDOGDEBUG

	IF :DEF: ADDTRACE
	IMPORT	ReturnAddr
	ENDIF	;:DEF: ADDTRACE

	EXPORT 	Vector_Init_Block

	EXPORT 	MT_DriverThreadExit
	EXPORT 	MT_GetContext

	EXPORT 	WaitForIRQ
	EXPORT 	EnableIRQ
  	EXPORT 	DisableIRQ
	EXPORT 	IRQ_Disabled

	EXPORT 	Get_SP
	EXPORT	ForceDataAbort
	EXPORT	ForceIllegalInstructiion
	EXPORT	ForcePrefetchAbort



;;
;;some defines for use with theading offsets must match those in thread.h
;;
	MAP	0

sv_psr 			FIELD 4
sv_restartaddr		FIELD 4
sv_r0	 		FIELD 4
sv_r1 			FIELD 4
sv_r2 			FIELD 4
sv_r3 			FIELD 4
sv_r4 			FIELD 4
sv_r5 			FIELD 4
sv_r6 			FIELD 4
sv_r7 			FIELD 4
sv_r8 			FIELD 4
sv_r9 			FIELD 4
sv_r10 			FIELD 4
sv_r11 			FIELD 4
sv_r12 			FIELD 4
sv_r13 			FIELD 4
sv_r14 			FIELD 4
StackBase 		FIELD 4
StackSize 		FIELD 4


;;
;;c equivalent to above. This is what is in thread .h
;;
;;typedef struct
;;{
;;	unsigned int 	sv_psr;
;;	unsigned int 	sv_restartaddr;
;;	unsigned int 	sv_r0;
;;	unsigned int 	sv_r1;
;;	unsigned int 	sv_r2;
;;	unsigned int 	sv_r3;
;;	unsigned int 	sv_r4;
;;	unsigned int 	sv_r5;
;;	unsigned int 	sv_r6;
;;	unsigned int 	sv_r7;
;;	unsigned int 	sv_r8;
;;	unsigned int 	sv_r9;
;;	unsigned int 	sv_r10;
;;	unsigned int 	sv_r11;
;;	unsigned int 	sv_r12;
;;	unsigned int 	sv_r13;
;;	unsigned int 	sv_r14;
;;	int * 		StackBase;
;;	int		StackSize;
;;	threadhandle_t	prev;
;;	int		next;
;;}PcbType;




;;
;; Setup interrupt/exception vectors
;;
;; New Entry --
;;   Old style below did a branch to an assembly handler and went from there
;;   New style says let's use a system that allows us to go anywhere in the mem model right from here, this style change is also being
;;   supported by a change in how boot code and app code work together.  That change imagines having the MMU dual map the external system
;;   RAM over 0x0 as control is handed over.  The advangates of this change are mostly that no ROM hits penalize IRQs and FIQs -- which
;;   implies no speed penalty as well as no need to keep interrupts off when writing Flash.
;;
;; Otherwise, copy a sequence of LDR PC instructions over the vectors
;; (Note: Copy LDR PC instruction because branch instructions
;; could not be simply copied, the offset in the branch instruction
;; would have to be modified so that it branched into ROM. Also, a
;; branch instruction might not reach if the ROM is at an address
;; >32MB).
;;
;; Note, I am not sure how this wants to work, most significantly who is going to do the copy of the block
;; In other words, if this is the start of boot code, this block will try and copy it to 0 and then proceed.  This is ok, but for the
;; fact that it can't actually do that as 0x0 is Flash ROM.  So essentially a reset will try and do something it can't do and be on it's
;; way, because as it gets to the first ldr pc of the actual vector it will jump to the right reset vector. Again, not sure how this
;; will be handled long term, but it should work -- by mistake -- from scratch
;;
;; use there example, but avoid using r9 as it may be used for relocateability
;; move vector table from some other location to virtual location 0x0
;; this will essentially move the vector table (8 words) and the associative pointers (8 words immediately following) that are used
;;

	ENTRY

;;
;;Define vector table
;;
Vector_Init_Block
	LDR	PC, Reset_Addr
	LDR	PC, Undefined_Addr
	LDR	PC, SWI_Addr
	LDR	PC, Prefetch_Addr
	LDR	PC, Abort_Addr
	NOP
	LDR	PC,IRQ_Addr
	LDR	PC,FIQ_Addr



;;
;;Set pointers to vector routines.  Instructions above use these to reach anywhre in memory
;;
Reset_Addr		DCD	Reset_Handler
Undefined_Addr	DCD	Undefined_Handler
SWI_Addr		DCD	SWI_Handler
Prefetch_Addr	DCD	Prefetch_Handler
Abort_Addr		DCD	Abort_Handler
				DCD	0			;;For allignment throw away, throw away with NOP vector above
IRQ_Addr		DCD	IRQ_Handler
FIQ_Addr		DCD	FIQ_Handler



	IF ( platform_current != "imx25" )
	INCLUDE 	matrix_app_header.i  		;Matrix App Header location, included here so it is defined close to the flash application starting address
	ENDIF ;; endof IF ( platform_current != "imx25" )



;;
;; here is the actual initial redirect table functions
;; The way these exception handlers work today (and they do NOT have to) is that the exception table (which vectors to the functions
;; listed below) always calls a BOOT ROM exception handler.  Those handlers based on the state of the magic # (that which tells
;; the working system whether code is running out of boot or operational code) to determine if an additional redirect/call is
;; necessary to call the associative operational code exception.  Ideally, this redirect extra step can be removed to save time
;;



;;
;; Switch_Context macro for context change in privilaged modes
;;
	MACRO
	Switch_Context
	ldmia	r1!,{r12,r14}			;; saved psr and  restart address
	msr		SPSR_fsxc,r12			;; saved psr in r12 into spsr
	ldmia	r1,{r0-r14}^			;; get user mode regiters from pcb
	nop							  	;; see arm doc cant do move /restore after user reg accsess
	movs	pc,lr
	MEND


	IF {CPU}="ARM926EJ-S"
	;;
;; GetThreadPtrs from hash table use clz for arm 9e v5
;;
	MACRO
	GetThreadPtrs
	sub		r1,r3,#1				;;
	bic		r1,r3,r1				;;r1 has bit set of highest pri poc (And of R3 and complement of R1, stored in R1)
	clz		r3,r1					;;
	str		r3,[r2],#8				;; store new index make r2 point to pcb pointer array
	ldr		r1,[r2,r3,LSL #2]	   	;; this is a pointer to the thread we want to swich to
	ldr		r0,[r2,r0,LSL #2]		;; r0 points to our thread
	MEND

	ELSE

;;
;; GetThreadPtrs from hash table
;;
	MACRO
	GetThreadPtrs
	sub		r1,r3,#1				;;
	bic		r1,r3,r1				;;r1 has bit set of highest pri poc (And of R3 and complement of R1, stored in R1)
	rsb		r1,r1,r1,lsl #16		;; reverse subtract r1 from r1 logically shifted left #16
	add		r1,r1,r1,lsl #6
	add		r1,r1,r1,lsl #4			;; r1 has hash index of new thread
	adr		r3,contab				;;
	ldrb	r3,[r3,r1, lsr #26]		;; r3 has index of new thead
	str		r3,[r2],#8				;; store new index make r2 point to pcb pointer array
	ldr		r1,[r2,r3,LSL #2]	   	;; this is a pointer to the thread we want to swich to
	ldr		r0,[r2,r0,LSL #2]		;; r0 points to our thread
	MEND
	ENDIF



;;
;; Undefined_Handler vector routine
;;
Undefined_Handler
	stmfd	sp!,{r0-r12,lr}
	stmfd	sp!,{r14}
	ldr		r14,=register_save
	stmia	r14,{r0-r14}^
	nop
	ldr		r14,=savedpsr
	mrs		r12,spsr					;; threads status into r12
	str		r12,[r14]
	ldmfd	sp!,{r0}
	sub		r0,r0,#4
	bl		UndefinedHandler
	ldmfd	sp!,{r0-r12,pc}^



;;
;;SWI_Handler vector routine (software interupt)
;;
SWI_Handler
SWIHandler
	stmfd	sp!,{r0-r12,lr}
	stmfd	sp!,{r14}
	ldr		r14,=register_save
	stmia	r14,{r0-r14}^
	nop
	ldr		r14,=savedpsr
	mrs		r12,spsr			;; threads status into r12
	str		r12,[r14]
	ldmfd	sp!,{r1}
	sub		r1,r1,#4			;back up to instuction
	ldr		r0,[r1]       		;get SWI instruction ...
	bic		r0,r0,#0xff000000   ;mask off to get SWI number
	bl		C_SWI_Handler     	;call C routine to actually handle this
	ldmfd	sp!, {r0-r12,pc}^	; restore and return



;;
;;Prefetch_Handler vector routine
;;
Prefetch_Handler
	sub		lr,lr,#4		; When (and if) we return, go to the next instruction after the one that caused the problem
	stmfd	sp!,{r0-r12,lr}
	stmfd	sp!,{r14}
	ldr		r14,=register_save
	stmia	r14,{r0-r14}^
	nop
	ldr		r14,=savedpsr
	mrs		r12,spsr		;; threads status into r12
	str		r12,[r14]
	ldmfd	sp!,{r0}
	bl		PrefetchHandler
	ldmfd	sp!,{r0-r12,pc}^



;;
;; Abort_Handler vector routine
;;
Abort_Handler
	sub		lr,lr,#4		; When (and if) we return, go to the next instruction after the one that caused the problem
	stmfd	sp!,{r0-r12,lr}
	stmfd	sp!,{r14}
	ldr		r14,=register_save
	stmia	r14,{r0-r14}^
	nop
	ldr		r14,=savedpsr
	mrs		r12,spsr		;; threads status into r12
	str		r12,[r14]
	ldmfd	sp!,{r0}
	bl		AbortHandler
	ldmfd	sp!,{r0-r12,pc}^



;;
;;IRQ_Handler vector routine
;;
IRQ_Handler
	sub		lr,lr,#4				;; This is the return address
	stmfd	sp!,{r0-r12,lr}			;; be safe for now, do everything

	IF :DEF: ADDTRACE
	ldr		r1,=ReturnAddr
	str		lr,[r1]
	ENDIF	;:DEF: ADDTRACE
irq_loop
	ldr     r2,=(AITC_OFFSET+BASE_INT_REG+NIVESCR_REG_OFFSET)
	ldr     r1,[r2]
	bics	r1,r1,#0x0000ff00			;; invalid interrupt  return don't know why this happens
	ldr 	r2, =UserIsrVectors			;; get actual pointer
	bmi		noirq
	ldr		r0,[r2,r1,LSR #13]!	  		;; this should switch pc control to the NIVESCR vector interupt handler
	adr		lr,irq_loop               		;; have to do some shifty stuff as you can't bl to a non compiled function reference
	ldr		pc,[r2,#4] 					;; this should switch pc control to the NIVESCR vector interupt handler
noirq
	ldr		r2,=Threadpointers
	mrs		r1,cpsr
	mov		r7,r1
	orr		r1,r1,#0xC0 
	msr 		CPSR_c,r1					;; disable fast interrupts to protect the que from a fast interrupt doing signal
	ldr		r1,=0x80000000
	ldr		r3,[r2,#4]					;; r0 is current thread index   r1 is rpiority mask r3 is q
	orr		r3,r3,r1,lsr r0					;; set the bit for this process
	str		r3,[r2,#4]					;; store back the q
	msr		CPSR_c,r7					;; restore the state of the interrupts.
	
	ldr		r0,[r2]						;; temp test r0 is handle of current thread

	GetThreadPtrs						;; get pointer to thread to switch to in r1 and pointer to current thread in r0
	cmp			r0,r1
	ldmfdeq	sp!,{r0-r12,pc}^			;; no switch needed return
	mrsne		r12,spsr					;; threads status into r12
	strne		r12,[r0],#8         		;; save it in our pcb
	ldmfdne 	sp!,{r2,r3}        			;; saved r0/1 into r2/3
	stmiane		r0!,{r2,r3}         		;; save saved r0/1 and update pointer in current r0
	ldmfdne		sp!,{r2-r12,r14}			;; get rest of  interrupt saved registers
	strne		r14,[r0,#-12]				;; irq mode link reg  this is were is I came from it goes 2nd in pcb after psr
	stmiane		r0,{r2-r14}^				;; store rest of regs in pcb
	nop                    				;; see arm doc maybe we need this becuase of caret above

	Switch_Context
;;
;; FIQ_Handler vector routine
;;

FIQ_Handler
		sub		lr,lr,#4					;; if user routine returns FALSE (0)  we are not swiching threads just return
		stmfd	sp!,{r0-r3,lr}     			;; be safe for now, do everything
 IF :DEF: WDOGDEBUG
		ldr		r1,=WDogAddr
		str		lr,[r1]
 ENDIF
		ldr		r2,=AITC_OFFSET+BASE_INT_REG
		ldr		r1,[r2,#FIVESCR_REG_OFFSET]
		cmp		r1,#64						;; invalid interrupt  return don't know why this happens
		ldrlt	r2,=UserIsrVectors      	;; get actual pointer
		ldrlt	r0,[r2,r1,LSL #3]!  		;; this should switch pc control to the NIVESCR vector interupt handler
		movlt	lr,pc               		;; have to do some shifty stuff as you can't bl to a non compiled function reference
		ldrlt	pc,[r2,#4]  						;; this should switch pc control to the NIVESCR vector interupt handler
		ldmia	sp!,{r0-r3,pc}^     		;; restore

;;
;;***************************************************************************************
;; Thread support stuff
;;***************************************************************************************
;;

;;
;; MT_GetContext
;;
MT_GetContext
	str		r14,[r0,#(4*(2+14))]	;;save lr
	mrs		r14,cpsr				;; save this threads ststus
	str		r14,[r0]
	ldr		r14,=Threadpointers		;; get the current thread index
	ldr		r14,[r14]
	str		r14,[r0,#(4*(2+15))]	;;save current thread index
	add		r14,r0,#8				;;address to store registers at
	stmia	r14,{r0-r12}			;; store registers
	str		r13,[r14]
	ldr		pc,[r0,#(4*(2+14))]		;;return to caller



;;
;; MT_DriverThreadExit
;;
MT_DriverThreadExit
	msr		cpsr_c,#Mode_SVC:OR:I_Bit:OR:F_Bit	;  No interrupts ...switch modes so we can restore regs and return somewhere else
	bl		MT_CloseCurrentThread
	ldr		r2,=Threadpointers
	ldr		r3,[r2,#4]						;; r3 is q
	sub		r1,r3,#1						;;
	bic		r1,r3,r1						;; r1 has bit set of highest pri poc
	IF {CPU}="ARM926EJ-S"
	clz		r3,r1
	ELSE
	rsb		r1,r1,r1,lsl #16
	add		r1,r1,r1,lsl #6
	add		r1,r1,r1,lsl #4					;; r1 has hash index of new thread
	adr		r3,contab						;;
	ldrb	r3,[r3,r1, lsr #26]
	ENDIF									;; r3 has index of new thead
	str		r3,[r2],#8          			;; store new index make r2 point to pcb pointer array
	ldr		r1,[r2,r3,LSL #2]				;; this is a pointer to the thread we want to swich to

	Switch_Context


	IF {CPU}="ARM926EJ-S"
	ELSE
;;
;;contab hash table
;;
contab
		dcb	0, 31, 30, 19		;;0
		dcb	29, 25, 31, 18		;;04
		dcb	28, 31, 24, 31		;;08
		dcb	31, 31 ,31, 17		;;0c
		dcb	21, 27, 31, 31	 	;;10
		dcb	23, 31, 31, 6		;;14
		dcb	31, 31, 31, 31		;;18
		dcb	31, 10, 4, 16		;;1c
		dcb 0, 20, 26, 31		;;20
		dcb	31, 31, 31, 31		;;24
		dcb	22, 31, 31, 7		;;28
		dcb	31, 31, 11, 5		;;2c
		dcb	1, 31, 31, 31		;;30
      	dcb	31, 8, 31, 12		;;34
      	dcb	2, 31, 9, 13		;;38
      	dcb	3, 14, 15, 31		;;3c

		ALIGN
		ENDIF


;;
;;***************************************************************************************
;; Interrupt support stuff
;;***************************************************************************************
;;
;;


;;WaitForIRQ
;;	 Will get us into wait mode
;;
WaitForIRQ
	WRCP15_WaitInt		r0
	bx	lr



;;
;;EnableIRQ
;;	MUST be called from a priviledged mode -- other than USER
;;	so I will always call it from SVC mode
;;
EnableIRQ
	mov		r0, #Mode_SYS           ;; FOR NOW (causin I have being some issues read/writing this cpsr, let's have the enableIRQ enable both... no harm, no foul!! :OR:F_Bit    ; enabel IRQs in system mode -- assumes I was in a priviledged mode
	msr		cpsr_c, r0
	bx		lr



;;
;;DisableIRQ
;;
DisableIRQ
	mov		r0,#Mode_SYS:OR:I_Bit:OR:F_Bit  ; disable IRQs, FIQs in system mode
	msr		cpsr_c,r0
	bx		lr



;;
;;IRQ_Disabled
;;
IRQ_Disabled
	mrs		a1,cpsr
	and		a1,a1,#0x80
	bx		lr



;;
;;***************************************************************************************
;; Debugging support stuff for interupts
;;***************************************************************************************
;;
;;


;;
;;Get_SP
;;
Get_SP
	mov		r0,r13
	bx		lr


;;
;;ForceDataAbort
;;
ForceDataAbort
	mov		r0,#0
	ldr		r0,[r0,#-0x10]


;;
;;ForceIllegalInstructiion
;;
ForceIllegalInstructiion
;;	bkpt		0x1234
	nop						;; this keeps away waning about executing data
	dcd		0xe1212374 		;; above causes warning since we are arch4 this is an illegal instruction


;;
;;ForcePrefetchAbort
;;
ForcePrefetchAbort
	ldr		r0,=0xfffffff0
	bx		r0



	END

