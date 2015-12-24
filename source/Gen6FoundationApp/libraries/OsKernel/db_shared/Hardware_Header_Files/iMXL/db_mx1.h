/* ==============================================================================
   Matrix DragonBall MC9328MX1 hardware header file
   ==============================================================================

   $RCSfile: db_shared/Hardware_Header_Files/iMXL/db_mx1.h $
   $Revision: 1.3 $
   $Date: 2008/11/03 13:02:36EST $

   ==============================================================================
   Copyright (c) 2002 by HHP, Inc.
   ============================================================================== */

#ifndef MX1_REGISTERS_H_INCLUDED_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define MX1_REGISTERS_H_INCLUDED_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#if ((PLATFORM_CURRENT == imx27) || (PLATFORM_CURRENT == imx25))
#error iMX27 or iMX25 is defined but have added MXL routines
#endif	//Check to make sure mxl is not defined

/* ==============================================================================
	External Interface Module (EMI)
   ============================================================================== */

/* Chip Select Upper Control Registers */
typedef struct ChipSelReg_t
{
	union
	{
		struct
  		{
	  		volatile unsigned long			EDC				: 4;
  			volatile unsigned long			WWS				: 3;
  			volatile unsigned long			unused39			: 1;
	  		volatile unsigned long			WSC				: 6;
  			volatile unsigned long			CNC				: 2;
			volatile unsigned long			DOL				: 4;
	  		volatile unsigned long			SYNC				: 1;
  			volatile unsigned long			PME				: 1;
  			volatile unsigned long			PSZ				: 2;
	  		volatile unsigned long			BCS				: 4;
  			volatile unsigned long			BCD				: 2;
  			volatile unsigned long			unused62			: 1;
	  		volatile unsigned long			unused63			: 1;
  		} b;
	  	volatile unsigned long f;
  } Upper;

/* Chip Select Lower Control Register */
	union
	{
		struct
		{
			volatile unsigned long			CSEN				: 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			WP					: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			SP					: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			DSZ				: 3;
			volatile unsigned long			EBC				: 1;
			volatile unsigned long			CSA				: 4;
			volatile unsigned long			WEN				: 4;
			volatile unsigned long			WEA				: 4;
			volatile unsigned long			OEN				: 4;
			volatile unsigned long			OEA				: 4;
		} b;
		volatile unsigned long f;
	} Lower;
} CHIPSELREG_T;

/* WEIM Configuration Register */
/* table 10-6 */
typedef union
{
   	struct
   	{
   		volatile unsigned long			SHEN  			: 2;
   		volatile unsigned long			BCM  	 			: 1;
   		volatile unsigned long			GPO0  			: 1;
   		volatile unsigned long			GPO1  			: 1;
   		volatile unsigned long			GPO2	 	 		: 1;
   		volatile unsigned long			GPO3 		 		: 1;
   		volatile unsigned long			GPO4 	 			: 1;
   		volatile unsigned long			GPO5  			: 1;
   		volatile unsigned long			GPO6  			: 1;
   		volatile unsigned long			GPO7	 	 		: 1;
   		volatile unsigned long			GPO8 				: 1;
   		volatile unsigned long			GPO9 				: 1;
   		volatile unsigned long			GPO10				: 1;
   		volatile unsigned long			GPO11				: 1;
   		volatile unsigned long			GPO12				: 1;
   		volatile unsigned long			unused16			: 1;
   		volatile unsigned long			unused17			: 1;
   		volatile unsigned long			unused18			: 1;
   		volatile unsigned long			unused19			: 1;
   		volatile unsigned long			unused20			: 1;
   		volatile unsigned long			unused21			: 1;
   		volatile unsigned long			unused22			: 1;
   		volatile unsigned long			unused23			: 1;
   		volatile unsigned long			unused24			: 1;
   		volatile unsigned long			unused25			: 1;
   		volatile unsigned long			unused26			: 1;
   		volatile unsigned long			unused27			: 1;
   		volatile unsigned long			unused28			: 1;
   		volatile unsigned long			unused29			: 1;
   		volatile unsigned long			unused30			: 1;
   		volatile unsigned long			unused31			: 1;
   	} b;
	  	volatile unsigned long f;
} WEIM_T;


/* Chip Select 0-5 Control Registers */
/* table 10-4 */

#define BASEADDR 0x00220000

#define pChipSelectReg0	((CHIPSELREG_T *) BASEADDR)
#define ChipSelectReg0	(pChipSelectReg0->Lower.f)
#define pChipSelectReg1	((CHIPSELREG_T *) (BASEADDR + 0x008))
#define pChipSelectReg2	((CHIPSELREG_T *) (BASEADDR + 0x010))
#define pChipSelectReg3	((CHIPSELREG_T *) (BASEADDR + 0x018))
#define pChipSelectReg4	((CHIPSELREG_T *) (BASEADDR + 0x020))
#define pChipSelectReg5	((CHIPSELREG_T *) (BASEADDR + 0x028))
#define pWeimReg			((WEIM_T *) 		(BASEADDR + 0x030))




/* ==============================================================================
	GPIO port definitions
   ============================================================================== */
typedef union
{
     struct porta
  	{
  		volatile unsigned long			Bit0	  	: 1;
  		volatile unsigned long			Bit1	  	: 1;
  		volatile unsigned long			Bit2	  	: 1;
  		volatile unsigned long			Bit3	  	: 1;
  		volatile unsigned long			Bit4	  	: 1;
  		volatile unsigned long			Bit5	  	: 1;
  		volatile unsigned long			Bit6	  	: 1;
  		volatile unsigned long			Bit7	  	: 1;
  		volatile unsigned long			Bit8	  	: 1;
  		volatile unsigned long			Bit9	  	: 1;
  		volatile unsigned long			Bit10	  	: 1;
  		volatile unsigned long			Bit11	  	: 1;
  		volatile unsigned long			Bit12	  	: 1;
  		volatile unsigned long			Bit13	  	: 1;
  		volatile unsigned long			Bit14	  	: 1;
  		volatile unsigned long			Bit15	  	: 1;
  		volatile unsigned long			Bit16	  	: 1;
  		volatile unsigned long			Bit17	  	: 1;
  		volatile unsigned long			Bit18	  	: 1;
  		volatile unsigned long			Bit19	  	: 1;
  		volatile unsigned long			Bit20	  	: 1;
  		volatile unsigned long			Bit21	  	: 1;
  		volatile unsigned long			Bit22	  	: 1;
  		volatile unsigned long			Bit23	  	: 1;
  		volatile unsigned long			Bit24	  	: 1;
  		volatile unsigned long			Bit25	  	: 1;
  		volatile unsigned long			Bit26	  	: 1;
  		volatile unsigned long			Bit27	  	: 1;
  		volatile unsigned long			Bit28	  	: 1;
  		volatile unsigned long			Bit29	  	: 1;
  		volatile unsigned long			Bit30	  	: 1;
  		volatile unsigned long			Bit31	  	: 1;
	} b;
	volatile unsigned long f;
} PORTA_T;


typedef union
{
  	struct portb
  	{
  		volatile unsigned long			unused_0		: 1;
  		volatile unsigned long			unused_1		: 1;
  		volatile unsigned long			unused_2		: 1;
  		volatile unsigned long			unused_3		: 1;
  		volatile unsigned long			unused_4		: 1;
  		volatile unsigned long			unused_5		: 1;
  		volatile unsigned long			unused_6		: 1;
  		volatile unsigned long			unused_7		: 1;
  		volatile unsigned long			Bit8		  	: 1;
  		volatile unsigned long			Bit9		  	: 1;
  		volatile unsigned long			Bit10		  	: 1;
  		volatile unsigned long			Bit11		  	: 1;
  		volatile unsigned long			Bit12		  	: 1;
  		volatile unsigned long			Bit13		  	: 1;
  		volatile unsigned long			Bit14		  	: 1;
  		volatile unsigned long			Bit15		  	: 1;
  		volatile unsigned long			Bit16		  	: 1;
  		volatile unsigned long			Bit17		  	: 1;
  		volatile unsigned long			Bit18		  	: 1;
  		volatile unsigned long			Bit19		  	: 1;
  		volatile unsigned long			Bit20		  	: 1;
  		volatile unsigned long			Bit21		  	: 1;
  		volatile unsigned long			Bit22		  	: 1;
  		volatile unsigned long			Bit23		  	: 1;
  		volatile unsigned long			Bit24		  	: 1;
  		volatile unsigned long			Bit25		  	: 1;
  		volatile unsigned long			Bit26		  	: 1;
  		volatile unsigned long			Bit27		  	: 1;
  		volatile unsigned long			Bit28		  	: 1;
  		volatile unsigned long			Bit29		  	: 1;
  		volatile unsigned long			Bit30		  	: 1;
  		volatile unsigned long			Bit31		  	: 1;
	} b;
	volatile unsigned long f;
} PORTB_T;


typedef union
{
  	struct portc
  	{
  		volatile unsigned long			unused_0		: 1;
  		volatile unsigned long			unused_1		: 1;
  		volatile unsigned long			Bit2			: 1;//Had to define for OCR2 register setup of bit17
  		volatile unsigned long			Bit3 			: 1;
  		volatile unsigned long			Bit4 			: 1;
  		volatile unsigned long			Bit5 			: 1;
  		volatile unsigned long			Bit6 			: 1;
  		volatile unsigned long			Bit7 			: 1;
  		volatile unsigned long			Bit8 			: 1;
  		volatile unsigned long			Bit9 			: 1;
  		volatile unsigned long			Bit10			: 1;
  		volatile unsigned long			Bit11			: 1;
  		volatile unsigned long			Bit12			: 1;
  		volatile unsigned long			Bit13			: 1;
  		volatile unsigned long			Bit14			: 1;
  		volatile unsigned long			Bit15			: 1;
  		volatile unsigned long			Bit16			: 1;
  		volatile unsigned long			Bit17			: 1;
  		volatile unsigned long			unused_18	: 1;
  		volatile unsigned long			Bit19			: 1;
  		volatile unsigned long			Bit20			: 1;
  		volatile unsigned long			Bit21			: 1;
  		volatile unsigned long			Bit22			: 1;
  		volatile unsigned long			Bit23			: 1;
  		volatile unsigned long			Bit24			: 1;
  		volatile unsigned long			Bit25			: 1;
  		volatile unsigned long			Bit26			: 1;
  		volatile unsigned long			Bit27			: 1;
  		volatile unsigned long			Bit28			: 1;
  		volatile unsigned long			Bit29			: 1;
  		volatile unsigned long			Bit30			: 1;
  		volatile unsigned long			Bit31			: 1;
	} b;
	volatile unsigned long f;
} PORTC_T;


typedef union
{
  	struct portd
  	{
  		volatile unsigned long			unused_0		: 1;
  		volatile unsigned long			unused_1		: 1;
  		volatile unsigned long			unused_2		: 1;
  		volatile unsigned long			unused_3		: 1;
  		volatile unsigned long			unused_4		: 1;
  		volatile unsigned long			unused_5		: 1;
  		volatile unsigned long			Bit6		  	: 1;
  		volatile unsigned long			Bit7		  	: 1;
  		volatile unsigned long			Bit8		  	: 1;
  		volatile unsigned long			Bit9		  	: 1;
  		volatile unsigned long			Bit10		  	: 1;
  		volatile unsigned long			Bit11		  	: 1;
  		volatile unsigned long			Bit12		  	: 1;
  		volatile unsigned long			Bit13 		: 1;
  		volatile unsigned long			Bit14 		: 1;
  		volatile unsigned long			Bit15 		: 1;
  		volatile unsigned long			Bit16 		: 1;
  		volatile unsigned long			Bit17 		: 1;
  		volatile unsigned long			Bit18  	  	: 1;
  		volatile unsigned long			Bit19  	  	: 1;
  		volatile unsigned long			Bit20  	  	: 1;
  		volatile unsigned long			Bit21  	  	: 1;
  		volatile unsigned long			Bit22  	  	: 1;
  		volatile unsigned long			Bit23  	  	: 1;
  		volatile unsigned long			Bit24  	  	: 1;
  		volatile unsigned long			Bit25  	  	: 1;
  		volatile unsigned long			Bit26  	  	: 1;
  		volatile unsigned long			Bit27  	  	: 1;
  		volatile unsigned long			Bit28  	  	: 1;
  		volatile unsigned long			Bit29  	  	: 1;
  		volatile unsigned long			Bit30  	  	: 1;
  		volatile unsigned long			Bit31  	  	: 1;
	} b;
	volatile unsigned long f;
} PORTD_T;



/* GPIO port pin assignments */
/* Bit field flags */
#define   BIT0				  ((ULONG)(0x00000001 << (0)))
#define   BIT1          	  ((ULONG)(0x00000001 << (1)))
#define   BIT2    		      ((ULONG)(0x00000001 << (2)))
#define   BIT3    		      ((ULONG)(0x00000001 << (3)))
#define   BIT4    			  ((ULONG)(0x00000001 << (4)))
#define   BIT5    		      ((ULONG)(0x00000001 << (5)))
#define   BIT6        		  ((ULONG)(0x00000001 << (6)))
#define   BIT7        		  ((ULONG)(0x00000001 << (7)))
#define   BIT8         		  ((ULONG)(0x00000001 << (8)))
#define   BIT9           	  ((ULONG)(0x00000001 << (9)))
#define   BIT10               ((ULONG)(0x00000001 << (10)))
#define   BIT11               ((ULONG)(0x00000001 << (11)))
#define   BIT12               ((ULONG)(0x00000001 << (12)))
#define   BIT13               ((ULONG)(0x00000001 << (13)))
#define   BIT14               ((ULONG)(0x00000001 << (14)))
#define   BIT15               ((ULONG)(0x00000001 << (15)))
#define   BIT16               ((ULONG)(0x00000001 << (16)))
#define   BIT17               ((ULONG)(0x00000001 << (17)))
#define   BIT18               ((ULONG)(0x00000001 << (18)))
#define   BIT19               ((ULONG)(0x00000001 << (19)))
#define   BIT20               ((ULONG)(0x00000001 << (20)))
#define   BIT21               ((ULONG)(0x00000001 << (21)))
#define   BIT22               ((ULONG)(0x00000001 << (22)))
#define   BIT23               ((ULONG)(0x00000001 << (23)))
#define   BIT24               ((ULONG)(0x00000001 << (24)))
#define   BIT25               ((ULONG)(0x00000001 << (25)))
#define   BIT26               ((ULONG)(0x00000001 << (26)))
#define   BIT27               ((ULONG)(0x00000001 << (27)))
#define   BIT28               ((ULONG)(0x00000001 << (28)))
#define   BIT29               ((ULONG)(0x00000001 << (29)))
#define   BIT30               ((ULONG)(0x00000001 << (30)))
#define   BIT31               ((ULONG)((ULONG)0x00000001 << (31)))

#define   BIT32 		      ((ULONG)(0x00000001 << (0)))
#define   BIT33      	      ((ULONG)(0x00000001 << (1)))
#define   BIT34         	  ((ULONG)(0x00000001 << (2)))
#define   BIT35         	  ((ULONG)(0x00000001 << (3)))
#define   BIT36         	  ((ULONG)(0x00000001 << (4)))
#define   BIT37           	  ((ULONG)(0x00000001 << (5)))
#define   BIT38           	  ((ULONG)(0x00000001 << (6)))
#define   BIT39           	  ((ULONG)(0x00000001 << (7)))
#define   BIT40           	  ((ULONG)(0x00000001 << (8)))
#define   BIT41           	  ((ULONG)(0x00000001 << (9)))
#define   BIT42               ((ULONG)(0x00000001 << (10)))
#define   BIT43               ((ULONG)(0x00000001 << (11)))
#define   BIT44               ((ULONG)(0x00000001 << (12)))
#define   BIT45               ((ULONG)(0x00000001 << (13)))
#define   BIT46               ((ULONG)(0x00000001 << (14)))
#define   BIT47               ((ULONG)(0x00000001 << (15)))
#define   BIT48               ((ULONG)(0x00000001 << (16)))
#define   BIT49               ((ULONG)(0x00000001 << (17)))
#define   BIT50               ((ULONG)(0x00000001 << (18)))
#define   BIT51               ((ULONG)(0x00000001 << (19)))
#define   BIT52               ((ULONG)(0x00000001 << (20)))
#define   BIT53               ((ULONG)(0x00000001 << (21)))
#define   BIT54               ((ULONG)(0x00000001 << (22)))
#define   BIT55               ((ULONG)(0x00000001 << (23)))
#define   BIT56               ((ULONG)(0x00000001 << (24)))
#define   BIT57               ((ULONG)(0x00000001 << (25)))
#define   BIT58               ((ULONG)(0x00000001 << (26)))
#define   BIT59               ((ULONG)(0x00000001 << (27)))
#define   BIT60               ((ULONG)(0x00000001 << (28)))
#define   BIT61               ((ULONG)(0x00000001 << (29)))
#define   BIT62               ((ULONG)(0x00000001 << (30)))
#define   BIT63               ((ULONG)((ULONG)0x00000001 << (31)))

/* PORT_A */
#define  BASE_PORTA	0x0021C000

#define  pDDIR_A     	((PORTA_T *) (BASE_PORTA))
#define  DDIR_A			(pDDIR_A->f)

#define  pOCR1_A     	((PORTA_T *) (BASE_PORTA +0x004))
#define  OCR1_A  			(pOCR1_A->f)

#define  pOCR2_A     	((PORTA_T *) (BASE_PORTA +0x008))
#define  OCR2_A			(pOCR2_A->f)

#define  pICONFA1_A		((PORTA_T *) (BASE_PORTA +0x00C))
#define  ICONFA1_A		(pICONFA1_A->f)

#define  pICONFA2_A		((PORTA_T *) (BASE_PORTA +0x010))
#define  ICONFA2_A		(pICONFA2_A->f)

#define  pICONFB1_A		((PORTA_T *) (BASE_PORTA +0x014))
#define  ICONFB1_A		(pICONFB1_A->f)

#define  pICONFB2_A		((PORTA_T *) (BASE_PORTA +0x018))
#define  ICONFB2_A		(pICONFB2_A->f)

#define  pDR_A				((PORTA_T *) (BASE_PORTA +0x01C))
#define  DR_A				(pDR_A->f)

#define  pGIUS_A			((PORTA_T *) (BASE_PORTA +0x020))
#define  GIUS_A			(pGIUS_A->f)

#define  pSSR_A			((PORTA_T *) (BASE_PORTA +0x024))
#define  SSR_A				(pSSR_A->f)

#define  pICR1_A			((PORTA_T *) (BASE_PORTA +0x028))
#define  ICR1_A			(pICR1_A->f)

#define  pICR2_A			((PORTA_T *) (BASE_PORTA +0x02C))
#define  ICR2_A			(pICR2_A->f)

#define  pIMR_A			((PORTA_T *) (BASE_PORTA +0x030))
#define  IMR_A				(pIMR_A->f)

#define  pISR_A			((PORTA_T *) (BASE_PORTA +0x034))
#define  ISR_A				(pISR_A->f)

#define  pGPR_A			((PORTA_T *) (BASE_PORTA +0x038))
#define  GPR_A				(pGPR_A->f)

#define  pSWR_A			((PORTA_T *) (BASE_PORTA +0x03C))
#define  SWR_A				(pSWR_A->f)

#define  pPUEN_A			((PORTA_T *) (BASE_PORTA +0x040))
#define  PUEN_A			(pPUEN_A->f)



/* PORT_B */
#define  BASE_PORTB	0x0021C100

#define  pDDIR_B     	((PORTB_T *)BASE_PORTB)
#define  DDIR_B			(pDDIR_B->f)

#define  pOCR1_B     	((PORTB_T *) (BASE_PORTB +0x004))
#define  OCR1_B  			(pOCR1_B->f)

#define  pOCR2_B     	((PORTB_T *) (BASE_PORTB +0x008))
#define  OCR2_B			(pOCR2_B->f)

#define  pICONFA1_B		((PORTB_T *) (BASE_PORTB +0x00C))
#define  ICONFA1_B		(pICONFA1_B->f)

#define  pICONFA2_B		((PORTB_T *) (BASE_PORTB +0x010))
#define  ICONFA2_B		(pICONFA2_B->f)

#define  pICONFB1_B		((PORTB_T *) (BASE_PORTB +0x014))
#define  ICONFB1_B		(pICONFB1_B->f)

#define  pICONFB2_B		((PORTB_T *) (BASE_PORTB +0x018))
#define  ICONFB2_B		(pICONFB2_B->f)

#define  pDR_B				((PORTB_T *) (BASE_PORTB +0x01C))
#define  DR_B				(pDR_B->f)

#define  pGIUS_B			((PORTB_T *) (BASE_PORTB +0x020))
#define  GIUS_B			(pGIUS_B->f)

#define  pSSR_B			((PORTB_T *) (BASE_PORTB +0x024))
#define  SSR_B				(pSSR_B->f)

#define  pICR1_B			((PORTB_T *) (BASE_PORTB +0x028))
#define  ICR1_B			(pICR1_B->f)

#define  pICR2_B			((PORTB_T *) (BASE_PORTB +0x02C))
#define  ICR2_B			(pICR2_B->f)

#define  pIMR_B			((PORTB_T *) (BASE_PORTB +0x030))
#define  IMR_B				(pIMR_B->f)

#define  pISR_B			((PORTB_T *) (BASE_PORTB +0x034))
#define  ISR_B				(pISR_B->f)

#define  pGPR_B			((PORTB_T *) (BASE_PORTB +0x038))
#define  GPR_B				(pGPR_B->f)

#define  pSWR_B			((PORTB_T *) (BASE_PORTB +0x03C))
#define  SWR_B				(pSWR_B->f)

#define  pPUEN_B			((PORTB_T *) (BASE_PORTB +0x040))
#define  PUEN_B			(pPUEN_B->f)



/* PORT_C */
#define  BASE_PORTC	0x0021C200

#define  pDDIR_C     	((PORTC_T *)BASE_PORTC)
#define  DDIR_C			(pDDIR_C->f)

#define  pOCR1_C     	((PORTC_T *) (BASE_PORTC +0x004))
#define  OCR1_C  			(pOCR1_C->f)

#define  pOCR2_C     	((PORTC_T *) (BASE_PORTC +0x008))
#define  OCR2_C			(pOCR2_C->f)

#define  pICONFA1_C		((PORTC_T *) (BASE_PORTC +0x00C))
#define  ICONFA1_C		(pICONFA1_C->f)

#define  pICONFA2_C		((PORTC_T *) (BASE_PORTC +0x010))
#define  ICONFA2_C		(pICONFA2_C->f)

#define  pICONFB1_C		((PORTC_T *) (BASE_PORTC +0x014))
#define  ICONFB1_C		(pICONFB1_C->f)

#define  pICONFB2_C		((PORTC_T *) (BASE_PORTC +0x018))
#define  ICONFB2_C		(pICONFB2_C->f)

#define  pDR_C				((PORTC_T *) (BASE_PORTC +0x01C))
#define  DR_C				(pDR_C->f)

#define  pGIUS_C			((PORTC_T *) (BASE_PORTC +0x020))
#define  GIUS_C			(pGIUS_C->f)

#define  pSSR_C			((PORTC_T *) (BASE_PORTC +0x024))
#define  SSR_C				(pSSR_C->f)

#define  pICR1_C			((PORTC_T *) (BASE_PORTC +0x028))
#define  ICR1_C			(pICR1_C->f)

#define  pICR2_C			((PORTC_T *) (BASE_PORTC +0x02C))
#define  ICR2_C			(pICR2_C->f)

#define  pIMR_C			((PORTC_T *) (BASE_PORTC +0x030))
#define  IMR_C				(pIMR_C->f)

#define  pISR_C			((PORTC_T *) (BASE_PORTC +0x034))
#define  ISR_C				(pISR_C->f)

#define  pGPR_C			((PORTC_T *) (BASE_PORTC +0x038))
#define  GPR_C				(pGPR_C->f)

#define  pSWR_C			((PORTC_T *) (BASE_PORTC +0x03C))
#define  SWR_C				(pSWR_C->f)

#define  pPUEN_C			((PORTC_T *) (BASE_PORTC +0x040))
#define  PUEN_C			(pPUEN_C->f)


/* PORT_D */
#define  BASE_PORTD	0x0021C300

#define  pDDIR_D     	((PORTD_T *)BASE_PORTD)
#define  DDIR_D			(pDDIR_D->f)

#define  pOCR1_D     	((PORTD_T *) (BASE_PORTD +0x004))
#define  OCR1_D  			(pOCR1_D->f)

#define  pOCR2_D     	((PORTD_T *) (BASE_PORTD +0x008))
#define  OCR2_D			(pOCR2_D->f)

#define  pICONFA1_D		((PORTD_T *) (BASE_PORTD +0x00C))
#define  ICONFA1_D		(pICONFA1_D->f)

#define  pICONFA2_D		((PORTD_T *) (BASE_PORTD +0x010))
#define  ICONFA2_D		(pICONFA2_D->f)

#define  pICONFB1_D		((PORTD_T *) (BASE_PORTD +0x014))
#define  ICONFB1_D		(pICONFB1_D->f)

#define  pICONFB2_D		((PORTD_T *) (BASE_PORTD +0x018))
#define  ICONFB2_D		(pICONFB2_D->f)

#define  pDR_D				((PORTD_T *) (BASE_PORTD +0x01C))
#define  DR_D				(pDR_D->f)

#define  pGIUS_D			((PORTD_T *) (BASE_PORTD +0x020))
#define  GIUS_D			(pGIUS_D->f)

#define  pSSR_D			((PORTD_T *) (BASE_PORTD +0x024))
#define  SSR_D				(pSSR_D->f)

#define  pICR1_D			((PORTD_T *) (BASE_PORTD +0x028))
#define  ICR1_D			(pICR1_D->f)

#define  pICR2_D			((PORTD_T *) (BASE_PORTD +0x02C))
#define  ICR2_D			(pICR2_D->f)

#define  pIMR_D			((PORTD_T *) (BASE_PORTD +0x030))
#define  IMR_D				(pIMR_D->f)

#define  pISR_D			((PORTD_T *) (BASE_PORTD +0x034))
#define  ISR_D				(pISR_D->f)

#define  pGPR_D			((PORTD_T *) (BASE_PORTD +0x038))
#define  GPR_D				(pGPR_D->f)

#define  pSWR_D			((PORTD_T *) (BASE_PORTD +0x03C))
#define  SWR_D				(pSWR_D->f)

#define  pPUEN_D			((PORTD_T *) (BASE_PORTD +0x040))
#define  PUEN_D			(pPUEN_D->f)


//These lines would not be in the header file but are examples of calls you could make in a C file that includes this heaeder.
/*
pDDIR_A->b.21 = 0;				These all do the same thing
pDDIR_A->f &= ~BIT21;
DDIR_A &= ~BIT21;

//These lines would not be in the header file but are examples of calls you could make in a C file that includes this heaeder.

pDDIR_B->b.21 = 0;            These all do the same thing
pDDIR_B->f &= ~BIT21;
DDIR_B &= ~BIT21;
*/


/* ==============================================================================
	Watchdog registers
   ============================================================================== */
/* Watchdog Control Register (WCR) */
/* table 13-3 */

typedef struct Watchdog_T
{
	union
	{
		struct
		{
			volatile unsigned long			WDE 				: 1;
			volatile unsigned long			WDEC		 		: 1;
			volatile unsigned long			SWR 				: 1;
			volatile unsigned long			TMD		 		: 1;
			volatile unsigned long			WIE		  		: 1;
			volatile unsigned long			unused5 			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7	 		: 1;
			volatile unsigned long			WT			  		: 7;
			volatile unsigned long			WHALT	  		   : 1;
			volatile unsigned long			unused16	  		: 1;
			volatile unsigned long			unused17	  		: 1;
			volatile unsigned long			unused18  		: 1;
			volatile unsigned long			unused19	  		: 1;
			volatile unsigned long			unused20		 	: 1;
			volatile unsigned long			unused21	 		: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26		  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;
  		volatile unsigned long f;
	} Wcr;

/* Watchdog Service Register (WSR) */
/* table 13-4 */
	union
	{
      struct
		{
			volatile unsigned long			HSC		  		: 8;
			volatile unsigned long			VSC			 	: 8;
			volatile unsigned long			LVRM		 		: 3;
			volatile unsigned long			BTS				: 2;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			SCE				: 1;
			volatile unsigned long			AFS				: 2;
			volatile unsigned long			DRM			  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;
  		volatile unsigned long f;
	} Wsr;


/* Watchdog Status Register */
/* table 13-5 */
	union
	{
      struct
		{
			volatile unsigned long			TOUT 				: 1;
			volatile unsigned long			unused1	 		: 1;
			volatile unsigned long			unused2 			: 1;
			volatile unsigned long			unused3	 		: 1;
			volatile unsigned long			unused4	  		: 1;
			volatile unsigned long			unused5 			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7	 		: 1;
			volatile unsigned long			TINT		  		: 1;
			volatile unsigned long			unused9	 		: 1;
			volatile unsigned long			unused10	 		: 1;
			volatile unsigned long			unused11	 		: 1;
			volatile unsigned long			unused12	 		: 1;
			volatile unsigned long			unused13	 		: 1;
			volatile unsigned long			unused14	 		: 1;
			volatile unsigned long			unused15	  		: 1;
			volatile unsigned long			unused16	  		: 1;
			volatile unsigned long			unused17	  		: 1;
			volatile unsigned long			unused18  		: 1;
			volatile unsigned long			unused19	  		: 1;
			volatile unsigned long			unused20		 	: 1;
			volatile unsigned long			unused21	 		: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26		  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;
  		volatile unsigned long f;
	} Wstr;

} WATCHDOG_T;

#define pWATCHDOG		((WATCHDOG_T *) 0x00201000)
#define WATCHDOG		(*pWATCHDOG)


/* ==============================================================================
	DMA Controller
   ============================================================================== */


typedef union
{
  	struct
	{
		volatile unsigned long			CHAN0				: 1;
		volatile unsigned long			CHAN1 			: 1;
  		volatile unsigned long			CHAN2 			: 1;
		volatile unsigned long			CHAN3 			: 1;
  		volatile unsigned long			CHAN4 			: 1;
  		volatile unsigned long			CHAN5		 		: 1;
  		volatile unsigned long			CHAN6 			: 1;
  		volatile unsigned long			CHAN7 			: 1;
  		volatile unsigned long			CHAN8 			: 1;
  		volatile unsigned long			CHAN9 			: 1;
  		volatile unsigned long			CHAN10			: 1;
  		volatile unsigned long			unused11			: 1;
  		volatile unsigned long			unused12			: 1;
  		volatile unsigned long			unused13			: 1;
  		volatile unsigned long			unused14			: 1;
  		volatile unsigned long			unused15			: 1;
  		volatile unsigned long			unused16			: 1;
  		volatile unsigned long			unused17			: 1;
		volatile unsigned long			unused18			: 1;
		volatile unsigned long			unused19			: 1;
  		volatile unsigned long			unused20			: 1;
  		volatile unsigned long			unused21			: 1;
  		volatile unsigned long			unused22			: 1;
  		volatile unsigned long			unused23			: 1;
		volatile unsigned long			unused24			: 1;
		volatile unsigned long			unused25			: 1;
  		volatile unsigned long			unused26			: 1;
  		volatile unsigned long			unused27			: 1;
  		volatile unsigned long			unused28			: 1;
		volatile unsigned long			unused29			: 1;
		volatile unsigned long			unused30			: 1;
  		volatile unsigned long			unused31			: 1;
  	} b;
	volatile unsigned long f;
}	DMA_CHAN_T;

typedef struct Dma_t
{

/* DMA Control Register */
/* table 12-3 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			DEN				: 1;
  			volatile unsigned long			DRST	 			: 1;
	  		volatile unsigned long			unused2 			: 1;
  			volatile unsigned long			unused3 			: 1;
  			volatile unsigned long			unused4 	 		: 1;
	  		volatile unsigned long			unused5	 		: 1;
  			volatile unsigned long			unused6 			: 1;
  			volatile unsigned long			unused7 			: 1;
	  		volatile unsigned long			unused8 			: 1;
  			volatile unsigned long			unused9 	 		: 1;
  			volatile unsigned long			unused10			: 1;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Dcr;

/* DMA Interrupt Status Register */
/* table 12-4 */
DMA_CHAN_T	  Disr;

/* DMA Interrupt Mask Register */
/* table 12-5 */
DMA_CHAN_T	Dimr;

/* DMA Burst Time-Out Status Register */
/* table 12-6 */
DMA_CHAN_T	Dbtosr;

/* DMA Request Time-Out Status Register */
/* table 12-7 */
DMA_CHAN_T	Drtosr;

/* DMA Transfer Error Status Register */
/* table 12-8 */
DMA_CHAN_T	Dsesr;

/* DMA Buffer Overflow Status Register */
/* table 12-9 */
DMA_CHAN_T	Dbosr;

/* DMA Burst Time-Out Control Register */
/* table 12-10 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			CNT				: 15;
  			volatile unsigned long			ENAB	 			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Dbtocr;
} DMA_GENERAL_T;


typedef struct Dma_Size_t
{
/* W-Size Register A */
/* table 12-11 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			WS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Wsra;

/* X-Size Register A */
/* table 12-12 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			XS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Xsra;

/* Y-Size Register A */
/* table 12-13 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			YS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Ysra;

/* W-Size Register b */
/* table 12-11 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			WS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Wsrb;

/* X-Size Register B */
/* table 12-12 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			XS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Xsrb;

/* Y-Size Register A */
/* table 12-13 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			YS					: 16;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Ysrb;
} DMA_SIZE_T;


typedef struct Dma_Chan_Control_T
{

/* Channel Source Address Register */
/* table 12-14 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			SA0			: 1;
  			volatile unsigned long			SA1	 		: 1;
	  		volatile unsigned long			SA2 			: 1;
  			volatile unsigned long			SA3 			: 1;
  			volatile unsigned long			SA4 	 		: 1;
	  		volatile unsigned long			SA5	 		: 1;
  			volatile unsigned long			SA6 			: 1;
  			volatile unsigned long			SA7 			: 1;
	  		volatile unsigned long			SA8 			: 1;
  			volatile unsigned long			SA9 	 		: 1;
  			volatile unsigned long			SA10			: 1;
  			volatile unsigned long			SA11			: 1;
	  		volatile unsigned long			SA12			: 1;
  			volatile unsigned long			SA13			: 1;
	  		volatile unsigned long			SA14			: 1;
  			volatile unsigned long			SA15			: 1;
  			volatile unsigned long			SA16			: 1;
	  		volatile unsigned long			SA17			: 1;
  			volatile unsigned long			SA18			: 1;
  			volatile unsigned long			SA19			: 1;
			volatile unsigned long			SA20			: 1;
			volatile unsigned long			SA21			: 1;
			volatile unsigned long			SA22			: 1;
			volatile unsigned long			SA23			: 1;
  			volatile unsigned long			SA24			: 1;
  			volatile unsigned long			SA25			: 1;
	  		volatile unsigned long			SA26			: 1;
			volatile unsigned long			SA27			: 1;
			volatile unsigned long			SA28			: 1;
  			volatile unsigned long			SA29			: 1;
  			volatile unsigned long			SA30			: 1;
	  		volatile unsigned long			SA31			: 1;
		} b;
  		volatile unsigned long f;
	}  Sar;


/* Channel Destination Address Register */
/* table 12-15 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			DA0			: 1;
  			volatile unsigned long			DA1 			: 1;
	  		volatile unsigned long			DA2 			: 1;
  			volatile unsigned long			DA3 			: 1;
  			volatile unsigned long			DA4 	 		: 1;
	  		volatile unsigned long			DA5	 		: 1;
  			volatile unsigned long			DA6 			: 1;
  			volatile unsigned long			DA7 			: 1;
	  		volatile unsigned long			DA8 			: 1;
  			volatile unsigned long			DA9 	 		: 1;
  			volatile unsigned long			DA10			: 1;
  			volatile unsigned long			DA11			: 1;
	  		volatile unsigned long			DA12			: 1;
  			volatile unsigned long			DA13			: 1;
	  		volatile unsigned long			DA14			: 1;
  			volatile unsigned long			DA15			: 1;
  			volatile unsigned long			DA16			: 1;
	  		volatile unsigned long			DA17			: 1;
  			volatile unsigned long			DA18			: 1;
  			volatile unsigned long			DA19			: 1;
			volatile unsigned long			DA20			: 1;
			volatile unsigned long			DA21			: 1;
			volatile unsigned long			DA22			: 1;
			volatile unsigned long			DA23			: 1;
  			volatile unsigned long			DA24			: 1;
  			volatile unsigned long			DA25			: 1;
	  		volatile unsigned long			DA26			: 1;
			volatile unsigned long			DA27			: 1;
			volatile unsigned long			DA28			: 1;
  			volatile unsigned long			DA29			: 1;
  			volatile unsigned long			DA30			: 1;
	  		volatile unsigned long			DA31			: 1;
		} b;
  		volatile unsigned long f;
	}  Dar;


/* Channel Count Register */
/* table 12-16 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			CNT0				: 1;
  			volatile unsigned long			CNT1 				: 1;
	  		volatile unsigned long			CNT2 				: 1;
  			volatile unsigned long			CNT3 				: 1;
  			volatile unsigned long			CNT4 	 			: 1;
	  		volatile unsigned long			CNT5	 			: 1;
  			volatile unsigned long			CNT6 				: 1;
  			volatile unsigned long			CNT7 				: 1;
	  		volatile unsigned long			CNT8 				: 1;
  			volatile unsigned long			CNT9 	 			: 1;
  			volatile unsigned long			CNT10				: 1;
  			volatile unsigned long			CNT11				: 1;
	  		volatile unsigned long			CNT12				: 1;
  			volatile unsigned long			CNT13				: 1;
	  		volatile unsigned long			CNT14				: 1;
  			volatile unsigned long			CNT15				: 1;
  			volatile unsigned long			CNT16				: 1;
	  		volatile unsigned long			CNT17				: 1;
  			volatile unsigned long			CNT18				: 1;
  			volatile unsigned long			CNT19				: 1;
			volatile unsigned long			CNT20				: 1;
			volatile unsigned long			CNT21				: 1;
			volatile unsigned long			CNT22				: 1;
			volatile unsigned long			CNT23				: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Cntr;

/* Channel Control Register */
/* table 12-17 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			CEN				: 1;
  			volatile unsigned long			FRC	 			: 1;
	  		volatile unsigned long			RPT	 			: 1;
  			volatile unsigned long			REN	 			: 1;
  			volatile unsigned long			SSIZ	 	 		: 2;
	  		volatile unsigned long			DSIZ		 		: 2;
  			volatile unsigned long			MSEL	 			: 1;
  			volatile unsigned long			MDIR	 			: 1;
	  		volatile unsigned long			SMOD	 			: 2;
  			volatile unsigned long			DMOD	 	 		: 2;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Ccr;


/* Channel Request Source Select Register */
/* table 12-19 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			RSS	 	 		: 5;
	  		volatile unsigned long			unused5	 		: 1;
  			volatile unsigned long			unused6 			: 1;
  			volatile unsigned long			unused7 			: 1;
	  		volatile unsigned long			unused8 			: 1;
  			volatile unsigned long			unused9 	 		: 1;
  			volatile unsigned long			unused10			: 1;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Rssr;

/* Channel Burst Length Register */
/* table 12-20 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			BL					: 6;
  			volatile unsigned long			unused6 			: 1;
  			volatile unsigned long			unused7 			: 1;
	  		volatile unsigned long			unused8 			: 1;
  			volatile unsigned long			unused9 	 		: 1;
  			volatile unsigned long			unused10			: 1;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Blr;


	union
	{
/* Channel Request Time-Out Register */
/* table 12-21 */
  		union
  		{
  		  	struct
  	  		{
  				volatile unsigned long			CNT				: 13;
  	  			volatile unsigned long			PSC	 			: 1;
  		  		volatile unsigned long			CLK	 			: 1;
  				volatile unsigned long			ENAB	 			: 1;
  				volatile unsigned long			unused16			: 1;
  		  		volatile unsigned long			unused17			: 1;
  	  			volatile unsigned long			unused18			: 1;
  				volatile unsigned long			unused19			: 1;
  				volatile unsigned long			unused20			: 1;
  				volatile unsigned long			unused21			: 1;
  				volatile unsigned long			unused22			: 1;
  				volatile unsigned long			unused23			: 1;
  				volatile unsigned long			unused24			: 1;
  				volatile unsigned long			unused25			: 1;
  		  		volatile unsigned long			unused26			: 1;
  				volatile unsigned long			unused27			: 1;
  				volatile unsigned long			unused28			: 1;
  				volatile unsigned long			unused29			: 1;
  	  			volatile unsigned long			unused30			: 1;
  		  		volatile unsigned long			unused31			: 1;
  			} b;
  			volatile unsigned long f;
  		}  Rtor;

/* Channel Bus Utilization Control Register */
/* table 12-22 */
  		union
  		{
  	  		struct
  		  	{
  				volatile unsigned long			CCNT				: 16;
  				volatile unsigned long			unused16			: 1;
 	  			volatile unsigned long			unused17			: 1;
  				volatile unsigned long			unused18			: 1;
  	  			volatile unsigned long			unused19			: 1;
  				volatile unsigned long			unused20			: 1;
  				volatile unsigned long			unused21			: 1;
  				volatile unsigned long			unused22			: 1;
  				volatile unsigned long			unused23			: 1;
				volatile unsigned long			unused24			: 1;
  				volatile unsigned long			unused25			: 1;
  	  			volatile unsigned long			unused26			: 1;
  				volatile unsigned long			unused27			: 1;
  				volatile unsigned long			unused28			: 1;
  				volatile unsigned long			unused29			: 1;
				volatile unsigned long			unused30			: 1;
  	  			volatile unsigned long			unused31			: 1;
  			} b;
 				volatile unsigned long f;
  		}  Bucr;
  	} SameAddress;

	unsigned reserved_padding[9];

} DMA_CHAN_CONTROL_T;



typedef struct
{
/* Test Control Register */
/* table 12-23 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			TEN				: 1;
  			volatile unsigned long			TFASIZ 			: 2;
	  		volatile unsigned long			TEAENAB 			: 1;
  			volatile unsigned long			EOBOCNT 			: 2;
  			volatile unsigned long			TREN	 	 		: 1;
	  		volatile unsigned long			TFBSIZ	 		: 2;
  			volatile unsigned long			TPRE	 			: 1;
  			volatile unsigned long			unused10			: 1;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Tcr;

/* Test FIFO A Register */
/* table 12-24 */
	volatile unsigned long Tfifoar;

/* Test DMA Request Register */
/* table 12-25 */
	volatile unsigned long Tdrr;

/* Test DMA In Progress Register */
/* table 12-26 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			TDIP				: 11;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
	  		volatile unsigned long			unused17			: 1;
  			volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
  			volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
	  		volatile unsigned long			unused31			: 1;
		} b;
  		volatile unsigned long f;
	}  Tdipr;

	/* Test FIFO B Register */
	/* table 12-27 */
	volatile unsigned long Tfifobr;

} DMA_TEST_T;

#define pDMA			((DMA_GENERAL_T *) 0x00209000)
#define DMA				(pDMA->)

#define pDmaSize		((DMA_SIZE_T *) 0x00209040)




#define pDmaChan0		((DMA_CHAN_CONTROL_T *) 0x00209080)
#define pDmaChan1		((DMA_CHAN_CONTROL_T *) 0x002090C0)
#define pDmaChan2		((DMA_CHAN_CONTROL_T *) 0x00209100)
#define pDmaChan3		((DMA_CHAN_CONTROL_T *) 0x00209140)
#define pDmaChan4		((DMA_CHAN_CONTROL_T *) 0x00209180)
#define pDmaChan5		((DMA_CHAN_CONTROL_T *) 0x002091C0)
#define pDmaChan6		((DMA_CHAN_CONTROL_T *) 0x00209200)
#define pDmaChan7		((DMA_CHAN_CONTROL_T *) 0x00209240)
#define pDmaChan8		((DMA_CHAN_CONTROL_T *) 0x00209280)
#define pDmaChan9		((DMA_CHAN_CONTROL_T *) 0x002092C0)
#define pDmaChan10	((DMA_CHAN_CONTROL_T *) 0x00209300)

#define pDmaTestT		((DMA_TEST_T *) 0x00209340)




/* ==============================================================================
	GP Timers
   ============================================================================== */

/* Timer 1 Control Register */
/* table 25-3 */
/* Separate now to allow optimization of accessor functions */
struct GPTimerControl_t
{
	union
	{
      struct
		{
			volatile unsigned long			TEN				: 1;
			volatile unsigned long			CLKSOURCE		: 3;
			volatile unsigned long			IRQEN				: 1;
			volatile unsigned long			OM					: 1;
			volatile unsigned long			CAP				: 2;
			volatile unsigned long			FRR				: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			SWR				: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	};
};


typedef struct GPTimers_t
{

/* Timer 1 Control Register */
/* table 25-3 */
	GPTimerControl_t  Tctl;

/* Timer Prescaler Register */
/* table 25-4 */
	union
	{
      struct
		{
			volatile unsigned long			PRESCALER		: 8;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Tprer;


/* Timer Compare Register */
/* table 25-5 */
	volatile unsigned long Tcmp;

/* Timer 1 Capture Register */
/* table 25-6 */
	volatile unsigned long Tcr;

/* Timer 1 Counter Register */
/* table 25-7 */
	volatile unsigned long Tcn;

/* Timer 1 Status Register */
/* table 25-8 */
	union
	{
      struct
		{
			volatile unsigned long			COMP				: 1;
			volatile unsigned long			CAPT				: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Tstat;

} GPTIMERS_T;

#define pGPTimer1		((GPTIMERS_T *) 0x00202000)
#define pGPTimer2		((GPTIMERS_T *) 0x00203000)

// Use these to clear the interupt flag
#define GPTIMER_COMP_BIT	1
#define GPTIMER_CAPT_BIT	2

/* ==============================================================================
	interrupt controller registers (AITC)
   ============================================================================== */
typedef struct Aitc_T
{

/* Interrupt Control Register */
/* table 9-7 */
	union
	{
		struct
		{
			volatile unsigned long			unused0			: 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			POINTER			: 10;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			MD					: 1;
			volatile unsigned long			FM					: 1;
			volatile unsigned long			NM					: 1;
			volatile unsigned long			FIAD				: 1;
			volatile unsigned long			NIAD				: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;
		volatile unsigned long f;
	} Intcntl;



/* Normal Interrupt Mask Register */
/* table 9-8 */
	union
	{
		struct
		{
			volatile unsigned long			NIMASK			: 5;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;
		volatile unsigned long f;
	} Nimask;

/* Interrupt Enable Number Register */
/* table 9-9 */
	union
	{
		struct
		{
			volatile unsigned long			ENNUM				: 6;
//			volatile unsigned long			ENNUM				: 5;
//			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;
		volatile unsigned long f;
	} Intennum;

/* Interrupt Disable Number Register */
/* table 9-10 */
	union
	{
		struct
		{
			volatile unsigned long			DISNUM		 	: 6;
//			volatile unsigned long			DISNUM		 	: 5;
//			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;
		volatile unsigned long f;
	} Intdisnum;


/* Interrupt Enable Register High */
/* table 9-11 */

	volatile unsigned long	Intenableh;

/* Interrupt Enable Register Low */
/* table 9-12 */

	volatile unsigned long	Intenablel;

/* Interrupt Type Register High */
/* table 9-13 */
	volatile unsigned long	Inttypeh;

/* Interrupt Type Register Low */
/* table 9-14 */
	volatile unsigned long	Inttypel;

/* Normal Interrupt Priority Level Register 7 */
/* table 9-15 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR56		 	: 4;
			volatile unsigned long			NIPR57			: 4;
			volatile unsigned long			NIPR58			: 4;
			volatile unsigned long			NIPR59			: 4;
			volatile unsigned long			NIPR60			: 4;
			volatile unsigned long			NIPR61			: 4;
			volatile unsigned long			NIPR62			: 4;
			volatile unsigned long			NIPR63			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority7;

/* Normal Interrupt Priority Level Register 6 */
/* table 9-16 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR48		 	: 4;
			volatile unsigned long			NIPR49			: 4;
			volatile unsigned long			NIPR50			: 4;
			volatile unsigned long			NIPR51			: 4;
			volatile unsigned long			NIPR52			: 4;
			volatile unsigned long			NIPR53			: 4;
			volatile unsigned long			NIPR54			: 4;
			volatile unsigned long			NIPR55			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority6;

/* Normal Interrupt Priority Level Register 5 */
/* table 9-17 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR40		 	: 4;
			volatile unsigned long			NIPR41			: 4;
			volatile unsigned long			NIPR42			: 4;
			volatile unsigned long			NIPR43			: 4;
			volatile unsigned long			NIPR44			: 4;
			volatile unsigned long			NIPR45			: 4;
			volatile unsigned long			NIPR46			: 4;
			volatile unsigned long			NIPR47			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority5;

/* Normal Interrupt Priority Level Register 4 */
/* table 9-18 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR32		 	: 4;
			volatile unsigned long			NIPR33			: 4;
			volatile unsigned long			NIPR34			: 4;
			volatile unsigned long			NIPR35			: 4;
			volatile unsigned long			NIPR36			: 4;
			volatile unsigned long			NIPR37			: 4;
			volatile unsigned long			NIPR38			: 4;
			volatile unsigned long			NIPR39			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority4;

/* Normal Interrupt Priority Level Register 3 */
/* table 9-19 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR24		 	: 4;
			volatile unsigned long			NIPR25			: 4;
			volatile unsigned long			NIPR26			: 4;
			volatile unsigned long			NIPR27			: 4;
			volatile unsigned long			NIPR28			: 4;
			volatile unsigned long			NIPR29			: 4;
			volatile unsigned long			NIPR30			: 4;
			volatile unsigned long			NIPR31			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority3;

/* Normal Interrupt Priority Level Register 2 */
/* table 9-20 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR16		 	: 4;
			volatile unsigned long			NIPR17			: 4;
			volatile unsigned long			NIPR18			: 4;
			volatile unsigned long			NIPR19			: 4;
			volatile unsigned long			NIPR20			: 4;
			volatile unsigned long			NIPR21			: 4;
			volatile unsigned long			NIPR22			: 4;
			volatile unsigned long			NIPR23			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority2;

/* Normal Interrupt Priority Level Register 1 */
/* table 9-21 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR8			 	: 4;
			volatile unsigned long			NIPR9				: 4;
			volatile unsigned long			NIPR10			: 4;
			volatile unsigned long			NIPR11			: 4;
			volatile unsigned long			NIPR12			: 4;
			volatile unsigned long			NIPR13			: 4;
			volatile unsigned long			NIPR14			: 4;
			volatile unsigned long			NIPR15			: 4;
		} b;
		volatile unsigned long f;
	} Nipriority1;


/* Normal Interrupt Priority Level Register 0 */
/* table 9-22 */
	union
	{
		struct
		{
			volatile unsigned long			NIPR0			 	: 4;
			volatile unsigned long			NIPR1				: 4;
			volatile unsigned long			NIPR2				: 4;
			volatile unsigned long			NIPR3				: 4;
			volatile unsigned long			NIPR4				: 4;
			volatile unsigned long			NIPR5				: 4;
			volatile unsigned long			NIPR6				: 4;
			volatile unsigned long			NIPR7				: 4;
		} b;
		volatile unsigned long f;
	} Nipriority0;


/* Normal Interrupt Vector and Status */
/* table 9-23 */
	union
	{
		struct
		{
			volatile unsigned long			NIPRILVL		: 16;
			volatile unsigned long			NIVECTOR		: 16;
		} b;
		volatile unsigned long f;
	} Nivecsr;

/* Fast Interrupt Vector and Status Register */
/* table 9-24 */
	volatile unsigned long		Fivecsr;

/* Interrupt Source Register High */
/* table 9-25 */
	union
	{
		struct
		{
			volatile unsigned long			Interrupt32		: 1;
			volatile unsigned long			Interrupt33		: 1;
			volatile unsigned long			Interrupt34		: 1;
			volatile unsigned long			Interrupt35		: 1;
			volatile unsigned long			Interrupt36		: 1;
			volatile unsigned long			Interrupt37		: 1;
			volatile unsigned long			Interrupt38		: 1;
			volatile unsigned long			Interrupt39		: 1;
			volatile unsigned long			Interrupt40		: 1;
			volatile unsigned long			Interrupt41		: 1;
			volatile unsigned long			Interrupt42		: 1;
			volatile unsigned long			Interrupt43		: 1;
			volatile unsigned long			Interrupt44		: 1;
			volatile unsigned long			Interrupt45		: 1;
			volatile unsigned long			Interrupt46		: 1;
			volatile unsigned long			Interrupt47		: 1;
			volatile unsigned long			Interrupt48		: 1;
			volatile unsigned long			Interrupt49		: 1;
			volatile unsigned long			Interrupt50		: 1;
			volatile unsigned long			Interrupt51		: 1;
			volatile unsigned long			Interrupt52		: 1;
			volatile unsigned long			Interrupt53		: 1;
			volatile unsigned long			Interrupt54		: 1;
			volatile unsigned long			Interrupt55		: 1;
			volatile unsigned long			Interrupt56		: 1;
			volatile unsigned long			Interrupt57		: 1;
			volatile unsigned long			Interrupt58		: 1;
			volatile unsigned long			Interrupt59		: 1;
			volatile unsigned long			Interrupt60		: 1;
			volatile unsigned long			Interrupt61		: 1;
			volatile unsigned long			Interrupt62		: 1;
			volatile unsigned long			Interrupt63		: 1;
		} b;
		volatile unsigned long f;
	} Intsrch;

/* Interrupt Source Register Low */
/* table 9-26 */
	union
	{
		struct
		{
			volatile unsigned long			Interrupt0		: 1;
			volatile unsigned long			Interrupt1		: 1;
			volatile unsigned long			Interrupt2		: 1;
			volatile unsigned long			Interrupt3		: 1;
			volatile unsigned long			Interrupt4		: 1;
			volatile unsigned long			Interrupt5		: 1;
			volatile unsigned long			Interrupt6		: 1;
			volatile unsigned long			Interrupt7		: 1;
			volatile unsigned long			Interrupt8		: 1;
			volatile unsigned long			Interrupt9		: 1;
			volatile unsigned long			Interrupt10		: 1;
			volatile unsigned long			Interrupt11		: 1;
			volatile unsigned long			Interrupt12		: 1;
			volatile unsigned long			Interrupt13		: 1;
			volatile unsigned long			Interrupt14		: 1;
			volatile unsigned long			Interrupt15		: 1;
			volatile unsigned long			Interrupt16		: 1;
			volatile unsigned long			Interrupt17		: 1;
			volatile unsigned long			Interrupt18		: 1;
			volatile unsigned long			Interrupt19		: 1;
			volatile unsigned long			Interrupt20		: 1;
			volatile unsigned long			Interrupt21		: 1;
			volatile unsigned long			Interrupt22		: 1;
			volatile unsigned long			Interrupt23		: 1;
			volatile unsigned long			Interrupt24		: 1;
			volatile unsigned long			Interrupt25		: 1;
			volatile unsigned long			Interrupt26		: 1;
			volatile unsigned long			Interrupt27		: 1;
			volatile unsigned long			Interrupt28		: 1;
			volatile unsigned long			Interrupt29		: 1;
			volatile unsigned long			Interrupt30		: 1;
			volatile unsigned long			Interrupt31		: 1;
		} b;
		volatile unsigned long f;
	} Intsrcl;

/* Interrupt Force Register High */
/* table 9-27 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptForce32		: 1;
			volatile unsigned long			InterruptForce33		: 1;
			volatile unsigned long			InterruptForce34		: 1;
			volatile unsigned long			InterruptForce35		: 1;
			volatile unsigned long			InterruptForce36		: 1;
			volatile unsigned long			InterruptForce37		: 1;
			volatile unsigned long			InterruptForce38		: 1;
			volatile unsigned long			InterruptForce39		: 1;
			volatile unsigned long			InterruptForce40		: 1;
			volatile unsigned long			InterruptForce41		: 1;
			volatile unsigned long			InterruptForce42		: 1;
			volatile unsigned long			InterruptForce43		: 1;
			volatile unsigned long			InterruptForce44		: 1;
			volatile unsigned long			InterruptForce45		: 1;
			volatile unsigned long			InterruptForce46		: 1;
			volatile unsigned long			InterruptForce47		: 1;
			volatile unsigned long			InterruptForce48		: 1;
			volatile unsigned long			InterruptForce49		: 1;
			volatile unsigned long			InterruptForce50		: 1;
			volatile unsigned long			InterruptForce51		: 1;
			volatile unsigned long			InterruptForce52		: 1;
			volatile unsigned long			InterruptForce53		: 1;
			volatile unsigned long			InterruptForce54		: 1;
			volatile unsigned long			InterruptForce55		: 1;
			volatile unsigned long			InterruptForce56		: 1;
			volatile unsigned long			InterruptForce57		: 1;
			volatile unsigned long			InterruptForce58		: 1;
			volatile unsigned long			InterruptForce59		: 1;
			volatile unsigned long			InterruptForce60		: 1;
			volatile unsigned long			InterruptForce61		: 1;
			volatile unsigned long			InterruptForce62		: 1;
			volatile unsigned long			InterruptForce63		: 1;
		} b;
		volatile unsigned long f;
	} Intfrch;

/* Interrupt Force Register Low */
/* table 9-28 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptForce0		: 1;
			volatile unsigned long			InterruptForce1		: 1;
			volatile unsigned long			InterruptForce2		: 1;
			volatile unsigned long			InterruptForce3		: 1;
			volatile unsigned long			InterruptForce4		: 1;
			volatile unsigned long			InterruptForce5		: 1;
			volatile unsigned long			InterruptForce6		: 1;
			volatile unsigned long			InterruptForce7		: 1;
			volatile unsigned long			InterruptForce8		: 1;
			volatile unsigned long			InterruptForce9		: 1;
			volatile unsigned long			InterruptForce10		: 1;
			volatile unsigned long			InterruptForce11		: 1;
			volatile unsigned long			InterruptForce12		: 1;
			volatile unsigned long			InterruptForce13		: 1;
			volatile unsigned long			InterruptForce14		: 1;
			volatile unsigned long			InterruptForce15		: 1;
			volatile unsigned long			InterruptForce16		: 1;
			volatile unsigned long			InterruptForce17		: 1;
			volatile unsigned long			InterruptForce18		: 1;
			volatile unsigned long			InterruptForce19		: 1;
			volatile unsigned long			InterruptForce20		: 1;
			volatile unsigned long			InterruptForce21		: 1;
			volatile unsigned long			InterruptForce22		: 1;
			volatile unsigned long			InterruptForce23		: 1;
			volatile unsigned long			InterruptForce24		: 1;
			volatile unsigned long			InterruptForce25		: 1;
			volatile unsigned long			InterruptForce26		: 1;
			volatile unsigned long			InterruptForce27		: 1;
			volatile unsigned long			InterruptForce28		: 1;
			volatile unsigned long			InterruptForce29		: 1;
			volatile unsigned long			InterruptForce30		: 1;
			volatile unsigned long			InterruptForce31		: 1;
		} b;
		volatile unsigned long f;
	} Intfrcl;

/* Normal Interrupt Pending Register High */
/* table 9-29 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptNormPending32		: 1;
			volatile unsigned long			InterruptNormPending33		: 1;
			volatile unsigned long			InterruptNormPending34		: 1;
			volatile unsigned long			InterruptNormPending35		: 1;
			volatile unsigned long			InterruptNormPending36		: 1;
			volatile unsigned long			InterruptNormPending37		: 1;
			volatile unsigned long			InterruptNormPending38		: 1;
			volatile unsigned long			InterruptNormPending39		: 1;
			volatile unsigned long			InterruptNormPending40		: 1;
			volatile unsigned long			InterruptNormPending41		: 1;
			volatile unsigned long			InterruptNormPending42		: 1;
			volatile unsigned long			InterruptNormPending43		: 1;
			volatile unsigned long			InterruptNormPending44		: 1;
			volatile unsigned long			InterruptNormPending45		: 1;
			volatile unsigned long			InterruptNormPending46		: 1;
			volatile unsigned long			InterruptNormPending47		: 1;
			volatile unsigned long			InterruptNormPending48		: 1;
			volatile unsigned long			InterruptNormPending49		: 1;
			volatile unsigned long			InterruptNormPending50		: 1;
			volatile unsigned long			InterruptNormPending51		: 1;
			volatile unsigned long			InterruptNormPending52		: 1;
			volatile unsigned long			InterruptNormPending53		: 1;
			volatile unsigned long			InterruptNormPending54		: 1;
			volatile unsigned long			InterruptNormPending55		: 1;
			volatile unsigned long			InterruptNormPending56		: 1;
			volatile unsigned long			InterruptNormPending57		: 1;
			volatile unsigned long			InterruptNormPending58		: 1;
			volatile unsigned long			InterruptNormPending59		: 1;
			volatile unsigned long			InterruptNormPending60		: 1;
			volatile unsigned long			InterruptNormPending61		: 1;
			volatile unsigned long			InterruptNormPending62		: 1;
			volatile unsigned long			InterruptNormPending63		: 1;
		} b;
		volatile unsigned long f;
	} Nipndh;

/* Normal Interrupt Pending Register Low */
/* table 9-30 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptNormPending0		: 1;
			volatile unsigned long			InterruptNormPending1		: 1;
			volatile unsigned long			InterruptNormPending2		: 1;
			volatile unsigned long			InterruptNormPending3		: 1;
			volatile unsigned long			InterruptNormPending4		: 1;
			volatile unsigned long			InterruptNormPending5		: 1;
			volatile unsigned long			InterruptNormPending6		: 1;
			volatile unsigned long			InterruptNormPending7		: 1;
			volatile unsigned long			InterruptNormPending8		: 1;
			volatile unsigned long			InterruptNormPending9		: 1;
			volatile unsigned long			InterruptNormPending10		: 1;
			volatile unsigned long			InterruptNormPending11		: 1;
			volatile unsigned long			InterruptNormPending12		: 1;
			volatile unsigned long			InterruptNormPending13		: 1;
			volatile unsigned long			InterruptNormPending14		: 1;
			volatile unsigned long			InterruptNormPending15		: 1;
			volatile unsigned long			InterruptNormPending16		: 1;
			volatile unsigned long			InterruptNormPending17		: 1;
			volatile unsigned long			InterruptNormPending18		: 1;
			volatile unsigned long			InterruptNormPending19		: 1;
			volatile unsigned long			InterruptNormPending20		: 1;
			volatile unsigned long			InterruptNormPending21		: 1;
			volatile unsigned long			InterruptNormPending22		: 1;
			volatile unsigned long			InterruptNormPending23		: 1;
			volatile unsigned long			InterruptNormPending24		: 1;
			volatile unsigned long			InterruptNormPending25		: 1;
			volatile unsigned long			InterruptNormPending26		: 1;
			volatile unsigned long			InterruptNormPending27		: 1;
			volatile unsigned long			InterruptNormPending28		: 1;
			volatile unsigned long			InterruptNormPending29		: 1;
			volatile unsigned long			InterruptNormPending30		: 1;
			volatile unsigned long			InterruptNormPending31		: 1;
		} b;
		volatile unsigned long f;
	} Nipndl;


/* Fast Interrupt Pending Register High */
/* table 9-31 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptFastPending32		: 1;
			volatile unsigned long			InterruptFastPending33		: 1;
			volatile unsigned long			InterruptFastPending34		: 1;
			volatile unsigned long			InterruptFastPending35		: 1;
			volatile unsigned long			InterruptFastPending36		: 1;
			volatile unsigned long			InterruptFastPending37		: 1;
			volatile unsigned long			InterruptFastPending38		: 1;
			volatile unsigned long			InterruptFastPending39		: 1;
			volatile unsigned long			InterruptFastPending40		: 1;
			volatile unsigned long			InterruptFastPending41		: 1;
			volatile unsigned long			InterruptFastPending42		: 1;
			volatile unsigned long			InterruptFastPending43		: 1;
			volatile unsigned long			InterruptFastPending44		: 1;
			volatile unsigned long			InterruptFastPending45		: 1;
			volatile unsigned long			InterruptFastPending46		: 1;
			volatile unsigned long			InterruptFastPending47		: 1;
			volatile unsigned long			InterruptFastPending48		: 1;
			volatile unsigned long			InterruptFastPending49		: 1;
			volatile unsigned long			InterruptFastPending50		: 1;
			volatile unsigned long			InterruptFastPending51		: 1;
			volatile unsigned long			InterruptFastPending52		: 1;
			volatile unsigned long			InterruptFastPending53		: 1;
			volatile unsigned long			InterruptFastPending54		: 1;
			volatile unsigned long			InterruptFastPending55		: 1;
			volatile unsigned long			InterruptFastPending56		: 1;
			volatile unsigned long			InterruptFastPending57		: 1;
			volatile unsigned long			InterruptFastPending58		: 1;
			volatile unsigned long			InterruptFastPending59		: 1;
			volatile unsigned long			InterruptFastPending60		: 1;
			volatile unsigned long			InterruptFastPending61		: 1;
			volatile unsigned long			InterruptFastPending62		: 1;
			volatile unsigned long			InterruptFastPending63		: 1;
		} b;
		volatile unsigned long f;
	} Fipndh;

/* Normal Interrupt Pending Register Low */
/* table 9-32 */
	union
	{
		struct
		{
			volatile unsigned long			InterruptFastPending0		: 1;
			volatile unsigned long			InterruptFastPending1		: 1;
			volatile unsigned long			InterruptFastPending2		: 1;
			volatile unsigned long			InterruptFastPending3		: 1;
			volatile unsigned long			InterruptFastPending4		: 1;
			volatile unsigned long			InterruptFastPending5		: 1;
			volatile unsigned long			InterruptFastPending6		: 1;
			volatile unsigned long			InterruptFastPending7		: 1;
			volatile unsigned long			InterruptFastPending8		: 1;
			volatile unsigned long			InterruptFastPending9		: 1;
			volatile unsigned long			InterruptFastPending10		: 1;
			volatile unsigned long			InterruptFastPending11		: 1;
			volatile unsigned long			InterruptFastPending12		: 1;
			volatile unsigned long			InterruptFastPending13		: 1;
			volatile unsigned long			InterruptFastPending14		: 1;
			volatile unsigned long			InterruptFastPending15		: 1;
			volatile unsigned long			InterruptFastPending16		: 1;
			volatile unsigned long			InterruptFastPending17		: 1;
			volatile unsigned long			InterruptFastPending18		: 1;
			volatile unsigned long			InterruptFastPending19		: 1;
			volatile unsigned long			InterruptFastPending20		: 1;
			volatile unsigned long			InterruptFastPending21		: 1;
			volatile unsigned long			InterruptFastPending22		: 1;
			volatile unsigned long			InterruptFastPending23		: 1;
			volatile unsigned long			InterruptFastPending24		: 1;
			volatile unsigned long			InterruptFastPending25		: 1;
			volatile unsigned long			InterruptFastPending26		: 1;
			volatile unsigned long			InterruptFastPending27		: 1;
			volatile unsigned long			InterruptFastPending28		: 1;
			volatile unsigned long			InterruptFastPending29		: 1;
			volatile unsigned long			InterruptFastPending30		: 1;
			volatile unsigned long			InterruptFastPending31		: 1;
		} b;
		volatile unsigned long f;
	} Fipndl;

}	AITC_T;

#define pAitc	((AITC_T *)	0x00223000)

/*
 Removed the following defines from the code to avoid potential problems. Some of them
 (number 32 and above are off by one!). They are very old and are not used anymore (Never have been?). To remove them,
 is for your safety against the wrong values.

 Use the "enum vector_number_t" ,further down below, to access interrupts and vectors.
 See the function DisableInterruptNumber() as an example.
 If you really want to access the registers of the AITC directly with masks, think carefully and use this
 example to create a mask (keep information at a single point):
			 doSomething(1<<VEC_RTC_INT);

 Dieter Fauth, 2006.12.04
 */

// Spd added because it would seem useful to know names of the 64 interrupt sources
// give to John for inclusion
// The table makes me real nervous because it addresses interrupt sources 0-63 in a table of 1-26 sources
// my assumption is bit 0 and bit 63 are not shown and reserved something... because the table approach (rows are the same)
// implies a similarity between 1-32, 2-33, etc...
// But I wonder if things aren't all off by one or something... anyway, we will very soon find out!!
// I will ease fixing this by relative shifting below
//Interrupt Assignment L int (0-31 interrupt sources
//#define   CSI_INT             BIT6      // Bit # in the spec says 6, but they count from 1, how annoying is that!!
//#define   DSPA_MAC_INTR       BIT7 //(CSI_INT<<1)
//#define   DSPA_INT            BIT8 //(DSPA_MAC_INTR<<1)
//#define   COMP_INT            BIT9 //(DSPA_INT<<1)
//#define   MSHC_XINT           BIT10 //(COMP_INT<<1)
//#define   GPIO_INT_PORTA      BIT11//(MSHC_XINT<<1)
//#define   GPIO_INT_PORTB      BIT12//(GPIO_INT_PORTA<<1)
//#define   GPIO_INT_PORTC      BIT13//(GPIO_INT_PORTB<<1)
//#define   LDCD_INT            BIT14//(GPIO_INT_PORTC<<1)
//#define   SIM_IRQ             BIT15//(LDCD_INT<<1)
//#define   SIM_DATA            BIT16//(SIM_IRQ<<1)
//#define   RTC_IRQ             BIT17//(SIM_DATA<<1)
//#define   RTC_SAMIRQ          BIT18//(RTC_IRQ<<1)
//#define   UART2_MINT_PFERR    BIT19//(RTC_SAMIRQ<<1)
//#define   UART2_MINT_RTS      BIT20//(UART2_MINT_PFERR<<1)
//#define   UART2_MINT_DTR      BIT21//(UART2_MINT_RTS<<1)
//#define   UART2_MINT_UARTC    BIT22//(UART2_MINT_DTR<<1)
//#define   UART2_MINT_TX       BIT23//(UART2_MINT_UARTC<<1)
//#define   nUART2_MINT_RX      BIT24//(UART2_MINT_TX<<1)
//#define   UART1_MINT_PFERR    BIT25//(nUART2_MINT_RX<<1)
//#define   UART1_MINT_RTS      BIT26//(UART1_MINT_PFERR<<1)
//#define   UART1_MINT_DTR      BIT27//(UART1_MINT_RTS<<1)
//#define   UART1_MINT_UARTC    BIT28//(UART1_MINT_DTR<<1)
//#define   nUART1_MINT_TX      BIT29//(UART1_MINT_UARTC<<1)
//#define   UART1_MINT_RX       BIT30
//#define   VOICE_DAC_INT       BIT31//(nUART1_MINT_TX<<1)
// with assumption above, this is the end of the low word assignments

//Interrupt Assignment H int (32-63)
// these associate with H int control, enable, etc...
// CAUTION: These values are off by one!!!! See above for a better way.
//#define   VOICE_ADC_INT       BIT1      // bit 32???
//#define   PEN_DATA_INT        BIT2//(VOICE_ADC_INT<<1)
//#define   PWM_INT             BIT3//(PEN_DATA_INT<<1)
//#define   SDHC_IRQ            BIT4//(PWM_INT<<1)
//// bit 5,6, and 7 are RSV
//#define   I2C_INT             BIT8//(SDHC_IRQ<<4)
//// bit 9 is RSV
//#define   CSPI_INT            BIT10//(I2C_INT<<2)
//#define   SSI_TX_INT          BIT11//(CSPI_INT<<1)
//#define   SSI_TX_ERR_INT      BIT12//(SSI_TX_INT<<1)
//#define   SSI_RX_INT          BIT13//(SSI_TX_ERR_INT<<1)
//#define   SSI_RX_ERR_INT      BIT14//(SSI_RX_INT<<1)
//#define   TOUCH_INT           BIT15//(SSI_RX_ERR_INT<<1)
//#define   nUSBD_INT0          BIT16//(TOUCH_INT<<1)
//#define   nUSBD_INT1          BIT17//(nUSBD_INT0<<1)
//#define   nUSBD_INT2          BIT18//(nUSBD_INT1<<1)
//#define   nUSBD_INT3          BIT19//(nUSBD_INT2<<1)
//#define   nUSBD_INT4          BIT20//(nUSBD_INT3<<1)
//#define   nUSBD_INT5          BIT21//(nUSBD_INT4<<1)
//#define   nUSBD_INT6          BIT22//(nUSBD_INT5<<1)
//// bit 23 is reserved
//#define   BTSYS               BIT24//(nUSBD_INT6<<2)
//#define   BTTIM               BIT25//(BTSYS<<1)
//#define   BTWUI               BIT26//(BTTIM<<1)
//#define   TIMER2_INT          BIT27//(BTWUI<<1)
//#define   TIMER1_INT          BIT28//(TIMER2_INT<<1)
//#define   DMA_ERR             BIT29//(TIMER1_INT<<1)
//#define   DMA_INT             BIT30//(DMA_ERR<<1)
//#define   GPIO_INT_PORTD      BIT31//(DMA_INT<<1)
// CAUTION: The above values are off by one!!!! See above for a better way.
// End of Spd add

// Vector numbers used be the Setvector and other functions/hardware
// I copied the tables from the "MC9328MX1RM/D Rev. 0, 04/2002" and
// used a MEW macro to bring it into it'scurrent form.
enum vector_number_t
{
	VEC_Unused0	 			= 0 ,
	VEC_Unused1 			= 1 ,
	VEC_Unused2 			= 2 ,
	VEC_Unused3 			= 3 ,
	VEC_Unused4	 			= 4 ,
	VEC_Unused5 			= 5 ,
	VEC_CSI_INT 			= 6 ,
	VEC_MMA_MAC_INT		= 7 ,
	VEC_MMA_INT 			= 8 ,
	VEC_COMP_INT			= 9 ,
	VEC_MSIRQ 				= 10,
	VEC_GPIO_INT_PORTA	= 11,
	VEC_GPIO_INT_PORTB	= 12 ,
	VEC_GPIO_INT_PORTC	= 13 ,
	VEC_LCDC_INT			= 14 ,
	VEC_SIM_IRQ 			= 15 ,
	VEC_SIM_DATA			= 16 ,
	VEC_RTC_INT 			= 17 ,
	VEC_RTC_SAM_INT		= 18 ,
	VEC_UART2_MINT_PFERR	= 19 ,
	VEC_UART2_MINT_RTS	= 20 ,
	VEC_UART2_MINT_DTR	= 21 ,
	VEC_UART2_MINT_UARTC	= 22 ,
	VEC_UART2_MINT_TX		= 23 ,
	VEC_UART2_MINT_RX		= 24 ,
	VEC_UART1_MINT_PFERR	= 25 ,
	VEC_UART1_MINT_RTS	= 26 ,
	VEC_UART1_MINT_DTR	= 27 ,
	VEC_UART1_MINT_UARTC	= 28 ,
	VEC_UART1_MINT_TX		= 29 ,
	VEC_UART1_MINT_RX		= 30 ,
	VEC_VOICE_DAC_INT		= 31 ,
	VEC_VOICE_ADC_INT		= 32 ,
	VEC_PEN_DATA_INT		= 33 ,
	VEC_PWM_INT				= 34 ,
	VEC_MMC_IRQ				= 35 ,
	VEC_Unused36			= 36 ,
	VEC_Unused37			= 37 ,
	VEC_Unused38			= 38 ,
	VEC_I2C_INT				= 39 ,
	VEC_SPI2_INT			= 40 ,
	VEC_SPI1_INT			= 41 ,
	VEC_SSI_TX_INT			= 42 ,
	VEC_SSI_TX_ERR_INT	= 43 ,
	VEC_SSI_RX_INT			= 44 ,
	VEC_SSI_RX_ERR_INT	= 45 ,
	VEC_TOUCH_INT			= 46 ,
	VEC_USBD_INT0			= 47 ,
	VEC_USBD_INT1			= 48 ,
	VEC_USBD_INT2			= 49 ,
	VEC_USBD_INT3			= 50 ,
	VEC_USBD_INT4			= 51 ,
	VEC_USBD_INT5			= 52 ,
	VEC_USBD_INT6			= 53 ,
	VEC_Unused54			= 54 ,
	VEC_BTSYS				= 55 ,
	VEC_BTTIM				= 56 ,
	VEC_BTWUI				= 57 ,
	VEC_TIMER2_INT			= 58 ,
	VEC_TIMER1_INT			= 59 ,
	VEC_DMA_ERR				= 60 ,
	VEC_DMA_INT				= 61 ,
	VEC_GPIO_INT_PORTD	= 62 ,
	VEC_WDT_INT 			= 63
};
/* ==============================================================================
	Phase-Locked Loop and Clock Controller
   ============================================================================== */

typedef struct Pll_t
{
/* Clock Source Control Register */
/* table 11-5 */

	union
	{
	  	struct
	  	{
  			volatile unsigned long			MPEN  			: 1;
  			volatile unsigned long			UPEN	 			: 1;
	  		volatile unsigned long			unused2 			: 1;
  			volatile unsigned long			unused3 			: 1;
  			volatile unsigned long			unused4 	 		: 1;
	  		volatile unsigned long			unused5	 		: 1;
  			volatile unsigned long			unused6 			: 1;
  			volatile unsigned long			unused7 			: 1;
	  		volatile unsigned long			unused8 			: 1;
  			volatile unsigned long			unused9 	 		: 1;
  			volatile unsigned long			BCLK_DIV			: 4;
  			volatile unsigned long			unused14			: 1;
	  		volatile unsigned long			PRESC				: 1;
  			volatile unsigned long			USB_SEL			: 1;
	  		volatile unsigned long			OSC_ENAB			: 1;
  			volatile unsigned long			CLK16_SEL		: 1;
  			volatile unsigned long			unused19			: 1;
	  		volatile unsigned long			unused20			: 1;
  			volatile unsigned long			MPLL_RESTART	: 1;
  			volatile unsigned long			SPLL_RESTART	: 1;
			volatile unsigned long			unused23			: 1;
  			volatile unsigned long			SD_CNT			: 2;
  			volatile unsigned long			USB_DIV			: 3;
	  		volatile unsigned long			CLKOSEL			: 3;
	  	} b;
  		volatile unsigned long f;
	}  Cscr;

/* MCU PLL Control Register 0 */
/* table 11-9 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			MFN	 			: 10;
  			volatile unsigned long			MFI  	 			: 4;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			MFD	 	 		: 10;
	  		volatile unsigned long			PD 		 		: 4;
  			volatile unsigned long			unused30			: 1;
  			volatile unsigned long			unused31			: 1;
	  	} b;
  		volatile unsigned long f;
	}  Mpctl0;

/* MCU PLL and System Clock Control Register 1 */
/* table 11-10 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			PD		  			: 2;
  			volatile unsigned long			unused2 			: 1;
	  		volatile unsigned long			unused3 			: 1;
  			volatile unsigned long			unused4 			: 1;
  			volatile unsigned long			MPEN	 	 		: 1;
	  		volatile unsigned long			BMRO 		 		: 1;
  			volatile unsigned long			unused7 			: 1;
  			volatile unsigned long			unused8			: 1;
	  		volatile unsigned long			unused9			: 1;
  			volatile unsigned long			unused10	 		: 1;
  			volatile unsigned long			unused11			: 1;
  			volatile unsigned long			unused12			: 1;
	  		volatile unsigned long			unused13			: 1;
  			volatile unsigned long			unused14			: 1;
	  		volatile unsigned long			unused15			: 1;
  			volatile unsigned long			unused16			: 1;
  			volatile unsigned long			unused17			: 1;
	  		volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
	  		volatile unsigned long			unused20			: 1;
  			volatile unsigned long			unused21			: 1;
  			volatile unsigned long			unused22			: 1;
	  		volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
  			volatile unsigned long			unused27			: 1;
  			volatile unsigned long			unused28			: 1;
	  		volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
  			volatile unsigned long			unused31			: 1;
	  	} b;
  		volatile unsigned long f;
	}  Mpctl1;

/* USB PLL Control Register 0 */
/* table 11-12 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			MFN	 			: 10;
  			volatile unsigned long			MFI  	 			: 4;
	  		volatile unsigned long			unused14			: 1;
  			volatile unsigned long			unused15			: 1;
  			volatile unsigned long			MFD	 	 		: 10;
	  		volatile unsigned long			PD 		 		: 4;
  			volatile unsigned long			unused30			: 1;
  			volatile unsigned long			unused31			: 1;
	  	} b;
  		volatile unsigned long f;
	}  Spctl0;


/* USB PLL Control Register 1 */
/* table 11-13 */
	union
	{
	  	struct
	  	{
  			volatile unsigned long			unused0			: 1;
  			volatile unsigned long			unused1 			: 1;
	  		volatile unsigned long			unused2			: 1;
  			volatile unsigned long			unused3			: 1;
  			volatile unsigned long			unused4 	 		: 1;
	  		volatile unsigned long			PEN		 		: 1;
  			volatile unsigned long			BMRO	 			: 1;
  			volatile unsigned long			unused7			: 1;
	  		volatile unsigned long			unused8			: 1;
  			volatile unsigned long			unused9 	 		: 1;
  			volatile unsigned long			unused10			: 1;
  			volatile unsigned long			unused11			: 1;
	  		volatile unsigned long			unused12			: 1;
  			volatile unsigned long			unused13			: 1;
	  		volatile unsigned long			unused14			: 1;
			volatile unsigned long			_LF				: 1;
			volatile unsigned long			unused16			: 1;
  			volatile unsigned long			unused17			: 1;
	  		volatile unsigned long			unused18			: 1;
  			volatile unsigned long			unused19			: 1;
	  		volatile unsigned long			unused20			: 1;
  			volatile unsigned long			unused21			: 1;
  			volatile unsigned long			unused22			: 1;
	  		volatile unsigned long			unused23			: 1;
  			volatile unsigned long			unused24			: 1;
  			volatile unsigned long			unused25			: 1;
	  		volatile unsigned long			unused26			: 1;
  			volatile unsigned long			unused27			: 1;
  			volatile unsigned long			unused28			: 1;
	  		volatile unsigned long			unused29			: 1;
  			volatile unsigned long			unused30			: 1;
  			volatile unsigned long			unused31			: 1;
	  	} b;
  		volatile unsigned long f;
	}  Spctl1;
} PLL_T;



typedef union
{
/* Peripheral Clock Divider Register */
/* table 11-7 */
  	struct
  	{
  		volatile unsigned long			PCLK_DIV1		: 4;
  		volatile unsigned long			PCLK_DIV2		: 4;
  		volatile unsigned long			unused8			: 1;
  		volatile unsigned long			unused9			: 1;
  		volatile unsigned long			unused10	 		: 1;
  		volatile unsigned long			unused11	 		: 1;
  		volatile unsigned long			unused12			: 1;
  		volatile unsigned long			unused13			: 1;
  		volatile unsigned long			unused14			: 1;
  		volatile unsigned long			unused15	 		: 1;
  		volatile unsigned long			PCLK_DIV3		: 7;
  		volatile unsigned long			unused23			: 1;
  		volatile unsigned long			unused24			: 1;
  		volatile unsigned long			unused25			: 1;
  		volatile unsigned long			unused26			: 1;
  		volatile unsigned long			unused27			: 1;
  		volatile unsigned long			unused28			: 1;
  		volatile unsigned long			unused29			: 1;
  		volatile unsigned long			unused30			: 1;
  		volatile unsigned long			unused31			: 1;
  	} b;
  	volatile unsigned long f;
} PCDR_T;


#define pPLL   ((PLL_T *)(0x0021B000))
#define pPCDR  ((PCDR_T *)(0x0021B020))


/* ==============================================================================
	LCD controller
	============================================================================== */

 typedef union
{
/* PWM Contrast Control Register */
/* table 18-17 */
	struct
	{
        	volatile unsigned long  	   PW		        	: 8;
			volatile unsigned long			CC_EN				: 1;
			volatile unsigned long			SCR				: 2;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned short f;
 } PWMR_T;
#define pPWMR         ((PWMR_T *) 0x0020502C)
#define PWMR          (*pPWMR)

 typedef union
{
/* Refresh Mode Control Register */
/* table 18-18 */
	struct
	{
        	volatile unsigned long  	   SELF_REF      	: 1;
			volatile unsigned long			LCDC_EN			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned short f;
 } RMCR_T;
#define pRMCR         ((RMCR_T *) 0x00205034)
#define RMCR          (*pRMCR)

 typedef union
{
/* Panel Configuration Register */
/* table 18-9 */
	struct
	{
        	volatile unsigned long  	   PCD	      	: 5;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned short f;
 } PCR_T;
#define pPCR         ((PCR_T *) 0x00205018)
#define PCR          (*pPCR)


/* ==============================================================================
	PWM controller
	============================================================================== */

typedef struct Pwm_T
{
/* PWM Control Register */
/* table 21-3 */
	union
	{
      struct
		{
			volatile unsigned long			CLKSEL		: 2;
			volatile unsigned long			REPEAT		: 2;
			volatile unsigned long			ENAB			: 1;
			volatile unsigned long			FIFOAV		: 1;
			volatile unsigned long			IRQENAB		: 1;
			volatile unsigned long			IRQ			: 1;
			volatile unsigned long			PRESCALER	: 7;
			volatile unsigned long			CLKSRC		: 1;
			volatile unsigned long			SWR			: 1;
			volatile unsigned long			unused17		: 1;
			volatile unsigned long			unused18		: 1;
			volatile unsigned long			unused19		: 1;
			volatile unsigned long			unused20		: 1;
			volatile unsigned long			unused21		: 1;
			volatile unsigned long			unused22		: 1;
			volatile unsigned long			unused23		: 1;
			volatile unsigned long			unused24		: 1;
			volatile unsigned long			unused25		: 1;
			volatile unsigned long			unused26		: 1;
			volatile unsigned long			unused27		: 1;
			volatile unsigned long			unused28		: 1;
			volatile unsigned long			unused29		: 1;
			volatile unsigned long			unused30		: 1;
			volatile unsigned long			unused31		: 1;
  		} b;
  		volatile unsigned long f;
	} Pwmc;

/* PWM Sample Register */
/* table 21-4 */
	union
	{
      struct
		{
			volatile unsigned long			SAMPLE		: 16;
			volatile unsigned long			unused16	  	: 1;
			volatile unsigned long			unused17	  	: 1;
			volatile unsigned long			unused18	  	: 1;
			volatile unsigned long			unused19	  	: 1;
			volatile unsigned long			unused20	  	: 1;
			volatile unsigned long			unused21	  	: 1;
			volatile unsigned long			unused22	  	: 1;
			volatile unsigned long			unused23	  	: 1;
			volatile unsigned long			unused24	  	: 1;
			volatile unsigned long			unused25	  	: 1;
			volatile unsigned long			unused26	  	: 1;
			volatile unsigned long			unused27	  	: 1;
			volatile unsigned long			unused28	  	: 1;
			volatile unsigned long			unused29	  	: 1;
			volatile unsigned long			unused30	  	: 1;
			volatile unsigned long			unused31	  	: 1;
  		} b;
  		volatile unsigned long f;
	} Pwms;

/* PWM Period Register */
/* table 21-5 */
	union
	{
      struct
		{
			volatile unsigned long			PERIOD		: 16;
			volatile unsigned long			unused16	  	: 1;
			volatile unsigned long			unused17	  	: 1;
			volatile unsigned long			unused18	  	: 1;
			volatile unsigned long			unused19	  	: 1;
			volatile unsigned long			unused20	  	: 1;
			volatile unsigned long			unused21	  	: 1;
			volatile unsigned long			unused22	  	: 1;
			volatile unsigned long			unused23	  	: 1;
			volatile unsigned long			unused24	  	: 1;
			volatile unsigned long			unused25	  	: 1;
			volatile unsigned long			unused26	  	: 1;
			volatile unsigned long			unused27	  	: 1;
			volatile unsigned long			unused28	  	: 1;
			volatile unsigned long			unused29	  	: 1;
			volatile unsigned long			unused30	  	: 1;
			volatile unsigned long			unused31	  	: 1;
  		} b;
  		volatile unsigned long f;
	} Pwmp;



/* PWM Counter Register */
/* table 21-6 */
	union
	{
      struct
		{
			volatile unsigned long			COUNT			: 1;
			volatile unsigned long			unused16	  	: 1;
			volatile unsigned long			unused17	  	: 1;
			volatile unsigned long			unused18	  	: 1;
			volatile unsigned long			unused19	  	: 1;
			volatile unsigned long			unused20	  	: 1;
			volatile unsigned long			unused21	  	: 1;
			volatile unsigned long			unused22	  	: 1;
			volatile unsigned long			unused23	  	: 1;
			volatile unsigned long			unused24	  	: 1;
			volatile unsigned long			unused25	  	: 1;
			volatile unsigned long			unused26	  	: 1;
			volatile unsigned long			unused27	  	: 1;
			volatile unsigned long			unused28	  	: 1;
			volatile unsigned long			unused29	  	: 1;
			volatile unsigned long			unused30	  	: 1;
			volatile unsigned long			unused31	  	: 1;
  		} b;
  		volatile unsigned long f;
	} Pwmcnt;
} PWM_T;


#define pPWM	((PWM_T *) 0x00208000)


/* ==============================================================================
	Synchronous Serial Interface SSI
	============================================================================== */
 typedef union
{
	struct
	{
        	volatile unsigned long  	   unused0        : 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			SSI_EN			: 1;
			volatile unsigned long			TE					: 1;
			volatile unsigned long			RE					: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } SCSR_T;
#define pSCSR         ((SCSR_T *) 0x00218008)
#define SCSR          (*pSCSR)

 typedef union
{
	struct
	{
        	volatile unsigned long  	   _PM		      : 8;
			volatile unsigned long			DC					: 5;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			PSR				: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } STCCR_T;
#define pSTCCR         ((STCCR_T *) 0x00218014)
#define STCCR          (*pSTCCR)

#define pSRCCR			((STCCR_T*) 0x00218018)
#define SRCCR			(*pSRCCR)

 typedef union
{
	struct
	{
        	volatile unsigned long  	   unused0        : 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			TFDIR				: 1;
			volatile unsigned long			TXDIR				: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } STCR_T;
#define pSTCR         ((STCR_T *) 0x0021800C)
#define STCR          (*pSTCR)

typedef union
{
	struct
	{
        	volatile unsigned long  	   HighLow        : 16;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } STX_T;
#define pSTX         ((STX_T *) 0x00218000)
#define STX_REG          (*pSTX)

typedef union
{
	struct
	{
        	volatile unsigned long  	   unused0        : 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4				: 1;
			volatile unsigned long			unused5				: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } SRCR_T;
#define pSRCR         ((STCR_T *) 0x00218010)
#define SRCR          (*pSRCR)

typedef union
{
	struct
	{
        	volatile unsigned long  	   TFWM        : 4;
			volatile unsigned long			unused4				: 1;
			volatile unsigned long			unused5				: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			TFCNT			: 4;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } SFCSR_T;
#define pSFCSR         ((SFCSR_T *) 0x00218020)
#define SFCSR          (*pSFCSR)

typedef union
{
	struct
	{
        	volatile unsigned long  	   unused0        : 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4				: 1;
			volatile unsigned long			unused5				: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
 } SOR_T;
#define pSOR         ((SOR_T *) 0x00218028)
#define SOR          (*pSOR)

/* ==============================================================================
	Realtime Clock Controller
	============================================================================== */
typedef struct RealTimeClock_t
{
/* RTC Hours and Minutes Counter Register */
/* table 22-4 */
	union
	{
      struct
		{
			volatile unsigned long			MINUTES			: 6;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			HOURS				: 5;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Hourmin;


/* RTC Seconds Counter Register */
/* table 22- 5*/
	union
	{
      struct
		{
			volatile unsigned long			SECONDS			: 6;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Seconds_s;                         // to be able to compile with H8 else remove _s ??


/* RTC Hours and Minutes Alarm Register */
/* table 22-7 */
	union
	{
      struct
		{
			volatile unsigned long			MINUTES			: 6;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			HOURS				: 5;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Alrm_Hm;


/* RTC Seconds Alarm Register */
/* table 22-8 */
	union
	{
      struct
		{
			volatile unsigned long			SECONDS			: 6;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Alrm_Sec;

/* RTC Control Register */
/* table 22-9 */
	union
	{
      struct
		{
			volatile unsigned long			SWR				: 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			XTL0				: 1;
			volatile unsigned long			XTL1				: 1;
			volatile unsigned long			ENAB				: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Rcctl;


/* RTC Interrupt Status Register */
/* table 22-10 */
	union
	{
      struct
		{
			volatile unsigned long			SW					: 1;
			volatile unsigned long			MIN				: 1;
			volatile unsigned long			ALM				: 1;
			volatile unsigned long			DAY				: 1;
			volatile unsigned long			ONEHZ				: 1;
			volatile unsigned long			HR					: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			TWOHZ				: 1;
			volatile unsigned long			SAM0				: 1;
			volatile unsigned long			SAM1				: 1;
			volatile unsigned long			SAM2				: 1;
			volatile unsigned long			SAM3				: 1;
			volatile unsigned long			SAM4				: 1;
			volatile unsigned long			SAM5				: 1;
			volatile unsigned long			SAM6				: 1;
			volatile unsigned long			SAM7				: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Rtcisr;

/* RTC Interrupt Enable Register */
/* table 22-11 */
	union
	{
      struct
		{
			volatile unsigned long			SW					: 1;
			volatile unsigned long			MIN				: 1;
			volatile unsigned long			ALM				: 1;
			volatile unsigned long			DAY				: 1;
			volatile unsigned long			ONEHZ				: 1;
			volatile unsigned long			HR					: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			TWOHZ				: 1;
			volatile unsigned long			SAM0				: 1;
			volatile unsigned long			SAM1				: 1;
			volatile unsigned long			SAM2				: 1;
			volatile unsigned long			SAM3				: 1;
			volatile unsigned long			SAM4				: 1;
			volatile unsigned long			SAM5				: 1;
			volatile unsigned long			SAM6				: 1;
			volatile unsigned long			SAM7				: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Rtcienr;


/* Stopwatch Minutes Register */
/* table 22-12 */
	union
	{
      struct
		{
			volatile unsigned long			CNT				: 6;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Stpwch;

/* RTC Days Counter Register */
/* table 22-3 */
	union
	{
      struct
		{
			volatile unsigned long			DAYS				: 9;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Dayr;

/* RTC Day Alarm Register */
/* table 22-6 */
	union
	{
      struct
		{
			volatile unsigned long			DAYSAL			: 9;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
  		} b;
  		volatile unsigned long f;
	} Dayalarm;

} REALTIMECLOCK_T;

#define pRTC	((REALTIMECLOCK_T *) 0x00204000)


/* ==============================================================================
	Reset register definitions
   ============================================================================== */

/* Reset Source Register */
/* table 6-2 */

typedef union Rsr_t
{
	struct
	{
		volatile unsigned long			EXR				: 1;
		volatile unsigned long			WDR			  	: 1;
		volatile unsigned long			unused2 		 	: 1;
		volatile unsigned long			unused3 		 	: 1;
		volatile unsigned long			unused4 		 	: 1;
		volatile unsigned long			unused5 		 	: 1;
		volatile unsigned long			unused6 		 	: 1;
		volatile unsigned long			unused7 		 	: 1;
		volatile unsigned long			unused8 			: 1;
		volatile unsigned long			unused9 		  	: 1;
		volatile unsigned long			unused10			: 1;
		volatile unsigned long			unused11			: 1;
		volatile unsigned long			unused12			: 1;
		volatile unsigned long			unused13			: 1;
		volatile unsigned long			unused14			: 1;
		volatile unsigned long			unused15			: 1;
		volatile unsigned long			unused16			: 1;
		volatile unsigned long			unused17			: 1;
		volatile unsigned long			unused18			: 1;
		volatile unsigned long			unused19			: 1;
		volatile unsigned long			unused20			: 1;
		volatile unsigned long			unused21			: 1;
		volatile unsigned long			unused22			: 1;
		volatile unsigned long			unused23			: 1;
		volatile unsigned long			unused24			: 1;
		volatile unsigned long			unused25			: 1;
		volatile unsigned long			unused26			: 1;
		volatile unsigned long			unused27			: 1;
		volatile unsigned long			unused28			: 1;
		volatile unsigned long			unused29			: 1;
		volatile unsigned long			unused30			: 1;
		volatile unsigned long			unused31			: 1;
	} b;
	volatile unsigned long f;
} RSR_T;


#define pResetReg	((RSR_T *) 0x0021B800)



/* ==============================================================================
	Sensor interface register definitions
   ============================================================================== */

/* CSI Control Register 1 */
/* table 30-5 */

typedef struct Sensor_T
{
	union
	{
      struct
		{
			volatile unsigned long			EN			  		: 1;
			volatile unsigned long			REDGE		  		: 1;
			volatile unsigned long			INV_PCLK	  		: 1;
			volatile unsigned long			INV_DATA	  		: 1;
			volatile unsigned long			GCLK_MODE  		: 1;
			volatile unsigned long			CLR_RXFIFO 		: 1;
			volatile unsigned long			CLR_STATFIFO	: 1;
			volatile unsigned long			BIG_ENDIAN_S	: 1;
			volatile unsigned long			FCC		  		: 1;
			volatile unsigned long			MCLKEN	  		: 1;
			volatile unsigned long			unused10	  		: 1;
			volatile unsigned long			unused11	  		: 1;
			volatile unsigned long			MCLKDIV	  		: 4;
			volatile unsigned long			SOF_INTEN  		: 1;
			volatile unsigned long			SOF_POL	  		: 1;
			volatile unsigned long			RXFF_INTEN	 	: 1;
			volatile unsigned long			RXFF_LEVEL 		: 2;
			volatile unsigned long			STATFF_INTEN	: 1;
			volatile unsigned long			STAFF_LEVEL		: 2;
			volatile unsigned long			RF_OR_INTEN		: 1;
			volatile unsigned long			SF_OR_INTEN		: 1;
			volatile unsigned long			unused26		  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;

  		volatile unsigned long f;
	} Csicr1;

/* CSI Control Register 2 */
/* table 30-6 */

	union
	{
      struct
		{
			volatile unsigned long			HSC		  		: 8;
			volatile unsigned long			VSC			 	: 8;
			volatile unsigned long			LVRM		 		: 3;
			volatile unsigned long			BTS				: 2;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			SCE				: 1;
			volatile unsigned long			AFS				: 2;
			volatile unsigned long			DRM			  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;

  		volatile unsigned long f;
	} Csicr2;


/* CSI Status Register 1 */
/* table 30-7 */

	union
	{
      struct
		{
			volatile unsigned long			DRDY		  		: 1;
			volatile unsigned long			unused1		  	: 1;
			volatile unsigned long			unused2		  	: 1;
			volatile unsigned long			unused3	  		: 1;
			volatile unsigned long			unused4	  		: 1;
			volatile unsigned long			unused5	  		: 1;
			volatile unsigned long			unused6	  		: 1;
			volatile unsigned long			unused7	  		: 1;
			volatile unsigned long			unused8	  		: 1;
			volatile unsigned long			unused9	  		: 1;
			volatile unsigned long			unused10		  	: 1;
			volatile unsigned long			unused11	  		: 1;
			volatile unsigned long			unused12	  		: 1;
			volatile unsigned long			unused13	  		: 1;
			volatile unsigned long			unused14	  		: 1;
			volatile unsigned long			unused15	  		: 1;
			volatile unsigned long			SOF_INT		 	: 1;
			volatile unsigned long			unused17		  	: 1;
			volatile unsigned long			RXFF_INT 		: 1;
			volatile unsigned long			unused19		  	: 1;
			volatile unsigned long			unused20	  		: 1;
			volatile unsigned long			STAFF_INT		: 1;
			volatile unsigned long			unused22		  	: 1;
			volatile unsigned long			unused23	  		: 1;
			volatile unsigned long			RFF_OR_INT		: 1;
			volatile unsigned long			SFF_OR_INT		: 1;
			volatile unsigned long			unused26		  	: 1;
			volatile unsigned long			unused27	  		: 1;
			volatile unsigned long			unused28	  		: 1;
			volatile unsigned long			unused29	  		: 1;
			volatile unsigned long			unused30	  		: 1;
			volatile unsigned long			unused31	  		: 1;
  		} b;

  		volatile unsigned long f;
	} Csisr;


/* CSI Statistic FIFO Register 1 */
/* table 30-8 */
	volatile unsigned long	Csistatr;

/* CSI RxFIFO Register 1 */
/* table 30-9 */

	union
	{
      struct
		{
			volatile unsigned long		byte4		  		: 8;
			volatile unsigned long		byte3			  	: 8;
			volatile unsigned long		byte2		  		: 8;
			volatile unsigned long		byte1		  		: 8;
  		} b;

  		volatile unsigned long f;
	} Csirxr;
} SENSOR_T;

#define	pCameraPort		((SENSOR_T *)		(0x00224000))



/* ==============================================================================
	Sdram memory controller
   ============================================================================== */

typedef struct Sdram_t
{

/* SDRAM Control Register */
/* table 23-6 */
	union
	{
      struct
		{
			volatile unsigned long			SRC				: 3;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			SRCD				: 2;
			volatile unsigned long			SRP				: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			SCL				: 2;
			volatile unsigned long			CI					: 2;
			volatile unsigned long			CLKST				: 2;
			volatile unsigned long			SREFR				: 2;
			volatile unsigned long			DSIZ				: 2;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			IAM				: 1;
			volatile unsigned long			COL				: 2;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			ROW				: 2;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			SP					: 1;
			volatile unsigned long			SMODE				: 3;
			volatile unsigned long			SDE				: 1;
  		} b;
  		volatile unsigned long f;
	} Sdctl0;


/* SDRAM Control Register */
/* table 23-6 */
	union
	{
      struct
		{
			volatile unsigned long			SRC				: 3;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			SRCD				: 2;
			volatile unsigned long			SRP				: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			SCL				: 2;
			volatile unsigned long			CI					: 2;
			volatile unsigned long			CLKST				: 2;
			volatile unsigned long			SREFR				: 2;
			volatile unsigned long			DSIZ				: 2;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			IAM				: 1;
			volatile unsigned long			COL				: 2;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			ROW				: 2;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			SP					: 1;
			volatile unsigned long			SMODE				: 3;
			volatile unsigned long			SDE				: 1;
  		} b;
  		volatile unsigned long f;
	} Sdctl1;

/* SDRAM Reset Register */
/* table 23-7 */
	union
	{
      struct
		{
			volatile unsigned long			unused0			: 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			RST1				: 1;
			volatile unsigned long			RST0				: 1;
  		} b;
  		volatile unsigned long f;
	} Sdrst;


/* Miscellaneous Register */
/* table 23-8 */
	union
	{
      struct
		{
			volatile unsigned long			RMA0				: 1;
			volatile unsigned long			unused1			: 1;
			volatile unsigned long			unused2			: 1;
			volatile unsigned long			unused3			: 1;
			volatile unsigned long			unused4			: 1;
			volatile unsigned long			unused5			: 1;
			volatile unsigned long			unused6			: 1;
			volatile unsigned long			unused7			: 1;
			volatile unsigned long			unused8			: 1;
			volatile unsigned long			unused9			: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			OMA				: 1;
  		} b;
  		volatile unsigned long f;
	} Miscellaneous;

} SDRAM_T;

#define pSdram	((SDRAM_T *) 	(0x00224000))


/* ==============================================================================
	system control register definitions
   ============================================================================== */

typedef struct System_control_T
{

/* Silicon ID Register */
/* table 7-2 */

	union
	{
		struct
		{
			volatile unsigned long			SID				: 16;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;

		volatile unsigned long f;
	} Sidr;


/* Function Multiplexing Control Register */
/* table 7-3 */
	union
	{
		struct
		{
			volatile unsigned long			SDCS0_SEL		: 1;
			volatile unsigned long			SDCS1_SEL	  	: 1;
			volatile unsigned long			EXT_BR_EN	 	: 1;
			volatile unsigned long			SSI_TXCLK_SEL 	: 1;
			volatile unsigned long			SSITXFS_SEL	 	: 1;
			volatile unsigned long			SSI_RXDAT_SEL 	: 1;
			volatile unsigned long			SSI_RXCLK_SEL 	: 1;
			volatile unsigned long			SSI_RXFS_SEL 	: 1;
			volatile unsigned long			unused8 			: 1;
			volatile unsigned long			unused9 		  	: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;

		volatile unsigned long f;
	} Fmcr;

/* Global Peripheral Control Register */
/* table 7-4 */
	union
	{
		struct
		{
			volatile unsigned long			CSI_PORT_ENAB	: 1;
			volatile unsigned long			MMA_PORT_ENAB 	: 1;
			volatile unsigned long			unused2		 	: 1;
			volatile unsigned long			unused3 			: 1;
			volatile unsigned long			DS_DATA			: 2;
			volatile unsigned long			DS_ADDR		 	: 2;
			volatile unsigned long			DS_CNTL		 	: 2;
			volatile unsigned long			DS_SLOW		 	: 2;
			volatile unsigned long			BTAENAB 			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
		} b;

		volatile unsigned long f;
	} Gpcr;

/* Global Clock Control Register */
/* table 8-1 **** added in newer manuals **** */
	union
	{
		struct
		{
			volatile unsigned long			USB0_CLK_EN		: 1;
			volatile unsigned long			MMA_CLK_EN 		: 1;
			volatile unsigned long			CSI_CLK_EN 		: 1;
			volatile unsigned long			DMA_CLK_EN		: 1;
			volatile unsigned long			BROM_CLK_EN		: 1;
			volatile unsigned long			unused5 			: 1;
			volatile unsigned long			unused6 			: 1;
			volatile unsigned long			unused7 			: 1;
			volatile unsigned long			unused8 			: 1;
			volatile unsigned long			unused9 		  	: 1;
			volatile unsigned long			unused10			: 1;
			volatile unsigned long			unused11			: 1;
			volatile unsigned long			unused12			: 1;
			volatile unsigned long			unused13			: 1;
			volatile unsigned long			unused14			: 1;
			volatile unsigned long			unused15			: 1;
			volatile unsigned long			unused16			: 1;
			volatile unsigned long			unused17			: 1;
			volatile unsigned long			unused18			: 1;
			volatile unsigned long			unused19			: 1;
			volatile unsigned long			unused20			: 1;
			volatile unsigned long			unused21			: 1;
			volatile unsigned long			unused22			: 1;
			volatile unsigned long			unused23			: 1;
			volatile unsigned long			unused24			: 1;
			volatile unsigned long			unused25			: 1;
			volatile unsigned long			unused26			: 1;
			volatile unsigned long			unused27			: 1;
			volatile unsigned long			unused28			: 1;
			volatile unsigned long			unused29			: 1;
			volatile unsigned long			unused30			: 1;
			volatile unsigned long			unused31			: 1;
			} b;

		volatile unsigned long f;
	} Gccr;


}	SYSTEM_CONTROL_T;

#define pSysCtrl	((SYSTEM_CONTROL_T*)	0x0021B804)




/* ==============================================================================
	UART 1 &2 port definitions
   ============================================================================== */

typedef union HalfWord_T
{
	struct
	{
		volatile unsigned long		WORD    	  			: 16;
		volatile unsigned long		unused	  			: 16;
	} b;
	volatile unsigned long f;
} HALFWORD_T;

typedef union Urxd_T
{
	struct
	{
		volatile unsigned long		 	RX_DATA		 		  	: 8;
		volatile unsigned long			Unused					: 2;
		volatile unsigned long		 	PRERR        	  		: 1;
		volatile unsigned long		 	BRK          		  	: 1;
		volatile unsigned long		 	FRMERR       		  	: 1;
		volatile unsigned long		 	OVRRUN       		  	: 1;
		volatile unsigned long		 	ERR          	   	: 1;
		volatile unsigned long		 	CHARRDY      	   	: 1;
		volatile unsigned long			unused31 	  	   	: 16;
	} b;
	volatile unsigned long f;
} URXD_T;

/* table 26-13 MX1 */
typedef union Byte_Word_Tx_T
{
	struct
	{
		volatile unsigned long		 	TX_DATA		 		  	: 8;
		volatile unsigned long			Unused					: 8;
		volatile unsigned long			unused31	   	   	: 16;
	} b;
	volatile unsigned long f;
} BYTE_WORD_TX_T;

typedef union Byte_Word_T
{
	struct
	{
		volatile unsigned long		 	BYTE 			 		  	: 8;
		volatile unsigned long			Unused					: 8;
		volatile unsigned long			unused31	   	   	: 16;
	} b;
	volatile unsigned long f;
} BYTE_WORD_T;



typedef struct Uart_T
{

/* table 26-12 MX1 */
     URXD_T                   Urxd [16];
/* table 26-13 MX1 */
     BYTE_WORD_TX_T           Utxd [16];

/* table 26-14 MX1 */
	union
	{
		struct
		{
			volatile unsigned long		UARTENAB   		  	: 1;
			volatile unsigned long		DOZE     		  	: 1;
			volatile unsigned long		UARTCLKENAB		  	: 1;
			volatile unsigned long		TDMAENAB   		  	: 1;
			volatile unsigned long		SNDBRK     		  	: 1;
			volatile unsigned long		RTSDENAB   	   	: 1;
			volatile unsigned long		TXMPTYENAB 	   	: 1;
			volatile unsigned long		IRENAB     	   	: 1;
			volatile unsigned long		RDMAENAB   	   	: 1;
			volatile unsigned long		RRDYENAB   	   	: 1;
			volatile unsigned long		ICD      	   	: 2;
			volatile unsigned long		IDENAB     	   	: 1;
			volatile unsigned long		TRDYENAB   	   	: 1;
			volatile unsigned long		ADBR     	   	: 1;
			volatile unsigned long		ADENAB     	   	: 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Ucr1;

/* table 26-15 MX1 */
	union
	{
		struct
		{
			volatile unsigned long		SRST     			: 1;
			volatile unsigned long		RXENAB   			: 1;
			volatile unsigned long		TXENAB   			: 1;
			volatile unsigned long		Unused3  			: 1;
			volatile unsigned long		RTSENAB  			: 1;
			volatile unsigned long		WS       			: 1;
			volatile unsigned long		STPB     			: 1;
			volatile unsigned long		PROE     			: 1;
			volatile unsigned long		PRENAB   			: 1;
			volatile unsigned long		RTEC					: 2;
			volatile unsigned long		ESCENAB  			: 1;
			volatile unsigned long		CTS      			: 1;
			volatile unsigned long		CTSC     			: 1;
			volatile unsigned long		IRTS     			: 1;
			volatile unsigned long		ESCI     			: 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Ucr2;

/* table 26-16 MX1 */
	union
	{
		struct
		{
			volatile unsigned long		BPEN         	  	: 1;
			volatile unsigned long		INVT         	  	: 1;
			volatile unsigned long		REF30        	  	: 1;
			volatile unsigned long		REF25        	  	: 1;
			volatile unsigned long		AWAKENAB     	  	: 1;
			volatile unsigned long		AIRINTENAB   	  	: 1;
			volatile unsigned long		RXDSENAB     	  	: 1;
			volatile unsigned long		TIMEOUTENAB  	  	: 1;
			volatile unsigned long		RI           	  	: 1;
			volatile unsigned long		DCD          	  	: 1;
			volatile unsigned long		DSR          	  	: 1;
			volatile unsigned long		FRAERRENAB   	  	: 1;
			volatile unsigned long		PARERRENAB   	  	: 1;
			volatile unsigned long		DTRENAB      	  	: 1;
			volatile unsigned long		DPEC		    	  	: 2;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Ucr3;

/* table 26-17 MX1 */
	union
	{
		struct
		{
			volatile unsigned long		DRENAB      	  	: 1;
			volatile unsigned long		ORENAB      	  	: 1;
			volatile unsigned long		BKENAB      	  	: 1;
			volatile unsigned long		TCENAB      	  	: 1;
			volatile unsigned long		Unused4     	  	: 1;
			volatile unsigned long		IRSC        	  	: 1;
			volatile unsigned long		REF16       	  	: 1;
			volatile unsigned long		WKENAB      	  	: 1;
			volatile unsigned long		ENIRI       	  	: 1;
			volatile unsigned long		INVR        	  	: 1;
			volatile unsigned long		CTSTL			  	  	: 6;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Ucr4;


/* table 26-18 MX1 */
/* fifo control register */
	union
	{
		struct
		{
			volatile unsigned long		RXTL		  			: 6;
			volatile unsigned long		unused7	  			: 1;
			volatile unsigned long		RFDIV0   			: 1;
			volatile unsigned long		RFDIV1   			: 1;
			volatile unsigned long		RFDIV2   			: 1;
			volatile unsigned long		TXTL		  			: 6;
			volatile unsigned long		unused31   	   	: 16;
		} b;
	} Ufcr;


/* table 26-19 */
/* Uart status register 1 */
	union
	{
		struct
		{
			volatile unsigned long		unused0    			: 1;
			volatile unsigned long		unused1    			: 1;
			volatile unsigned long		unused2    			: 1;
			volatile unsigned long		unused3    			: 1;
			volatile unsigned long		AWAKE      			: 1;
			volatile unsigned long		AIRINT     			: 1;
			volatile unsigned long		RXDS       			: 1;
			volatile unsigned long		TIMEOUT    			: 1;
			volatile unsigned long		unused8    			: 1;
			volatile unsigned long		RRDY       			: 1;
			volatile unsigned long		FRAMERR    			: 1;
			volatile unsigned long		ESCF       			: 1;
			volatile unsigned long		RTSD       			: 1;
			volatile unsigned long		TRDY       			: 1;
			volatile unsigned long		RTSS              : 1;
			volatile unsigned long		PARITYERR         : 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Usr1;

/* table 26-20 */
/* Uart status register 2 */
	union
	{
		struct
		{
			volatile unsigned long		RDR     	  			: 1;
			volatile unsigned long		ORE     	  			: 1;
			volatile unsigned long		BRCD    	  			: 1;
			volatile unsigned long		TXDC    	  			: 1;
			volatile unsigned long		RTSF    	  			: 1;
			volatile unsigned long		unused5 	  			: 1;
			volatile unsigned long		unused6 	  			: 1;
			volatile unsigned long		WAKE    	  			: 1;
			volatile unsigned long		IRINT   	  			: 1;
			volatile unsigned long		unused9 	  			: 1;
			volatile unsigned long		unused10	  			: 1;
			volatile unsigned long		unused11	  			: 1;
			volatile unsigned long		IDLE    	  			: 1;
			volatile unsigned long		DTRF    	  			: 1;
			volatile unsigned long		TXFE    	         : 1;
			volatile unsigned long		ADET    	         : 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Usr2;


/* table 26-21 */
/* Escape character registers */
	BYTE_WORD_T	Uesc;


/* table 26-22 */
/* Uart Eecape timer register 2 */
	union
	{
		struct
		{
			volatile unsigned long		TIM     	  			: 11;
			volatile unsigned long		unused12	  			: 1;
			volatile unsigned long		unused13	  			: 1;
			volatile unsigned long		unused14	  			: 1;
			volatile unsigned long		unused15	  			: 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned long f;
	} Utim;

/* table 26-23 */
/* Uart baud rate multiplier */
HALFWORD_T		Ubir;
/* table 26-24 */
/* Uart moidulator dominator register */
/* Updating this register using byte accesses is not recommended and undefined. */
HALFWORD_T		Ubmr;
/* table 26-25 */
/* Uart baud rate count register */
HALFWORD_T		Ubrc;

/* table 26-26 */
/* registers for autobaud detect */
HALFWORD_T		Bipr1;
HALFWORD_T		Bipr2;
HALFWORD_T		Bipr3;
HALFWORD_T		Bipr4;

/* table 26-27 */
/* UART BRM MOD Preset Register */
HALFWORD_T		Bmpr1;
HALFWORD_T		Bmpr2;
HALFWORD_T		Bmpr3;
HALFWORD_T		Bmpr4;


/* table 26-28 */
/* UART Test Register */
	union
	{
		struct
		{
			volatile unsigned long		SOFTRST  	  	  	: 1;
			volatile unsigned long		unused1  	  	  	: 1;
			volatile unsigned long		unused2  	  	  	: 1;
			volatile unsigned long		RXFULL   	  	  	: 1;
			volatile unsigned long		TXFULL   	  	  	: 1;
			volatile unsigned long		RXEMPTY  	  	  	: 1;
			volatile unsigned long		TXEMPTY  	  	  	: 1;
			volatile unsigned long		unused7  	  	  	: 1;
			volatile unsigned long		unused8  	  	  	: 1;
			volatile unsigned long		unused9  	  	  	: 1;
			volatile unsigned long		unused10 	  	  	: 1;
			volatile unsigned long		unused11 	  	  	: 1;
			volatile unsigned long		LOOP     	  	  	: 1;
			volatile unsigned long		FRCPERR  	  	  	: 1;
			volatile unsigned long		RAMT     	      : 1;
			volatile unsigned long		unused15 	      : 1;
			volatile unsigned long		unused31   	   	: 16;
		} b;
		volatile unsigned short f;
	} Uts;

} UART_T;

#define	pUart1			((UART_T *) 0x00206000)
#define   Uart1               (*pUart1) //     (pUart1->)

#define	pUart2			((UART_T *) 0x00207000)
#define	Uart2				(*pUart2)

/* just a note to get to Uart1 data would look like this */
//	xx= pUart1->URXD[1].bByte;
/* or an error condition */
// if(pUart1->URXD[1].bERR);
//		Flag error
//	else
//		....


/* I2C Registers */
typedef union
{
	struct
	{
		volatile unsigned long		unused0  	  	 	: 1;
		volatile unsigned long		ADR		 	      	: 7;  /* slave address bits */
		volatile unsigned long		unused31   	   		: 24;
	} b;
	volatile unsigned short f;
} IADR_T;
#define	pIADR			((IADR_T *) 0x00217000)
#define	IADR				(*pIADR)


typedef union
{
	struct
	{
		volatile unsigned long		IC		  	  	 	: 6;
		volatile unsigned long		unused31   	   		: 26;
	} b;
	volatile unsigned short f;
} IFDR_T;
#define	pIFDR			((IFDR_T *) 0x00217004)
#define	IFDR				(*pIFDR)


typedef union
{
	struct
	{
		volatile unsigned long		unused1   	   		: 2;
		volatile unsigned long		RSTA	   	   		: 1;
		volatile unsigned long		TXAK	   	   		: 1;
		volatile unsigned long		MTX		   	   		: 1;
		volatile unsigned long		MSTA	   	   		: 1;
		volatile unsigned long		IIEN	   	   		: 1;
		volatile unsigned long		IEN		   	   		: 1;
		volatile unsigned long		unused31   	   		: 24;
	} b;
	volatile unsigned short f;
} I2CR_T;
#define	pI2CR			((I2CR_T *) 0x00217008)
#define	I2CR				(*pI2CR)


typedef union
{
	struct
	{
		volatile unsigned long		RXAK	   	   		: 1;
		volatile unsigned long		IIF		   	   		: 1;
		volatile unsigned long		SRW		   	   		: 1;
		volatile unsigned long		unused3	   	   		: 1;
		volatile unsigned long		IAL		   	   		: 1;
		volatile unsigned long		IBB		   	   		: 1;
		volatile unsigned long		IAAS	   	   		: 1;
		volatile unsigned long		ICF		   	   		: 1;
		volatile unsigned long		unused31   	   		: 24;
	} b;
	volatile unsigned short f;
 } I2CSR_T;
#define	pI2CSR			((I2CSR_T *) 0x0021700C)
#define	I2CSR				(*pI2CSR)


typedef union
{
	struct
	{
		volatile unsigned long		D		  	  	 	: 8;
		volatile unsigned long		unused31   	   		: 24;
	} b;
	volatile unsigned short f;
} I2DR_T;
#define	pI2DR			((I2DR_T *) 0x00217010)
#define	I2DR				(*pI2DR)



/* SPI Registers */

typedef union
{
	struct
	{
        volatile unsigned long      DATA                : 16;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} RXDATAREG1_T;
#define pRXDATAREG1           ((RXDATAREG1_T *) 0x00213000)
#define RXDATAREG1                (*pRXDATAREG1)


typedef union
{
	struct
	{
        volatile unsigned long      DATA                : 16;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} RXDATAREG2_T;
#define pRXDATAREG2           ((RXDATAREG2_T *) 0x00219000)
#define RXDATAREG2                (*pRXDATAREG2)


typedef union
{
	struct
	{
        volatile unsigned long      DATA                : 16;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} TXDATAREG1_T;
#define pTXDATAREG1           ((TXDATAREG1_T *) 0x00213004)
#define TXDATAREG1                (*pTXDATAREG1)


typedef union
{
	struct
	{
        volatile unsigned long      DATA                : 16;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} TXDATAREG2_T;
#define pTXDATAREG2           ((TXDATAREG2_T *) 0x00219004)
#define TXDATAREG2                (*pTXDATAREG2)


typedef union
{
	struct
	{
        volatile unsigned long      BIT_COUNT           : 4;
        volatile unsigned long      POL                 : 1;
        volatile unsigned long      PHA                 : 1;
        volatile unsigned long      SSCTL               : 1;
        volatile unsigned long      SSPOL               : 1;
        volatile unsigned long      XCH                 : 1;
        volatile unsigned long      SPIEN               : 1;
        volatile unsigned long      MODE                : 1;
        volatile unsigned long      DRCTL               : 2;
        volatile unsigned long      DATARATE            : 3;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} CONTROLREG1_T;
#define pCONTROLREG1           ((CONTROLREG1_T *) 0x00213008)
#define CONTROLREG1                (*pCONTROLREG1)


typedef union
{
	struct
	{
        volatile unsigned long      BIT_COUNT           : 4;
        volatile unsigned long      POL                 : 1;
        volatile unsigned long      PHA                 : 1;
        volatile unsigned long      SSCTL               : 1;
        volatile unsigned long      SSPOL               : 1;
        volatile unsigned long      XCH                 : 1;
        volatile unsigned long      SPIEN               : 1;
        volatile unsigned long      MODE                : 1;
        volatile unsigned long      DRCTL               : 2;
        volatile unsigned long      DATARATE            : 3;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} CONTROLREG2_T;
#define pCONTROLREG2           ((CONTROLREG2_T *) 0x00219008)
#define CONTROLREG2                (*pCONTROLREG2)


typedef union
{
	struct
	{
        volatile unsigned long      TE                  : 1;
        volatile unsigned long      TH                  : 1;
        volatile unsigned long      TF                  : 1;
        volatile unsigned long      RR                  : 1;
        volatile unsigned long      RH                  : 1;
        volatile unsigned long      RF                  : 1;
        volatile unsigned long      RO                  : 1;
        volatile unsigned long      BO                  : 1;
        volatile unsigned long      TEEN                : 1;
        volatile unsigned long      THEN                : 1;
        volatile unsigned long      TFEN                : 1;
        volatile unsigned long      RREN                : 1;
        volatile unsigned long      RHEN                : 1;
        volatile unsigned long      RFEN                : 1;
        volatile unsigned long      ROEN                : 1;
        volatile unsigned long      BOEN                : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} INTREG1_T;
#define pINTREG1           ((INTREG1_T *) 0x0021300C)
#define INTREG1                (*pINTREG1)


typedef union
{
	struct
	{
        volatile unsigned long      TE                  : 1;
        volatile unsigned long      TH                  : 1;
        volatile unsigned long      TF                  : 1;
        volatile unsigned long      RR                  : 1;
        volatile unsigned long      RH                  : 1;
        volatile unsigned long      RF                  : 1;
        volatile unsigned long      RO                  : 1;
        volatile unsigned long      BO                  : 1;
        volatile unsigned long      TEEN                : 1;
        volatile unsigned long      THEN                : 1;
        volatile unsigned long      TFEN                : 1;
        volatile unsigned long      RREN                : 1;
        volatile unsigned long      RHEN                : 1;
        volatile unsigned long      RFEN                : 1;
        volatile unsigned long      ROEN                : 1;
        volatile unsigned long      BOEN                : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
} INTREG2_T;
#define pINTREG2           ((INTREG2_T *) 0x0021900C)
#define INTREG2                (*pINtREG2)


typedef union
{
	struct
	{
        volatile unsigned long      TXCNT               : 4;
        volatile unsigned long      RXCNT               : 4;
        volatile unsigned long      SSTATUS             : 4;
        volatile unsigned long      unused3             : 2;
        volatile unsigned long      LBC                 : 1;
        volatile unsigned long      unused31            : 17;
	} b;
	volatile unsigned short f;
 } TESTREG1_T;
#define pTESTREG1          ((TESTREG1_T *) 0x00213010)
#define TESTREG1               (*pTESTREG1)


typedef union
{
	struct
	{
        volatile unsigned long      TXCNT               : 4;
        volatile unsigned long      RXCNT               : 4;
        volatile unsigned long      SSTATUS             : 4;
        volatile unsigned long      unused3             : 2;
        volatile unsigned long      LBC                 : 1;
        volatile unsigned long      unused31            : 17;
	} b;
	volatile unsigned short f;
 } TESTREG2_T;
#define pTESTREG2          ((TESTREG2_T *) 0x00219010)
#define TESTREG2               (*pTESTREG2)


 typedef union
{
	struct
	{
        volatile unsigned long      WAIT                : 15;
        volatile unsigned long      CSRC                : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
 } PERIODREG1_T;
#define pPERIODREG1          ((PERIODREG1_T *) 0x00213014)
#define PERIODREG1               (*pPERIODREG1)


 typedef union
{
	struct
	{
        volatile unsigned long      WAIT                : 15;
        volatile unsigned long      CSRC                : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
 } PERIODREG2_T;
#define pPERIODREG2          ((PERIODREG2_T *) 0x00219014)
#define PERIODREG2               (*pPERIODREG2)


 typedef union
{
	struct
	{
        volatile unsigned long      unused0             : 4;
        volatile unsigned long      RHDMA               : 1;
        volatile unsigned long      RFDMA               : 1;
        volatile unsigned long      TEDMA               : 1;
        volatile unsigned long      THDMA               : 1;
        volatile unsigned long      unused5             : 4;
        volatile unsigned long      RHDEN               : 1;
        volatile unsigned long      RFDEN               : 1;
        volatile unsigned long      TEDEN               : 1;
        volatile unsigned long      THDEN               : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
 } DMAREG1_T;
#define pDMAREG1          ((DMAREG1_T *) 0x00213018)
#define DMAREG1               (*pDMAREG1)


 typedef union
{
	struct
	{
        volatile unsigned long      unused0             : 4;
        volatile unsigned long      RHDMA               : 1;
        volatile unsigned long      RFDMA               : 1;
        volatile unsigned long      TEDMA               : 1;
        volatile unsigned long      THDMA               : 1;
        volatile unsigned long      unused5             : 4;
        volatile unsigned long      RHDEN               : 1;
        volatile unsigned long      RFDEN               : 1;
        volatile unsigned long      TEDEN               : 1;
        volatile unsigned long      THDEN               : 1;
        volatile unsigned long      unused31            : 16;
	} b;
	volatile unsigned short f;
 } DMAREG2_T;
#define pDMAREG2          ((DMAREG2_T *) 0x00219018)
#define DMAREG2               (*pDMAREG2)


 typedef union
{
	struct
	{
        volatile unsigned long      START               : 1;
        volatile unsigned long      unused31            : 31;
	} b;
	volatile unsigned short f;
 } RESETREG1_T;
#define pRESETREG1          ((RESETREG1_T *) 0x0021301C)
#define RESETREG1               (*pRESETREG1)


 typedef union
{
	struct
	{
        volatile unsigned long      START               : 1;
        volatile unsigned long      unused31            : 31;
	} b;
	volatile unsigned short f;
 } RESETREG2_T;
#define pRESETREG2          ((RESETREG2_T *) 0x0021901C)
#define RESETREG2               (*pRESETREG2)

// a few helpers to make generic port usage easier
typedef union
{
	struct tag_porta
  	{
  		volatile unsigned long			Bit0	  	: 1;
  		volatile unsigned long			Bit1	  	: 1;
  		volatile unsigned long			Bit2	  	: 1;
  		volatile unsigned long			Bit3	  	: 1;
  		volatile unsigned long			Bit4	  	: 1;
  		volatile unsigned long			Bit5	  	: 1;
  		volatile unsigned long			Bit6	  	: 1;
  		volatile unsigned long			Bit7	  	: 1;
  		volatile unsigned long			Bit8	  	: 1;
  		volatile unsigned long			Bit9	  	: 1;
  		volatile unsigned long			Bit10	  	: 1;
  		volatile unsigned long			Bit11	  	: 1;
  		volatile unsigned long			Bit12	  	: 1;
  		volatile unsigned long			Bit13	  	: 1;
  		volatile unsigned long			Bit14	  	: 1;
  		volatile unsigned long			Bit15	  	: 1;
  		volatile unsigned long			Bit16	  	: 1;
  		volatile unsigned long			Bit17	  	: 1;
  		volatile unsigned long			Bit18	  	: 1;
  		volatile unsigned long			Bit19	  	: 1;
  		volatile unsigned long			Bit20	  	: 1;
  		volatile unsigned long			Bit21	  	: 1;
  		volatile unsigned long			Bit22	  	: 1;
  		volatile unsigned long			Bit23	  	: 1;
  		volatile unsigned long			Bit24	  	: 1;
  		volatile unsigned long			Bit25	  	: 1;
  		volatile unsigned long			Bit26	  	: 1;
  		volatile unsigned long			Bit27	  	: 1;
  		volatile unsigned long			Bit28	  	: 1;
  		volatile unsigned long			Bit29	  	: 1;
  		volatile unsigned long			Bit30	  	: 1;
  		volatile unsigned long			Bit31	  	: 1;
	} b;
	volatile unsigned long f;
} GpioPort_t;

typedef struct tagDoubleBit
{
  	volatile unsigned long			Bit0	  	: 2;
  	volatile unsigned long			Bit1	  	: 2;
  	volatile unsigned long			Bit2	  	: 2;
  	volatile unsigned long			Bit3	  	: 2;
  	volatile unsigned long			Bit4	  	: 2;
  	volatile unsigned long			Bit5	  	: 2;
  	volatile unsigned long			Bit6	  	: 2;
  	volatile unsigned long			Bit7	  	: 2;
  	volatile unsigned long			Bit8	  	: 2;
  	volatile unsigned long			Bit9	  	: 2;
  	volatile unsigned long			Bit10	  	: 2;
  	volatile unsigned long			Bit11	  	: 2;
  	volatile unsigned long			Bit12	  	: 2;
  	volatile unsigned long			Bit13	  	: 2;
  	volatile unsigned long			Bit14	  	: 2;
  	volatile unsigned long			Bit15	  	: 2;
  	volatile unsigned long			Bit16	  	: 2;
  	volatile unsigned long			Bit17	  	: 2;
  	volatile unsigned long			Bit18	  	: 2;
  	volatile unsigned long			Bit19	  	: 2;
  	volatile unsigned long			Bit20	  	: 2;
  	volatile unsigned long			Bit21	  	: 2;
  	volatile unsigned long			Bit22	  	: 2;
  	volatile unsigned long			Bit23	  	: 2;
  	volatile unsigned long			Bit24	  	: 2;
  	volatile unsigned long			Bit25	  	: 2;
  	volatile unsigned long			Bit26	  	: 2;
  	volatile unsigned long			Bit27	  	: 2;
  	volatile unsigned long			Bit28	  	: 2;
  	volatile unsigned long			Bit29	  	: 2;
  	volatile unsigned long			Bit30	  	: 2;
  	volatile unsigned long			Bit31	  	: 2;
}DoubleBit_t;


typedef union
{
	DoubleBit_t b;
	volatile unsigned __int64 f;
} GPIO_2BIT_t;

enum bit_number_t
{
	BitNum0  = 0,
	BitNum1  = 1,
	BitNum2  = 2,
	BitNum3  = 3,
	BitNum4  = 4,
	BitNum5  = 5,
	BitNum6  = 6,
	BitNum7  = 7,
	BitNum8  = 8,
	BitNum9  = 9,
	BitNum10 = 10,
	BitNum11 = 11,
	BitNum12 = 12,
	BitNum13 = 13,
	BitNum14 = 14,
	BitNum15 = 15,
	BitNum16 = 16,
	BitNum17 = 17,
	BitNum18 = 18,
	BitNum19 = 19,
	BitNum20 = 20,
	BitNum21 = 21,
	BitNum22 = 22,
	BitNum23 = 23,
	BitNum24 = 24,
	BitNum25 = 25,
	BitNum26 = 26,
	BitNum27 = 27,
	BitNum28 = 28,
	BitNum29 = 29,
	BitNum30 = 30,
	BitNum31 = 31,

	BitNumBorder = BitNum16
};
// force a byte store at pDest of value
inline void StoreByte(volatile void *pDest,unsigned char value)
{
#ifndef WINSIM
	asm
	{
		strb	value,[pDest]
	}
#else
	unsigned char *pDestination = (unsigned char *) pDest;
	*pDestination = value;
#endif
}

#endif // MX1_REGISTERS_H_INCLUDED_F3E1EF42_35D6_11D2_93BE_00805F2987D9


