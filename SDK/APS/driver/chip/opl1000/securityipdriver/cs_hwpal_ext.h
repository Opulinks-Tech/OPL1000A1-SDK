/* cs_hwpal_ext.h
 *
 * VaultIP-1XX (FPGA) chip specific configuration parameters
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

#ifndef CS_HWPAL_EXT_H_
#define CS_HWPAL_EXT_H_


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

// For obtaining the IRQ number
#ifdef DRIVER_INTERRUPTS
#define HWPAL_INTERRUPTS
#endif

// Disable PCI Configuration Space support support
#define HWPAL_REMOVE_DEVICE_PCICONFIGSPACE

// Device name in the Device Tree Structure
#define HWPAL_PLATFORM_DEVICE_NAME  "vaultip"
//#define HWPAL_DRIVER_NAME           "vaultip-1xx"

// Index of the IRQ in the "interrupts" property of the Open Firmware device
// tree entry. 0 is the first IRQ listed, 1 is the second IRQ listed, etc.
#define HWPAL_PLATFORM_IRQ_IDX 0

#define HWPAL_REMAP_ADDRESSES   ;


// Definition of static resources inside the device
// Refer to the data sheet of device for the correct values
//                       Name           DevNr  Start    Last  Flags (see below)
#if 0
#define HWPAL_DEVICES \
        HWPAL_DEVICE_ADD("VAULTIP",         0, 0x60000000, 0x6FFFFFFF, 0), \
        HWPAL_DEVICE_ADD("VaultIP-1xx_NS0", 0, 0x00020000, 0x0003FFFF, 0), \
        HWPAL_DEVICE_ADD("VaultIP-1xx_S1",  0, 0x00040000, 0x0005FFFF, 0), \
        HWPAL_DEVICE_ADD("VaultIP-1xx_NS1", 0, 0x00060000, 0x0007FFFF, 0), \
        HWPAL_DEVICE_ADD("VaultIP-1xx_AIC", 0, 0x00003E00, 0x00003E1F, 0),
#else
#define HWPAL_DEVICES \
        HWPAL_DEVICE_ADD("VAULTIP",         0, 0x60000000, 0x6FFFFFFF, 0),
#endif //#if 0

// Flags:
//   bit0 = Trace reads (requires HWPAL_TRACE_DEVICE_READ)
//   bit1 = Trace writes (requires HWPAL_TRACE_DEVICE_WRITE)
//   bit2 = Swap word endianess (requires HWPAL_DEVICE_ENABLE_SWAP)

// Enables DMA resources banks so that different memory regions can be used
// for DMA buffer allocation
#ifdef DRIVER_DMARESOURCE_BANKS_ENABLE
#define HWPAL_DMARESOURCE_BANKS_ENABLE
#endif // DRIVER_DMARESOURCE_BANKS_ENABLE

#ifdef HWPAL_DMARESOURCE_BANKS_ENABLE
// Definition of DMA banks, one dynamic and 1 static
//                                 Bank    Type   Shared  Cached  Addr  Blocks   Block Size
#define HWPAL_DMARESOURCE_BANKS                                                              \
        HWPAL_DMARESOURCE_BANK_ADD (0,       0,     0,      1,      0,    0,         0),     \
        HWPAL_DMARESOURCE_BANK_ADD (1,       1,     1,      1,      0,                       \
                                    DRIVER_DMA_BANK_ELEMENT_COUNT,                           \
                                    DRIVER_DMA_BANK_ELEMENT_BYTE_COUNT)
#endif // HWPAL_DMARESOURCE_BANKS_ENABLE

#define HWPAL_DMARESOURCE_ADDR_MASK               0xffffffffULL


#endif /* CS_HWPAL_EXT_H_ */


/* end of file cs_hwpal_ext.h */
