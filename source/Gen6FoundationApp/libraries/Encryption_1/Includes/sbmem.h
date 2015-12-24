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
 * $Source: Includes/sbmem.h $
 * $Name:  $
 */
#ifndef _SBMEM_H
#define _SBMEM_H

/** @file */

#ifdef __cplusplus
extern "C" {
#endif

#include "sbdef.h"

/** @addtogroup hu_callback_api Callback APIs
 *
 * @{
 */

typedef unsigned long sb_memHandle;

#define SB_NULL_HANDLE 0


/** Callback interface for allocating memory.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.
 
@param size [Input]
       Number of bytes to allocate.
@param cbData [Input]
       Global context.

@retval Pointer to allocated memory, or <code>NULL</code> if insufficient
        memory is available.
*/

extern
SB_EXPORT
void *
SB_CALLCONV
sb_malloc(
    size_t size,
    void *cbData
);


/** Callback interface for freeing memory.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.

@param ptr [Input]
       Pointer to allocated memory.
@param cbData [Input]
       Global context.
*/

extern
SB_EXPORT
void
SB_CALLCONV
sb_free(
    void *ptr,
    void *cbData
);


/** Callback interface for copying memory.

The <code>dst</code> and <code>src</code> buffers may overlap.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.
 
@param dst [Output]
       Pointer to destination memory location.
@param src [Input]
       Pointer to source memory location.
@param len [Input]
       Number of bytes to copy.
@param cbData [Input]
       Global context.
*/

extern
SB_EXPORT
void
SB_CALLCONV
sb_memcpy(
    void *dst,
    const void *src,
    size_t len,
    void *cbData
);


/** Callback interface for comparing memory.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.
 
@param block1 [Input]
       Pointer to first memory location.
@param block2 [Input]
       Pointer to second memory pointer.
@param len [Input]
       Number of bytes to compare.
@param cbData [Input]
       Global context.

@retval Integer less than, equal to, or greater than zero if the first
        <code>len</code> bytes of <code>block1</code> is less than, equal
        to, or greater than the first <code>len</code> bytes of
        <code>block2</code>, respectively.
*/

extern
SB_EXPORT
int
SB_CALLCONV
sb_memcmp(
    const void *block1,
    const void *block2,
    size_t len,
    void *cbData
);


/** Callback interface for filling memory with a value.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.

@param buf [Output]
       Pointer to memory location.
@param value [Input]
       Character to fill with.
@param len [Input]
       Number of bytes to fill.
@param cbData [Input]
       Global context.
*/

extern
SB_EXPORT
void
SB_CALLCONV
sb_memset(
    void *buf,
    int value,
    size_t len,
    void *cbData
);


/** Callback interface for getting the time in seconds since the Epoch.

The <code>cbData</code> argument must be a global context so that this 
function can call the user-provided callback that was registered during
global context creation.
 
@param cbData [Input]
       Global context.

@retval An unsigned 32-bit integer.
*/

extern
SB_EXPORT
sb_Uint32
SB_CALLCONV
sb_time(
    void *cbData
);


/* Deprecated. 
*/

extern
SB_EXPORT
void *
SB_CALLCONV
sb_memHandleLock (
    sb_memHandle memHandle,
    void * cbData
);


/* Deprecated. 
*/

extern
SB_EXPORT
sb_memHandle
SB_CALLCONV
sb_memUnlock (
    void * memPtr,
    void * cbData
);


#ifdef __cplusplus
}
#endif

#endif
