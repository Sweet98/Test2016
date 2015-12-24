;/************************************************************************/
;//
;// HHP 4480 embedded firmware
;//
;//  Shared assembler module for boot and app code
;//  Vector table and common functions included
;//  All shared code for interrupt setup (entry/exit) is handled here
;//
;//  Date:       4/23/02
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
;//  Target:     Dragonball Development Board
;//
;//======================================================================================
;//	Revision information
;//======================================================================================
;// $RCSfile: db_boot/vectors.s $
;// $Revision: 1.9 $
;// $Date: 2009/10/07 17:12:27EDT $
;
     INCLUDE db_memap.s
     INCLUDE db_hdw.s

     REQUIRE8
     PRESERVE8

   
     IMPORT	Reset_Handler
     IMPORT	UndefinedHandler
     IMPORT	C_SWI_Handler
     IMPORT	PrefetchHandler
     IMPORT	AbortHandler
     IMPORT	IRQHandler
     IMPORT	FIQHandler
     IMPORT	memcpy

	 IMPORT MMU_CleanAndInvalidateBothCaches
	 
  ;   EXPORT	SwitchToAsync
  ;   EXPORT	EnableFIQ
  ;  EXPORT	DisableFIQ
  ;   EXPORT	EnableIRQ
  ;   EXPORT	DisableIRQ
  ;   EXPORT	IRQ_Disabled
     EXPORT	Run_op_code
     EXPORT	Run_application_code
	 EXPORT Vector_Init_Block



;; Setup interrupt/exception vectors

;; --- Define entry point
;; --- Interrupt vector table must be put at 0, so put them here
;; --- could have used B instructions, but then I would have been restricted to 32M branching
;; --- long term, these interrupt handlers will want to be in RAM, not in ROM... so way
;; --- out of the 32M reference area


;; New Entry --
;;   New style says let's use a system that allows us to go anywhere in the mem model right from here, this style change is also being
;;   supported by a change in how boot code and app code work together.  That change imagines having the MMU dual map the external system
;;   RAM over 0x0 as control is handed over.  The advangates of this change are mostly that no ROM hits penalize IRQs and FIQs -- which
;;   implies no speed penalty as well as no need to keep interrupts off when writing Flash.

   AREA Vector,CODE,READONLY


     ENTRY

;;Define vector table
;;
Vector_Init_Block
     B		Reset_Handler
     LDR	PC, Undefined_Addr
     LDR	PC, SWI_Addr
     LDR	PC, Prefetch_Addr
     LDR	PC, Abort_Addr
     NOP
     LDR	PC,IRQ_Addr
     LDR	PC,FIQ_Addr

;;Set pointers to vector routines.  Instructions above use these to reach anywhre in memory
;;
Reset_Addr       DCD  Reset_Handler
Undefined_Addr   DCD  Undefined_Handler
SWI_Addr         DCD  SWI_Handler
Prefetch_Addr    DCD  Prefetch_Handler
Abort_Addr       DCD  Abort_Handler
                 DCD  0                    ;; align throw away with throw away vector above
IRQ_Addr         DCD  IRQ_Handler
FIQ_Addr         DCD  FIQ_Handler

	include 	MatrixBootHeader.i  		;Matrix Boot Header location, included here so it is defined close to the Flash Starting Address

	
;; ************************
;; Day 1 Exception handlers... anything you need to do in assembly, do it here and leave to do everything else
;; in C
;; **** Note, the Dragonball has what they refer to as an AITC which allows for handling interrupt vectors somewhat more flexibly than
;; the implied ARM 0x0-0x0000001c vector table approach.  After several reads of that alternative approach, it seems that using that approach
;; will allow us to avoid for sure the ROM accesses AND the redirect upon redirect approach for IRQ handling.   At the same time, this implies a slight
;; learning curve and to be ABSOLUTELY honest, I am not sure it gives us THAT much of an advantage to complicate the architecture, which it
;; clearly implies doing.  Either way, for starters, it has been agreed as we had a model for this in the StrongArm Img8k design, to use that first
;; and then climb the learning curve...
;;
;; so for those viewing this code, the way this works is as follows:
;;  (1) True ARM core vector is immediately above (THESE HAVE TO BE DEFINED @ 0)
;;  (2) Those vectors jump to the routines below, which do the necessary assembly wrapper details, if necessary (the vector above may jump direct)
;;  (3) Boot code has C (_irq, etc) functions that do the necessary things per type of suggestion to return appropriately
;;  (4) Those boot code special function redirect to an op code vector table if magic number is set.
;;  (5) The op code vector table, defined similar to the above table @ Code space offset of 0 (0x08000000) then does the same thing as (1) and so on
;;  (6) Noting that the C functions for interrupt handling in operational code DO NOT need to be special functions with regard to returning as this
;;         is all handled by the boot C functions.
;;
;; That's it in a nutshell, let's see if this works ****
;;
;; As a side note, this model may be just fine for long term, using FIQ for time sensitive things and doing that interrupt routine in boot code.
;; This is just a thought, as using the AITC seems to rely on quite a few other details to even imagine running (MMU redirects, etc...)
;; ************************

;; here is the actual initial redirect table functions
;; The way these exception handlers work today (and they do NOT have to) is that the exception table (which vectors to the functions
;; listed below) always calls a BOOT ROM exception handler.  Those handlers based on the state of the magic # (that which tells
;; the working system whether code is running out of boot or operational code) to determine if an additional redirect/call is
;; necessary to call the associative operational code exception.  Ideally, this redirect extra step can be removed to save time
;;


Undefined_Handler
     mov   	r0,lr     ; send address of instruction causing exception
     sub   	r0,r0,#4
     bl    	UndefinedHandler



;;SWIHandler
;;
SWI_Handler
     stmfd		sp!,{r0-r12,lr}	; store registers
     ldr		r0,[lr,#-4]       ; calculate address of SWI instruction ...
     bic		r0,r0,#0xff000000	; mask off to get SWI number
     mov		r1,lr     			; send address of SWI instruction as well
     sub		r1,r1,#4
     bl			C_SWI_Handler     ; call C routine to actually handle this
     ldmfd 		sp!,{r0-r12,pc}^	; restore and return


Prefetch_Handler
     mov		r0,lr     			; send address of instruction causing exception
     sub  		r0,r0,#4
     b			PrefetchHandler


Abort_Handler
     mov		r0,lr     			; send address of instruction causing exception
     sub		r0,r0,#8
     b    		AbortHandler


IRQ_Handler
     sub		lr, lr, # 4
     stmfd		sp!,{r0-r12,lr}	;;;;@ save / restore registers used in C functions
     bl			IRQHandler
     ldmfd		sp!,{r0-r12,pc}^


FIQ_Handler
     sub		lr, lr, # 4
     stmfd		sp!,{r0-r12,lr}	;;;;@ save / restore registers used in C functions
     bl			FIQHandler
     ldmfd		sp!,{r0-r12,pc}^


 
	
	  AREA IRQ_EN_ASYNC_MODE,CODE,READONLY

;; Switch to async mode using coprcessor 15 control reg 1
;;
SwitchToAsync
     ldr		r0,=MPCTL0
     ldr		r1,=MPCTL0_SETTING 		;; value for register
     str		r1,[r0]
     mrc		p15, 0, r0, c1, c0, 0
     orr		r0,r0,#0xc0000000		;; mask the upper 2 bits
     mcr		p15, 0, r0, c1, c0, 0      	;; write it back
     bx			lr


EnableFIQ
     mrs		r0,cpsr       ;; read/write to clear FIQ bit ONLY, that enables
     bic		r0,r0,#F_Bit
     msr		cpsr_c,r0
     bx			lr


DisableFIQ
     mrs		r0,cpsr			;; read/write to set FIQ bit ONLY, that disables
     orr		r0,r0,#F_Bit
     msr		cpsr_c,r0
     bx			lr


;; MUST be called from a priviledged mode -- other than USER
;;
EnableIRQ
     mrs		r0,cpsr			;; read/write to clear IRQ bit ONLY, that enables
     bic		r0,r0,#I_Bit
     msr		cpsr_c,r0
     bx			lr


DisableIRQ
     mrs 		r0,cpsr       ;; read/write to set IRQ bit ONLY, that disables
     orr 		r0,r0,#I_Bit
     msr 		cpsr_c,r0
     bx			lr


IRQ_Disabled
     mrs		r0,cpsr
     and 		r0,r0,#0x80
     bx			lr


	  AREA RUN_OP_CODE,CODE,READONLY

Run_op_code
     ldr 	r0,=RAMStartOpCode    ; run op code that has been downloaded, NOTE: this will only really run op code when boot code calls it
	 mov	r1,r0

;void Run_application_code( void *AddressToRelocateApplicationToBeforeExecuting, void *AddressOfApplication, int SizeOfApplication )
;;
Run_application_code
;	If the address where the application currently is doesn't match the address where it is supposed to be executed from, relocate it.
	stmfd		sp!,{r0,r1}	;save run address on stack 
	cmp			r0,r1
	beq			Run_application_code_Execute
;;	Relocate the application before executing.  Parameters are already setup correctly, so all we need to do is call the routine.
	bl			memcpy	
Run_application_code_Execute
	bl			MMU_CleanAndInvalidateBothCaches
	ldmfd		sp!,{r0,r1}
	bx r0
 	END


