/*===================================================================
  See .cpp file for description 
=====================================================================
   $Source: Gen6FoundationApp/FoundationApp/RemRouter.h $
   $Date: 2011/04/13 15:08:49EDT $
   $Revision: 1.1 $


===================================================================*/
#ifndef _RemRouter_h_
#define _RemRouter_h_

#include "barcodedata.h"
#include "ioman.h"

#define HTAG_HDR_LEN	7 // Syn, Fe + 4 binary length + 0x0d
#define MSGGET_HDR_LEN	14 // "MSGGET" + 4 char len + 3 for Symbology IDs + 0x1d
#define DECHDR_LEN		(HTAG_HDR_LEN + MSGGET_HDR_LEN)


class CRemRouter
{
public:
	CRemRouter(int RemEnaVal) {m_RemEnaVal = RemEnaVal;}
	
	int GetWhichRouter(void) {return m_RemEnaVal;}
	virtual void RouteBarcodeLob(CBarcodeData * pBarcodeDataObject, const wchar_t * pBarcodeData, int BarcodeDataLength) = 0;
	virtual int GetBarcodeLobHeaderSize(void){return 0;}
//	virtual void RouteBarcodeRaw() = 0;
	
	virtual void RouteImageLob(int ImageFormat, const HSTRING * pCmd, const HSTRING * pImage) = 0;
//	virtual void RouteImageRaw() = 0;
	
protected:
	 int m_RemEnaVal;
};


class CRemRouterNull : public CRemRouter
{
public:
	CRemRouterNull(int RemEnaVal) : CRemRouter(RemEnaVal){};
	
	virtual void RouteBarcodeLob(CBarcodeData * pBarcodeDataObject, const wchar_t * pBarcodeData, int BarcodeDataLength) {pBarcodeDataObject->Free();}
//	virtual void RouteBarcodeRaw() {}
	
	virtual void RouteImageLob(int ImageFormat, const HSTRING * pCmd, const HSTRING * pImage) {}
//	virtual void RouteImageRaw() {}
};


class CRemRouterLob : public CRemRouterNull
{
public:
	CRemRouterLob(int RemEnaVal) : CRemRouterNull(RemEnaVal){};

	virtual void RouteBarcodeLob(CBarcodeData * pBarcodeDataObject, const wchar_t * pBarcodeData, int BarcodeDataLength);
	virtual int GetBarcodeLobHeaderSize(void){return DECHDR_LEN;}  // returns any packet overhead so subsequent CBarcodeData can sized to hold it
	virtual void RouteImageLob(int ImageFormat, const HSTRING * pCmd, const HSTRING * pImage);
};

/*  It turns out ES doesn't need this one yet, but since the code's written, we'll leave it commented out for now
class CRemRouterRaw : public CRemRouterNull
{
public:
	CRemRouterRaw(int RemEnaVal) : CRemRouterNull(RemEnaVal){};

	virtual void RouteBarcodeRaw() {};
	virtual void RouteImageRaw() {};
};
*/



class CRemRouterHandler
{
public:
	static void CreateRouter(void);
	static void OnChangedConfiguration(void);

protected:
	enum {RouterNull = 0, RouterLob, RouterRaw};
};



extern CRemRouter * g_pRemRouter;


#endif // endof #ifndef _RemRouter_h_


