;***************************************************************************************/
;*
;* HSM 48X0G GENERATION 6 SOFTWARE
;*
;*  HARDWARE MACROS 
;*  Date:       9/29/2008
;*  Tools:      RVCT Build 674
;*  Target:     Freescale MXL
;*
;*======================================================================================
;* Revision information
;*======================================================================================
;* $RCSfile: db_shared/Hardware_Header_Files/iMXL/MXL_hdw.i $
;* $Revision: 1.1 $
;* $Date: 2008/09/29 14:29:12EDT $
;*======================================================================================
;*------------------------------------------------------------------------------
;* Header:  MXL_hdw.i
;*
;* This File defines the iMX25 ASM Hardware
;*
;*------------------------------------------------------------------------------ 

;; PLL and Clock Controller Module
CLOCK_BASE     EQU  0x0021b000
CSCR           EQU  CLOCK_BASE + 0      ;; Clock Source Control Register
MPCTL0         EQU  CLOCK_BASE + 0x4    ;; MCU PLL Control Register 0
MPCTL1         EQU  CLOCK_BASE + 0x8    ;; MCU PLL and Sysetm Clock Control Register
UPCTL0         EQU  CLOCK_BASE + 0xc    ;; USB PLL Control Register 0
UPCTL1         EQU  CLOCK_BASE + 0x10   ;; USB PLL Control Register 1
PCDR           EQU  CLOCK_BASE + 0x20   ;; Peripheral Clock Divide Register
GCCR	       EQU  CLOCK_BASE + 0x810	;; Global Clock Controller.

;;CLOCK settings for registers
;;
MHZ150			EQU  0x07e723ad          ;; should be 150mhz, Mot said so!!
MHZ192			EQU  0x013914E2
DEFAULT_MCU_PLL		EQU  0x003f1437
DEFAULT_USB_PLL		EQU  0x043f1437

;; CLOCK  register stuff for UART1 and 2
;; perclk1
PERCLK1MASK    EQU  0x0000000f
NPERCLK1MASK   EQU  0xfffffff0
PERCLK1_DIV6   EQU  (NPERCLK1MASK :OR: 5)    ;; 5 means 6, which should mean 16MHz as input to Uart1 and Uart2
PERCLK1_DIV4   EQU  (NPERCLK1MASK :OR: 3)    ;; 3 means 4, which should mean 24MHz as input to Uart1 and Uart2

;; change bclk
CSCRCLEAR	EQU  0x8f600403	;; BOOT still seems to fail mem test at 96MHz BCLK?! ;;003 ;; 403 ;; if written to cscr, should say (1) hcklk out on clk0, and (2) restart pll,usbpll and (3) pll divide by 2 (PRESC) NOT
                                       	;;  and bclkdiv = 1/2 FULL core freq  (didn't run at 192/1!!... might be a core voltage issue!!)
CSCR_SETTING	EQU  0x8f600003	;; also make hclck out on clk0... and hclk /1

;;CLOCK name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
MPCTL0_SETTING			EQU		MHZ150
PCDR_SETTINGS			EQU		PERCLK1_DIV6


;; Cs0 for ROM
CS0U      EQU  0x00220000     ;; CS0 uppder and lower control register          ;; default has wsc =111110, I want to clear it for 2 waits (min)
CS0L      EQU  0x00220004

WSCEQU1   	EQU  0x00000700 	;; wait states = 7, works for boot (48Mhz system clk, 90 ns Flash) and app (96MHz,90ns), but should it?  Original setting was done when 70ns parts were thought to be used
APPWSCEQU1	EQU  0x00000900   	;; up wait states to 9 when 96MHz bus


;; Uart 1 SSI registers
URXDn_1        EQU  0x00206000
UTXDn_1        EQU  0x00206040
UCR1_1         EQU  0x00206080
UCR2_1         EQU  0x00206084
UCR3_1         EQU  0x00206088
UCR4_1         EQU  0x0020608c
UFCR_1         EQU  0x00206090
USR1_1         EQU  0x00206094
USR2_1         EQU  0x00206098
UESC_1         EQU  0x0020609c
UTIM_1         EQU  0x002060a0
UBIR_1         EQU  0x002060a4
UBMR_1         EQU  0x002060a8
UBRC_1         EQU  0x002060ac
BIPR1_1        EQU  0x002060b0
BIPR2_1        EQU  0x002060b4
BIPR3_1        EQU  0x002060b8
BIPR4_1        EQU  0x002060bc
BIMPR1_1       EQU  0x002060c0
BIMPR2_1       EQU  0x002060c4
BIMPR3_1       EQU  0x002060c8
BIMPR4_1       EQU  0x002060cc
UTS_1          EQU  0x002060d0

;; pins in GPR_C and GIUS_C that pertain to alternative functions
;; of uart1 tx,rx,rts,cts
UART1PINS      EQU  (BIT9 :OR: BIT10 :OR: BIT11 :OR: BIT12)
NUART1PINS     EQU  (:NOT: UART1PINS)

;;Uart1 masks
;;
USR2_1_BIT_14_TRANS_BUF_EMPTY_MASK		EQU		0x4000

;;UART1 name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
UFCR_1_SETTINGS			EQU		0x0A81
UCR1_1_SETTINGS			EQU		0x00000005
UCR2_1_SETTINGS			EQU		0x00005026
UCR3_1_SETTINGS			EQU             0x00000000	;;default values
UCR4_1_SETTINGS			EQU		0x8040
UBIR_1_SETTINGS			EQU		1151		;; So 1151 and 9999 works good for mxl with 16mhz clock
UBMR_1_SETTINGS			EQU		9999


;; Watchdog definitions
WATCHDOG_BASE		EQU	0x00201000
WCR			EQU	WATCHDOG_BASE + 0x00
WSR			EQU	WATCHDOG_BASE + 0x04

;;Watchdog name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
WATCHDOG_BASE_ADDR_UPPER		EQU	0x00200000
WATCHDOG_BASE_ADDR_LOWER		EQU	0x00001000
WATCHDOG_BASE_ADDR_WCR_OFFSET		EQU	0x00000000
WATCHDOG_BASE_ADDR_WSR_OFFSET		EQU	0x00000004


;; General purpose timers
TCTL1		EQU	0x00202000
TPRER1		EQU	0x00202004
TCMP1		EQU	0x00202008
TCR1		EQU	0x0020200C
TCN1		EQU	0x00202010
TSTAT1		EQU	0x00202014

TCTL2		EQU	0x00203000
TPRER2		EQU	0x00203004
TCMP2		EQU	0x00203008
TCR2		EQU	0x0020300C
TCN2		EQU	0x00203010
TSTAT2		EQU	0x00203014

;;GPT1 name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
GPT1_BASE_ADDR_UPPER			EQU	0x00200000
GPT1_BASE_ADDR_LOWER			EQU	0x00002000
GPT1_BASE_ADDR_TCN1_OFFSET		EQU	0x00000010



;; Real Time Clock
RTC_BASE_ADDR		EQU		0x00204000
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

;;RTC name conversion table
;; mxl has the following nomenclature, so to ease the conversion of manual to platform naming convention, redundantly re-define pin names.
;;
RTC_BASE_ADDR_UPPER			EQU	0x00200000
RTC_BASE_ADDR_LOWER			EQU	0x00004000
RTC_BASE_ADDR_RTC_SECOND_OFFSET		EQU	0x00000004



;; Port A
PORTA_BASE          EQU  0x0021c000
PORTA_DDIR          EQU  PORTA_BASE
PORTA_OCR1          EQU  PORTA_BASE + 0x4
PORTA_OCR2          EQU  PORTA_BASE + 0x8
PORTA_ICONFA1       EQU  PORTA_BASE + 0x0c
PORTA_ICONFA2       EQU  PORTA_BASE + 0x10
PORTA_ICONFB1       EQU  PORTA_BASE + 0x14
PORTA_ICONFB2       EQU  PORTA_BASE + 0x18
PORTA_DR            EQU  PORTA_BASE + 0x1c
PORTA_GIUS          EQU  PORTA_BASE + 0x20
PORTA_SSR           EQU  PORTA_BASE + 0x24
PORTA_ICR1          EQU  PORTA_BASE + 0x28
PORTA_ICR2          EQU  PORTA_BASE + 0x2c
PORTA_IMR           EQU  PORTA_BASE + 0x30
PORTA_ISR           EQU  PORTA_BASE + 0x34
PORTA_GPR           EQU  PORTA_BASE + 0x38
PORTA_SWR           EQU  PORTA_BASE + 0x3c
PORTA_PUEN          EQU  PORTA_BASE + 0x040

PORTA_MASK          EQU  0xffffffff

;;PortA name conversion table
;; new manual has the following nomenclature, so to ease the conversion of manual to naming convention, redundantly re-define pin names.
;;PORTA_BASE    EQU	0x0021c000
DDIR_A    	EQU	PORTA_DDIR
OCR1_A		EQU     PORTA_OCR1
OCR2_A		EQU     PORTA_OCR2
ICONFA1_A	EQU	PORTA_ICONFA1
ICONFA2_A	EQU	PORTA_ICONFA2
ICONFB1_A	EQU	PORTA_ICONFB1
ICONFB2_A	EQU	PORTA_ICONFB2
DR_A		EQU	PORTA_DR
GIUS_A		EQU	PORTA_GIUS
SSR_A		EQU	PORTA_SSR
ICR1_A		EQU	PORTA_ICR1
ICR2_A		EQU	PORTA_ICR2
IMR_A		EQU	PORTA_IMR
ISR_A		EQU	PORTA_ISR
GPR_A		EQU	PORTA_GPR
SWR_A		EQU	PORTA_SWR
PUEN_A		EQU	PORTA_PUEN

;; Port B
PORTB_BASE          EQU  0x0021c100
PORTB_DDIR          EQU  PORTB_BASE
PORTB_OCR1          EQU  PORTB_BASE + 0x4
PORTB_OCR2          EQU  PORTB_BASE + 0x8
PORTB_ICONFA1       EQU  PORTB_BASE + 0x0c
PORTB_ICONFA2       EQU  PORTB_BASE + 0x10
PORTB_ICONFB1       EQU  PORTB_BASE + 0x14
PORTB_ICONFB2       EQU  PORTB_BASE + 0x18
PORTB_DR            EQU  PORTB_BASE + 0x1c
PORTB_GIUS          EQU  PORTB_BASE + 0x20
PORTB_SSR           EQU  PORTB_BASE + 0x24
PORTB_ICR1          EQU  PORTB_BASE + 0x28
PORTB_ICR2          EQU  PORTB_BASE + 0x2c
PORTB_IMR           EQU  PORTB_BASE + 0x30
PORTB_ISR           EQU  PORTB_BASE + 0x34
PORTB_GPR           EQU  PORTB_BASE + 0x38
PORTB_SWR           EQU  PORTB_BASE + 0x3c
PORTB_PUEN          EQU  PORTB_BASE + 0x040

PORTB_MASK          EQU  0xffffffff

;;PortB name conversion table
;; new manual has the following nomenclature, so to ease the conversion of manual to naming convention, redundantly re-define pin names.
;;PORTB_BASE          EQU  0x0021c100
DDIR_B		EQU  PORTB_DDIR
OCR1_B		EQU  PORTB_OCR1
OCR2_B		EQU  PORTB_OCR2
ICONFA1_B	EQU  PORTB_ICONFA1
ICONFA2_B	EQU  PORTB_ICONFA2
ICONFB1_B	EQU  PORTB_ICONFB1
ICONFB2_B	EQU  PORTB_ICONFB2
DR_B		EQU  PORTB_DR
GIUS_B		EQU  PORTB_GIUS
SSR_B		EQU  PORTB_SSR
ICR1_B		EQU  PORTB_ICR1
ICR2_B		EQU  PORTB_ICR2
IMR_B		EQU  PORTB_IMR
ISR_B		EQU  PORTB_ISR
GPR_B		EQU  PORTB_GPR
SWR_B		EQU  PORTB_SWR
PUEN_B		EQU  PORTB_PUEN


;; Port C
PORTC_BASE          EQU  0x0021c200
PORTC_DDIR          EQU  PORTC_BASE
PORTC_OCR1          EQU  PORTC_BASE + 0x4
PORTC_OCR2          EQU  PORTC_BASE + 0x8
PORTC_ICONFA1       EQU  PORTC_BASE + 0x0c
PORTC_ICONFA2       EQU  PORTC_BASE + 0x10
PORTC_ICONFB1       EQU  PORTC_BASE + 0x14
PORTC_ICONFB2       EQU  PORTC_BASE + 0x18
PORTC_DR            EQU  PORTC_BASE + 0x1c
PORTC_GIUS          EQU  PORTC_BASE + 0x20
PORTC_SSR           EQU  PORTC_BASE + 0x24
PORTC_ICR1          EQU  PORTC_BASE + 0x28
PORTC_ICR2          EQU  PORTC_BASE + 0x2c
PORTC_IMR           EQU  PORTC_BASE + 0x30
PORTC_ISR           EQU  PORTC_BASE + 0x34
PORTC_GPR           EQU  PORTC_BASE + 0x38
PORTC_SWR           EQU  PORTC_BASE + 0x3c
PORTC_PUEN          EQU  PORTC_BASE + 0x040

PORTC_MASK          EQU  0xffffffff

;;PortC name conversion table
;; new manual has the following nomenclature, so to ease the conversion of manual to naming convention, redundantly re-define pin names.
;;PORTC_BASE    EQU	0x0021c200
DDIR_C    	EQU     PORTC_DDIR
OCR1_C    	EQU     PORTC_OCR1
OCR2_C		EQU     PORTC_OCR2
ICONFA1		EQU	PORTC_ICONFA1
ICONFA2		EQU	PORTC_ICONFA2
ICONFB1		EQU	PORTC_ICONFB1
ICONFB2		EQU	PORTC_ICONFB2
DR_C		EQU	PORTC_DR
GIUS_C		EQU	PORTC_GIUS
SSR_C		EQU	PORTC_SSR
ICR1_C		EQU	PORTC_ICR1
ICR2_C		EQU	PORTC_ICR2
IMR_C		EQU	PORTC_IMR
ISR_C		EQU	PORTC_ISR
GPR_C		EQU	PORTC_GPR
SWR_C		EQU	PORTC_SWR
PUEN_C		EQU	PORTC_PUEN


;; Port D
PORTD_BASE          EQU  0x0021c300
PORTD_DDIR          EQU  PORTD_BASE
PORTD_OCR1          EQU  PORTD_BASE + 0x4
PORTD_OCR2          EQU  PORTD_BASE + 0x8
PORTD_ICONFA1       EQU  PORTD_BASE + 0x0c
PORTD_ICONFA2       EQU  PORTD_BASE + 0x10
PORTD_ICONFB1       EQU  PORTD_BASE + 0x14
PORTD_ICONFB2       EQU  PORTD_BASE + 0x18
PORTD_DR            EQU  PORTD_BASE + 0x1c
PORTD_GIUS          EQU  PORTD_BASE + 0x20
PORTD_SSR           EQU  PORTD_BASE + 0x24
PORTD_ICR1          EQU  PORTD_BASE + 0x28
PORTD_ICR2          EQU  PORTD_BASE + 0x2c
PORTD_IMR           EQU  PORTD_BASE + 0x30
PORTD_ISR           EQU  PORTD_BASE + 0x34
PORTD_GPR           EQU  PORTD_BASE + 0x38
PORTD_SWR           EQU  PORTD_BASE + 0x3c
PORTD_PUEN          EQU  PORTD_BASE + 0x040

PORTD_MASK          EQU  0xffffffff

;;PortD name conversion table
;; new manual has the following nomenclature, so to ease the conversion of manual to naming convention, redundantly re-define pin names.
;;PORTD_BASE    EQU   0x0021c300
DDIR_D		EQU   PORTD_DDIR
OCR1_D		EQU   PORTD_OCR1
OCR2_D		EQU   PORTD_OCR2
ICONFA1_D	EQU   PORTD_ICONFA1
ICONFA2_D	EQU   PORTD_ICONFA2
ICONFB1_D	EQU   PORTD_ICONFB1
ICONFB2_D	EQU   PORTD_ICONFB2
DR_D		EQU   PORTD_DR
GIUS_D		EQU   PORTD_GIUS
SSR_D		EQU   PORTD_SSR
ICR1_D		EQU   PORTD_ICR1
ICR2_D		EQU   PORTD_ICR2
IMR_D		EQU   PORTD_IMR
ISR_D		EQU   PORTD_ISR
GPR_D		EQU   PORTD_GPR
SWR_D		EQU   PORTD_SWR
PUEN_D		EQU   PORTD_PUEN


;; generic NOT bit defines... Recall having some trouble relying on standard ~ nomenclature with teh Strong ARM... just don't feel
;; debugging the same problem TWICE (or was it 3 times??)
NBIT20		EQU	0xffefffff
ZERO		EQU	0x0

BEEPER		EQU	BIT27
GRLED		EQU	BIT20
TERMCLK		EQU	BIT25

;; intending to step through some states for debug, looking at using the 3 bits bove
;; Beeper/Termclk/Grled = d2/d1/d0
DZERO		EQU	BIT20
DONE		EQU	BIT25
DTWO		EQU	BIT27
;; note state on power up of these bits are all 1's so state 8
;;
State0    EQU  0
State1    EQU  DZERO				;; d0 set
State2    EQU  DONE				;; d1 set
State3    EQU  (DONE :OR: DZERO)		;; count of 3
State4    EQU  DTWO				;; count 4
State5    EQU  (DTWO :OR: DZERO)		;; count 5
State6    EQU  (DTWO :OR: DONE)
State7    EQU  (DTWO :OR: DONE :OR: DZERO)     ;; step through


;; Stuff needed to support interrupts!!
;; raw registers
NIVESCR			EQU  0x00223040
FIVESCR			EQU  0x00223044
;; to avoid literal pool entering the game -- not even sure what the time/other implications are, but...
BASE_INT_REG		EQU  0x00200000
AITC_OFFSET		EQU  0x00023000
NIVESCR_REG_OFFSET	EQU  0x40
FIVESCR_REG_OFFSET	EQU  0x44
NIVESCR_RAW_OFFSET	EQU  (AITC_OFFSET+NIVESCR_REG_OFFSET)
FIVESCR_RAW_OFFSET	EQU  (AITC_OFFSET+FIVESCR_REG_OFFSET)

	END
; * ==========================================================================================================================
; * $Log: db_shared/Hardware_Header_Files/iMXL/MXL_hdw.i  $
; * Revision 1.1 2008/09/29 14:29:12EDT VanVolkD 
; * Initial revision
; * Member added to project d:/MKS_IT/Gen6FoundationApp/Gen6FoundationApp/Matrix_OS.pj
; * Initial revision
; * ======================================================================================================================= */
