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
 * $Source: Includes/husha1.h $
 * $Name:  $
 */
#ifndef HUSHA1_H
#define HUSHA1_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_sha1_api SHA-1 APIs
 *
 * @{
 */

#define SHA1_MSG_BLK_LEN 64
#define SB_SHA1_DIGEST_LEN 20

#define SB_HMAC_SHA1_160_TAG_LEN  SB_SHA1_DIGEST_LEN
#define SB_HMAC_SHA1_80_TAG_LEN   (SB_SHA1_DIGEST_LEN/2)


/** Creates a SHA-1 context object.

<em>Note</em>: Yielding is not supported for message digest operations.

@param digestLen   [Input]  The length (in bytes) of a SHA-1 digest. The only 
                            acceptable value is <tt>SB_SHA1_DIGEST_LEN</tt>.
@param yieldCtx    [Input]  Ignored.
@param hashContext [Output] The SHA-1 context object pointer.
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
hu_SHA1Begin(
    size_t digestLen,
    sb_YieldCtx yieldCtx,
    sb_Context *hashContext,
    sb_GlobalCtx sbCtx
);


/** Updates a SHA-1 context with the given data.

@param hashContext [Input] The SHA-1 context object.
@param messageLen  [Input] The length (in bytes) of the data.
@param messageData [Input] The data buffer.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT   The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT    The <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF The <tt>messageData</tt> data buffer is 
                              <tt>NULL</tt>.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1Hash(
    sb_Context hashContext,
    size_t messageLen,
    const unsigned char *messageData,
    sb_GlobalCtx sbCtx
);


/** Completes the message digest operation by generating the digest
and destroying the SHA-1 context object.

@param hashContext [Input/Output] The SHA-1 context object pointer.
@param digest      [Output]       The message digest buffer. The length (in 
                                  bytes) of the buffer must be at least 
                                  <tt>SB_SHA1_DIGEST_LEN</tt>.
@param sbCtx       [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>hashContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT     The <tt>hashContext</tt> object is 
                                <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      The <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF  The message digest buffer is <tt>NULL</tt>.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1End(
    sb_Context *hashContext,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Generates the SHA-1 message digest for the given data.

<em>Note</em>: Yielding is not supported for message digest operations.

<em>Note</em>: This function should only be called for data that can be stored 
entirely in memory.

@param digestLen   [Input]  The length (in bytes) of a SHA-1 digest. The only 
                            acceptable value is <tt>SB_SHA1_DIGEST_LEN</tt>.
@param yieldCtx    [Input]  Ignored.
@param messageLen  [Input]  The length (in bytes) of the data.
@param messageData [Input]  The data buffer.
@param digest      [Output] The message digest buffer. The length (in bytes) 
                            of the buffer must be at least 
                            <tt>SB_SHA1_DIGEST_LEN</tt>.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_BAD_DIGEST_LEN  The length of the digest, <tt>messageLen</tt>, 
                               is invalid.
@retval SB_ERR_NULL_INPUT_BUF  The <tt>messageData</tt> data buffer is 
                               <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF <tt>digest</tt>, the message digest buffer, is 
                               <tt>NULL</tt>.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1Msg(
    size_t digestLen,
    sb_YieldCtx yieldCtx,
    size_t messageLen,
    const unsigned char *messageData,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Generates the message digest from the data that has been updated
to the given SHA-1 context.

<em>Note</em>: This function does not change the state of the context object.

@param hashContext [Input]  The SHA-1 context object.
@param digest      [Output] The message digest buffer. The length (in bytes) 
                            of the buffer must be at least 
                            <tt>SB_SHA1_DIGEST_LEN</tt>.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT    The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT     The <tt>hashContext</tt> object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF The message digest buffer is <tt>NULL</tt>.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1DigestGet(
    sb_Context hashContext,
    unsigned char *digest,
    sb_GlobalCtx sbCtx
);


/** Resets the given SHA-1 context so it can be reused.

<em>Note</em>: This function returns the state of the context to the same state
as a context created after a call to <tt>hu_SHA1Begin()</tt>.

@param hashContext [Input] SHA-1 context object.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT The <tt>hashContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT  The <tt>hashContext</tt> object is invalid.
@retval SB_SUCCESS          Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1CtxReset(
    sb_Context hashContext,
    sb_GlobalCtx sbCtx
);


/** Duplicates a SHA-1 context.

This function can be used to compute the digest of multiple items of data
with identical prefixes.

@param hashContext    [Input]  The original SHA-1 context object.
@param newHashContext [Output] The target SHA-1 context object pointer.
@param sbCtx          [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT     <tt>hashContext</tt>, the original SHA-1 
                                context object, is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      <tt>hashContext</tt>, the original SHA-1 
                                context object, is invalid.
@retval SB_ERR_NULL_CONTEXT_PTR <tt>newHashContext</tt>, the target SHA-1 
                                context object pointer, is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SHA1CtxDuplicate(
    sb_Context hashContext,
    sb_Context *newHashContext,
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hu_hmac_sha1_api HMAC SHA-1 APIs
 *
 * @{
 */


/** Creates an HMAC-SHA-1 context object with the given key.

<em>Note</em>: Yielding is not supported for HMAC operations.

@param keyLen      [Input]  The length (in bytes) of the key.
@param key         [Input]  The key value.
@param yieldCtx    [Input]  Ignored.
@param hmacContext [Output] The HMAC-SHA-1 context object pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The <tt>hmacContext</tt> object pointer is 
                                <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF   The key value is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACSHA1Begin(
   size_t keyLen,
   const unsigned char *key,
   sb_YieldCtx yieldCtx,
   sb_Context *hmacContext,
   sb_GlobalCtx sbCtx
);


/** Updates an HMAC-SHA-1 context with the given data.

@param hmacContext [Input] The HMAC-SHA-1 context object.
@param dataBlkLen  [Input] The length (in bytes) of the data.
@param dataBlk     [Input] The data buffer.
@param sbCtx       [Input] A global context.

@retval SB_ERR_NULL_CONTEXT   The <tt>hmacContext</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT    The <tt>hmacContext</tt> object is invalid.
@retval SB_ERR_NULL_INPUT_BUF The data buffer is <tt>NULL</tt>.
@retval SB_SUCCESS            Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACSHA1Hash(
    sb_Context hmacContext,
    size_t dataBlkLen,
    const unsigned char *dataBlk,
    sb_GlobalCtx sbCtx
);


/** Completes the HMAC operation by generating a MAC tag of the specified
length for the given data and destroying the HMAC-SHA-1 context.

@param hmacContext [Input/Output] The HMAC-SHA-1 context object pointer.
@param tagLen      [Input]        The length (in bytes) of MAC tag. This value 
                                  must be between 
                                  <tt>SB_HMAC_SHA1_80_TAG_LEN</tt> and 
                                  <tt>SB_HMAC_SHA1_160_TAG_LEN</tt> inclusive.
@param tag         [Output]       The MAC tag buffer.
@param sbCtx       [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR   The <tt>hmacContext</tt> object pointer is 
                                  <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT       The <tt>hmacContext</tt> object is 
                                  <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT        The <tt>hmacContext</tt> object is invalid.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The tag buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF    The tag buffer is <tt>NULL</tt>.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_HMACSHA1End(
    sb_Context *hmacContext,
    size_t tagLen,
    unsigned char *tag,
    sb_GlobalCtx sbCtx
);


/** Generates a MAC tag of the specified length for the given data using
HMAC-SHA-1.

<em>Note</em>: Yielding is not supported for message digest operations.

<em>Note</em>: This function should only be called for data that can be stored 
entirely in memory.

@param keyLen     [Input]  The length (in bytes) of the key.
@param key        [Input]  The key value.
@param yieldCtx   [Input]  Ignored.
@param dataBlkLen [Input]  The length (in bytes) of the data.
@param dataBlk    [Input]  The data buffer.
@param tagLen     [Input]  The length (in bytes) of MAC tag. This value must be
                           between <tt>SB_HMAC_SHA1_80_TAG_LEN</tt> and 
                           <tt>SB_HMAC_SHA1_160_TAG_LEN</tt>, inclusive.
@param tag        [Output] The MAC tag buffer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_KEY           The key value is <tt>NULL</tt>.
@retval SB_ERR_NULL_INPUT_BUF     The data buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The tag buffer length is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF    The tag buffer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC             Memory allocation failure.
@retval SB_SUCCESS                Success.
*/

int
SB_CALLCONV
hu_HMACSHA1Msg(
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
