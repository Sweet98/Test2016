/**************************************************************************************/
//
// HIM 48X0G GENERATION 6 SOFTWARE
//
//  HARDWARE MACROS 
//  Date:       5/09/2008
//  Tools:      RVCT Build 674
//  Target:     Freescale iMX27
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: db_shared/Hardware_Header_Files/HardwareMacros.h $
// $Revision: 1.15 $
// $Date: 2011/11/08 18:01:12EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// Header:  HardwareMacros.h
//
// Provides Macros for Writing and Reading Hardware Registers
// Refer to ds005_Hardware_Definitions.doc in the Gen 6 documentation for more info
//
// Use HAZARD Macros when you do not want the macro to turn the off Interrupts
// MACROS are used because Inline will not always inline
//
//------------------------------------------------------------------------------

#ifndef HARDWAREMACROS_H_
#define HARDWAREMACROS_H_
#ifndef MATRIX_BOOT_CODE
#include "interruptMacros.h"		 //Used to inable or disable interrupts
#endif

#if PLATFORM_CURRENT == mxl
#error MXL is defined but have added MX27 routines
#endif	//Check to make sure mxl is not defined

//------------------------------------------------------------------------------
// MACROS
//------------------------------------------------------------------------------
// ===  MACRO  ======================================================================
// 
//         Name:  READ_REG(address)
// 
//  Description:  Reads a value of a register
//				  Use read_16Bit_reg for 16Bit Registers
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32bit             address        	address to access
//    in-out:  
//       out:  
//    return:  32bit/16bit					 	value in register
// -------------------------------------------------------------------------------------
#define READ_REG(address)     	 (*((volatile unsigned long *)(address)))

#define READ_16BIT_REG(address)  (*((volatile unsigned short *)(address)))

// ===  MACRO  ======================================================================
// 
//         Name:  FIELD_MASK(LSH,WID,value)
// 
//  Description:  Generates the mask for a Field.
//				  This is used to build a register value before writing to it.
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  INT		        LSH				Left Shift amount           
//        in:  INT              WID             Width of Field
//        in:  INT			    val             Value to write in Field
//    in-out:  
//       out:  32bit								Mask of Field with data
//    return:  
// -------------------------------------------------------------------------------------
// -EXAMPLE----
// --Set the USB_DIV in the Clock Source Control Register (CSCR)to divide the SPLL_CLK by 2
// field_mask(CRM_CSCR_USB_DIV,0x0001) Should return: 0x10000000
// --Setup the MPLL Control Register 0 (MPCTL0) This will set MPCTL0 to: 0x142119FE
// MPCTL0 = FIELD_MASK(CRM_MPCTL0_CPLM_LSH,CRM_MPCTL0_CPLM_WID, CRM_MPCTL0_CPLM_MODE_FOL) |
//			FIELD_MASK(CRM_MPCTL0_PD_LSH,CRM_MPCTL0_PD_WID, 0x05) |
//			FIELD_MASK(CRM_MPCTL0_MFD_LSH,CRM_MPCTL0_MFD_WID, 0x011) |
//			FIELD_MASK(CRM_MPCTL0_MFI_LSH,CRM_MPCTL0_MFI_WID, 0x0110) |
//			FIELD_MASK(CRM_MPCTL0_MFN_LSH,CRM_MPCTL0_MFN_WID, 1FE)
//
// =====================================================================================
#define FIELD_MASK(LSH,WID,value) ((value & WID) << LSH)


// ===  MACRO  ======================================================================
// 
//         Name:  READ_FIELD(address)
// 
//  Description:  Reads a value to a specific field in a register
//				  
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32bit             address        	address to access
//		  in:  32bit			 LSH				Left Shift amount
//		  in:  32bit			 WID				Width of Field
//    in-out:  
//       out:  
//    return:  int					 				value in field
// -------------------------------------------------------------------------------------
#define READ_FIELD(address,LSH,WID) \
     (((*((volatile unsigned long *)(address))) & (WID << LSH)) >> LSH)
     
#define READ_16BIT_FIELD(address,LSH,WID) \
     (((*((volatile unsigned short *)(address))) & (WID << LSH)) >> LSH)
          
//Interrupts are not turned on in Boot code so there is no reason to turn them off
#ifndef MATRIX_BOOT_CODE

// ===  MACRO  ======================================================================
// 
//         Name:  WRITE_FIELD_TO_REG(addr,LSH,WID,value)
// 
//  Description:  Writes a value to a specific field in a register.
//					This is done by anding the with with the value, 
//					shifting it to the Field Location and anding it with whats alread 
//					in the register.  Then writing the new value
//					This Macro uses the Read_Reg Macro for reading.
//				  Use write_field_to_16Bit_reg for 16Bit Registers 
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32bit             address        address to access
//        in:  INT		         LSH			Left Shift amount           
//        in:  32bit             WID            Width of Field
//        in:  INT				 value          Value to write in Field
//    in-out:  
//       out:  
//    return:  
// -------------------------------------------------------------------------------------
// -EXAMPLE----
// --Set the USB_DIV in the Clock Source Control Register (CSCR)to divide the SPLL_CLK by 2
// WRITE_FIELD_TO_REG(CRM_CSCR,CRM_CSCR_USB_DIV_LSH,CRM_CSCR_USB_DIV_WID,0x0001) 
// --Enable the CSI AHB Clock. 
// WRITE_FIELD_TO_REG(CRM_PCCR1,CRM_PCCR1_HCLK_CSI_LSH,CRM_PCCR1_HCLK_CSI_WID,CRM_ENABLE)
//
// =====================================================================================
#define WRITE_FIELD_TO_REG(address,LSH,WID,value) \
do{ \
	DISABLE_INTERRUPTS(); \
	*((volatile unsigned long *)(address)) = ((READ_REG(address) & ~(WID << LSH)) | ((value & WID) << LSH)); \
	RESTORE_INTERRUPTS(); \
}while(0)

#define WRITE_FIELD_TO_16BIT_REG(address,LSH,WID,value) \
do{ \
	DISABLE_INTERRUPTS(); \
	*((volatile unsigned short *)(address)) = ((READ_16BIT_REG(address) & ~(WID << LSH)) | ((value & WID) << LSH)); \
	RESTORE_INTERRUPTS(); \
}while(0)

// ===  MACRO  ======================================================================
// 
//         Name:  WRITE_REG (address,value)
// 
//  Description:  writes val to addr
//				  Use write_16Bit_reg for 16Bit Registers              
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32bit/16bit      address           address to access
//        in:  32bit/16bit      value             value to write      
//    in-out:  
//       out:  
//    return:  
// =====================================================================================
#define WRITE_REG(address,value) \
do{ \
	DISABLE_INTERRUPTS(); \
	*((volatile unsigned long *)(address)) = (value); \
	RESTORE_INTERRUPTS(); \
}while(0)
	 
#define WRITE_16BIT_REG(address,value)\
do{ \
	DISABLE_INTERRUPTS(); \
	*((volatile unsigned short *)(address)) = (value); \
	RESTORE_INTERRUPTS(); \
}while(0)

#endif //End - #ifndef MATRIX_BOOT_CODE

// ===  MACRO  ======================================================================
// 
//         Name:  HAZARD_WRITE_FIELD_TO_REG(addr,LSH,WID,value)
// 
//  Description:  Writes a value to a specific field in a register.
//					This is done by anding the with with the value, 
//					shifting it to the Field Location and anding it with whats alread 
//					in the register.  Then writing the new value
//					This Macro uses the Read_Reg Macro for reading.
//				  Use hazard_write_field_to_16Bit_reg for 16Bit Registers
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32Bit            address         address to access
//        in:  INT		        LSH				Left Shift amount           
//        in:  INT              WID             Width of Field
//        in:  INT			    val             Value to write in Field
//    in-out:  
//       out:  
//    return:  
// -------------------------------------------------------------------------------------
// -EXAMPLE----
// --Set the USB_DIV in the Clock Source Control Register (CSCR)to divide the SPLL_CLK by 2
// HAZARD_WRITE_FIELD_TO_REG(CRM_CSCR,CRM_CSCR_USB_DIV_LSH,CRM_CSCR_USB_DIV_WID,0x0001) 
// --Enable the CSI AHB Clock. 
// HAZARD_WRITE_FIELD_TO_REG(CRM_PCCR1,CRM_PCCR1_HCLK_CSI_LSH,CRM_PCCR1_HCLK_CSI_WID,CRM_ENABLE)
//
// =====================================================================================
#define HAZARD_WRITE_FIELD_TO_REG(address,LSH,WID,value) \
	(*((volatile unsigned long *)(address)) = ((READ_REG(address) & ~(WID << LSH)) | (((unsigned long)value & WID) << LSH)))

#define HAZARD_WRITE_FIELD_TO_16BIT_REG(address,LSH,WID,value) \
	(*((volatile unsigned short *)(address)) = ((READ_16BIT_REG(address) & ~(WID << LSH)) | (((unsigned short)value & WID) << LSH)))
	
// ===  MACRO  ======================================================================
// 
//         Name:  HAZARD_WRITE_REG(address,value)
// 
//  Description:  writes val to addr
//				  Use hazard_write_16Bit_reg for 16Bit Registers               
// 
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:  32Bit\16Bit      address           address to access
//        in:  32Bit\16Bit		value             value to write      
//    in-out:  
//       out:  
//    return:  
// =====================================================================================
#define HAZARD_WRITE_REG(address,value) (*((volatile unsigned long *)(address)) = (value))

#define HAZARD_WRITE_16BIT_REG(address,value) (*((volatile unsigned short *)(address)) = (value))


#endif /*HARDWAREMACROS_H_*/



