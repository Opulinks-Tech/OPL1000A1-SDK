/**
 * @file api_val_service.h
 *
 * @brief VAL API - Service related services
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

#ifndef INCLUDE_GUARD_API_VAL_SERVICE_H
#define INCLUDE_GUARD_API_VAL_SERVICE_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "basic_defs.h"             // uint8_t, uint16_t, uint32_t, uint64_t

#include "api_val_result.h"         // ValStatus_t


/**----------------------------------------------------------------------------
 * val_ServiceRegisterRead
 *
 * This function reads the specified internal register (address) of VaultIP.
 *
 * @param [in] Address
 *     Address of the internal register to write.
 *
 * @param [out] Value_p
 *     Pointer to the buffer in which the value needs to be returned.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceRegisterRead(
        uint32_t         Address,
        uint32_t * const Value_p);


/**----------------------------------------------------------------------------
 * val_ServiceRegisterWrite
 *
 * This function writes a value to the specified internal register (address)
 * of VaultIP.
 *
 * @param [in] Address
 *     Address of the internal register to write.
 *
 * @param [in] Value
 *     Value to write.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceRegisterWrite(
        uint32_t Address,
        uint32_t Value);


/**----------------------------------------------------------------------------
 * val_ServiceZeroizeMailbox
 *
 * This function zeroizes the output mailbox to make sure that the output
 * mailbox does not hold sensitive information when the mailbox is released.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceZeroizeMailbox(void);


/**----------------------------------------------------------------------------
 * val_ServiceSelectOTPZeroize
 *
 * This function selects the OTP zeroize functionality only. To execute the
 * actual OTP zeroize operation, this function call must be followed by calling
 * the function val_ServiceZeroizeOTP().
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceSelectOTPZeroize(void);


/**----------------------------------------------------------------------------
 * val_ServiceZeroizeOTP
 *
 * This function executes the OTP zeroize operation, when the OTP zeroize
 * functionality was previously selected with the val_ServiceSelectOTPZeroize()
 * function.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceZeroizeOTP(void);


/**----------------------------------------------------------------------------
 * val_ServiceClockSwitch
 *
 * This function controls the switch that activates the Encryption, Hash, TRNG,
 * PKCP, VaultIP and Custom Engine clocks. Normally, the firmware switches the
 * clocks automatically for optimized power consumption.
 * Note that all clocks with 0b’s in the ClocksForcedOn and ClocksForcedOff
 * bitmask are set to 'Automatic mode'.
 *
 * @param [in] ClocksForcedOn
 *     Bitmask specifying (with 1b’s) the clocks that needs to be forced ON
 *     (always ON).
 *
 * @param [in] ClocksForcedOff
 *     Bitmask specifying (with 1b’s) the clocks that needs to be forced OFF
 *     (always OFF).
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_ServiceClockSwitch(
        uint16_t ClocksForcedOn,
        uint16_t ClocksForcedOff);


#endif /* Include Guard */


/* end of file api_val_service.h */
