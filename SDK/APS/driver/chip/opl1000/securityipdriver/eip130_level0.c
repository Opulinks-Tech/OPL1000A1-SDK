#include "eip130_level0.h"

uint32_t
EIP130_RegisterReadMailboxStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_STAT);
}

void
EIP130_RegisterWriteMailboxControl(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_CTRL, Value);
}

#ifndef EIP130_REMOVE_MAILBOXRAWSTATUS
uint32_t
EIP130_RegisterReadMailboxRawStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_RAWSTAT);
}
#endif

#if !defined(EIP130_REMOVE_MAILBOXRESET) || \
    !defined(EIP130_REMOVE_MAILBOXLINKRESET) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
void
EIP130_RegisterWriteMailboxReset(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_RESET, Value);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXLINKID
uint32_t
EIP130_RegisterReadMailboxLinkId(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_LINKID);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXOUTID
uint32_t
EIP130_RegisterReadMailboxOutId(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_OUTID);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXACCESSCONTROL
uint32_t
EIP130_RegisterReadMailboxLockout(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXACCESSCONTROL
void
EIP130_RegisterWriteMailboxLockout(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT, Value);
}
#endif

uint32_t
EIP130_RegisterReadVersion(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_VERSION);
}

#if !defined(EIP130_REMOVE_MAILBOXGETOPTIONS) || \
    !defined(EIP130_REMOVE_MAILBOXACCESSVERIFY) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
uint32_t
EIP130_RegisterReadOptions(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_OPTIONS);
}
#endif

uint32_t
EIP130_RegisterReadOptions2(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_OPTIONS2);
}

uint32_t
EIP130_RegisterReadModuleStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MODULE_STATUS);
}

#if !defined(EIP130_REMOVE_MODULEFIRMWAREWRITTEN) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
void
EIP130_RegisterWriteModuleStatus(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MODULE_STATUS, Value);
}
#endif

