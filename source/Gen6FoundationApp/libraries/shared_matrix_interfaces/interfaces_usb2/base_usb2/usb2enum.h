/* ==============================================================================
   USB enumeration declarations
   ==============================================================================

   $RCSfile: interfaces_usb2/base_usb2/usb2enum.h $
   $Revision: 1.11 $
   $Date: 2011/03/08 15:23:52EST $

//=================================================================================

//=================================================================================*/

#ifndef USBENUM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define USBENUM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

#include "Usb2Chip.h"
#include "usb2.h"

#define baseCUsb2Enum CUsb2Chip

class CUsb2Enum : public baseCUsb2Enum
{
public:
	CUsb2Enum(CRxFifo *pRxFifo, CThreadRealTime *pThread);
	virtual ~CUsb2Enum();
	
private:

// For chapter 9
protected:
	virtual void stall_ep0(void);
	virtual void stall_eps(UCHAR bEndp, UCHAR bStalled);
	void code_transmit(const UCHAR *pRomData, USHORT len);
	void get_status(void);
	void clear_feature(void);
	void set_feature(void);
	void set_address(void);
	void get_descriptor(void);
	void get_configuration(void);
	void set_configuration(void);
	void get_interface(void);
	void set_interface(void);
	virtual void control_handler(void);
	
	virtual void OnClassRequest(void){};
	virtual void OnVendorRequest(void){};	
			
private:
	UCHAR bInterface_num;	
};


#endif	// USB2ENUM_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

  
