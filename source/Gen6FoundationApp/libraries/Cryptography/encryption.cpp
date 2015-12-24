/*=================================================================================
 $Source: encryption.cpp $
 
 Class implementation for encryption/decryption.
===================================================================================
 $Date: 2010/08/10 13:20:43EDT $
 $Revision: 1.3 $

===================================================================================*/
#include <stdlib.h>
#include <string.h>

#include "encryption.h"




/*----------------------------------------------------------------------------------------
EcbEncryptAddPadding

  Implements ECB mode of encryption for block ciphers and does the padding, if necessary.
  The padding is all 0s.

  Parameters:  
		pInput[in]:  pointer to input (plaintext) data

		InputLength[in]:  input data length.  It does not have to be a multiple of
							the encryption algorithm's block size

		pOutput[out]:  pointer to output (encrypted) data

		MaxOutputLength[in]:  max size for the encrypted data

		pOutputLength[in]:  actual size of the encrypted data (including any padding bytes)


  Returns:  true if OK, false otherwise
----------------------------------------------------------------------------------------*/
bool CBlockCipher::EcbEncryptAddPadding(const unsigned char * pInput, size_t InputLength,
										unsigned char * pOutput, size_t MaxOutputLength, size_t * pOutputLength)
{
	const int BlockSize = GetBlockSizeBytes();
	unsigned char * pPaddedBlock;
	size_t NumToEncrypt;
	int OddNumLeft;
	bool bIsOk;



	if ( ! (pInput && pOutput) ) return false;
	if ( MaxOutputLength < (InputLength + (BlockSize - InputLength % BlockSize)) ) return false;

	pPaddedBlock = (unsigned char *) malloc(BlockSize);
	if ( ! pPaddedBlock ) return false;
	OddNumLeft = InputLength % BlockSize;
	NumToEncrypt = InputLength - OddNumLeft;
	bIsOk = EcbEncrypt(pInput, NumToEncrypt, pOutput, NumToEncrypt);
	if ( bIsOk && OddNumLeft )
	{	// It's the last block of input data and it's too small--pad it.
		pInput += NumToEncrypt;
		pOutput += NumToEncrypt;
		memcpy(pPaddedBlock, pInput, OddNumLeft);
		memset(pPaddedBlock + OddNumLeft, 0, BlockSize - OddNumLeft);
		bIsOk = Encrypt(pPaddedBlock, BlockSize, pOutput, BlockSize);
	}

	*pOutputLength = InputLength;
	if ( OddNumLeft ) *pOutputLength += BlockSize - OddNumLeft;
	free(pPaddedBlock);
	return bIsOk;
}




/*----------------------------------------------------------------------------------------
EcbEncrypt

  Implements ECB mode of encryption for block ciphers WITHOUT padding.  The input data
  length must be a multiple of the algorithm's block size.

  Parameters:  
		pInput[in]:  pointer to input (plaintext) data

		InputLength[in]:  input data length.  It must be a multiple of
							the encryption algorithm's block size

		pOutput[out]:  pointer to output (encrypted) data

		MaxOutputLength[in]:  max size for the encrypted data


  Returns:  true if OK, false otherwise
----------------------------------------------------------------------------------------*/
bool CBlockCipher::EcbEncrypt(const unsigned char * pInput, size_t InputLength,
								unsigned char * pOutput, size_t MaxOutputLength)
{
	const int BlockSize = GetBlockSizeBytes();
	bool bIsOk = true;


	if ( ! (pInput && pOutput) ) return false;
	if ( (InputLength % BlockSize) != 0 ) return false;
	if ( MaxOutputLength < InputLength ) return false;

	while ( bIsOk && InputLength )
	{
		///AES_encrypt(pInput, pOutput, pKey);
		bIsOk = Encrypt(pInput, BlockSize, pOutput, BlockSize);
		pInput += BlockSize;
		pOutput += BlockSize;
		InputLength -= BlockSize;
	}

	return bIsOk;
}




/*----------------------------------------------------------------------------------------
EcbDecrypt

  Implements ECB mode of decryption for block ciphers WITHOUT padding.  The input data
  length must be a multiple of the algorithm's block size.

  Parameters:  
		pInput[in]:  pointer to input (encrypted) data

		InputLength[in]:  input data length.  It must be a multiple of
							the encryption algorithm's block size

		pOutput[out]:  pointer to output (plaintext) data

		MaxOutputLength[in]:  max size for the plaintext data


  Returns:  true if OK, false otherwise
----------------------------------------------------------------------------------------*/
bool CBlockCipher::EcbDecrypt(const unsigned char * pInput, size_t InputLength,
								unsigned char * pOutput, size_t MaxOutputLength)
{
	const int BlockSize = GetBlockSizeBytes();
	bool bIsOk = true;


	if ( ! (pInput && pOutput) ) return false;
	if ( (InputLength % BlockSize) != 0 ) return false;
	if ( MaxOutputLength < InputLength ) return false;

	while ( bIsOk && InputLength )
	{
		bIsOk = Decrypt(pInput, BlockSize, pOutput, BlockSize);
		pInput += BlockSize;
		pOutput += BlockSize;
		InputLength -= BlockSize;
	}

	return bIsOk;
}




