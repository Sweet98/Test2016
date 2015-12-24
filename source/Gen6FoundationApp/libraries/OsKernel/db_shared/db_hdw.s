;;
;    Code attempting to build from scratch assembly necessary internal regs for Dragonball processor
;;
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/db_hdw.s $
;; $Revision: 1.3.3.29 $
;; $Date: 2009/01/22 11:52:55EST $
;;======================================================================================

;;
;;Define flag to allow memory tests to run
;;
     GBLL	MEMORY_DEBUG
MEMORY_DEBUG	SETL	{FALSE}
;MEMORY_DEBUG	SETL	{TRUE}

;;
;;Define flag to allow for DDR memory width for use in configuration
;;
;;GBLL		Eval_Bd_with_MEMORY_32W
;;Eval_Bd_with_MEMORY_32W		SETL	{TRUE}
;;Eval_Bd_with_MEMORY_32W		SETL	{FALSE}

;;
;;This is set by assembler build options in "Mks_project_target_foundations.mk"
;;
;;GBLS    platform_current
;;platform_current	SETS	"mxl"		;;"mxl"
;;platform_current	SETS	"imx27"		;;"imx27"
;;platform_current	SETS	"imx25"		;;"imx25"
;;
;;
;;This is set by assembler build options in "Mks_project_target_foundations.mk"
;;
;;GBLS    Board_TARGET
;;Board_TARGET	SETS	"MXL_Matrix"			;;MXL Matrix Board (GEN 5)
;;Board_TARGET	SETS	"iMX27_Development"		;;Logic PD Development Board For iMX27
;;Board_TARGET	SETS	"iMX27_FF1"				;;Form Factor Board for iMX27
;;Board_TARGET	SETS	"iMX25_Development"		;;Development Board For iMX25 (plugs into Logic PD board)
;;Board_TARGET	SETS	"iMX25_Gen6_Board"		;;Cordless/Corded Form Factor Boards for iMX25
;;Board_TARGET	SETS	"iMX25_Trace"			;;Form Factor Board for iMX25 with Trace Lines initilized



;; ARM Program Status Register
;; 31  30  29  28  27  .........  7   6   5  4  3  2  1  0
;; ----------------------------------------------------------
;; | N | Z | C | V | Q |         | I | F | T |   M4 - M0    |
;; ----------------------------------------------------------
;;
;; #########################################
;; # ARM INTERRUPTS                        #
;; #########################################
INTERRUPT_BITS		EQU  0xC0
DISABLE_FIQ		EQU  0x40
DISABLE_IRQ		EQU  0x80
;;
;;Setup for the status register control bits, can only be changed on a privilaged mode
;;
I_Bit          EQU  0x80	;;disables IRQ interupts when set
F_Bit          EQU  0x40	;;disables FIQ interupts when set

;;
;; #########################################
;; # ARM MODES = {M4, M3, M2, M1, M0}      #
;; #########################################
;;
;; The M_bits for status register
;;
;; Some standard ARM v4T definitions, assuming for the moment none of this changes from StrongArm's implementation.
;; Current program status register (CSPR) bits.
;; Bits [4:0] are used to define the processor modes as detailed below.
;;
Mode_USR	EQU  0x10	;;User mode
Mode_FIQ	EQU  0x11	;;FIQ - fast interupt request,  a privilaged mode
Mode_IRQ	EQU  0x12	;;IRQ - interupt request,  a privilaged mode
Mode_SVC	EQU  0x13	;;Supervisor,  a privilaged mode
Mode_ABT	EQU  0x17	;;Abort,  a privilaged mode
Mode_UNDEF	EQU  0x1B	;;Undefined,  a privilaged mode
Mode_SYS	EQU  0x1F	;;System,  a privilaged mode , only available on ARM Arch. v4 and above
Mode_MON	EQU  0x16	;;Security Extensions only , only available on ARM Arch. v6 and above

;;
;; #########################################
;; # ARM CONDITION CODE FLAG = {N,Z,C,V,Q} #
;; #########################################
FLAG_BITS	EQU  0xF8000000
NFLAG		EQU  0x80000000
ZFLAG		EQU  0x40000000
CFLAG		EQU  0x20000000
VFLAG		EQU  0x10000000
QFLAG		EQU  0x08000000


;;
;;Platform Check
;;*************************************************************************

	IF platform_current = "imx25"
		INCLUDE iMX25_hdw.i
	ELSE
		IF platform_current = "imx27"
			INCLUDE iMX27_hdw.i
		ELSE
			IF platform_current = "mxl"
				INCLUDE MXL_hdw.i
			ELSE
				;;No Platform Defined
			ENDIF
		ENDIF
	ENDIF

;;*************************************************************************

;;*************************************************************************
;; Macros and common stuff
;;*************************************************************************
;;

;;
;; generic bit mask defines
;;
BIT0 		EQU		0x00000001
BIT1 		EQU		0x00000002
BIT2 		EQU		0x00000004
BIT3 		EQU		0x00000008
BIT4 		EQU		0x00000010
BIT5 		EQU		0x00000020
BIT6 		EQU		0x00000040
BIT7 		EQU		0x00000080
BIT8 		EQU		0x00000100
BIT9  		EQU		0x00000200
BIT10 		EQU		0x00000400
BIT11 		EQU		0x00000800
BIT12 		EQU		0x00001000
BIT13 		EQU		0x00002000
BIT14 		EQU		0x00004000
BIT15 		EQU		0x00008000
BIT16 		EQU		0x00010000
BIT17 		EQU		0x00020000
BIT18 		EQU		0x00040000
BIT19 		EQU		0x00080000
BIT20 		EQU		0x00100000
BIT21 		EQU		0x00200000
BIT22 		EQU		0x00400000
BIT23 		EQU		0x00800000
BIT24 		EQU		0x01000000
BIT25 		EQU		0x02000000
BIT26 		EQU		0x04000000
BIT27 		EQU		0x08000000
BIT28 		EQU		0x10000000
BIT29 		EQU		0x20000000
BIT30 		EQU		0x40000000
BIT31		EQU		0x80000000

;;
;; RESET_WATCHDOG_TIMER macro is made to accommodate relocatable code.  We do this by loading constants with no more than 8 contiguous bits set
;; at a time, in order to accommodate the 8 bit constant maximum.
;;
	MACRO
	RESET_WATCHDOG_TIMER
	stmdb	sp!,{r0-r1}			;; Save registers

	ldr		r0,=WATCHDOG_BASE_ADDR_UPPER	;; To get WSR address into r0, add one byte at a time to accommodate 8 bit constant maximum
	ldr		r1,=WATCHDOG_BASE_ADDR_LOWER
	add		r0,r0,r1
	ldr		r1,=WATCHDOG_BASE_ADDR_WSR_OFFSET
	add		r0,r0,r1

	ldr		r1,=0x5500
	add		r1,r1,#0x0055

	IF platform_current = "mxl"

	str		r1,[r0]		;; Write 0x5555 to start the reset sequence.  Build command one byte at a time to accommodate 8 bit constant maximum

	add		r1,r1,r1
	str		r1,[r0]		;; Write 0xAAAA to start the reset sequence.  Build command one byte at a time to accommodate 8 bit constant maximum

	ELSE
		IF ((platform_current = "imx27") || (platform_current = "imx25"))

	strh		r1,[r0]		;; Write 0x5555 to start the reset sequence.  Build command one byte at a time to accommodate 8 bit constant maximum

	add		r1,r1,r1
	strh		r1,[r0]		;; Write 0xAAAA to start the reset sequence.  Build command one byte at a time to accommodate 8 bit constant maximum

		ENDIF
	ENDIF

	ldmia	sp!,{r0-r1}			;; Restore registers
	MEND


;; FORCE_WATCHDOG_RESET macro is made to accommodate relocatable code.  We do this by loading constants with no more than 8 contiguous bits set
;; at a time, in order to accommodate the 8 bit constant maximum and make this code relocatable.
;; Watchdog must be enabled before calling this function (one time only write the bit to enable DOG on the imx27 platform).
;;
	MACRO
	FORCE_WATCHDOG_RESET

	stmdb	sp!,{r0-r1}				;; Save registers

	ldr		r0,=WATCHDOG_BASE_ADDR_UPPER	;; To get WCR address into r0, add one byte at a time to accommodate 8 bit constant maximum
	ldr		r1,=WATCHDOG_BASE_ADDR_LOWER
	add		r0,r0,r1

	ldr		r1,=0x0100			;; Write bit 8 to cause the timer to expire right away (short time set).

	IF platform_current = "mxl"

	str		r1,[r0]

	ELSE
		IF ((platform_current = "imx27") || (platform_current = "imx25"))

	strh		r1,[r0]

		ENDIF
	ENDIF

	b	{PC}					;;; Keep looping until we reset, branching to same location

	ldmia	sp!,{r0-r1}				;; Restore registers (we should have reset before we arrived here).
	MEND




	END



