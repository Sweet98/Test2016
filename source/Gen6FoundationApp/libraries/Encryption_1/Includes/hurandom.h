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
 * $Source: Includes/hurandom.h $
 * $Name:  $
 */
#ifndef HURANDOM_H
#define HURANDOM_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_rng_cb RNG callbacks
 *
 * @{
 */

/** Prototype of user-provided callback function that generates random seeds
of the specified length.

This function should copy <tt>bufsize</tt> bytes of seed data into <tt>buf</tt>.
This callback should be registered during RNG context creation.

@param rsourceParam [Input]  A user-defined data pointer.
@param bufsize      [Input]  The length (in bytes) of desired seed data.
@param buf          [Output] The seed data buffer. This buffer will be at least
                             <tt>bufsize</tt> bytes in length.
@param sbCtx        [Input]  A global context.

@retval int <tt>SB_SUCCESS</tt>, to indicate successful completion.
*/

typedef
int
SB_CALLBACK_CALLCONV
hu_ReseedCallbackFunc(
    void *rsourceParam,
    size_t bufsize,
    unsigned char *buf,
    sb_GlobalCtx sbCtx
);


/** Prototype of user-provided callback function that performs initialization
for a user-defined RNG context.

This function is called by <tt>hu_CustomRngCreate()</tt> to allow users to
create and initialize their own context. On completion, set <tt>*rngctx</tt>
to the context structure you allocate.

This callback should be registered during custom RNG context creation.

@param initinput [Input]  Arbitrary parameter to user-defined initialization
                          function. This is just the data that was passed in
                          the <tt>initInput</tt> argument in
                          <tt>hu_CustomRngCreate()</tt>.
@param rngctx    [Output] User defined RNG context parameter.
@param sbCtx     [Input]  SB context or memory callback data.
                          (This may not be used by the user implementation.)

@retval int <tt>SB_SUCCESS</tt>, to indicate successful completion.
*/

typedef
int
SB_CALLBACK_CALLCONV
hu_RngInitFunc(
    void  *initinput,
    void **rngctx,
    void *sbCtx
);


/** Prototype of user-provided callback function that generates random data.

This function is called by <tt>hu_RngGetBytes()</tt> and
<tt>hu_RngReseedGetBytes()</tt>. It will be passed the <tt>rngctx</tt> object
that was created by the <tt>hu_RngInitFunc()</tt> callback. This callback may
be passed seeding data as well. On completion, you should write
<tt>bufsize</tt> bytes of random data into <tt>buf</tt>.

This callback should be registered during custom RNG context creation.

@param rngctx    [Input]  User defined RNG context.
@param seedlen   [Input]  Length of <tt>seed</tt> argument, in bytes.
@param seed      [Input]  Additional seed data. May be NULL.
@param bufsize   [Input]  Amount of random data requested, in bytes.
@param buf       [Output] Buffer to hold requested random data.
@param sbCtx     [Input]  SB context or memory callback data.
                          (This may not be used by the user implementation.)

@retval int <tt>SB_SUCCESS</tt>, to indicate successful completion.
*/
typedef
int
SB_CALLBACK_CALLCONV
hu_RngGetBytesFunc(
    void                *rngctx,
    size_t               seedlen,
    const unsigned char *seed,
    size_t               bufsize,
    unsigned char       *buf,
    void *sbCtx
);


/** Prototype of user-provided callback function that reseeds a user-defined
RNG context.

This function is called by <tt>hu_RngReseed()</tt>. It will be passed seeding
material that the user-defined RNG can use to reseed itself.

This callback should be registered during custom RNG context creation.

@param rngctx    [Input]  User defined RNG context.
@param seedlen   [Input]  Length of <tt>seed</tt> argument, in bytes.
@param seed      [Input]  Additional seed data.
@param sbCtx     [Input]  SB context or memory callback data.
                          (This may not be used by the user implementation.)

@retval int <tt>SB_SUCCESS</tt>, to indicate successful completion.
*/
typedef
int
SB_CALLBACK_CALLCONV
hu_RngReseedFunc(
    void                *rngctx,
    size_t               seedlen,
    const unsigned char *seed,
    void *sbCtx
);


/** Prototype of user-provided callback function that cleans up a
user-defined RNG context.

This function is called by <tt>hu_CustomRngDestroy()</tt>. It will be passed
the <tt>rngctx</tt> object that was created by the <tt>hu_RngInitFunc()</tt>
callback. You should perform any cleanup and deallocation of the object.

This callback should be registered during custom RNG context creation.

@param rngctx    [Input/Output]  User defined RNG context.
@param sbCtx     [Input]  SB context or memory callback data.
                          (This may not be used by the user implementation.)

@retval int <tt>SB_SUCCESS</tt>, to indicate successful completion.
*/
typedef
int
SB_CALLBACK_CALLCONV
hu_RngEndFunc(
    void **rngctx,
    void *sbCtx
);


/** @} */


/** @addtogroup hu_rng_api RNG APIs
 *
 * @{
 */

/** Creates an RNG context object with optional initial seed and reseed
callback.

If a reseed callback function is provided, it will be called periodically to
obtain additional seeding data.

@param seedLen      [Input]  The length (in bytes) of initial seed. (Optional)
@param seed         [Input]  The initial seed value. Set to <tt>NULL</tt> if not
                             used.
@param seedFunc     [Input]  A user-provided callback function. (Optional - set
                             to <tt>NULL</tt> if not used.)
@param rsourceParam [Input]  A user-defined data that will be passed to 
                             <tt>seedFunc</tt>. (Optional - set to 
                             <tt>NULL</tt> if not used.)
@param yieldCtx     [Input]  A yield context. (Optional - set to <tt>NULL</tt> 
                             if not used.
@param rngCtx       [Output] An RNG context object pointer.
@param sbCtx        [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The RNG context object pointer is <tt>NULL</tt>.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RngCreate(
    size_t seedLen,
    const unsigned char *seed,
    hu_ReseedCallbackFunc* seedFunc,
    void *rsourceParam,
    sb_YieldCtx yieldCtx,
    sb_RNGCtx *rngCtx,
    sb_GlobalCtx sbCtx
);


/** Destroys an RNG context object.

@param rngCtx [Input/Output] An RNG context object pointer.
@param sbCtx  [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR The RNG context object pointer is <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT     The RNG context object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT      The RNG context is invalid.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RngDestroy(
    sb_RNGCtx *rngCtx,
    sb_GlobalCtx sbCtx
);


/** @} */ 

/** @addtogroup hu_rng_custom_api Custom RNG APIs
 *
 * @{
 */

/** Initializes a random number generator (RNG) context for the user defined
RNG using user-provided callback functions and seed value.

@param initInput   [Input]  Input to the supplied initialization function.
                            This value is passed to the user-defined init
                            function.
@param init        [Input]  RNG initialization callback function pointer. 
                            (Optional - set to <tt>NULL</tt> if not used.)
@param end         [Input]  RNG end callback function pointer. (Optional - set 
                            to <tt>NULL</tt> if not used.)
@param getbytes    [Input]  RNG get bytes callback function pointer. This 
                            function pointer must be supplied.
@param reseed      [Input]  RNG reseed callback function pointer. (Optional - 
                            set to <tt>NULL</tt> if not used.)
@param yieldCtx    [Input]  Yield context. (Optional - set to <tt>NULL</tt> if 
                            not used.)
@param rngCtx      [Output] RNG context pointer.
@param sbCtx       [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT_PTR RNG context pointer is NULL.
@retval SB_FAIL_ALLOC           Memory allocation failure.
@retval SB_X                    Failure return from the callback function.
@retval SB_SUCCESS              Success.

*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_CustomRngCreate(
    void *initInput,
    hu_RngInitFunc* init,
    hu_RngEndFunc* end,
    hu_RngGetBytesFunc* getbytes,
    hu_RngReseedFunc* reseed,
    sb_YieldCtx yieldCtx,
    sb_RNGCtx *rngCtx,
    sb_GlobalCtx sbCtx
);


/** Destroys RNG context for the user defined RNG.

This function will call the registered <tt>hu_RngEndFunc()</tt> function
(if any) before the RNG context is destroyed.

@param rngCtx [Input/Output] RNG context pointer.
@param sbCtx  [Input]        A global context.

@retval SB_ERR_NULL_CONTEXT_PTR RNG context pointer is NULL.
@retval SB_ERR_NULL_CONTEXT     RNG context is NULL.
@retval SB_ERR_BAD_CONTEXT      RNG context is invalid.
@retval SB_X                    Failure return from the callback function.
@retval SB_SUCCESS              Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_CustomRngDestroy(
    sb_RNGCtx *rngCtx,
    sb_GlobalCtx sbCtx
);

/** @} */

/** @addtogroup hu_rng_api RNG APIs
 *
 * @{
 */

/** Updates the RNG context object with the given seed data and generates
random data of the specified length.

Reseeding the RNG context with whatever new entropy is collected is excellent
practice.

@param rngCtx  [Input]  An RNG context object.
@param seedLen [Input]  The length (in bytes) of seed.
@param seed    [Input]  The seed value.
@param bufSize [Input]  The length (in bytes) of random data buffer.
@param buf     [Output] Random data.
@param sbCtx   [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT       The RNG context object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT        The RNG context object is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF    The output buffer is <tt>NULL</tt>.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the output buffer is invalid.
@retval SB_FAIL_RANDOM_GEN        A FIPS 140-1/2 condition failed.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RngReseedGetBytes(
    sb_RNGCtx rngCtx,
    size_t seedLen,
    const unsigned char *seed,
    size_t bufSize,
    unsigned char *buf,
    sb_GlobalCtx sbCtx
);


/** Generates random data of the specified length.

@param rngCtx  [Input]  An RNG context object.
@param bufSize [Input]  The length (in bytes) of random data buffer.
@param buf     [Output] Random data.
@param sbCtx   [Input]  A global context.

@retval SB_ERR_NULL_CONTEXT       The RNG context object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT        The RNG context object is invalid.
@retval SB_ERR_BAD_OUTPUT_BUF_LEN The length of the output buffer is invalid.
@retval SB_ERR_NULL_OUTPUT_BUF    The output buffer is <tt>NULL</tt>.
@retval SB_FAIL_RANDOM_GEN        FIPS 140-1/2 condition failed.
@retval SB_SUCCESS                Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RngGetBytes(
    sb_RNGCtx rngCtx,
    size_t bufSize,
    unsigned char *buf,
    sb_GlobalCtx sbCtx
);


/** Updates the RNG context object with the given seed data.

Reseeding the RNG context with whatever new entropy is collected is excellent
practice.

@param rngCtx  [Input] An RNG context object.
@param seedLen [Input] The length (in bytes) of seed.
@param seed    [Input] The seed value.
@param sbCtx   [Input] A global context.

@retval SB_ERR_BAD_INPUT_BUF_LEN The length of the input buffer is invalid.
@retval SB_ERR_NULL_INPUT_BUF    The input buffer is <tt>NULL</tt>.
@retval SB_ERR_NULL_CONTEXT      The RNG context object is <tt>NULL</tt>.
@retval SB_ERR_BAD_CONTEXT       The RNG context object is invalid.
@retval SB_SUCCESS               Success.
*/

extern
SB_EXPORT
int
SB_CALLCONV
hu_RngReseed(
    sb_RNGCtx rngCtx,
    size_t seedLen,
    const unsigned char *seed,
    sb_GlobalCtx sbCtx
);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
