/**
 * @file api_val_nop.h
 *
 * @brief VAL API - NOP service
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

#ifndef INCLUDE_GUARD_API_VAL_NOP_H
#define INCLUDE_GUARD_API_VAL_NOP_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "basic_defs.h"             // uint8_t, uint16_t, uint32_t, uint64_t

#include "api_val_result.h"         // ValStatus_t
#include "api_val_buffers.h"        // ValOctetsIn_t, ValOctetsOut_t


/**----------------------------------------------------------------------------
 * val_NOP
 *
 * This function allows data to be copied with the DMA engine.
 *
 * @param [in] SrcData_p
 *     Pointer to the buffer with the data that needs to be copied.
 *
 *
 * @param [in] DstData_p
 *     Pointer to the buffer in which the data needs to be copied.
 *
 * @param [in] DataSize
 *     The size of data to be copied.
 *     Note that the size is equal for the Source and Destination Data and
 *     will be round-up to a multiple of 4 bytes.
 *
 * @return One of the ValStatus_t values.
 */
ValStatus_t
val_NOP(
        ValOctetsIn_t * const  SrcData_p,
        ValOctetsOut_t * const DstData_p,
        const ValSize_t  DataSize);


#endif /* Include Guard */

/* end of file api_val_nop.h */
