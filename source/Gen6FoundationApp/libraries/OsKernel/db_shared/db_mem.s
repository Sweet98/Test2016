;;
;    Assembly defines and code to support memory initialization necessary being done with assembly code
;;
;;======================================================================================
;;	Revision information
;;======================================================================================
;; $RCSfile: db_shared/db_mem.s $
;; $Revision: 1.11.2.39 $
;; $Date: 2010/12/20 14:15:18EST $
;;======================================================================================
;;

     INCLUDE   db_memap.s
     INCLUDE   db_hdw.s      ; Definitions of SA-11x0 registers and bit settings

     EXPORT Fast_memcpy
     EXPORT	Set_ram_to_zero
     EXPORT	Test_ram_memory

	 IF ((platform_current = "imx25") || (platform_current = "imx27"))
	    EXPORT Init_ddr_sdram
     	EXPORT WhatIsMemSizeMB
		EXPORT DRAM1GIG
		EXPORT DRAM256M
		EXPORT DRAM128M

     ELSE	;;mxl
		IF platform_current = "mxl"
     		EXPORT	Init_SDRAM
     		EXPORT	IsMem8or16MB

		ENDIF
     ENDIF

     ;IMPORT Send_Address_in_r0			;;used for debug

     AREA MEMCFG, CODE, READONLY

;;
;;******************************************************************************************************
	IF ((platform_current = "imx25") || (platform_current = "imx27"))	;;DDR memory configuration routine
;;******************************************************************************************************
;;

;;
;;******************************************************************************************************
		IF Board_TARGET = "iMX27_Development" 		;;Use settings for 32 bit wide bus (two mem chips)
;;******************************************************************************************************
;;
;;DDR sdram configuration for mx27 lite demo board (MT46H32M16LFCK-75 @ 133mhz)
;;

ESD_ESDCONFIG_0		EQU 0xD8001004	 	;; CONFIG Register Address for CSD0
ESDCONFIG_0_VAL1	EQU 0x00695729	 	;; CONGIG Register settings

SDRAM_CSD_0_BASE 	EQU CSD0_BASE_ADDR	;; CSD0 base address 0xA0000000
DDR_SDRAM_A10 		EQU 0xA0000400	 	;; set A10 = 1 for all precharge, by adding 0x400 to CSD0 base address
DDR_SDRAM_GEN_DATA	EQU 0xDEADBEEF          ;; do not care generic data

ESD_ESDCTL_0 		EQU 0xD8001000	 	;; Control Register Address for CSD0
PRE_CH_ALL_CMD 		EQU 0x92220080	 	;; precharge mode control settings (SMODE=001)
AUTO_REFRESH_CMD	EQU 0xA2226080 		;; auto refresh mode control settings (SMODE=010)
SET_MOD_REG_CMD 	EQU 0xB2220080 		;; set mode register control settings (SMODE=011)
NORMAL_MODE 		EQU 0x82226080	 	;; normal mode control settings (SMODE=000)
MODE_REG_STANDARD	EQU 0xA0000033 		;; Memory standard register device dependent data on address (0x33 on address bus)
MODE_REG_EXTENDED	EQU 0xA4000000	 	;; Memory extended register device dependent data on address (0x00 on address bus)
						;; (0xA4...... sets the bank BA1 = 1)
;;
;;******************************************************************************************************
		ENDIF 		;;END of DDR memory configuration data section Board_TARGET = "iMX27_Development"
;;******************************************************************************************************
;;
;;
;;******************************************************************************************************
		IF (Board_TARGET = "iMX27_FF1")  		;;Use settings for 16 bit wide bus (one mem chip)
;;******************************************************************************************************
;;

ESD_ESDCONFIG_0		EQU 0xD8001004	 	;; CONFIG Register Address for CSD0
ESDCONFIG_0_VAL1	EQU 0x0029572C	 	;; CONGIG Register settings

SDRAM_CSD_0_BASE 	EQU CSD0_BASE_ADDR 	;; CSD0 base address 0xA0000000
DDR_SDRAM_A10 		EQU 0xA0000400	 	;; set A10 = 1 for all precharge, by adding 0x400 to CSD0 base address
DDR_SDRAM_GEN_DATA	EQU 0xDEADBEEF          ;; do not care generic data

ESD_ESDCTL_0 		EQU 0xD8001000	 	;; Control Register Address for CSD0
PRE_CH_ALL_CMD 		EQU 0x93210080	 	;; precharge mode control settings (SMODE=001)
AUTO_REFRESH_CMD	EQU 0xA3218080 		;; auto refresh mode control settings (SMODE=010)
SET_MOD_REG_CMD 	EQU 0xB3210080 		;; set mode register control settings (SMODE=011)
NORMAL_MODE 		EQU 0x83218080	 	;; normal mode control settings (SMODE=000)
MODE_REG_STANDARD	EQU 0xA0000033 		;; Memory standard register device dependent data on address (0x33 on address bus)
MODE_REG_EXTENDED	EQU 0xA4000000	 	;; Memory extended register device dependent data on address (0x00 on address bus)
						;; (0xA4...... sets the bank BA1 = 1)

;;
;;******************************************************************************************************
		ENDIF 		;;END of DDR memory configuration data section ((Board_TARGET = "iMX27_FF1") || (Board_TARGET = "iMX25_Development"))
;;******************************************************************************************************
;;

;;
;;******************************************************************************************************
		IF ((Board_TARGET = "iMX25_Gen6_Board") || (Board_TARGET = "iMX25_Development")|| (Board_TARGET = "iMX25_OEM")) 		;;Use settings for 16 bit wide bus (one mem chip)
;;******************************************************************************************************
SDRAM_CSD_0_BASE 	EQU CSD0_BASE_ADDR 			;; CSD0 base address 0x80000000
SMODE_PRE_CH		EQU 0x10000000								;; bit mask or precharge mode
SMODE_AUTO_RF		EQU	0x20000000								;; bit mask for auto refresh
SMODE_LOAD_MR		EQU 0x30000000								;; bit mask for load register
SMODE_MAN_SR		EQU 0x40000000								;; manual self refresh
REFRESH_DISABLE		EQU (:NOT:((7<<13)+0xc00))							;; bitmask to disable refresh

;;******************************************************************************************************
			;;Use settings for 16 bit wide bus (one mem chip) with 128M size
;;******************************************************************************************************
;;	1Gb mobile ddr
ESDCONFIG_0_VAL11G	EQU 0x00295720	 			;; CONGIG Register settings; tRC set to 20 clocks
NORMAL_MODE1G 		EQU 0x83218880	 			;; normal mode control settings (SMODE=000)
EXTMODE1G			EQU 0x04000000				;; Bank address 0

;NORMAL_MODE1G 		EQU 0x83218080	 			;; normal mode control settings (SMODE=000), low power dissabled
		
;;******************************************************************************************************
OEM			;;Use settings for 16 bit wide bus (one mem chip) with 32M size
;;******************************************************************************************************
;;
ESDCONFIG_0_VAL1256	EQU 0x00095729	 			;; CONGIG Register settings; tRC set to 20 clocks
NORMAL_MODE256 		EQU 0x82116880	 			;; normal mode control settings (SMODE=000)

	IF :DEF: OEMBOOT
EXTMODE256			EQU 0x01018000              ;;1/8 Drive Strength Bank address 0
	ELSE
EXTMODE256			EQU 0x01000000				;;Full Drive Strength Bank address 0
	ENDIF
;NORMAL_MODE256		EQU 0x82116080	 			;; normal mode control settings (SMODE=000), low power dissabled

;;******************************************************************************************************
			;;Use settings for 16 bit wide bus (one mem chip) with 16M size
;;******************************************************************************************************
;;
;;ESDCONFIG_0_VAL1128	EQU 0x0029572f	 							;; CONGIG Register settings; tRC set to 20 clocks
;;config Winbon 16M LPDDR Lincoln Lu@2011.10.18
ESDCONFIG_0_VAL1128	EQU 0x00095729  ;;for 133MHZ, set tRC set to 10 clocks, tRAS set to 6 clocks, tWR set to 2 clocks, tMRD set to 2 clocks, tRP set to 3 clocks, tWTR set to 1 clocks, tXP set to 1 clocks,
NORMAL_MODE128 		EQU 0x81114880	 							;; normal mode control settings (SMODE=000)
EXTMODE128		EQU 0x00800000								;; Bank address 0

;NORMAL_MODE128		EQU 0x81114080	 							;; normal mode control settings (SMODE=000), low power dissabled

;;
;;******************************************************************************************************
		ENDIF 		;;END of DDR memory configuration data section Board_TARGET =
					;;     "iMX25_Development", "iMX25_Gen6_Board"
;;******************************************************************************************************
;;
DRAM128M
	DCD	NORMAL_MODE128			;control register value
	DCD	ESDCONFIG_0_VAL1128 	;config register value
	DCD	ESDMISC_VAL1 			;misc register value
	DCD	0x33					;ddr mode register settings
	DCD	EXTMODE128				;set BA1 for extended mode reg all other 0

DRAM256M
	DCD	NORMAL_MODE256			;control register value
	DCD	ESDCONFIG_0_VAL1256 	;config register value
	DCD	ESDMISC_VAL1 			;misc register value
	DCD	0x33					;ddr mode register settings
	DCD	EXTMODE256				;set BA1 for extended mode reg all other 0

DRAM1GIG
	DCD	NORMAL_MODE1G			;control register value
	DCD	ESDCONFIG_0_VAL11G	 	;config register value
	DCD	ESDMISC_VAL1 			;misc register value
	DCD	0x33					;ddr mode register settings
	DCD	EXTMODE1G				;set BA1 for extended mode reg all other 0

;; Initialze DDR memory on CS0
;; R0 has pointer to parameter block e.g DRAM256M above
;; during execution registers are:
;; R0 Control register value
;; R1 Config register value
;; R2 Misc Register value
;; R3 Mode register
;; R4 Extended mode register
;; R5 CSD0 base address
;; R6 scratch
;; R7 Dram controller register base address

Init_ddr_sdram
	mov		r5,r0
	ldmia	r5,{r0,r1,r2,r3,r4}
	ldr 	r7,=ESDCTL_BASE_ADDR		;; base address of MISC register
	ldr 	r5,=SDRAM_CSD_0_BASE 		;; CSD0 base address
	orr		r6,r2,#(1<<1)+(1<<3)		;; set bits 1 and 3 to reset controller and delay line
	str 	r6,[r7,#ESDMISC_OFFSET]		;; Reset the ESDRAMC
wait_ddr_ready
	ldr 	r6,[r7,#ESDMISC_OFFSET]		;; Reset the ESDRAMC
	tst 	r6,#(1<<31)					;; read from MISC register to see if ready (after 200 uSec delay finished)
	beq 	wait_ddr_ready              ;; if still zero, then branch till set
	nop									;; need one NOP here per spec
	str 	r1,[r7,#ESDCFG0_OFFSET] 	;; write to configure CONFIG0 register

	orr 	r6,r0,#SMODE_PRE_CH 		;; settings for precharge mode SMODE=001
	and		r6,r6,#REFRESH_DISABLE		;; disable precharge etc
	str 	r6,[r7,#ESDCTL0_OFFSET] 	;; write control register settings for precharge mode

	str 	r1,[r5,#0x400] 				;; write to precharge CSD0 A10 =1 means all banks

	orr 	r6,r0,#SMODE_AUTO_RF 		;; auto refresh mode SMODE=010
;; note some examples from FS show aut_RF disabled when sending this command our old code leaves the these on
;; removing the comment below masks the auto refresh timer settings off
	and		r6,r6,#REFRESH_DISABLE		;; disable precharge etc
	str 	r6,[r7,#ESDCTL0_OFFSET] 	;; write control register settings for auto refresh mode

	str 	r6,[r5] 					;; write to run auto-refresh cycle to array CSD0 #1 data is dont care
	str 	r6,[r5] 					;; write to run auto-refresh cycle to array CSD0 #2

	orr 	r6,r0,#SMODE_LOAD_MR 		;; settings for load mode registers SMODE=011
	and		r6,r6,#REFRESH_DISABLE		;; disable precharge etc
	str 	r6,[r7,#ESDCTL0_OFFSET] 	;; write control register settings for load mode registers mode
	strb 	r6,[r3,r5] 					;; write to set standard mode register value
	strb 	r6,[r4,r5] 					;; write to set standard mode register value

	str 	r0,[r7,#ESDCTL0_OFFSET] 	;; write to setup CSD0 for normal operation

;;
;;Delay Added to prevent memory failures at startup.  When boot runs from iRAM of the iMX25
;; this delay is needed.  Believed to be needed to account for the wait time after the
;; extended mode register is written
;;
	ldr 	r6,=0x60 					;; load loop counter, each loop should be one refresh cycle
post_init_delay
	strh 	r5,[r5] 					;; write somthing to set up csd0 dq pins
	subs 	r6,r6,#1 					;; decrease counter value
	bne 	post_init_delay
	bx		lr                       ;; return


;;
;;******************************************************************************************************
	ELSE            ;;MXL
;;******************************************************************************************************
;;
;;Configuration that is specific for the MXL SDRAM
;;

;; Sdram related internal registers
SCTL0          EQU  0x00221000     ;; SDRAm 0 Control register
SCTL1          EQU  0x00221004     ;; SDRAm 1 Control register
SDRST          EQU  0x00221018     ;; SDRAm Reset register
MISCELLANEOUS  EQU  0x00221014     ;; Miscellaneous register (says so right in the spec!!)

;; More sequential sdram setup
PRE_ALL_CMD       			EQU     0x91028300     	;; (register data)
AUTO_REF_CMD      			EQU     0xA1028300     	;; (register data)
SET_MODE_REG_CMD   			EQU     0xB1028300	;; (register data)
EDDY2_MODE_REG_VAL_0   		EQU     0x0808CC00   	;; (nCSD0 address and data)
NORMAL_MODE_CMD     		EQU     0x81028300    	;; (register data)
NORMAL_MODE_CMD_8_BY_11 	EQU     0x80028300    	;; (register data)

MODE_REG_VAL_0      		EQU  	0x08038000
EDDY_MODE_REG_VAL_0 		EQU  	0x08033000

SDRAM_ARRAY_0       		EQU 	0x08000000
SDRAM_ARRAY_0_PA    		EQU 	0x08ffffff

;; FMCR
FMCR      			EQU  	0x0021b808

;; Bit defines for the internal registers
SDE            			EQU  	BIT31		;; SDRAM Controller Enable


Init_SDRAM

     ldr 	r0,=FMCR
     ldr 	r1,=BIT0
     ldr 	r1,[r0]    ;; make sure sdram cs is enabled

     ldr 	r0,=SCTL0
     ldr 	r1,[r0]
     orr  	r1,r1,#SDE
     str 	r1,[r0]    ;; set bit

     ldr  	r2,=SCTL0         ;; base address of SDRAM internal reg's --> r2 throughout rest of function

;; put array 0/1 in precharge command mode
     ldr	r3,=PRE_ALL_CMD
     str	r3,[r2]

;; precharge array 0/1
     ldr	r4,=SDRAM_ARRAY_0_PA    ;; address of first array -> r4, address of 2nd --> r5
     str	r3,[r4]

;; put array 0/1 in auto refresh mode
     ldr	r3,=AUTO_REF_CMD
     str	r3,[r2]

;; run auto refresh cycle for array 0/1 -- 8 cycles necessary
;; do something 8 times
     ldr  	r4,=SDRAM_ARRAY_0
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]
     ldr        r3,[r4]


;; set SDRAM ctl registers for mode register write
     ldr	r3,=SET_MODE_REG_CMD
     str   	r3, [r2]

;; Array 0/1 mode register valu, new mode register value on address bus
     ldr  	r3,=EDDY2_MODE_REG_VAL_0;;EDDY2_MODE_REG_VAL_0
     str  	r3,[r3]

;; Put SDCTL regs in normal mode
     ldr  	r3,=NORMAL_MODE_CMD
     str   	r3,[r2]

;; wait a little time before doing anything
     mov 	r4,#0x200

waitram3
     subs  	r4,r4,#1
     bne 	waitram3

    bx 	lr		;;return
;;
;;******************************************************************************************************
	ENDIF	;;end sdram setup
;;******************************************************************************************************

;;******************************************************************************************************
;; sdram memory test values
;;
LOW_MEM_TEST_VALUE	EQU 0x5a5a1234    ;; r4 temp holds low mem test value
HIGH_MEM_TEST_VALUE	EQU 0xa5a54321    ;; r5 temp holds low mem test value

;;
;;******************************************************************************************************
	IF ((platform_current = "imx25") || (platform_current = "imx27"))
;;******************************************************************************************************
;;
;;Mem Size Test
;;
;; bare bones check, don't want to waste any time actually testing memory,
;; so just write a couple locations, enough to no difference between NO mem / 128MB
;;
;; Exit -->
;; Shared table entry SIZE_OF_SDRAM_S has been loaded with an error value corresponding to SDRAM size found in the mem size test,
;; and the results are represented by the following list:
;;
;;         0 --> write to low location did not read --> no RAM?  (seems unlikely with RAM test done just before calling this)
;; 	MXL returns:
;;         1 --> write to low location read, write to high location also read at low location --> 8MB
;;         2 --> write to low location reads, write to high location reads --> 16mB
;;         3 --> write to low location reads, write to high doesn't read --> no idea, treat in later life as 8MB, but be very afraid!!
;;	Imx27 returns:
;;         1 --> write to low location reads, write to high location reads --> 8MB
;;         2 --> write to low location reads, write to high location reads --> 16mB
;;         4 --> write to low location reads, write to high location reads --> 32MB
;;         5 --> write to low location reads, write to high location reads --> 128MB
;;         6 --> write to low location reads, write to high doesn't read --> no idea, have less than 8MB, but be very afraid!!
;;

ONE_TWO_EIGHT_MEM_ERROR_ADDR_A		EQU 0x06000000    ;; error address #A for 128M test, which correctly writes to 0x07000000
ONE_TWO_EIGHT_MEM_ERROR_ADDR_B		EQU 0x01000000    ;; error address #B for 128M test, which correctly writes to 0x07000000
THREE_TWO_MEM_ERROR_ADDR_A		EQU 0x01800000    ;; error address #A for 32M test, which correctly writes to 0x01C00000
THREE_TWO_MEM_ERROR_ADDR_B		EQU 0x00C00000    ;; error address #B for 32M test, which correctly writes to 0x01C00000
ONE_SIX_MEM_ERROR_ADDR			EQU 0x00600000    ;; error address for 16M test, which correctly writes to 0x00E00000      //lmc  this one value may not be correct - test


WhatIsMemSizeMB

	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_128M
	sub 	r2,r2,#SZ_16M				;; r2 holds temp variable of last 16M area of 128MB, if it exists
	ldr 	r3,=HIGH_MEM_TEST_VALUE   	;; r3 temp holds high mem test value
	str 	r3,[r2]         			;; write high mem test value to high mem
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_32M
	sub 	r2,r2,#SZ_4M				;; r2 holds temp variable of last 4M area of 32MB, if it exists
	str 	r3,[r2]         			;; write high mem test value to high mem
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_16M
	sub 	r2,r2,#SZ_2M				;; r2 holds temp variable of last 2M area of 16MB, if it exists
	str 	r3,[r2]         			;; write high mem test value to high mem
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_8M
	sub 	r2,r2,#SZ_1M				;; r2 holds temp variable of last 1M area of 8MB, if it exists
	str 	r3,[r2]         			;; write high mem test value to high mem
	ldr 	r3,=LOW_MEM_TEST_VALUE    			;; r3 temp holds low mem test value
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	add 	r1,r1,#ONE_TWO_EIGHT_MEM_ERROR_ADDR_A
	str 	r3,[r1]    					;; store low mem test value out to 128M ERROR memory location second, so it writes over last value if in lower size
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	add 	r1,r1,#ONE_TWO_EIGHT_MEM_ERROR_ADDR_B
	str 	r3,[r1]    					;; store low mem test value out to 128M ERROR memory location second, so it writes over last value if in lower size
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	add 	r1,r1,#THREE_TWO_MEM_ERROR_ADDR_A
	str 	r3,[r1]    					;; store low mem test value out to 32M ERROR memory location second, so it writes over last value if in lower size
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	add 	r1,r1,#THREE_TWO_MEM_ERROR_ADDR_B
	str 	r3,[r1]    					;; store low mem test value out to 32M ERROR memory location second, so it writes over last value if in lower size
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	add 	r1,r1,#ONE_SIX_MEM_ERROR_ADDR
	str 	r3,[r1]    					;; store low mem test value out to 16M ERROR memory location second, so it writes over last value if in lower size
	ldr 	r1,=ExternalRamBase				;; r1 temp variable has first location in RAM
	str 	r3,[r1]    					;; store low mem test value out to low memory location second, so it writes over last value if mirrored
	ldr 	r0,[r1]    					;; read low mem
	cmp 	r3,r0      					;; compare read with write value
    beq  	RamLowOk        			;; branch if read back of low mem was OK

;;
;; total failure, even write to the first address in Ram failed ... by the way if this fails, the mem test should have failed!!
;; But in the event that it does fail, inform the rest of the system by storing a 0 in the shared table entrys associated with SDRAM size
;;
	ldr 	r1,=0						;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=0          				;;save to SIZE_OF_SDRAM_S, the sdram size error information
    b 		RamTestFinished

RamLowOk
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_128M
	sub 	r2,r2,#SZ_16M				;; r2 holds temp variable of last 16M area of 128MB, if it exists
	ldr 	r0,[r2]         			;; read high mem back
	ldr 	r3,=HIGH_MEM_TEST_VALUE   	;; r3 temp holds high mem test value
	cmp 	r0,r3
	beq 	got128MBofRAM    			;; branch if read back of upper mem of 128M was OK
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_32M
	sub 	r2,r2,#SZ_4M				;; r2 holds temp variable of last 4M area of 32MB, if it exists
	ldr 	r0,[r2]         			;; read high mem back
	cmp 	r0,r3
	beq 	got32MBofRAM    			;; branch if read back of upper mem of 32M was OK
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_16M
	sub 	r2,r2,#SZ_2M				;; r2 holds temp variable of last 2M area of 16MB, if it exists
	ldr 	r0,[r2]         			;; read high mem back
	cmp 	r0,r3
	beq 	got16MBofRAM    			;; branch if read back of upper mem of 16M was OK
	ldr 	r2,=ExternalRamBase
	add 	r2,r2,#SZ_8M
	sub 	r2,r2,#SZ_1M				;; r2 holds temp variable of last 1M area of 8MB, if it exists
	ldr 	r0,[r2]         			;; read high mem back
	cmp 	r0,r3
	beq 	got8MBofRAM    			;; branch if read back of upper mem of 8M was OK

;; if I got here I have no idea... must have less memory than 8MB, but why?
;; so return a six in SIZE_OF_SDRAM_S to question results, and 0M (0x00000000) in MEMORY_SIZE for unknown size
;;
	ldr 	r1,=0						;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=6						;;save to SIZE_OF_SDRAM_S, the size return information
	b    	RamTestFinished

;; Return a five in SIZE_OF_SDRAM_S to indicate 128 meg, and 1288M (0x08000000) in MEMORY_SIZE for actual size found
;;
got128MBofRAM
	ldr 	r1,=SZ_128M	;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=5		;;save to SIZE_OF_SDRAM_S, the size error information
	b    	RamTestFinished

;; Return a four in SIZE_OF_SDRAM_S to indicate 32 meg, and 32M (0x02000000) in MEMORY_SIZE for actual size found
;;
got32MBofRAM
	ldr 	r1,=SZ_32M	;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=4		;;save to SIZE_OF_SDRAM_S, the size error information
	b    	RamTestFinished

;; Return a two in SIZE_OF_SDRAM_S to indicate 16 meg, and 32M (0x01000000) in MEMORY_SIZE for actual size found
;;
got16MBofRAM
	ldr 	r1,=SZ_16M	;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=2		;;save to SIZE_OF_SDRAM_S, the size error information
	b    	RamTestFinished

;; Return a one in SIZE_OF_SDRAM_S to indicate 8 meg, and 8M (0x00800000) in MEMORY_SIZE for actual size found
;;
got8MBofRAM
	ldr 	r1,=SZ_8M	;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=1		;;save to SIZE_OF_SDRAM_S, the size error information

RamTestFinished
	ldr 	r3,=MEMORY_SIZE                 ;; r3 has the shared table location to store the resulting actual size
	str 	r1,[r3]
	ldr 	r3,=SIZE_OF_SDRAM_S		;; r3 has the shared table location to store the resulting sdram size error information
	str 	r0,[r3]

	 bx		lr                       	;;return

;;
;;******************************************************************************************************
	ELSE            ;;MXL
;;******************************************************************************************************
;;
;;******************************************************************************************************
;;
;;MXL Mem Size Test
;;
;; bare bones check, don't want to waste any time actually testing memory,
;; so just write a couple locations, enough to no difference between NO mem/ 128MB / 16MB
;;
;; Exit -->
;;    shared table entry has been loaded with a value corrseponding to SDRAM (SIZE_OF_SDRAM_S) size per:
;;         0 --> write to low location did not read --> no RAM?  (seems unlikely with RAM test done just before calling this)
;;         1 --> write to low location read, write to high location also read at low location --> could be 8MB
;;         2 --> write to low location reads, write to high location reads --> 16mB?
;;         3 --> write to low location reads, write to high doesn't read --> no idea, treat in later life as 8MB, but be very afraid!!
;;
IsMem8or16MB

	ldr 	r1,=ExternalRamBase			;; r1 temp has first location in RAM
	mov 	r2,r1
	add 	r2,r2,#SZ_8M				;; r2 holds temp of 1str location of upper 8MB if it exists
	ldr 	r3,=HIGH_MEM_TEST_VALUE   	;; r3 temp holds high mem test value
	str 	r3,[r2]         			;; write high mem test value to high mem
	ldr 	r3,=LOW_MEM_TEST_VALUE    	;; r3 temp holds low mem test value
	str 	r3,[r1]    					;; store low mem test value out to low memory location
	ldr 	r0,[r1]    					;; read low mem
	cmp 	r3,r0      					;; compare read with write value
	beq  	Ram0Ok          			;; branch if read back of low mem was OK
;;
;; total failure, even write to the first address in Ram failed ... by the way if this fails, the mem test should have failed!!
;; But in the event that it does fail, inform the rest of the system by storing a 0 in the shared table entrys associated with SDRAM size
;;
	ldr 	r1,=0						;;save to MEMORY_SIZE, the actual mem size
	ldr 	r0,=0          				;;save to SIZE_OF_SDRAM_S, the sdram size error information
	b 	QuestionAnswered

Ram0Ok
	ldr 	r0,[r2]         			;; read high mem back
	ldr 	r3,=HIGH_MEM_TEST_VALUE   	;; r3 temp holds high mem test value
	cmp 	r0,r3
	beq 	got16MBofRAM    ;; branch if read back of upper mem was OK
;; Return a one in SIZE_OF_SDRAM_S to indicate 8 meg, and 8M (0x00800000) in MEMORY_SIZE for actual size
;;
got8MBofRAM
;; switch normal mode to 8 by 11
	ldr 	r1,=SCTL0
	ldr  	r3,=NORMAL_MODE_CMD_8_BY_11
	str   	r3,[r1]

;; please leave in this wait.  The spec doesn't demand it, but I see memory errors if I don't wait.
;; Wait a little time before doing anything.
;;
	mov 	r0,#0x200
waitram8
	subs  	r0,r0,#1
	bne 	waitram8
	ldr 	r0,=1
	ldr 	r1,=SZ_8M
  	b    	QuestionAnswered

;; Return a two in SIZE_OF_SDRAM_S to indicate 16 meg, and 16M (0x01000000) in MEMORY_SIZE for actual size
;;
got16MBofRAM
	ldr 	r0,=2
	ldr 	r1,=SZ_16M

QuestionAnswered
	ldr 	r3,=MEMORY_SIZE                 ;; r3 has the shared table location to store the resulting actual size
	str 	r1,[r3]
	ldr 	r3,=SIZE_OF_SDRAM_S		;; r3 has the shared table location to store the resulting sdram size error information
	str 	r0,[r3]
	bx 	lr                       	;;return


;;
;;******************************************************************************************************
	ENDIF	;;end memory size check
;;******************************************************************************************************
;;

;;
;;******************************************************************************************************
	IF MEMORY_DEBUG		;;Turn on memory debug tests in Db_hdw.s
;;******************************************************************************************************
;;

;;
;; This will zero out all of RAM for debug and test purposes.
;; Must be ran only after memory size has been found and stored in shared table
;;
;;Returns in r0:
;;	0 for memory fill passed
;;	1 for no memory available - failed

;;

Set_ram_to_zero

	ldr	r1,=MEMORY_SIZE
	ldr	r3,[r1]				;;load r3 with memory size from shared table
     	ldr 	r1,=ExternalRamBase		;; r1 temp has first location in RAM
     	ldr 	r2,=ExternalRamBase
     	add 	r2,r2,r3			;; r2 temp holds last location +1 of ram if it exists
     	ldr 	r0,=0				;; value to store in memory
	cmp	r3,r0
	beq	Zero_test_error			;;if no memory found (0 in shared table), then lets get out of here

	;;push shared table
	ldr 	r0,=MEMORY_SIZE                 ;; r0 temp has the shared table location to store the resulting actual sdram size
	ldr 	r3,[r0]                         ;; save shared table MEMORY_SIZE to r3
	ldr 	r0,=SIZE_OF_SDRAM_S		;; r0 temp has the shared table location to store the resulting size error information
	ldr 	r12,[r0]                        ;; save shared table SIZE_OF_SDRAM_S to r12

	ldr 	r0,=0				;; restore value to store in memory

Zero_RAM_loop
	str	r0, [r1], #4			;;store zero and post increment address (r1) by a word
	cmp	r2,r1
	bls	Zero_test_pass			;;finished zeroing data, lets get out of here
	bne	Zero_RAM_loop

Zero_test_error
	ldr 	r0,=1				;;return 1 for error
	b	Zero_test_finish

Zero_test_pass
	;;pop shared table
	ldr 	r0,=SIZE_OF_SDRAM_S		;; r0 temp has the shared table location to store the resulting size error information
	str 	r12,[r0]                        ;; save shared table SIZE_OF_SDRAM_S  that was stored in r12
	ldr 	r0,=MEMORY_SIZE                 ;; r0 temp has the shared table location to store the resulting actual size
	str 	r3,[r0]                         ;;save shared table MEMORY_SIZE that was stored in r3

	ldr 	r0,=0				;;return 0 for a pass

Zero_test_finish
	bx 	lr                       	;;return


;;
;; This will test all of RAM in 1k jumps for debug and test purposes.
;;Must be ran only after memory size has been found and stored in shared table
;;
;;Returns in r0:
;;	0 for memory pass
;;	1 for no memory available
;;	2 for memory error
;;
;;Note: this test violates 'C' standards and returns failed values in r1, r2, and r3 (this is so I do not use stack)
;;
Test_ram_memory

	;;set test parameters
	ldr	r1,=MEMORY_SIZE
	ldr	r2,[r1]				;;load r2 with memory size from shared table
	ldr 	r1,=ExternalRamBase             ;;load r1 with the location of the start of memory
     	ldr 	r0,=0x0                         ;;load temp value r0 with 0
	cmp	r2,r0
	beq	dram_test_error			;;if no memory found (0 in shared table), then lets get out of here

	;;push shared table, only restored on a pass condition
	mov 	r12,r2				;; save shared table MEMORY_SIZE in r2 to r12 (actual size), using upper 7 nibbles
	ldr 	r0,=SIZE_OF_SDRAM_S		;; r0 temp has the shared table location to store the resulting size error information
	ldr 	r3,[r0]				;; save shared table SIZE_OF_SDRAM_S to temp r3
	add	r12,r12,r3			;; add r3 to r12 to store SIZE_OF_SDRAM_S  in the lowest nibble of r12

	ldr 	r0,=0				;; restore value to store in memory

write_ram_w_incrementing
    	str 	r0,[r1]				;;store 0 to start of ram
	add 	r1,r1,#1024                     ;;increment memory location by 1k
	add 	r0,r0,#1024			;;add 1k to temp value
	subs 	r2,r2,#1024			;;subtract 1k from memory size and set cond flags
	bls	out_of_mem			;;branch to out of memory if results are: operand1 <= operand2
	add 	r1,r1,#4
    	add 	r0,r0,#1
    	subs 	r2,r2,#4
	bls	out_of_mem			;;branch to out of memory if results are: operand1 <= operand2
     	bne 	write_ram_w_incrementing

out_of_mem
	;;reset the test parameters
	ldr	r1,=MEMORY_SIZE
	ldr	r2,[r1]				;;load r2 with memory size from shared table
	ldr 	r1,=ExternalRamBase             ;;load r1 withthe location of the start of memory
     	ldr 	r0,=0x0                         ;;load temp value r0 with 0

read_ram_w_incrementing
     	ldr 	r3,[r1]				;;move value in memory to r3
	cmp 	r3,r0
     	bne 	dram_fault			;;branch if values do not match

     	add 	r1,r1,#1024
     	add 	r0,r0,#1024
     	subs 	r2,r2,#1024
	bls	dram_no_fault			;;branch to passed memory test if results are: operand1 <= operand2 (made it all the way)
	add 	r1,r1,#4
     	add 	r0,r0,#1
	subs 	r2,r2,#4
	bls	dram_no_fault			;;branch to passed memory test if results are: operand1 <= operand2 (made it all the way)
     	bne 	read_ram_w_incrementing

dram_fault

	mov	r2,r0				;;save what the should have been in r2
	;;
	;; r3 now has value read from mem
	;; r2 now has what should have been read in memory
	;; r1 now has the memory location that failed
	;;

	;;
	;;** Don't send any message if RAM FAILED. Not using stack **
	;;
	;bl	Send_CRLF
	;mov 	r0,r1			;; location
	;bl 	Send_Address_in_r0
	;mov 	r0,r2			;; value should have been
	;bl 	Send_Address_in_r0
	;mov 	r0,r3			;; value read
	;bl 	Send_Address_in_r0
	;bl	Send_CRLF

	;;
	;;pop shared table
	;;
	;; Shared table entry SIZE_OF_SDRAM_S has been loaded with an error value corresponding to SDRAM size found in the mem size test,
	;; and the results are represented by the following list:
	;;
	;;         0 --> write to low location did not read --> no RAM?  (seems unlikely with RAM test done just before calling this)
	;; 	MXL returns:
	;;         1 --> write to low location read, write to high location also read at low location --> 8MB
	;;         2 --> write to low location reads, write to high location reads --> 16mB
	;;         3 --> write to low location reads, write to high doesn't read --> no idea, treat in later life as 8MB, but be very afraid!!
	;;	Imx27 returns:
	;;         4 --> write to low location reads, write to high doesn't read --> no idea, have less than 128MB, but be very afraid!!
	;;         5 --> write to low location reads, write to high location reads --> 128MB
	;;
	;; If error occured, lets change the shared table values that were found in function WhatIsMemSizeMB, ran previously,
	;; and replace them with the correct error condition that represents current error.
	;;
	ldr	r12,=0				;; load error condition 0x0 into r12, which also represents 0x0 sdram memory available
	ldr 	r0,=SIZE_OF_SDRAM_S		;; r0 temp has the shared table location to store the resulting size error information
	str 	r12,[r0]                        ;; save shared table SIZE_OF_SDRAM_S , to indicate failure error condition of 0x0
	ldr 	r0,=MEMORY_SIZE                 ;; r0 temp has the shared table location to store the resulting actual size of dram
	str 	r12,[r0]                         ;;save shared table MEMORY_SIZE , to indicate no memory available (0 MB available)

	ldr 	r0,=2				;;load r0 with a return value of  2 to indicate a failure
	b	dram_exit

dram_test_error
	ldr 	r0,=1				;;load r0 with a return value of  1 to indicate a test error
	ldr 	r3,=0x0				;;zero other registers since we had no memory to test
	ldr 	r2,=0x0
	ldr 	r1,=0x0

	b	dram_exit

dram_no_fault
	;;
	;;** Don't send any message if RAM is PASSED. Not using stack **
	;;
	;bl	Send_RAM_Pass

	;;pop shared table
	ldr 	r0,=SIZE_OF_SDRAM_S		;; r0 temp has the shared table location to store the resulting size error information
	mov	r3,r12                          ;; save r12 to temp r3
	and	r3,r3,#0x0000000F		;; Get the last nibble of r12 with mask
	str 	r3,[r0]				;; save shared table SIZE_OF_SDRAM_S  that was stored in last nibble of r12
	ldr 	r0,=MEMORY_SIZE                 ;; r0 temp has the shared table location to store the resulting actual size
	mov	r2,r12				;; save r12 to temp r2
	sub     r2,r2,r3			;; subtract out lower nibble to get original value for mem size
	str 	r2,[r0]                         ;; save shared table MEMORY_SIZE that was stored in the upper 7 nibbles of r12

	ldr 	r0,=0x0                         ;;load r0 with a return value of  0 to indicate a pass

dram_exit
	bx	lr                       	;;return


;;
;;******************************************************************************************************
	ELSE	;;Dummy test stubs that always returns a pass
;;******************************************************************************************************
;;
Test_ram_memory
Set_ram_to_zero
	ldr 	r0,=0				;;return 0 for pass

	bx		lr                       	;;return


;;
;;******************************************************************************************************
	ENDIF	;;end memory debug tests
;;******************************************************************************************************
;;

;;
;;******************************************************************************************************
;;
;; Do a fast copy (44 bytes at a time).  Arguments are as follows:
;;	R0 - Destination address
;;	R1 - Source address
;;	R2 - Number of bytes to copy
;;
;;	NOTE:	Make sure that the only data memory usage by this routine is on the stack.  This routine may be used to relocate code and may scribble
;;			on data space.  This also implies that constants should be one byte.
;;
	AREA	FASTMEMCPY,CODE,READONLY
Fast_memcpy
	stmdb	sp!,{r0-r12,lr}

	mov	r11,r0
	mov	r12,r1
	mov	r14,r2

;  If either the source or destination address is not longword aligned, do a single
;  byte (SLOW!) copy.
;;
	and	r0,r0,#3
	cmp	r0,#0
	bne	Fast_memcpy_copy_single_bytes_at_a_time
	and	r1,r1,#3
	cmp	r1,#0
	bne	Fast_memcpy_copy_single_bytes_at_a_time

	cmp	r14,#44
	blt	Fast_memcpy_copy_single_bytes_at_a_time

Fast_memcpy_copy_until_we_have_less_than_44_bytes_left
	ldmia	r12!, {r0-r10}				; Copy 44 bytes at a time...
	stmia	r11!, {r0-r10}
	sub	r14,r14,#44
	cmp	r14,#44					; If we don't have at least 44 bytes left, let's go to a smaller copy
	bge	Fast_memcpy_copy_until_we_have_less_than_44_bytes_left

Fast_memcpy_copy_single_bytes_at_a_time
	cmp		r14,#0
	ble		Fast_memcpy_finish

Fast_memcpy_copy_until_no_bytes_are_left
	ldrb	r0,[r12],#1
	strb	r0,[r11],#1
	subs	r14,r14,#1
	bgt		Fast_memcpy_copy_until_no_bytes_are_left


Fast_memcpy_finish
	ldmia	sp!,{r0-r12,pc}



	END

