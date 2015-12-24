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
 * $Source: Includes/huarc4.h $
 * $Name:  $
 */
#ifndef HUARC4_H
#define HUARC4_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_arc4_api ARC4 APIs
 *
 * @{
 */

/** Key length */
#define SB_ARC4_MAX_KEY_LEN  256


/** Creates an ARC4 parameters object.

An RNG context and/or yielding context must be supplied if key generation
and/or yielding will be performed, respectively.

@param rngContext [Input]  An RNG context. (Optional - set to <tt>NULL</tt> if 
                           key generation will not be performed.)
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> if
                           yielding is not required.)
@param arc4Params [Output] The ARC4 parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>arc4Params</tt> parameters object 
                               pointer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4ParamsCreate(
    sb_RNGCtx rngContext,
    sb_YieldCtx yieldCtx,
    sb_Params *arc4Params,
    sb_GlobalCtx sbCtx
);


/** Destroys an ARC4 parameters object.

ARC4 contexts and key objects must be destroyed before their corresponding
ARC4 parameters object is destroyed.

@param arc4Params [Input/Output] ARC4 parameters object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS_PTR   The <tt>arc4Params</tt> parameters object 
                                 pointer is <tt>NULL</tt>.
@retval SB_ERR_NULL_PARAMS       The <tt>arc4Params</tt> parameters object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS 
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4ParamsDestroy(
    sb_Params *arc4Params,
    sb_GlobalCtx sbCtx
);


/** Creates an ARC4 key object from the given key value that can later be
used for encryption and/or decryption.

@param arc4Params [Input]  An ARC4 parameters object.
@param keyLen     [Input]  The length (in bytes) of the ARC4 key value. The 
                           maximum allowable length is 
                           <tt>SB_ARC4_MAX_KEY_LEN</tt>.
@param keyValue   [Input]  The key value.
@param arc4Key    [Output] The ARC4 key object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS    The <tt>arc4Params</tt> parameters object is 
                              <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_BAD_KEY_LEN    <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_INPUT_BUF Key value is <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY_PTR   The <tt>arc4Key</tt> object pointer is 
                              <tt>NULL</tt>.
@retval SB_FAIL_ALLOC         Memory allocation failure.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4KeySet(
    sb_Params arc4Params,
    size_t keyLen,
    const unsigned char *keyValue,
    sb_Key *arc4Key,
    sb_GlobalCtx sbCtx
);


/** Creates an ARC4 key object of the specified length from random data that
can later be used for encryption and/or decryption.

The ARC4 parameter object must have been created with an RNG context.

@param arc4Params [Input]  An ARC4 parameters object.
@param keyLen     [Input]  The length (in bytes) of the ARC4 key value. The 
                           maximum allowable length is 
                           <tt>SB_ARC4_MAX_KEY_LEN</tt>.
@param arc4Key    [Output] The ARC4 key object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS  The <tt>arc4Params</tt> parameters object is 
                            <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_BAD_KEY_LEN  <tt>keyLen</tt> is an invalid key length.
@retval SB_ERR_NULL_KEY_PTR The <tt>arc4Key</tt> object pointer is 
                            <tt>NULL</tt>.
@retval SB_FAIL_ALLOC       Memory allocation failure.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4KeyGen(
   sb_Params arc4Params,
   size_t keyLen,
   sb_Key *arc4Key,
   sb_GlobalCtx sbCtx
);


/** Retrieves the key value and its length from an ARC4 key object.

If the length of the key value is known, a pointer to a buffer large enough
to hold the key value should be passed in <tt>keyValue</tt> and its
length in <tt>keyLen</tt>. This function will copy the key value into
<tt>keyValue</tt> and set the actual length of the key value in
<tt>keyLen</tt>.

If <tt>keyValue</tt> is <tt>NULL</tt>, then this function will set the
correct length of the key value in <tt>keyLen</tt>.  If
<tt>keyValue</tt> is not <tt>NULL</tt> but <tt>keyLen</tt> is too
small, this function will return an error and also will set the correct length
of the key value in <tt>keyLen</tt>.

@param arc4Params [Input]        An ARC4 parameters object.
@param arc4Key    [Input]        An ARC4 key object.
@param keyLen     [Input/Output] The length (in bytes) of the ARC4 key value.
@param keyValue   [Output]       The key value.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_KEY           The <tt>arc4Key</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY            The <tt>arc4Key</tt> object is invalid.
@retval SB_ERR_NULL_KEY_LEN       <tt>keyLen</tt> is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the <tt>keyValue</tt> buffer is
                                  invalid.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4KeyGet(
   sb_Params arc4Params,
   sb_Key arc4Key,
   size_t *keyLen,
   unsigned char *keyValue,
   sb_GlobalCtx sbCtx
);


/** Destroys an ARC4 key object.

ARC4 contexts must be destroyed before any ARC4 key objects. ARC4 parameter
objects must be destroyed after ARC4 key objects.

@param arc4Params [Input]        An ARC4 parameters object.
@param arc4Key    [Input/Output] An ARC4 key object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS  The <tt>arc4Params</tt> parameters object is 
                            <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_NULL_KEY_PTR The <tt>arc4Key</tt> object pointer is 
                            <tt>NULL</tt>.
@retval SB_ERR_NULL_KEY     The <tt>arc4Key</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY      The <tt>arc4Key</tt> object is invalid.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4KeyDestroy(
    sb_Params arc4Params,
    sb_Key *arc4Key,
    sb_GlobalCtx sbCtx
);


/** Creates an ARC4 context initialized with the given parameters and key
that can later be used for encryption and/or decryption.

@param arc4Params  [Input]  An ARC4 parameters object.
@param arc4Key     [Input]  An ARC4 key object.
@param arc4Context [Output] The ARC4 context object pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>arc4Params</tt> parameters object is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS       
@retval SB_ERR_NULL_KEY         <tt>arc4Key</tt> is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY          <tt>arc4Key</tt> is invalid.
@retval SB_ERR_NULL_CONTEXT_PTR The <tt>sbCtx</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4Begin(
   sb_Params arc4Params,
   sb_Key arc4Key,
   sb_Context *arc4Context,
   sb_GlobalCtx sbCtx
);


/** Encrypts a plaintext buffer using the given ARC4 context.

The plaintext and ciphertext buffers must be the same length. They may point
to the same address in memory but must not otherwise overlap. This function
can be called repeatedly to encrypt more blocks of plaintext.

@param arc4Context [Input]  An ARC4 context object.
@param length      [Input]  The length (in bytes) of plaintext.
@param plaintext   [Input]  The plaintext buffer.
@param ciphertext  [Output] The ciphertext buffer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT      The <tt>arc4Context</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT       The <tt>arc4Context</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the <tt>plaintext</tt> buffer is
                                 invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4Encrypt(
    sb_Context arc4Context,
    size_t length,
    const unsigned char *plaintext,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts a ciphertext buffer using the given ARC4 context.

The plaintext and ciphertext buffers must be the same length. They may point
to the same address in memory but must not otherwise overlap. This function
can be called repeatedly to decrypt more blocks of ciphertext.

@param arc4Context [Input]  An ARC4 context object.
@param length      [Input]  The length (in bytes) of ciphertext.
@param ciphertext  [Input]  The ciphertext buffer.
@param plaintext   [Output] The plaintext buffer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT      The <tt>arc4Context</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT       The <tt>arc4Context</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the <tt>ciphertext</tt> buffer is
                                 invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4Decrypt(
   sb_Context arc4Context,
   size_t length,
   const unsigned char *ciphertext,
   unsigned char *plaintext,
   sb_GlobalCtx sbCtx
);


/** Destroys an ARC4 context object.

This function must be called to terminate an encryption or decryption
operation. An ARC4 context must be destroyed before the corresponding ARC4
key object and ARC4 parameters object are destroyed.

@param arc4Context [Input/Output] An ARC4 context object pointer.
@param sbCtx       [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>arc4Context</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT     The <tt>arc4Context</tt> object is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      The <tt>arc4Context</tt> object is invalid.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4End(
    sb_Context *arc4Context,
    sb_GlobalCtx sbCtx
);


/** Encrypts a plaintext buffer using the given ARC4 parameters and key.

<em>Note</em>: This function should only be called for buffers of plaintext 
and ciphertext that can be stored entirely in memory.

@param arc4Params [Input]  An ARC4 parameters object.
@param arc4Key    [Input]  An ARC4 key object.
@param length     [Input]  The length (in bytes) of plaintext.
@param plaintext  [Input]  The plaintext buffer.
@param ciphertext [Output] The ciphertext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>arc4Params</tt> parameters object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_KEY          The <tt>arc4Key</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY           The <tt>arc4Key</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>plaintext</tt> buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The <tt>plaintext</tt> buffer length is 
                                 invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4EncryptMsg(
    sb_Params arc4Params,
    sb_Key arc4Key,
    size_t length,
    const unsigned char *plaintext,
    unsigned char *ciphertext,
    sb_GlobalCtx sbCtx
);


/** Decrypts a ciphertext buffer using the given ARC4 parameters and key.

This function should only be called for buffers of plaintext and ciphertext
that can be stored entirely in memory.

@param arc4Params [Input]  An ARC4 parameters object.
@param arc4Key    [Input]  An ARC4 key object.
@param length     [Input]  The length (in bytes) of ciphertext.
@param ciphertext [Input]  The ciphertext buffer.
@param plaintext  [Output] The plaintext buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS The <tt>arc4Params</tt> parameters object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS   
@retval SB_ERR_NULL_KEY          The <tt>arc4Key</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_KEY           The <tt>arc4Key</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The <tt>ciphertext</tt> buffer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the <tt>ciphertext</tt> buffer 
                                 is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF   The plaintext buffer is <tt>NULL</tt>.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_ARC4DecryptMsg(
    sb_Params arc4Params,
    sb_Key arc4Key,
    size_t length,
    const unsigned char *ciphertext,
    unsigned char *plaintext,
    sb_GlobalCtx sbCtx
);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
