/*=================================================================================
  class: CWedgeLabel

//=================================================================================
   $Source: interfaces_wedge/wedgelabel.h $
   $Date: 2011/08/24 11:21:40EDT $
   $Revision: 1.15 $



//=================================================================================*/

#ifndef WEDGE_LABEL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define WEDGE_LABEL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "WedgeThread.h"

#ifdef UNICODE_FOR_INTERFACES
#define WEDGE_DATATYPE	IO_DATATYPE_UNICODE
#else
#define WEDGE_DATATYPE	IO_DATATYPE_CP_SYS
#endif

//#define WEDGE_ESCAPE_CHARACTER			0xF001 	// is in the private use area
//#define SUB_KEY_NUMBER						0xF003	// Key number
//#define SUB_KEY_NUMBER_MAKE				0xF004	// Key number (make key)
//#define SUB_KEY_NUMBER_BREAK				0xF005	// key number (break key)
//#define SUB_SCANCODE						0xF006	// scancode (direct byte)
//#define SUB_DELAY_TIME						0xF007	// delay time in mSec
//#define SUB_CTRLASCII_MODE					0xF008
//#define STARTOF_FIX							0x01
//#define ENDOF_FIX							0x02



#define baseCWedgeLabel CWedgeThread
class CWedgeLabel : public baseCWedgeLabel
{
public:
	CWedgeLabel(CThreadedDriver *pDriver, const CWedgeTerminal *pTerminal);


	int SendLabel(void);
	void SendLabelThread(void);

protected:
	virtual void Add_CloseLabelCommands(void);

protected:
	void TranslateEscSequence(tichar_t cmd, tichar_t param1);
	void CheckEscSequence(CBarcodeData *pBarcode, size_t &Position);

	void InterfaceDelay(CBarcodeData *pBarcode, size_t &Position);

	void Add_PressCapsKey(void);
	void Add_PressNumLockKey(void);
	void Add_MakeShiftKey(void);

	void SendCommands(void);
	void BitBangWrite(unsigned int data, unsigned int extrainfo);
	void OpenKeyEmu(void);
	bool CheckKeyEmu(void);


	void WaitUntilLEDsValid(void);
	void TurnCapsLockOff(void);
	void OpenStyleShiftLock(void);
	void OpenStyleAutoCaps(void);
	void OpenStyleAutoCapsViaNumLock(void);
	void OpenStyleKeyEmu(void);
	void OpenLockStyles(void);
	void CloseLockStyles(void);

	//ewr-04771
	void OpenStyleKbdAltNum(void);
	void OpenKBDALTstyle(void);
	//end ewr-04771

	void DelayCommand(ULONG uSec);
	void MT_Driver_Sleep(long uSec);

	virtual MatrixError_t CheckKeyboardEmulation(void)=0;

#ifdef _SIMULATION	// these do help in qualifying the product
	unsigned int GeneralBitBang(unsigned int ucData, unsigned int exinfo);
	void WriteDelay(long delay);
	void WriteEndMsg(void);
	void WriteBadMsg(void);
	void WriteAnsiChar(unsigned int ansi);
	void WriteModifierStatus(void);
#endif
};

#endif /* WEDGE_LABEL__H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */




