/*=================================================================================
 $Source: AesEncryption.cpp $
 
  Class implementation for AES block cipher (FIPS 197).  This uses source from the
  FIPS 140-2 certified Open SSL library (version 1.1.1).  The license is commercial
  use friendly, but does require us to place the license text in documentation.  
  See the file LICENSE.txt for details.

  Although the source came from FIPS certified sources, this implementation did
  not inlude the self tests mandated by FIPS 140-2.  In other words, this
  implementation is not FIPS 140-2 compliant.
===================================================================================
 $Date: 2008/11/21 13:56:38EST $
 $Revision: 1.2 $

===================================================================================*/
#include "AesEncryption.h"




CAesEncryption::CAesEncryption(void)
{
	m_AesKeyEncrypt.rounds = 0;
}


// See encryption.h for how this function works
bool CAesEncryption::SetKey(const unsigned char * pKey, int KeyLengthBytes)
{
	int status;
	int KeyLengthBits = KeyLengthBytes * 8;


	if ( (KeyLengthBits == 128) || (KeyLengthBits == 192) || (KeyLengthBits == 256) )
	{
		status = AES_set_encrypt_key(pKey, KeyLengthBits, &m_AesKeyEncrypt);
		if ( status ) return false;
		status = AES_set_decrypt_key(pKey, KeyLengthBits, &m_AesKeyDecrypt);
		if ( status ) return false;
	}
	else return false;

	return true;
}




// See encryption.h for how this function works
bool CAesEncryption::Encrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength)
{
	if ( ! (pInput && pOutput) ) return false;
	if ( InputLength != GetBlockSizeBytes() ) return false;
	if ( MaxOutputLength < InputLength ) return false;
	if ( ! m_AesKeyEncrypt.rounds ) return false;

	AES_encrypt(pInput, pOutput, &m_AesKeyEncrypt);

	return true;
}




// See encryption.h for how this function works
bool CAesEncryption::Decrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength)
{
	if ( ! (pInput && pOutput) ) return false;
	if ( InputLength != GetBlockSizeBytes() ) return false;
	if ( MaxOutputLength < InputLength ) return false;
	if ( ! m_AesKeyEncrypt.rounds ) return false;

	AES_decrypt(pInput, pOutput, &m_AesKeyDecrypt);

	return true;
}





