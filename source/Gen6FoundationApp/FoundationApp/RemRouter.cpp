/*============================================================================================
  These classes are used to route data going out the main interface.  ES calls this the Line 
  of Business or LOB interface.  The routing is controlled by the REMENA Menu Setting.  This
  Menu Setting's permanent storage value cannot be changed from 0 (disabled).  The ReM 
  application will enable the routing after the device has booted.
  
  CRemRouter is an abstract base class and we instantiate the correct version of the CRemRouter 
  class depending on the REMENA setting.  If not enabled, a null, or empty class is instantiated.
  This avoids peppering the code with if statements.
  
  The CRemRouterHandler class is responsible for managing the CRemRouter classes.  It creates
  and, if necessary, destroys the classes.
  
  There was some confusion concerning the valid values for REMENA.  The ReM document refers to
  a value of 2 which means to route the data "raw", i.e. no formatting (it was designed for
  barcode data).  Later, we realized we didn't have to implement this value, but some of
  the code was already written.  The code remains (commented out) to make it easier to add
  this functionality when and if it's needed.
==============================================================================================
   $Source: Gen6FoundationApp/FoundationApp/RemRouter.cpp $
   $Date: 2011/04/28 10:28:29EDT $
   $Revision: 1.2 $


============================================================================================*/
#include <wchar.h>
#include <string.h>
#include <assert.h>
#include "RemRouter.h"
#include "menu.h"
#include "ReaderConfiguration.h"
#include "htagimage.h"

CRemRouter * g_pRemRouter = NULL;  // this will be set at startup, but just in case someone does something wrong some day, let's init to NULL


#define MAX_FORMAT_CHARS	(3 + 1) // nnnF.  We should never get beyone 999 different formats.
#define MAX_IMGSHP_HDR_LEN	(6 + MAX_FORMAT_CHARS + 1) // "040807" + nnnF + 0x1d.
#define MAX_IMGSHP_LEN		(HTAG_HDR_LEN + MAX_IMGSHP_HDR_LEN)






/*----------------------------------------------------------------------------------------
RouteBarcodeLob

  This routes the same barcode data sent out the Line Of Business (LOB) interface to the
  ReM interface.  The CBarcodeData is in UNICODE.  We send this object instead of trying to send
  the data because of the translations CIoMan does.  It's necessary the same barcode that goes
  out the LOB interface goes out the ReM interface.  The only exception to this is that if
  DECHDR is turned on (which means the barcode is going out with an HTAG header), we don't
  add another one.  In other words, we send HTAG followed by the barcode plus any prefix,
  suffix and any data formatting.

  Parameters:  
  				pBarcodeDataObject[in]:  CBarcodeData object that has barcode IDs.  It also
  				has enough capacity to store the DECHDR.  This was a tradeoff between coupling
  				this with manhhplproc and efficiently sending the data.
  				
  				pBarcodeData[in]:  barcode data
  				
  				BarcodeDataLength[in]:  length of barcode data

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CRemRouterLob::RouteBarcodeLob(CBarcodeData * pBarcodeDataObject, const wchar_t * pBarcodeData, int BarcodeDataLength)
{
	// Ensure the barcodeData does not have a DECHDR header already applied (from RF scanner)
	if( pBarcodeData[0] != 0x02 && pBarcodeData[1] != 0xFE &&
		NULL == wcsstr( pBarcodeData + 7, L"091200" ) )	// skip 7 characters because SYN FE length usually contains zeroes ending the string search early
	{
		wchar_t DecodeHeader[DECHDR_LEN + 1];
		DWORD HtagLen = MSGGET_HDR_LEN + BarcodeDataLength;
		swprintf(DecodeHeader, sizeof DecodeHeader, L"\x02\xfe%lc%lc%lc%lc\x0d091200%04d%lc%lc%lc\x1d",
    				HtagLen & 0xff, (HtagLen >> 8) & 0xff, (HtagLen >> 16) & 0xff, HtagLen >> 24,
    				BarcodeDataLength, pBarcodeDataObject->GetHHPId(), pBarcodeDataObject->GetAimId(), pBarcodeDataObject->GetAimModifier()
    				);

		pBarcodeDataObject->AddText(DecodeHeader, DECHDR_LEN);
	}
	// Add the barcode data to the object
	pBarcodeDataObject->AddText(pBarcodeData, BarcodeDataLength);
    
	theRemInterface.Write(pBarcodeDataObject);
}



/*----------------------------------------------------------------------------------------
RouteImageLob

  This routes the same image data sent out the Line Of Business (LOB) interface to the
  ReM interface.  The ReM document has no information about the format for this.  We
  decided to mimic the barcode data.  Thus, there should only be a single HTAG header
  before the image data.
  
  Note:  unlike RouteBarcodeLob(), we don't use snprintf() to build the HTAG header because
  it can't handle binary data (those > 0xf0 won't be placed in the buffer).

  Parameters:  
  				ImageFormat[in]:  The image format
  				
  				pCmd[in]:  first part of HTAG header, if any 
  				
  				pImage[in]:  image data (it includes some HTAG parameters as well, if
  				pCmd has HTAG).

  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CRemRouterLob::RouteImageLob(int ImageFormat, const HSTRING * pCmd, const HSTRING * pImage)
{
	if ( pCmd->Size == 0 )
	{	// The IMGSHP command used to ship the image must had a "0P" in it, so there's no HTAG for it.  Create a simple one.
		char ImageFormatStr[MAX_FORMAT_CHARS + 1];
		int ImageFormatLen;
		if ( ImageFormat == HTAG_IMAGE::END_HTAG_IMAGE_FORMATS ) ImageFormatLen = 0;
		else
		{
			snprintf(ImageFormatStr, MAX_FORMAT_CHARS, "%dF", ImageFormat);
			ImageFormatLen = strlen(ImageFormatStr);
		}
		
		unsigned char HtagHeader[MAX_IMGSHP_LEN];
		DWORD HtagLen = 6 + ImageFormatLen + 1 + pImage->Size;  // "IMGSHP" + ParamterStr + 0x1d + DataLen
		HtagHeader[0] = 0x02;
		HtagHeader[1] = 0xfe;
		HtagHeader[2] = HtagLen & 0xff;
		HtagHeader[3] = (HtagLen >> 8) & 0xff;
		HtagHeader[4] = (HtagLen >> 16) & 0xff;
		HtagHeader[5] = (HtagLen >> 24) & 0xff;
		HtagHeader[6] = 0x0d;
		memcpy(&HtagHeader[7], "040807", 6);
		int index = 7 + 6; // we now have a variable number of paramters, so use this index variable
		memcpy(&HtagHeader[index], ImageFormatStr, ImageFormatLen);  // These are the HTAG parameters (only one for now)
		index += ImageFormatLen;
		HtagHeader[index] = 0x1d;
		assert(index <= sizeof HtagHeader);
		
		theRemInterface.Write(HtagHeader, index + 1);
	}
	else theRemInterface.Write(pCmd);
	theRemInterface.Write(pImage);
}





/*----------------------------------------------------------------------------------------
CreateRouter

  This creates the appropriate CRemRouter depending on the REMENA Menu Setting.  Note:
  this is ALL it does--it does NOT destroy it (it's up to the caller to destroy first).

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CRemRouterHandler::CreateRouter(void)
{
	int RemEnaVal = GET_SETTING(RemRouting);
	switch ( RemEnaVal )
	{
		default: // just in case--should never happen
			assert(0);
		case RouterNull:
			g_pRemRouter = new CRemRouterNull(RemEnaVal);
		break;
		
		case RouterLob:
			g_pRemRouter = new CRemRouterLob(RemEnaVal);
		break;
		
/*  It turns out ES doesn't need this one yet, but since the code's written, we'll leave it commented out for now
		case RouterRaw:
			g_pRemRouter = new CRemRouterRaw(RemEnaVal);
		break;
*/
	}
}

/*----------------------------------------------------------------------------------------
OnChangedConfiguration

  If REMENA changes, this will delete the old CRemRouter and create a new one.  There should
  be no thread safety issues here, even though barcode routing and this method happen in
  different threads.  There's no thread context switches in CRemRouter's methods, so we don't
  have to worry about this changing while the old CRemRouter has partially sent barcode data.
  
  By design, we don't flush any outgoing ReM data when we change routing, even if it's
  disabled.  Note:  the ReM document doesn't cover this.

  Parameters:  none

  Returns:  nothing
----------------------------------------------------------------------------------------*/
void CRemRouterHandler::OnChangedConfiguration(void)
{
	if ( g_pRemRouter->GetWhichRouter() != GET_SETTING(RemRouting) )
	{
		delete g_pRemRouter;
		CreateRouter();
	}
}


