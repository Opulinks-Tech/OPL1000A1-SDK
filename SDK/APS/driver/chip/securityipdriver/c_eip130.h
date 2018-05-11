/* c_eip130.h
 *
 * Configuration options for the EIP130 module.
 * The project-specific cs_eip130.h file is included,
 * whereafter defaults are provided for missing parameters.
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

#ifndef INCLUDE_GUARD_C_EIP130_H
#define INCLUDE_GUARD_C_EIP130_H

/*----------------------------------------------------------------
 * Defines that can be used in the cs_eip130.h file
 */
// Set this option to enable checking of all arguments to all EIP130 functions
// disable it to reduce code size and reduce overhead
// #define EIP130_STRICT_ARGS

// Footprint reduction switches
// #define EIP130_REMOVE_MAILBOXACCESSVERIFY
// #define EIP130_REMOVE_MAILBOXGETOPTIONS
// #define EIP130_REMOVE_MAILBOXACCESSCONTROL
// #define EIP130_REMOVE_MAILBOXLINK
// #define EIP130_REMOVE_MAILBOXUNLINK
// #define EIP130_REMOVE_MAILBOXCANWRITETOKEN
// #define EIP130_REMOVE_MAILBOXRAWSTATUS
// #define EIP130_REMOVE_MAILBOXRESET
// #define EIP130_REMOVE_MAILBOXLINKID
// #define EIP130_REMOVE_MAILBOXOUTID
// #define EIP130_REMOVE_FIRMWAREDOWNLOAD

/*----------------------------------------------------------------
 * inclusion of cs_eip130.h
 */
#include "cs_eip130.h"


/*----------------------------------------------------------------
 * provide backup values for all missing configuration parameters
 */


/*----------------------------------------------------------------
 * other configuration parameters that cannot be set in cs_xxx.h
 * but are considered product-configurable anyway
 */


#endif /* INCLUDE_GUARD_C_EIP130_H */

/* end of file c_eip130.h */
