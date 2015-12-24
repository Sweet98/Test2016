/*=================================================================================
 $Source: AesEncryption.h $
 
  Class definition for AES block cipher (FIPS 197).  This uses source from the
  FIPS 140-2 certified Open SSL library (version 1.1.1).  The license is commercial
  use friendly, but does require us to place the license text in documentation.  
  See the file LICENSE.txt for details.

  Although the source came from FIPS certified sources, this implementation did
  not use the FIPS version with the self tests that are mandated by FIPS 140-2.  
  In other words, this implementation is not FIPS 140-2 compliant.
===================================================================================
 $Date: 2007/05/03 14:49:27EDT $
 $Revision: 1.2 $
  

===================================================================================
              Modification History
===================================================================================
 $Log: AesEncryption.h  $
 Revision 1.2 2007/05/03 14:49:27EDT RosettiJ 
 Updated #include files to reflect new src code organization.
 Revision 1.1 2007/05/03 14:24:40EDT RosettiJ 
 Initial revision
 Member added to project d:/MKS_Common/cryptography/aes/src/src.pj
 Revision 1.1 2007/05/03 14:13:40EDT RosettiJ 
 Initial revision
 Member added to project d:/MKSUsers/RosettiJ/zzTesting/cryptography/aes/aes/aes.pj
 Revision 1.2 2007/03/26 13:51:18EDT RosettiJ 
 Moved aes_OpenSsl.pj project up a level (by sharing the original subproject location).
 Revision 1.1 2007/03/26 12:41:06EDT RosettiJ 
 Initial revision
 Member added to project d:/MKS_Common/cryptography/aes_OpenSsl/aes_OpenSsl.pj
===================================================================================*/
#ifndef _AesEncryption_h_
#define _AesEncryption_h_

#include "encryption.h"
#include "aes.h"
#include "aes_locl.h"



class CAesEncryption : public CBlockCipher
{
public:


	CAesEncryption(void);
	virtual bool SetKey(const unsigned char * pKey, int KeyLengthBytes);
	virtual bool Encrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength);
	virtual bool Decrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength);

protected:
	AES_KEY m_AesKeyEncrypt;
	AES_KEY m_AesKeyDecrypt;

private:
};













#endif // #ifndef _AesEncryption_h_
