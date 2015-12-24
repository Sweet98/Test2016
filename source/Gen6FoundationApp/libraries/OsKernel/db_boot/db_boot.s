;/************************************************************************/
;//
;// HHP 4480 embedded firmware
;//
;//  Boot Startup -- C Init --
;//  Date:       1/28/02
;//  Tools:      ARM SDT 2.5 for starters... will be ARM ADS 1.1 or Metaware
;//  Target:     Dragonball Development Board
;//
;//======================================================================================
;//	Revision information
;//======================================================================================
;// $RCSfile: db_boot/db_boot.s $
;// $Revision: 1.7.2.56 $
;// $Date: 2010/12/20 14:13:59EST $
;//======================================================================================
;****************************************************************************
;*     File: sa_boot.s                                        				*
;*  Purpose: Application Startup Code.                        				*
;*     This code is based on chapter 13 of ARM user guide     				*
;*     and sample code from Digital Semiconductor	      	  				*
;****************************************************************************
;Code Flow:
;	1.  Copy Boot to iRAM
;	2.  Initialize items from the table like: GPIO, Clocks, Flash Timing
;		 This is done because Flash timing should change before the clocks.
;	3.  Reset PLLs for iMX25, iMX27
;	4.  Initialize stack pointer 
;	5.  Initialize RAM
;	6.  Initialize UART
;	7.  Test RAM
;	8.  Initialize all shared table memory variables
;	9.  Initialize the MMU
;	10. Initialize C Code
;	11. Jump to C code
;****************************************************************************


     REQUIRE8  	      	; added to compile with new tool note code and constans in db_memmap.s should be checked
     PRESERVE8

     INCLUDE db_memap.s
     INCLUDE db_hdw.s    ; definitions of dragonball registers, etc

     AREA Boot, CODE, READONLY

	IMPORT Init_Uart1
	IMPORT Send_Address_in_r0
	IMPORT Send_CRLF
	IMPORT Send_RAM_Pass
	IMPORT MMU_init
	IMPORT Move_Vector_Table_To_Zero
	IMPORT Set_ram_to_zero
	IMPORT Test_ram_memory
	IMPORT DeviceTypeLocation
     ;;Make platform specific imports
     IF ((platform_current = "imx25") || (platform_current = "imx27"))
		IMPORT Init_ddr_sdram
		IMPORT WhatIsMemSizeMB
		IMPORT DRAM1GIG
		IMPORT DRAM256M
		IMPORT DRAM128M
		
	 ELSE	;;MXL
	 	IF platform_current = "mxl"
     		IMPORT Init_SDRAM
     		IMPORT IsMem8or16MB
		ENDIF
     ENDIF

;; Import c function to determine ROM Size
     IMPORT |DetermineROMSize|

;; Import shared table from c
     IMPORT  |Image$$RO$$Base|		; Start of ROM code (=start of ROM data)
     IMPORT  |Image$$RO$$Limit|    	; End of ROM code (=start of ROM data)
     IMPORT  |Image$$RW$$Base|     	; Base of RAM to initialize
     IMPORT  |Image$$RW$$Limit|		; End of RAM to initialize
     IMPORT  |Image$$ZI$$Base|     	; Base and limit of area
     IMPORT  |Image$$ZI$$Limit|    	; to zero initialize

;; Import the C main entry point of code
     IMPORT	C_Entry


     EXPORT 	Reset_Handler


;; ************************
;; Actual Start of Code under all conditions, wakeup from sleep, power on, etc.
;; Reset Vector points here.
;; ************************
Reset_Handler
Start_Boot

	bl	device_cofig				;Configure 
	
	IF platform_current = "imx25"
	
;; ****************************
;; 	Clock/Reset Module
;; ****************************
	ldr	r3, =CRM_MPCTL_SETTING		;Set MPCTL to the Correct value
	ldr	r2, =CRM_MPCTL
	str   	r3, [r2]

	ldr	r3, =CRM_MPLL_RST			;Reset the MPCTL
	ldr	r2, =CRM_CCTL
	str   	r3, [r2]

	;;PLLs are defulted to the correct setting
	;;Wait for MPLL to lock new frequency
	ldr	r2, =CRM_MPCTL
lock_loop_MPCTL						;Wait for MPCTL to Lock
	ldr	r3,[r2]
	tst	r3,#CRM_MPCTL1_MASK_LOCK
	beq	lock_loop_MPCTL

	ldr	r3, =CRM_UPCTL_SETTING		;Set UPCTL
	ldr	r2, =CRM_UPCTL
	str   	r3, [r2]
	
	ldr	r3, =CRM_UPLL_RST			;Reset the UPCTL
	ldr	r2, =CRM_CCTL
	str   	r3, [r2]
	
	ldr	r2, =CRM_UPCTL	
lock_loop_UPCTL						;Wait for UPCTL to Lock
	ldr	r3,[r2]
	tst	r3,#CRM_MPCTL1_MASK_LOCK
	beq	lock_loop_UPCTL
	

;; *END iMX25*******************************************************
	ELSE
		IF platform_current = "imx27"
	
;; ****************************
;; 	Clock/Reset Module
;; ****************************
;;
;; Set MCU (Fclk) and AHB (Hclk) clocks so that DDR memory can be initialized
;;    Note- AHB-Lite must be initialized prior to setting up clocks
;;

;;Set MCU PLL to 399 MHz
	ldr	r3, =CRM_MPCTL0_SETTING_MHZ399
	ldr	r2, =CRM_MPCTL0
	str   	r3, [r2]
	ldr	r3, =CRM_MPCTL1_SETTING_MHZ399
	ldr	r2, =CRM_MPCTL1
	str   	r3, [r2]

;;Set-  ARM_SRC = MPLL CLK,  AHB_DIV = 2, ARM_DIV = 1
;;Reset MCU PLL to lock in new frequency
	ldr	r3, = 0x33F4A107 ; CRM_CSCR_SETTING_MHZ399 
	ldr	r2, =CRM_CSCR
	str   	r3, [r2]

;;Wait for MPLL to lock new frequency
	ldr	r1, =CRM_MPCTL1_MASK_LOCK
lock_loop
	ldr	r2, =CRM_MPCTL1
	ldr	r3,[r2]
	tst	r3,r1
	beq	lock_loop
	
		ENDIF	;;endof iMX27
;; *END iMX27*******************************************************		
	ENDIF	;;end set up clocks platform_current = "iMX25"
;; ****************************************************************
;; --- Initialise stack pointer registers
;;

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


;;
;; Set up the SVC stack pointer (Supervisor Mode)
;;
	MOV	R0, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
     	MSR 	cpsr_c, R0
     	LDR 	sp, =SVC_Stack

;;
;; ok, now that I know interrupts and mode flags can only be set up
;; in priviledged mode... I am going to make sure the system is ALWAYS in
;; priviledged mode -- SYSTEM mode.  The intent is to never run in user mode.
;;
;; Set up the SYS stack pointer (System Mode) Last
;;
     	mov 	r0, #Mode_SYS:OR:I_Bit:OR:F_Bit ; no interrupts for now
     	msr 	cpsr_c, r0
     	ldr  	sp, =SYS_Stack           ; note above Sys_stack=Usr_stack so I really should never use user mode

;;
;; --- END Initialise stack pointer registers
;; ****************************************************************


;;
;;******************************************************************************************************
	IF Board_TARGET = "MXL_Matrix" 		;;Use GPIO settings Matrix Board
;;
;;******************************************************************************************************
;;
;; John's stuff added for form factor for I/O contention
;; Note here is this does not appear to be detrimental to the development board environment as the
;; 3 pins in question -- port b bit 20, port d bits 16 and 17 are not connected in that environment
;; so initial thought is to include without conditional compile
;; enable 232tx and disable good read led's
;; 1 for each pin [i] that is utilized as gpio, set gius
;; I am adding the power supply hold signal and the True232 drive enable to this.

;; tried this first without read/write protection and it did NOT work... not sure why, but clearly it was significant
;; this deals with oe and gr led movement  ** this will turn off the gr led on the form factor board
;; now added in uWake signal and enable for True Rs232 driver.
;;
    ldr 	r0,=PORTD_GIUS
	ldr 	r1,[r0]
	orr 	r1,r1,#(BIT16:OR:BIT17) ;;0x30000
	orr 	r1,r1,#(BIT6:OR:BIT9)
	orr 	r1,r1,#(BIT11)
	str 	r1,[r0]

	ldr 	r0,=PORTD_OCR2
	ldr 	r1,[r0]
	orr 	r1,r1,#0xf
	str 	r1,[r0]

	ldr 	r0,=PORTD_OCR1
	ldr 	r1,[r0]
	orr 	r1,r1,#(BIT12:OR:BIT13:OR:BIT18:OR:BIT19) ;; sets up PD6 and PD9.
	orr 	r1,r1,#(BIT22:OR:BIT23) ;;sets up PD11.
	str 	r1,[r0]

	ldr	r0,=PORTD_DR
	ldr	r1,[r0]
	and	r1,r1,#(:NOT:BIT16)
	and	r1,r1,#(:NOT:BIT17)
	and	r1,r1,#(:NOT:BIT11)
	orr	r1,r1,#(BIT6:OR:BIT9)
	str	r1,[r0]

	ldr	r0,=PORTD_DDIR
	ldr	r1,[r0]
	orr	r1,r1,#(BIT16:OR:BIT17) ;;0x30000
	orr	r1,r1,#(BIT6:OR:BIT9)
	orr	r1,r1,#(BIT11)
	str	r1,[r0]

;;
;; tried this first without read/write protection and it did NOT work... not sure why, but clearly it was significant
;; what the code does below is handle the form factor port b pin 20  -- analog switch enable NEEDS to be low
;; not connected on development board
;;
	ldr 	r0,=PORTB_GIUS
	ldr 	r1,[r0]
	orr 	r1,r1,#BIT20 ;;0x100000
	str 	r1,[r0]

	ldr 	r0,=PORTB_OCR2
	ldr 	r1,[r0]
	orr 	r1,r1,#0x300
	str 	r1,[r0]

	ldr 	r0,=PORTB_DR
	ldr 	r1,[r0]
	and 	r1,r1,#(:NOT:BIT20)
	str 	r1,[r0]

 	ldr 	r0,=PORTB_DDIR
 	ldr 	r1,[r0]

;; Change made for wedge
;;
    and 	r1,r1,#(:NOT:BIT20)
;	orr 	r1,r1,#BIT20
	str 	r1,[r0]
;;
;; end of form factor add stuff for mxl
;;
;;******************************************************************************************************
	ENDIF		;;end hardware configuration MXL
;;******************************************************************************************************
;;


;;
;;Setup UART1 for use as a debug output, for iMX25 Corded HW the output enable will not 
;;	be enabled until boot knows to stay in boot or when the APP knows it’s a corded board
;;
     	bl	Init_Uart1	;; initialize  uart one


;; Initialize the ram

	IF ((platform_current = "imx25") || (platform_current = "imx27"))
		ldr		r0,=DRAM1GIG
		bl		Init_ddr_sdram          ;;Configure mobile DDR1 SDRAM
		bl		WhatIsMemSizeMB
		cmp		r0,#5					;; return value for 128M
		beq		raminitdone 
		ldr		r0,=DRAM256M
		bl		Init_ddr_sdram          ;;Configure mobile DDR1 SDRAM
		bl		WhatIsMemSizeMB
		cmp		r0,#4					;; 4 is return value for 32MB
		beq		raminitdone 
		ldr		r0,=DRAM128M
		bl		Init_ddr_sdram          ;;Configure mobile DDR1 SDRAM
		bl		WhatIsMemSizeMB
		
raminitdone		
		
			
	 ELSE	;;MXL
	 	IF platform_current = "mxl"
			bl	Init_SDRAM     			;;try and init SDRAM to some state
			bl	IsMem8or16MB     		
		ENDIF
     ENDIF
	bl	Set_ram_to_zero

	bl	Test_ram_memory

	IF MEMORY_DEBUG			;;set in Db_hdw.s
;;
;; Output the results for the sdram memory test
;; Results from test "Test_ram_memory" are stored in r0
;; this output section must always follow "Test_ram_memory"
;;
	mov	r4,r0	    		;;save test results to r4
	cmp	r4,#0
	beq	pass_test_ram_memory
	cmp	r4,#1
	beq	finish_test_ram_memory
	cmp	r4,#2
	bne	finish_test_ram_memory

	;;if failure, output results
	bl	Send_CRLF
	mov 	r0,r1			;; location
	bl 	Send_Address_in_r0
	mov 	r0,r2			;; value should have been
	bl 	Send_Address_in_r0
	mov 	r0,r3			;; value read
	bl 	Send_Address_in_r0
	b	finish_test_ram_memory

pass_test_ram_memory
	bl	Send_CRLF
	bl	Send_RAM_Pass
	;;output shared table values for mem size
	ldr 	r3,=SIZE_OF_SDRAM_S		;; r3 temp has the shared table location of sdram size error information
	ldr 	r0,[r3]				;; load r0 with shared table SIZE_OF_SDRAM_S
	bl 	Send_Address_in_r0

	ldr 	r2,=MEMORY_SIZE                 ;; r2 temp has the shared table location of sdram actual size
	ldr 	r0,[r2]                         ;;load ro with shared table MEMORY_SIZE
	bl 	Send_Address_in_r0

finish_test_ram_memory
	bl	Send_CRLF

	ENDIF				;;end memory test output


;; After establishing the memory size, initialize all shared table memory variables
;;
	ldr r1,=DeviceTypeLocation		;device type
	ldr r1,[r1]
	ldr r0,=DEVICE_TYPE_ST
	str	r1,[r0]
	
	ldr	r0,=DYNAMIC_MEMORY_MAP
	ldr	r1,=0x12345678 			;;tag to indicate shared table filled in
	str	r1,[r0]

	ldr	r0,=INVERSE_DYNAMIC_MEMORY_MAP
	ldr	r1,=0xEDCBA987			;;tag to indicate shared table filled in
	str	r1,[r0]

	ldr	r0,=CACHED_MEMORY_BASE	;; shared table address of CACHED_MEMORY_BASE
	ldr	r1,=ExternalRamBase		;; cached memory mapped to physical memory address
	str	r1,[r0]					;; store the value

								; r1 holds the non-cached RAM offset based on actual memory size
	ldr	r2,=MEMORY_SIZE
	ldr	r0,[r2]

;; The minimum offset from cached memory to non-cached memory is 16 MB, in order to ensure compatibility with old application code.
;;
	cmp		r0,#0x1000000
	movle	r0,#0x1000000
	add		r1,r1,r0
	ldr		r0,=NON_CACHED_MEMORY_BASE
	str		r1,[r0]

	ldr		r1,=CACHED_ROM_BASE
	ldr		r0,=CachedExternalRomBase
	str		r0,[r1]
	ldr		r1,=NON_CACHED_ROM_BASE
	ldr		r2,=UncachedExternalRomBase
	str		r2,[r1]

	IF platform_current = "imx25"
	mov	r0,#0
	ELSE
	bl	DetermineROMSize
	ENDIF
	ldr	r1,=ROM_SIZE
	str	r0,[r1]

;; We're going to be adding tables and stacks to the end of memory space.  Use r2 to point to the next location to allocate.
;;
	IF :DEF: RAMBASEDBOOT
		ldr r2,=VRAM_Limit	;;Set the stacks up in iRAM for Ram Based Boot
	ELSE
		ldr	r3,=CACHED_MEMORY_BASE
		ldr	r2,[r3]
		ldr	r3,=MEMORY_SIZE
		ldr	r4,[r3]
		add	r2,r2,r4
	ENDIF

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
	MOV	R0, #Mode_SVC:OR:I_Bit:OR:F_Bit 	; No interrupts
   	MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=ABT_STACK
	ldr	r1,=ABT_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_ABT:OR:I_Bit:OR:F_Bit 	; No interrupts
    MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=UNDEF_STACK
	ldr	r1,=UNDEF_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_UNDEF:OR:I_Bit:OR:F_Bit 	; No interrupts
    MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=IRQ_STACK
	ldr	r1,=IRQ_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_IRQ:OR:I_Bit:OR:F_Bit 	; No interrupts
    MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=FIQ_STACK
	ldr	r1,=FIQ_StackSize
	str	r2,[r0]
	MOV	R0, #Mode_FIQ:OR:I_Bit:OR:F_Bit 	; No interrupts
    MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=SYS_STACK
	ldr	r1,=SYS_StackSize
	str	r2,[r0]
    mov 	r0, #Mode_SYS:OR:I_Bit:OR:F_Bit 	; no interrupts for now
    MSR 	cpsr_c, R0
    mov	sp, r2
	sub	r2,r2,r1

	ldr	r0,=HEAP_END
	str	r2,[r0]

;;
;; Now it is time to initialize MMU and then run some 'C'
;;
	IF :DEF: RAMBASEDBOOT
		;;Don't initialize MMU for Ram Based Boot, this is for the HW memory test
	ELSE
	
		bl	MMU_init			; Turn on the MMU
		bl	Move_Vector_Table_To_Zero	; Copy the vectors to zero, now that we've turned on the MMU.
	
	ENDIF
;;
;;Do initialization for 'C' and then run it
;;
C_Init
;;
;; Load boot image information in the shared table.
;;
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

;; --- Now we enter the main C application code
;; --- do not use main(), this will confuse the C compiler.
;;
start_c
	BL	C_Entry

;; better never get here
;;
here
	B	here
;;****************************************************************************************************************

device_cofig
;; --------------------------------------------------------------------
;; This Loop Uses the Devive_configTab Table at the end of the File.
;; It initialize drive strengths, clock gating, cs0 timing, and GPIO for
;;	all processors.
;; -------------------------------------------------------------------- 
	adr		r0,Devive_configTab	; this is the table 
devcofigloop
	ldmia	r0!,{r2,r3}				;load address into r2 and data in r3
	cmp		r2,#0					;if the address is 0 we are at end
	strne	r3,[r2]					;else *addresss=sata;
	bne		devcofigloop			;loop till done
 
 	bx 	 lr


	IF platform_current = "imx25"
;;********************************************************
;;Device Config Table
;;Table Layout:
;;			DCD - register base address 
;; 			DCD - register setting
;;********************************************************
Devive_configTab
	IF :DEF: INITAIPS2 ;; dont need to do this so remove later

;; ******************************
;; 	AIPS Settings
;; ******************************
;--- AIPS1 ---
	DCD	AIPS1_PACR0_7
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_PACR8_15
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_PACR16_23
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_PACR24_31
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_OPACR0_7
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_OPACR8_15
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_OPACR16_23
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_OPACR24_31
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_OPACR32_33
	DCD	AIPS1_ZERO_Setting
	DCD	AIPS1_MPROT0_7
	DCD	AIPS1_MPROT0_7_Setting
	DCD	AIPS1_MPROT8_15
	DCD	AIPS1_MPROT8_15_Setting
;--- AIPS2 ---
	DCD	AIPS2_PACR0_7
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_PACR8_15
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_PACR16_23
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_PACR24_31
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_OPACR0_7
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_OPACR8_15
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_OPACR16_23
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_OPACR24_31
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_OPACR32_33
	DCD	AIPS2_ZERO_Setting
	DCD	AIPS2_MPROT0_7
	DCD	AIPS2_MPROT0_7_Setting
	DCD	AIPS2_MPROT8_15
	DCD	AIPS2_MPROT8_15_Setting
	ENDIF
;; ******************************
;; 	System Drive Level 
;; ******************************
	DCD	IOMUXC_SW_PAD_CTL_GRP_DSE_DDR	 			;; DDR Group Drive Level
	DCD	IOMUXC_SW_PAD_CTL_GRP_DSE_DDR_SETTING		;; High Drive Strengh for DDR Group Pads

;----------------------------------------------------------------------------------	
	IF Board_TARGET = "iMX25_Development"
;;---  SET GPIO BOOT Pin 
		DCD IOMUXC_SW_MUX_CTL_PAD_CS1				;GPIO Port 4 Pin 3
		DCD MUX_MODE_5
		DCD GPIO4_GDIR0								;GPIO Port 4 Pin 3 DIR
		DCD 0x00000000
		
;;---   NOR Flash data pin drive strengh etc ---
		DCD	IOMUXC_SW_PAD_CTL_PAD_D15
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D14
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D13
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D12
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D11
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D10
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D9
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D8
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D7
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D6
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D5
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D4
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D3
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D2
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D1
		DCD	NOR_FLASH_PAD_CTL_SETTING
		DCD	IOMUXC_SW_PAD_CTL_PAD_D0
		DCD	NOR_FLASH_PAD_CTL_SETTING
		
;; --- Initialise CSO NOR FLASH ---
		DCD	WEIM_CS0U        	;; upper control register for CS0
   		DCD	APPWSCEQU1  	;; correct timing for BCLK for APP running Hclk at MAX SPEED for sdram mem.
		DCD	WEIM_CS0L        	;; lower control register for CS0
		DCD	APPWSCEQUL  	;; correct timing for BCLK for APP running Hclk at MAX SPEED for sdram mem.
		DCD	WEIM_CS0A        	;; additional control register for CS0
		DCD	APPWSCEQUA  	;; correct timing for BCLK for APP running Hclk at MAX SPEED for sdram mem.
	
	ENDIF 		;;END NOR Flash configuration Board_TARGET = "iMX25_Development"
;----------------------------------------------------------------------------------
;----------------------------------------------------------------------------------		
	IF (Board_TARGET = "iMX25_Gen6_Board")

			;; --- GPIO Setting (See iMX25_hdw.i for Config)---
		IF :DEF: OEMBOOT	
			DCD GPIO1_DR0							;GPIO Port 1 Data
			DCD GPIO1_DR_OEM_SETTING
			DCD GPIO1_GDIR0							;GPIO Port 1 Direction
			DCD GPIO1_DIR_OEM_SETTING
		
			DCD GPIO2_DR0							;GPIO Port 2 Data
			DCD GPIO2_DR_OEM_SETTING
			DCD GPIO2_GDIR0							;GPIO Port 2 Direction
			DCD GPIO2_DIR_OEM_SETTING
		
			DCD GPIO3_DR0							;GPIO Port 3 Data
			DCD GPIO3_DR_OEM_SETTING
			DCD GPIO3_GDIR0							;GPIO Port 3 Direction
			DCD GPIO3_DIR_OEM_SETTING
		
			DCD GPIO4_DR0							;GPIO Port 4 Data
			DCD GPIO4_DR_OEM_SETTING
			DCD GPIO4_GDIR0							;GPIO Port 4 Direction
			DCD GPIO4_DIR_OEM_SETTING

		ELSE
			DCD GPIO1_DR0							;GPIO Port 1 Data
			DCD GPIO1_DR_SETTING
			DCD GPIO1_GDIR0							;GPIO Port 1 Direction
			DCD GPIO1_DIR_SETTING
			
			DCD GPIO2_DR0							;GPIO Port 2 Data
			DCD GPIO2_DR_SETTING
			DCD GPIO2_GDIR0							;GPIO Port 2 Direction
			DCD GPIO2_DIR_SETTING
			
			DCD GPIO3_DR0							;GPIO Port 3 Data
			DCD GPIO3_DR_SETTING
			DCD GPIO3_GDIR0							;GPIO Port 3 Direction
			DCD GPIO3_DIR_SETTING
			
			DCD GPIO4_DR0							;GPIO Port 4 Data
			DCD GPIO4_DR_SETTING
			DCD GPIO4_GDIR0							;GPIO Port 4 Direction
			DCD GPIO4_DIR_SETTING
	ENDIF		
	;; --- Pin Mux Control ---	
		DCD IOMUXC_SW_MUX_CTL_PAD_OE_ACD           ;Change Mux to GPIO for FLASH_WP
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_LSCLK            ;Change Mux to GPIO for nBT_RESET
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_I2C1_DAT         ;Change Mux to GPIO for CABLE_SEL1
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_I2C1_CLK         ;Change Mux to GPIO for CABLE_SEL0
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_CSI_MCLK         ;Change Mux to GPIO for CSI_MCLK
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_GPIO_F           ;Change Mux to GPIO for RED_ILL_CTRL
		DCD MUX_MODE_0							    ;GPIO Pin has ALT0 and not ALT5 for GPIO
		DCD IOMUXC_SW_MUX_CTL_PAD_GPIO_E           ;Change Mux to GPIO for PS_MODE
		DCD MUX_MODE_0							    ;GPIO Pin has ALT0 and not ALT5 for GPIO
		DCD IOMUXC_SW_MUX_CTL_PAD_GPIO_D           ;Change Mux to GPIO for SHUTDOWN
		DCD MUX_MODE_0							    ;GPIO Pin has ALT0 and not ALT5 for GPIO
		DCD IOMUXC_SW_MUX_CTL_PAD_GPIO_C           ;Change Mux to GPIO for BOOT_PWR_SELECT 
		DCD MUX_MODE_0							    ;GPIO Pin has ALT0 and not ALT5 for GPIO
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_ROW2         ;Change Mux to GPIO for PWR_ENA
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_CLKO             ;Change Mux to GPIO for BATT_LVL_EN
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_DE_B             ;Change Mux to GPIO for uP_WAKE
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_EB0              ;Change Mux to GPIO for KBD_SW_CNTRL
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_LBA              ;Change Mux to GPIO for REED_SWITCH
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_ECB              ;Change Mux to GPIO for FLASH_OUT
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA1       ;Change Mux to GPIO for HWT3
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_MDC          ;Change Mux to GPIO for HWT0
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_VSTBY_REQ        ;Change Mux to GPIO for GREEN_LED
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_MDIO         ;Change Mux to GPIO for HWT1
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_TDATA0       ;Change Mux to GPIO for HWT2
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK       ;Change Mux to GPIO for LED_BOOST_PWM
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA0       ;Change Mux to GPIO for HWT5
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_TX_EN        ;Change Mux to GPIO for HWT4
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_FEC_RX_DV        ;Change Mux to GPIO for AIM_ON
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_ROW3         ;Change Mux to GPIO for ENG_RESET
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_EXT_ARMCLK       ;Change Mux to GPIO for ILL_ON
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_POWER_FAIL       ;Change Mux to GPIO for VIBRATE
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_VSTBY_ACK        ;Change Mux to GPIO for RED_LED
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_UPLL_BYPCLK      ;Change Mux to GPIO for ISET2_SEL
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_CS1              ;Change Mux to GPIO for nBOOT
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D15              ;Change Mux to GPIO for CHG_TMR_CTRL
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D14              ;Change Mux to GPIO for BT_CONNECTED
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D13              ;Change Mux to GPIO for BT_HOST_WAKEUP
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D12              ;Change Mux to GPIO for BT_HOST_SLEEP
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D11              ;Change Mux to GPIO for BT_CMD_DATA
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D10              ;Change Mux to GPIO for BT_READY
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D9               ;Change Mux to GPIO for nSPI_FLASH_HOLD
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D8               ;Change Mux to GPIO for nCHG_ENABLE
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D7               ;Change Mux to GPIO for CHG_STAT2
		DCD MUX_MODE_5		
		DCD IOMUXC_SW_MUX_CTL_PAD_D6               ;Change Mux to GPIO for CHG_STAT1
		DCD MUX_MODE_5			
		DCD IOMUXC_SW_MUX_CTL_PAD_D5               ;Change Mux to GPIO for AUX_PWR_GD
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D4               ;Change Mux to GPIO for HOST_PWR_GD
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D3               ;Change Mux to GPIO for n232/KBD_OE4
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D2		       ;Change Mux to GPIO for n232/KBD_OE3
		DCD MUX_MODE_5	
		DCD IOMUXC_SW_MUX_CTL_PAD_D1               ;Change Mux to GPIO for n232/KBD_OE2
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_D0               ;Change Mux to GPIO for n232/KBD_OE1
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_UART2_CTS        ;Change Mux to GPIO for AIM_CTRL
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_BCLK             ;Change Mux to GPIO for TRIG_uP
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_COL3         ;Change Mux to GPIO for KEY_CLK_OUT
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_COL2         ;Change Mux to GPIO for nKEY_DATA_IN
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_COL1         ;Change Mux to GPIO for TERM_CLK_OUT
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_KPP_COL0         ;Change Mux to GPIO for nTERM_DATA_IN
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_UART2_RXD        ;Change Mux to GPIO for CRADLE_RX
		DCD MUX_MODE_5
		DCD IOMUXC_SW_MUX_CTL_PAD_UART2_TXD        ;Change Mux to GPIO for CRADLE_TX
		DCD MUX_MODE_5	
		DCD IOMUXC_SW_MUX_CTL_PAD_LD7              ;Change Mux to GPIO for BT_PWR_EN
		DCD MUX_MODE_5			
		DCD IOMUXC_SW_PAD_CTL_PAD_LD7              ;Change Pad Setting for BT_PWR_EN
		DCD PAD_CTL_PULLDW_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_LBA              ;Change Pad Setting for REED_SWITCH
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_ECB              ;Change Pad Setting for FLASH_OUT
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_RDATA0       ;Change Pad Setting for HWT5
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_TX_EN        ;Change Pad Setting for HWT4
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA1       ;Change Pad Setting for HWT3
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_TDATA0       ;Change Pad Setting for HWT2
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_MDIO         ;Change Pad Setting for HWT1
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_FEC_MDC          ;Change Pad Setting for HWT0
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_KPP_COL3         ;Change Pad Setting for KEY_CLK_OUT
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_KPP_COL1         ;Change Pad Setting for TERM_CLK_OUT
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D0               ;Change Pad Setting for n232/KBD_OE1
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D1               ;Change Pad Setting for n232/KBD_OE2
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D2               ;Change Pad Setting for n232/KBD_OE3
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D3               ;Change Pad Setting for n232/KBD_OE4
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D4               ;Change Pad Setting for HOST_PWR_GD
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D5               ;Change Pad Setting for AUX_PWR_GD
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D6               ;Change Pad Setting for CHG_STAT1
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D7               ;Change Pad Setting for CHG_STAT2
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D8               ;Change Pad Setting for nCHG_ENABLE
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D10              ;Change Pad Setting for BT_READY
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D11              ;Change Pad Setting for BT_CMD_DATA
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D12              ;Change Pad Setting for BT_HOST_SLEEP
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D13              ;Change Pad Setting for BT_HOST_WAKEUP
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_D14              ;Change Pad Setting for BT_CONNECTED
		DCD PAD_CTL_PULLUP_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_KPP_ROW2         ;Change Pad Setting for PWR_ENA
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_KPP_COL0         ;Change Pad Setting for nTERM_DATA_IN
		DCD PAD_CTL_NOPULL_INPUT
		DCD IOMUXC_SW_PAD_CTL_PAD_LSCLK            ;Change Pad Setting for nBT_RESET
		DCD PAD_CTL_NOPULL_INPUT				   ;This pin is an output but requires the same settings
		
		IF :DEF: OEMBOOT
			DCD IOMUXC_SW_MUX_CTL_PAD_FEC_RDATA1	   ;Change Mux to GPIO for OEM uP_WAKE, 1902 SHUTDOWN
			DCD MUX_MODE_5	
			DCD IOMUXC_SW_MUX_CTL_PAD_KPP_ROW0	   	   ;Change Mux to GPIO for PSOC_PWR_ENA
			DCD MUX_MODE_5
			DCD IOMUXC_SW_PAD_CTL_PAD_KPP_ROW0	   	   ;Change Pad Setting for PSOC_PWR_ENA
			DCD PAD_CTL_NOPULL_INPUT	
			DCD IOMUXC_SW_MUX_CTL_PAD_NFRB	   	   	   ;Change Mux to GPIO for PWRDWN/nREADY (OEM)
			DCD MUX_MODE_5
			DCD IOMUXC_SW_PAD_CTL_PAD_NFRB	   	       ;Change Pad Setting for PWRDWN/nREADY (OEM)
			DCD PAD_CTL_NOPULL_INPUT	
			DCD IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0	   	   ;Change Mux to GPIO for nBEEPER_PWM (OEM)
			DCD MUX_MODE_5
			DCD IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0	   	   ;Change Pad Setting for nBEEPER_PWM (OEM)
			DCD PAD_CTL_NOPULL_INPUT	
		ENDIF
	ENDIF 		;;END Cordless GPIO configuration Board_TARGET = "iMX25_Gen6_Board"				

	;----------------------------------------------------------------------------------	
;; ******************************
;; 	Clock Control / Clock Gating
;; ****************************** 
	DCD	CRM_CCTL
	DCD	CRM_CCTL_SETTING
	DCD	CRM_CGR0
	DCD	CRM_CGR0_SETTING
	DCD	CRM_CGR1
	DCD	CRM_CGR1_SETTING
	DCD	CRM_CGR2
	DCD	CRM_CGR2_SETTING
	DCD	CRM_PDR0
	DCD	CRM_PDR0_SETTING
	DCD	CRM_PDR1
	DCD	CRM_PDR1_SETTING
	DCD	CRM_PDR2
	DCD	CRM_PDR2_SETTING
	DCD	CRM_PDR3
	DCD	CRM_PDR3_SETTING
	DCD	CRM_MCR
	DCD	CRM_MCR_SETTING

;; end of table 	
	DCD	0
	DCD	0
	
	ENDIF 		;;End of Device Config Table for platform_current = "imx25" 
	
	
	IF platform_current = "imx27"
;;********************************************************
;;Device Config Table
;;Table Layout:
;;			DCD - register base address 
;; 			DCD - register setting
;;********************************************************
Devive_configTab
;; ****************************
;; 	AHB-Lite IP Interface
;; ****************************
	DCD	AIPI1_PSR1_REG
	DCD	AIPI1_PSR1_SETTING
	DCD	AIPI1_PSR0_REG
	DCD	AIPI1_PSR0_SETTING
	DCD	AIPI2_PSR1_REG
	DCD	AIPI2_PSR1_SETTING
	DCD	AIPI2_PSR0_REG
	DCD	AIPI2_PSR0_SETTING
;; ****************************
;; 	System Drive Level
;; ****************************
	DCD	CSD0_ESD_ESDMISC 		;; base address of MISC register
	DCD	CSD0_ESDMISC_VAL3		;; configure MISC register, reset delay line with ddr disabled
	DCD	SYS_DSCR3	 			;; base address of register
	DCD	ALL_HIGH_STRENGTH		;; configure drive strength control register 3
	DCD	SYS_DSCR5	 			;; base address of register
	DCD	ALL_HIGH_STRENGTH		;; configure drive strength control register 5
	DCD	SYS_DSCR6	 			;; base address of register
	DCD	ALL_HIGH_STRENGTH		;; configure drive strength control register 6
	DCD	SYS_DSCR7	 			;; base address of register
	DCD	SYS_DSCR7_SETTING	 	;; configure drive strength control register 7
	DCD	SYS_DSCR8	 			;; base address of register
	DCD	SYS_DSCR8_SETTING		;; configure drive strength control register 8
	DCD	SYS_DSCR10	 			;; base address of register
	DCD	SYS_DSCR10_SETTING 		;; configure drive strength control register 10
	DCD	SYS_DSCR11	 			;; base address of register
	DCD	SYS_DSCR11_SETTING		;; configure drive strength control register 11
	
	IF Board_TARGET = "iMX27_FF1" 		;;Use GPIO settings for 16 bit wide bus FF1 board (one mem chip)
;; -- Data Register Settings --
		DCD GPIOA_DR
		DCD PORT_A_DR_SETTING
		DCD GPIOB_DR
		DCD PORT_B_DR_SETTING
		DCD GPIOC_DR
		DCD PORT_C_DR_SETTING
		DCD GPIOD_DR
		DCD PORT_D_DR_SETTING
		DCD GPIOE_DR
		DCD PORT_E_DR_SETTING
		DCD GPIOF_DR
		DCD PORT_F_DR_SETTING
;; -- GPIO IN USE Register Settings --
		DCD GPIOA_GIUS
		DCD PORT_A_GIUS_SETTING
		DCD GPIOB_GIUS
		DCD PORT_B_GIUS_SETTING
		DCD GPIOC_GIUS
		DCD PORT_C_GIUS_SETTING
		DCD GPIOD_GIUS
		DCD PORT_D_GIUS_SETTING
		DCD GPIOE_GIUS
		DCD PORT_E_GIUS_SETTING
		DCD GPIOF_GIUS
		DCD PORT_F_GIUS_SETTING
;; -- Output Configuration Register 1 Settings --
		DCD GPIOA_OCR1
		DCD PORT_A_OCR1_SETTING
		DCD GPIOB_OCR1
		DCD PORT_B_OCR1_SETTING
		DCD GPIOC_OCR1
		DCD PORT_C_OCR1_SETTING
		DCD GPIOD_OCR1
		DCD PORT_D_OCR1_SETTING
		DCD GPIOE_OCR1
		DCD PORT_E_OCR1_SETTING
		DCD GPIOF_OCR1
		DCD PORT_F_OCR1_SETTING
;; -- Output Configuration Register 2 Settings --
		DCD GPIOA_OCR2
		DCD PORT_A_OCR2_SETTING
		DCD GPIOB_OCR2
		DCD PORT_B_OCR2_SETTING
		DCD GPIOC_OCR2
		DCD PORT_C_OCR2_SETTING
		DCD GPIOD_OCR2
		DCD PORT_D_OCR2_SETTING
		DCD GPIOE_OCR2
		DCD PORT_E_OCR2_SETTING
		DCD GPIOF_OCR2
		DCD PORT_F_OCR2_SETTING
;; -- Data Direction Register Settings --
		DCD GPIOA_DDIR
		DCD PORT_A_DDIR_SETTING
		DCD GPIOB_DDIR
		DCD PORT_B_DDIR_SETTING
		DCD GPIOC_DDIR
		DCD PORT_C_DDIR_SETTING
		DCD GPIOD_DDIR
		DCD PORT_D_DDIR_SETTING
		DCD GPIOE_DDIR
		DCD PORT_E_DDIR_SETTING
		DCD GPIOF_DDIR
		DCD PORT_F_DDIR_SETTING
	ENDIF 		;;END Cordless GPIO configuration Board_TARGET = "iMX27_FF1"

;; ******************************
;; 	Clock Control / Clock Gating
;; ****************************** 
	DCD CRM_PCCR0
	DCD CRM_PCCR0_SETTING_DISABLE
	DCD CRM_PCCR1
	DCD CRM_PCCR1_SETTING_DISABLE
	DCD CRM_PCDR0
	DCD CRM_PCDR0_SETTING_DISABLE
	
;; end of table 	
	DCD	0
	DCD	0

	ENDIF 		;;End of Device Config Table for platform_current = "imx27" 

	IF platform_current = "mxl"
;;********************************************************
;;Device Config Table
;;Table Layout:
;;			DCD - register base address 
;; 			DCD - register setting
;;********************************************************
Devive_configTab
;; ****************************
;; 	Clock Settings
;; ****************************
	DCD	CSCR
	DCD	CSCR_SETTING
	DCD	GCCR
	DCD	0x0
	
	;; end of table 	
	DCD	0
	DCD	0
	ENDIF 		;;End of Device Config Table for platform_current = "mxl" 

	END


