/* c_adapter_vex.h
 *
 * Configuration options for the VaultIP Exchange
 * The project-specific cs_adapter_vex.h file is included,
 * whereafter defaults are provided for missing parameters.
 */

/*****************************************************************************
* Copyright (c) 2015 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://essoemsupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_C_ADAPTER_VEX_H
#define INCLUDE_GUARD_C_ADAPTER_VEX_H

/*----------------------------------------------------------------
 * Defines that can be used in the cs_xxx.h file
 */

/* currently none */


/*----------------------------------------------------------------
 *  cs_adapter_vex.h inclusion
 */
#include "cs_adapter_vex.h"

#ifndef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX  LOG_SEVERITY_WARN
#endif

#ifndef VEX_LOG_PREFIX
#define VEX_LOG_PREFIX     "VEX: "
#endif

#ifndef VEX_MAILBOX_NR
// Set the default mailbox to use
#define VEX_MAILBOX_NR  1
#endif

#ifndef VEX_CRYPTO_OFFICER_ID
// Set the default Crypto Officer ID to use
#define VEX_CRYPTO_OFFICER_ID  0x4F5A3647
#endif

#ifndef VEX_POLLING_SKIP_FIRST_DELAYS
// Set the default
#define VEX_POLLING_SKIP_FIRST_DELAYS  50
#endif

#ifndef VEX_POLLING_DELAY_MS
// Set the default
#define VEX_POLLING_DELAY_MS  1
#endif

#ifndef VEX_POLLING_MAXLOOPS
// Set the default
#define VEX_POLLING_MAXLOOPS  2000
#endif

#endif /* INCLUDE_GUARD_C_ADAPTER_VEX_H */

/* end of file c_adapter_vex.h */
