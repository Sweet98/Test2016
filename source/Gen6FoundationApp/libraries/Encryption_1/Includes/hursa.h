/*
 * Copyright © 1996-2011 Certicom Corp. All rights reserved.
 * 
 * This software contains trade secrets, confidential information, and other
 * intellectual property of Certicom Corp. and its licensors. This software
 * cannot be used, reproduced, or distributed in whole or in part by any means
 * without the explicit prior consent of Certicom Corp. Such consent must arise
 * from a separate license agreement from Certicom or its licensees, as 
 * appropriate.
 * 
 * Certicom, Certicom AMS, ACC, Asset Control Core, Certicom Bar Code
 * Authentication Agent, Certicom ECC Core, Certicom Security Architecture,
 * Certicom Trusted Infrastructure, Certicom CodeSign, Certicom KeyInject,
 * ChipActivate, DieMax, Security Builder, Security Builder API, 
 * Security Builder API for .NET, Security Builder BSP, Security Builder Crypto,
 * Security Builder ETS, Security Builder GSE, Security Builder IPSec, Security
 * Builder MCE, Security Builder NSE, Security Builder PKI, Security Builder SSL
 * and SysActivate are trademarks or registered trademarks of Certicom Corp.
 * All other companies and products listed herein are trademarks or registered
 * trademarks of their respective holders.
 *
 * BlackBerry®, RIM®, Research In Motion® and related trademarks are owned by
 * Research In Motion Limited. Used under license.
 * 
 * Certicom Corp. has intellectual property rights relating to technology
 * embodied in the product that is described in this document. In particular,
 * and without limitation, these intellectual property rights may include one or
 * more of the U.S. and non-U.S. patents listed at www.certicom.com/patents and
 * one or more additional patents or pending patent applications in the U.S. and
 * in other countries. Information subject to change.
 *
 */
 * $Source: Includes/hursa.h $
 * $Name:  $
 */
#ifndef HURSA_H
#define HURSA_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_rsa_api RSA APIs
 *
 *@{
 */

/** Creates an RSA parameters object for the specified modulus size.

An RNG context must be supplied if key generation or encryption will be
performed. A yielding context must be supplied if yielding will be performed.

@param modulusSize [Input]  The length (in bits) of the RSA modulus. This value
                            must be greater than or equal to 512.
@param rngContext  [Input]  An RNG context. (Optional - set to <tt>NULL</tt> if
                            key generation and encryption will not be 
			    performed.)
@param yieldCtx    [Input]  A yield context. (Optional - set to <tt>NULL</tt> if
                            yielding is not required.)
@param rsaParams   [Output] The RSA parameters object pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_BAD_INPUT       The modulus size is invalid.
@retval SB_ERR_NULL_PARAMS_PTR The <tt>rsaParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAParamsCreate(
    size_t modulusSize,
    sb_RNGCtx rngContext,
    sb_YieldCtx yieldCtx,
    sb_Params *rsaParams,
    sb_GlobalCtx sbCtx
);


/** Retrieves settings from an RSA parameters object.

@param rsaParams   [Input]  RSA parameters object.
@param modulusSize [Output] The length (in bits) of the RSA modulus.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_PARAMS The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS  
@retval SB_ERR_NULL_OUTPUT The output buffer is <tt>NULL</tt>.
@retval SB_SUCCESS         Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAParamsGet(
    sb_Params rsaParams,
    size_t *modulusSize,
    sb_GlobalCtx sbCtx
);


/** Destroys an RSA parameters object.

@param rsaParams [Input/Output] The RSA parameters object pointer.
@param sbCtx     [Input]        A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>rsaParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_NULL_PARAMS     The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS      
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAParamsDestroy(
    sb_Params *rsaParams,
    sb_GlobalCtx sbCtx
);


/** Creates an RSA private and/or public key object from the given key
value(s).

A private key object can be created by supplying one of the following
combinations of key data:

<ul>
<li><tt>p</tt>, <tt>q</tt>, <tt>d</tt>
<li><tt>p</tt>, <tt>q</tt>, <tt>d mod p-1</tt>,
<tt>d mod q-1</tt>
<li><tt>n</tt>, <tt>d</tt>, <tt>e</tt>
<li><tt>n</tt>, <tt>d</tt>
</ul>

Private key objects will always be stored with the CRT coefficients. If any
of these values are not supplied, they will be computed from the available
data subject to the following:

<ul>
<li>If <tt>p</tt> and <tt>q</tt> are supplied, either
<tt>d mod p-1</tt> and <tt>d mod q-1</tt>, or <tt>d</tt>
must be supplied. If all three are passed, <tt>d</tt> will be ignored,
and no check is done to ensure that the values are self-consistent.
<li>If <tt>p</tt>, <tt>q</tt> and <tt>qInvModP</tt> are supplied,
then no check is done to ensure that the values are self-consistent.
<li>If <tt>n</tt> and <tt>d</tt> are supplied, but <tt>e</tt>
is not supplied, this function will assume a default value of
65537 (0x10001) as the public exponent.
</ul>

A public key object can be created by supplying <tt>n</tt> and,
optionally, <tt>e</tt>. If <tt>e</tt> is not supplied, this function
will assume a default value of 65537 (0x10001) as the public exponent.

@param rsaParams  [Input]  An RSA parameters object.
@param eLen       [Input]  The length (in bytes) of the public exponent. Must 
                           be less than or equal to <tt>nLen</tt>.
@param e          [Input]  The public exponent.
@param nLen       [Input]  The length (in bytes) of the modulus.
@param n          [Input]  The modulus.
@param dLen       [Input]  The length (in bytes) of the private exponent. Must 
                           be less than or equal to <tt>nLen</tt>.
@param d          [Input]  The private exponent.
@param pLen       [Input]  The length (in bytes) of the first prime p. Must be 
                           less than or equal to <tt>nLen</tt>.
@param p          [Input]  First large prime factor of the modulus.
@param qLen       [Input]  The length (in bytes) of the second prime p. Must 
                           be less than or equal to <tt>nLen</tt>.
@param q          [Input]  Second large prime factor of the modulus.
@param dModPLen   [Input]  The length (in bytes) of the d mod p-1 CRT 
                           coefficient. Must be equal to <tt>pLen</tt>.
@param dModPm1    [Input]  d mod p-1 CRT coefficient.
@param dModQLen   [Input]  The length (in bytes) of the d mod q-1 CRT 
                           coefficient. Must be equal to <tt>qLen</tt>.
@param dModQm1    [Input]  d mod q-1 CRT coefficient.
@param qInvLen    [Input]  The length (in bytes) of q inverse mod p CRT 
                           coefficient.
@param qInvModP   [Input]  q inverse mod p CRT coefficient.
@param privateKey [Output] The private key object pointer.
@param publicKey  [Output] The public key object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS   The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS    
@retval SB_ERR_NULL_OUTPUT   Both the private and public key object pointers 
                             are <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT     Not enough key data supplied to create a key.
@retval SB_ERR_BAD_RSA_E_LEN The length of the public exponent is invalid.
@retval SB_FAIL_ALLOC        Memory allocation failure.
@retval SB_SUCCESS           Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAKeySet(
    sb_Params rsaParams,
    size_t eLen,
    const unsigned char *e,
    size_t nLen,
    const unsigned char *n,
    size_t dLen,
    const unsigned char *d,
    size_t pLen,
    const unsigned char *p,
    size_t qLen,
    const unsigned char *q,
    size_t dModPLen,
    const unsigned char *dModPm1,
    size_t dModQLen,
    const unsigned char *dModQm1,
    size_t qInvLen,
    const unsigned char *qInvModP,
    sb_PrivateKey *privateKey,
    sb_PublicKey *publicKey,
    sb_GlobalCtx sbCtx
);


/** Creates an RSA private and public key object from random data.

If the public exponent is not supplied (i.e. set to <tt>NULL</tt>),
this function will assume a default value of 65537 (0x10001).

@param rsaParams  [Input]  An RSA parameters object.
@param eLen       [Input]  The length (in bytes) of the public exponent. 
                           (Optional)
@param e          [Input]  The public exponent. (Optional - set to 
                           <tt>NULL</tt> if the default value should be used.)
@param privateKey [Output] The private key object pointer.
@param publicKey  [Output] The public key object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS   The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS    
@retval SB_ERR_NULL_OUTPUT   Both the private and public key object pointers 
                             are <tt>NULL</tt>.
@retval SB_ERR_BAD_RSA_E_LEN The length of the public exponent is invalid.
@retval SB_FAIL_ALLOC        Memory allocation failure.
@retval SB_SUCCESS           Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAKeyGen(
    sb_Params rsaParams,
    size_t eLen,
    const unsigned char *e,
    sb_PrivateKey *privateKey,
    sb_PublicKey *publicKey,
    sb_GlobalCtx sbCtx
);


/** Retrieves the key values and their lengths from an RSA private
and/or public key object.

The following values and their lengths can be retrieved from an RSA private
key: <tt>n</tt>, <tt>d</tt>, <tt>p</tt>, <tt>q</tt>, <tt>d
mod p-1</tt>, <tt>d mod q-1</tt>, <tt>q inverse mod p</tt>.

The following values and their lengths can be retrieved from an RSA public
key: <tt>n</tt>, <tt>e</tt>.

If the prime factors <tt>p</tt> and <tt>q</tt> were generated or
computed by the library, this function will return the primes such that
<tt>p</tt> is numerically greater or equal to <tt>q</tt>. Otherwise,
if these primes were supplied during key setting, this function will
return them as given.

If the length of a key data value is known, a pointer to a buffer large
enough to hold the key data value should be passed in the appropriate
argument and its length in the corresponding length argument. This function
will copy the value into the buffer and set the actual length of the value in
the length argument.

If a key data value argument is <tt>NULL</tt>, or the argument is not
<tt>NULL</tt> but the corresponding length argument is too small, this
function will set the correct length of the key data value in the length
argument.

Set both the parameter argument and its length to <tt>NULL</tt> for
any parameters that are to be ignored.

@param rsaParams  [Input]        An RSA parameters object.
@param privateKey [Input]        An RSA private key object.
@param publicKey  [Input]        An RSA public key object.
@param eLen       [Input/Output] The length (in bytes) of the public exponent.
@param e          [Output]       The public exponent.
@param nLen       [Input/Output] The length (in bytes) of the modulus.
@param n          [Output]       Modulus.
@param dLen       [Input/Output] The length (in bytes) of the private exponent.
                                 (This is the same as <tt>nLen</tt>.)
@param d          [Output]       The private exponent.
@param pLen       [Input/Output] The length (in bytes) of the first prime p.
@param p          [Output]       First large prime factor of the modulus.
@param qLen       [Input/Output] The length (in bytes) of the second prime q.
@param q          [Output]       Second large prime factor of the modulus.
@param dModPLen   [Input/Output] The length (in bytes) of the d mod p-1 CRT 
                                 coefficient. (This is the same as 
				 <tt>pLen</tt>.)
@param dModPm1    [Output]       d mod p-1 CRT coefficient.
@param dModQLen   [Input/Output] The length (in bytes) of the d mod q-1 CRT 
                                 coefficient. (This is the same as 
				 <tt>qLen</tt>.)
@param dModQm1    [Output]       d mod q-1 CRT coefficient.
@param qInvLen    [Input/Output] The length (in bytes) of q inverse mod p CRT 
                                 coefficient. (This is the same as 
				 <tt>pLen</tt>.)
@param qInvModP   [Output]       q inverse mod p CRT coefficient.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS            The <tt>rsaParams</tt> object is 
                                      <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS             
@retval SB_ERR_NULL_INPUT             Both the private key and public key 
                                      objects are <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY        The private key object is invalid.
@retval SB_ERR_BAD_PUBLIC_KEY         The public key object is invalid.
@retval SB_ERR_PRI_KEY_NOT_EXPORTABLE The private exponent (d) is not 
                                      exportable.
@retval SB_ERR_RSA_CRT_NOT_AVAILABLE  The CRT components (p, q, d mod p-1, 
                                      d mod q-1, q inverse mod p) cannot be 
				      retrieved.
@retval SB_SUCCESS                    Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAKeyGet(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    sb_PublicKey publicKey,
    size_t *eLen,
    unsigned char *e,
    size_t *nLen,
    unsigned char *n,
    size_t *dLen,
    unsigned char *d,
    size_t *pLen,
    unsigned char *p,
    size_t *qLen,
    unsigned char *q,
    size_t *dModPLen,
    unsigned char *dModPm1,
    size_t *dModQLen,
    unsigned char *dModQm1,
    size_t *qInvLen,
    unsigned char *qInvModP,
    sb_GlobalCtx sbCtx
);


/** Destroys an RSA private and/or public key object.

@param rsaParams  [Input]        An RSA parameters object.
@param privateKey [Input/Output] The private key object pointer.
@param publicKey  [Input/Output] The public key object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS       
@retval SB_ERR_NULL_PRIVATE_KEY The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY  The private key object is invalid.
@retval SB_ERR_NULL_PUBLIC_KEY  The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY   The public key object is invalid.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAKeyDestroy(
    sb_Params rsaParams,
    sb_PrivateKey *privateKey,
    sb_PublicKey  *publicKey,
    sb_GlobalCtx sbCtx
);


/** Produces an output block by applying the basic RSA primitive on an
input block using a public key.

The mathematical operation performed is raising the input value
to the public exponent modulo the modulus.

The input value, treated as the octet string encoding of an integer, must be
numerically smaller than the modulus.

The <tt>input</tt> and <tt>output</tt> buffers may point to the
same address.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams [Input]  An RSA parameters object.
@param publicKey [Input]  An RSA public key object.
@param input     [Input]  The input buffer. This must be equal to the modulus 
                          length.
@param output    [Output] The output buffer. This must be equal to the modulus 
                          length.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS     The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS      
@retval SB_ERR_NULL_PUBLIC_KEY The <tt>publicKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY  The <tt>publicKey</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF  The <tt>input</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF The <tt>output</tt> buffer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPublicEncrypt(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    const unsigned char * input,
    unsigned char * output,
    sb_GlobalCtx sbCtx
);


/** Produces an output block by applying the basic RSA primitive on an
input block using a private key.

The mathematical operation performed is raising the input value
to the private exponent modulo the modulus.

The input value, treated as the octet string encoding of an integer, must be
numerically smaller than the modulus.

The <tt>input</tt> and <tt>output</tt> buffers may point to the
same address.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams  [Input]  An RSA parameters object.
@param privateKey [Input]  An RSA private key object.
@param input      [Input]  The input buffer. This must be equal to the modulus 
                           length.
@param output     [Output] The output buffer. This must be equal to the modulus
                           length.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS       
@retval SB_ERR_NULL_PRIVATE_KEY The <tt>privateKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY  The <tt>privateKey</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF   The <tt>input</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF  The <tt>output</tt> buffer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPrivateDecrypt(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    const unsigned char *input,
    unsigned char *output,
    sb_GlobalCtx sbCtx
);


/** This function is identical to the <tt>hu_RSAPublicEncrypt()</tt> function.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPublicDecrypt(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    const unsigned char * input,
    unsigned char * output,
    sb_GlobalCtx sbCtx
);


/** This function is identical to the <tt>hu_RSAPrivateDecrypt()</tt> function.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPrivateEncrypt(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    const unsigned char *input,
    unsigned char *output,
    sb_GlobalCtx sbCtx
);

/** @} */

/** @addtogroup hu_pkcs_api PKCS APIs
 *
 *@{
 */

/** Encrypts a single plaintext block by applying the PKCS#1 v1.5 encryption
scheme with an RSA public key.

This function implements the operation known as RSAES-PKCS1-v1_5-Encrypt in
PKCS#1 v2.1.

The RSA parameter object must have been created with an RNG context.

If the length of the ciphertext is known, a pointer to a buffer large enough
to hold the ciphertext should be passed in <tt>ciphertext</tt> and its
length in <tt>ciphertextLen</tt>. This function will copy the ciphertext
into <tt>ciphertext</tt> and set the actual length of the ciphertext in
<tt>ciphertextLen</tt>.

If <tt>ciphertext</tt> is <tt>NULL</tt>, then this function will set
the correct length of the ciphertext value in <tt>ciphertextLen</tt>.  If
<tt>ciphertext</tt> is not <tt>NULL</tt> but <tt>ciphertextLen</tt>
is too small, this function will return an error and also will set the correct
length of the ciphertext value in <tt>ciphertextLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams     [Input]        An RSA parameters object.
@param publicKey     [Input]        An RSA public key object.
@param plaintextLen  [Input]        The length (in bytes) of the plaintext. 
                                    This must be at most the modulus length 
				    minus 11.
@param plaintext     [Input]        The plaintext buffer.
@param ciphertextLen [Input/Output] The length (in bytes) of the ciphertext 
                                    buffer. This must be at least the modulus 
				    length.
@param ciphertext    [Output]       The ciphertext buffer.
@param sbCtx         [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PUBLIC_KEY         The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY          The public key object is invalid.
@retval SB_ERR_NULL_INPUT_BUF          The plaintext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The length of the plaintext is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The ciphertext buffer length is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The ciphertext buffer length is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15Enc(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    size_t plaintextLen,
    const unsigned char *plaintext,
    size_t *ciphertextLen,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts a single ciphertext block by applying the PKCS#1 v1.5 decryption
scheme with an RSA private key.

This function implements the operation known as RSAES-PKCS1-v1_5-Decrypt in
PKCS#1 v2.1.

If the length of the plaintext is known, a pointer to a buffer large enough
to hold the plaintext should be passed in <tt>plaintext</tt> and its
length in <tt>plaintextLen</tt>. This function will copy the plaintext
into <tt>plaintext</tt> and set the actual length of the plaintext in
<tt>plaintextLen</tt>.

If <tt>plaintext</tt> is <tt>NULL</tt>, or <tt>plaintext</tt>
is not <tt>NULL</tt> but <tt>plaintextLen</tt> is too small, this
function will set the maximum length of the plaintext in
<tt>plaintextLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams     [Input]        An RSA parameters object.
@param privateKey    [Input]        An RSA private key object.
@param ciphertextLen [Input]        The length (in bytes) of the ciphertext 
                                    buffer. This must be equal to the modulus 
				    length.
@param ciphertext    [Input]        The ciphertext buffer.
@param plaintextLen  [Input/Output] The length (in bytes) of the plaintext. 
                                    This will be at most the modulus length 
				    minus 11.
@param plaintext     [Output]       The plaintext buffer.
@param sbCtx         [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PRIVATE_KEY        The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY         The private key object is invalid.
@retval SB_ERR_NULL_INPUT_BUF          The ciphertext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The ciphertext buffer length is 
                                       incorrect.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The plaintext buffer length is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The plaintext buffer length is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15Dec(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    size_t ciphertextLen,
    const unsigned char *ciphertext,
    size_t *plaintextLen,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);


/** Message digest algorithm identifiers for use with the PKCS#1 v1.5
signature functions */

#define SB_RSA_PKCS1_V15_SIG_SHA1    0
#define SB_RSA_PKCS1_V15_SIG_MD5     1
#define SB_RSA_PKCS1_V15_SIG_MD2     2
#define SB_RSA_PKCS1_V15_SIG_SHA224  3
#define SB_RSA_PKCS1_V15_SIG_SHA256  4
#define SB_RSA_PKCS1_V15_SIG_SHA384  5
#define SB_RSA_PKCS1_V15_SIG_SHA512  6


/** Generates a signature on the given message digest by applying the
PKCS#1 v1.5 signature scheme.

This function implements the operation known as RSASSA-PKCS1-v1_5-Sign
in PKCS#1 v2.1.

This function assumes that the input is a message digest produced by the
specified digest algorithm; no digest operation will be performed on the
input.

If the length of the signature is known, a pointer to a buffer large enough
to hold the signature should be passed in <tt>signature</tt> and its
length in <tt>signatureLen</tt>. This function will copy the signature
into <tt>signature</tt> and set the actual length of the signature in
<tt>signatureLen</tt>.

If <tt>signature</tt> is <tt>NULL</tt>, or <tt>signature</tt>
is not <tt>NULL</tt> but <tt>signatureLen</tt> is too small, this
function will set the maximum length of the signature in
<tt>signatureLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams    [Input]        An RSA parameters object.
@param privateKey   [Input]        An RSA private key object.
@param hashAlgId    [Input]        The message digest algorithm identifier. 
                                   The acceptable values are one of the 
				   <tt>SB_RSA_PKCS1_V15_SIG_*</tt> macros.
@param digestLen    [Input]        The length (in bytes) of the message digest.
@param digest       [Input]        The message digest.
@param signatureLen [Input/Output] The length (in bytes) of the signature. 
                                   This must be at least the modulus length.
@param signature    [Output]       The signature value.
@param sbCtx        [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PRIVATE_KEY        The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY         The private key object is invalid.
@retval SB_ERR_BAD_HASH_TYPE           The message digest algorithm identifier 
                                       is invalid.
@retval SB_ERR_NULL_INPUT_BUF          The message digest buffer is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The length of the message digest is 
                                       invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The length of the signature buffer is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The signature buffer is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15NoHashSign(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    int hashAlgId,
    size_t digestLen,
    const unsigned char *digest,
    size_t *signatureLen,
    unsigned char *signature,
    sb_GlobalCtx sbCtx
);


/** Verifies a PKCS#1 v1.5 signature on the given message digest.

This function implements the operation known as RSASSA-PKCS1-v1_5-Verify
in PKCS#1 v2.1.

This function assumes that the input is a message digest produced by the
specified digest algorithm; no digest operation will be performed on the
input. If the specified digest algorithm does not match the algorithm encoded
in the signature, an error will be returned.

If the signature is valid for the given digest, this function will return
<tt>SB_SUCCESS</tt> and set <tt>result</tt> to a non-zero value.

If the signature is not valid for the given digest, this function may return
<tt>SB_SUCCESS</tt> but <tt>result</tt> will be set to zero.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams    [Input]  An RSA parameters object.
@param publicKey    [Input]  An RSA public key object.
@param hashAlgId    [Input]  The message digest algorithm identifier. The 
                             acceptable values are one of the 
			     <tt>SB_RSA_PKCS1_V15_SIG_*</tt> macros.
@param digestLen    [Input]  The length (in bytes) of the message digest.
@param digest       [Input]  The message digest.
@param signatureLen [Input]  The length (in bytes) of the signature. This must 
                             be equal to the modulus length.
@param signature    [Input]  The signature value.
@param result       [Output] Verification result. This is non-zero if the 
                             signature is valid; otherwise this is zero, meaning
			     that the signature is invalid.
@param sbCtx        [Input]  A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>rsaParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_PUBLIC_KEY   The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY    The public key object is invalid.
@retval SB_ERR_BAD_HASH_TYPE     The message digest algorithm identifier is 
                                 invalid.
@retval SB_ERR_NULL_INPUT_BUF    The message digest buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the message digest is invalid.
@retval SB_ERR_NULL_SIGNATURE    The signature buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_SIGNATURE_LEN The length of the signature buffer is invalid.
@retval SB_ERR_BAD_HASH_TYPE     The message digest algorithm identifier is 
                                 invalid.
@retval SB_FAIL_BAD_PADDING      The signature was not properly padded.
@retval SB_ERR_NULL_OUTPUT       The verification result pointer is 
                                 <tt>NULL</tt>.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15NoHashVerify(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    int hashAlgId,
    size_t digestLen,
    const unsigned char *digest,
    size_t signatureLen,
    const unsigned char *signature,
    int *result,
    sb_GlobalCtx sbCtx
);


/** Produces a ciphertext block by applying the basic RSA primitive on
a PKCS#1 v1.5 signature padded plaintext block using an RSA private key.

This function implements a modified version of the operation known as
RSASSA-PKCS1-v1_5-Sign in PKCS#1 v2.1. The encoding of a message digest into
an ASN.1 DigestInfo structure with DER is replaced by the plaintext message
unchanged.

In other words, the operation consists of padding the plaintext
message using the block formatting of EMSA-PKCS1-v1_5-Encode, and applying
the basic RSA primitive on the padded message using a private key.

If the length of the ciphertext is known, a pointer to a buffer large enough
to hold the ciphertext should be passed in <tt>sigtext</tt> and its
length in <tt>sigtextLen</tt>. This function will copy the ciphertext
into <tt>sigtext</tt> and set the actual length of the ciphertext in
<tt>sigtextLen</tt>.

If <tt>sigtext</tt> is <tt>NULL</tt>, this function will set the
correct length of the ciphertext in <tt>sigtextLen</tt>.  If
<tt>sigtext</tt> is not <tt>NULL</tt> but <tt>sigtextLen</tt> is
too small, this function will return an error and set the correct length of the
ciphertext in <tt>sigtextLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams  [Input]        An RSA parameters object.
@param privateKey [Input]        An RSA private key object.
@param messageLen [Input]        The length (in bytes) of the plaintext buffer.
                                 This must be at most the modulus length minus 
				 11.
@param message    [Input]        The plaintext buffer.
@param sigtextLen [Input/Output] The length (in bytes) of the ciphertext buffer.
                                 This must be at least the modulus length.
@param sigtext    [Output]       The ciphertext buffer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PRIVATE_KEY        The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY         The private key object is invalid.
@retval SB_ERR_NULL_INPUT_BUF          The plaintext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The plaintext buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The ciphertext buffer length is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The ciphertext buffer length is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15SigPadExponent(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    size_t messageLen,
    const unsigned char *message,
    size_t *sigtextLen,
    unsigned char *sigtext,
    sb_GlobalCtx sbCtx
);


/** Produces a plaintext block by applying the basic RSA primitive using a
public key on a ciphertext block that was PKCS#1 v1.5 signature padded.

This function implements a modified version of the operation known as
RSASSA-PKCS1-v1_5-Verify in PKCS#1 v2.1. The encoding of a message digest into
an ASN.1 DigestInfo structure with DER is replaced by the plaintext message
unchanged.

In other words, the operation consists of applying the basic RSA primitive on
the ciphertext using a public key, ensuring that the decrypted block uses the
block formatting of EMSA-PKCS1-v1_5-Encode, and returning the unpadded
plaintext.

If the length of the plaintext is known, a pointer to a buffer large enough
to hold the plaintext should be passed in <tt>message</tt> and its
length in <tt>messageLen</tt>. This function will copy the plaintext
into <tt>message</tt> and set the actual length of the plaintext in
<tt>messageLen</tt>.

If <tt>message</tt> is <tt>NULL</tt>, or <tt>message</tt>
is not <tt>NULL</tt> but <tt>messageLen</tt> is too small, this
function will set the maximum length of the plaintext in
<tt>messageLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams  [Input]        An RSA parameters object.
@param publicKey  [Input]        An RSA public key object.
@param sigtextLen [Input]        The length (in bytes) of the ciphertext buffer.
                                 This must be equal to the modulus length.
@param sigtext    [Input]        The ciphertext buffer.
@param messageLen [Input/Output] The length (in bytes) of the plaintext buffer.
                                 This must be at most the modulus length minus 
				 11.
@param message    [Output]       The plaintext buffer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PUBLIC_KEY         The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY          The public key object is invalid.
@retval SB_ERR_NULL_INPUT_BUF          The ciphertext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The ciphertext buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The plaintext buffer length is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The plaintext buffer length is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v15VerPadExponent(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    size_t sigtextLen,
    const unsigned char *sigtext,
    size_t *messageLen,
    unsigned char *message,
    sb_GlobalCtx sbCtx
);


/** Encrypts a single plaintext block by applying the PKCS#1 v2.1
RSAES-OAEP-Encrypt operation with SHA-1 and MGF1.

The RSA parameter object must have been created with an RNG context.

If the length of the ciphertext is known, a pointer to a buffer large enough
to hold the ciphertext should be passed in <tt>ciphertext</tt> and its
length in <tt>ciphertextLen</tt>. This function will copy the ciphertext
into <tt>ciphertext</tt> and set the actual length of the ciphertext in
<tt>ciphertextLen</tt>.

If <tt>ciphertext</tt> is <tt>NULL</tt>, then this function will set
the correct length of the ciphertext value in <tt>ciphertextLen</tt>.  If
<tt>ciphertext</tt> is not <tt>NULL</tt> but <tt>ciphertextLen</tt>
is too small, this function will return an error and also will set the correct
length of the ciphertext value in <tt>ciphertextLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams     [Input]        An RSA parameters object.
@param publicKey     [Input]        An RSA public key object.
@param addInfoLen    [Input]        The length (in bytes) of the message label.
                                    (Optional)
@param addInfo       [Input]        The label to be associated with the message.
                                    (Optional - set to <tt>NULL</tt> if not 
				    used.)
@param plaintextLen  [Input]        The length (in bytes) of the plaintext 
                                    buffer. This must be at most the modulus 
				    length minus 42.
@param plaintext     [Input]        The plaintext buffer.
@param ciphertextLen [Input/Output] The length (in bytes) of the ciphertext 
                                    buffer. This must be at least the modulus 
				    length.
@param ciphertext    [Output]       The ciphertext buffer.
@param sbCtx         [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PUBLIC_KEY         The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY          The public key object is invalid.
@retval SB_ERR_NULL_ADDINFO            The message label is <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF          The plaintext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The plaintext buffer length is too large.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The ciphertext buffer length is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The ciphertext buffer length is invalid.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v21SHA1Encrypt(
    sb_Params rsaParams,
    sb_PublicKey publicKey,
    size_t addInfoLen,
    const unsigned char *addInfo,
    size_t plaintextLen,
    const unsigned char *plaintext,
    size_t *ciphertextLen,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts a single ciphertext block by applying the PKCS#1 v2.1
RSAES-OAEP-Decrypt operation with SHA-1 and MGF1.

If the length of the plaintext is known, a pointer to a buffer large enough
to hold the plaintext should be passed in <tt>plaintext</tt> and its
length in <tt>plaintextLen</tt>. This function will copy the plaintext
into <tt>plaintext</tt> and set the actual length of the plaintext in
<tt>plaintextLen</tt>.

If <tt>plaintext</tt> is <tt>NULL</tt>, or <tt>plaintext</tt>
is not <tt>NULL</tt> but <tt>plaintextLen</tt> is too small, this
function will set the maximum length of the plaintext in
<tt>plaintextLen</tt>.

The length of the modulus can be determined by calling
<tt>hu_RSAKeyGet()</tt> and retrieving the <tt>nLen</tt> argument.

@param rsaParams     [Input]        An RSA parameters object.
@param privateKey    [Input]        An RSA private key object.
@param addInfoLen    [Input]        The length (in bytes) of the message label.
                                    (Optional)
@param addInfo       [Input]        The label to be associated with the message.
                                    (Optional - set to <tt>NULL</tt> if not 
				    used.)
@param ciphertextLen [Input]        The length (in bytes) of the ciphertext 
                                    buffer. This must be equal to the modulus 
				    length.
@param ciphertext    [Input]        The ciphertext buffer.
@param plaintextLen  [Input/Output] The length (in bytes) of the plaintext 
                                    buffer. This will be at most the modulus 
				    length minus 42.
@param plaintext     [Output]       The plaintext buffer.
@param sbCtx         [Input]        A global context.

@retval SB_ERR_NULL_PARAMS             The <tt>rsaParams</tt> object is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS              
@retval SB_ERR_NULL_PRIVATE_KEY        The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY         The private key object is invalid.
@retval SB_ERR_NULL_ADDINFO            The message label is <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF          The ciphertext buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN       The ciphertext buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN_PTR The plaintext buffer length is 
                                       <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN      The plaintext buffer length is invalid.
@retval SB_FAIL_PKCS1_DECRYPT          The decrypted data is incorrectly padded
                                       or <tt>addInfo</tt> does not match what 
				       was used to generate the ciphertext.
@retval SB_FAIL_ALLOC                  Memory allocation failure.
@retval SB_SUCCESS                     Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RSAPKCS1v21SHA1Decrypt(
    sb_Params rsaParams,
    sb_PrivateKey privateKey,
    size_t addInfoLen,
    const unsigned char *addInfo,
    size_t ciphertextLen,
    const unsigned char *ciphertext,
    size_t *plaintextLen,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
