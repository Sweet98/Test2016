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
// $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/PWM_Driver.cpp $
// $Revision: 1.8 $
// $Date: 2010/11/11 16:28:17EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// File:  PWM_Driver.cpp
//
//  The Clock Controller should setup the FEQ of the PER Clock based on the board it is.
//  Each Modual (Laser, Beeper, LEDs) should come up with a prescaler to pass into the PWM Driver
//  PWR Clock will only be enabled when it is needed.
//------------------------------------------------------------------------------
#if PLATFORM_CURRENT == imx25
	#include "PWM_Driver.h"
	#include "HardwareMacros.h"
	#include "iMX25_PWM.h"
	#include "iMX25_IOMUX.h"
	#include "iMX25_CRM.h"
	#include "iMX25_GPIO.h"
#else
	#error The PWM Driver is not used for this chip
#endif

#define PWM1_ROV_INT_EN(state)		do{WRITE_FIELD_TO_REG(PWM1_PWMIR,GPIO_BIT1_LSH,GPIO_BIT1_WID,state);}while(0)	//Enable/Disable the Roll-over Interrupt
#define PWM1_CLEAR_ROV_INT_SIG()	*((volatile unsigned long *)(PWM1_PWMSR)) = ((unsigned long)0x00000001 << (4)) 	//Clears the Roll-over Interrupt Status Register Flag
#define PWM1_READ_ROV_INT_SIG()		(READ_FIELD(PWM1_PWMSR,GPIO_BIT4_LSH,GPIO_BIT4_WID))							//Reads the Roll-over Interrupt Status Register Flag
#define PWM1_CMP_INT_EN(state)		do{WRITE_FIELD_TO_REG(PWM1_PWMIR,GPIO_BIT2_LSH,GPIO_BIT1_WID,state);}while(0)	//Enable/Disable the Compare Interrupt
#define PWM1_CLEAR_CMP_INT_SIG()	*((volatile unsigned long *)(PWM1_PWMSR)) = ((unsigned long)0x00000001 << (5)) 	//Clears the Compare Interrupt Status Register Flag
#define PWM1_READ_CMP_INT_SIG()		(READ_FIELD(PWM1_PWMSR,GPIO_BIT5_LSH,GPIO_BIT5_WID))							//Reads the Compare Interrupt Status Register Flag

#define PWM2_ROV_INT_EN(state)		do{WRITE_FIELD_TO_REG(PWM2_PWMIR,GPIO_BIT1_LSH,GPIO_BIT1_WID,state);}while(0)	//Enable/Disable the Roll-over Interrupt
#define PWM2_CLEAR_ROV_INT_SIG()	*((volatile unsigned long *)(PWM2_PWMSR)) = ((unsigned long)0x00000001 << (4)) 	//Clears the Roll-over Interrupt Status Register Flag
#define PWM2_READ_ROV_INT_SIG()		(READ_FIELD(PWM2_PWMSR,GPIO_BIT4_LSH,GPIO_BIT4_WID))							//Reads the Roll-over Interrupt Status Register Flag
#define PWM2_CMP_INT_EN(state)		do{WRITE_FIELD_TO_REG(PWM2_PWMIR,GPIO_BIT2_LSH,GPIO_BIT1_WID,state);}while(0)	//Enable/Disable the Compare Interrupt
#define PWM2_CLEAR_CMP_INT_SIG()	*((volatile unsigned long *)(PWM2_PWMSR)) = ((unsigned long)0x00000001 << (5)) 	//Clears the Compare Interrupt Status Register Flag
#define PWM2_READ_CMP_INT_SIG()		(READ_FIELD(PWM2_PWMSR,GPIO_BIT5_LSH,GPIO_BIT5_WID))							//Reads the Compare Interrupt Status Register Flag


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
bool PWM_Init(PWM_BASE Base, PWM_PIN_NAME Pin_Name, short Prescaler)
{
#if PLATFORM_CURRENT == imx25
	int Control_Register;
	//Setup Control Register
	Control_Register = (FIELD_MASK(PWM_CR_DBGEN_LSH,PWM_CR_DBGEN_WID,0x1)|  //ENABLE PWM in DEBUG Mode
	 					//FIELD_MASK(PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x3)|  //Disconnect the PWM Output
	                    FIELD_MASK(PWM_CR_CLKSRC_LSH,PWM_CR_CLKSRC_WID,0x2)|  //Clock Source Set to ipg_clock_highfreq
	                    FIELD_MASK(PWM_CR_WAITEN_LSH,PWM_CR_WAITEN_WID,0x1)|  //Wait Mode Enable
	                    FIELD_MASK(PWM_CR_PRESCALER_LSH,PWM_CR_PRESCALER_WID,Prescaler)); //Set teh Prescaler
	switch (Base)
		{
			case PWM3:
				//Init Clock
				WRITE_FIELD_TO_REG(CRM_CGR2,CRM_CGCR2_IPG_CLK_PWM3_LSH,CRM_CGCR2_IPG_CLK_PWM3_WID,0x1); //Turn On IPG CLK
				if (READ_FIELD(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID) == 0x0) //Check to see if the PWM PER Clock is ON
				{
					//If not turn it on.  the CLOCK Controller should have already set the Frequency.
					WRITE_FIELD_TO_REG(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID,0x1);  
				}
				
				//Disable and Reset the PWM
				WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0); //Diable the PWM
				WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_SWR_LSH,PWM_CR_SWR_WID,0x1); //RESET the PWM
				
				//Configure Output Pin
				if(Pin_Name == FEC_TX_CLK)
				{
					WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_FEC_TX_CLK,MUX_MODE_1);	//Set Pad Mux for PWM
					WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_FEC_TX_CLK,0x00000007); //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else if(Pin_Name == GPIO_B)
				{
					WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_B,MUX_MODE_1);	//Set Pad Mux for PWM
					WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_B,0x00000007); //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else
				{
					return 0;
				}//endof Pin_Name Switch
				
				//Write the Control Register
				WRITE_REG(PWM3_PWMCR,Control_Register);
				break;
				
			case PWM4:
				//Init Clock
				WRITE_FIELD_TO_REG(CRM_CGR2,CRM_CGCR2_IPG_CLK_PWM4_LSH,CRM_CGCR2_IPG_CLK_PWM4_WID,0x1); //Turn On IPG CLK
				if (READ_FIELD(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID) == 0x0) //Check to see if the PWM PER Clock is ON
				{
					//If not turn it on.  the CLOCK Controller should have already set the Frequency.
					WRITE_FIELD_TO_REG(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID,0x1);  
				}
				
				//Disable and Reset the PWM
				WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   //Diable the PWM
				WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_SWR_LSH,PWM_CR_SWR_WID,0x1); //RESET the PWM
				
				//Configure Output Pin
				if (Pin_Name == CONTRAST)
				{
					WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CONTRAST,MUX_MODE_4);	//Set Pad Mux for PWM
					WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CONTRAST,0x00000007);   //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else if(Pin_Name == GPIO_C)
				{
					WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_C,MUX_MODE_1); 	//Set Pad Mux for PWM
					WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_C,0x00000007);     //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else
				{
					return 0;
				}
				
				//Write the Control Register
				WRITE_REG(PWM4_PWMCR,Control_Register);
				break;
				
			case PWM1:
				//Init Clock
				WRITE_FIELD_TO_REG(CRM_CGR1,CRM_CGCR1_IPG_CLK_PWM1_LSH,CRM_CGCR1_IPG_CLK_PWM1_WID,1); //Turn On IPG CLK
				if (READ_FIELD(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID) == 0x0) //Check to see if the PWM PER Clock is ON
				{
					//If not turn it on.  the CLOCK Controller should have already set the Frequency.
					WRITE_FIELD_TO_REG(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID,0x1);  
				}
				
				//Disable and Reset the PWM
				WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   //Diable the PWM
				WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_SWR_LSH,PWM_CR_SWR_WID,0x1); //RESET the PWM
				
				//Configure Output Pin
				if(Pin_Name == PWM)
				{
					WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_PWM,MUX_MODE_0);	//Set Pad Mux for PWM
					WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_PWM,0x00000007);    //Set Pad Control, MAX Drive, Fast Slew Rate 
				}
				else
				{
					return 0;
				}
				
				//Write the Control Register
				WRITE_REG(PWM1_PWMCR,Control_Register);
				break;
				
			case PWM2:
				//Init Clock
				WRITE_FIELD_TO_REG(CRM_CGR2,CRM_CGCR2_IPG_CLK_PWM2_LSH,CRM_CGCR2_IPG_CLK_PWM2_WID,0x1); //Turn On IPG CLK
				if (READ_FIELD(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID) == 0x0) //Check to see if the PWM PER Clock is ON
				{
					//If not turn it on.  the CLOCK Controller should have already set the Frequency.
					WRITE_FIELD_TO_REG(CRM_CGR0,CRM_CGCR0_IPG_PER_10_PWM_LSH,CRM_CGCR0_IPG_PER_10_PWM_WID,0x1);  
				}
				
				//Disable and Reset the PWM
				WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   //Diable the PWM
				WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_SWR_LSH,PWM_CR_SWR_WID,0x1); //RESET the PWM
				
				//Configure Output Pin
				if (Pin_Name == CSPI1_SS0)
				{
						WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_CSPI1_SS0,MUX_MODE_2);	//Set Pad Mux for PWM
						WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_CSPI1_SS0,0x00000007);   //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else if (Pin_Name == GPIO_A)
				{
						WRITE_REG(IOMUXC_SW_MUX_CTL_PAD_GPIO_A,MUX_MODE_1); 	//Set Pad Mux for PWM
						WRITE_REG(IOMUXC_SW_PAD_CTL_PAD_GPIO_A,0x00000007);     //Set Pad Control, MAX Drive, Fast Slew Rate
				}
				else
				{
					return 0;
				}
				
				//Write the Control Register
				WRITE_REG(PWM2_PWMCR,Control_Register);
				
				break;
		}//endof Base Switch
		return 1;
#endif
}//endof PWM_Init

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
bool PWM_Start(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	DISABLE_INTERRUPTS();
	switch (Base)
	{
		case PWM3:
			HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x0);   //Set the Output pin to be set at rollover and cleared at comparison
			HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);  		//Enable the PWM
			break;
		case PWM4:
			HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x0);
			HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);
			break;
		case PWM1:
			HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x0);
			HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);
			break;
		case PWM2:
			HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x0);
			HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);
			break;
	}//endof Base Switch
	RESTORE_INTERRUPTS();
	return 1;
#endif
}//endof PWM_Start

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
bool PWM_Stop(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	DISABLE_INTERRUPTS();
	switch (Base)
	{
		case PWM3:
			HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x2);   //Disconnect the PWM Output
			HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   		//Disable the PWM
			break;
		case PWM4:
			HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x2);
			HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);
			break;
		case PWM1:
			HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x2);
			HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);
			break;
		case PWM2:
			HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_POUTC_LSH,PWM_CR_POUTC_WID,0x2);
			HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);
			break;
	}//endof Base Switch
	RESTORE_INTERRUPTS();
	return 1;
#endif
}//endof PWM_Stop

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
bool PWM_Period(PWM_BASE Base, int Period)
{
#if PLATFORM_CURRENT == imx25
	DISABLE_INTERRUPTS();
	switch (Base)
	{
		case PWM3:
			if(READ_FIELD(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)	//If the PWM is running the turn it off before writing this register
			{
				HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   	//Disable the PWM
				HAZARD_WRITE_REG(PWM3_PWMPR,Period);   									//Write the Period Register
				HAZARD_WRITE_FIELD_TO_REG(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);   	//Enable the PWM
			}
			else
			{
				HAZARD_WRITE_REG(PWM3_PWMPR,Period);   //Write the Period Register
			}
			break;
		case PWM4:
			if(READ_FIELD(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)		//If the PWM is running the turn it off before writing this register
			{
				HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   	//Disable the PWM
				HAZARD_WRITE_REG(PWM4_PWMPR,Period);   									//Write the Period Register
				HAZARD_WRITE_FIELD_TO_REG(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);   	//Enable the PWM
			}
			else
			{
				HAZARD_WRITE_REG(PWM4_PWMPR,Period);   //Write the Period Register
			}
			break;
		case PWM1:
			if(READ_FIELD(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)		//If the PWM is running the turn it off before writing this register
			{
				HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   	//Disable the PWM
				HAZARD_WRITE_REG(PWM1_PWMPR,Period);   									//Write the Period Register
				HAZARD_WRITE_FIELD_TO_REG(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);   	//Enable the PWM
			}
			else
			{
				HAZARD_WRITE_REG(PWM1_PWMPR,Period);   //Write the Period Register
			}
			break;
		case PWM2:
			if(READ_FIELD(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)		//If the PWM is running the turn it off before writing this register
			{
				HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x0);   	//Disable the PWM
				HAZARD_WRITE_REG(PWM2_PWMPR,Period);   									//Write the Period Register
				HAZARD_WRITE_FIELD_TO_REG(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID,0x1);   	//Enable the PWM
			}
			else
			{
				HAZARD_WRITE_REG(PWM2_PWMPR,Period);   //Write the Period Register
			}
			break;
	}//endof Base Switch
	RESTORE_INTERRUPTS();
	return 1;
#endif
}//endof PWM_Period

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
bool PWM_Sample(PWM_BASE Base, int Sample)
{
#if PLATFORM_CURRENT == imx25
	
	DISABLE_INTERRUPTS();
	
	switch (Base)
	{
		case PWM3:
			if(READ_FIELD(PWM3_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)		//If the PWM is running the turn it off before writing this register
			{
				PWM_Stop(PWM3); 						//Stop the PWM, also disconect the output
				HAZARD_WRITE_REG(PWM3_PWMSAR,Sample);   //Write the Sample Register
				PWM_Start(PWM3);						//Start the PWM, and reconnect the output
			}
			else
			{
				HAZARD_WRITE_REG(PWM3_PWMSAR,Sample);   //Write the Sample Register
			}
			break;
		case PWM4:
			if(READ_FIELD(PWM4_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)
			{
				PWM_Stop(PWM4); 						
				HAZARD_WRITE_REG(PWM4_PWMSAR,Sample);
				PWM_Start(PWM4);
			}
			else
			{
				HAZARD_WRITE_REG(PWM4_PWMSAR,Sample);
			}
			break;
		case PWM1:
			if(READ_FIELD(PWM1_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)
			{
				PWM_Stop(PWM1); 						
				HAZARD_WRITE_REG(PWM1_PWMSAR,Sample);
				PWM_Start(PWM1);
			}
			else
			{
				HAZARD_WRITE_REG(PWM1_PWMSAR,Sample);
			}
			break;
		case PWM2:
			if(READ_FIELD(PWM2_PWMCR,PWM_CR_EN_LSH,PWM_CR_EN_WID) == 0x1)
			{
				PWM_Stop(PWM2); 						
				HAZARD_WRITE_REG(PWM2_PWMSAR,Sample);
				PWM_Start(PWM2);
			}
			else
			{
				HAZARD_WRITE_REG(PWM2_PWMSAR,Sample);
			}
			break;
	}//endof Base Switch
	
	RESTORE_INTERRUPTS();
		
	return 1;
#endif
}//endof PWM_Sample

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
int Read_PWM_Count(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	switch (Base)
	{
		case PWM3:
			return READ_REG(PWM3_PWMSAR);
		case PWM4:
			return READ_REG(PWM4_PWMSAR);
		case PWM1:
			return READ_REG(PWM1_PWMSAR);
		case PWM2:
			return READ_REG(PWM2_PWMSAR);
	}//endof Base Switch
#endif
	return 0;
}//endof Read_PWM_Count

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
int PWM_Count(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	switch (Base)
	{
		case PWM3:
			return READ_REG(PWM3_PWMCNR);   //Read the Counter Register
		case PWM4:
			return READ_REG(PWM4_PWMCNR);   //Read the Counter Register
		case PWM1:
			return READ_REG(PWM1_PWMCNR);   //Read the Counter Register
		case PWM2:
			return READ_REG(PWM2_PWMCNR);   //Read the Counter Register
	}//endof Base Switch
#endif
	return 0;
}//endof PWM_Count

// ===  Function  ======================================================================
//
//         Name:  ROV_Interrupt_Enable
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void ROV_Interrupt_Enable(PWM_BASE Base, bool ROVIState)
{
#if PLATFORM_CURRENT == imx25
	switch (Base)
	{
		case PWM1:
			PWM1_ROV_INT_EN(ROVIState);
			break;
		case PWM2:
			PWM2_ROV_INT_EN(ROVIState);
			break;
		case PWM3:
		case PWM4:
			break;
	}//endof Base Switch
#endif
}//endof ROV_Interrupt_Enable

// ===  Function  ======================================================================
//
//         Name:  Clear_ROV_Interrupt_Signal
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Clear_ROV_Interrupt_Signal(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	switch (Base)
	{
		case PWM1:
			PWM1_CLEAR_ROV_INT_SIG();
			break;
		case PWM2:
			PWM2_CLEAR_ROV_INT_SIG();
			break;
		case PWM3:
		case PWM4:
			break;
	}//endof Base Switch
#endif
}//endof Clear_ROV_Interrupt_Signal

// ===  Function  ======================================================================
//
//         Name:  Read_ROV_Interrupt_Signal
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
bool Read_ROV_Interrupt_Signal(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	bool ret = false;
	switch (Base)
	{
		case PWM1:
			ret = PWM1_READ_ROV_INT_SIG();
			break;
		case PWM2:
			ret = PWM2_READ_ROV_INT_SIG();
			break;
		case PWM3:
		case PWM4:
			ret = 0;
			break;
	}//endof Base Switch
	return ret;
#endif
}//endof Read_ROV_Interrupt_Signal

// ===  Function  ======================================================================
//
//         Name:  CMP_Interrupt_Enable
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void CMP_Interrupt_Enable(PWM_BASE Base, bool CMPIState)
{
#if PLATFORM_CURRENT == imx25
	
	switch (Base)
	{
		case PWM1:
			PWM1_CMP_INT_EN(CMPIState);
			break;
		case PWM2:
			PWM2_CMP_INT_EN(CMPIState);
			break;
		case PWM3:
		case PWM4:
			break;
	}//endof Base Switch
#endif
}//endof CMP_Interrupt_Enable

// ===  Function  ======================================================================
//
//         Name:  Clear_CMP_Interrupt_Signal
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
void Clear_CMP_Interrupt_Signal(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	
	switch (Base)
	{
		case PWM1:
			PWM1_CLEAR_CMP_INT_SIG();
			break;
		case PWM2:
			PWM2_CLEAR_CMP_INT_SIG();
			break;
		case PWM3:
		case PWM4:
			break;
	}//endof Base Switch
#endif
}//endof Clear_CMP_Interrupt_Signal

// ===  Function  ======================================================================
//
//         Name:  Read_CMP_Interrupt_Signal
//
//  Description:
//
//
// - PARAMETER -------------------------------------------------------------------------
//      Mode   Type             Name            Description
// -------------------------------------------------------------------------------------
//        in:
//    in-out:
//       out:
//    return:
// -------------------------------------------------------------------------------------
bool Read_CMP_Interrupt_Signal(PWM_BASE Base)
{
#if PLATFORM_CURRENT == imx25
	bool ret = false; 
	switch (Base)
	{
		case PWM1:
			ret = PWM1_READ_CMP_INT_SIG();
			break;
		case PWM2:
			ret = PWM2_READ_CMP_INT_SIG();
			break;
		case PWM3:
		case PWM4:
			ret = 0;
			break;
	}//endof Base Switch
	return ret;
#endif
}//endof Read_CMP_Interrupt_Signal

