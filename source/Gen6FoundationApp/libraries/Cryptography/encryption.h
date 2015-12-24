/*=================================================================================
 $Source: encryption.h $
 
  Class definitions for encryption/decryption.
===================================================================================
 $Date: 2010/04/23 10:05:01EDT $
 $Revision: 1.2 $

===================================================================================*/
#ifndef _encryption_h_
#define _encryption_h_

#include <stdio.h>


/*----------------------------------------------------------------------------------------
CSymmetricKeyCipher

  Abstract base class for symmetric key ciphers (stream and block).



  SetKey() is used to set the key for both encryption and decryption.
	Parameters:  
			pKey[in]:  pointer to key data

			KeyLengthBytes[in]:  the size of key data, in bytes


	Returns:  true if OK, false otherwise



  Encrypt() encrypts data.
	Parameters:  
			pInput[in]:  pointer to data to encrypt

			InputLength[in]:  number of bytes of data.

			pOutput[out]:  pointer to resulting encryped data

			MaxOutputLength[in]:  max number of bytes pOutput points to

	Returns:  true if OK, false otherwise



  Decrypt() decrypts a single block of data.
	Parameters:  
			pInput[in]:  pointer to data to decrypt

			InputLength[in]:  number of bytes of data.

			pOutput[out]:  pointer to resulting decryped data

			MaxOutputLength[in]:  max number of bytes pOutput points to


	Returns:  true if OK, false otherwise
----------------------------------------------------------------------------------------*/
class CSymmetricKeyCipher
{
public:

	virtual bool SetKey(const unsigned char * pKey, int KeyLengthBytes) = 0;
	virtual bool Encrypt(const unsigned char * pInput, size_t InputLength,
						unsigned char * pOutput, size_t MaxOutputLength) = 0;
	virtual bool Decrypt(const unsigned char * pInput, size_t InputLength,
						unsigned char * pOutput, size_t MaxOutputLength) = 0;

protected:
	int BlockSizeBytes;

private:
};


/*----------------------------------------------------------------------------------------
CBlockCipher

  Base class for symmetric key block ciphers.  See implementation for description of
  each function.
----------------------------------------------------------------------------------------*/
class CBlockCipher : public CSymmetricKeyCipher
{
public:

	virtual size_t GetBlockSizeBytes(void) {return 16;} // Many algorithms operate on a block size of 16 bytes
	virtual bool EcbEncryptAddPadding(const unsigned char * pInput, size_t InputLength,
										unsigned char * pOutput, size_t MaxOutputLength, size_t * pOutputLength);
	virtual bool EcbEncrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength);
	virtual bool EcbDecrypt(const unsigned char * pInput, size_t InputLength,
							unsigned char * pOutput, size_t MaxOutputLength);

protected:

private:
};





#endif // #ifndef _encryption_h_
