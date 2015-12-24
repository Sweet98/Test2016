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
 * $Source: Includes/hugse4.h $
 * $Name:  $
 */
#ifndef HUGSE4_H
#define HUGSE4_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

#define SBG4_STATE_INSTALLED   (0)
#define SBG4_STATE_ENABLED     (1)
#define SBG4_STATE_DISABLED    (2)


/** @addtogroup hugse4_initialization SB GSE-C 4 Initialization APIs

@{
*/

/** Initializes the module and performs a series of self-tests
to ensure the integrity of the module and correct operation of its
cryptographic algorithms.

Upon a successful call to this function, the state of the module
will be set to <tt>SBG4_STATE_ENABLED</tt>.

If for any reason the self-tests fail, the state of the module
will be set to <tt>SBG4_STATE_DISABLED</tt>.

This function may only be called if the state of the module is
<tt>SBG4_STATE_INSTALLED</tt>.  Calling of this function from any other
state will result in a <tt>SB_FAIL_LIBRARY_ALREADY_INIT</tt> error.

<em>Note</em>: This function must be called once to initialize the module 
before any cryptographic functions are called. It is not re-entrant.  

@param state [Input]
       Points to the state of the FIPS FSM.

@param sbCtx [Input] 
       A global context.  
       
@retval SB_FAIL_KAT
        The module failed one or more known-answer tests.

@retval SB_FAIL_INTEGRITY
        The integrity test failed.

@retval SB_FAIL_LIBRARY_ALREADY_INIT
        The module has already been initialized.

@retval SB_FAIL_LIBRARY_DISABLED
        The module is disabled and this operation is not allowed.

@retval SB_SUCCESS
        Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_InitSbg4(
    sb_GlobalCtx sbCtx
);

/** Performs a series of self-tests to ensure the integrity of the 
module and proper functioning of its cryptographic algorithms.

This function uses known-answer tests for the following algorithms:
FIPS 140-2 ANSI RNG, HMAC-SHA-1, HMAC-SHA-2, AES, and DSA.

If the self-test fails for any reason, the state of the module 
will be set to <tt>SBG4_STATE_DISABLED</tt>.

The function may only be called if the module is in 
the <tt>SBG4_STATE_ENABLED</tt> state. Calling this function
from any other state will result in an error being returned.

@param sbCtx [Input]
       A global context.

@retval SB_FAIL_LIBRARY_DISABLED
        The self-tests could not be run because the module has been disabled.

@retval SB_FAIL_LIBRARY_INTEGRITY
        The integrity of the module was compromised. The module has been
        disabled.

@retval SB_FAIL_KAT
        The module failed one or more known-answer tests. The module has
        been disabled.

@retval SB_FAIL_LIBRARY_NOT_INIT
        The module has not been initialized.

@retval SB_FAIL_LIBRARY_DISABLED
        The module is disabled and this operation is not allowed.

@retval SB_SUCCESS
        Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_SelfTestSbg4(
    sb_GlobalCtx sbCtx
);


/** Uninitializes the module.

This function can be used to reset the module to 
the <tt>SBG4_STATE_INSTALLED</tt>.

The function may not be called if the module is in
the <tt>SBG4_STATE_INSTALLED</tt> state.  Calling this 
function from this state will result in an error being 
returned.

Care must be used when calling this function.  If the 
module has entered the <tt>SBG4_STATE_DISABLED</tt>
state, it usually indicates a serious error.  Any applications using the
module should be terminated, and the module should
be reinstalled.

This function is not thread safe, and should not be called if any other 
threads are using the module.

@param sbCtx [Input]
       A global context.

@retval SB_FAIL_LIBRARY_NOT_INIT
        The module is not initialized.

@retval SB_SUCCESS
        Success.

*/
extern
int
SB_CALLCONV
hu_UninitSbg4(
    sb_GlobalCtx sbCtx
);


/** Disables the module.

This function can be used to disable the module and 
change the state of the module to <tt>SBG4_STATE_DISABLED</tt>.

The function may only be called if the module is in the
<tt>SBG4_STATE_ENABLED</tt> state.

This function is not thread safe, and should not be called if any other 
threads are using the module.

@param sbCtx [Input]
       A global context.

@retval SB_FAIL_LIBRARY_NOT_INIT
        The module is not initialized.

@retval SB_SUCCESS
        Success.

*/
extern
int
SB_CALLCONV
hu_DisableSbg4(
    sb_GlobalCtx sbCtx
);


/** Retrieves the state of the module.  

Possible values for <tt>state</tt> are: 
<ul>
<li><tt>SBG4_STATE_INSTALLED</tt> - The module is installed.
It is uninitialized, and cannot perform cryptographic operations.
<li><tt>SBG4_STATE_ENABLED</tt>   - The module has been initialized,
and is ready to perform cryptographic operations.
<li><tt>SBG4_STATE_DISABLED</tt>  - The module has been disabled,
and cannot perform cryptographic operations.
</ul>

@param state [Output] 
       The state of the module.

@param sbCtx [Input]
       A global context.

@retval SB_ERR_NULL_OUTPUT
        The <tt>state</tt> pointer is <tt>NULL</tt>.

@retval SB_SUCCESS
        Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_GetStateSbg4(
    int* state,
    sb_GlobalCtx sbCtx
);



/** @addtogroup hugse4_register_rng Random Number Generators
 * @{
 */

/** Enables support to create an RNG context that uses the FIPS 140-2
compliant ANSI RNG from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_RngCreate()
  <li>hu_RngDestroy()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4FIPS140ANSIRng(
    sb_GlobalCtx sbCtx
);

/** @} */


/** @addtogroup hugse4_register_idlc IDLC
 * @{
 */

/** Enables support for IDLC functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCDHRawSharedGen()
  <li>hu_IDLCDHSharedGen()
  <li>hu_IDLCDSANoHashSign()
  <li>hu_IDLCDSANoHashVerify()
  <li>hu_IDLCKeyDestroy()
  <li>hu_IDLCKeyGen()
  <li>hu_IDLCKeyGet()
  <li>hu_IDLCKeySet()
  <li>hu_IDLCParamsDestroy()
  <li>hu_IDLCParamsGet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLC(
    sb_GlobalCtx sbCtx
);

/** @} */


/** @addtogroup hugse4_register_idlc_optimization IDLC Optimization
 * @{
 */

/** This is the generic prototype for the IDLC registration functions 
 * listed below. The name of each function consists of two parts: a constant 
 * part (<tt>hu_RegisterSbg4IDLCParamsIDLC</tt>), and the optimization level 
 * (<tt>0</tt>, <tt>1</tt>, or <tt>2</tt>).
 *
 * For example, the function <tt>hu_RegisterSbg4IDLCParamsIDLC_0()</tt> breaks 
 * down into:
 * <ul>
 * <li>constant: <tt>hu_RegisterSbg4IDLCParamsIDLC</tt> 
 * <li>optimization level: <tt>0</tt>
 * </ul>
 * 
 * Each of these functions enables support for the creation of a parameter 
 * object for an IDLC group with a given optimization level.  
 *
 * With this information you should easily be able to pick the registration
 * function you want from the list below and determine the identifier 
 * associated with it.
 *
 * <tt>hu_RegisterSbg4IDLCParamsIDLC_0()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsIDLC_1()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsIDLC_2()</tt>
 *
 * Each of the above functions enable the following functions to be called: 
 *
 * <ul>
 * <li>hu_IDLCDHParamsSet()
 * <li>hu_IDLCDSAParamsSet()
 * <li>hu_IDLCParamsGen()
 * <li>hu_IDLCParamsSet()
 * </ul>
 * @param sbCtx [Input] A global context.
 *
 */

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParams_IDLC_Optimization(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_idlc_ansi_optimization IDLC ANSI Optimization
 * @{
 */

/** This is the generic prototype for the IDLC registration functions 
 * listed below which adhere to the ANSI X9.42 specifications. The name of 
 * each function consists of two parts: a constant part 
 * (<tt>hu_RegisterSbg4IDLCParamsANSI</tt>) and the optimization level 
 * (<tt>0</tt>, <tt>1</tt>, or <tt>2</tt>).
 *
 * For example, the function <tt>hu_RegisterSbg4IDLCParamsANSI_0()</tt> breaks 
 * down into:
 * <ul>
 * <li>constant: <tt>hu_RegisterSbg4IDLCParamsANSI</tt> 
 * <li>optimization level: <tt>0</tt>
 * </ul>
 * 
 * Each of these functions enables support for the creation of a parameter 
 * object for an IDLC group with a given optimization level.  
 *
 * With this information you should easily be able to pick the registration
 * function you want from the list below and determine the identifier 
 * associated with it.
 *
 * <tt>hu_RegisterSbg4IDLCParamsANSI_0()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsANSI_1()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsANSI_2()</tt>
 *
 * Each of the above functions enable the following function to be called: 
 *
 * <ul>
 * <li>hu_IDLCANSIParamsGen()
 * </ul>
 * @param sbCtx [Input] A global context.
 *
 */

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParams_ANSI_Optimization(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_idlc_fips_optimization IDLC FIPS Optimization
 * @{
 */

/** This is the generic prototype for the IDLC registration functions 
 * listed below which adhere to the FIPS 186 specification. The name of 
 * each function consists of two parts: a constant part 
 * (<tt>hu_RegisterSbg4IDLCParamsFIPS</tt>) and the optimization level 
 * (<tt>0</tt>, <tt>1</tt>, or <tt>2</tt>).
 *
 * For example, the function <tt>hu_RegisterSbg4IDLCParamsFIPS_0()</tt> breaks 
 * down into:
 * <ul>
 * <li>constant: <tt>hu_RegisterSbg4IDLCParamsFIPS</tt> 
 * <li>optimization level: <tt>0</tt>
 * </ul>
 * 
 * Each of these functions enables support for the creation of a parameter 
 * object for an IDLC group with the given optimization level.  
 *
 * With this information you should easily be able to pick the registration
 * function you want from the list below and determine the identifier 
 * associated with it.
 *
 * <tt>hu_RegisterSbg4IDLCParamsFIPS_0()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsFIPS_1()</tt>
 * <tt>hu_RegisterSbg4IDLCParamsFIPS_2()</tt>
 *
 * Each of the above functions enable the following function to be called: 
 *
 * <ul>
 * <li>hu_IDLCFIPSParamsGen()
 * </ul>
 * @param sbCtx [Input] A global context.
 *
 */

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParams_FIPS_Optimization(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_idlc_individual IDLC
 * @{
 */


/** Enables support to generate a parameters object for IDLC with
    level 0 optimization from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCDHParamsSet()
  <li>hu_IDLCDSAParamsSet()
  <li>hu_IDLCParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsIDLC_0(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC with
    level 1 optimization from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCDHParamsSet()
  <li>hu_IDLCDSAParamsSet()
  <li>hu_IDLCParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsIDLC_1(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC with
    level 2 optimization from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCDHParamsSet()
  <li>hu_IDLCDSAParamsSet()
  <li>hu_IDLCParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsIDLC_2(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    ANSI X9.42 specifications with level 0 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCANSIParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsANSI_0(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    ANSI X9.42 specifications with level 1 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCANSIParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsANSI_1(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    ANSI X9.42 specifications with level 2 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCANSIParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsANSI_2(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    FIPS 186 specifications with level 0 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCFIPSParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsFIPS_0(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    FIPS 186 specifications with level 1 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCFIPSParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsFIPS_1(
    sb_GlobalCtx sbCtx
);


/** Enables support to generate a parameters object for IDLC according to
    FIPS 186 specifications with level 2 optimization from the GSE4 software
    provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_IDLCFIPSParamsGen()
  <li>hu_IDLCParamsSet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4IDLCParamsFIPS_2(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_block Block Ciphers
 * @{
 */

/** Enables support for AES functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_AESBegin()
  <li>hu_AESBeginV2()
  <li>hu_AESCCMAuthDecrypt()
  <li>hu_AESCCMAuthEncrypt()
  <li>hu_AESCCMStarAuthDecrypt()
  <li>hu_AESCCMStarAuthEncrypt()
  <li>hu_AESDecrypt()
  <li>hu_AESDecryptKeySet()
  <li>hu_AESDecryptMsg()
  <li>hu_AESEncrypt()
  <li>hu_AESEncryptKeyGen()
  <li>hu_AESEncryptKeySet()
  <li>hu_AESEncryptMsg()
  <li>hu_AESEnd()
  <li>hu_AESKeyDestroy()
  <li>hu_AESKeyGen()
  <li>hu_AESKeyGet()
  <li>hu_AESKeySet()
  <li>hu_AESKeyUnwrap()
  <li>hu_AESKeyWrap()
  <li>hu_AESParamsCreate()
  <li>hu_AESParamsDestroy()
  <li>hu_AESParamsGet()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4AES(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_hash Hash
 * @{
 */

/** Enables support for SHA-1 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_SHA1Begin()
  <li>hu_SHA1CtxDuplicate()
  <li>hu_SHA1CtxReset()
  <li>hu_SHA1DigestGet()
  <li>hu_SHA1End()
  <li>hu_SHA1Hash()
  <li>hu_SHA1Msg()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4SHA1(
    sb_GlobalCtx sbCtx
);


/** Enables support for SHA-224 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_SHA224Begin()
  <li>hu_SHA224CtxDuplicate()
  <li>hu_SHA224CtxReset()
  <li>hu_SHA224DigestGet()
  <li>hu_SHA224End()
  <li>hu_SHA224Hash()
  <li>hu_SHA224Msg()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4SHA224(
    sb_GlobalCtx sbCtx
);


/** Enables support for SHA-256 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_SHA256Begin()
  <li>hu_SHA256CtxDuplicate()
  <li>hu_SHA256CtxReset()
  <li>hu_SHA256DigestGet()
  <li>hu_SHA256End()
  <li>hu_SHA256Hash()
  <li>hu_SHA256Msg()
</ul>
$FunctionListEnd

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4SHA256(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_hmac HMAC
 * @{
 */

/** Enables support for HMAC-SHA-1 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_HMACSHA1Begin()
  <li>hu_HMACSHA1End()
  <li>hu_HMACSHA1Hash()
  <li>hu_HMACSHA1Msg()
  <li>hu_MACBegin()
  <li>hu_MACEnd()
  <li>hu_MACKeyDestroy()
  <li>hu_MACKeyGen()
  <li>hu_MACKeyGet()
  <li>hu_MACKeySet()
  <li>hu_MACMsg()
  <li>hu_MACUpdate()
</ul>
$FunctionListEnd

The MAC identifier for this algorithm is <tt>HU_DIGEST_SHA1</tt>.

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4HMACSHA1(
    sb_GlobalCtx sbCtx
);


/** Enables support for HMAC-SHA-224 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_HMACSHA224Begin()
  <li>hu_HMACSHA224End()
  <li>hu_HMACSHA224Hash()
  <li>hu_HMACSHA224Msg()
  <li>hu_MACBegin()
  <li>hu_MACEnd()
  <li>hu_MACKeyDestroy()
  <li>hu_MACKeyGen()
  <li>hu_MACKeyGet()
  <li>hu_MACKeySet()
  <li>hu_MACMsg()
  <li>hu_MACUpdate()
</ul>
$FunctionListEnd

The MAC identifier for this algorithm is <tt>HU_DIGEST_SHA224</tt>.

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4HMACSHA224(
    sb_GlobalCtx sbCtx
);


/** Enables support for HMAC-SHA-256 functions from the GSE4 software provider.

This function enables the following function(s) to be called:
$FunctionListBegin
<ul>
  <li>hu_HMACSHA256Begin()
  <li>hu_HMACSHA256End()
  <li>hu_HMACSHA256Hash()
  <li>hu_HMACSHA256Msg()
  <li>hu_MACBegin()
  <li>hu_MACEnd()
  <li>hu_MACKeyDestroy()
  <li>hu_MACKeyGen()
  <li>hu_MACKeyGet()
  <li>hu_MACKeySet()
  <li>hu_MACMsg()
  <li>hu_MACUpdate()
</ul>
$FunctionListEnd

The MAC identifier for this algorithm is <tt>HU_DIGEST_SHA256</tt>.

@param sbCtx [Input]
       A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4HMACSHA256(
    sb_GlobalCtx sbCtx
);


/** @} */


/** @addtogroup hugse4_register_all All
 * @{
 */

/** Enables support for all functions from the GSE4 software provider.

This function calls the following functions:
$FunctionListBegin
<ul>
  <li>hu_RegisterSbg4AES()
  <li>hu_RegisterSbg4FIPS140ANSIRng()
  <li>hu_RegisterSbg4HMACSHA1()
  <li>hu_RegisterSbg4HMACSHA224()
  <li>hu_RegisterSbg4HMACSHA256()
  <li>hu_RegisterSbg4IDLC()
  <li>hu_RegisterSbg4IDLCParamsANSI_2()
  <li>hu_RegisterSbg4IDLCParamsFIPS_2()
  <li>hu_RegisterSbg4IDLCParamsIDLC_2()
  <li>hu_RegisterSbg4SHA1()
  <li>hu_RegisterSbg4SHA224()
  <li>hu_RegisterSbg4SHA256()
</ul>
$FunctionListEnd

<em>Note</em>: Any registration functions that require an optimization level 
(e.g. <tt>hu_RegisterSbg4IDLCParams_IDLC_</tt><em>optlevel</em><tt>()</tt>) will 
have the optimization level set to 2 by default.

@param sbCtx [Input] A global context.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RegisterSbg4(
    sb_GlobalCtx sbCtx
);

/** @} */


#ifdef __cplusplus
}
#endif

#endif /* !HUGSE4_H */
