/** @file cs_hwpal_lkm.h
 *
 * @brief Configuration Settings for the hardware platform abstraction layer
 *        intended for the Linux Kernel Module.
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
* In case you do not have an account for
* this system, please send an e-mail to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_CS_HWPAL_LKM_H
#define INCLUDE_GUARD_CS_HWPAL_LKM_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_hwpal.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
#ifdef ARCH_ARM64
// Enable cache-coherent DMA buffer allocation
#define HWPAL_DMARESOURCE_ALLOC_CACHE_COHERENT
#endif

#if defined(ARCH_ARM) || defined(ARCH_ARM64)
// Use non-cached DMA buffer allocation
//#define HWPAL_DMARESOURCE_ALLOC_CACHE_COHERENT

// Use minimum required cache-control functionality for DMA-safe buffers
//#define HWPAL_DMARESOURCE_MINIMUM_CACHE_CONTROL

//#define HWPAL_DMARESOURCE_DIRECT_DCHACHE_CONTROL
//#define HWPAL_DMARESOURCE_DSB_ENABLE
#endif // defined(ARCH_ARM) || defined(ARCH_ARM64)

#if defined(DRIVER_SWAPENDIAN) && defined(DRIVER_ENABLE_SWAP_SLAVE)
//#define HWPAL_DEVICE_ENABLE_SWAP
#endif // DRIVER_SWAPENDIAN


#define HWPAL_DMARESOURCE_BANK_STATIC_PCI_OFFSET  0x200000


#endif // INCLUDE_GUARD_CS_HWPAL_LKM_H

/* end of file cs_hwpal_lkm.h */
