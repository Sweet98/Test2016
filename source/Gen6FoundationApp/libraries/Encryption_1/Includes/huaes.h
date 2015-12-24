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
 * $Source: Includes/huaes.h $
 * $Name:  $
 */
#ifndef HUAES_H
#define HUAES_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_aes_api AES APIs
 *
 * @{
 */


#define SB_AES_CTR_BASE 7
#define SB_AES_CTR(ctrBits) (SB_AES_CTR_BASE | (ctrBits << 8))

/** Modes of operation */
#define SB_AES_ECB      1
#define SB_AES_CBC      2
#define SB_AES_CFB128   3
#define SB_AES_OFB128   4
#define SB_AES_KEYWRAP  5
#define SB_AES_CTR8    SB_AES_CTR(8)
#define SB_AES_CTR16   SB_AES_CTR(16)
#define SB_AES_CTR32   SB_AES_CTR(32)
#define SB_AES_CTR64   SB_AES_CTR(64)
#define SB_AES_CTR128  SB_AES_CTR(128)



/** Block length */
#define SB_AES_128_BLOCK_BITS   128
#define SB_AES_128_BLOCK_BYTES  (SB_AES_128_BLOCK_BITS >> 3)

#define SB_AES_KEYWRAP_BLOCK_BITS   64
#define SB_AES_KEYWRAP_BLOCK_BYTES  (SB_AES_KEYWRAP_BLOCK_BITS >> 3)

/** Key length */
#define SB_AES_128_KEY_BITS   128
#define SB_AES_128_KEY_BYTES  (SB_AES_128_KEY_BITS >> 3)
#define SB_AES_192_KEY_BITS   192
#define SB_AES_192_KEY_BYTES  (SB_AES_192_KEY_BITS >> 3)
#define SB_AES_256_KEY_BITS   256
#define SB_AES_256_KEY_BYTES  (SB_AES_256_KEY_BITS >> 3)


/** Creates an AES parameters object.

The mode of operation and block length must be specified. An RNG context
and/or yielding context must be supplied if key generation and/or yielding
will be performed, respectively.

@param mode       [Input]  The mode of operation. The acceptable values are 
                           <tt>SB_AES_ECB</tt>, <tt>SB_AES_CBC</tt>, 
                           <tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt> and 
                           <tt>SB_AES_KEYWRAP</tt> and 
                           <tt>SB_AES_CTR(ctrBits)</tt>. For 
                           <tt>SB_AES_CTR(ctrBits)</tt>, <tt>ctrBits</tt> 
                           is the number of bits that will be used for the 
                           counter. <tt>ctrBits</tt> must be less 
                           than or equal to <tt>SB_AES_128_BLOCK_BITS</tt>.
                           <tt>0</tt> is also acceptable since some adapters 
                           support <tt>hu_AESBeginV2()</tt>.
@param blockLen   [Input]  The length (in bits) of an encryption block. If 
                           <tt>mode</tt> is <tt>SB_AES_KEYWRAP</tt>, the length 
                           must be <tt>SB_AES_KEYWRAP_BLOCK_BITS</tt>; 
                           otherwise, the length must be 
                           <tt>SB_AES_128_BLOCK_BITS</tt>.
@param rngContext [Input]  An RNG context. (Optional - set to 
                           <tt>NULL</tt> if key generation will not be 
                           performed.)
@param yieldCtx   [Input]  A yield context. (Optional - set to 
                           <tt>NULL</tt> if yielding is not required.)
@param aesParams  [Output] The AES parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_BAD_MODE        <tt>mode</tt> is an invalid mode of operation.
@retval SB_ERR_BAD_BLOCK_LEN   <tt>blockLen</tt> is an invalid block length.
@retval SB_ERR_NULL_PARAMS_PTR The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESParamsCreate(
    int mode,
    size_t blockLen,
    sb_RNGCtx rngContext,
    sb_YieldCtx yieldCtx,
    sb_Params *aesParams,
    sb_GlobalCtx sbCtx
);


/** Retrieves settings from an AES parameters object.

@param aesParams [Input]  The AES parameters object.
@param mode      [Output] The mode of operation.
@param blockLen  [Output] The length (in bits) of an encryption block.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS     The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS      
@retval SB_ERR_NULL_OUTPUT_BUF All output pointers are <tt>NULL</tt>.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESParamsGet(
    sb_Params aesParams,
    int *mode,
    size_t *blockLen,
    sb_GlobalCtx sbCtx
);


/** Destroys an AES parameters object.

AES contexts and key objects must be destroyed before their corresponding AES
parameters object is destroyed.

@param aesParams [Input/Output] The AES parameters object pointer.
@param sbCtx     [Input]        A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>aesParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_NULL_PARAMS     The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS      
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESParamsDestroy(
    sb_Params *aesParams,
    sb_GlobalCtx sbCtx
);


/** Creates an AES key object from the given key value that can later be
used for encryption and/or decryption.

If the key will be used exclusively for either encryption or decryption,
then <tt>hu_AESEncryptKeySet()</tt> or <tt>hu_AESDecryptKeySet()</tt> should be
called, respectively, as they use fewer resources.

@param aesParams [Input]  An AES parameters object.
@param keyLen    [Input]  The length (in bits) of the AES key value. The 
                          acceptable values are <tt>SB_AES_128_KEY_BITS</tt>, 
                          <tt>SB_AES_192_KEY_BITS</tt> and
                          <tt>SB_AES_256_KEY_BITS</tt>.
@param keyValue  [Input]  The key value.
@param aesKey    [Output] The AES key object pointer.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS    The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS     
@retval SB_ERR_BAD_KEY_LEN    <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_INPUT_BUF <tt>keyValue</tt> is <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY_PTR   <tt>aesKey</tt> is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC         Memory allocation failure.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeySet(
    sb_Params aesParams,
    size_t keyLen,
    const unsigned char *keyValue,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Creates an AES key object from the given key value that can later be
used for encryption.

If the key will be used for both encryption and decryption, then
<tt>hu_AESKeySet()</tt> should be called instead of this function.

@param aesParams [Input]  The AES parameters object.
@param keyLen    [Input]  The length (in bits) of the AES key value. The 
                          acceptable values are <tt>SB_AES_128_KEY_BITS</tt>, 
                          <tt>SB_AES_192_KEY_BITS</tt> and
                          <tt>SB_AES_256_KEY_BITS</tt>.
@param keyValue  [Input]  The key value.
@param aesKey    [Output] The AES key object pointer.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS    The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS     
@retval SB_ERR_BAD_KEY_LEN    <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_INPUT_BUF <tt>keyValue</tt> is <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY_PTR   The <tt>aesKey</tt> object pointer is 
                              <tt>NULL</tt>.
@retval SB_FAIL_ALLOC         Memory allocation failure.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESEncryptKeySet(
    sb_Params aesParams,
    size_t keyLen,
    const unsigned char *keyValue,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Creates an AES key object from the given key value that can later be
used for decryption.

If the key will be used for both encryption and decryption, then
<tt>hu_AESKeySet()</tt> should be called instead of this function.

@param aesParams [Input]  The AES parameters object.
@param keyLen    [Input]  The length (in bits) of the AES key value. The 
                          acceptable values are <tt>SB_AES_128_KEY_BITS</tt>, 
                          <tt>SB_AES_192_KEY_BITS</tt> and
                          <tt>SB_AES_256_KEY_BITS</tt>.
@param keyValue  [Input]  The key value.
@param aesKey    [Output] The AES key object pointer.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS    The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS     
@retval SB_ERR_BAD_KEY_LEN    <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_INPUT_BUF <tt>keyValue</tt> is <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY_PTR   The <tt>aesKey</tt> object pointer is 
                              <tt>NULL</tt>.
@retval SB_FAIL_ALLOC         Memory allocation failure.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESDecryptKeySet(
    sb_Params aesParams,
    size_t keyLen,
    const unsigned char *keyValue,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Creates an AES key object of the specified length from random data that
can later be used for encryption and/or decryption.

The AES parameter object must have been created with an RNG context.

If the key will be used only for encryption or only for decryption, then you
should use <tt>hu_AESEncryptKeyGen()</tt> or <tt>hu_AESEncryptKeyGen()</tt>, 
respectively, as they use fewer resources.

@param aesParams [Input]  The AES parameters object.
@param keyLen    [Input]  The length (in bits) of the AES key value. The 
                          acceptable values are <tt>SB_AES_128_KEY_BITS</tt>, 
                          <tt>SB_AES_192_KEY_BITS</tt> and
                          <tt>SB_AES_256_KEY_BITS</tt>.
@param aesKey    [Output] The AES key object pointer.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS  The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_BAD_KEY_LEN  <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_KEY_PTR The <tt>aesKey</tt> object pointer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC       Memory allocation failure.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeyGen(
    sb_Params aesParams,
    size_t keyLen,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Creates an AES key object of the specified length from random data that
can later be used for encryption.

If the key will be used for encryption and decryption, <tt>hu_AESKeyGen()</tt> 
should be called instead of this function.

The AES parameter object must have been created with an RNG context.

@param aesParams [Input]  The AES parameters object.
@param keyLen    [Input]  The length (in bits) of the AES key value. The 
                          acceptable values are <tt>SB_AES_128_KEY_BITS</tt>, 
                          <tt>SB_AES_192_KEY_BITS</tt> and
                          <tt>SB_AES_256_KEY_BITS</tt>.
@param aesKey    [Output] The AES key object pointer.
@param sbCtx     [Input]  A global context.

@retval SB_ERR_NULL_PARAMS  The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_BAD_KEY_LEN  <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_KEY_PTR The <tt>aesKey</tt> object pointer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC       Memory allocation failure.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESEncryptKeyGen(
    sb_Params aesParams,
    size_t keyLen,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Retrieves the key value and its length from an AES key object.

If the length of the key value is known, a pointer to a buffer large enough
to hold the key value should be passed in <tt>keyValue</tt> and its
length in <tt>keyLen</tt>. This function will copy the key value into
<tt>keyValue</tt> and set the actual length of the key value in
<tt>keyLen</tt>.

If <tt>keyValue</tt> is <tt>NULL</tt>, then this function will set the
correct length of the key value in <tt>keyLen</tt>. If <tt>keyValue</tt> is 
not <tt>NULL</tt> but <tt>keyLen</tt> is too small, this function will return 
an error and also will set the correct length of the key value in 
<tt>keyLen</tt>.

@param aesParams [Input]        The AES parameters object.
@param aesKey    [Input]        The AES key object.
@param keyLen    [Input/Output] The length (in bits) of the AES key value.
@param keyValue  [Output]       The key value.
@param sbCtx     [Input]        A global context.

@retval SB_ERR_NULL_PARAMS            The <tt>aesParams</tt> object is 
                                      <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS
@retval SB_ERR_NULL_KEY               The <tt>aesKey</tt> object is 
                                      <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY                The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_NULL_KEY_LEN           <tt>keyLen</tt> is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN     The <tt>keyValue</tt> buffer length is 
                                      invalid.
@retval SB_ERR_SYM_KEY_NOT_EXPORTABLE The key value cannot be exported from 
                                      the <tt>aesKey</tt> object.
@retval SB_SUCCESS                    Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeyGet(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t *keyLen,
    unsigned char *keyValue,
    sb_GlobalCtx sbCtx
);


/** Destroys an AES key object.

AES contexts must be destroyed before any AES key objects. AES parameter
objects must be destroyed after AES key objects.

@param aesParams [Input]        The AES parameters object.
@param aesKey    [Input/Output] The AES key object pointer.
@param sbCtx     [Input]        Global context.

@retval SB_ERR_NULL_PARAMS  The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS
@retval SB_ERR_NULL_KEY_PTR The <tt>aesKey</tt> object pointer is <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY     The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY      The <tt>aesKey</tt> object is invalid.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeyDestroy(
    sb_Params aesParams,
    sb_Key *aesKey,
    sb_GlobalCtx sbCtx
);


/** Creates an AES context initialized with the given parameters and key.

An initial vector is required for the <tt>SB_AES_CBC</tt>, 
<tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt> and <tt>SB_AES_CTR(X)</tt> modes
of operation. For <tt>SB_AES_CTR(ctrBits)</tt>, 
<tt>iv</tt> will be used as the initial counter block.
<tt>iv</tt> is ignored for <tt>SB_AES_ECB</tt>, as an initial
vector is not required for that mode.

@param aesParams  [Input]  The AES parameters object.
@param aesKey     [Input]  The AES key object.
@param ivLen      [Input]  The length (in bytes) of initial vector. The only 
                           acceptable value is <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param iv         [Input]  The initial vector.
@param aesContext [Output] The AES context object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS       
@retval SB_ERR_NULL_KEY         The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY          The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_NULL_IV          <tt>iv</tt>, the initial vector, is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_IV_LEN       <tt>ivLen</tt> is an invalid vector length.
@retval SB_ERR_NULL_CONTEXT_PTR The context object pointer is <tt>NULL</tt>.
@retval SB_ERR_NO_MODE          No mode has been specified.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESBegin(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t ivLen,
    const unsigned char *iv,
    sb_Context *aesContext,
    sb_GlobalCtx sbCtx
);

/** Creates an AES context initialized with the given parameters (with the
ability to override the mode in the parameters) and key.

An initial vector is required for the <tt>SB_AES_CBC</tt>, 
<tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt>, and 
<tt>SB_AES_CTR(ctrBits)</tt> modes of operation. For 
<tt>SB_AES_CTR(ctrBits)</tt>, <tt>iv</tt> will be used as the initial counter 
block.  <tt>iv</tt> is ignored for <tt>SB_AES_ECB</tt>, as an initial vector
is not required for that mode.

@param aesParams  [Input]  The AES parameters object.
@param aesKey     [Input]  The AES key object.
@param mode       [Input]  The mode of operation. The acceptable values are 0, 
                           <tt>SB_AES_ECB</tt>, <tt>SB_AES_CBC</tt>, 
                           <tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt>,
                           <tt>SB_AES_KEYWRAP</tt> and 
                           <tt>SB_AES_CTR(ctrBits)</tt>. For 
                           <tt>SB_AES_CTR(ctrBits)</tt>, <tt>ctrBits</tt> is 
                           the number of bits that will be used for the counter.
                           <tt>ctrBits</tt> must be less 
                           than or equal to <tt>SB_AES_128_BLOCK_BITS</tt>. 
                           Specifying <tt>0</tt> will 
                           use the mode from the parameters object.
@param ivLen      [Input]  The length (in bytes) of initial vector. The only 
                           acceptable value is <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param iv         [Input]  The initial vector.
@param aesContext [Output] The AES context object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS       
@retval SB_ERR_NULL_KEY         The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY          The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_NULL_IV          <tt>iv</tt>, the initial vector, is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_IV_LEN       <tt>ivLen</tt> is an invalid vector length.
@retval SB_ERR_NULL_CONTEXT_PTR The context object pointer is <tt>NULL</tt>.
@retval SB_ERR_NO_MODE          No mode has been specified.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESBeginV2(
    sb_Params aesParams,
    sb_Key aesKey,
    int mode,
    size_t ivLen,
    const unsigned char *iv,
    sb_Context *aesContext,
    sb_GlobalCtx sbCtx
);


/** Encrypts one or more blocks of plaintext using the given AES context.

The plaintext and ciphertext buffers must be the same length, and may overlap
in memory subject to the constraints described in the API Reference section
on overlapping buffers. This function can be called repeatedly to encrypt 
more blocks of plaintext.

@param aesContext [Input]  AES context object.
@param length     [Input]  The length (in bytes) of plaintext. The acceptable 
                           values are multiples of 
                           <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param plaintext  [Input]  The plaintext buffer.
@param ciphertext [Output] The ciphertext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_BAD_KEY           The key associated with <tt>aesContext</tt>
                                 cannot be used for encryption.
@retval SB_ERR_NULL_CONTEXT      The <tt>aesContext</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT       The <tt>aesContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The plaintext buffer, <tt>plaintext</tt>, is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN <tt>length</tt>, the length of the plaintext 
                                 buffer, is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The ciphertext buffer is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESEncrypt(
    sb_Context aesContext,
    size_t length,
    const unsigned char *plaintext,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts one or more blocks of ciphertext using the given AES context.

The plaintext and ciphertext buffers must be the same length, and may overlap
in memory subject to the constraints described in the API Reference section
on overlapping buffers. This function can be called repeatedly to decrypt 
more blocks of ciphertext.

@param aesContext [Input]  AES context object.
@param length     [Input]  The length (in bytes) of ciphertext. The acceptable 
                           values are multiples of 
                           <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param ciphertext [Input]  The ciphertext buffer.
@param plaintext  [Output] The plaintext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_BAD_KEY           The key associated with <tt>aesContext</tt>
                                 cannot be used for decryption.
@retval SB_ERR_NULL_CONTEXT      The <tt>aesContext</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT       The <tt>aesContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN <tt>length</tt> is an invalid ciphertext 
                                 buffer length.
@retval SB_ERR_NULL_OUTPUT_BUF   The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESDecrypt(
    sb_Context aesContext,
    size_t length,
    const unsigned char *ciphertext,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);


/** Destroys an AES context object.

This function must be called to terminate an encryption or decryption
operation. An AES context must be destroyed before the corresponding AES key
object and AES parameters object are destroyed.

@param aesContext [Input/Output] The AES context object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>aesContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT     The <tt>aesContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      <tt>aesContext</tt> is invalid.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESEnd(
    sb_Context *aesContext,
    sb_GlobalCtx sbCtx
);


/** Encrypts a plaintext buffer using the given AES parameters and key.

An initial vector is required for the <tt>SB_AES_CBC</tt>, 
<tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt> and <tt>SB_AES_CTR(ctrBits)</tt>
modes of operation. For <tt>SB_AES_CTR(ctrBits)</tt>, <tt>iv</tt> will be used 
as the initial counter block.
<tt>iv</tt> is ignored for <tt>SB_AES_ECB</tt>, as an initial vector
is not required for that mode. The plaintext and ciphertext buffers must be 
the same length, and may overlap in memory subject to the constraints 
described in the API Reference section on overlapping buffers.

This function should only be called for blocks of plaintext and ciphertext
that can be stored entirely in memory.

@param aesParams  [Input]  The AES parameters object.
@param aesKey     [Input]  The AES key object.
@param ivLen      [Input]  The length (in bytes) of initial vector. The only 
                           acceptable value is <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param iv         [Input]  The initial vector.
@param length     [Input]  The length (in bytes) of plaintext. The acceptable 
                           values are multiples of 
                           <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param plaintext  [Input]  The plaintext buffer.
@param ciphertext [Output] The ciphertext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_KEY          The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY           The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_NULL_IV           The initial vector, <tt>iv</tt>, is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_IV_LEN        The initial vector length, <tt>ivLen</tt>, is 
                                 invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN <tt>length</tt>, the length of the plaintext 
                                 buffer, is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   <tt>ciphertext</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESEncryptMsg(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t ivLen,
    const unsigned char *iv,
    size_t length,
    const unsigned char *plaintext,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts a ciphertext buffer using the given AES parameters and key.

An initial vector is required for the <tt>SB_AES_CBC</tt>, 
<tt>SB_AES_CFB128</tt>, <tt>SB_AES_OFB128</tt> and 
<tt>SB_AES_CTR(ctrBits)</tt> modes of operation.
For <tt>SB_AES_CTR(ctrBits)</tt>, <tt>iv</tt> will be used 
as the initial counter block.
<tt>iv</tt> is ignored for <tt>SB_AES_ECB</tt>, as an initial vector 
is not required for that mode. The plaintext and ciphertext buffers must be 
the same length, and may overlap in memory subject to the constraints 
described in the API Reference section on overlapping buffers.

This function should only be called for blocks of plaintext and ciphertext
that can be stored entirely in memory.

@param aesParams  [Input]  The AES parameters object.
@param aesKey     [Input]  The AES key object.
@param ivLen      [Input]  The length (in bytes) of initial vector. The only 
                           acceptable value is <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param iv         [Input]  The initial vector.
@param length     [Input]  The length (in bytes) of <tt>ciphertext</tt>. The 
                           acceptable values are multiples of 
                           <tt>SB_AES_128_BLOCK_BYTES</tt>.
@param ciphertext [Input]  The ciphertext buffer.
@param plaintext  [Output] The plaintext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>aesParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_KEY          The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY           The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_NULL_IV           <tt>iv</tt>, the initial vector, is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_IV_LEN        <tt>ivLen</tt> is an invalid vector length.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The ciphertext buffer length, <tt>length</tt>,
                                 is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESDecryptMsg(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t ivLen,
    const unsigned char *iv,
    size_t length,
    const unsigned char *ciphertext,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);


/** Wraps a plaintext buffer according to the AES Key Wrap algorithm with
the given AES parameters and key objects.

The AES parameters must have been created with <tt>SB_AES_KEYWRAP</tt>
mode. The length of the plaintext must be a multiple of 
<tt>SB_AES_KEYWRAP_BLOCK_BYTES</tt> and at least 
<tt>2 * SB_AES_KEYWRAP_BLOCK_BYTES</tt>. The length of the ciphertext will be
<tt>SB_AES_KEYWRAP_BLOCK_BYTES</tt> bytes longer than the plaintext
length. The plaintext and ciphertext buffers may overlap 
in memory subject to the constraints described in the API Reference section
on overlapping buffers.

If the length of the ciphertext is known, a pointer to a buffer large enough
to hold the ciphertext should be passed in <tt>ciphertext</tt> and its
length in <tt>ciphertextLength</tt>. This function will copy the
ciphertext value into <tt>ciphertext</tt> and set the actual length of
the ciphertext in <tt>ciphertextLength</tt>.

If <tt>ciphertext</tt> is <tt>NULL</tt>, then this function will set
the correct length of the ciphertext value in <tt>ciphertextLen</tt>. If
<tt>ciphertext</tt> is not <tt>NULL</tt> but <tt>ciphertextLen</tt>
is too small, this function will return an error and also will set the correct
length of the ciphertext value in <tt>ciphertextLen</tt>.

@param aesParams        [Input]        The AES parameters object.
@param aesKey           [Input]        The AES key object.
@param plaintextLength  [Input]        The length (in bytes) of plaintext.
@param plaintext        [Input]        Plaintext buffer.
@param ciphertextLength [Input/Output] The length (in bytes) of ciphertext.
@param ciphertext       [Output]       The ciphertext buffer.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      The <tt>plaintext</tt> buffer is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   <tt>plaintextLength</tt> is incorrect.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN The <tt>ciphertextLength</tt> pointer is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN  <tt>ciphertextLength</tt> is invalid.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeyWrap(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t plaintextLength,
    const unsigned char *plaintext,
    size_t *ciphertextLength,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Unwraps a ciphertext buffer according to the AES Key Wrap algorithm with
the given AES parameters and key objects.

The AES parameters must have been created with <tt>SB_AES_KEYWRAP</tt> mode. 
The length of the ciphertext must be a multiple of 
<tt>SB_AES_KEYWRAP_BLOCK_BYTES</tt> and at least 
<tt>3 * SB_AES_KEYWRAP_BLOCK_BYTES</tt>. The length of the plaintext will be
<tt>SB_AES_KEYWRAP_BLOCK_BYTES</tt> bytes shorter than the ciphertext length. 
The ciphertext and plaintext buffers may overlap in memory subject to the 
constraints described in the API Reference section on overlapping buffers.

If the length of the plaintext is known, a pointer to a buffer large enough
to hold the plaintext should be passed in <tt>plaintext</tt> and its
length in <tt>plaintextLength</tt>. This function will copy the
plaintext value into <tt>plaintext</tt> and set the actual length of
the plaintext in <tt>plaintextLength</tt>.

If <tt>plaintext</tt> is <tt>NULL</tt>, then this function will set
the correct length of the plaintext value in <tt>plaintextLen</tt>. If
<tt>plaintext</tt> is not <tt>NULL</tt> but <tt>plaintextLen</tt>
is too small, this function will return an error and also will set the correct
length of the plaintext value in <tt>plaintextLen</tt>.

@param aesParams        [Input]        The AES parameters object.
@param aesKey           [Input]        The AES key object.
@param ciphertextLength [Input]        The length (in bytes) of ciphertext.
@param ciphertext       [Input]        The ciphertext buffer.
@param plaintextLength  [Input/Output] The length (in bytes) of plaintext.
@param plaintext        [Output]       The plaintext buffer.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      <tt>ciphertext</tt> is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   <tt>ciphertextLength</tt> is incorrect.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN The <tt>plaintextLength</tt> pointer is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN  <tt>plaintextLength</tt> is invalid.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESKeyUnwrap(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t ciphertextLength,
    const unsigned char *ciphertext,
    size_t *plaintextLength,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);



/** Does an authenticated encryption according to the AES CCM* algorithm with
the given AES parameters and key objects.

CCM is described in the NIST Special Publication 800-38C and CCM* is described 
in Version 1.0 of the Zigbee Specification. CCM* is a variant of CCM that 
allows encryption without authentication. Please note that CCM* is not NIST 
certified and therefore this function should not be used if your implementation 
must comply with the NIST CCM mode of operation. 

The <tt>plaintext</tt> and <tt>ciphertext</tt> buffers may overlap in memory, 
subject to the constraints described in the API Reference section on 
overlapping buffers.

The provider registered for AES must support <tt>SB_AES_ECB</tt>
and <tt>SB_AES_CBC</tt>.

<em>Note</em>: This function should only be called for buffers of plaintext and ciphertext
that can be stored entirely in memory.

@param aesParams        [Input]        The AES parameters object.
@param aesKey           [Input]        The AES key object. The key must be a 
                                       valid encryption key.
@param nonceLen         [Input]        The length (in bytes) of nonce. 
                                       Acceptable values are <tt>7</tt>, 
                                       <tt>8</tt>, <tt>9</tt>, <tt>10</tt>, 
                                       <tt>11</tt>, <tt>12</tt> and <tt>13</tt>.
@param nonce            [Input]        Nonce buffer.
@param addDataLen       [Input]        The length (in bytes) of additional data.
                                       If <tt>addDataLen</tt> is not 0, 
                                       <tt>addData</tt> cannot be <tt>NULL</tt>.
@param addData          [Input]        Additional data buffer. This data
                                       will be authenticated, but will not be
                                       encrypted.
@param plaintextLen     [Input]        The length (in bytes) of plaintext. If 
                                       <tt>plaintextLen</tt> is not 0, 
                                       <tt>plaintext</tt> cannot be 
                                       <tt>NULL</tt>.
@param plaintext        [Input]        Plaintext buffer. This data will be 
                                       authenticated and encrypted.
@param macLen           [Input]        The length of the CBC-MAC used for 
                                       authentication. Acceptable values are 
                                       <tt>0</tt>, <tt>4</tt>, <tt>6</tt>, 
                                       <tt>8</tt>, <tt>10</tt>, <tt>12</tt>, 
                                       <tt>14</tt> and <tt>16</tt>. If 
                                       <tt>macLen</tt> is 0, then 
                                       <tt>addData</tt> and <tt>plaintext</tt> 
                                       will not be authenticated.
@param ciphertext       [Output]       The ciphertext buffer. The ciphertext 
                                       buffer must be at least 
                                       <tt>plaintextLen</tt> + <tt>macLen</tt> 
                                       bytes in length.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      The <tt>plaintext</tt>, <tt>nonce</tt>,
                                   or <tt>addData</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   The <tt>nonceLen</tt> is invalid.
@retval SB_ERR_BAD_LENGTH          The <tt>macLen</tt> is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF     The <tt>ciphertext</tt> buffer is 
                                   <tt>NULL</tt>.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESCCMStarAuthEncrypt(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t nonceLen,
    const unsigned char* nonce,
    size_t addDataLen,
    const unsigned char* addData,
    size_t plaintextLen,
    const unsigned char* plaintext,    
    size_t macLen,
    unsigned char* ciphertext,
    sb_GlobalCtx sbCtx
    );



/** Does an authenticated decryption according to the AES CCM* algorithm with
the given AES parameters and key objects.

This is a variant of CCM that allows the encryption without
authentication. CCM* is not NIST certified. This function should 
not be used if your implementation must comply with the NIST
CCM mode of operation. 
CCM is described in NIST Special Publication 800-38C.
CCM* is described in the Zigbee Specification, Version 1.0.
The plaintext and ciphertext buffers may overlap 
in memory subject to the constraints described in the API Reference section
on overlapping buffers.
The adapter registered for AES must support <tt>SB_AES_ECB</tt>,
and <tt>SB_AES_CBC</tt>.
This function should only be called for buffers of plaintext and ciphertext
that can be stored entirely in memory.


@param aesParams        [Input]        The AES parameters object.
@param aesKey              [Input]        The AES key object. The key must be a 
                                       valid encryption key.
@param nonceLen         [Input]        The length (in bytes) of nonce. 
                                       Acceptable values are <tt>7</tt>, 
                                       <tt>8</tt>, <tt>9</tt>, <tt>10</tt>, 
                                       <tt>11</tt>, <tt>12</tt> and 
                                       <tt>13</tt>.
@param nonce            [Input]        Nonce buffer.
@param addDataLen       [Input]        The length (in bytes) of additional data.
                                       If <tt>addDataLen</tt> is not 0, then
                                       <tt>addData</tt> cannot be <tt>NULL</tt>.
@param addData          [Input]        Additional data buffer. This data
                                       will be authenticated, but will not be
                                       encrypted.
@param ciphertextLen    [Input]        The length (in bytes) of ciphertext. If 
                                       <tt>ciphertextLen</tt> is not 0, then
                                       <tt>ciphertext</tt> cannot be 
                                       <tt>NULL</tt>.
@param ciphertext       [Input]        Ciphertext buffer. This data will be 
                                       authenticated and decrypted.
@param macLen           [Input]        The length of the CBC-MAC used for 
                                       authentication. Acceptable values are 
                                       <tt>4</tt>, <tt>6</tt>, <tt>8</tt>, 
                                       <tt>10</tt>, <tt>12</tt>, <tt>14</tt>
                                       and <tt>16</tt>.
@param plaintext        [Output]       The plaintext buffer. The plaintext 
                                       buffer must be at least 
                                       <tt>ciphertextLen</tt> - <tt>macLen</tt> 
                                       bytes in length.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      The <tt>ciphertext</tt>, <tt>nonce</tt>,
                                   or <tt>addData</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   The <tt>nonceLen</tt> or 
                                   <tt>ciphertextLen</tt> is invalid.
@retval SB_ERR_BAD_LENGTH          The <tt>macLen</tt> is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF     The <tt>plaintext</tt> buffer is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN  <tt>ciphertextLen</tt> is not consistent
                                   with <tt>nonceLen</tt>.
@retval SB_ERR_MAC_INVALID         The MAC is invalid.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESCCMStarAuthDecrypt(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t nonceLen,
    const unsigned char* nonce,
    size_t addDataLen,
    const unsigned char* addData,
    size_t ciphertextLen,
    const unsigned char* ciphertext,
    size_t macLen,
    unsigned char* plaintext,
    sb_GlobalCtx sbCtx
    );



/** Does an authenticated encryption according to the AES CCM algorithm with
the given AES parameters and key objects.
CCM is described in NIST Special Publication 800-38C.

The plaintext and ciphertext buffers may overlap 
in memory subject to the constraints described in the API Reference section
on overlapping buffers.
The adapter registered for AES must support <tt>SB_AES_ECB</tt>,
and <tt>SB_AES_CBC</tt>.
This function should only be called for buffers of plaintext and ciphertext
that can be stored entirely in memory.


@param aesParams        [Input]        The AES parameters object.
@param aesKey           [Input]        The AES key object. The key must be a 
                                       valid encryption key.
@param nonceLen         [Input]        The length (in bytes) of nonce. 
                                       Acceptable values are <tt>7</tt>, 
                                       <tt>8</tt>, <tt>9</tt>, <tt>10</tt>, 
                                       <tt>11</tt>, <tt>12</tt> and 
                                       <tt>13</tt>.
@param nonce            [Input]        Nonce buffer.
@param addDataLen       [Input]        The length (in bytes) of additional data.
                                       If <tt>addDataLen</tt> is not 0, then 
                                       <tt>addData</tt> cannot be <tt>NULL</tt>.
@param addData          [Input]        Additional data buffer. This data will 
                                       be authenticated, but will not be 
                                       encrypted.
@param plaintextLen     [Input]        The length (in bytes) of plaintext. If 
                                       <tt>plaintextLen</tt> is not 0, then
                                       <tt>plaintext</tt> cannot be 
                                       <tt>NULL</tt>.
@param plaintext        [Input]        Plaintext buffer. This data will be 
                                       authenticated and encrypted.
@param macLen           [Input]        The length of the CBC-MAC used for 
                                       authentication. Acceptable values are 
                                       <tt>4</tt>, <tt>6</tt>, <tt>8</tt>, 
                                       <tt>10</tt>, <tt>12</tt>, <tt>14</tt>
                                       and <tt>16</tt>.
@param ciphertext       [Output]       The ciphertext buffer. The ciphertext 
                                       buffer must be at least 
                                       <tt>plaintext</tt> + <tt>macLen</tt> 
                                       bytes in length.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      The <tt>plaintext</tt>, <tt>nonce</tt>,
                                   or <tt>addData</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   The <tt>nonceLen</tt> is invalid.
@retval SB_ERR_BAD_LENGTH          The <tt>macLen</tt> is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF     The <tt>ciphertext</tt> buffer is 
                                   <tt>NULL</tt>.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESCCMAuthEncrypt(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t nonceLen,
    const unsigned char* nonce,
    size_t addDataLen,
    const unsigned char* addData,
    size_t plaintextLen,
    const unsigned char* plaintext,    
    size_t macLen,
    unsigned char* ciphertext,
    sb_GlobalCtx sbCtx
    );



/** Does an authenticated decryption according to the AES CCM algorithm with
the given AES parameters and key objects.
CCM is described in NIST Special Publication 800-38C.

The plaintext and ciphertext buffers may overlap 
in memory subject to the constraints described in the API Reference section
on overlapping buffers.
The adapter registered for AES must support <tt>SB_AES_ECB</tt>,
and <tt>SB_AES_CBC</tt>.
This function should only be called for buffers of plaintext and ciphertext
that can be stored entirely in memory.


@param aesParams        [Input]        The AES parameters object.
@param aesKey           [Input]        The AES key object. The key must be a 
                                       valid encryption key.
@param nonceLen         [Input]        The length (in bytes) of nonce. 
                                       Acceptable values are <tt>7</tt>, 
                                       <tt>8</tt>, <tt>9</tt>, <tt>10</tt>, 
                                       <tt>11</tt>, <tt>12</tt> and <tt>13</tt>.
@param nonce            [Input]        Nonce buffer.
@param addDataLen       [Input]        The length (in bytes) of additional data.
                                       If <tt>addDataLen</tt> is not 0, then 
                                       <tt>addData</tt> cannot be <tt>NULL</tt>.
@param addData          [Input]        Additional data buffer. This data
                                       will be authenticated, but will not be
                                       encrypted.
@param ciphertextLen    [Input]        The length (in bytes) of ciphertext. If 
                                       <tt>ciphertextLen</tt> is not 0, then 
                                       <tt>ciphertext</tt> cannot be 
                                       <tt>NULL</tt>.
@param ciphertext       [Input]        Ciphertext buffer. This data will be 
                                       authenticated and decrypted.
@param macLen           [Input]        The length of the CBC-MAC used for 
                                       authentication. Acceptable values are 
                                       <tt>0</tt>, <tt>4</tt>, <tt>6</tt>, 
                                       <tt>8</tt>, <tt>10</tt>, <tt>12</tt>, 
                                       <tt>14</tt> and <tt>16</tt>. If 
                                       <tt>macLen</tt> is 0, then 
                                       <tt>addData</tt> and <tt>ciphertext</tt>
                                       will not be authenticated.
@param plaintext        [Output]       The plaintext buffer. The plaintext 
                                       buffer must be at least 
                                       <tt>ciphertext</tt> - <tt>macLen</tt> 
                                       bytes in length.
@param sbCtx            [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>aesParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_KEY            The <tt>aesKey</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY             The <tt>aesKey</tt> object is invalid.
@retval SB_ERR_BAD_MODE            <tt>aesParams</tt> uses an invalid mode of 
                                   operation.
@retval SB_ERR_NULL_INPUT_BUF      The <tt>ciphertext</tt>, <tt>nonce</tt>,
                                   or <tt>addData</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   The <tt>nonceLen</tt> or 
                                   <tt>ciphertextLen</tt> is invalid.
@retval SB_ERR_BAD_LENGTH          The <tt>macLen</tt> is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF     The <tt>plaintext</tt> buffer is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN   <tt>ciphertextLen</tt> is not consistent
                                   with <tt>nonceLen</tt>.
@retval SB_ERR_MAC_INVALID         The MAC is invalid.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_AESCCMAuthDecrypt(
    sb_Params aesParams,
    sb_Key aesKey,
    size_t nonceLen,
    const unsigned char* nonce,
    size_t addDataLen,
    const unsigned char* addData,
    size_t ciphertextLen,
    const unsigned char* ciphertext,
    size_t macLen,
    unsigned char* plaintext,
    sb_GlobalCtx sbCtx
    );







/** @} */

#ifdef __cplusplus
}
#endif

#endif

