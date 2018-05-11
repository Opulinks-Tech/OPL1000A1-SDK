/**
 * @file api_val_system.h
 *
 * @brief VAL API - System related services
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

#ifndef INCLUDE_GUARD_API_VAL_SYSTEM_H
#define INCLUDE_GUARD_API_VAL_SYSTEM_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "basic_defs.h"             // uint8_t, uint16_t, uint32_t, uint64_t

#include "api_val_result.h"         // ValStatus_t
#include "api_val_buffers.h"        // ValOctetsIn_t, ValOctetsOut_t
#include "api_val_asset.h"          // ValAssetId_t


/**----------------------------------------------------------------------------
 * val_SystemGetVersion
 *
 * This function retrieves the version information and memory size from the
 * VaultIP device and returns this in a string.
 *
 * @param [out] Version_p
 *     Pointer to the buffer in which the version information and memory size
 *     needs to be returned.
 *
 * @param [in,out] VersionSize_p
 *     Pointer to the size of buffer, upon return it is updated with
 *     the actual size of the version information.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemGetVersion(
        ValOctetsOut_t * const Version_p,
        ValSize_t * const VersionSize_p);


/**----------------------------------------------------------------------------
 * val_SystemGetState
 *
 * This function retrieves the version information from the VaultIP and
 * returns this in a string.
 *
 * @param [out] OtpErrorCode_p
 *     Pointer to the variable in which the OTP scan error must be returned.
 *     Note: Pointer may be NULL if information is not required.
 *
 * @param [out] OtpErrorLocation_p
 *     Pointer to the variable in which the first location of OTP scan error
 *     must be returned.
 *     Note: Pointer may be NULL if information is not required.
 *
 * @param [out] Mode_p
 *     Pointer to the variable in which the mode must be returned.
 *     Note: Pointer may be NULL if information is not required.
 *     The value can be:
 *     0   : Non-FIPS mode
 *     4..6: Error mode (see VaultIP Firmware manual for details
 *     15  : FIPS mode
 *
 * @param [out] CryptoOfficer_p
 *     Pointer to the variable in which the availability of the Crypto Officer
 *     ID must be returned.
 *     Note: Pointer may be NULL if information is not required.
 *     The value can be:
 *     0 : Not available
 *     !0: Available
 *
 * @param [out] HostID_p
 *     Pointer to the variable in which the host ID of the caller must be
 *     returned.
 *     Note: Pointer may be NULL if information is not required.
 *     The value can be in the range [0..7].
 *
 * @param [out] NonSecure_p
 *     Pointer to the variable in which the secure/non-secure state of the
 *     caller must be returned.
 *     Note: Pointer may be NULL if information is not required.
 *     The value can be:
 *     0 : Secure
 *     !0: Non-Secure
 *
 * @param [out] Identity_p
 *     Pointer to the variable in which the identity of the caller must be
 *     returned.
 *     Note: Pointer may be NULL if information is not required.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemGetState(
        uint8_t * const OtpErrorCode_p,
        uint16_t * const OtpErrorLocation_p,
        uint8_t * const Mode_p,
        uint8_t * const CryptoOfficer_p,
        uint8_t * const HostID_p,
        uint8_t * const NonSecure_p,
        uint32_t * const Identity_p);


/**----------------------------------------------------------------------------
 * val_SystemReset
 *
 * This function resets the VaultIP firmware.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemReset(void);


/**----------------------------------------------------------------------------
 * val_SystemSelfTest
 *
 * This function starts the self test either to switch to FIPS mode or recheck
 * the system.
 * NOTE: The function switches to FIPS mode in which strict users checking is
 *       performed. Therefore, this function is classified as FIPS function
 *       and can only be used FIPS mode is enabled.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemSelfTest(void);


/**----------------------------------------------------------------------------
 * val_SystemDefineUsers
 *
 * This function defines the allowed users for the FIPS mode.
 * NOTE: The function can only be used by the Crypto Officer and is intended
 *       to configure FIPS mode functionality, therefore this function can only
 *       be used when FIPS and Crypto Officer functionality are enabled.
 *
 * @param [in] User1
 *     The identity of User1.
 *
 * @param [in] User2
 *     The identity of User2.
 *
 * @param [in] User3
 *     The identity of User3.
 *
 * @param [in] User4
 *     The identity of User4.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemDefineUsers(
        const uint32_t User1,
        const uint32_t User2,
        const uint32_t User3,
        const uint32_t User4);


/**----------------------------------------------------------------------------
 * val_SystemSleep
 *
 * This function forces the VaultIP to go to sleep mode. This means that after
 * successful execution, the VaultIP with exception of the DataRAM and optional
 * the FW-RAM can be powered off.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemSleep(void);


/**----------------------------------------------------------------------------
 * val_SystemResumeFromSleep
 *
 * This function resumes the VaultIP from sleep mode and switches to normal
 * operation again.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemResumeFromSleep(void);


/**----------------------------------------------------------------------------
 * val_SystemHibernation
 *
 * This function forces the VaultIP to go to hibernation mode. This means that
 * after successful execution, the VaultIP can be powered off.
 *
 * @param [in] StateAssetId
 *     Asset ID of the Asset that holds the state.
 *
 * @param [in] KeyAssetId
 *     Reference to the Key Encryption Key Asset to be used when generating
 *     the data blob.
 *
 * @param [in] AssociatedData_p
 *     Pointer to the Associated Data used for the data blob authentication.
 *     The exact same Associated Data must be provided for the Resume From
 *     Hibernation operation.
 *
 * @param [in] AssociatedDataSize
 *     The size in bytes for the Associated Data used for the data blob
 *     authentication. (AssociatedData_p)
 *
 * @param [out] DataBlob_p
 *     Pointer to the data blob buffer.
 *
 * @param [in,out] DataBlobSize_p
 *     Size of the buffer intended for the data blob (DataBlob_p).
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemHibernation(
        const ValAssetId_t StateAssetId,
        const ValAssetId_t KEKAssetId,
        ValOctetsIn_t * const AssociatedData_p,
        const ValSize_t AssociatedDataSize,
        ValOctetsOut_t * const DataBlob_p,
        ValSize_t * const DataBlobSize_p);


/**----------------------------------------------------------------------------
 * val_SystemResumeFromHibernation
 *
 * This function resumes the VaultIP from hibernation mode and switches to
 * normal operation again.
 *
 * @param [in] StateAssetId
 *     Asset ID of the Asset that holds the state.
 *
 * @param [in] KeyAssetId
 *     Reference to the Key Encryption Key Asset to be used when generating
 *     the data blob.
 *
 * @param [in] AssociatedData_p
 *     Pointer to the Associated Data used for the data blob authentication.
 *     The exact same Associated Data must be provided for the Hibernation
 *     operation.
 *
 * @param [in] AssociatedDataSize
 *     The size in bytes for the Associated Data used for the data blob
 *     authentication. (AssociatedData_p)
 *
 * @param [in] DataBlob_p
 *     Pointer to the data blob buffer.
 *
 * @param [in] DataBlobSize
 *     Size of the buffer of the data blob (DataBlob_p).
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SystemResumeFromHibernation(
        const ValAssetId_t StateAssetId,
        const ValAssetId_t KEKAssetId,
        ValOctetsIn_t * const AssociatedData_p,
        const ValSize_t AssociatedDataSize,
        ValOctetsIn_t * const DataBlob_p,
        const ValSize_t DataBlobSize);


/**----------------------------------------------------------------------------
 * val_IsAccessSecure
 *
 * This function returns an indication of the device access is secure or
 * non-secure.
 * Note: If no device connection exists, the function will assume Non-secure
 *       access.
 *
 * @return true  = Secure.
 * @return false = Non-secure.
 */
bool
val_IsAccessSecure(void);


#endif /* Include Guard */

/* end of file api_val_system.h */
