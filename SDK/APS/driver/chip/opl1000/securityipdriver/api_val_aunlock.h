/**
 * @file api_val_aunlock.h
 *
 * @brief VAL API - Authenticated Unlock and Secure Debug related services
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

#ifndef INCLUDE_GUARD_API_VAL_AUNLOCK_H
#define INCLUDE_GUARD_API_VAL_AUNLOCK_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "api_val_buffers.h"        // ValOctetsIn_t, ValOctetsOut_t, ValSize_t
#include "api_val_asset.h"          // ValAssetId_t, ValAssetNumber_t
#include "api_val_asym.h"           // ValAsymBigInt_t


/**----------------------------------------------------------------------------
 * val_AuthenticatedUnlockStart
 *
 * This function initializes and starts the Authenticated Unlocked scheme.
 *
 * @param [in] AuthKeyNumber
 *     The asset number to search for in the (static) Asset Store that contains
 *     the Authenticated Unlocked public key. Note that the supported values
 *     for AuthKeyNumber are defined by the device manufacturer and vendor
 *     and can differ from product to product.
 *
 * @param [out] AuthStateASId_p
 *     Pointer to the memory location where the AssetId for Authenticated
 *     Unlock state will be written. The AssetId is needed
 *     for all Authenticated Unlock related operations.
 *
 * @param [out] Nonce_p
 *     Pointer to the memory location where the VaultIP Nonce will be
 *     written for the signing and is later on used for the verification.
 *
 * @param [in,out] NonceLength_p
 *     Pointer to the memory location where the size of the Nonce in bytes
 *     will be written.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_AuthenticatedUnlockStart(
        ValAssetNumber_t AuthKeyNumber,
        ValAssetId_t * AuthStateASId_p,
        ValOctetsOut_t * Nonce_p,
        ValSize_t * NonceLength_p);


/**----------------------------------------------------------------------------
 * val_AuthenticatedUnlockVerify
 *
 * This function verifies the signature to get to the unlocked state.
 *
 * @param [in] AuthStateASId
 *     Reference to the Authenticated Unlock state Asset.
 *     This reference is provided by val_AuthenticatedUnlockStart().
 *
 * @param [in] Signature_p
 *     Pointer to the memory location where the signature is located for
 *     the verification.
 *
 * @param [in] Nonce_p
 *     Pointer to the memory location where the host Nonce is located for
 *     the verification.
 *
 * @param [in] NonceLength
 *     Size of the Nonce in bytes.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_AuthenticatedUnlockVerify(
        const ValAssetId_t AuthStateASId,
        ValAsymBigInt_t * const Signature_p,
        ValOctetsIn_t * Nonce_p,
        const ValSize_t NonceLength);


/**----------------------------------------------------------------------------
 * val_AuthenticatedUnlockRelease
 *
 * This function release the Authenticated Unlock state Asset.
 *
 * @param [in] AuthStateASId
 *     Reference to the Authenticated Unlock state Asset.
 *     This reference is provided by val_AuthenticatedUnlockStart().
 *
 * @return One of the ValStatus_t values.
 */
#define val_AuthenticatedUnlockRelease(AuthStateASId) val_AssetFree(AuthStateASId)


/**----------------------------------------------------------------------------
 * val_SecureDebug
 *
 * This function set the Secure Debug port state after a successfull
 * authenticated unlock operation.
 *
 * @param [in] AuthStateASId
 *     Reference to the Authenticated Unlock state Asset.
 *     This reference is provided by val_AuthenticatedUnlockStart().
 *
 * @param [in] fSet
 *     Indication to enable (true) or disable (false) the debug port.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SecureDebug(
        const ValAssetId_t AuthStateASId,
        const bool fSet);


#endif /* Include Guard */

/* end of file api_val_aunlock.h */
