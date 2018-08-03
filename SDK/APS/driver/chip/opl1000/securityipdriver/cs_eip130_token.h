/** @file cs_eip130_token.h
 *
 * @brief Configuration Settings for the EIP130 Token helper functions.
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

#ifndef INCLUDE_GUARD_CS_EIP130_TOKEN_H
#define INCLUDE_GUARD_CS_EIP130_TOKEN_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_driver.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** Strict argument checking use */
#ifndef DRIVER_PERFORMANCE
//#define EIP130TOKEN_STRICT_ARGS
#endif

// Use the options below to selectively enable specific functions
#ifdef DRIVER_ENABLE_AES_CCM
#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM
#endif
#ifdef DRIVER_ENABLE_AES_GCM
#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM
#endif
#ifdef DRIVER_ENABLE_AES_F8
#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_F8
#endif
#ifdef DRIVER_ENABLE_CHACHA20
#define EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20
#endif
#ifdef DRIVER_ENABLE_ENCRYPTED_VECTOR
#define EIP130TOKEN_ENABLE_ENCRYPTED_VECTOR
#endif


#endif /* INCLUDE_GUARD_CS_EIP130_TOKEN_H */

/* end of file cs_eip130token.h */
