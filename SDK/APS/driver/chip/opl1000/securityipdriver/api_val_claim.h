/**
 * @file api_val_claim.h
 *
 * @brief VAL API - Mailbox locking control services
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

#ifndef INCLUDE_GUARD_API_VAL_CLAIM_H
#define INCLUDE_GUARD_API_VAL_CLAIM_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "api_val_result.h"         // ValStatus_t


/**----------------------------------------------------------------------------
 * Next functions are intended to control an exclusive lock of the VaultIP
 * mailbox for the calling process. Generally, the mailbox is locked for each
 * VaultIP operation and unlocked immediately afterwards, but with these
 * functions the mailbox can be locked exclusively for the calling process so
 * that a sequence of VaultIP operations can be performed.
 * The main procedure is:
 * - val_Claim                  exclusively locks the mailbox for use
 * - ...                        various VaultIP operations
 * - val_ServiceZeroizeMailbox  if needed, clears security relevant information
 * - val_ClaimRelease           releases the exclusive mailbox lock
 */

/**----------------------------------------------------------------------------
 * val_Claim
 *
 * This function claims exclusive use of the mailbox for the calling process,
 * if possible.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_Claim(void);

/**----------------------------------------------------------------------------
 * val_ClaimOverrule
 *
 * This function claims exclusive use of the mailboxfor the calling process.
 * Note that this function overrules any existing claim and can only be
 * performed by processes running on the master CPU (Host).
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ClaimOverrule(void);

/**----------------------------------------------------------------------------
 * val_Release
 *
 * This function releases the exclusive use of the mailbox, if one was pending
 * for the calling process.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ClaimRelease(void);


#endif /* Include Guard */

/* end of file api_val_claim.h */
