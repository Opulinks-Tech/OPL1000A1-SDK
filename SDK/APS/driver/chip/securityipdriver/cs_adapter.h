/* cs_adapter.h
 *
 * Configuration Settings for the Driver Adapter module.
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
* https://essoemsupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_CS_ADAPTER_H
#define INCLUDE_GUARD_CS_ADAPTER_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "cs_driver.h"


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** Adapter/driver name */
#define ADAPTER_DRIVER_NAME "driver-eip130"

/** Maximum log severity - log level for the entire adapter (for now)
 *  choose from LOG_SEVERITY_INFO, LOG_SEVERITY_WARN, LOG_SEVERITY_CRIT */
#ifdef DRIVER_PERFORMANCE
#define LOG_SEVERITY_MAX LOG_SEVERITY_CRITICAL
#else
#define LOG_SEVERITY_MAX  LOG_SEVERITY_WARN
#endif

/** Bouncebuffers use */
#ifndef DRIVER_BOUNCEBUFFERS
#define ADAPTER_REMOVE_BOUNCEBUFFERS
#endif

/** Endian swap use */
#ifdef DRIVER_SWAPENDIAN
#define ADAPTER_ENABLE_SWAP
#endif

/** Adapter/driver host and device type */
// Is host platform 64-bit?
#ifdef DRIVER_64BIT_HOST
#define ADAPTER_64BIT_HOST
// Is device 64-bit? Only makes sense on 64-bit host.
#ifdef DRIVER_64BIT_DEVICE
#define ADAPTER_64BIT_DEVICE
#endif  // DRIVER_64BIT_DEVICE
#endif  // DRIVER_64BIT_HOST

/** Strict argument checking use */
#ifndef DRIVER_PERFORMANCE
#define ADAPTER_STRICT_ARGS
#endif

// Misc options
/** Maximum DMA resource handles */
#define ADAPTER_MAX_DMARESOURCE_HANDLES  128

/** Interrupt use */
#ifdef DRIVER_INTERRUPTS
#define ADAPTER_EIP130_INTERRUPTS_ENABLE
#endif

/** EIP130 interrupt signals\n
 *  Assigned values represent interrupt source bit numbers */
enum
{
    IRQ_MBX1_IN_FREE_IRQ  = 0,
    IRQ_MBX1_OUT_FULL_IRQ = 1,
    IRQ_MBX2_IN_FREE_IRQ  = 2,
    IRQ_MBX2_OUT_FULL_IRQ = 3,
    IRQ_MBX3_IN_FREE_IRQ  = 4,
    IRQ_MBX3_OUT_FULL_IRQ = 5,
    IRQ_MBX4_IN_FREE_IRQ  = 6,
    IRQ_MBX4_OUT_FULL_IRQ = 7,
    IRQ_MBX_LINKABLE_IRQ  = 8,
};

#define ADAPTER_IRQ_MBX1_IN_FREE_IRQ_NAME     "IRQ_MBX1_FREE"
#define ADAPTER_IRQ_MBX1_OUT_FULL_IRQ_NAME    "IRQ_MBX1_FULL"
#define ADAPTER_IRQ_MBX2_IN_FREE_IRQ_NAME     "IRQ_MBX2_FREE"
#define ADAPTER_IRQ_MBX2_OUT_FULL_IRQ_NAME    "IRQ_MBX2_FULL"
#define ADAPTER_IRQ_MBX3_IN_FREE_IRQ_NAME     "IRQ_MBX3_FREE"
#define ADAPTER_IRQ_MBX3_OUT_FULL_IRQ_NAME    "IRQ_MBX3_FULL"
#define ADAPTER_IRQ_MBX4_IN_FREE_IRQ_NAME     "IRQ_MBX4_FREE"
#define ADAPTER_IRQ_MBX4_OUT_FULL_IRQ_NAME    "IRQ_MBX4_FULL"
#define ADAPTER_IRQ_MBX_LINKABLE_IRQ_NAME     "IRQ_MBX_LINKABLE"

/** Cache line size */
#define ADAPTER_PCICONFIG_CACHELINESIZE 1

#endif /* INCLUDE_GUARD_CS_ADAPTER_H */

/* end of file cs_adapter.h */
