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
 * $Source: Includes/humd5.h $
 * $Name:  $
 */
#ifndef HUMD5_H
#define HUMD5_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_md5_api MD5 APIs
 *
 * @{
 */

#define SB_MD5_DIGEST_LEN 16

#define SB_HMAC_MD5_128_TAG_LEN  SB_MD5_DIGEST_LEN
#define SB_HMAC_MD5_64_TAG_LEN   (SB_MD5_DIGEST_LEN/2)


/** Creates an MD5 context object.

<em>Note</em>: Yielding is not supported for message digest operations.

@param digestLen   [Input]  The length (in bytes) of an MD5 digest. The only 
                            acceptable value is <tt>SB_MD5_DIGEST_LEN</tt>.
@param yieldCtx    [Input]  Ignored.
@param hashContext [Output] The MD5 context object pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_BAD_DIGEST_LEN   <tt>digestLen</tt> is an invalid digest length.
@retval SB_ERR_NULL_CONTEXT_PTR The <tt>hashContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5Begin(
    size_t digestLen,
    sb_YieldCtx yieldCtx,
    sb_Context *hashContext,
    sb_GlobalCtx sbCtx
);


/** Updates an MD5 context with the given data.

@param hashContext [Input] The MD5 context object.
@param messageLen  [Input] The length (in bytes) of the data.
@param messageData [Input] A data buffer.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT The  <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF   <tt>messageData</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5Hash(
    sb_Context hashContext,
    size_t messageLen,
    const unsigned char *messageData,
    sb_GlobalCtx sbCtx
);


/** Completes the message digest operation by generating the digest
and destroying the MD5 context object.

@param hashContext [Input/Output] The MD5 context object pointer.
@param digest      [Output]       The message digest buffer. The length in ]
                                  bytes of the buffer must be at least 
                                  <tt>SB_MD5_DIGEST_LEN</tt>.
@param sbCtx       [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>hashContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT     The <tt>hashContext</tt> object is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      The <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF  <tt>digest</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5End(
    sb_Context *hashContext,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Generates the MD5 message digest for the given data.

<em>Note</em>: Yielding is not supported for message digest operations.

<em>Note</em>: This function should only be called for data that can be stored 
entirely in memory.

@param digestLen   [Input]  The length (in bytes) of an MD5 digest. The only 
                            acceptable value is <tt>SB_MD5_DIGEST_LEN</tt>.
@param yieldCtx    [Input]  Ignored.
@param messageLen  [Input]  The length (in bytes) of the data.
@param messageData [Input]  The data buffer.
@param digest      [Output] The message digest buffer. The length (in bytes) of 
                            the buffer must be at least 
                            <tt>SB_MD5_DIGEST_LEN</tt>.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_BAD_DIGEST_LEN  <tt>digestLen</tt> is an invalid digest length.
@retval SB_ERR_NULL_INPUT_BUF  <tt>messageData</tt> is <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF <tt>digest</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5Msg(
    size_t digestLen,
    sb_YieldCtx yieldCtx,
    size_t messageLen,
    const unsigned char *messageData,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Generates the message digest from the data that has been updated
to the given MD5 context.

This function does not change the state of the context object.

@param hashContext [Input]  An MD5 context object.
@param digest      [Output] The message digest buffer. The length (in bytes) of 
                            the buffer must be at least 
                            <tt>SB_MD5_DIGEST_LEN</tt>.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT    The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT     The <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF <tt>digest</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5DigestGet(
    sb_Context hashContext,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Resets the given MD5 context so it can be reused.

This function returns the state of the context to the same state as
a context created after a call to <tt>hu_MD5Begin()</tt>.

@param hashContext [Input] An MD5 context object.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT  The <tt>hashContext</tt> object is invalid.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5CtxReset(
    sb_Context hashContext,
    sb_GlobalCtx sbCtx
);


/** Duplicates an MD5 context.

This function can be used to compute the digest of multiple items of data
with identical prefixes.

@param hashContext    [Input]  The original MD5 context object.
@param newHashContext [Output] The target MD5 context object pointer.
@param sbCtx          [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT     The original MD5 context object is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      The original MD5 context object is invalid.
@retval SB_ERR_NULL_CONTEXT_PTR The target MD5 context object pointer is 
                                <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_MD5CtxDuplicate(
    sb_Context hashContext,
    sb_Context *newHashContext,
    sb_GlobalCtx sbCtx
);


/** @} */

/** @addtogroup hu_hmac_md5_api HMAC MD5 APIs
 *
 * @{
 */


/** Creates an HMAC-MD5 context object with the given key.

<em>Note</em>: Yielding is not supported for HMAC operations.

@param keyLen      [Input]  The length (in bytes) of the key.
@param key         [Input]  The key value.
@param yieldCtx    [Input]  Ignored.
@param hmacContext [Output] The HMAC-MD5 context object pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>hmacContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF   The <tt>key</tt> value is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACMD5Begin(
   size_t keyLen,
   const unsigned char *key,
   sb_YieldCtx yieldCtx,
   sb_Context *hmacContext,
   sb_GlobalCtx sbCtx
);


/** Updates an HMAC-MD5 context with the given data.

@param hmacContext [Input] An HMAC-MD5 context object.
@param dataBlkLen  [Input] The length (in bytes) of the data.
@param dataBlk     [Input] The data buffer.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT   The <tt>hmacContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT    The <tt>hmacContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF <tt>dataBlk</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACMD5Hash(
    sb_Context hmacContext,
    size_t dataBlkLen,
    const unsigned char *dataBlk,
    sb_GlobalCtx sbCtx
);


/** Completes the HMAC operation by generating a MAC tag of the specified
length for the given data and destroying the HMAC-MD5 context.

@param hmacContext [Input/Output] The HMAC-MD5 context object pointer.
@param tagLen      [Input]        The length (in bytes) of MAC tag. This value 
                                  must be between 
                                  <tt>SB_HMAC_MD5_64_TAG_LEN</tt> and 
                                  <tt>SB_HMAC_MD5_128_TAG_LEN</tt>, inclusive.
@param tag         [Output]       The MAC tag buffer.
@param sbCtx       [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR   The <tt>hmacContext</tt> object pointer is 
                                  <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT       The <tt>hmacContext</tt> object is 
                                  <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT        The <tt>hmacContext</tt> object is invalid.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN <tt>tagLen</tt> is an invalid MAC tag length.
@retval SB_ERR_NULL_OUTPUT_BUF    <tt>tag</tt> is <tt>NULL</tt>.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACMD5End(
    sb_Context *hmacContext,
    size_t tagLen,
    unsigned char *tag,
    sb_GlobalCtx sbCtx
);


/** Generates a MAC tag of the specified length for the given data using
HMAC-MD5.

<em>Note</em>: Yielding is not supported for message digest operations.

<em>Note</em>: This function should only be called for data that can be stored 
entirely in memory.

@param keyLen     [Input]  The length (in bytes) of the key.
@param key        [Input]  A key value.
@param yieldCtx   [Input]  Ignored.
@param dataBlkLen [Input]  The length (in bytes) of the data.
@param dataBlk    [Input]  The data buffer.
@param tagLen     [Input]  The length (in bytes) of MAC tag. This value must be 
                           between <tt>SB_HMAC_MD5_64_TAG_LEN</tt> and 
                           <tt>SB_HMAC_MD5_128_TAG_LEN</tt>, inclusive.
@param tag        [Output] The MAC tag buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_KEY           <tt>key</tt> is <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF     <tt>dataBlk</tt> is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The tag buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF    The tag buffer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC             Memory allocation failure.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACMD5Msg(
    size_t keyLen,
    const unsigned char *key,
    sb_YieldCtx yieldCtx,
    size_t dataBlkLen,
    const unsigned char *dataBlk,
    size_t tagLen,
    unsigned char *tag,
    sb_GlobalCtx sbCtx
);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
