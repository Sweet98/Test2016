/*===================================================================
  CHash

  Base class for hashing functions.  Source came from
  MKS_TT/TT8800/shared/HashBase/HashBase.pj
 
=====================================================================
   $Source: hash.h $
   $Date: 2011/10/14 14:53:55EDT $
   $Revision: 1.2 $


===================================================================*/
#ifndef _hash_h_
#define _hash_h_



class CHash
{
public:
	static const int HashLength = 0;  //NOTE:  update this to the number of bytes in the hash so the caller an create appropriately sized arrays
	
	virtual ~CHash(void) {};
	virtual void DoHash(const unsigned char * pBuffer, unsigned int length, unsigned char * pHash) = 0;  // Does a hash on the supplied data

	// These functions are used to hash a stream of data peices at a time
	void virtual Init(void) = 0; // First call
	virtual void Update(const unsigned char * pBuffer, unsigned int length) = 0;  // Call repeatedly on streaming data
	virtual void GetResult(unsigned char * pHash) = 0;  // Gets the result.
};


#endif // endof #ifndef _hash_h_


