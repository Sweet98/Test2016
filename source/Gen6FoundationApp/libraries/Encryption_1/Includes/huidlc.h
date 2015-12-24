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
 * $Source: Includes/huidlc.h $
 * $Name:  $
 */
#ifndef HUIDLC_H
#define HUIDLC_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_idlc_params IDLC Parameters
 *
 * @{
 */

/** ID for IPSec Group 1 (RFC 2409 First Oakley Group). */
#define HU_IDLC_GROUP_IPSEC_1  1

/** ID for IPSec Group 2 (RFC 2409 Second Oakley Group). */
#define HU_IDLC_GROUP_IPSEC_2  2

/** ID for IPSec Group 5 (RFC 3526 Group 5). */
#define HU_IDLC_GROUP_IPSEC_5  3

/** ID for WTLS Diffie-Hellman Group 1. */
#define HU_IDLC_GROUP_WTLS_1   4

/** ID for WTLS Diffie-Hellman Group 2. */
#define HU_IDLC_GROUP_WTLS_2   5

/** ID for IPSec Group 14 (RFC 3526 Group 14). */
#define HU_IDLC_GROUP_IPSEC_14  6

/** ID for IPSec Group 15 (RFC 3526 Group 15). */
#define HU_IDLC_GROUP_IPSEC_15  7

/** ID for IPSec Group 16 (RFC 3526 Group 16). */
#define HU_IDLC_GROUP_IPSEC_16  8

/** ID for IPSec Group 17 (RFC 3526 Group 17). */
#define HU_IDLC_GROUP_IPSEC_17  9

/** ID for IPSec Group 18 (RFC 3526 Group 18). */
#define HU_IDLC_GROUP_IPSEC_18  10


/** Creates an IDLC parameters object, generating the parameters randomly,
that can later be used for DH or DSA.

An RNG context must be supplied. A yielding context must be supplied if
yielding will be performed.

This function will generate values p, q, and g such that p and q are prime,
q is a divisor of (p-1), and g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be greater than or equal to 512.
@param qSize      [Input]  The length (in bits) of the prime divisor q. The 
                           length must be greater than or equal to 160 and 
			   less than <tt>pSize</tt>.
@param rngCtx     [Input]  An RNG context.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> 
                           if yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>idlcParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT       The length (in bits) of p or q was invalid.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCParamsGen(
    size_t pSize,
    size_t qSize,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);


/** Creates an IDLC parameters object according to ANSI X9.42 specifications,
generating the parameters randomly, that can later be used for DH or DSA.

An RNG context must be supplied. A yielding context must be supplied if
yielding will be performed.

This function will generate values p, q and g such that p and q are prime,
q is a divisor of (p-1) and g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be greater than or equal to 512.
@param qSize      [Input]  The length (in bits) of the prime divisor q. The 
                           length must be greater than or equal to 160 and 
			   less than <tt>pSize</tt>.
@param rngCtx     [Input]  An RNG context.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> 
                           if yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>idlcParams</tt> object pointer is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT       The length of p and/or q was invalid.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCANSIParamsGen(
    size_t pSize,
    size_t qSize,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);


/** Creates an IDLC parameters object according to FIPS 186 specifications,
generating the parameters randomly, that can later be used for DH or DSA.

An RNG context must be supplied. A yielding context must be supplied if
yielding will be performed.

This function will generate values p, q, and g such that p and q are prime,
q is a divisor of (p-1) and, g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be between 512 and 1024 inclusive, and a
			   multiple of 64.
@param qSize      [Input]  The length (in bits) of the prime divisor q. The 
                           only acceptable value is 160.
@param rngCtx     [Input]  An RNG context.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> 
                           if yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>idlcParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT       The length of p and/or q was invalid.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCFIPSParamsGen(
    size_t pSize,
    size_t qSize,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);

/** Creates an IDLC parameters object from the given values, that can later
be used for DSA.

An RNG context must be supplied. A yielding context must be supplied if
yielding will be performed.

The values should satisfy the following properties: p and q are prime, q is a
divisor of (p-1), and g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be greater than or equal to 512.
@param qSize      [Input]  The length (in bits) of the prime divisor q. The 
                           length must be greater than or equal to 160 and less 
			   than <tt>pSize</tt>.
@param pLength    [Input]  The length (in bytes) of the prime modulus.
@param p          [Input]  Prime modulus.
@param gLength    [Input]  The length (in bytes) of the generator. Must be less 
                           than or equal to <tt>pLength</tt>.
@param g          [Input]  Generator of order q.
@param qLength    [Input]  The length (in bytes) of the prime divisor.
@param q          [Input]  Prime divisor of p-1.
@param rngCtx     [Input]  An RNG context.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> 
                           if yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR   The <tt>idlcParams</tt> object pointer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_NULL_IDLC_P       The prime modulus value is <tt>NULL</tt>.
@retval SB_ERR_NULL_IDLC_Q       The prime divisor value is <tt>NULL</tt>.
@retval SB_ERR_NULL_IDLC_G       The generator value is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT         The length of p and/or q is invalid.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the p, q and/or g buffers is 
                                 invalid.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDSAParamsSet(
    size_t pSize,
    size_t qSize,
    size_t pLength,
    const unsigned char *p,
    size_t gLength,
    const unsigned char *g,
    size_t qLength,
    const unsigned char *q,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);

/** Creates an IDLC parameters object from the given values, that can later
be used for DH.

An RNG context and/or yielding context must be supplied if key
generation and/or yielding will be performed, respectively.

The values should satisfy the following properties: p and q are prime, q is a
divisor of (p-1), and g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

<tt>q</tt> is optional. If <tt>q</tt> is not supplied, then
<tt>qSize</tt> specifies the size of private keys to be used with these
parameters. In this case, if <tt>qSize</tt> is set to zero, the default
size of 160 bits will be used.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be greater than or equal to 512.
@param qSize      [Input]  The length (in bits) of the prime divisor q, if 
                           using. Otherwise, the length (in bits) of the 
			   private keys.
@param pLength    [Input]  The length (in bytes) of the prime modulus.
@param p          [Input]  Prime modulus.
@param gLength    [Input]  The length (in bytes) of the generator. Must be less
                           than or equal to <tt>pLength</tt>.
@param g          [Input]  Generator of order q.
@param qLength    [Input]  The length (in bytes) of the prime divisor. 
                           (Optional)
@param q          [Input]  Prime divisor of p-1. (Optional)
@param rngCtx     [Input]  An RNG context. (Optional - set to <tt>NULL</tt> if 
                           key generation will not be performed.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> if 
                           yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR   The <tt>idlcParams</tt> object pointer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT         The length of p and/or q is invalid.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the p, q and/or g buffers is 
                                 invalid.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDHParamsSet(
   size_t pSize,
   size_t qSize,
   size_t pLength,
   const unsigned char *p,
   size_t gLength,
   const unsigned char *g,
   size_t qLength,
   const unsigned char *q,
   sb_RNGCtx rngCtx,
   sb_YieldCtx yieldCtx,
   sb_Params *idlcParams,
   sb_GlobalCtx sbCtx
);


/** Creates an IDLC parameters object from the given values, that can later
be used for DH or DSA.

An RNG context must be supplied if key generation or DSA will be performed.
A yielding context must be supplied if yielding will be performed.

The values should satisfy the following properties: p and q are prime, q is a
divisor of (p-1), and g has order q modulo p. p will have bitlength
<tt>pSize</tt> and q will have bitlength <tt>qSize</tt>.

<tt>q</tt> is optional. If <tt>q</tt> is not supplied, then
<tt>qSize</tt> specifies the size of private keys to be used with these
parameters. In this case, if <tt>qSize</tt> is set to zero, the default
size of 160 bits will be used.

@param pSize      [Input]  The length (in bits) of the prime modulus p. The 
                           length must be greater than or equal to 512.
@param qSize      [Input]  The length (in bits) of the prime divisor q, if 
                           using. Otherwise, the length (in bits) of the 
			   private keys.
@param pLength    [Input] The length (in bytes) of the prime modulus.
@param p          [Input]  Prime modulus.
@param gLength    [Input]  The length (in bytes) of the generator. Must be less
                           than or equal to <tt>pLength</tt>.
@param g          [Input]  Generator of order q.
@param qLength    [Input]  The length (in bytes) of the prime divisor. 
                           (Optional)
@param q          [Input]  Prime divisor of p-1. (Optional)
@param rngCtx     [Input]  An RNG context. (Optional - set to <tt>NULL</tt> if 
                           key generation and DSA will not be performed.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> if
                           yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR   The <tt>idlcParams</tt> object pointer is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT         The length of p and/or q is invalid.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the p, q and/or g buffers is 
                                 invalid.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCParamsSet(
    size_t pSize,
    size_t qSize,
    size_t pLength,
    const unsigned char *p,
    size_t gLength,
    const unsigned char *g,
    size_t qLength,
    const unsigned char *q,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);


/** Creates an IDLC parameters object for the specified parameters, that can
later be used for DH.

An RNG context must be supplied if key generation will be performed.
A yielding context must be supplied if yielding will be performed.

For IPSec Groups 1, 2 and 5, and wTLS Groups 1 and 2, private keys may be as
large as 160 bits.

For IPSec Group 14, private keys may be as large as 224 bits; for IPSec Group
15, 264 bits; for IPSec Group 16, 304 bits; for IPSec 17, 344 bits; for IPSec
Group 18, 384 bits.

@param paramset   [Input]  A parameter set identifier. The acceptable values 
                           are one of the <tt>HU_IDLC_GROUP_*</tt> macros.
@param rngCtx     [Input]  An RNG context. (Optional - set to <tt>NULL</tt> if 
                           key generation will not be performed.
@param yieldCtx   [Input]  A yield context. (Optional - set to <tt>NULL</tt> if
                           yielding is not required.
@param idlcParams [Output] The IDLC parameters object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>idlcParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_IDLC_BAD_GROUP  The parameter set identifier is invalid.
@retval SB_FAIL_ALLOC          Memory allocation failure.
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCParamsCreate(
    int paramset,
    sb_RNGCtx rngCtx,
    sb_YieldCtx yieldCtx,
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);


/** Retrieves settings from an IDLC parameters object.

If the length of a parameter is known, a pointer to a buffer large enough to
hold the parameter should be passed in the appropriate argument and its
length in the corresponding length argument. This function will copy the
value into the buffer and set the actual length of the value in
the length argument.

If a parameter argument is <tt>NULL</tt>, this function will set the
correct length of the parameter in the length argument.  If the argument is not
<tt>NULL</tt> but the corresponding length argument is too small, this
function will return an error and set the correct length of the parameter in
the length argument.

Set both the parameter argument and its length to <tt>NULL</tt> for
any parameters that are to be ignored.

@param idlcParams [Input]  An IDLC parameters object.
@param pSize      [Output] The length (in bits) of the prime modulus p.
@param qSize      [Output] The length (in bits) of the prime divisor q, if 
                           using. Otherwise, the length (in bits) of the 
			   private keys.
@param pLength    [Output] The length (in bytes) of the prime modulus.
@param p          [Output] Prime modulus.
@param gLength    [Output] The length (in bytes) of the generator. Equal to <tt>pLength</tt>.
@param g          [Output] Generator of order q.
@param qLength    [Output] The length (in bytes) of the prime divisor, if using.
                           Otherwise, 0 will be returned.
@param q          [Output] Prime divisor of p-1, if using.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS        The <tt>idlcParams</tt> object is 
                                  <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS         
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the output buffer is invalid.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCParamsGet(
    sb_Params idlcParams,
    size_t *pSize,
    size_t *qSize,
    size_t *pLength,
    unsigned char *p,
    size_t *gLength,
    unsigned char *g,
    size_t *qLength,
    unsigned char *q,
    sb_GlobalCtx sbCtx
);


/** Destroys an IDLC parameters object.

IDLC contexts and key objects must be destroyed before their corresponding
IDLC parameters object is destroyed.

@param idlcParams [Input/Output] The IDLC parameters object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS_PTR The <tt>idlcParams</tt> object pointer is 
                               <tt>NULL</tt>.
@retval SB_ERR_NULL_PARAMS     The <tt>idlcParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS      
@retval SB_SUCCESS             Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCParamsDestroy(
    sb_Params *idlcParams,
    sb_GlobalCtx sbCtx
);

/** @} */

/** @addtogroup hu_idlc_key IDLC Keys
 *
 * @{
 *
 */

/** Creates an IDLC private and/or public key object from the given key
value(s).

If both private and public key values are supplied, a private key and/or a
public key object can be created. The key objects will be created from the
corresponding key values.

If only a private key value is supplied, a private key and/or a public key
object can be created. The public key will be computed from the private key.

If only a public key value is supplied, only a public key object can be
created.
 
@param idlcParams      [Input]  IDLC parameters object.
@param privateKeyLen   [Input]  The length (in bytes) of the private key value.
@param privateKeyValue [Input]  The private key value.
@param publicKeyLen    [Input]  The length (in bytes) of the public key value.
@param publicKeyValue  [Input]  The public key value.
@param privateKey      [Output] The private key object pointer.
@param publicKey       [Output] The public key object pointer.
@param sbCtx           [Input]  A global context.

@retval SB_ERR_NULL_PARAMS          The <tt>idlcParams</tt> object is 
                                    <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS           
@retval SB_ERR_NULL_PRI_KEY_BUF     The private key value is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRI_KEY_BUF_LEN  The private key length is invalid.
@retval SB_ERR_NULL_PUB_KEY_BUF     The public key value is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUB_KEY_BUF_LEN  The public key length is invalid.
@retval SB_FAIL_INVALID_PRIVATE_KEY The private key value is invalid.
@retval SB_FAIL_ALLOC               Memory allocation failure.
@retval SB_SUCCESS                  Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCKeySet(
    sb_Params idlcParams,
    size_t privateKeyLen,
    const unsigned char *privateKeyValue,
    size_t publicKeyLen,
    const unsigned char *publicKeyValue,
    sb_PrivateKey *privateKey,
    sb_PublicKey *publicKey,
    sb_GlobalCtx sbCtx
);


/** Creates an IDLC private key object from random data, and, optionally,
the corresponding public key object.

The IDLC parameter object must have been created with an RNG context.

@param idlcParams [Input]  An IDLC parameters object.
@param privateKey [Output] The private key object pointer.
@param publicKey  [Output] The public key object pointer.
@param sbCtx      [Input]  A global context.

@retval SB_ERR_NULL_PARAMS The <tt>idlcParams</tt> object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS  
@retval SB_FAIL_ALLOC      Memory allocation failure.
@retval SB_SUCCESS         Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCKeyGen(
    sb_Params idlcParams,
    sb_PrivateKey *privateKey,
    sb_PublicKey *publicKey,
    sb_GlobalCtx sbCtx
);


/** Retrieves the key values and their lengths from an IDLC private
and/or public key object.

If the length of the key value is known, a pointer to a buffer large enough
to hold the key value should be passed in the key value buffer and its length
in the corresponding length argument. This function will copy the key value
into the buffer and set the actual length of the key value in the length
argument.

If <tt>keyValue</tt> is <tt>NULL</tt>, then this function will set the correct 
length of the key value in <tt>keyLen</tt>.  If <tt>keyValue</tt> is not 
<tt>NULL</tt> but <tt>keyLen</tt> is too small, this function will return an 
error and also will set the correct length of the key value in <tt>keyLen</tt>.

@param idlcParams      [Input]        An IDLC parameters object.
@param privateKey      [Input]        An IDLC private key object.
@param publicKey       [Input]        An IDLC public key object.
@param privateKeyLen   [Input/Output] The length (in bytes) of the private key 
                                      buffer.
@param privateKeyValue [Output]       The private key buffer.
@param publicKeyLen    [Input/Output] The length (in bytes) of the public key 
                                      buffer.
@param publicKeyValue  [Output]       The public key buffer.
@param sbCtx           [Input]        A global context.

@retval SB_ERR_NULL_PARAMS            The <tt>idlcParams</tt> object is 
                                      <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS             
@retval SB_ERR_NULL_INPUT             Both of the private and public key objects
                                      are <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY        The private key object is invalid.
@retval SB_ERR_BAD_PRI_KEY_BUF_LEN    The private key buffer length is invalid.
@retval SB_ERR_BAD_PUBLIC_KEY         The public key object is invalid.
@retval SB_ERR_BAD_PUB_KEY_BUF_LEN    The public key buffer length is invalid.
@retval SB_ERR_NULL_KEY_LEN           The length of the key buffer is 
                                      <tt>NULL</tt>.
@retval SB_ERR_PRI_KEY_NOT_EXPORTABLE The private key buffer cannot be 
                                      retrieved.
@retval SB_SUCCESS                    Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCKeyGet(
    sb_Params idlcParams,
    sb_PrivateKey privateKey,
    sb_PublicKey publicKey,
    size_t *privateKeyLen,
    unsigned char *privateKeyValue,
    size_t *publicKeyLen,
    unsigned char *publicKeyValue,
    sb_GlobalCtx sbCtx
);


/** Destroys an IDLC private and/or public key object.

IDLC key objects must be destroyed before the IDLC parameters object is
destroyed.

@param idlcParams [Input]        The IDLC parameters object.
@param privateKey [Input/Output] The IDLC private key object pointer.
@param publicKey  [Input/Output] The IDLC public key object pointer.
@param sbCtx      [Input]        A global context.

@retval SB_ERR_NULL_PARAMS      The <tt>idlcParams</tt> object is <tt>NULL</tt>.
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
hu_IDLCKeyDestroy(
    sb_Params idlcParams,
    sb_PrivateKey *privateKey,
    sb_PublicKey *publicKey,
    sb_GlobalCtx sbCtx
);

/** @} */

/** @addtogroup hu_idlc_dh IDLC DH
 *
 * @{
 *
 */

/** Generates a shared secret of the requested length by applying the ANSI
X9.42/X9.63 KDF with SHA-1 to the output of a DH key agreement.

@param idlcParams      [Input]  IDLC parameters object.
@param privateKey      [Input]  IDLC private key object.
@param remotePublicKey [Input]  IDLC public key object.
@param addInfoLen      [Input]  The length (in bytes) of additional information.
                                (Optional)
@param addInfo         [Input]  Additional information for use with the KDF. 
                                (Optional - set to <tt>NULL</tt> if not used.
@param secretLen       [Input]  The length (in bytes) of the shared secret.
@param sharedSecret    [Output] Shared secret value.
@param sbCtx           [Input]  A global context.

@retval SB_ERR_NULL_PARAMS        The <tt>idlcParams</tt> object is 
                                  <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS         
@retval SB_ERR_NULL_PRIVATE_KEY   The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY    The private key object is invalid.
@retval SB_ERR_NULL_PUBLIC_KEY    The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY     The public key object is invalid.
@retval SB_ERR_NULL_ADDINFO       Additional information is <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF    The shared secret buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the shared secret is invalid.
@retval SB_FAIL_ALLOC             Memory allocation failure.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDHSharedGen(
    sb_Params idlcParams,
    sb_PrivateKey privateKey,
    sb_PublicKey remotePublicKey,
    size_t addInfoLen,
    const unsigned char *addInfo,
    size_t secretLen,
    unsigned char *sharedSecret,
    sb_GlobalCtx sbCtx
);


/** Generates a shared secret that is the output of a DH key agreement.

If the length of the shared secret is known, a pointer to a buffer large
enough to hold the shared secret should be passed in
<tt>sharedSecret</tt> and its length in <tt>secretLen</tt>. This
function will copy the shared secret into <tt>sharedSecret</tt> and set
the actual length of the shared secret in <tt>secretLen</tt>.

If <tt>sharedSecret</tt> is <tt>NULL</tt>, this function will set the
correct length of the shared secret in <tt>secretLen</tt>.  If
<tt>sharedSecret</tt> is not <tt>NULL</tt> but <tt>secretLen</tt>
is too small, this function will return an error and set the correct length of
the shared secret in <tt>secretLen</tt>.

The length of the shared secret can also be determined by calling
<tt>hu_IDLCParamsGet()</tt> and retrieving the <tt>pLength</tt> argument.

@param idlcParams      [Input]        An IDLC parameters object.
@param privateKey      [Input]        An IDLC private key object.
@param remotePublicKey [Input]        An IDLC public key object.
@param secretLen       [Input/Output] The length (in bytes) of the shared 
                                      secret.
@param sharedSecret    [Output]       The shared secret buffer.
@param sbCtx           [Input]        A global context.

@retval SB_ERR_NULL_PARAMS         The <tt>idlcParams</tt> object is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS          
@retval SB_ERR_NULL_PRIVATE_KEY    The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY     The private key object is invalid.
@retval SB_ERR_NULL_PUBLIC_KEY     The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY      The public key object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF_LEN The shared secret buffer length is 
                                   <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN  The length of the shared secret is invalid.
@retval SB_FAIL_ALLOC              Memory allocation failure.
@retval SB_SUCCESS                 Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDHRawSharedGen(
    sb_Params idlcParams,
    sb_PrivateKey privateKey,
    sb_PublicKey remotePublicKey,
    size_t *secretLen,
    unsigned char *sharedSecret,
    sb_GlobalCtx sbCtx
);


/** @} */

/** @addtogroup hu_idlc_dsa IDLC DSA
 *
 * @{
 *
 */

/** Generates a signature on the given message digest using DSA.

This function assumes that the input is a message digest (of any length);
no digest operation will be performed on the input.

The IDLC parameter object must have been created with an RNG context.

If the length of s is known, a pointer to a buffer large enough to hold s
should be passed in <tt>sValue</tt> and its length in
<tt>sLength</tt>. This function will copy s into <tt>sValue</tt> and
set the actual length of s in <tt>sLength</tt>.

If <tt>sValue</tt> is <tt>NULL</tt>, this function will set the correct
length of s in <tt>sLength</tt>.  If <tt>sValue</tt> is not
<tt>NULL</tt> but <tt>sLength</tt> is too small, this function will
return an error and set the correct length of s in <tt>sLength</tt>.

Similarly for r.

s and r will always have the same length.

The length of s or r can also be determined by calling 
<tt>hu_IDLCParamsGet()</tt> and retrieving the <tt>qLength</tt> argument.

@param idlcParams    [Input]        An IDLC parameters object.
@param privateKey    [Input]        An IDLC private key object.
@param length        [Input]        The length (in bytes) of the message digest.
@param messageDigest [Input]        A message digest.
@param sLength       [Input/Output] The length (in bytes) of <tt>sValue</tt>.
@param sValue        [Output]       The 's' component from the signature 
                                    computation.
@param rLength       [Input/Output] The length (in bytes) of <tt>rValue</tt>.
@param rValue        [Output]       The 'r' component from the signature 
                                    computation.
@param sbCtx         [Input]        A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>idlcParams</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_PRIVATE_KEY  The private key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PRIVATE_KEY   The private key object is invalid.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the message digest is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The message digest is <tt>NULL</tt>.
@retval SB_ERR_NULL_S_VALUE_LEN  The signature component length is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_S_VALUE_LEN   The signature component length is invalid.
@retval SB_ERR_NULL_R_VALUE_LEN  The signature component length is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_R_VALUE_LEN   The signature component length is invalid.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDSANoHashSign(
    sb_Params idlcParams,
    sb_PrivateKey privateKey,
    size_t length,
    const unsigned char *messageDigest,
    size_t *sLength,
    unsigned char *sValue,
    size_t *rLength,
    unsigned char *rValue,
    sb_GlobalCtx sbCtx
);


/** Verifies a DSA signature on the given message digest.

This function assumes that the input is a message digest (of any length);
no digest operation will be performed on the input.

If the signature is valid for the given digest, this function may return
<tt>SB_SUCCESS</tt> and set <tt>result</tt> to a non-zero value.

If the signature is not valid for the given digest, this function may return
<tt>SB_SUCCESS</tt> but <tt>result</tt> will be set to zero.

@param idlcParams    [Input]  An IDLC parameters object.
@param publicKey     [Input]  An IDLC public key object.
@param length        [Input]  The length (in bytes) of the message digest.
@param messageDigest [Input]  The message digest.
@param sLength       [Input]  The length (in bytes) of <tt>sValue</tt>.
@param sValue        [Input]  The <tt>s</tt> component of the signature.
@param rLength       [Input]  The length (in bytes) of <tt>rValue</tt>.
@param rValue        [Input]  The <tt>r</tt> component of the signature.
@param result        [Output] Verification result. This is non-zero if the 
                              signature is valid; otherwise it is zero, meaning
			      the signature is invalid.
@param sbCtx         [Input]  A global context.

@retval SB_ERR_NULL_PARAMS       The <tt>idlcParams</tt> object is 
                                 <tt>NULL</tt>.
@retval SB_ERR_BAD_PARAMS        
@retval SB_ERR_NULL_PUBLIC_KEY   The public key object is <tt>NULL</tt>.
@retval SB_ERR_BAD_PUBLIC_KEY    The public key object is invalid.
@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the message digest is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The message digest is <tt>NULL</tt>.
@retval SB_ERR_NULL_S_VALUE      The signature component is <tt>NULL</tt>.
@retval SB_ERR_BAD_S_VALUE_LEN   The signature component length is invalid.
@retval SB_ERR_NULL_R_VALUE      The signature component is <tt>NULL</tt>.
@retval SB_ERR_BAD_R_VALUE_LEN   The signature component length is invalid.
@retval SB_ERR_NULL_OUTPUT       The verification result pointer is 
                                 <tt>NULL</tt>.
@retval SB_FAIL_ALLOC            Memory allocation failure.
@retval SB_SUCCESS Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_IDLCDSANoHashVerify(
    sb_Params idlcParams,
    sb_PublicKey publicKey,
    size_t length,
    const unsigned char *messageDigest,
    size_t sLength,
    const unsigned char *sValue,
    size_t rLength,
    const unsigned char *rValue,
    int *result,
    sb_GlobalCtx sbCtx
);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
