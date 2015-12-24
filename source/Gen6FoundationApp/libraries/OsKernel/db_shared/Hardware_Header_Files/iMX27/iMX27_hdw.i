;***************************************************************************************/
;*
;* HSM 48X0G GENERATION 6 SOFTWARE
;*
;*  HARDWARE MACROS 
;*  Date:       9/29/2008
;*  Tools:      RVCT Build 674
;*  Target:     Freescale iMX27
;*
;*======================================================================================
;* Revision information
;*======================================================================================
;* $RCSfile: db_shared/Hardware_Header_Files/iMX27/iMX27_hdw.i $
;* $Revision: 1.1 $
;* $Date: 2008/09/29 14:28:36EDT $
;*======================================================================================
;*------------------------------------------------------------------------------
;* Header:  iMX27_hdw.i
;*
;* This File defines the iMX25 ASM Hardware
;*
;*------------------------------------------------------------------------------ 
  
;; *****************************************
;;Define the AHB-Lite registers and settings
;; *****************************************
;;
AIPI1_PSR1_REG			EQU     0x10000004     	;;AIPI1 - PSR1 REGISTER ADDRESS
AIPI1_PSR0_REG			EQU     0x10000000     	;;AIPI1 - PSR0 REGISTER ADDRESS
AIPI2_PSR1_REG			EQU     0x10020004     	;;AIPI2 - PSR1 REGISTER ADDRESS
AIPI2_PSR0_REG			EQU     0x10020000     	;;AIPI2 - PSR0 REGISTER ADDRESS

AIPI1_PSR1_SETTING		EQU     0xFFFBFCFB     	;;AIPI1 - PSR1 REGISTER PERIPHERAL WIDTH SETTING
AIPI1_PSR0_SETTING		EQU     0x00040304     	;;AIPI1 - PSR0 REGISTER PERIPHERAL WIDTH SETTING
AIPI2_PSR1_SETTING		EQU     0xFFFFFFFF     	;;AIPI2 - PSR1 REGISTER PERIPHERAL WIDTH SETTING
AIPI2_PSR0_SETTING		EQU     0x3FFC0000     	;;AIPI2 - PSR0 REGISTER PERIPHERAL WIDTH SETTING

	
;; #########################################
;; # Clock / Reset (CRM)                   #
;; # System control                        #
;; # $1002_7000 to $1002_7FFF              #
;; #########################################
CRM_BASE_ADDR		EQU		0x10027000
CRM_CSCR		EQU		(CRM_BASE_ADDR+0x00)       ;; 32bit Clock Source Control Reg
CRM_MPCTL0		EQU		(CRM_BASE_ADDR+0x04)       ;; 32bit MCU PLL Control Reg
CRM_MPCTL1		EQU		(CRM_BASE_ADDR+0x08)       ;; 32bit MCU PLL
CRM_SPCTL0		EQU		(CRM_BASE_ADDR+0x0C)       ;; 32bit Serial Perpheral PLL Ctrl 0
CRM_SPCTL1		EQU		(CRM_BASE_ADDR+0x10)       ;; 32bit Serial Perpheral PLL Ctrl 1
CRM_OSC26MCTL		EQU		(CRM_BASE_ADDR+0x14)       ;; 32bit Osc 26M register
CRM_PCDR0		EQU		(CRM_BASE_ADDR+0x18)       ;; 32bit Serial Perpheral Clk Div Reg 0
CRM_PCDR1		EQU		(CRM_BASE_ADDR+0x1C)       ;; 32bit Serial Perpheral Clk Div Reg 1
CRM_PCCR0		EQU		(CRM_BASE_ADDR+0x20)       ;; 32bit Perpheral Clk Control Reg 0
CRM_PCCR1		EQU		(CRM_BASE_ADDR+0x24)       ;; 32bit Perpheral Clk Control Reg 1
CRM_CCSR		EQU		(CRM_BASE_ADDR+0x28)       ;; 32bit Clock Control Status Reg
CRM_PMCTL		EQU		(CRM_BASE_ADDR+0x2C)       ;; 32bit PMOS Control Reg
CRM_PMCOUNT		EQU		(CRM_BASE_ADDR+0x30)       ;; 32bit PMOS Counter Reg
CRM_WKGDCTL		EQU		(CRM_BASE_ADDR+0x34)       ;; 32bit Wakeup Guard Mode Control Reg

SYS_BASE_ADDR		EQU		0x10027800
SYS_FMCR		EQU		(SYS_BASE_ADDR+0x14)       ;; Functional Muxing Control Reg
SYS_GPCR		EQU		(SYS_BASE_ADDR+0x18)       ;; Global Peripheral Control Reg
SYS_WBCR		EQU		(SYS_BASE_ADDR+0x1C)       ;; Well Bias Control Reg
SYS_DSCR1		EQU		(SYS_BASE_ADDR+0x20)       ;; Drive Strength Crtl Reg 1
SYS_DSCR2		EQU		(SYS_BASE_ADDR+0x24)       ;; Drive Strength Crtl Reg 2
SYS_DSCR3		EQU		(SYS_BASE_ADDR+0x28)       ;; Drive Strength Crtl Reg 3
SYS_DSCR4		EQU		(SYS_BASE_ADDR+0x2C)       ;; Drive Strength Crtl Reg 4
SYS_DSCR5		EQU		(SYS_BASE_ADDR+0x30)       ;; Drive Strength Crtl Reg 5
SYS_DSCR6		EQU		(SYS_BASE_ADDR+0x34)       ;; Drive Strength Crtl Reg 6
SYS_DSCR7		EQU		(SYS_BASE_ADDR+0x38)       ;; Drive Strength Crtl Reg 7
SYS_DSCR8		EQU		(SYS_BASE_ADDR+0x3C)       ;; Drive Strength Crtl Reg 8
SYS_DSCR9		EQU		(SYS_BASE_ADDR+0x40)       ;; Drive Strength Crtl Reg 9
SYS_DSCR10		EQU		(SYS_BASE_ADDR+0x44)       ;; Drive Strength Crtl Reg 10
SYS_DSCR11		EQU		(SYS_BASE_ADDR+0x48)       ;; Drive Strength Crtl Reg 11
SYS_DSCR12		EQU		(SYS_BASE_ADDR+0x4c)       ;; Drive Strength Crtl Reg 12
SYS_DSCR13		EQU		(SYS_BASE_ADDR+0x50)       ;; Drive Strength Crtl Reg 13
SYS_PSCR		EQU		(SYS_BASE_ADDR+0x54)       ;; Pull Strength Control Reg
SYS_PCSR		EQU		(SYS_BASE_ADDR+0x58)       ;; Priority Control/select Reg
SYS_PMCR		EQU		(SYS_BASE_ADDR+0x60)       ;; Power Management Control Reg
SYS_DCVR0		EQU		(SYS_BASE_ADDR+0x64)       ;; DPTC Comparator Value Reg 0
SYS_DCVR1		EQU		(SYS_BASE_ADDR+0x68)       ;; DPTC Comparator Value Reg 1
SYS_DCVR2		EQU		(SYS_BASE_ADDR+0x6C)       ;; DPTC Comparator Value Reg 2
SYS_DCVR3		EQU		(SYS_BASE_ADDR+0x70)       ;; DPTC Comparator Value Reg 3

;;CLOCK settings for registers
;;
CRM_CSCR_SETTING_MHZ399		EQU	0x33f48107	;;CRM - CSCR SETTING FOR FClk = 399 MHz, Hclk = 133 MHz (assumes MPLL = 399 MHz)
CRM_MPCTL0_SETTING_MHZ399	EQU	0x01EF15D5	;;CRM - MPCTL0 SETTING FOR MPLL = 399 MHz
CRM_MPCTL1_SETTING_MHZ399	EQU	0x00000040	;;CRM - MPCTL1 SETTING FOR BRM Second Order since MFN/MFD > 9/10 for 399MHz
CRM_PCCR0_SETTING_DISABLE	EQU	0x03010300	;;CRM - PCCR0 SETTING to disable peripheral clocks, except GPIO, GPT1, IIM, RTC, and RTIC
CRM_PCCR1_SETTING_DISABLE	EQU	0x81484408	;;CRM - PCCR1 SETTING to disable peripheral clocks, except UART1, WDT, BROM, EMI, RTIC, PERCLK1, AND NFC
CRM_PCDR0_SETTING_DISABLE	EQU	0x100410C3	;;CRM - PCDR0 SETTING to disable CLKO

;;System Drive Level Settings
;;
ALL_NORM_STRENGTH			EQU		0x00000000			;;Set all regitsers to Normal strength level
ALL_HIGH_STRENGTH			EQU 	0x55555555          ;;Set all registers to High strength level
ALL_MAXH_STRENGTH			EQU		0xFFFFFFFF			;;Set all registers to MAX High strength level
SYS_DSCR7_SETTING			EQU		0x000F5005			;;SYS DSCR7  Set Drive for Group 89,90 to MAX High, and Set CS and BANK drive registers to high strength level
SYS_DSCR8_SETTING			EQU 	0x15555555          ;;Set all  data strobe-mask registers to high strength level (except reserved)
SYS_DSCR10_SETTING			EQU		0x0C003FFF			;;SYS DSCR10 Set Drive for Group 149,150,152,153,154,155,156,157,158,159,160 to MAX High
SYS_DSCR11_SETTING			EQU		0xFFFFCF00			;;SYS DSCR11 Set Drive for Group 129,130,131,132,133,134,135, 142 to MAX High

;;Bit Masks
;;
CRM_MPCTL1_MASK_LOCK	EQU	0x00008000	;;CRM - MPCTL1 MASK for determing if the MPLL has locked in the new frequency

;;CLOCK name conversion table
;; mx27 has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
CLOCK_BASE			EQU  		CRM_BASE_ADDR
CSCR				EQU  		CRM_CSCR			;; Clock Source Control Register
CSCR_SETTING			EQU		CRM_CSCR_SETTING_MHZ399		;; also make hclck out on clk0...
MPCTL0				EQU		CRM_MPCTL0
MPCTL0_SETTING			EQU		CRM_MPCTL0_SETTING_MHZ399
PCDR				EQU		CRM_PCDR1			;;UART1 setup
;PCDR_SETTINGS			EQU             0x03030303                      ;;66.5   mhz PERCLK's WITH DIVIDE BY 4, using 133mhz x2 HCLK
;PCDR_SETTINGS			EQU             0x03030305                      ;;44.33  mhz PERCLK1 WITH DIVIDE BY 6, using 133mhz x2 HCLK
PCDR_SETTINGS			EQU             0x03030301                      ;;133.00 mhz PERCLK1 WITH DIVIDE BY 2, using 133mhz x2 HCLK
MHZ150				EQU		MPCTL0_SETTING 	;;temperary for app compression subproject only - remove later - lmc

;; #########################################
;; # External Memory CSD0 CSD1             #
;; # $A000_0000 to $AFFF_FFFF (CSD0)       #
;; # $B000_0000 to $BFFF_FFFF (CSD1)       #
;; #########################################
CSD0_BASE_ADDR		EQU	0xA0000000
CSD0_END_ADDR		EQU	(CSD0_BASE_ADDR+0x0FFFFFFF)
CSD1_BASE_ADDR		EQU	0xB0000000
CSD1_END_ADDR		EQU	(CSD1_BASE_ADDR+0x0FFFFFFF)

;;#DDR sdram configuration for mx27 lite demo board (MT46H32M16LFCK-75 @ 133mhz)
;;#
;;#**These values are used in db_boot.s and are also defined in db_mem.s and used there
CSD0_ESD_ESDMISC 		EQU 0xD8001010	 	;; MISC Register Address for CSD0
CSD0_ESDMISC_VAL3		EQU 0x00000008	 	;; MISC Register, reset delay line with ddr disabled


;; #########################################
;; # External Memory CS0 - CS5             #
;; # $C000_0000 to $C7FF_FFFF (CS0)        #
;; # $C800_0000 to $CFFF_FFFF (CS1)        #
;; # $D000_0000 to $D1FF_FFFF (CS2)        #
;; # $D200_0000 to $D3FF_FFFF (CS3)        #
;; # $D400_0000 to $D5FF_FFFF (CS4)        #
;; # $D600_0000 to $D7FF_FFFF (CS5)        #
;; #########################################
CS0_BASE_ADDR		EQU	    0xC0000000
CS0_END_ADDR		EQU	    (CS0_BASE_ADDR+0x7FFFFFF)



;; #########################################
;; # WEIM - NOR Flash CS0 - 5              #
;; # $D800_2000 to $D800_2FFF              #
;; #########################################
WEIM_BASE_ADDR		EQU		0xD8002000
WEIM_CS0U		EQU		(WEIM_BASE_ADDR+0x00)      ;;CS0 NOR flash upper control register
WEIM_CS0L		EQU		(WEIM_BASE_ADDR+0x04)      ;;CS0 NOR flash lower control register
WEIM_CS0A		EQU		(WEIM_BASE_ADDR+0x08)      ;;CS0 NOR flash addition control register
WEIM_CS1U		EQU		(WEIM_BASE_ADDR+0x10)      ;;
WEIM_CS1L		EQU		(WEIM_BASE_ADDR+0x14)      ;;
WEIM_CS1A		EQU		(WEIM_BASE_ADDR+0x18)      ;;
WEIM_CS2U		EQU		(WEIM_BASE_ADDR+0x20)      ;;
WEIM_CS2L		EQU		(WEIM_BASE_ADDR+0x24)      ;;
WEIM_CS2A		EQU		(WEIM_BASE_ADDR+0x28)      ;;
WEIM_CS3U		EQU		(WEIM_BASE_ADDR+0x30)      ;;
WEIM_CS3L		EQU		(WEIM_BASE_ADDR+0x34)      ;;
WEIM_CS3A		EQU		(WEIM_BASE_ADDR+0x38)      ;;
WEIM_CS4U		EQU		(WEIM_BASE_ADDR+0x40)      ;;
WEIM_CS4L		EQU		(WEIM_BASE_ADDR+0x44)      ;;
WEIM_CS4A		EQU		(WEIM_BASE_ADDR+0x48)      ;;
WEIM_CS5U		EQU		(WEIM_BASE_ADDR+0x50)      ;;
WEIM_CS5L		EQU		(WEIM_BASE_ADDR+0x54)      ;;
WEIM_CS5A		EQU		(WEIM_BASE_ADDR+0x58)      ;;
WEIM_EIM		EQU		(WEIM_BASE_ADDR+0x60)      ;;


;;CS0 NOR Flash name conversion table
;; mx27 has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
CS0U			EQU	WEIM_CS0U		;; CS0 upper control register
CS0L			EQU	WEIM_CS0L		;; CS0 lower control register
WSCEQU1			EQU	0x00000F03		;; wait states = 11, works for BOOT (133Mhz system Hclk, 70 ns Flash)
APPWSCEQU1		EQU	0x00000F03		;; wait states = 11, works for APP (133Mhz system Hclk, 70 ns Flash)


;; #########################################
;; # NFC - NAND FLASH CONTROL              #
;; # $D800_0000 to $D800_0FFF              #
;; #########################################
NFC_BASE_ADDR			EQU	    0xD8000000
NFC_REG_BASE_ADDR		EQU	    (NFC_BASE_ADDR+0xE00)      ;;
NFC_BUFSIZE			EQU	    (NFC_REG_BASE_ADDR+0x00)   ;;
NFC_BLK_ADD_LOCK		EQU	    (NFC_REG_BASE_ADDR+0x02)   ;;
NFC_RAM_BUF_ADDR		EQU	    (NFC_REG_BASE_ADDR+0x04)   ;;
NFC_NAND_FLASH_ADDR		EQU	    (NFC_REG_BASE_ADDR+0x06)   ;;
NFC_NAND_FLASH_CMD		EQU	    (NFC_REG_BASE_ADDR+0x08)   ;;
NFC_CONFIGURATION		EQU	    (NFC_REG_BASE_ADDR+0x0A)   ;;
NFC_ECC_STAT_RES		EQU	    (NFC_REG_BASE_ADDR+0x0C)   ;;
NFC_ECC_RSLT_MA	     		EQU	    (NFC_REG_BASE_ADDR+0x0E)   ;;
NFC_ECC_RSLT_SA	    		EQU	    (NFC_REG_BASE_ADDR+0x10)   ;;
NFC_NF_WR_PROT			EQU	    (NFC_REG_BASE_ADDR+0x12)   ;;
NFC_ULOCK_START_BLK		EQU	    (NFC_REG_BASE_ADDR+0x14)   ;;
NFC_ULOCK_END_BLK		EQU	    (NFC_REG_BASE_ADDR+0x16)   ;;
NFC_NF_WR_PROT_STAT		EQU	    (NFC_REG_BASE_ADDR+0x18)   ;;
NFC_NF_CONFIG1			EQU	    (NFC_REG_BASE_ADDR+0x1A)   ;;
NFC_NF_CONFIG2			EQU	    (NFC_REG_BASE_ADDR+0x1C)   ;;

NFC_MAB0_BASE 			EQU	    (NFC_BASE_ADDR+0x000)	;;  main area buffer0 (3000 - 31FE)
NFC_MAB1_BASE 			EQU	    (NFC_BASE_ADDR+0x200)	;;  main area buffer1 (3200 - 33FE)
NFC_MAB2_BASE 			EQU	    (NFC_BASE_ADDR+0x400)	;;  main area buffer2 (3400 - 35FE)
NFC_MAB3_BASE			EQU	    (NFC_BASE_ADDR+0x600)	;;  main area buffer3 (3600 - 37FE)

NFC_SAB0_BASE 			EQU	    (NFC_BASE_ADDR+0x800)	;;  spare area buffer0 (3800 - 380E)
NFC_SAB1_BASE 			EQU	    (NFC_BASE_ADDR+0x810)	;;  spare area buffer1 (3810 - 381E)
NFC_SAB2_BASE 			EQU	    (NFC_BASE_ADDR+0x820)	;;  spare area buffer2 (3820 - 382E)
NFC_SAB3_BASE 			EQU	    (NFC_BASE_ADDR+0x830)	;;  spare area buffer3 (3830 - 383E)


;; #########################################
;; # UART1                                 #
;; # $1000_A000 to $1000_AFFF              #
;; #########################################
;;
UART1_BASE_ADDR		EQU	    0x1000A000
UART1_URXD_1		EQU	    (UART1_BASE_ADDR+0x00)     ;; 32bit uart1 receiver reg
UART1_UTXD_1		EQU	    (UART1_BASE_ADDR+0x40)     ;; 32bit uart1 transmitter reg
UART1_UCR1_1		EQU	    (UART1_BASE_ADDR+0x80)     ;; 32bit uart1 control 1 reg
UART1_UCR2_1		EQU	    (UART1_BASE_ADDR+0x84)     ;; 32bit uart1 control 2 reg
UART1_UCR3_1		EQU	    (UART1_BASE_ADDR+0x88)     ;; 32bit uart1 control 3 reg
UART1_UCR4_1		EQU	    (UART1_BASE_ADDR+0x8C)     ;; 32bit uart1 control 4 reg
UART1_UFCR_1		EQU	    (UART1_BASE_ADDR+0x90)     ;; 32bit uart1 fifo control reg
UART1_USR1_1		EQU	    (UART1_BASE_ADDR+0x94)     ;; 32bit uart1 status 1 reg
UART1_USR2_1		EQU	    (UART1_BASE_ADDR+0x98)     ;; 32bit uart1 status 2 reg
UART1_UESC_1		EQU	    (UART1_BASE_ADDR+0x9C)     ;; 32bit uart1 escape char reg
UART1_UTIM_1		EQU	    (UART1_BASE_ADDR+0xA0)     ;; 32bit uart1 escape timer reg
UART1_UBIR_1		EQU	    (UART1_BASE_ADDR+0xA4)     ;; 32bit uart1 BRM incremental reg
UART1_UBMR_1		EQU	    (UART1_BASE_ADDR+0xA8)     ;; 32bit uart1 BRM modulator reg
UART1_UBRC_1		EQU	    (UART1_BASE_ADDR+0xAC)     ;; 32bit uart1 baud rate count reg
UART1_ONEMS_1		EQU	    (UART1_BASE_ADDR+0xB0)     ;; 32bit uart1 one ms reg
UART1_UTS_1		EQU	    (UART1_BASE_ADDR+0xB4)     ;; 32bit uart1 test reg

;;Uart1 masks
;;
USR2_1_BIT_14_TRANS_BUF_EMPTY_MASK		EQU		0x4000

;;UART1 name conversion table
;; mx27 has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
UFCR_1				EQU		UART1_UFCR_1
;UFCR_1_SETTINGS		EQU		0x0A81		;; sets refdiv / to 1, which means clock into uart IS perclk1 above
UFCR_1_SETTINGS			EQU		0x0981		;; sets refdiv / to 3, which means clock into uart IS perclk1(133MHz)/3 = 44.33MHz
UCR1_1				EQU		UART1_UCR1_1
UCR1_1_SETTINGS			EQU		0x00000001	;;Enable UART1, disable interupts
UCR2_1				EQU		UART1_UCR2_1
UCR2_1_SETTINGS			EQU		0x00005026	;;set up rts,cts, and message parameters
UCR3_1				EQU		UART1_UCR3_1
UCR3_1_SETTINGS			EQU             0x00000004	;;mux input selected for UART receiver
UCR4_1				EQU		UART1_UCR4_1
UCR4_1_SETTINGS			EQU		0x00008000	;;SET cts trigger level
UBIR_1				EQU		UART1_UBIR_1
;UBIR_1_SETTINGS		EQU             575		;; So 575 NUM and 20780 DENOM works good for imx27 with 66.5 mhz PERCLK1, (with / 4) clock DIV
UBIR_1_SETTINGS			EQU             0x0732		;; So 1842 NUM and 44329 DENOM works good for imx27 with 44.33 mhz PERCLK1, (with / 6) clock DIV
UBMR_1				EQU		UART1_UBMR_1
;UBMR_1_SETTINGS		EQU             20780		;; So 575 NUM and 20780 DENOM works good for imx27 with 66.5 mhz PERCLK1, (with / 4) clock DIV
UBMR_1_SETTINGS			EQU             0xAD29		;; So 1842 NUM and 44329 DENOM works good for imx27 with 44.33 mhz PERCLK1, (with / 6) clock DIV
UTXDn_1				EQU		UART1_UTXD_1
USR2_1				EQU		UART1_USR2_1


;; #########################################
;; # UART2                                 #
;; # $1000_B000 to $1000_BFFF              #
;; #########################################
UART2_BASE_ADDR		EQU	    0x1000B000
UART2_URXD_2		EQU	    (UART2_BASE_ADDR+0x00)     ;; 32bit uart2 receiver reg
UART2_UTXD_2		EQU	    (UART2_BASE_ADDR+0x40)     ;; 32bit uart2 transmitter reg
UART2_UCR1_2		EQU	    (UART2_BASE_ADDR+0x80)     ;; 32bit uart2 control 1 reg
UART2_UCR2_2		EQU	    (UART2_BASE_ADDR+0x84)     ;; 32bit uart2 control 2 reg
UART2_UCR3_2		EQU	    (UART2_BASE_ADDR+0x88)     ;; 32bit uart2 control 3 reg
UART2_UCR4_2		EQU	    (UART2_BASE_ADDR+0x8C)     ;; 32bit uart2 control 4 reg
UART2_UFCR_2		EQU	    (UART2_BASE_ADDR+0x90)     ;; 32bit uart2 fifo control reg
UART2_USR1_2		EQU	    (UART2_BASE_ADDR+0x94)     ;; 32bit uart2 status 1 reg
UART2_USR2_2		EQU	    (UART2_BASE_ADDR+0x98)     ;; 32bit uart2 status 2 reg
UART2_UESC_2		EQU	    (UART2_BASE_ADDR+0x9C)     ;; 32bit uart2 escape char reg
UART2_UTIM_2		EQU	    (UART2_BASE_ADDR+0xA0)     ;; 32bit uart2 escape timer reg
UART2_UBIR_2		EQU	    (UART2_BASE_ADDR+0xA4)     ;; 32bit uart2 BRM incremental reg
UART2_UBMR_2		EQU	    (UART2_BASE_ADDR+0xA8)     ;; 32bit uart2 BRM modulator reg
UART2_UBRC_2		EQU	    (UART2_BASE_ADDR+0xAC)     ;; 32bit uart2 baud rate count reg
UART2_ONEMS_2		EQU	    (UART2_BASE_ADDR+0xB0)     ;; 32bit uart2 one ms reg
UART2_UTS_2		EQU	    (UART2_BASE_ADDR+0xB4)     ;; 32bit uart2 test reg


;; #########################################
;; # WDOG                                  #
;; # $1000_2000 to $1000_2FFF              #
;; #########################################
WDOG_BASE_ADDR		EQU	    0x10002000
WDOG_WCR		EQU	    (WDOG_BASE_ADDR+0x00)      ;; 16bit watchdog control reg
WDOG_WSR		EQU	    (WDOG_BASE_ADDR+0x02)      ;; 16bit watchdog service reg
WDOG_WRSR		EQU	    (WDOG_BASE_ADDR+0x04)      ;; 16bit watchdog reset status reg

;;Watchdog name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
WATCHDOG_BASE_ADDR_UPPER		EQU	0x10000000
WATCHDOG_BASE_ADDR_LOWER		EQU	0x00002000
WATCHDOG_BASE_ADDR_WCR_OFFSET		EQU	0x00000000
WATCHDOG_BASE_ADDR_WSR_OFFSET		EQU	0x00000002
WATCHDOG_BASE_ADDR_WRSR_OFFSET		EQU	0x00000004


;;#########################################
;; # GPT1                                  #
;; # $1000_3000 to $1000_3FFF              #
;; #########################################
GPT1_BASE_ADDR		EQU	    0x10003000
GPT1_TCTL1		EQU	    (GPT1_BASE_ADDR+0x00)      ;; 32bit timer 1 control reg
GPT1_TPRER1		EQU	    (GPT1_BASE_ADDR+0x04)      ;; 32bit timer 1 prescaler reg
GPT1_TCMP1		EQU	    (GPT1_BASE_ADDR+0x08)      ;; 32bit timer 1 compare reg
GPT1_TCR1		EQU	    (GPT1_BASE_ADDR+0x0C)      ;; 32bit timer 1 capture reg
GPT1_TCN1		EQU	    (GPT1_BASE_ADDR+0x10)      ;; 32bit timer 1 counter reg
GPT1_TSTAT1		EQU	    (GPT1_BASE_ADDR+0x14)      ;; 32bit timer 1 status reg

;;GPT1 name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
GPT1_BASE_ADDR_UPPER			EQU	0x10000000
GPT1_BASE_ADDR_LOWER			EQU	0x00003000
GPT1_BASE_ADDR_TCN1_OFFSET		EQU	0x00000010


;; #########################################
;; # GPT2                                  #
;; # $1000_4000 to $1000_4FFF              #
;; #########################################
GPT2_BASE_ADDR		EQU	    0x10004000
GPT2_TCTL2		EQU	    (GPT2_BASE_ADDR+0x00)      ;; 32bit timer 2 control reg
GPT2_TPRER2		EQU	    (GPT2_BASE_ADDR+0x04)      ;; 32bit timer 2 prescaler reg
GPT2_TCMP2		EQU	    (GPT2_BASE_ADDR+0x08)      ;; 32bit timer 2 compare reg
GPT2_TCR2		EQU	    (GPT2_BASE_ADDR+0x0C)      ;; 32bit timer 2 capture reg
GPT2_TCN2		EQU	    (GPT2_BASE_ADDR+0x10)      ;; 32bit timer 2 counter reg
GPT2_TSTAT2		EQU	    (GPT2_BASE_ADDR+0x14)      ;; 32bit timer 2 status reg

;; #########################################
;; # GPT3                                  #
;; # $1000_5000 to $1000_5FFF              #
;; #########################################
GPT3_BASE_ADDR		EQU	    0x10005000
GPT3_TCTL3		EQU	    (GPT3_BASE_ADDR+0x00)      ;; 32bit timer 3 control reg
GPT3_TPRER3		EQU	    (GPT3_BASE_ADDR+0x04)      ;; 32bit timer 3 prescaler reg
GPT3_TCMP3		EQU	    (GPT3_BASE_ADDR+0x08)      ;; 32bit timer 3 compare reg
GPT3_TCR3		EQU	    (GPT3_BASE_ADDR+0x0C)      ;; 32bit timer 3 capture reg
GPT3_TCN3		EQU	    (GPT3_BASE_ADDR+0x10)      ;; 32bit timer 3 counter reg
GPT3_TSTAT3		EQU	    (GPT3_BASE_ADDR+0x14)      ;; 32bit timer 3 status reg

;; #########################################
;; # GPT4                                  #
;; # $1001_9000 to $1001_9FFF              #
;; #########################################
GPT4_BASE_ADDR		EQU	    0x10019000
GPT4_TCTL4		EQU	    (GPT4_BASE_ADDR+0x00)      ;; 32bit timer 4 control reg
GPT4_TPRER4		EQU	    (GPT4_BASE_ADDR+0x04)      ;; 32bit timer 4 prescaler reg
GPT4_TCMP4		EQU	    (GPT4_BASE_ADDR+0x08)      ;; 32bit timer 4 compare reg
GPT4_TCR4		EQU	    (GPT4_BASE_ADDR+0x0C)      ;; 32bit timer 4 capture reg
GPT4_TCN4		EQU	    (GPT4_BASE_ADDR+0x10)      ;; 32bit timer 4 counter reg
GPT4_TSTAT4		EQU	    (GPT4_BASE_ADDR+0x14)      ;; 32bit timer 4 status reg

;; #########################################
;; # GPT5                                  #
;; # $1001_A000 to $1001_AFFF              #
;; #########################################
GPT5_BASE_ADDR		EQU	    0x1001A000
GPT5_TCTL5		EQU	    (GPT5_BASE_ADDR+0x00)      ;; 32bit timer 5 control reg
GPT5_TPRER5		EQU	    (GPT5_BASE_ADDR+0x04)      ;; 32bit timer 5 prescaler reg
GPT5_TCMP5		EQU	    (GPT5_BASE_ADDR+0x08)      ;; 32bit timer 5 compare reg
GPT5_TCR5		EQU	    (GPT5_BASE_ADDR+0x0C)      ;; 32bit timer 5 capture reg
GPT5_TCN5		EQU	    (GPT5_BASE_ADDR+0x10)      ;; 32bit timer 5 counter reg
GPT5_TSTAT5		EQU	    (GPT5_BASE_ADDR+0x14)      ;; 32bit timer 5 status reg

;; #########################################
;; # GPT6                                  #
;; # $1001_F000 to $1001_FFFF              #
;; #########################################
GPT6_BASE_ADDR		EQU	    0x1001F000
GPT6_TCTL6		EQU	    (GPT6_BASE_ADDR+0x00)      ;; 32bit timer 6 control reg
GPT6_TPRER6		EQU	    (GPT6_BASE_ADDR+0x04)      ;; 32bit timer 6 prescaler reg
GPT6_TCMP6		EQU	    (GPT6_BASE_ADDR+0x08)      ;; 32bit timer 6 compare reg
GPT6_TCR6		EQU	    (GPT6_BASE_ADDR+0x0C)      ;; 32bit timer 6 capture reg
GPT6_TCN6		EQU	    (GPT6_BASE_ADDR+0x10)      ;; 32bit timer 6 counter reg
GPT6_TSTAT6		EQU	    (GPT6_BASE_ADDR+0x14)      ;; 32bit timer 6 status reg


;; #########################################
;; # RTC  -  Real Time Clock               #
;; # $1000_7000 to $1000_7FFF              #
;; #########################################
RTC_BASE_ADDR		EQU		0x10007000
RTC_HOURMIN		EQU	       (RTC_BASE_ADDR+0x00)       ;; 32bit rtc hour/min counter reg
RTC_SECOND		EQU	       (RTC_BASE_ADDR+0x04)       ;; 32bit rtc seconds counter reg
RTC_ALRM_HM		EQU	       (RTC_BASE_ADDR+0x08)       ;; 32bit rtc alarm hour/min reg
RTC_ALRM_SEC		EQU	       (RTC_BASE_ADDR+0x0C)       ;; 32bit rtc alarm seconds reg
RTC_RTCCTL		EQU	       (RTC_BASE_ADDR+0x10)       ;; 32bit rtc control reg
RTC_RTCISR		EQU	       (RTC_BASE_ADDR+0x14)       ;; 32bit rtc interrupt status reg
RTC_RTCIENR		EQU	       (RTC_BASE_ADDR+0x18)       ;; 32bit rtc interrupt enable reg
RTC_STPWCH		EQU	       (RTC_BASE_ADDR+0x1C)       ;; 32bit rtc stopwatch min reg
RTC_DAYR		EQU	       (RTC_BASE_ADDR+0x20)       ;; 32bit rtc days counter reg
RTC_DAYALARM		EQU	       (RTC_BASE_ADDR+0x24)       ;; 32bit rtc day alarm reg
RTC_TEST1		EQU	       (RTC_BASE_ADDR+0x28)       ;; 32bit rtc test reg 1
RTC_TEST2		EQU	       (RTC_BASE_ADDR+0x2C)       ;; 32bit rtc test reg 2
RTC_TEST3		EQU	       (RTC_BASE_ADDR+0x30)       ;; 32bit rtc test reg 3

;;RTC name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
RTC_BASE_ADDR_UPPER			EQU	0x10000000
RTC_BASE_ADDR_LOWER			EQU	0x00007000
RTC_BASE_ADDR_RTC_SECOND_OFFSET		EQU	0x00000004


;; #########################################
;; # GPIO                                  #
;; # $1001_5000 to $1001_5FFF              #
;; #########################################
GPIOA_BASE_ADDR		EQU	    0x10015000
GPIOA_DDIR		EQU	    (GPIOA_BASE_ADDR+0x00)     ;; 32bit gpio pta data direction reg
GPIOA_OCR1		EQU	    (GPIOA_BASE_ADDR+0x04)     ;; 32bit gpio pta output config 1 reg
GPIOA_OCR2		EQU	    (GPIOA_BASE_ADDR+0x08)     ;; 32bit gpio pta output config 2 reg
GPIOA_ICONFA1		EQU	    (GPIOA_BASE_ADDR+0x0C)     ;; 32bit gpio pta input config A1 reg
GPIOA_ICONFA2		EQU	    (GPIOA_BASE_ADDR+0x10)     ;; 32bit gpio pta input config A2 reg
GPIOA_ICONFB1		EQU	    (GPIOA_BASE_ADDR+0x14)     ;; 32bit gpio pta input config B1 reg
GPIOA_ICONFB2		EQU	    (GPIOA_BASE_ADDR+0x18)     ;; 32bit gpio pta input config B2 reg
GPIOA_DR		EQU	    (GPIOA_BASE_ADDR+0x1C)     ;; 32bit gpio pta data reg
GPIOA_GIUS		EQU	    (GPIOA_BASE_ADDR+0x20)     ;; 32bit gpio pta in use reg
GPIOA_SSR		EQU	    (GPIOA_BASE_ADDR+0x24)     ;; 32bit gpio pta sample status reg
GPIOA_ICR1		EQU	    (GPIOA_BASE_ADDR+0x28)     ;; 32bit gpio pta interrupt ctrl 1 reg
GPIOA_ICR2		EQU	    (GPIOA_BASE_ADDR+0x2C)     ;; 32bit gpio pta interrupt ctrl 2 reg
GPIOA_IMR		EQU	    (GPIOA_BASE_ADDR+0x30)     ;; 32bit gpio pta interrupt mask reg
GPIOA_ISR		EQU	    (GPIOA_BASE_ADDR+0x34)     ;; 32bit gpio pta interrupt status reg
GPIOA_GPR		EQU	    (GPIOA_BASE_ADDR+0x38)     ;; 32bit gpio pta general purpose reg
GPIOA_SWR		EQU	    (GPIOA_BASE_ADDR+0x3C)     ;; 32bit gpio pta software reset reg
GPIOA_PUEN		EQU	    (GPIOA_BASE_ADDR+0x40)     ;; 32bit gpio pta pull up enable reg

GPIOB_BASE_ADDR		EQU	    0x10015100
GPIOB_DDIR		EQU	    (GPIOB_BASE_ADDR+0x00)     ;; 32bit gpio ptb data direction reg
GPIOB_OCR1		EQU	    (GPIOB_BASE_ADDR+0x04)     ;; 32bit gpio ptb output config 1 reg
GPIOB_OCR2		EQU	    (GPIOB_BASE_ADDR+0x08)     ;; 32bit gpio ptb output config 2 reg
GPIOB_ICONFA1		EQU	    (GPIOB_BASE_ADDR+0x0C)     ;; 32bit gpio ptb input config A1 reg
GPIOB_ICONFA2		EQU	    (GPIOB_BASE_ADDR+0x10)     ;; 32bit gpio ptb input config A2 reg
GPIOB_ICONFB1		EQU	    (GPIOB_BASE_ADDR+0x14)     ;; 32bit gpio ptb input config B1 reg
GPIOB_ICONFB2		EQU	    (GPIOB_BASE_ADDR+0x18)     ;; 32bit gpio ptb input config B2 reg
GPIOB_DR		EQU	    (GPIOB_BASE_ADDR+0x1C)     ;; 32bit gpio ptb data reg
GPIOB_GIUS		EQU	    (GPIOB_BASE_ADDR+0x20)     ;; 32bit gpio ptb in use reg
GPIOB_SSR		EQU	    (GPIOB_BASE_ADDR+0x24)     ;; 32bit gpio ptb sample status reg
GPIOB_ICR1		EQU	    (GPIOB_BASE_ADDR+0x28)     ;; 32bit gpio ptb interrupt ctrl 1 reg
GPIOB_ICR2		EQU	    (GPIOB_BASE_ADDR+0x2C)     ;; 32bit gpio ptb interrupt ctrl 2 reg
GPIOB_IMR		EQU	    (GPIOB_BASE_ADDR+0x30)     ;; 32bit gpio ptb interrupt mask reg
GPIOB_ISR		EQU	    (GPIOB_BASE_ADDR+0x34)     ;; 32bit gpio ptb interrupt status reg
GPIOB_GPR		EQU	    (GPIOB_BASE_ADDR+0x38)     ;; 32bit gpio ptb general purpose reg
GPIOB_SWR		EQU	    (GPIOB_BASE_ADDR+0x3C)     ;; 32bit gpio ptb software reset reg
GPIOB_PUEN		EQU	    (GPIOB_BASE_ADDR+0x40)     ;; 32bit gpio ptb pull up enable reg

GPIOC_BASE_ADDR		EQU	    0x10015200
GPIOC_DDIR		EQU	    (GPIOC_BASE_ADDR+0x00)     ;; 32bit gpio ptc data direction reg
GPIOC_OCR1		EQU	    (GPIOC_BASE_ADDR+0x04)     ;; 32bit gpio ptc output config 1 reg
GPIOC_OCR2		EQU	    (GPIOC_BASE_ADDR+0x08)     ;; 32bit gpio ptc output config 2 reg
GPIOC_ICONFA1		EQU	    (GPIOC_BASE_ADDR+0x0C)     ;; 32bit gpio ptc input config A1 reg
GPIOC_ICONFA2		EQU	    (GPIOC_BASE_ADDR+0x10)     ;; 32bit gpio ptc input config A2 reg
GPIOC_ICONFB1		EQU	    (GPIOC_BASE_ADDR+0x14)     ;; 32bit gpio ptc input config B1 reg
GPIOC_ICONFB2		EQU	    (GPIOC_BASE_ADDR+0x18)     ;; 32bit gpio ptc input config B2 reg
GPIOC_DR		EQU	    (GPIOC_BASE_ADDR+0x1C)     ;; 32bit gpio ptc data reg
GPIOC_GIUS		EQU	    (GPIOC_BASE_ADDR+0x20)     ;; 32bit gpio ptc in use reg
GPIOC_SSR		EQU	    (GPIOC_BASE_ADDR+0x24)     ;; 32bit gpio ptc sample status reg
GPIOC_ICR1		EQU	    (GPIOC_BASE_ADDR+0x28)     ;; 32bit gpio ptc interrupt ctrl 1 reg
GPIOC_ICR2		EQU	    (GPIOC_BASE_ADDR+0x2C)     ;; 32bit gpio ptc interrupt ctrl 2 reg
GPIOC_IMR		EQU	    (GPIOC_BASE_ADDR+0x30)     ;; 32bit gpio ptc interrupt mask reg
GPIOC_ISR		EQU	    (GPIOC_BASE_ADDR+0x34)     ;; 32bit gpio ptc interrupt status reg
GPIOC_GPR		EQU	    (GPIOC_BASE_ADDR+0x38)     ;; 32bit gpio ptc general purpose reg
GPIOC_SWR		EQU	    (GPIOC_BASE_ADDR+0x3C)     ;; 32bit gpio ptc software reset reg
GPIOC_PUEN		EQU	    (GPIOC_BASE_ADDR+0x40)     ;; 32bit gpio ptc pull up enable reg

GPIOD_BASE_ADDR		EQU	    0x10015300
GPIOD_DDIR		EQU	    (GPIOD_BASE_ADDR+0x00)     ;; 32bit gpio ptd data direction reg
GPIOD_OCR1		EQU	    (GPIOD_BASE_ADDR+0x04)     ;; 32bit gpio ptd output config 1 reg
GPIOD_OCR2		EQU	    (GPIOD_BASE_ADDR+0x08)     ;; 32bit gpio ptd output config 2 reg
GPIOD_ICONFA1		EQU	    (GPIOD_BASE_ADDR+0x0C)     ;; 32bit gpio ptd input config A1 reg
GPIOD_ICONFA2		EQU	    (GPIOD_BASE_ADDR+0x10)     ;; 32bit gpio ptd input config A2 reg
GPIOD_ICONFB1		EQU	    (GPIOD_BASE_ADDR+0x14)     ;; 32bit gpio ptd input config B1 reg
GPIOD_ICONFB2		EQU	    (GPIOD_BASE_ADDR+0x18)     ;; 32bit gpio ptd input config B2 reg
GPIOD_DR		EQU	    (GPIOD_BASE_ADDR+0x1C)     ;; 32bit gpio ptd data reg
GPIOD_GIUS		EQU	    (GPIOD_BASE_ADDR+0x20)     ;; 32bit gpio ptd in use reg
GPIOD_SSR		EQU	    (GPIOD_BASE_ADDR+0x24)     ;; 32bit gpio ptd sample status reg
GPIOD_ICR1		EQU	    (GPIOD_BASE_ADDR+0x28)     ;; 32bit gpio ptd interrupt ctrl 1 reg
GPIOD_ICR2		EQU	    (GPIOD_BASE_ADDR+0x2C)     ;; 32bit gpio ptd interrupt ctrl 2 reg
GPIOD_IMR		EQU	    (GPIOD_BASE_ADDR+0x30)     ;; 32bit gpio ptd interrupt mask reg
GPIOD_ISR		EQU	    (GPIOD_BASE_ADDR+0x34)     ;; 32bit gpio ptd interrupt status reg
GPIOD_GPR		EQU	    (GPIOD_BASE_ADDR+0x38)     ;; 32bit gpio ptd general purpose reg
GPIOD_SWR		EQU	    (GPIOD_BASE_ADDR+0x3C)     ;; 32bit gpio ptd software reset reg
GPIOD_PUEN		EQU	    (GPIOD_BASE_ADDR+0x40)     ;; 32bit gpio ptd pull up enable reg

GPIOE_BASE_ADDR		EQU	    0x10015400
GPIOE_DDIR		EQU	    (GPIOE_BASE_ADDR+0x00)     ;; 32bit gpio pte data direction reg
GPIOE_OCR1		EQU	    (GPIOE_BASE_ADDR+0x04)     ;; 32bit gpio pte output config 1 reg
GPIOE_OCR2		EQU	    (GPIOE_BASE_ADDR+0x08)     ;; 32bit gpio pte output config 2 reg
GPIOE_ICONFA1		EQU	    (GPIOE_BASE_ADDR+0x0C)     ;; 32bit gpio pte input config A1 reg
GPIOE_ICONFA2		EQU	    (GPIOE_BASE_ADDR+0x10)     ;; 32bit gpio pte input config A2 reg
GPIOE_ICONFB1		EQU	    (GPIOE_BASE_ADDR+0x14)     ;; 32bit gpio pte input config B1 reg
GPIOE_ICONFB2		EQU	    (GPIOE_BASE_ADDR+0x18)     ;; 32bit gpio pte input config B2 reg
GPIOE_DR		EQU	    (GPIOE_BASE_ADDR+0x1C)     ;; 32bit gpio pte data reg
GPIOE_GIUS		EQU	    (GPIOE_BASE_ADDR+0x20)     ;; 32bit gpio pte in use reg
GPIOE_SSR		EQU	    (GPIOE_BASE_ADDR+0x24)     ;; 32bit gpio pte sample status reg
GPIOE_ICR1		EQU	    (GPIOE_BASE_ADDR+0x28)     ;; 32bit gpio pte interrupt ctrl 1 reg
GPIOE_ICR2		EQU	    (GPIOE_BASE_ADDR+0x2C)     ;; 32bit gpio pte interrupt ctrl 2 reg
GPIOE_IMR		EQU	    (GPIOE_BASE_ADDR+0x30)     ;; 32bit gpio pte interrupt mask reg
GPIOE_ISR		EQU	    (GPIOE_BASE_ADDR+0x34)     ;; 32bit gpio pte interrupt status reg
GPIOE_GPR		EQU	    (GPIOE_BASE_ADDR+0x38)     ;; 32bit gpio pte general purpose reg
GPIOE_SWR		EQU	    (GPIOE_BASE_ADDR+0x3C)     ;; 32bit gpio pte software reset reg
GPIOE_PUEN		EQU	    (GPIOE_BASE_ADDR+0x40)     ;; 32bit gpio pte pull up enable reg

GPIOF_BASE_ADDR		EQU	    0x10015500
GPIOF_DDIR		EQU	    (GPIOF_BASE_ADDR+0x00)     ;; 32bit gpio ptf data direction reg
GPIOF_OCR1		EQU	    (GPIOF_BASE_ADDR+0x04)     ;; 32bit gpio ptf output config 1 reg
GPIOF_OCR2		EQU	    (GPIOF_BASE_ADDR+0x08)     ;; 32bit gpio ptf output config 2 reg
GPIOF_ICONFA1		EQU	    (GPIOF_BASE_ADDR+0x0C)     ;; 32bit gpio ptf input config A1 reg
GPIOF_ICONFA2		EQU	    (GPIOF_BASE_ADDR+0x10)     ;; 32bit gpio ptf input config A2 reg
GPIOF_ICONFB1		EQU	    (GPIOF_BASE_ADDR+0x14)     ;; 32bit gpio ptf input config B1 reg
GPIOF_ICONFB2		EQU	    (GPIOF_BASE_ADDR+0x18)     ;; 32bit gpio ptf input config B2 reg
GPIOF_DR		EQU	    (GPIOF_BASE_ADDR+0x1C)     ;; 32bit gpio ptf data reg
GPIOF_GIUS		EQU	    (GPIOF_BASE_ADDR+0x20)     ;; 32bit gpio ptf in use reg
GPIOF_SSR		EQU	    (GPIOF_BASE_ADDR+0x24)     ;; 32bit gpio ptf sample status reg
GPIOF_ICR1		EQU	    (GPIOF_BASE_ADDR+0x28)     ;; 32bit gpio ptf interrupt ctrl 1 reg
GPIOF_ICR2		EQU	    (GPIOF_BASE_ADDR+0x2C)     ;; 32bit gpio ptf interrupt ctrl 2 reg
GPIOF_IMR		EQU	    (GPIOF_BASE_ADDR+0x30)     ;; 32bit gpio ptf interrupt mask reg
GPIOF_ISR		EQU	    (GPIOF_BASE_ADDR+0x34)     ;; 32bit gpio ptf interrupt status reg
GPIOF_GPR		EQU	    (GPIOF_BASE_ADDR+0x38)     ;; 32bit gpio ptf general purpose reg
GPIOF_SWR		EQU	    (GPIOF_BASE_ADDR+0x3C)     ;; 32bit gpio ptf software reset reg
GPIOF_PUEN		EQU	    (GPIOF_BASE_ADDR+0x40)     ;; 32bit gpio ptf pull up enable reg

GPIO_REG_BASE		EQU	    0x10015600
GPIO_PMASK		EQU	    (GPIO_REG_BASE+0x00)       ;; 32bit gpio interrupt mask reg


;;
;; *********************************************************************************
;;Define the GPIO Initialization Settings for Ports A through F on the mx27 platform
;; *********************************************************************************
;;
	IF Board_TARGET = "iMX27_Development" 	;;Use GPIO settings for 32 bit wide bus Eval board (two mem chips)
;;with DDR sdram configuration for mx27 lite demo board (MT46H32M16LFCK-75 @ 133mhz)
;;

;;
;;Future home for our Eval Board GPIO hardware setup
;;

;;
;;******************************************************************************************************
	ENDIF 		;;END GPIO Initial setting configuration section Board_TARGET = "iMX27_Development" 
;;******************************************************************************************************
;;


	IF Board_TARGET = "iMX27_FF1"		;;Use GPIO settings for 16 bit wide bus FF1 board (one mem chip)
;;with DDR sdram configuration for mx27 engineering FF1 proto board (MT46H64M16LFCK-75 @ 133mhz)

;;These Settings were generated with an EXCEL Doc "ds017_iMX27_FF1_GPIO_Setup.xls"
PORT_A_DR_SETTING		EQU		0x00210000
PORT_B_DR_SETTING		EQU		0x40000000
PORT_C_DR_SETTING		EQU		0x00000000
PORT_D_DR_SETTING		EQU		0x10E00000
PORT_E_DR_SETTING		EQU		0x00000040
PORT_F_DR_SETTING		EQU		0x00000000

PORT_A_GPR_SETTING		EQU		0x00000000
PORT_B_GPR_SETTING		EQU		0x00000000
PORT_C_GPR_SETTING		EQU		0x00000000
PORT_D_GPR_SETTING		EQU		0x00000000
PORT_E_GPR_SETTING		EQU		0x00000000
PORT_F_GPR_SETTING		EQU		0x00000000

PORT_A_GIUS_SETTING		EQU		0xFFFFFFFF
PORT_B_GIUS_SETTING		EQU		0xFF0043F3
PORT_C_GIUS_SETTING		EQU		0xFFFFFFFF
PORT_D_GIUS_SETTING		EQU		0xFFF90000
PORT_E_GIUS_SETTING  	EQU 	0xFFFC0047
PORT_F_GIUS_SETTING		EQU		0xFF180000

PORT_A_OCR1_SETTING		EQU		0x00000003
PORT_B_OCR1_SETTING		EQU		0x000FFC00
PORT_C_OCR1_SETTING		EQU		0x0FFFC000
PORT_D_OCR1_SETTING		EQU		0x00000000
PORT_E_OCR1_SETTING  	EQU  	0x0000303F
PORT_F_OCR1_SETTING		EQU		0x00000000

PORT_A_OCR2_SETTING		EQU		0x00003FFF
PORT_B_OCR2_SETTING		EQU		0x300C0000
PORT_C_OCR2_SETTING		EQU		0x00000000
PORT_D_OCR2_SETTING		EQU		0xFF00FC00
PORT_E_OCR2_SETTING		EQU		0x000F0CC0
PORT_F_OCR2_SETTING		EQU		0x000003C0

PORT_A_PUEN_SETTING		EQU		0xFFFFFFFF
PORT_B_PUEN_SETTING		EQU		0xFFFFFFFF
PORT_C_PUEN_SETTING		EQU		0xFFFFFFFF
PORT_D_PUEN_SETTING		EQU		0xFFFFFFFF
PORT_E_PUEN_SETTING		EQU		0xFFFFFFFF
PORT_F_PUEN_SETTING		EQU		0xFFFFFFFF

PORT_A_DDIR_SETTING		EQU		0x00350001
PORT_B_DDIR_SETTING		EQU		0x420083C0
PORT_C_DDIR_SETTING		EQU		0x00000000
PORT_D_DDIR_SETTING		EQU		0x10E64000
PORT_E_DDIR_SETTING	  	EQU  	0x00080060
PORT_F_DDIR_SETTING		EQU		0x00080000

;;
;;******************************************************************************************************
	ENDIF 		;;END GPIO Initial setting configuration section Board_TARGET = "iMX27_FF1" 
;;******************************************************************************************************
;;



;; #########################################
;; # AITC - INTERUPT CONTROLLER            #
;; # $1004_0000 to $1004_0FFF              #
;; #########################################
AITC_BASE_ADDR			EQU	    0x10040000
AITC_INTCNTL			EQU	    (AITC_BASE_ADDR+0x00)      ;; 32bit aitc int control reg
AITC_NIMASK				EQU	    (AITC_BASE_ADDR+0x04)      ;; 32bit aitc int mask reg
AITC_INTENNUM			EQU	    (AITC_BASE_ADDR+0x08)      ;; 32bit aitc int enable number reg
AITC_INTDISNUM			EQU	    (AITC_BASE_ADDR+0x0C)      ;; 32bit aitc int disable number reg
AITC_INTENABLEH			EQU	    (AITC_BASE_ADDR+0x10)      ;; 32bit aitc int enable reg high
AITC_INTENABLEL			EQU	    (AITC_BASE_ADDR+0x14)      ;; 32bit aitc int enable reg low
AITC_INTTYPEH			EQU	    (AITC_BASE_ADDR+0x18)      ;; 32bit aitc int type reg high
AITC_INTTYPEL	   		EQU	    (AITC_BASE_ADDR+0x1C)      ;; 32bit aitc int type reg low
AITC_NIPRIORITY7		EQU	    (AITC_BASE_ADDR+0x20)      ;; 32bit aitc norm int priority lvl7
AITC_NIPRIORITY6		EQU	    (AITC_BASE_ADDR+0x24)      ;; 32bit aitc norm int priority lvl6
AITC_NIPRIORITY5		EQU	    (AITC_BASE_ADDR+0x28)      ;; 32bit aitc norm int priority lvl5
AITC_NIPRIORITY4		EQU	    (AITC_BASE_ADDR+0x2C)      ;; 32bit aitc norm int priority lvl4
AITC_NIPRIORITY3		EQU	    (AITC_BASE_ADDR+0x30)      ;; 32bit aitc norm int priority lvl3
AITC_NIPRIORITY2		EQU	    (AITC_BASE_ADDR+0x34)      ;; 32bit aitc norm int priority lvl2
AITC_NIPRIORITY1		EQU	    (AITC_BASE_ADDR+0x38)      ;; 32bit aitc norm int priority lvl1
AITC_NIPRIORITY0		EQU	    (AITC_BASE_ADDR+0x3C)      ;; 32bit aitc norm int priority lvl0
AITC_NIVECSR			EQU	    (AITC_BASE_ADDR+0x40)      ;; 32bit aitc norm int vector/status
AITC_FIVECSR	   		EQU	    (AITC_BASE_ADDR+0x44)      ;; 32bit aitc fast int vector/status
AITC_INTSRCH	   		EQU	    (AITC_BASE_ADDR+0x48)      ;; 32bit aitc int source reg high
AITC_INTSRCL	   		EQU	    (AITC_BASE_ADDR+0x4C)      ;; 32bit aitc int source reg low
AITC_INTFRCH	   		EQU	    (AITC_BASE_ADDR+0x50)      ;; 32bit aitc int force reg high
AITC_INTFRCL	   		EQU	    (AITC_BASE_ADDR+0x54)      ;; 32bit aitc int force reg low
AITC_NIPNDH	   		EQU	    (AITC_BASE_ADDR+0x58)      ;; 32bit aitc norm int pending high
AITC_NIPNDL	   		EQU	    (AITC_BASE_ADDR+0x5C)      ;; 32bit aitc norm int pending low
AITC_FIPNDH	   		EQU	    (AITC_BASE_ADDR+0x60)      ;; 32bit aitc fast int pending high
AITC_FIPNDL	   		EQU	    (AITC_BASE_ADDR+0x64)      ;; 32bit aitc fast int pending low

;;AITC name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
NIVESCR				EQU		AITC_NIVECSR		;;NIVECSR - Normal Interrupt Vector and Status Register
FIVESCR				EQU		AITC_FIVECSR		;;FIVECSR - Fast Interrupt Vector and Status Register
;; to avoid literal pool entering the game -- not even sure what the time/other implications are, but...
BASE_INT_REG			EQU  0x10000000				;;Upper half of register base address
AITC_OFFSET			EQU  0x00040000				;;Lower half of register base address
NIVESCR_REG_OFFSET		EQU  0x40                               ;;offset for NIVECSR
FIVESCR_REG_OFFSET		EQU  0x44				;;offset for FIVECSR
	
	END
; * ==========================================================================================================================
; * $Log: db_shared/Hardware_Header_Files/iMX27/iMX27_hdw.i  $
; * Revision 1.1 2008/09/29 14:28:36EDT VanVolkD 
; * Initial revision
; * Member added to project d:/MKS_IT/Gen6FoundationApp/Gen6FoundationApp/Matrix_OS.pj
; * Initial revision
; * ======================================================================================================================= */
