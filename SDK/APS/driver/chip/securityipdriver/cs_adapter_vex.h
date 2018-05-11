/** @file cs_adapter_vex.h
 *
 * @brief Configuration Settings for the VaultIP Exchange..
 */

/*****************************************************************************
* Copyright (c) 2014-2017 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_CS_ADAPTER_VEX_H
#define INCLUDE_GUARD_CS_ADAPTER_VEX_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_driver.h"
#include "cs_adapter.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** Strict argument checking use */
#ifdef ADAPTER_STRICT_ARGS
//#define VEX_STRICT_ARGS
#endif

/** Token tracing use */
//#define VEX_TRACE_TOKENS

/** Defines the VaultIP device name use
 *  Please, look at the cs_hwpal_ext.h for the usable device names */
//#define VEX_DEVICE_NAME "VaultIP-1xx"
#define VEX_DEVICE_NAME "VAULTIP"


/** SHA512 related functionality use */
#ifdef DRIVER_ENABLE_SHA512
#define VEX_ENABLE_SYM_ALGO_SHA512
#endif

/** DES related functionality use */
#ifdef DRIVER_ENABLE_DES
#define VEX_ENABLE_SYM_ALGO_DES
#endif

/** Triple-DES (3DES) related functionality use */
#ifdef DRIVER_ENABLE_3DES
#define VEX_ENABLE_SYM_ALGO_3DES
#endif

/** ChaCha20 related functionality use */
#ifdef DRIVER_ENABLE_CHACHA20
#define VEX_ENABLE_SYM_ALGO_CHACHA20
#endif

/** Poly1305 related functionality use */
#ifdef DRIVER_ENABLE_POLY1305
#define VEX_ENABLE_SYM_ALGO_POLY1305
#endif

/** AES-GCM related functionality use */
#ifdef DRIVER_ENABLE_AES_GCM
#define VEX_ENABLE_SYM_ALGO_AES_CGM
#endif

/** Encrypted Vector for PKI related functionality use */
#ifdef DRIVER_ENABLE_ENCRYPTED_VECTOR
#define VEX_ENABLE_ENCRYPTED_VECTOR
#endif

/** Firmware load related functionality use */
#ifdef DRIVER_ENABLE_FIRMWARE_LOAD

/** Defines that the firmware load must be enabled */
#define VEX_ENABLE_FIRMWARE_LOAD

/** Defines the firmware filename */
#define VEX_FIRMWARE_FILE DRIVER_FIRMWARE_FILE

/** Defines the firmware sleep is enabled */
#ifdef DRIVER_ENABLE_FIRMWARE_SLEEP
#define VEX_ENABLE_FIRMWARE_SLEEP
#endif

/** Defines the firmware hibernation is enabled */
#ifdef DRIVER_ENABLE_FIRMWARE_HIBERATION
#define VEX_ENABLE_FIRMWARE_HIBERATION
#endif

#endif

/** Defines the mailbox to use */
#define VEX_MAILBOX_NR 1

/** Defines if the mailbox must be link only once, otherwise the mailbox will
 *  be linked for every submitted token and released (unlinked) when the
 *  result token is read. */
#define VEX_MAILBOX_LINK_ONLY_ONCE

/** DMA operation finished checking based on TokenID use */
#ifdef DRIVER_DMAREADYCHECK
//#define VEX_CHECK_DMA_WITH_TOKEN_ID
#endif


#endif /* INCLUDE_GUARD_CS_ADAPTER_VEX_H */

/* end of file cs_adapter_vex.h */
