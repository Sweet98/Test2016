/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       1/23/2009
//  Tools:      RVCT Build 739
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/PWM_Driver.h $
// $Revision: 1.4 $
// $Date: 2010/11/11 15:47:07EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  PWM_Driver.h
//
// 
//------------------------------------------------------------------------------
#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_

typedef enum {PWM1, PWM2, PWM3, PWM4} PWM_BASE;
typedef enum {PWM, CSPI1_SS0, GPIO_A, FEC_TX_CLK, GPIO_B, CONTRAST, GPIO_C} PWM_PIN_NAME;

// ===  Function  ======================================================================
//
//  Name:  PWM_init
//
//  Description:  Initializes the PWM Channel selected by the input base.  The PWM is 
//		disabled then reset.  The output pin is configured to use the PWM.  The clock 
//		source is set to ipg_clock_highfreq (PER Clock 10).  The Frequency of the PER 
//		Clock should already be set in the clock controller section based on the system 
//		requirements of what each PWM is used for.  The clock can be divided down using 
//		the Prescaler.  The PWM is enabled during debug mode.  All other fields are not 
//		changed from the default.		
//		
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//				PWM_PIN_NAME	Pin_Name		Default pin name the PWM is connected to.
//												PWM,CSPI1_SS0,GPIO_A,FEC_TX_CLK,GPIO_B,
//												CONTRAST,GPIO_C
//				short			Prescaler		Input to the Counter Clock Prescaler field 
//												which divides the clock before it goes to 
//												the counter
//    in-out:
//       out:
//    return:	bool							returns a 1 if successful, returns a 0 if 
//												there was a mismatch between the pin 
//												and PWM channel
// -------------------------------------------------------------------------------------

bool PWM_Init(PWM_BASE, PWM_PIN_NAME, short);

// ===  Function  ======================================================================
//
//  Name:  PWM_Start
//
//  Description:  Sets the PWM Enable field in the PWM Control Register to start the PWM.  
//		The PWM should be initialized, and the Period and Sample registers should be set 
//		before enabling the PWM.  The output is reconnected to the pint, because the Stop
//		function will disconnect it to allow the output to enter a low state faster then 
//		if it was connected. 
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//    in-out:
//       out:
//    return:	bool							returns a 1 when the function has finsihed
// -------------------------------------------------------------------------------------

bool PWM_Start(PWM_BASE);

// ===  Function  ======================================================================
//
//  Name:  PWM_Stop
//
//  Description:  Clears the PWM Enable field in the PWM Control Register to stop the PWM.
//					Disconnects the PWM output from the PIN, This allows the external line
//						to go Low faster, then if the output was connected.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//    in-out:
//       out:
//    return:	bool							returns a 1 when the function has finsihed
// -------------------------------------------------------------------------------------

bool PWM_Stop(PWM_BASE);


// ===  Function  ======================================================================
//
//         Name:  PWM_Period
//
//  Description:  Writes the period register.  The period is based off the following formula:
//		fPWMO = fPCLK / (PERIOD +2) fPWMO is the output frequency.  fPCLK is the input 
//		to the counter which is based of the PER Clock and Prescaler settings.  If the 
//		PWM is enabled the function disables it writes the register and then re-enables it.
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//				int				Period			Value to write into the period register
//    in-out:
//       out:
//    return:	bool							returns a 1 when the function has finsihed
// -------------------------------------------------------------------------------------

bool PWM_Period(PWM_BASE, int);

// ===  Function  ======================================================================
//
//         Name:  PWM_Sample
//
//  Description: Writes the Sample register.  The Sample register sets up the duty cycle for 
//		the PWM signal
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//				int				Sample			Value to write into the Sample register
//    in-out:
//       out:
//    return:	bool							returns a 1 when the function has finsihed
// -------------------------------------------------------------------------------------

bool PWM_Sample(PWM_BASE, int);

// ===  Function  ======================================================================
//
//         Name:  Read_PWM_Count
//
//  Description:  Reads the Sample register for a PWM Base.
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//    in-out:
//       out:
//    return:	int				Sample			Value of the Sample Register
// -------------------------------------------------------------------------------------

int Read_PWM_Count(PWM_BASE);

// ===  Function  ======================================================================
//
//         Name:  PWM_Count
//
//  Description:  Reads the count register for a PWM Base
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:	PWM_BASE		Base  			Tells the function which PWM channel to 
//												initialize.  The iMX25 has 4 PWM channels
//												PWM1,PWM2,PWM3,PWM4
//    in-out:
//       out:
//    return:	int				Count			Current count of the PWM
// -------------------------------------------------------------------------------------
int PWM_Count(PWM_BASE);

void ROV_Interrupt_Enable(PWM_BASE, bool ROVIState);
void Clear_ROV_Interrupt_Signal(PWM_BASE);
bool Read_ROV_Interrupt_Signal(PWM_BASE);
void CMP_Interrupt_Enable(PWM_BASE, bool CMPIState);
void Clear_CMP_Interrupt_Signal(PWM_BASE);
bool Read_CMP_Interrupt_Signal(PWM_BASE);

#endif /*PWM_DRIVER_H_*/

