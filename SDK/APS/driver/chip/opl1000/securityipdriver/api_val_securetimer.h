/**
 * @file api_val_securetimer.h
 *
 * @brief VAL API - Secure Timer related services
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

#ifndef INCLUDE_GUARD_API_VAL_SECURETIMER_H
#define INCLUDE_GUARD_API_VAL_SECURETIMER_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "api_val_asset.h"          // ValAssetId_t, val_AssetFree


/**----------------------------------------------------------------------------
 * val_SecureTimerStart
 *
 * This function starts or restarts a Secure Timer. Based on the Second value
 * either a seconds timer or 100 useconds timer created and started.
 * The return or given AssetId is used as Secure Timer reference.
 *
 * @param [in] fSecond
 *     Indication that a seconds timer must be created, otherwise a 100 useconds
 *     timer is created.
 *
 * @param [out] AssetId_p
 *     Pointer to the memory location where the AssetId for Secure Timer will
 *     be written when the Timer is successfully started. The AssetId is needed
 *     for all Secure Timer operations.
 *     In case of a restart this pointer points of to the AssetId of an already
 *     created timer
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SecureTimerStart(
        const bool fSecond,
        ValAssetId_t * const AssetId_p);


/**----------------------------------------------------------------------------
 * val_SecureTimerStop
 *
 * This function stops the Secure Timer after reading the elapsed time.
 * The returned time can be either a number of seconds or 100 useconds
 * depending on the timer.
 * Note that the Secure Timer Asset is released automatically when this
 * function is called, so val_SecureTimerRelease() does not have to be called.
 *
 * @param [in] AssetId
 *     Reference to the Secure Timer Asset.
 *     This reference is provided by val_SecureTimerStart().
 *
 * @param [out] ElapsedTime_p
 *     Pointer to the memory location where the elapsed time must be written.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SecureTimerStop(
        const ValAssetId_t AssetId,
        uint32_t * const ElapsedTime_p);


/**----------------------------------------------------------------------------
 * val_SecureTimerRead
 *
 * This function reads the elapsed time of Secure Timer. The returned time
 * can be either a number of seconds or 100 useconds depending on the timer.
 *
 * @param [in] AssetId
 *     Reference to the Secure Timer Asset.
 *     This reference is provided by val_SecureTimerStart().
 *
 * @param [out] ElapsedTime_p
 *     Pointer to the memory location where the elapsed time must be written.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_SecureTimerRead(
        const ValAssetId_t AssetId,
        uint32_t * const ElapsedTime_p);


/**----------------------------------------------------------------------------
 * val_SecureTimerRelease
 *
 * This function release the Secure Timer asset.
 * Note that the Secure Timer Asset is released automatically when the function
 * val_SecureTimerStop is called.
 *
 * @param [in] AssetId
 *     Reference to the Secure Timer Asset for releasing the timer.
 *     This reference is provided by val_SecureTimerStart().
 *
 * @return One of the ValStatus_t values.
 */
#define val_SecureTimerRelease(AssetId) val_AssetFree(AssetId)


#endif /* Include Guard */


/* end of file api_val_securetimer.h */
