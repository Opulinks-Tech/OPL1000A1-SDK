/* adapter_vex_device.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the device related functionality.
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

#include "c_adapter_vex.h"          // configuration

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "adapter_vex_internal.h"   // API implementation
#include "adapter_lock.h"           // Adapter_Lock_t, Adapter_Lock*()
#ifdef VEX_ENABLE_FIRMWARE_LOAD
#include "adapter_firmware.h"       // Adapter_Firmware_*
#endif
#include "device_mgmt.h"            // Device_Find()
#include "eip130.h"                 // EIP130_Mailbox*()
#include "eip130_token_system.h"    // Eip130Token_Command_System*()
#include "msg.h"

//#define VEX_DEVICE_MAX_MAILBOXES    8
#define VEX_DEVICE_MAX_MAILBOXES    4


static Device_Handle_t gl_VexDevice = NULL;
static int gl_VexDeviceRefCnt = 0;              // vex_DeviceInit call reference counter
static int gl_VexDevicePWRState = VEX_DEVICE_POWER_UNKNOWN;
static uint8_t gl_VexDeviceMyHostID = 0;        // My Host ID
static uint8_t gl_VexDeviceMyProt = 0;          // My Protection bit setting
static uint8_t gl_VexDeviceMasterID = 0;        // Host ID of master
static uint8_t gl_VexDeviceProtAvailable = 0;   // Protection functionality available

//static uint8_t gl_VexDeviceNrOfMailboxes = 0;   // Number of mailboxes
static uint8_t gl_VexDeviceNrOfMailboxes = 4;   // Number of mailboxes

static uint16_t gl_VexDeviceMailboxSize = 256;  // Mailbox size (256 bytes)
static uint16_t gl_VexDeviceTokenID = 0;        // TokenID reference counter

// Mailbox linking state
// 0x0000   = Not Linked
// 0xFFFF   = Linked for the sequence ->
//                link - one operation - unlink
// identity = Linked by a calling process for the sequence ->
//                link - multiple operations - unlink
static uint32_t gl_VexDeviceMailboxLinked[VEX_DEVICE_MAX_MAILBOXES];



int
vexLocal_DeviceInit(
        int FirmwareCheckInfo)
{
    int i;

#if 0
#ifdef VEX_ENABLE_FIRMWARE_LOAD
    if (FirmwareCheckInfo == 0)
    {
        Adapter_Firmware_t FirmwareHandle;
        const uint32_t * Firmware_p;
        unsigned int Firmware_Word32Count;

        // Get the firmware image
        FirmwareHandle = Adapter_Firmware_Acquire(VEX_FIRMWARE_FILE,
                                                  &Firmware_p,
                                                  &Firmware_Word32Count);
        if (FirmwareHandle == NULL)
        {
            return -3;
        }

        // Load the firmware image
        i = EIP130_FirmwareLoad(gl_VexDevice, VEX_MAILBOX_NR,
                                 Firmware_p, Firmware_Word32Count);
        Adapter_Firmware_Release(FirmwareHandle);
        if (i < 0)
        {
            return -4;
        }
        FirmwareCheckInfo = 2;          // Indicate firmware image active
    }
#endif
    if (FirmwareCheckInfo != 2)
    {
        gl_VexDevicePWRState = VEX_DEVICE_POWER_UNKNOWN;
        return -5;
    }
#endif

    gl_VexDevicePWRState = VEX_DEVICE_POWER_ACTIVE;

    // Get exclusive access to the requested mailbox
    if (EIP130_MailboxLink(gl_VexDevice, VEX_MAILBOX_NR) != 0)
    {	
        msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, mailbox link fail \r\n");
        return -6;
    }

    //We'd like to link all mailbox here
    EIP130_MailboxLink(gl_VexDevice, 1);
    EIP130_MailboxLink(gl_VexDevice, 2);
    EIP130_MailboxLink(gl_VexDevice, 3);
    EIP130_MailboxLink(gl_VexDevice, 4);

    msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, mailbox link success \r\n");

    // Check if OUT mailbox is unexpectedly FULL?
    // -> Remove result tokens
    i = 0;
    while (EIP130_MailboxCanReadToken(gl_VexDevice, VEX_MAILBOX_NR))
    {
        Eip130Token_Result_t ResultToken;

        if (EIP130_MailboxReadToken(gl_VexDevice, VEX_MAILBOX_NR, &ResultToken) != 0)
        {
            msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, read token fail \r\n");
            return -7;
        }
        
        msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, read token success \r\n");

        i++;
        if (i > 3)
        {
            return -7;
        }
    }

    // Check if IN mailbox is unexpectedly FULL?
    if (!EIP130_MailboxCanWriteToken(gl_VexDevice, VEX_MAILBOX_NR))
    {
        msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, write token fail \r\n");
        return -8;
    }

    msg_print(LOG_HIGH_LEVEL, "[security] vexLocal_DeviceInit, write token success \r\n");

#ifndef VEX_MAILBOX_LINK_ONLY_ONCE
    // Release exclusive access to the requested mailbox
    if (EIP130_MailboxUnlink(gl_VexDevice, VEX_MAILBOX_NR) != 0)
    {
        return -9;
    }
#endif

    return 0;
}

/*----------------------------------------------------------------------------
 * vex_DeviceInit
 */
int
vex_DeviceInit(void)
{
    int rc = 0;

/*
    if(vex_LockAcquire() < 0)
    {
        return -100;
    }
*/

    if (gl_VexDevice == NULL)
    {
        int i;

        // Get the VaultIP (EIP130) device reference
        gl_VexDevice = Device_Find(VEX_DEVICE_NAME);
        if (gl_VexDevice == NULL)
        {
            rc = -1;

            //msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceInit, gl_VexDevice == NULL \r\n");
            
            goto func_return;
        }
        gl_VexDeviceRefCnt++;
        gl_VexDevicePWRState = VEX_DEVICE_POWER_UNKNOWN;

        // Reset mailbox linked indications
        for (i = 0; i < VEX_DEVICE_MAX_MAILBOXES; i++)
        {
            gl_VexDeviceMailboxLinked[i] = 0;
        }

        // Check device and firmware state
        rc = EIP130_FirmwareCheck(gl_VexDevice);
        if (rc == -1)
        {
            //msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceInit, EIP130_FirmwareCheck return fail, rc == -1 \r\n");
            rc = -2;
            goto func_return;
        }

        // Get mailbox related information and check given mailbox
        EIP130_MailboxGetOptions(gl_VexDevice,
                                 &gl_VexDeviceMyHostID,
                                 &gl_VexDeviceMasterID,
                                 &gl_VexDeviceMyProt,
                                 &gl_VexDeviceProtAvailable,
                                 &gl_VexDeviceNrOfMailboxes,
                                 &gl_VexDeviceMailboxSize);

        //msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceInit, gl_VexDeviceNrOfMailboxes:%d \r\n", gl_VexDeviceNrOfMailboxes);
        //msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceInit, do vexLocal_DeviceInit() \r\n");

        
        if (VEX_MAILBOX_NR < gl_VexDeviceNrOfMailboxes)
        {
            rc = vexLocal_DeviceInit(rc);
        }
        else
        {
            rc = -2;
        }


        //msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceInit, rc:%d \r\n", rc);
    }
    else
    {
        gl_VexDeviceRefCnt++;
    }

func_return:
    
    //vex_LockRelease();
    return rc;
}



/*----------------------------------------------------------------------------
 * vex_DeviceExit
 */
void
vex_DeviceExit(void)
{
    //if (vex_LockAcquire() == 0)
    //{
        if (gl_VexDevice != NULL)
        {
            gl_VexDeviceRefCnt--;
            if (gl_VexDeviceRefCnt <= 0)
            {
                uint8_t i;

                for (i = 0; i < VEX_DEVICE_MAX_MAILBOXES; i++)
                {
                    if (gl_VexDeviceMailboxLinked[i])
                    {
                        (void)EIP130_MailboxUnlink(gl_VexDevice, i+1);
                        gl_VexDeviceMailboxLinked[i] = 0;
                    }
                }

                gl_VexDevice = NULL;
                gl_VexDeviceRefCnt = 0;
                gl_VexDeviceMyProt = 0; // Protection bit setting reset
            }
        }

    //    vex_LockRelease();
    //}
    //else if (gl_VexDevice != NULL)
    //{
    //    LOG_CRIT(VEX_LOG_PREFIX "FAILED to un-initialize device");
    //}
}


/*----------------------------------------------------------------------------
 * vex_DeviceGetHandle
 */
Device_Handle_t
vex_DeviceGetHandle(void)
{
    if ((gl_VexDevice != NULL) &&
        (gl_VexDevicePWRState == VEX_DEVICE_POWER_UNKNOWN))
    {
        // Check device and firmware state
        int rc = EIP130_FirmwareCheck(gl_VexDevice);

        msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceGetHandle, rc:%d \r\n", rc);
        
        switch (rc)
        {
        case 2:
            break;

        case 0:
            msg_print(LOG_HIGH_LEVEL, "[security] vex_DeviceGetHandle, firmware state is ok \r\n");
            
            if (vexLocal_DeviceInit(rc) == 0)
            {
                break;
            }
            // NO BREAK

        default:
            // Possible return situations:
            // - No device connection
            // - Device initialization failed
            // - Device initialization busy (other host)
            // - Host not allowed to do the image load
            return NULL;
        }
    }
    return gl_VexDevice;
}


/*----------------------------------------------------------------------------
 * vex_DeviceIsConnected
 */
bool
vex_DeviceIsConnected(void)
{
    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return false;
    }
    return true;
}


/*----------------------------------------------------------------------------
 * vex_DeviceIsSecureConnected
 */
bool
vex_DeviceIsSecureConnected(void)
{
    // Device connection available and protection bit set
    if (gl_VexDeviceMyProt)
    {
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------------
 * vex_DeviceStateIssue
 */
void
vex_DeviceStateIssue(void)
{
    gl_VexDevicePWRState = VEX_DEVICE_POWER_UNKNOWN;
}


/*----------------------------------------------------------------------------
 * vex_DeviceLinkMailbox
 */
int
vex_DeviceLinkMailbox(
           const uint8_t MailboxNr,
           uint32_t Identity)
{
    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return -1;
    }

    // Valid mailbox
    if ((MailboxNr == 0) || (MailboxNr > gl_VexDeviceNrOfMailboxes))
    {
        return -2;
    }

    if (gl_VexDeviceMailboxLinked[MailboxNr - 1] == 0)
    {
#ifndef VEX_MAILBOX_LINK_ONLY_ONCE
        // Get exclusive access to the requested mailbox
        if (EIP130_MailboxLink(gl_VexDevice, MailboxNr) != 0)
        {
            // Mailbox could be locked by another host
            return -3;
        }
#endif

        gl_VexDeviceMailboxLinked[MailboxNr - 1] = Identity;
    }
    else if (gl_VexDeviceMailboxLinked[MailboxNr - 1] != Identity)
    {
        // Mailbox lock for another process
        return -4;
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * vex_DeviceLinkMailboxOverrule
 */
int
vex_DeviceLinkMailboxOverrule(
           const uint8_t MailboxNr,
           uint32_t Identity)
{
    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return -1;
    }

    // Valid mailbox
    if ((MailboxNr == 0) || (MailboxNr > gl_VexDeviceNrOfMailboxes))
    {
        return -2;
    }

#ifndef VEX_MAILBOX_LINK_ONLY_ONCE
    // Get exclusive access to the requested mailbox
    if (EIP130_MailboxLink(gl_VexDevice, MailboxNr) != 0)
    {
        // Mailbox could be locked by another host
        return -3;
    }
#endif
    gl_VexDeviceMailboxLinked[MailboxNr - 1] = Identity;

    return 0;
}

/*----------------------------------------------------------------------------
 * vex_DeviceUnlinkMailbox
 */
int
vex_DeviceUnlinkMailbox(
           const uint8_t MailboxNr,
           uint32_t Identity)
{
    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return -1;
    }

    // Valid mailbox
    if ((MailboxNr == 0) || (MailboxNr > gl_VexDeviceNrOfMailboxes))
    {
        return -2;
    }

    if (gl_VexDeviceMailboxLinked[MailboxNr - 1] == Identity)
    {
        gl_VexDeviceMailboxLinked[MailboxNr - 1] = 0;

#ifndef VEX_MAILBOX_LINK_ONLY_ONCE
        // Release exclusive access to the requested mailbox
        if (EIP130_MailboxUnlink(gl_VexDevice, MailboxNr) != 0)
        {
            // Probably forced link by another host
            return -3;
        }
#endif
    }
    else if (gl_VexDeviceMailboxLinked[MailboxNr - 1] != 0)
    {
        // Not allowed to unlink the mailbox that is locked by another process
        return -4;
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * vex_DeviceGetTokenID
 */
uint16_t
vex_DeviceGetTokenID(void)
{
    // Generate a TokenID
    gl_VexDeviceTokenID++;
    if (gl_VexDeviceTokenID == 0)
    {
        gl_VexDeviceTokenID++;
    }
    return gl_VexDeviceTokenID;
}

/*----------------------------------------------------------------------------
 * vex_DeviceSleep
 */
#ifdef VEX_ENABLE_FIRMWARE_SLEEP
int
vex_DeviceSleep(void)
{
    int rc;

    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return VEX_NOT_CONNECTED;
    }

    if (gl_VexDevicePWRState == VEX_DEVICE_POWER_ACTIVE)
    {
        Eip130Token_Command_t CommandResult;
        int nRetries;
        uint32_t Identity;
        uint16_t TokenId;
        uint8_t MailboxNumber;

        // Check if firmware needs to be loaded
        rc = EIP130_FirmwareCheck(gl_VexDevice);
        switch (rc)
        {
        case 2:
            break;
        case -1:
            vex_DeviceStateIssue();
            return VEX_NOT_CONNECTED;
        case -2:
            return VEX_OPERATION_NOT_ALLOWED;
        default:
            // The firmware is not in accepted state
            vex_DeviceStateIssue();
            return VEX_POWER_STATE_ERROR;
        }

        // Get and set identity of calling process
        Identity = vex_IdentityGet();
        if (Identity == 0)
        {
            return VEX_NO_IDENTITY;
        }

        // Get mailbox number to use
        MailboxNumber = vex_MailboxGet(Identity);
        if (MailboxNumber == 0)
        {
            return VEX_NO_MAILBOX;
        }

        // Make sure the mailbox is linked for use
        for (nRetries = 0; ; nRetries++)
        {
            rc = EIP130_MailboxLink(gl_VexDevice, MailboxNumber);
            if (rc == 0)
            {
                break;
            }
            if (nRetries)
            {
                return VEX_MAILBOX_IN_USE;
            }

            // Reset link assume that mailbox is in use by another Host.
            rc = EIP130_MailboxLinkReset(gl_VexDevice, MailboxNumber);
            if (rc < 0)
            {
                return VEX_MAILBOX_IN_USE; // Link reset error
            }
        }
        gl_VexDeviceMailboxLinked[MailboxNumber - 1] = Identity;

        // Check if mailbox is ready for the token
        if (!EIP130_MailboxCanWriteToken(gl_VexDevice, MailboxNumber))
        {
            rc = VEX_MAILBOX_IN_USE;    // General error
            goto func_error;            // Unlink mailbox and return error
        }

        // Write Sleep token
        Eip130Token_Command_SystemSleep(&CommandResult);
        Eip130Token_Command_Identity(&CommandResult, Identity);
        TokenId = vex_DeviceGetTokenID();
        CommandResult.W[0] |= TokenId;
        rc = EIP130_MailboxWriteAndSubmitToken(gl_VexDevice, MailboxNumber, &CommandResult);
        switch (rc)
        {
        case 0:
            break;
        case -3:
            rc = VEX_MAILBOX_IN_USE;
            goto func_error;            // Unlink mailbox and return error
        case -4:
            vex_DeviceStateIssue();     // Report device state issue detected
            rc = VEX_POWER_STATE_ERROR;
            goto func_error;            // Unlink mailbox and return error
        default:
            rc = VEX_INTERNAL_ERROR;
            goto func_error;            // Unlink mailbox and return error
        }

        // Wait and read output token
        do
        {
            for (nRetries = 0x00FFFFFF; nRetries; nRetries--)
            {
                if (EIP130_MailboxCanReadToken(gl_VexDevice, MailboxNumber))
                {
                    break;
                }
            }

            rc = EIP130_MailboxReadToken(gl_VexDevice, MailboxNumber,
                                         (Eip130Token_Result_t *)&CommandResult);
            if (rc < 0)
            {
                rc = VEX_INTERNAL_ERROR;
                goto func_error;        // Unlink mailbox and return error
            }
        } while ((CommandResult.W[0] & MASK_16_BITS) != TokenId);

        rc = Eip130Token_Result_Code((Eip130Token_Result_t *)&CommandResult);
        if (rc < 0)
        {
            rc -= 256;
        }
        else
        {
            gl_VexDevicePWRState = VEX_DEVICE_POWER_SLEEP;
        }

func_error:
        // Unlink the mailbox
        (void)vex_DeviceUnlinkMailbox(MailboxNumber, Identity);
    }
    else
    {
        rc = VEX_POWER_STATE_ERROR;     // Invalid state
    }

    return rc;
}
#endif

/*----------------------------------------------------------------------------
 * vex_DeviceResumeFromSleep
 */
#ifdef VEX_ENABLE_FIRMWARE_SLEEP
int
vex_DeviceResumeFromSleep(void)
{
    Eip130Token_Command_t CommandResult;
    int rc;
    int nRetries;
    uint32_t Identity;
    uint16_t TokenId;
    uint8_t MailboxNumber;
    uint8_t fFWRam;

    // Device connection available
    if (gl_VexDevice == NULL)
    {
        return -1;
    }

    if (gl_VexDeviceMyHostID != gl_VexDeviceMasterID)
    {
        // This Host is NOT allowed to do the Sleep operation
        return -2;
    }

    // Check if firmware needs to be loaded
    rc = EIP130_FirmwareCheck(gl_VexDevice);
    if (rc < 0)
    {
        return rc;                      // General error
    }
    if (rc > 0)
    {
        // The firmware is already loaded and the firmware checks are busy
        // or the firmware is accepted
        return 0;
    }

    // Get and set identity of calling process
    Identity = vex_IdentityGet();
    if (Identity == 0)
    {
        return VEX_NO_IDENTITY;
    }

    // Get mailbox number to use
    MailboxNumber = vex_MailboxGet(Identity);
    if (MailboxNumber == 0)
    {
        return VEX_NO_MAILBOX;
    }

    // Link mailbox
    // Note: VaultIP is expected to come out of sleep state, so
    //       mailbox linking should be possible!
    for (nRetries = 0; ; nRetries++)
    {
        rc = EIP130_MailboxLink(gl_VexDevice, MailboxNumber);
        if (rc == 0)
        {
            break;
        }
        if (nRetries)
        {
            return rc;                  // General error
        }

        // Reset link assume that mailbox is in use by another Host
        rc = EIP130_MailboxLinkReset(gl_VexDevice, MailboxNumber);
        if (rc < 0)
        {
            return rc;                  // Link reset error
        }
    }

    // Check if mailbox is ready for the token
    // Note: VaultIP is expected to come out of HW reset state, so
    //       direct mailbox use should be possible!
    if (!EIP130_MailboxCanWriteToken(gl_VexDevice, MailboxNumber))
    {
        rc = -1;                        // General error
        goto func_error;                // Unlink mailbox and return error
    }

    // Write Resume From Sleep token
    Eip130Token_Command_SystemResumeFromSleep(&CommandResult);
    TokenId = vex_DeviceGetTokenID();
    CommandResult.W[0] |= TokenId;
    rc = EIP130_MailboxWriteAndSubmitToken(gl_VexDevice, MailboxNumber,
                                           &CommandResult);
    if (rc < 0)
    {
        goto func_error;                // Unlink mailbox and return error
    }

    // Check firmware configuration
    EIP130_ModuleGetOptions(gl_VexDevice, &fFWRam, NULL, NULL, NULL);
    if (fFWRam)
    {
        uint8_t fFirmwareWritten;
        uint8_t fFirmwareChecksDone;
        uint8_t fFirmwareAccepted;

        // Report that the firmware is written
        EIP130_ModuleFirmwareWritten(gl_VexDevice);
        EIP130_ModuleGetStatus(gl_VexDevice, NULL, NULL, NULL, NULL, NULL,
                               &fFirmwareWritten,
                               &fFirmwareChecksDone,
                               &fFirmwareAccepted);
        if (fFirmwareWritten == 0)
        {
            goto func_error;            // Unlink mailbox and return error
        }

        // Check if firmware check is started
        for (nRetries = 0x0000FFFF; nRetries && (fFirmwareChecksDone != 0); nRetries--)
        {
            EIP130_ModuleGetStatus(gl_VexDevice, NULL, NULL, NULL, NULL, NULL, NULL,
                                   &fFirmwareChecksDone,
                                   &fFirmwareAccepted);
        }
        if (fFirmwareChecksDone != 0)
        {
            rc = -3;                    // Timeout error (VaultIP active?)
            goto func_error;            // Unlink mailbox and return error
        }

        // Check if firmware is accepted
        for (nRetries = 0x000FFFFF; nRetries && (fFirmwareChecksDone == 0); nRetries--)
        {
            EIP130_ModuleGetStatus(gl_VexDevice, NULL, NULL, NULL, NULL, NULL, NULL,
                                   &fFirmwareChecksDone,
                                   &fFirmwareAccepted);
        }
        if (fFirmwareChecksDone == 0)
        {
            rc = -3;                    // Timeout error
            goto func_error;            // Unlink mailbox and return error
        }
        if (fFirmwareAccepted == 0)
        {
            rc = -4;                    // Firmware is not accepted (not loaded)
            goto func_error;            // Unlink mailbox and return error
        }
    }

    // Wait and read output token
    do
    {
        for (nRetries = 0x000FFFFF; nRetries ; nRetries--)
        {
            if (EIP130_MailboxCanReadToken(gl_VexDevice, MailboxNumber))
            {
                break;
            }
        }

        rc = EIP130_MailboxReadToken(gl_VexDevice, MailboxNumber,
                                     (Eip130Token_Result_t *)&CommandResult);
        if (rc < 0)
        {
            goto func_error;            // Unlink mailbox and return error
        }
    } while ((CommandResult.W[0] & MASK_16_BITS) != TokenId);

    rc = Eip130Token_Result_Code((Eip130Token_Result_t *)&CommandResult);
    if (rc < 0)
    {
        rc -= 256;
    }
    else
    {
        gl_VexDevicePWRState = VEX_DEVICE_POWER_ACTIVE;
    }

func_error:
    // Unlink mailbox 1
    (void)EIP130_MailboxUnlink(gl_VexDevice, MailboxNumber);

    return rc;
}
#endif


/* end of file adapter_vex_device.c */
