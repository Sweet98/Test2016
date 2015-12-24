/*=================================================================================
  class:

//=================================================================================
   $RCSfile: interfaces_usb2_hw/usb2_mx25.h $
   $Revision: 1.9 $
   $Date: 2009/05/21 03:28:28EDT $

//=================================================================================

//=================================================================================*/

#ifndef USB2_MX25_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USB2_MX25_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "iMX25_USBOTG.h"
#include "iMX25_CRM.h"

// MX25usb2 regs
#define		  UOG_ID           		         (*((volatile UINT *)       USB_OTG_ID))
#define		  UOG_HWGENERAL			         (*((volatile UINT *)       USB_OTG_HWGENERAL))
#define		  UOG_HWHOST				 (*((volatile UINT *)       USB_OTG_HWHOST))
#define		  UOG_HWDEVICE				 (*((volatile UINT *)       USB_OTG_HWDEVICE))
#define		  UOG_HWTXBUF			         (*((volatile UINT *)       USB_OTG_HWTXBUF))
#define		  UOG_HWRXBUF			         (*((volatile UINT *)       USB_OTG_HWRXBUF))
#define		  UOG_GPTIMER0LD		         (*((volatile UINT *)       USB_OTG_GPTIMER0LD))
#define		  UOG_GPTIMER0CTRL		         (*((volatile UINT *)       USB_OTG_GPTIMER0CTRL))
#define		  UOG_GPTIMER1LD		         (*((volatile UINT *)       USB_OTG_GPTIMER1LD))
#define		  UOG_GPTIMER1CTRL			 (*((volatile UINT *)       USB_OTG_GPTIMER1CTRL))
#define		  UOG_SBUSCFG			         (*((volatile UINT *)       (USB_OTG_BASE_ADDR + 0x090)))
#define		  UOG_CAPLENGTH			         (*((volatile UCHAR *)      USB_OTG_CAPLENGTH))
#define		  UOG_HCIVERSION			 (*((volatile USHORT *)     USB_OTG_HCIVERSION))
#define		  UOG_HCSPARAMS			         (*((volatile UINT *)       USB_OTG_HCSPARAMS))
#define		  UOG_HCCPARAMS			         (*((volatile UINT *)       USB_OTG_HCCPARAMS))
#define		  UOG_DCIVERSION			 (*((volatile USHORT *)     USB_OTG_DCIVERSION))
#define		  UOG_DCCPARAMS			         (*((volatile UINT *)       USB_OTG_DCCPARAMS))
#define		  UOG_USBCMD			         (*((volatile UINT *)       USB_OTG_USBCMD))
#define		  UOG_USBSTS			         (*((volatile UINT *)       USB_OTG_USBSTS))
#define		  UOG_USBINTR			         (*((volatile UINT *)       USB_OTG_USBINTR))
#define		  UOG_FRINDEX			         (*((volatile UINT *)       USB_OTG_FRINDEX))
#define		  UOG_PERIODICLISTBASE			 (*((volatile UINT *)       USB_OTG_PERIODICLISTBASE))
#define		  UOG_ASYNCLISTADDR			 (*((volatile UINT *)       USB_OTG_ASYNCLISTADDR))
#define		  UOG_BURSTSIZE			         (*((volatile UINT *)       USB_OTG_BURSTSIZE))
#define		  UOG_TXFILLTUNING			 (*((volatile UINT *)       USB_OTG_TXFILLTUNING))
#define		  UOG_ULPIVIEW			         (*((volatile UINT *)       (USB_OTG_BASE_ADDR + 0x170)))
#define		  UOG_CFGFLAG			         (*((volatile UINT *)       USB_OTG_CONFIGFLAG))
#define		  UOG_PORTSC1			         (*((volatile UINT *)       USB_OTG_PORTSC1))
#define		  UOG_OTGSC			         (*((volatile UINT *)       USB_OTG_OTGSC))
#define		  UOG_USBMODE			         (*((volatile UINT *)       USB_OTG_USBMODE))
#define		  UOG_ENDPTSETUPSTAT			 (*((volatile UINT *)       USB_OTG_ENPDTSETUPSTAT))
#define		  UOG_ENDPTPRIME		         (*((volatile UINT *)       USB_OTG_ENDPTPRIME))
#define		  UOG_ENDPTFLUSH			 (*((volatile UINT *)       USB_OTG_ENDPTFLUSH))
#define		  UOG_ENDPTSTAT			         (*((volatile UINT *)       USB_OTG_ENDPTSTATUS))
#define		  UOG_ENDPTCOMPLETE			 (*((volatile UINT *)       USB_OTG_ENDPTCOMPLETE))
#define		  UOG_ENDPTCTRL0			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL0))
#define		  UOG_ENDPTCTRL1			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL1))
#define		  UOG_ENDPTCTRL2			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL2))
#define		  UOG_ENDPTCTRL3			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL3))
#define		  UOG_ENDPTCTRL4			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL4))
#define		  UOG_ENDPTCTRL5			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL5))
#define		  UOG_ENDPTCTRL6			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL6))
#define		  UOG_ENDPTCTRL7			 (*((volatile UINT *)       USB_OTG_ENDPTCTRL7))

#define		  USB_CTRL			         (*((volatile UINT *)       USB_CONTROL_REG))
#define		  USB_OTG_MIRROR			 (*((volatile UCHAR *)      USB_OTG_MIRROR_REG))
#define		  USB_PHY_CTRL_FUNC			 (*((volatile UINT *)       USB_OTG_UTMI_PHY_CTRL_FUNC))
#define		  USB_PHY_CTRL_TEST			 (*((volatile UINT *)       USB_OTG_UTMI_PHY_CTRL_TEST))

// iMX25 Clock regs
#define		  CLK_MPCTL			         (*((volatile UINT *)       CRM_MPCTL))
#define		  CLK_UPCTL			         (*((volatile UINT *)       CRM_UPCTL))
#define		  CLK_CCTL			         (*((volatile UINT *)       CRM_CCTL))
#define		  CLK_CGCR0			         (*((volatile UINT *)       CRM_CGR0))
#define		  CLK_CGCR1			         (*((volatile UINT *)       CRM_CGR1))
#define		  CLK_CGCR2			         (*((volatile UINT *)       CRM_CGR2))
#define		  CLK_PCDR0			         (*((volatile UINT *)       CRM_PDR0))
#define		  CLK_PCDR1			         (*((volatile UINT *)       CRM_PDR1))
#define		  CLK_PCDR2			         (*((volatile UINT *)       CRM_PDR2))
#define		  CLK_PCDR3			         (*((volatile UINT *)       CRM_PDR3))
#define		  CLK_RCSR			         (*((volatile UINT *)       CRM_CRSR))
#define		  CLK_CRDR			         (*((volatile UINT *)       CRM_CRDR))

#define		  CLK_MCR			         (*((volatile UINT *)       CRM_MCR))

// register bits
// PORTSCx
#define PORTSC_PTS_MASK			(3U << 30)	
#define PORTSC_PTS_UTMI			(0 << 30)	
#define PORTSC_PTS_PHILIPS		(1U << 30)	
#define PORTSC_PTS_ULPI			(2U << 30)	
#define PORTSC_PTS_SERIAL		(3U << 30)	
#define PORTSC_STS			(1 << 29)	
#define PORTSC_PTW			(1 << 28)	
#define PORTSC_PSPD_FS			(0 << 26)	
#define PORTSC_PSPD_LS			(1 << 26)
#define PORTSC_PSPD_HS			(2 << 26)		
#define PORTSC_PFSC			(1 << 24)		
#define PORTSC_PHCD			(1 << 23)
#define PORTSC_WKOC			(1 << 22)
#define PORTSC_WKDS			(1 << 21)
#define PORTSC_WKCN			(1 << 20)

#define PORTSC_PTC_MASK		        (15 << 16)
#define PORTSC_PTC_TMDS		        (0 << 16)
#define PORTSC_PTC_JSTATE		(1 << 16)
#define PORTSC_PTC_KSTATE		(2 << 16)
#define PORTSC_PTC_SE0		        (3 << 16)
#define PORTSC_PTC_PACKAGE		(4 << 16)
#define PORTSC_PTC_FORCE_HS		(5 << 16)
#define PORTSC_PTC_FORCE_FS		(6 << 16)
#define PORTSC_PTC_FORCE_LS		(7 << 16)

#define PORTSC_PIC_MASK		        (3 << 14)
#define PORTSC_PIC_OFF		        (0 << 14)
#define PORTSC_PIC_AMBER		(1 << 14)
#define PORTSC_PIC_GREEN		(2 << 14)
#define PORTSC_PO		        (1 << 13)
#define PORTSC_PP		        (1 << 12)	
#define PORTSC_LS_MASK			(3 << 10)	
#define PORTSC_LS_SE0			(0 << 10)	
#define PORTSC_LS_K_STATE		(1 << 10)	
#define PORTSC_LS_J_STATE		(2 << 10)	
#define PORTSC_HSP		        (1 <<  9)
#define PORTSC_PR  		        (1 <<  8)	
#define PORTSC_SUSP		        (1 <<  7)	
#define PORTSC_FPR	                (1 <<  6)	
#define PORTSC_OCC		        (1 <<  5)	
#define PORTSC_OCA		        (1 <<  4)	
#define PORTSC_PEC	                (1 <<  3)	
#define PORTSC_PE		        (1 <<  2)	
#define PORTSC_CSC             	        (1 <<  1)	
#define PORTSC_CCS	                (1 <<  0)	

// UOG_OTGSC Register
// control bits: 
#define  OTGSC_CTRL_VBUS_DISCHARGE	(1 <<  0)
#define  OTGSC_CTRL_VBUS_CHARGE		(1 <<  1)
#define  OTGSC_CTRL_OTG_TERM		(1 <<  3)  
#define  OTGSC_CTRL_DATA_PULSING	(1 <<  4)
#define  OTGSC_CTRL_USB_ID_PU		(1 <<  5)  
// current status: (R/O) 
#define  OTGSC_STS_USB_ID		(1 <<  8)  
#define  OTGSC_STS_A_VBUS_VALID		(1 <<  9)
#define  OTGSC_STS_A_SESSION_VALID	(1 << 10)
#define  OTGSC_STS_B_SESSION_VALID	(1 << 11)
#define  OTGSC_STS_B_SESSION_END	(1 << 12)
#define  OTGSC_STS_1ms_TIMER		(1 << 13)
#define  OTGSC_STS_DATA_PULSE		(1 << 14)
// interrupt status: (write to clear)
#define  OTGSC_IS_MASK			(0x7f << 16)
#define  OTGSC_IS_USB_ID		(1 << 16)
#define  OTGSC_IS_A_VBUS_VALID		(1 << 17)
#define  OTGSC_IS_A_SESSION_VALID	(1 << 18)
#define  OTGSC_IS_B_SESSION_VALID	(1 << 19)
#define  OTGSC_IS_B_SESSION_END		(1 << 20)
#define  OTGSC_IS_1ms_TIMER		(1 << 21)
#define  OTGSC_IS_DATA_PULSE		(1 << 22)
// interrupt enables:
#define  OTGSC_IE_MASK			(0x7f << 24)
#define  OTGSC_IE_USB_ID		(1 << 24)
#define  OTGSC_IE_A_VBUS_VALID		(1 << 25)
#define  OTGSC_IE_A_SESSION_VALID	(1 << 26)
#define  OTGSC_IE_B_SESSION_VALID	(1 << 27)
#define  OTGSC_IE_B_SESSION_END		(1 << 28)
#define  OTGSC_IE_1ms_TIMER		(1 << 19)
#define  OTGSC_IE_DATA_PULSE		(1 << 30)

// USBMODE
#define USBMODE_SDIS		(1 << 4)	/* stream disable mode */
#define USBMODE_SLOM		(1 << 3)	/* setup lockout mode */
#define USBMODE_ES		(1 << 2)	/* (big) endian select */
#define USBMODE_CM_MASK		(3 << 0)	/* controller mode mask */
#define USBMODE_CM_HOST		(3 << 0)	/* host */
#define USBMODE_CM_DEVICE	(2 << 0)	/* device */
#define USBMODE_CM_reserved	(1 << 0)	/* reserved */
#define USBMODE_CM_IDLE		(0 << 0)	/* idle */

// USBCTRL 
#define UCTRL_OWIR		(1 << 31)	/* OTG wakeup intr request received */
#define UCTRL_OSIC_MASK		(3 << 29)	/* OTG  Serial Interface Config: */
#define UCTRL_OSIC_DU6		(0 << 29)	/* Differential/unidirectional 6 wire */
#define UCTRL_OSIC_DB4		(1 << 29)	/* Differential/bidirectional  4 wire */
#define UCTRL_OSIC_SU6		(2 << 29)	/* single-ended/unidirectional 6 wire */
#define UCTRL_OSIC_SB3		(3 << 29)	/* single-ended/bidirectional  3 wire */

#define UCTRL_OUIE		(1 << 28)	/* OTG ULPI intr enable */
#define UCTRL_OWIE		(1 << 27)	/* OTG wakeup intr enable */
#define UCTRL_HEXTEN    	(1 << 26)	
#define UCTRL_OEXTEN    	(1 << 25)	
#define UCTRL_OPM		(1 << 24)	/* OTG power mask */
#define UCTRL_H2WIR		(1 << 23)	/* HOST2 wakeup intr request received */
#define UCTRL_H2SIC_MASK	(3 << 21)	/* HOST2 Serial Interface Config: */
#define UCTRL_H2SIC_DU6		(0 << 21)	/* Differential/unidirectional 6 wire */
#define UCTRL_H2SIC_DB4		(1 << 21)	/* Differential/bidirectional  4 wire */
#define UCTRL_H2SIC_SU6		(2 << 21)	/* single-ended/unidirectional 6 wire */
#define UCTRL_H2SIC_SB3		(3 << 21)	/* single-ended/bidirectional  3 wire */

#define UCTRL_H2UIE		(1 << 20)	/* HOST2 ULPI intr enable */
#define UCTRL_H2WIE		(1 << 19)	/* HOST2 wakeup intr enable */
#define UCTRL_H2PM		(1 << 16)	/* HOST2 power mask */

#define UCTRL_PP		(1 << 11)	
#define UCTRL_XCSO      	(1 << 10)	
#define UCTRL_XCSH2     	(1 <<  9)	
#define UCTRL_IPPUIDP		(1 <<  8)	
#define UCTRL_IPPUEDUP		(1 <<  7)	
#define UCTRL_IPPUEDOWN		(1 <<  6)	
#define UCTRL_H2DT		(1 <<  5)	/* HOST2 TLL disabled */
#define UCTRL_USBTE		(1 <<  4)	
#define UCTRL_OCPOL		(1 <<  3)	
#define UCTRL_OCE		(1 <<  2)	
#define UCTRL_H2OCS		(1 <<  1)	
#define UCTRL_OOCS		(1 <<  0)	

// USB_PHY_CTRL_FUNC 
#define UTMI_CLKVALID 		(1 << 27)	
#define UTMI_SUSPENDM 		(1 << 26)	
#define UTMI_RESET 		(1 << 25)	
#define UTMI_USBEN 		(1 << 24)	
#define UTMI_EVDO 		(1 << 23)	
#define UTMI_LSFE 		(1 << 22)	
#define UTMI_HOSTPORT 		(1 << 21)	
#define UTMI_VCONTROLLOADM 	(1 << 20)	


// USBCMD
#define UCMD_RESET		(1 << 1)	/* controller reset */

// OTG_MIRROR 
#define OTGM_SESEND		(1 << 4)	/* B device session end */
#define OTGM_VBUSVAL		(1 << 3)	/* Vbus valid */
#define OTGM_BSESVLD		(1 << 2)	/* B session Valid */
#define OTGM_ASESVLD		(1 << 1)	/* A session Valid */
#define OTGM_IDIDG		(1 << 0)	/* OTG ID pin status */
						/* 1=high: Operate as B-device */
						/* 0=low : Operate as A-device */
// ULPIVIEW register bits
#define ULPIVW_WU		(1 << 31)	/* Wakeup */
#define ULPIVW_RUN		(1 << 30)	/* read/write run */
#define ULPIVW_WRITE		(1 << 29)	/* 0=read  1=write */
#define ULPIVW_SS		(1 << 27)	/* SyncState */
#define ULPIVW_PORT_MASK	0x07		/* Port field */
#define ULPIVW_PORT_SHIFT	24
#define ULPIVW_ADDR_MASK	0xFF		/* data address field */
#define ULPIVW_ADDR_SHIFT	16
#define ULPIVW_RDATA_MASK	0xFF		/* read data field */
#define ULPIVW_RDATA_SHIFT	8
#define ULPIVW_WDATA_MASK	0xFF		/* write data field */
#define ULPIVW_WDATA_SHIFT	0

// USB CMD  Register
#define  USB_CMD_RUN_STOP                     (0x00000001)
#define  USB_CMD_CTRL_RESET                   (0x00000002)
#define  USB_CMD_PERIODIC_SCHEDULE_EN         (0x00000010)
#define  USB_CMD_ASYNC_SCHEDULE_EN            (0x00000020)
#define  USB_CMD_INT_AA_DOORBELL              (0x00000040)
#define  USB_CMD_ASP                          (0x00000300)
#define  USB_CMD_ASYNC_SCH_PARK_EN            (0x00000800)
#define  USB_CMD_SUTW                         (0x00002000)
#define  USB_CMD_ATDTW                        (0x00004000)
#define  USB_CMD_ITC                          (0x00FF0000)

// USB MODE Register
#define  USB_MODE_CTRL_MODE_IDLE              (0x00000000)
#define  USB_MODE_CTRL_MODE_DEVICE            (0x00000002)
#define  USB_MODE_CTRL_MODE_HOST              (0x00000003)
#define  USB_MODE_CTRL_MODE_RSV               (0x00000001)
#define  USB_MODE_SETUP_LOCK_OFF              (0x00000008)
#define  USB_MODE_STREAM_DISABLE              (0x00000010)

// PORTSCX  Register
#define  PORTSCX_CURRENT_CONNECT_STATUS       (0x00000001)
#define  PORTSCX_CONNECT_STATUS_CHANGE        (0x00000002)
#define  PORTSCX_PORT_ENABLE                  (0x00000004)
#define  PORTSCX_PORT_EN_DIS_CHANGE           (0x00000008)
#define  PORTSCX_OVER_CURRENT_ACT             (0x00000010)
#define  PORTSCX_OVER_CURRENT_CHG             (0x00000020)
#define  PORTSCX_PORT_FORCE_RESUME            (0x00000040)
#define  PORTSCX_PORT_SUSPEND                 (0x00000080)
#define  PORTSCX_PORT_RESET                   (0x00000100)
#define  PORTSCX_LINE_STATUS_BITS             (0x00000C00)
#define  PORTSCX_PORT_POWER                   (0x00001000)
#define  PORTSCX_PORT_INDICTOR_CTRL           (0x0000C000)
#define  PORTSCX_PORT_TEST_CTRL               (0x000F0000)
#define  PORTSCX_WAKE_ON_CONNECT_EN           (0x00100000)
#define  PORTSCX_WAKE_ON_CONNECT_DIS          (0x00200000)
#define  PORTSCX_WAKE_ON_OVER_CURRENT         (0x00400000)
#define  PORTSCX_PHY_LOW_POWER_SPD            (0x00800000)
#define  PORTSCX_PORT_FORCE_FULL_SPEED        (0x01000000)
#define  PORTSCX_PORT_SPEED_MASK              (0x0C000000)
#define  PORTSCX_PORT_WIDTH                   (0x10000000)
#define  PORTSCX_PHY_TYPE_SEL                 (0xC0000000)
// bit 11-10 are line status
#define  PORTSCX_LINE_STATUS_SE0              (0x00000000)
#define  PORTSCX_LINE_STATUS_JSTATE           (0x00000400)
#define  PORTSCX_LINE_STATUS_KSTATE           (0x00000800)
#define  PORTSCX_LINE_STATUS_UNDEF            (0x00000C00)
#define  PORTSCX_LINE_STATUS_BIT_POS          (10)
// bit 15-14 are port indicator control
#define  PORTSCX_PIC_OFF                      (0x00000000)
#define  PORTSCX_PIC_AMBER                    (0x00004000)
#define  PORTSCX_PIC_GREEN                    (0x00008000)
#define  PORTSCX_PIC_UNDEF                    (0x0000C000)
#define  PORTSCX_PIC_BIT_POS                  (14)
// bit 19-16 are port test control 
#define  PORTSCX_PTC_DISABLE                  (0x00000000)
#define  PORTSCX_PTC_JSTATE                   (0x00010000)
#define  PORTSCX_PTC_KSTATE                   (0x00020000)
#define  PORTSCX_PTC_SEQNAK                   (0x00030000)
#define  PORTSCX_PTC_PACKET                   (0x00040000)
#define  PORTSCX_PTC_FORCE_EN                 (0x00050000)
#define  PORTSCX_PTC_BIT_POS                  (16)
// bit 27-26 are port speed 
#define  PORTSCX_PORT_SPEED_FULL              (0x00000000)
#define  PORTSCX_PORT_SPEED_LOW               (0x04000000)
#define  PORTSCX_PORT_SPEED_HIGH              (0x08000000)
#define  PORTSCX_PORT_SPEED_UNDEF             (0x0C000000)
#define  PORTSCX_SPEED_BIT_POS                (26)
// bit 28 is parallel transceiver width for UTMI interface 
#define  PORTSCX_PTW                          (0x10000000)
#define  PORTSCX_PTW_8BIT                     (0x00000000)
#define  PORTSCX_PTW_16BIT                    (0x10000000)
// bit 31-30 are port transceiver select 
#define  PORTSCX_PTS_UTMI                     (0x00000000)
#define  PORTSCX_PTS_ULPI                     (0x80000000)
#define  PORTSCX_PTS_FSLS                     (0xC0000000)
#define  PORTSCX_PTS_BIT_POS                  (30)

// USB STS Register
#define  USB_STS_INT                          (0x00000001)
#define  USB_STS_ERR                          (0x00000002)
#define  USB_STS_PORT_CHANGE                  (0x00000004)
#define  USB_STS_FRM_LST_ROLL                 (0x00000008)
#define  USB_STS_SYS_ERR                      (0x00000010)
#define  USB_STS_IAA                          (0x00000020)
#define  USB_STS_RESET                        (0x00000040)
#define  USB_STS_SOF                          (0x00000080)
#define  USB_STS_SUSPEND                      (0x00000100)
#define  USB_STS_HC_HALTED                    (0x00001000)
#define  USB_STS_RCL                          (0x00002000)
#define  USB_STS_PERIODIC_SCHEDULE            (0x00004000)
#define  USB_STS_ASYNC_SCHEDULE               (0x00008000)

#define  USB_STS_RESUME                       (0x00000000)

// USB INTR Register
#define  USB_INTR_INT_EN                      (0x00000001)
#define  USB_INTR_ERR_INT_EN                  (0x00000002)
#define  USB_INTR_PTC_DETECT_EN               (0x00000004)
#define  USB_INTR_FRM_LST_ROLL_EN             (0x00000008)
#define  USB_INTR_SYS_ERR_EN                  (0x00000010)
#define  USB_INTR_ASYN_ADV_EN                 (0x00000020)
#define  USB_INTR_RESET_EN                    (0x00000040)
#define  USB_INTR_SOF_EN                      (0x00000080)
#define  USB_INTR_DEVICE_SUSPEND              (0x00000100)

// Endpoint Setup Status
#define  EP_SETUP_STATUS_MASK                 (0x0000003F)
#define  EP_SETUP_STATUS_EP0		      (0x00000001)

#define USB_MAX_ENDPOINTS		8
#define USB_MAX_PIPES			(USB_MAX_ENDPOINTS*2)
#define USB_MAX_CTRL_PAYLOAD		64
#define	USB_DR_SYS_OFFSET		0x400

#endif /* USB_MX25_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */



