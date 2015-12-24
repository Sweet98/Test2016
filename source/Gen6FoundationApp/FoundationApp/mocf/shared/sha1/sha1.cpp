/*===================================================================
  This is a wrapper for a SHA1 implementaion.  The SHA1 implementation
  is from RFC4634.  You can calculate a hash two ways.  If you have
  all the data, you can call DoHash().  If the data is streaming in
  and/or you with to calculate the hash on pieces of the data at a
  time, you:
	- Call Init()
	- Call Update() on each piece of data
	- Call GetResult() to get the final hash

  This code must compile under these OSs:  Microsoft, Linux, Gen6 embedded
=====================================================================
   $Source: sha1.cpp $
   $Date: 2010/01/11 13:45:36EST $
   $Revision: 1.1 $


===================================================================*/
#include <assert.h>
#include "sha1.h"




/*----------------------------------------------------------------------------------------
DoHash

  Calculates the hash of the input data.

  Parameters: 
		pBuffer[in]:  data to hash

		length[in]:  length of data to hash

		pHash[out]:  resulting hash.  It is up to the caller to ensure enough space
		             exists for the hash (use CSha1::HashLength).


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CSha1::DoHash(const unsigned char * pBuffer, unsigned int length, unsigned char * pHash)
{
	Init();
	Update(pBuffer, length);
	GetResult(pHash);
}


/*----------------------------------------------------------------------------------------
Init

  Prepares to calculate a hash pieces at a time.  This internally resets the state of the
  hashing algorithm.

  Parameters: none


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CSha1::Init(void)
{
	SHA1Reset(&m_Sha1Context);
}



/*----------------------------------------------------------------------------------------
Update

  This calculates the hash of a piece of input data.

  Parameters: 
		pBuffer[in]:  data to hash

		length[in]:  length of data to hash


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CSha1::Update(const unsigned char * pBuffer, unsigned int length)
{
	assert(pBuffer);
	assert(! m_Sha1Context.Computed);  // GetResult() was already called--need to call Init() again.

	SHA1Input(&m_Sha1Context, pBuffer, length);
}



/*----------------------------------------------------------------------------------------
GetResult

  This gets the results of the streaming hash calculations.  This internally finalizes state
  of the hashing algorithm.

  Parameters: 
		pHash[out]:  resulting hash.  It is up to the caller to ensure enough space
		             exists for the hash (use CSha1::HashLength).


  Returns:  nothing.
----------------------------------------------------------------------------------------*/
void CSha1::GetResult(unsigned char * pHash)
{
	SHA1Result(&m_Sha1Context, pHash);
}




#ifdef INCLUDE_TEST
#include <string.h>

/*----------------------------------------------------------------------------------------
RunTests

  This performs a series of tests to validate the hashing algorithm.  Most of the test
  cases came from RFC4634.  The rest came from "The Secure Hash Algorithm Validation 
  System (SHAVS)".

  Parameters: none


  Returns:  true if all tests passed, false otherwise.
----------------------------------------------------------------------------------------*/
bool CSha1::RunTests(void)
{
// These are from RFC4634
#define TEST1    "abc"	// Simple, with padding
#define TEST2_1	\
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"	// most amount of padding (creates 2 blocks)
#define TEST8_1 \
  "\x9a\x7d\xfd\xf1\xec\xea\xd0\x6e\xd6\x46\xaa\x55\xfe\x75\x71\x46"
#define TEST3    "a"                            /* times 1000000 */
#define TEST4a   "01234567012345670123456701234567"
#define TEST4b   "01234567012345670123456701234567"
#define TEST4   TEST4a TEST4b                   /* times 10, an exact multiple of 512 bits */
#define TEST8_1 \
  "\x9a\x7d\xfd\xf1\xec\xea\xd0\x6e\xd6\x46\xaa\x55\xfe\x75\x71\x46"
#define TEST10_1 \
  "\xf7\x8f\x92\x14\x1b\xcd\x17\x0a\xe8\x9b\x4f\xba\x15\xa1\xd5\x9f" \
  "\x3f\xd8\x4d\x22\x3c\x92\x51\xbd\xac\xbb\xae\x61\xd0\x5e\xd1\x15" \
  "\xa0\x6a\x7c\xe1\x17\xb7\xbe\xea\xd2\x44\x21\xde\xd9\xc3\x25\x92" \
  "\xbd\x57\xed\xea\xe3\x9c\x39\xfa\x1f\xe8\x94\x6a\x84\xd0\xcf\x1f" \
  "\x7b\xee\xad\x17\x13\xe2\xe0\x95\x98\x97\x34\x7f\x67\xc8\x0b\x04" \
  "\x00\xc2\x09\x81\x5d\x6b\x10\xa6\x83\x83\x6f\xd5\x56\x2a\x56\xca" \
  "\xb1\xa2\x8e\x81\xb6\x57\x66\x54\x63\x1c\xf1\x65\x66\xb8\x6e\x3b" \
  "\x33\xa1\x08\xb0\x53\x07\xc0\x0a\xff\x14\xa7\x68\xed\x73\x50\x60" \
  "\x6a\x0f\x85\xe6\xa9\x1d\x39\x6f\x5b\x5c\xbe\x57\x7f\x9b\x38\x80" \
  "\x7c\x7d\x52\x3d\x6d\x79\x2f\x6e\xbc\x24\xa4\xec\xf2\xb3\xa4\x27" \
  "\xcd\xbb\xfb"
// These are from "The Secure Hash Algorithm Validation System (SHAVS)"
#define TEST_SHAVS_2	"\x5e"


	const unsigned char Output_1[HashLength] = {0xA9, 0x99, 0x3E, 0x36, 0x47, 0x06, 0x81, 0x6A, 0xBA, 0x3E, 0x25, 0x71, 0x78, 0x50, 0xC2, 0x6C, 0x9C, 0xD0, 0xD8, 0x9D};
	const unsigned char Output_2_1[HashLength] = {0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E, 0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5, 0xE5, 0x46, 0x70, 0xF1};
	const unsigned char Output_3[HashLength] = {0x34, 0xAA, 0x97, 0x3C, 0xD4, 0xC4, 0xDA, 0xA4, 0xF6, 0x1E, 0xEB, 0x2B, 0xDB, 0xAD, 0x27, 0x31, 0x65, 0x34, 0x01, 0x6F};
	const unsigned char Output_4[HashLength] = {0xDE, 0xA3, 0x56, 0xA2, 0xCD, 0xDD, 0x90, 0xC7, 0xA7, 0xEC, 0xED, 0xC5, 0xEB, 0xB5, 0x63, 0x93, 0x4F, 0x46, 0x04, 0x52};
	const unsigned char Output_8[HashLength] = {0x82, 0xAB, 0xFF, 0x66, 0x05, 0xDB, 0xE1, 0xC1, 0x7D, 0xEF, 0x12, 0xA3, 0x94, 0xFA, 0x22, 0xA8, 0x2B, 0x54, 0x4A, 0x35};
	const unsigned char Output_10[HashLength] = {0xCB, 0x00, 0x82, 0xC8, 0xF1, 0x97, 0xD2, 0x60, 0x99, 0x1B, 0xA6, 0xA4, 0x60, 0xE7, 0x6E, 0x20, 0x2B, 0xAD, 0x27, 0xB3};

	const unsigned char Output_Shavs_2[HashLength] = {0x5e, 0x6f, 0x80, 0xa3, 0x4a, 0x97, 0x98, 0xca, 0xfc, 0x6a, 0x5d, 0xb9, 0x6c, 0xc5, 0x7b, 0xa4, 0xc4, 0xdb, 0x59, 0xc2};


	if ( ! TestHash((const unsigned char *) TEST1, sizeof TEST1 - 1, 1, Output_1) ) return false;
	if ( ! TestHash((const unsigned char *) TEST2_1, sizeof TEST2_1 - 1, 1, Output_2_1) ) return false;
	if ( ! TestHash((const unsigned char *) TEST3, sizeof TEST3 - 1, 1000000, Output_3) ) return false;
	if ( ! TestHash((const unsigned char *) TEST4, sizeof TEST4 - 1, 10, Output_4) ) return false;
	if ( ! TestHash((const unsigned char *) TEST8_1, sizeof TEST8_1 - 1, 1, Output_8) ) return false;
	if ( ! TestHash((const unsigned char *) TEST10_1, sizeof TEST10_1 - 1, 1, Output_10) ) return false;

	if ( ! TestHash((const unsigned char *) TEST_SHAVS_2, sizeof TEST_SHAVS_2 - 1, 1, Output_Shavs_2) ) return false;

	return true;
}


/*----------------------------------------------------------------------------------------
TestHash

  Helper method for RunTests().  It hashes a block of data one or more times.

  Parameters: 
		pBuffer[in]:  data to hash

		InputLength[in]:  length of data to hash

		NumTimes[in]:  number of times to hash thedata

		pHash[out]:  resulting hash.  It is up to the caller to ensure enough space
		             exists for the hash (use CSha1::HashLength).




  Returns:  true if all tests passed, false otherwise.
----------------------------------------------------------------------------------------*/
bool CSha1::TestHash(const unsigned char * pBuffer, int length, int NumTimes, const unsigned char * pHash)
{
	unsigned char TheHash[HashLength];

	Init();
	for (int i = 0; i < NumTimes; i++) Update(pBuffer, length);
	GetResult(TheHash);
	if ( memcmp(TheHash, pHash, HashLength) != 0 ) return false;

	return true;
}

#endif // endof #ifdef INCLUDE_TEST



