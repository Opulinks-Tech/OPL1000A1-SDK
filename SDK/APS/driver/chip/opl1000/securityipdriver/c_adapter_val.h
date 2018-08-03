/** @file c_adapter_val.h
 *
 * @brief Configuration options for the VaultIP Abstraction Layer
 *
 * The project-specific cs_adapter_val.h file is included,
 * whereafter defaults are provided for missing parameters.
 */

/*****************************************************************************
* Copyright (c) 2014-2016 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_C_ADAPTER_VAL_H
#define INCLUDE_GUARD_C_ADAPTER_VAL_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_adapter_val.h"

/** Maximum log severity */
#ifndef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX  LOG_SEVERITY_WARN
#endif

/** Authenticated Unlock avialability */
#ifndef VAL_REMOVE_SECURE_DEBUG
#ifdef VAL_REMOVE_AUTH_UNLOCK
#undef VAL_REMOVE_AUTH_UNLOCK
#endif
#endif


#endif /* INCLUDE_GUARD_C_ADAPTER_VAL_H */

/* end of file c_adapter_val.h */
