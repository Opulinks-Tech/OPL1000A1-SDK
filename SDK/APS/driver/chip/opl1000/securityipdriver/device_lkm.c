/* device_lkm.c
 *
 * This is the Linux Kernel-mode Driver Framework v4 Device API
 * implementation for open Firmware. The implementation is device-agnostic and
 * receives configuration details from the c_device_lkm.h file.
 *
 */

/*****************************************************************************
* Copyright (c) 2010-2017 INSIDE Secure B.V. All Rights Reserved.
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

#include "device_mgmt.h"            // API to implement
#include "device_rw.h"              // API to implement

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_device_lkm.h"

// Driver Framework Device API
#include "device_swap.h"            // Device_SwapEndian32

// Logging API
#undef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX  HWPAL_LOG_SEVERITY
//#include "log.h"                    // LOG_*

// Driver Framework C Run-Time Library API
#include "clib.h"                   // memcmp

// Driver Framework Basic Definitions API
#include "basic_defs.h"             // uint32_t, NULL, inline, bool,
                                    // IDENTIFIER_NOT_USED

#ifdef HWPAL_USE_UMDEVXS_DEVICE
#include "umdevxs_ofdev.h"
#endif


/*----------------------------------------------------------------------------
 * Definitions and macros
 */

#define HWPAL_PLATFORM_DEVICE_COMPATIBLE     HWPAL_PLATFORM_DEVICE_NAME

#define HWPAL_FLAG_READ     BIT_0   // 1
#define HWPAL_FLAG_WRITE    BIT_1   // 2
#define HWPAL_FLAG_SWAP     BIT_2   // 4
#define HWPAL_FLAG_HA       BIT_5   // 32

// Device administration structure
typedef struct
{
#ifdef HWPAL_DEVICE_MAGIC
    // Magic value for detecting valid handles
    unsigned int ValidHandle;
#endif

    // Name string used in Device_Find
    const char * DeviceName_p;

    // device offset range inside OF device
    unsigned int StartByteOffset;
    unsigned int LastByteOffset;

    // Trace Read, Write flags,
    // Enable byte swap by the host processor flag
    char Flags;
} HWPAL_Device_Administration_t;

// the c_device_lkm.h file defines a HWPAL_DEVICES that
// depends on the following HWPAL_DEVICE_ADD
#ifdef HWPAL_DEVICE_MAGIC
#define HWPAL_DEVICE_ADD(_name, _devrn, _start, _last, _flags) \
        { HWPAL_DEVICE_MAGIC, _name, _start, _last, _flags }
#else
#define HWPAL_DEVICE_ADD(_name, _devrn, _start, _last, _flags) \
        { _name, _start, _last, _flags }
#endif

// the c_device_lkm.h file defines a HWPAL_REMAP_ADDRESSES that
// depends on the following HWPAL_REMAP_ONE
#define HWPAL_REMAP_ONE(_old, _new) \
    case _old: \
        DeviceByteOffset = _new; \
        break;

// number of devices supported calculated on HWPAL_DEVICES defined
// in c_device_lkm.h
#define DEVICE_COUNT \
        (sizeof(HWPAL_Devices) \
         / sizeof(HWPAL_Device_Administration_t))

// checks that byte offset is in range
#define IS_INVALID_OFFSET(_ofs, _devp) \
    (((_devp)->StartByteOffset + (_ofs) > (_devp)->LastByteOffset) || \
     (((_ofs) & 3) != 0))

#ifdef HWPAL_DEVICE_MAGIC
// checks that device handle is valid
#define IS_INVALID_DEVICE(_devp) \
    ((_devp) < HWPAL_Devices || \
     (_devp) >= HWPAL_Devices + DEVICE_COUNT || \
     (_devp)->ValidHandle != HWPAL_DEVICE_MAGIC)
#endif /* HWPAL_DEVICE_MAGIC */


#ifndef HWPAL_USE_UMDEVXS_DEVICE
// Device virtual IRQ number
static unsigned int HWPAL_Device_OF_IRQ = 0;
#endif


/*----------------------------------------------------------------------------
 * Forward declarations
 */

#ifndef HWPAL_USE_UMDEVXS_DEVICE
static int
HWPAL_OFDev_Probe(
        struct platform_device * OF_Device_p);

static int
HWPAL_OFDev_Remove(
        struct platform_device * OF_Device_p);
#endif


// Global administration data
typedef struct
{
    // Device data
    struct platform_device * Platform_Device_p;

    // Physical base address of the device resource (MMIO space)
    void * PhysBaseAddr;

    // Mapped (virtual) address of the device resource (MMIO space)
    //uint32_t __iomem * MappedBaseAddr_p;
    uint32_t * MappedBaseAddr_p;

} HWPAL_Global_Admin_t;


/*----------------------------------------------------------------------------
 * Local variables
 */

static const HWPAL_Device_Administration_t HWPAL_Devices[] =
{
    HWPAL_DEVICES
};

// declarations native to Linux kernel
static struct platform_device * HWPAL_OF_Device_p = NULL;

// virtual address returned by ioremap()
static uint32_t * HWPAL_MappedBaseAddr_p = NULL;

// Global administration data
static HWPAL_Global_Admin_t HWPAL_Admin;


/*----------------------------------------------------------------------------
 * HWPAL_Hexdump
 *
 * This function hex-dumps an array of uint32_t.
 */
#if ((defined(HWPAL_TRACE_DEVICE_READ)) || (defined(HWPAL_TRACE_DEVICE_WRITE)))
static void
HWPAL_Hexdump(
        const char * ArrayName_p,
        const char * DeviceName_p,
        const unsigned int ByteOffset,
        const uint32_t * WordArray_p,
        const unsigned int WordCount,
        bool fSwapEndianness)
{
    unsigned int i;

    Log_FormattedMessage(
        "%s: "
        "byte offsets 0x%x - 0x%x"
        " (%s)\n"
        "  ",
        ArrayName_p,
        ByteOffset,
        ByteOffset + WordCount*4 -1,
        DeviceName_p);

    for (i = 1; i <= WordCount; i++)
    {
        uint32_t Value = WordArray_p[i - 1];

        if (fSwapEndianness)
            Value = Device_SwapEndian32(Value);

        Log_FormattedMessage(" 0x%08x", Value);

        if ((i & 7) == 0)
            Log_Message("\n  ");
    }

    if ((WordCount & 7) != 0)
        Log_Message("\n");
}
#endif


/*----------------------------------------------------------------------------
 * Device_RemapDeviceAddress
 *
 * This function remaps certain device addresses (relative within the whole
 * device address map) to other addresses. This is needed when the integration
 * has remapped some EIP device registers to other addresses. The EIP Driver
 * Libraries assume the devices always have the same internal layout.
 */
int
Device_RemapDeviceAddress(
        unsigned int DeviceByteOffset)
{
#if 0
#ifdef HWPAL_REMAP_ADDRESSES
    switch(DeviceByteOffset)
    {
        // include the remap statements
        HWPAL_REMAP_ADDRESSES

        default:
            break;
    }
#endif
#endif
    return DeviceByteOffset;
}



#define LOG_WARN printf

#if 0
/*-----------------------------------------------------------------------------
 * device_mgmt API
 *
 * These functions support finding a device given its name.
 * A handle is returned that is needed in the device_rw API
 * to read or write the device
 */


/*-----------------------------------------------------------------------------
 * Device_Initialize
 */
int
Device_Initialize(
        void * CustomInitData_p)
{
#ifndef HWPAL_USE_UMDEVXS_DEVICE
    unsigned int i;
    LKM_Init_t  LKMInit;

    ZEROINIT(LKMInit);

    LKMInit.DriverName_p        = HWPAL_DRIVER_NAME;
    LKMInit.ResId               = HWPAL_DEVICE_RESOURCE_ID;
    LKMInit.ResByteCount        = HWPAL_DEVICE_RESOURCE_BYTE_COUNT;
    LKMInit.fRetainMap          = true;

#ifdef HWPAL_DEVICE_USE_RPM
    LKMInit.PM_p                = RPM_OPS_PM;
#endif

    if (LKM_Init(&LKMInit) < 0)
    {
        //LOG_CRIT("%s: Failed to register the platform device\n", __func__);
        return -1;
    }

    // Output default IRQ line number in custom data
    {
        int * p = (int *)CustomInitData_p;

        int * IRQ_p = (int *)LKMInit.CustomInitData_p;

        *p = IRQ_p[0];
    }

#ifdef HWPAL_DEVICE_USE_RPM
    if (RPM_INIT_MACRO(LKM_DeviceGeneric_Get()) != RPM_SUCCESS)
    {
        //LOG_CRIT("%s: RPM_Init() failed\n", __func__);
        LKM_Uninit();
        return -3; // error
    }
#endif

    HWPAL_Admin.MappedBaseAddr_p    = LKM_MappedBaseAddr_Get();
    HWPAL_Admin.Platform_Device_p   = LKM_DeviceSpecific_Get();
    HWPAL_Admin.PhysBaseAddr        = LKM_PhysBaseAddr_Get();

    for(i = 0; i < DEVICE_COUNT; i++)
    {
        LOG_INFO("%s: mapped device '%s', "
                 "virt base addr 0x%p, "
                 "start byte offset 0x%x, "
                 "last byte offset 0x%x\n",
                 __func__,
                 HWPAL_Devices[i].DeviceName_p,
                 HWPAL_Admin.MappedBaseAddr_p,
                 HWPAL_Devices[i].StartByteOffset,
                 HWPAL_Devices[i].LastByteOffset);
    }
#else
    UMDevXS_OFDev_GetDevice(0,
                            &HWPAL_Admin.Platform_Device_p,
                            (void __iomem **)&HWPAL_Admin.MappedBaseAddr_p);

    if (HWPAL_Admin.Platform_Device_p == NULL ||
        HWPAL_Admin.MappedBaseAddr_p == NULL)
    {
        /*LOG_CRIT("%s: Failed, UMDevXS device %p, map address %p\n",
                 __func__,
                 HWPAL_Admin.Platform_Device_p,
                 HWPAL_Admin.MappedBaseAddr_p);*/
        return -1;
    }

    {
        int * p = (int *)CustomInitData_p;

        // Exported under GPL
        *p = platform_get_irq(HWPAL_Admin.Platform_Device_p,
                              HWPAL_PLATFORM_IRQ_IDX);
    }
#endif // HWPAL_USE_UMDEVXS_DEVICE

    return 0;
}
#endif

#if 0
/*-----------------------------------------------------------------------------
 * Device_UnInitialize
 */
void
Device_UnInitialize(void)
{
    LOG_INFO("%s: unregister driver\n", __func__);

#ifndef HWPAL_USE_UMDEVXS_DEVICE

#ifdef HWPAL_DEVICE_USE_RPM
    // Check if a race condition is possible here with auto-suspend timer
    (void)RPM_UNINIT_MACRO();
#endif

    LKM_Uninit();
#endif

    // Reset global administration
    ZEROINIT(HWPAL_Admin);
}
#endif

/*-----------------------------------------------------------------------------
 * Device_Find
 */
Device_Handle_t
Device_Find(
        const char * DeviceName_p)
{
    int i;
    unsigned int NameLen;

    if (DeviceName_p == NULL)
        return NULL; // not supported, thus not found

    // count the device name length, including the terminating zero
    NameLen = 0;
    while (DeviceName_p[NameLen++])
        if (NameLen == HWPAL_MAX_DEVICE_NAME_LENGTH)
            break;

    // walk through the defined devices and compare the name
    for (i = 0; i < DEVICE_COUNT; i++)
        if (memcmp(DeviceName_p, HWPAL_Devices[i].DeviceName_p, NameLen) == 0)
            return (Device_Handle_t)(HWPAL_Devices + i); // device handle

    //LOG_CRIT("%s: Could not find device '%s'", __func__, DeviceName_p);

    return NULL;
}

#if 0
/*-----------------------------------------------------------------------------
 * Device_GetReference
 */
Device_Reference_t
Device_GetReference(
        const Device_Handle_t Device,
        Device_Data_t * const Data_p)
{
    Device_Reference_t DevReference;

    // There exists only one reference for this implementation
    IDENTIFIER_NOT_USED(Device);

    // Not used yet
    IDENTIFIER_NOT_USED(Data_p);

    // Return the platform device reference
    // (pointer to the Linux device structure)
    DevReference = &HWPAL_Admin.Platform_Device_p->dev;

    if (Data_p)
        Data_p->PhysAddr = NULL;

    return DevReference;
}
#endif


/*-----------------------------------------------------------------------------
 * device_rw API
 *
 * These functions can be used to transfer a single 32bit word or an array of
 * 32bit words to or from a device.
 * Endianess swapping is performed on the fly based on the configuration for
 * this device.
 *
 */
#if 0
/*-----------------------------------------------------------------------------
 * Device_Read32
 */
uint32_t
Device_Read32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset)
{
    HWPAL_Device_Administration_t * Device_p;
    uint32_t Value = 0;

    Device_p = (HWPAL_Device_Administration_t *)Device;

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (Device_p == NULL)
        return 0xEEEEEEEE;
#endif

#ifdef HWPAL_DEVICE_MAGIC
    if (IS_INVALID_DEVICE(Device_p))
    {
        //LOG_CRIT("%s: Invalid device handle provided\n", __func__);
        return 0xEEEEEEEE;
    }
#endif /* HWPAL_DEVICE_MAGIC */

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (IS_INVALID_OFFSET(ByteOffset, Device_p))
    {
        /*LOG_CRIT("%s: Invalid ByteOffset 0x%x (device %s)\n",
                 __func__,
                ByteOffset,
                Device_p->DeviceName_p);*/
        return 0xEEEEEEEE;
    }
#endif /* HWPAL_STRICT_ARGS_CHECK */

#ifdef HWPAL_ENABLE_HA_SIMULATION
    if (Device_p->Flags & HWPAL_FLAG_HA)
    {
        // HA simulation mode
        // disable access to PKA_MASTER_SEQ_CTRL
        if (ByteOffset == 0x3FC8)
        {
            Value = 0;
            goto HA_SKIP;
        }
    }
#endif

    {
        unsigned int DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;

        DeviceByteOffset = Device_RemapDeviceAddress(DeviceByteOffset);

#ifdef HWPAL_DEVICE_DIRECT_MEMIO
        Value = *(uint32_t *)(HWPAL_Admin.MappedBaseAddr_p +
                (DeviceByteOffset / 4));
#else
        //Value = ioread32(HWPAL_Admin.MappedBaseAddr_p + (DeviceByteOffset / 4));
#endif

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            Value = Device_SwapEndian32(Value);
#endif

        //smp_rmb();
    }

#ifdef HWPAL_ENABLE_HA_SIMULATION
HA_SKIP:
#endif

#ifdef HWPAL_TRACE_DEVICE_READ
    if (Device_p->Flags & HWPAL_FLAG_READ)
    {
        unsigned int DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;
        unsigned int DeviceByteOffset2 =
                Device_RemapDeviceAddress(DeviceByteOffset);
        if (DeviceByteOffset2 != DeviceByteOffset)
        {
            DeviceByteOffset2 -= Device_p->StartByteOffset;
            Log_FormattedMessage("%s: 0x%x(was 0x%x) = 0x%08x (%s)\n",
                                 __func__,
                    DeviceByteOffset2,
                    ByteOffset,
                    (unsigned int)Value,
                    Device_p->DeviceName_p);
        }
        else
            Log_FormattedMessage("%s: 0x%x = 0x%08x (%s)\n",
                                 __func__,
                    ByteOffset,
                    (unsigned int)Value,
                    Device_p->DeviceName_p);
        }
#endif /* HWPAL_TRACE_DEVICE_READ */

    return Value;
}
#else //#if 0
/*-----------------------------------------------------------------------------
 * Device_Read32
 */
uint32_t
Device_Read32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset)
{
    HWPAL_Device_Administration_t * Device_p = NULL;
    uint32_t Value = 0;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    {
        unsigned int DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;

        Value = *((volatile unsigned int *)DeviceByteOffset);
    }
    
    return Value;
}
#endif

#if 0
/*-----------------------------------------------------------------------------
 * Device_Write32
 */
void
Device_Write32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset,
        const uint32_t ValueIn)
{
    HWPAL_Device_Administration_t * Device_p;
    uint32_t Value = ValueIn;

    Device_p = (HWPAL_Device_Administration_t *)Device;
    if (Device_p == NULL)
        return;

#ifdef HWPAL_DEVICE_MAGIC
    if (IS_INVALID_DEVICE(Device_p))
    {
        //LOG_CRIT("%s: Invalid device handle provided\n", __func__);
        return;
    }
#endif /* HWPAL_DEVICE_MAGIC */

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (IS_INVALID_OFFSET(ByteOffset, Device_p))
    {
        /*LOG_CRIT("%s: Invalid ByteOffset 0x%x (device %s)\n",
                 __func__,
                ByteOffset,
                Device_p->DeviceName_p);*/
        return;
    }
#endif /* HWPAL_STRICT_ARGS_CHECK */

#ifdef HWPAL_TRACE_DEVICE_WRITE
    if (Device_p->Flags & HWPAL_FLAG_WRITE)
        Log_FormattedMessage("%s: 0x%x = 0x%08x (%s)\n",
                             __func__,
                             ByteOffset,
                             (unsigned int)Value,
                             Device_p->DeviceName_p);
#endif /* HWPAL_TRACE_DEVICE_WRITE*/

#ifdef HWPAL_ENABLE_HA_SIMULATION
    if (Device_p->Flags & HWPAL_FLAG_HA)
    {
        // HA simulation mode
        // disable access to PKA_MASTER_SEQ_CTRL
        if (ByteOffset == 0x3FC8)
        {
            /*LOG_CRIT("%s: Unexpected write to PKA_MASTER_SEQ_CTRL\n",
                        __func__);*/
            return;
        }
    }
#endif

    {
        uint32_t DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;

        DeviceByteOffset = Device_RemapDeviceAddress(DeviceByteOffset);

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            Value = Device_SwapEndian32(Value);
#endif

#ifdef HWPAL_DEVICE_DIRECT_MEMIO
        *(uint32_t *)(HWPAL_Admin.MappedBaseAddr_p + (DeviceByteOffset / 4)) =
                    Value;
#else
        //iowrite32(Value, HWPAL_Admin.MappedBaseAddr_p + (DeviceByteOffset / 4));
#endif

        //smp_wmb();
    }
}
#else //#if 0
/*-----------------------------------------------------------------------------
 * Device_Write32
 */
void
Device_Write32(
        const Device_Handle_t Device,
        const unsigned int ByteOffset,
        const uint32_t ValueIn)
{
    HWPAL_Device_Administration_t * Device_p;
    uint32_t Value = ValueIn;
    uint32_t DeviceByteOffset;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    DeviceByteOffset = Device_p->StartByteOffset + ByteOffset;

    *(volatile unsigned int *)DeviceByteOffset = ValueIn;
}
#endif //#if 0

#if 0
/*-----------------------------------------------------------------------------
 * Device_Read32Array
 */
void
Device_Read32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // read starts here, +4 increments
        uint32_t * MemoryDst_p,         // writing starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;

    Device_p = (HWPAL_Device_Administration_t *)Device;

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (Device_p == NULL || MemoryDst_p == NULL || Count <= 0)
        return;

    if (IS_INVALID_OFFSET(Offset, Device_p))
    {
        LOG_CRIT("%s: Invalid ByteOffset 0x%x (device %s)\n",
                 __func__,
               Offset,
               Device_p->DeviceName_p);
        return;
    }
#endif // HWPAL_STRICT_ARGS_CHECK

#ifdef HWPAL_ENABLE_HA_SIMULATION
    if (Device_p->Flags & HWPAL_FLAG_HA)
    {
        // HA simulation mode
        // disable access to PKA_MASTER_SEQ_CTRL
        return;
    }
#endif

    DeviceByteOffset = Device_p->StartByteOffset + Offset;

    {
        unsigned int RemappedOffset;
        uint32_t Value;
        int i;

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        bool fSwap = false;
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif
        for (i = 0; i < Count; i++)
        {
            RemappedOffset = Device_RemapDeviceAddress(DeviceByteOffset);

#ifdef HWPAL_DEVICE_DIRECT_MEMIO
            Value = *(uint32_t*)(HWPAL_Admin.MappedBaseAddr_p +
                                                (RemappedOffset / 4));
#else
            //Value = ioread32(HWPAL_Admin.MappedBaseAddr_p +
            //                                    (RemappedOffset / 4));
#endif

            //smp_rmb();

#ifdef HWPAL_DEVICE_ENABLE_SWAP
            // swap endianess if required
            if (fSwap)
                Value = Device_SwapEndian32(Value);
#endif

            MemoryDst_p[i] = Value;
            DeviceByteOffset +=  4;
        } // for
        }

#ifdef HWPAL_TRACE_DEVICE_READ
    if (Device_p->Flags & HWPAL_FLAG_READ)
    {
        HWPAL_Hexdump("Device_Read32Array",
                      Device_p->DeviceName_p,
                      Device_p->StartByteOffset + Offset,
                      MemoryDst_p,
                      Count,
                      false);     // already swapped during read above
    }
#endif /* HWPAL_TRACE_DEVICE_READ */
}
#else //#if 0
/*-----------------------------------------------------------------------------
 * Device_Read32Array
 */
void
Device_Read32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // read starts here, +4 increments
        uint32_t * MemoryDst_p,         // writing starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    DeviceByteOffset = Device_p->StartByteOffset + Offset;

    {
        uint32_t Value;
        int i;

        for (i = 0; i < Count; i++)
        {
            Value = Device_Read32(Device_p, DeviceByteOffset);

            MemoryDst_p[i] = Value;
            DeviceByteOffset +=  4;
        } // for
    }
}
#endif //#if 0

#if 0
/*----------------------------------------------------------------------------
 * Device_Write32Array
 */
void
Device_Write32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // write starts here, +4 increments
        const uint32_t * MemorySrc_p,   // reading starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;

    Device_p = (HWPAL_Device_Administration_t *)Device;

#ifdef HWPAL_STRICT_ARGS_CHECK
    if (Device_p == NULL || MemorySrc_p == NULL || Count <= 0)
        return;

    if (IS_INVALID_OFFSET(Offset, Device_p))
    {
        LOG_CRIT("%s: Invalid ByteOffset 0x%x (device %s)\n",
                 __func__,
            Offset,
            Device_p->DeviceName_p);
        return;
    }
#endif // HWPAL_STRICT_ARGS_CHECK

    DeviceByteOffset = Device_p->StartByteOffset + Offset;

#ifdef HWPAL_ENABLE_HA_SIMULATION
    if (Device_p->Flags & HWPAL_FLAG_HA)
    {
        // HA simulation mode
        // disable access to PKA_MASTER_SEQ_CTRL
        return;
    }
#endif

#ifdef HWPAL_TRACE_DEVICE_WRITE
    if (Device_p->Flags & HWPAL_FLAG_WRITE)
    {
        bool fSwap = false;
#ifdef HWPAL_DEVICE_ENABLE_SWAP
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif

        HWPAL_Hexdump("Device_Write32Array",
                      Device_p->DeviceName_p,
                      DeviceByteOffset,
                      MemorySrc_p,
                      Count,
                      fSwap);
    }
#endif /* HWPAL_TRACE_DEVICE_WRITE */

    {
        unsigned int RemappedOffset;
        uint32_t Value;
        int i;

#ifdef HWPAL_DEVICE_ENABLE_SWAP
        bool fSwap = false;
        if (Device_p->Flags & HWPAL_FLAG_SWAP)
            fSwap = true;
#endif

        for (i = 0; i < Count; i++)
        {
            RemappedOffset = Device_RemapDeviceAddress(DeviceByteOffset);
            Value = MemorySrc_p[i];
#ifdef HWPAL_DEVICE_ENABLE_SWAP
            if (fSwap)
                Value = Device_SwapEndian32(Value);
#endif

#ifdef HWPAL_DEVICE_DIRECT_MEMIO
            *(uint32_t*)(HWPAL_Admin.MappedBaseAddr_p + (RemappedOffset / 4)) =
                                                       Value;
#else
            //iowrite32(Value, HWPAL_Admin.MappedBaseAddr_p +
            //                                    (RemappedOffset / 4));
#endif

            //smp_wmb();

            DeviceByteOffset += 4;
        } // for
    }
}
#else //#if 0
/*----------------------------------------------------------------------------
 * Device_Write32Array
 */
void
Device_Write32Array(
        const Device_Handle_t Device,
        const unsigned int Offset,      // write starts here, +4 increments
        const uint32_t * MemorySrc_p,   // reading starts here
        const int Count)                // number of uint32's to transfer
{
    HWPAL_Device_Administration_t * Device_p;
    unsigned int DeviceByteOffset;

    Device_p = (HWPAL_Device_Administration_t *)Device;

    DeviceByteOffset = Device_p->StartByteOffset + Offset;

    {
        uint32_t Value;
        int i;

        for (i = 0; i < Count; i++)
        {
            Value = MemorySrc_p[i];

            Device_Write32(Device_p, DeviceByteOffset, Value);

            DeviceByteOffset += 4;
        } // for
    }
}
#endif //#if 0

/* end of file device_lkm.c */
