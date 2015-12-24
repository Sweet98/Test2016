/*=================================================================================
  class: CParseHelper

//=================================================================================
   $Source: interfaces/parsehelper.h $
   $Date: 2003/11/10 14:35:21EST $
   $Revision: 1.8 $


//=================================================================================*/

#ifndef PARSE_HELPER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9
#define PARSE_HELPER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9

class CParseHelper
{
public:
//	static TCHAR *Find (tichar_t *pIn, tichar_t *pSearch);
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, volatile bool &Result);
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, volatile int &Result);
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, volatile ULONG &Result);

	static bool Extract(const tichar_t *pIn, int delimiter, int index, volatile int &Result);
	static bool Extract(const tichar_t *pIn, int delimiter, int index, const tichar_t *&pFound);
	static bool Extract(const tichar_t *pIn, int index, volatile int &Result);
	static bool Extract(const tichar_t *pIn, int index, const tichar_t *&pFound);

// some inlines to help reduce the writing effort
//	static bool Extract(const tichar_t *pIn, const schar_t *pSearch, volatile bool &Result);
//
//	static bool Extract(const tichar_t *pIn, const schar_t *pSearch, volatile int &Result)
//	{
//		return Extract(pIn, (const tichar_t*) pSearch, Result);
//	}
//
//	static bool Extract(const tichar_t *pIn, const schar_t *pSearch, volatile ULONG &Result)
//	{
//		return Extract(pIn, (const tichar_t*) pSearch, Result);
//	}

	static bool Extract(const tichar_t *pIn, int index, volatile unsigned long &Result)
	{
		return Extract(pIn, index, (int &)Result);
	}

	#ifdef UNICODE_FOR_INTERFACES
	// this is used by the SerialBase.cpp
	static bool Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile bool &Result);
	static bool Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile ULONG &Result);
	static bool Extract(const bchar_t *pIn, const bchar_t *pSearch, volatile int &Result);
	#endif
protected:
};

/*
 How to use it:
	This class contains helpers to extract parameters in binary form out of a (ASCII) string.
	The parameter strings are similar to the ones we used in the IT3800.

	The helper functions all work very similar from the user point if view. You probably need one of these:
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, bool &Result);
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, int &Result);
	static bool Extract(const tichar_t *pIn, const tichar_t *pSearch, ULONG &Result);

	pIn is the parameter string
	pSearch is the part of the string that is leading the parameter.
	The number that follows the search string (in the parameter string) is returned in Result.

	Return value is true if the extract is successfully.

	Example:
		from serial parameters, we want to read the ACK/NACK mode.

		int m_AckNakEnabled;
 		....
		bool Retval = CParseHelper::Extract(pParameters, "ACK", m_AckNakEnabled);

		pParameters  contains:   xxxxxxxACK0xxxxxxxxxxxxxxx
	Now the function searches for "ACK", then it reads the integer that follows the "ACK" (in our case it's "ACK0", so
	it's 0). m_AckNakEnabled is passed as a reference, so the function does directly write to it (0 in this case).

	So we don't care if that variable is a bool, int or ULONG, the compiler will select the correct overload.
	There are also overloads that take a 'char* pSearch' instread of tichar_t (is a UCHAR right now).

	That really is it.

	Another example of a parameter string:
	UCHAR *pParameters =
	"Country00,Style0,AutoDC0,CtrlAsc0,Turbo0,NumPad0,FcnCodeXmit1,"
	"IC_X_DLY00,DelayChar0,ICDLY0,IFDLY0,IMDLY25,DeadK0,AltNp0,Html0,Hex0";

	And we extract inter message delay (25 in the example):
		int IntermessageDelay;
 		....
		bool Retval = CParseHelper::Extract(pParameters, "IMDLY", IntermessageDelay);
		// now IntermessageDelay contains 25.


	If you need to read more difficult stuff, we add a new function to this class.

*/


#endif /* PARSE_HELPER_H_F3E1EF42_35D6_11D2_93BE_00805F2987D9 */





