/*===================================================================

=====================================================================
   $Source: sha1.h $
   $Date: 2010/01/11 13:45:36EST $
   $Revision: 1.1 $


===================================================================*/
// linux, embedded, MS
#ifndef _sha1_h_
#define _sha1_h_

#include "hash.h"
#include "sha1_Rfc4634.h"

#define INCLUDE_TEST  // define this if you want to include the methods used to test the algorithm.


class CSha1 : public CHash
{
public:
	static const int HashLength = 20;

	virtual void DoHash(const unsigned char * pBuffer, unsigned int length, unsigned char * pHash);  // Does a hash on the supplied data

	// These functions are used to hash a stream of data peices at a time
	void virtual Init(void); // First call
	virtual void Update(const unsigned char * pBuffer, unsigned int length);  // Call repeatedly on streaming data
	virtual void GetResult(unsigned char * pHash);  // Gets the result.

protected:
	SHA1Context m_Sha1Context;


#ifdef INCLUDE_TEST
public:
	bool RunTests(void);
protected:
	bool TestHash(const unsigned char * pInput, int InputLength, int NumTimes, const unsigned char * pHash);
#endif // endof #ifdef INCLUDE_TEST
};


#endif // endof #ifndef _sha1_h_



