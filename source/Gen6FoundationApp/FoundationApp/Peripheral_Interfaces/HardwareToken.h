/*=================================================================================

//=================================================================================
   $RCSfile: Gen6FoundationApp/FoundationApp/Peripheral_Interfaces/HardwareToken.h $
   $Revision: 1.26 $
   $Date: 2011/11/18 17:13:03EST $

//=================================================================================

//=================================================================================*/

#ifndef HARDWARETOKEN_H_
#define HARDWARETOKEN_H_

//These tokens are listed here so the interface so can determine the correct population.
#define OEM_TOKEN_RS232_ONLY	0x22
#define OEM_TOKEN_HISPEED		0x23
#define OEM_TOKEN_FULLSPEED		0x24

class CHardwareToken
{
public:
	static void CreateHardwareToken(void);
	static void DestroyHardwareToken(void);
		
	//Base
	bool IAmHAP(void);
	bool IAmCCB(void);  // This is the actual MODEL (CCB01)
	bool IAmCCB02(void);  // This is the Industrial CCB
	bool IAmCCBType(void); // This is for any derivatives (similar behavior, i.e. Industrial version)
	// bool IAmChargeOnlyBase(void);  // There is no main interface, so there is no need to know actual MODEL (COB)
	bool IAmChargeOnlyBaseType(void); // This is for any derivatives (similar behavior, i.e. Industrial version)

	//Corded 
	bool IAmThreeLevelIllumination_NonOem(void); 
	bool IAmOEM(void);
	bool IAmOEMPrototype(void);
	bool IAmOEMJade(void);
	int InterfaceToken(void);
	
	//RF_Scanner
	bool IAmLowXmitPowerScanner(void);

	//Corded and RF_Scanner
	bool IAmTwoLevelIllumination(void);
	
	// All
	bool IAmIndustrial(void);

	//Vuquest 3310
	bool IAmVuquest3310(void);
	
	//Voyager1400
	bool IAmVoyager1400(void);

	//Youjie
	bool IAmYoujie4600(void);

	int GetHardwareToken(void) {return m_HWTValue;}

private:
	CHardwareToken();
    ~CHardwareToken(){};

	void InitHardwareToken(void);
		
	int m_HWTValue;

}; //endof class CHardwareToken

extern CHardwareToken *pCHardwareToken;
#define HardwareToken (*pCHardwareToken)

#endif /* HARDWARETOKEN_H_ */



