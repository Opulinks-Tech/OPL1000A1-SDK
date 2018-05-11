/* adapter_getpid.h
 *
 * Data types and Interfaces
 */

/*****************************************************************************
* Copyright (c) 2015-2016 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_ADAPTER_GETPID_H
#define INCLUDE_GUARD_ADAPTER_GETPID_H

// Driver Framework Basic Defs API
#include "basic_defs.h"

/*----------------------------------------------------------------------------
 *                           Adapter time management
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 * Adapter_SleepMS
 *
 * This function will sleep the calling context for at most the requested
 * amount of time (milliseconds) and then returns.
 *
 */
int
Adapter_GetPid(void);

#endif /* Include Guard */

/* end of file adapter_getpid.h */
