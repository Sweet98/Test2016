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
 * $Source: Includes/hukdf.h $
 * $Name:  $
 */
#ifndef HUKDF_H
#define HUKDF_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_kdf_api KDF APIs
 *
 * @{
 */

/** ID for IEEE 1363-2000 KDF1 based on SHA-1. */
#define HU_KDF_IEEE_KDF1_SHA1     1

/** ID for ANSI X9.42/X9.63 KDF based on SHA-1. */
#define HU_KDF_ANSI_SHA1          2

/** ID for ANSI X9.42/X9.63 KDF based on SHA-224. */
#define HU_KDF_ANSI_SHA224        3

/** ID for ANSI X9.42/X9.63 KDF based on SHA-256. */
#define HU_KDF_ANSI_SHA256        4

/** ID for ANSI X9.42/X9.63 KDF based on SHA-384. */
#define HU_KDF_ANSI_SHA384        5

/** ID for ANSI X9.42/X9.63 KDF based on SHA-512. */
#define HU_KDF_ANSI_SHA512        6

/** IEEE 1363a-2004 KDF2 is based on the constructions in 
ANSI X9.42-2001 and ANSI X9.63. */
#define HU_KDF_IEEE_KDF2_SHA1     HU_KDF_ANSI_SHA1
#define HU_KDF_IEEE_KDF2_SHA256   HU_KDF_ANSI_SHA256
#define HU_KDF_IEEE_KDF2_SHA384   HU_KDF_ANSI_SHA384
#define HU_KDF_IEEE_KDF2_SHA512   HU_KDF_ANSI_SHA512

/** Derives a value of the requested length based on shared secret
information, suitable for use as a key value.

Additional shared information may also be given.

For the IEEE KDF1 algorithm, the requested length must be the underlying
digest algorithm's output length.

@param algid        [Input]  A KDF algorithm. The acceptable values are one of 
                             the <tt>HU_KDF_*</tt> macros.
@param secretLen    [Input]  The length (in bytes) of the shared secret data.
@param sharedSecret [Input]  The shared secret data.
@param addInfoLen   [Input]  The length (in bytes) of the additional 
                             information. (Optional)
@param addInfo      [Input]  Additional information. (Optional - set to 
                             <tt>NULL</tt> if not used.)
@param keyLen       [Input]  The length (in bytes) of the key buffer.
@param keyValue     [Output] The key buffer.
@param sbCtx        [Input]  A global context.

@retval SB_ERR_KDF_BAD_ALGORITHM  The KDF algorithm identifier is invalid.
@retval SB_ERR_NULL_INPUT_BUF     The shared secret value is <tt>NULL</tt>.
@retval SB_ERR_BAD_INPUT_BUF_LEN  The length of the shared secret length is 
                                  invalid.
@retval SB_ERR_NULL_ADDINFO       The additional information value is 
                                  <tt>NULL</tt>.
@retval SB_ERR_NULL_OUTPUT_BUF    The key buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the key buffer length is 
                                  invalid.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_KDFDerive(
    int algid,
    size_t secretLen,
    const unsigned char *sharedSecret,
    size_t addInfoLen,
    const unsigned char *addInfo,
    size_t keyLen,
    unsigned char *keyValue,
    sb_GlobalCtx sbCtx
);


/** @} */

#ifdef __cplusplus
}
#endif

#endif
