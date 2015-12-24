/**************************************************************************************/
//
// HSM 48X0G GENERATION 6 SOFTWARE
//
//  Date:       10/20/2010
//  Tools:      RVCT Build 862
//  Target:     Freescale iMX25
//
//======================================================================================
// Revision information
//======================================================================================
// $RCSfile: Gen6FoundationApp/FoundationApp/Plugin/PluginGpioInterface.h $
// $Revision: 1.3 $
// $Date: 2010/11/26 19:40:49EST $
//======================================================================================

//======================================================================================
//------------------------------------------------------------------------------
// HEADER:  PluginGpioInterface.h
// 
//------------------------------------------------------------------------------
#ifndef PLUGINGPIOINTERFACE_H_
#define PLUGINGPIOINTERFACE_H_

bool DoesPluginHaveControlofUART1_RXD(void);
bool DoesPluginHaveControlofUART1_TXD(void);
bool DoesPluginHaveControlofUART1_RTS(void);
bool DoesPluginHaveControlofUART1_CTS(void);
bool DoesPluginHaveControlofNFRB(void);
bool DoesPluginHaveControlofCSPI1_SS0(void);
bool DoesPluginHaveControlofCSPI1_SS1(void);
bool DoesPluginHaveControlofKPP_ROW1(void);
bool DoesPluginHaveControlofKPP_COL2(void);
bool DoesPluginHaveControlofKPP_ROW0(void);

class CPluginGpio
{
public:
	static void CreatePluginGpio(void);
	static void DestroyPluginGpio(void);
		
	int GpioInit(int GpioPins, int ReadWrite);
	int GpioRead(void);
	int GpioWrite(int GpioPins, int WriteData);

protected:

	    
private:
	CPluginGpio();
	virtual ~CPluginGpio(){};
	bool PluginHasGpioControl(int);
	
	int m_PluginGpioControlStatus;
}; //endof class CHardwareToken

extern CPluginGpio *pCPluginGpio;
#define PluginGpio (*pCPluginGpio)


class COemPluginGpio // : public CPluginGpio
{
public:
	static void CreateOemPluginGpio(void);
	static void DestroyOemPluginGpio(void);
	
	void TakeControlofGPIO0(bool);
	void TakeControlofGPIO1(bool);
	void TakeControlofGPIO2(bool);
	void TakeControlofGPIO3(bool);
	void TakeControlofGPIO4(bool);
	void TakeControlofGPIO5(bool);
	void TakeControlofGPIO6(bool);
	void TakeControlofGPIO7(bool);
	void TakeControlofGPIO8(bool);

	void ReturnControlofGPIO0(void);
	void ReturnControlofGPIO1(void);
	void ReturnControlofGPIO2(void);
	void ReturnControlofGPIO3(void);
	void ReturnControlofGPIO4(void);
	void ReturnControlofGPIO5(void);
	void ReturnControlofGPIO6(void);
	void ReturnControlofGPIO7(void);
	void ReturnControlofGPIO8(void);
	
	void WriteGPIO0(bool);
	void WriteGPIO1(bool);
	void WriteGPIO2(bool);
	void WriteGPIO3(bool);
	void WriteGPIO4(bool);
	void WriteGPIO5(bool);
	void WriteGPIO6(bool);
	void WriteGPIO7(bool);
	void WriteGPIO8(bool);
	
	bool ReadGPIO0(void);
	bool ReadGPIO1(void);
	bool ReadGPIO2(void);
	bool ReadGPIO3(void);
	bool ReadGPIO4(void);
	bool ReadGPIO5(void);
	bool ReadGPIO6(void);
	bool ReadGPIO7(void);
	bool ReadGPIO8(void);
	
	int ReportNumberofGpio(void);

	bool m_PluginHasControlofUART1_RXD;
	bool m_PluginHasControlofUART1_TXD;
	bool m_PluginHasControlofUART1_RTS;
	bool m_PluginHasControlofUART1_CTS;
	bool m_PluginHasControlofNFRB;
	bool m_PluginHasControlofCSPI1_SS0;
	bool m_PluginHasControlofCSPI1_SS1;
	bool m_PluginHasControlofKPP_ROW1;
	bool m_PluginHasControlofKPP_COL2;
	bool m_PluginHasControlofKPP_ROW0;
	
private:
	COemPluginGpio();
	
	int m_SavedMuxGPIO0;
	int m_SavedMuxGPIO1;
	int m_SavedMuxGPIO2;
	int m_SavedMuxGPIO3;
	int m_SavedMuxGPIO4;
	int m_SavedMuxGPIO5;
	int m_SavedMuxGPIO6;
	int m_SavedMuxGPIO7;
	int m_SavedMuxGPIO8Out;
	int m_SavedMuxGPIO8In;
	
	int m_SavedPadGPIO0;
	int m_SavedPadGPIO1;
	int m_SavedPadGPIO2;
	int m_SavedPadGPIO3;
	int m_SavedPadGPIO4;
	int m_SavedPadGPIO5;
	int m_SavedPadGPIO6;
	int m_SavedPadGPIO7;
	int m_SavedPadGPIO8Out;
	int m_SavedPadGPIO8In;
	
	bool m_SavedDirGPIO0;
	bool m_SavedDirGPIO1;
	bool m_SavedDirGPIO2;
	bool m_SavedDirGPIO3;
	bool m_SavedDirGPIO4;
	bool m_SavedDirGPIO5;
	bool m_SavedDirGPIO6;
	bool m_SavedDirGPIO7;
	bool m_SavedDirGPIO8Out;
	bool m_SavedDirGPIO8In;
	
	bool m_SavedDatGPIO0;
	bool m_SavedDatGPIO1;
	bool m_SavedDatGPIO2;
	bool m_SavedDatGPIO3;
	bool m_SavedDatGPIO4;
	bool m_SavedDatGPIO5;
	bool m_SavedDatGPIO6;
	bool m_SavedDatGPIO7;
	bool m_SavedDatGPIO8Out;
	bool m_SavedDatGPIO8In;

	int m_OEMNumberOfGpioSupported;
	
}; //endof class CHardwareToken

extern COemPluginGpio *pCOemPluginGpio;
#define OemPluginGpio (*pCOemPluginGpio)
#endif /*PLUGINGPIOINTERFACE_H_*/

