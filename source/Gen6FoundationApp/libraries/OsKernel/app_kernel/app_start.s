;/************************************************************************/
;//
;// HHP 4480 embedded firmware
;//
;//  Op Code Startup -- C Init --
;//  Date:       1/28/02
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
;//  Target:     Dragonball Development Board
;//
;//======================================================================================
;//	Revision information
;//======================================================================================
;// $RCSfile: app_kernel/app_start.s $
;// $Revision: 1.9 $
;// $Date: 2009/12/21 16:26:34EST $
;//======================================================================================

;
;*|=======================================================================
; |			MODIFICATION HISTORY
; |=======================================================================

		REQUIRE8
		PRESERVE8


	INCLUDE db_memap.s
	INCLUDE db_hdw.s    ; definitions of dragonball registers, etc
	INCLUDE mmumacro.s


		AREA Start, CODE, READONLY


	IMPORT  Init_Uart1
	IMPORT	MMU_init
	IMPORT	C_Entry
	IMPORT  |Image$$RO$$Base|	; Start of ROM code (=start of ROM data)
	IMPORT  |Image$$RO$$Limit|    	; End of ROM code (=start of ROM data)
  	IMPORT  |Image$$RW$$Base|     	; Base of RAM to initialize
  	IMPORT  |Image$$RW$$Limit|	; End of RAM to initialize
  	IMPORT  |Image$$ZI$$Base|     	; Base and limit of area
  	IMPORT  |Image$$ZI$$Limit|    	; to zero initialize


	EXPORT 	Reset_Handler



		IF :DEF: ALLOW_BUS_MODE_FUNCTIONS
	EXPORT 	SwitchToAsync
	EXPORT Async_mode
	EXPORT FastBus_mode
	EXPORT GetCoreClockMode
	EXPORT RestoreCoreClockMode
	EXPORT GetCPSR
	EXPORT SetCPSR
		ENDIF	;:DEF: ALLOW_BUS_MODE_FUNCTIONS



; ************************
; Actual Start of Code under all conditions, wakeup from sleep, power on, etc.
; Reset Vector points here.
; ************************
Reset_Handler

     IF platform_current = "imx27"
     IF :DEF: LOGIC_PD_LED_TEST
;Turn the Logic PD LED on as a Test

	ldr	r3, =0x04000000
	ldr	r2, =0x10015220
	str	r3, [r2]

	ldr	r3, =0x04000000
	ldr	r2, =0x10015200
	str   	r3, [r2]

	ldr	r3, =0x00300000
	ldr	r2, =0x10015208
	str   	r3, [r2]

	ldr	r3, =0x00000000
	ldr	r2, =0x1001521C
	str   	r3, [r2]
;END TEST
	ENDIF
	ENDIF

;;
;;**START of ETM Build Option***************************************
;;This Option need to setup everything that boot would have
;;Old ETMs could not run from flash and could not run Boot
;;This setup the system so the App could run for Testing
;;******************************************************************
;;
	IF :DEF: ETM

;; --- Initialise stack pointer registers
;; Enter IRQ mode and set up the IRQ stack pointer
	MOV	R0, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
	LDR	sp, =IRQ_Stack

;; Enter FIQ mode and set up the FIQ stack pointer
	MOV	R0, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
	LDR	sp, =FIQ_Stack

;; Enter ABT mode and set up the ABT stack pointer
	MOV	R0, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
	LDR	sp, =ABT_Stack

;; Enter Undefined mode and set up the Undefined stack pointer
	MOV	R0, #Mode_UNDEF:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
	LDR	sp, =UNDEF_Stack


;; Set up the SVC stack pointer last and return to SVC mode
	MOV	R0, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
     	LDR 	sp, =SVC_Stack

;;
;; ok, now that I know interrupts and mode flags can only be set up
;; in priviledged mode... I am going to make sure the system is ALWAYS in
;; priviledged mode -- SYSTEM mode.  The intent is to never run in user mode.
;;
	mov	r0, #Mode_SYS:OR:I_Bit:OR:F_Bit ; no interrupts for now
	msr	cpsr_c, r0
	ldr	sp, =SYS_Stack           	; note above Sys_stack=Usr_stack so I really should never use user mode

;; set up plls, just in cas they were not reset to reset
;; write the defaults to MPCTL0 and UPCTL0, just so I don't assume
;	ldr r0,=MPCTL0
;	ldr r1,=DEFAULT_MCU_PLL
;	str r1,[r0]
;	ldr r0,=UPCTL0
;	ldr r1,=DEFAULT_USB_PLL
;	str r1,[r0]

;; This is the slow one.  What Boot would do 48Mhz
	ldr	r0,=CSCR
	ldr	r1,=CSCRCLEAR
	str	r1,[r0]         ;; start new clocks AND perclk1 //FCLK// out

	ldr	r0,=CS0U
	ldr	r1,=WSCEQU1
	str	r1,[r0]         ;; change wait states for cs0 from 64 to 2
	bl	Init_Uart1
;;
;;	Use default 8MB memory size for emulator, then initialize all shared table memory variables
	ldr	r1,=MEMORY_SIZE
	ldr	r0,=0x800000
	str	r0,[r1]

	ldr	r0,=DYNAMIC_MEMORY_MAP
	ldr	r1,=0x12345678
	str	r1,[r0]

	ldr	r0,=INVERSE_DYNAMIC_MEMORY_MAP
	ldr	r1,=0xEDCBA987
	str	r1,[r0]

	ldr	r0,=CACHED_MEMORY_BASE
	ldr	r1,=ExternalRamBase
	str	r1,[r0]

	ldr	r0,=CACHED_MEMORY_BASE		; r2 will hold the non-cached RAM offset based on actual memory size
	ldr	r2,[r0]
	ldr	r1,=MEMORY_SIZE
	ldr	r3,[r1]

;; The minimum offset from cached memory to non-cached memory is 16 MB, in order to ensure compatibility with old application code.
	ldr	r0,=0x1000000
	cmp	r0,r3
	bgt	Use16MBOffsetToNonCachedRAM_ETM
	mov	r0,r3

Use16MBOffsetToNonCachedRAM_ETM
	add	r2,r2,r0
	ldr	r0,=NON_CACHED_MEMORY_BASE
	str	r2,[r0]

	ldr	r0,=CACHED_ROM_BASE
	ldr	r1,=CachedExternalRomBase
	str	r1,[r0]

	ldr	r0,=NON_CACHED_ROM_BASE
	ldr	r1,=UncachedExternalRomBase
	str	r1,[r0]

	ldr	r0,=ROM_SIZE
	ldr	r1,=ExternalRomSize
	str	r1,[r0]

;; We're going to be adding tables and stacks to the end of memory space.  Use r2 to point to the next location to allocate.
	ldr	r3,=CACHED_MEMORY_BASE
	ldr	r2,[r3]
	ldr	r3,=MEMORY_SIZE
	ldr	r4,[r3]
	add	r2,r2,r4

	ldr	r0,=MMU_LEVEL_1_TABLE
	ldr	r1,=Level1tabSize
	sub	r2,r2,r1
	str	r2,[r0]
	ldr	r0,=MMU_LEVEL_1_TABLE_SIZE
	str	r1,[r0]

	ldr	r0,=MMU_LEVEL_2_TABLE
	ldr	r1,=Level2VectorTableSize
	sub	r2,r2,r1
	str	r2,[r0]
	ldr	r0,=MMU_LEVEL_2_TABLE_SIZE
	str	r1,[r0]

	ldr	r0,= NOT_USED1				;no longer used, was MMU_DISABLE_BUFFER
	mov	r1,#0
	str	r1,[r0]
	ldr 	r0,= NOT_USED2				;no longer used, was MMU_DISABLE_BUFFER_SIZE
	str	r1,[r0]

	ldr	r0,=SVC_STACK
	ldr	r1,=SVC_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=ABT_STACK
	ldr	r1,=ABT_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=UNDEF_STACK
	ldr	r1,=UNDEF_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_UNDEF:OR:I_Bit:OR:F_Bit ; No interrupts
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=IRQ_STACK
	ldr	r1,=IRQ_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=FIQ_STACK
	ldr	r1,=FIQ_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=SYS_STACK
	ldr	r1,=SYS_StackSize
	str	r2,[r0]
	mov	r0, #Mode_SYS:OR:I_Bit:OR:F_Bit ; no interrupts for now
	MSR	cpsr_c, R0
	mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=HEAP_END
	str	r2,[r0]

	bl	MMU_init					; Turn on the MMU
	bl	Move_Vector_Table_To_Zero	; Copy the vectors to zero, now that we've turned on the MMU.

	ENDIF ;:DEF: ETM
;;
;;*****************END of ETM Build Option***************************************
;;*******************************************************************************
;;


	bl	App_Clock_Speed			;speed up BCLK for MXL, Turns on Clocks for iMX25

;;
;; In case boot code didn't supply memory variables in the shared table, set them to the defaults.  If DYNAMIC_MEMORY_MAP = 0x12345678 and
;; INVERSE_DYNAMIC_MEMORY_MAP = 0xEDCBA987, then boot code configured them.
;;

	ldr	r0,=DYNAMIC_MEMORY_MAP
	ldr	r1,[r0]
	ldr	r0,=INVERSE_DYNAMIC_MEMORY_MAP
	ldr	r2,[r0]
	ldr	r3,=0x12345678
	cmp	r3,r1
	bne	SetMemoryVariableDefaults
	ldr	r3,=0xEDCBA987
	cmp	r3,r2
	beq	SkipSetMemoryVariableDefaults

SetMemoryVariableDefaults
	ldr	r0,=CACHED_MEMORY_BASE
	ldr	r1,=ExternalRamBase
	str	r1,[r0]

	ldr	r1,=MEMORY_SIZE
	ldr	r3,=ExternalRamSize
	str	r3,[r1]

;; The minimum offset from cached memory to non-cached memory is 16 MB, in order to ensure compatibility with old application code.
	ldr	r0,=CACHED_MEMORY_BASE		; r2 will hold the non-cached RAM offset based on actual memory size
	ldr	r2,[r0]
	ldr	r0,=0x1000000
	cmp	r0,r3
	bgt	Use16MBOffsetToNonCachedRAM
	mov	r0,r3

Use16MBOffsetToNonCachedRAM
	add	r2,r2,r0
	ldr	r0,=NON_CACHED_MEMORY_BASE
	str	r2,[r0]

	ldr	r0,=CACHED_ROM_BASE
	ldr	r1,=CachedExternalRomBase
	str	r1,[r0]

	ldr	r0,=NON_CACHED_ROM_BASE
	ldr	r1,=UncachedExternalRomBase
	str	r1,[r0]

	ldr	r0,=ROM_SIZE
	ldr	r1,=ExternalRomSize
	str	r1,[r0]

;; We're going to be adding tables and stacks to the end of memory space.  Use r2 to point to the next location to allocate.
	ldr	r3,=CACHED_MEMORY_BASE
	ldr	r2,[r3]
	ldr	r3,=MEMORY_SIZE
	ldr	r4,[r3]
	add	r2,r2,r4

	ldr	r0,=MMU_LEVEL_1_TABLE
	ldr	r1,=Level1tabSize
	sub	r2,r2,r1
	str	r2,[r0]
	ldr	r0,=MMU_LEVEL_1_TABLE_SIZE
	str	r1,[r0]

	ldr	r0,=MMU_LEVEL_2_TABLE
	ldr	r1,=Level2VectorTableSize
	sub	r2,r2,r1
	str	r2,[r0]
	ldr	r0,=MMU_LEVEL_2_TABLE_SIZE
	str	r1,[r0]

	ldr	r0,= NOT_USED1				;no longer used, was MMU_DISABLE_BUFFER
	mov	r1,#0
	str	r1,[r0]
	ldr 	r0,= NOT_USED2				;no longer used, was MMU_DISABLE_BUFFER_SIZE
	str	r1,[r0]

	ldr	r0,=SVC_STACK
	ldr	r1,=SVC_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=ABT_STACK
	ldr	r1,=ABT_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=UNDEF_STACK
	ldr	r1,=UNDEF_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=IRQ_STACK
	ldr	r1,=IRQ_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=FIQ_STACK
	ldr	r1,=FIQ_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=SYS_STACK
	ldr	r1,=SYS_StackSize
	str	r2,[r0]
	sub	r2,r2,r1

	ldr	r0,=HEAP_END
	str	r2,[r0]


SkipSetMemoryVariableDefaults
;; put the processor in system mode loading elfs in EVD debugger leaves the processor in
;; SVC mode normally boot leaves it in system mode
	mov	r0, #Mode_SYS:OR:I_Bit:OR:F_Bit ; no interrupts for now
	MSR	cpsr_c, R0


;;
;; Load FF over all the stacks for later tracing as to how much of the stack has been untouched.
;;
	ldr	r2, =0xFFFFFFFF
	ldr	r1, =MMU_LEVEL_2_TABLE
	ldr	r0, =HEAP_END
	ldr	r0, [r0]
	ldr	r1, [r1]
	sub	r1,r1,r0

StackInitialize
	str	r2,[r0],#4
	subs	r1,r1,#4
	bne	StackInitialize


C_Init

;;
;; Setup heap addresses.
;;
	ldr	r1,=HEAP_START
	ldr	r2,=|Image$$ZI$$Limit|
	add	r2,r2,#7                  ;8 alligned
	bic	r2,r2,#7
	str	r2,[r1]

;; HEAP_END is now setup by boot code
;	ldr	r1,=HEAP_END
;	ldr	r2,=End_Of_Heap
;	str	r2,[r1]

;; Load application image information in the shared table.
	ldr	r0, =|Image$$RO$$Base|
	ldr	r1, =APP_IMAGE_RO_BASE
	str	r0,[r1]
	ldr	r0, =|Image$$RO$$Limit|
	ldr	r1, =APP_IMAGE_RO_END
	str	r0,[r1]
	ldr	r0, =|Image$$RW$$Base|
	ldr	r1, =APP_IMAGE_RW_BASE
	str	r0,[r1]
	ldr	r0, =|Image$$RW$$Limit|
	ldr	r1, =APP_IMAGE_RW_END
	str	r0,[r1]
	ldr	r0, =|Image$$ZI$$Base|
	ldr	r1, =APP_IMAGE_ZI_BASE
	str	r0,[r1]
	ldr	r0, =|Image$$ZI$$Limit|
	ldr	r1, =APP_IMAGE_ZI_END
	str	r0,[r1]

	LDR     r0, =|Image$$RO$$Limit| ; Get pointer to ROM data
	LDR     r1, =|Image$$RW$$Base|  ; and RAM copy
	LDR     r3, =|Image$$ZI$$Base|  ; Zero init base => top of ...
					; ... initialized data
	CMP     r0, r1                  ; Check that they are different
	BEQ     do_zinit

do_copy
	CMP     r1, r3                  ; Copy init data
	LDRCC   r2, [r0], #4            ; CC gives unsigned lower
	STRCC   r2, [r1], #4
	BCC     do_copy

do_zinit
	LDR     r1, =|Image$$ZI$$Limit| ; Top of zero init segment
	MOV     r2, #0

zinit
	CMP     r3, r1                  ; Create zero init data
	STRCC   r2, [r3], #4            ; CC gives unsigned lower
	BCC     zinit


; --- Now we enter the main C application code
; --- do not use main(), this will confuse the C compiler.

start_c
	BL	C_Entry

;ERROR, better never get here
here	B	here



;;
;;***************************************************************************************
;; Bus mode support stuff
;;***************************************************************************************
;;
;;


;;
;;SwitchToAsync
;;	Switch to async mode using coprcessor 15 control reg 1
;;
	IF :DEF: ALLOW_BUS_MODE_FUNCTIONS
SwitchToAsync
     ;;for now, trying to get this to work, setup MPCPLL for high speed... long run don't want this here!!
	ldr		r0,= MPCTL0
        ldr		r1,= MPCTL0_SETTING
        str		r1,[r0]

        mrc		p15, 0, r0, c1, c0, 0
        orr		r0,r0,#0xc0000000         	;; mask the upper 2 bits
        mcr		p15, 0, r0, c1, c0, 0     	;; write it back

	mov		pc,lr






;;
;;Async_mode
;;
Async_mode
    mrc p15,0,r0,c1,c0,0
	mov r2, #0xC0000000
	orr r0,r2,r0
   	mcr p15,0,r0,c1,c0,0
	mov pc,lr


;;
;;FastBus_mode
;;
FastBus_mode
	mrc		p15,0,r0,c1,c0,0
	bic		r0,r0,#0xC0000000
	mcr		p15,0,r0,c1,c0,0
	mov		pc,lr


;;
;;GetCoreClockMode
;;
GetCoreClockMode
	mrc		p15,0,r0,c1,c0,0
	mov		pc,lr


;;
;;RestoreCoreClockMode
;;
RestoreCoreClockMode
	mrc		p15,0,r1,c1,c0,0
	bic		r1,r1,#0xC0000000
	orr		r0,r1,r0
	mcr		p15,0,r0,c1,c0,0
	mov		pc,lr

	ENDIF	;:DEF: ALLOW_BUS_MODE_FUNCTIONS
;;
;;GetCPSR
;;
GetCPSR
	mrs		r0,cpsr
	bx		lr


;;
;;SetCPSR
;;
SetCPSR
	msr		CPSR_fsxc,r0				;; saved psr in r12 into spsr
	bx		lr

;;
;;App_Clock_Speed
;;
App_Clock_Speed
	IF platform_current = "imx25"
		;ldr	r0,=CS0U        ;; before I up the system clock, up wait states to flash accordingly
	;	ldr	r1,=APPWSCEQU1
	;	str	r1,[r0]         ;; change wait states for cs0

	ELSE
		IF platform_current = "mxl"
			ldr	r0,=CS0U        ;; before I up the system clock, up wait states to flash accordingly
			ldr	r1,=APPWSCEQU1
			str	r1,[r0]         ;; change wait states for cs0

			ldr	r0,= CSCR        ;; speed up BCLK
			ldr	r1,= CSCR_SETTING
			str	r1,[r0]         ;; start new clocks AND perclk1


		ENDIF
	ENDIF
	bx lr                       ;; return

	END



