/** @file cs_eip130.h
 *
 * @brief Configuration Settings for the EIP130 module.
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

#ifndef INCLUDE_GUARD_CS_EIP130_H
#define INCLUDE_GUARD_CS_EIP130_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_driver.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** Strict argument checking use */
#ifndef DRIVER_PERFORMANCE
//#define EIP130_STRICT_ARGS
#endif

/** Footprint reduction switches */
//#define EIP130_REMOVE_MODULEGETOPTIONS
//#define EIP130_REMOVE_MODULEGETSTATUS
#ifndef DRIVER_ENABLE_FIRMWARE_SLEEP
#define EIP130_REMOVE_MODULEFIRMWAREWRITTEN
#endif

//#define EIP130_REMOVE_MAILBOXGETOPTIONS
//#define EIP130_REMOVE_MAILBOXACCESSVERIFY
//#define EIP130_REMOVE_MAILBOXACCESSCONTROL
//#define EIP130_REMOVE_MAILBOXLINK
//#define EIP130_REMOVE_MAILBOXLINKRESET
//#define EIP130_REMOVE_MAILBOXUNLINK
//#define EIP130_REMOVE_MAILBOXCANWRITETOKEN
#define EIP130_REMOVE_MAILBOXRAWSTATUS
#define EIP130_REMOVE_MAILBOXRESET
#define EIP130_REMOVE_MAILBOXLINKID
#define EIP130_REMOVE_MAILBOXOUTID

#ifndef DRIVER_ENABLE_FIRMWARE_LOAD
#define EIP130_REMOVE_FIRMWAREDOWNLOAD
#endif

#endif /* INCLUDE_GUARD_CS_EIP130_H */

/* end of file cs_eip130.h */
