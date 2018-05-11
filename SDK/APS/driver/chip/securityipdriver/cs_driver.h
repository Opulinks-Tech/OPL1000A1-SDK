/** @file cs_driver.h
 *
 * @brief Configuration Settings for the EIP-130 Global Control Driver.
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

#ifndef INCLUDE_GUARD_CS_DRIVER_H
#define INCLUDE_GUARD_CS_DRIVER_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_systemtestconfig.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** System behaviour specifics:\n */
/** Big-endian CPU use */
#ifdef ARCH_POWERPC
#define DRIVER_SWAPENDIAN
#endif

/** General driver/device name */
#define DRIVER_NAME             "vaultip-1xx"

/** Firmware file name */
#define DRIVER_FIRMWARE_FILE    "firmware_eip130ram.sbi"


/** DMA operation finished checking based on TokenID use */
#define DRIVER_DMAREADYCHECK

/** Hardware and firmware specifics related to the used configuration:\n */
#define DRIVER_ENABLE_SHA512        /** SHA512 use */
//#define DRIVER_ENABLE_DES           /** DES use */
//#define DRIVER_ENABLE_3DES          /** Triple DES use */
//#define DRIVER_ENABLE_CHACHA20          /** ChaCha20 use */
//#define DRIVER_ENABLE_POLY1305          /** Poly1305 use */
//#define DRIVER_ENABLE_AES_F8        /** AES_f8 use */
#define DRIVER_ENABLE_AES_CCM       /** AES-CCM use */
#define DRIVER_ENABLE_AES_GCM       /** AES_GCM use */
//#define DRIVER_ENABLE_AES_XTS       /** XTS-AES use */
//#define DRIVER_ENABLE_AES_KEYWRAP   /** AES Keywrap use */
//#define DRIVER_ENABLE_ENCRYPTED_VECTOR  /** Encrypted vector for PKI use */

/** System Configuration */
// Driver Conf  Poll/Int  BounceBuff
// C0             Poll       Off
#ifdef SYSTEMTEST_CONFIGURATION_C0
//#define DRIVER_INTERRUPT
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C0

// Driver Conf  Poll/Int  BounceBuff
// C1             Int        Off
#ifdef SYSTEMTEST_CONFIGURATION_C1
#define DRIVER_INTERRUPT
//#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C1

// Driver Conf  Poll/Int  BounceBuff
// C2             Poll       Off
#ifdef SYSTEMTEST_CONFIGURATION_C2
//#define DRIVER_INTERRUPT
//#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C2

// Driver Conf  Poll/Int  BounceBuff
// C3             Poll       On
#ifdef SYSTEMTEST_CONFIGURATION_C3
//#define DRIVER_INTERRUPT
#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C3

// Driver Conf  Poll/Int  BounceBuff
// C4             Int        On
#ifdef SYSTEMTEST_CONFIGURATION_C4
#define DRIVER_INTERRUPT
#define DRIVER_BOUNCEBUFFERS
#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C4

// Driver Conf  Poll/Int  BounceBuff
// C5             Int        On
#ifdef SYSTEMTEST_CONFIGURATION_C5
#define DRIVER_INTERRUPT
#define DRIVER_BOUNCEBUFFERS
//#define DRIVER_PERFORMANCE
#endif // SYSTEMTEST_CONFIGURATION_C5


#include "cs_driver_ext.h"


#endif // INCLUDE_GUARD_CS_DRIVER_H


/* end of file cs_driver.h */
