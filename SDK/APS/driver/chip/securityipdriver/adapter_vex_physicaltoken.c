/* adapter_vex_physicaltoken.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file contains the physical token exchange implementation between the
 * VaultIP Exchange and VaultIP hardware.
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

// For tracing/debugging purpose, dump the command and result tokens
//#define TOKENS_VERBOSE

#include "c_adapter_vex.h"          // configuration

#include "basic_defs.h"             // uint16_t
#include "log.h"

#include "adapter_vex_internal.h"   // API implementation
#include "eip130.h"                 // EIP130_Mailbox*()
#include "adapter_sleep.h"          // Adapter_SleepMS()
#include "msg.h"


/*----------------------------------------------------------------------------
 * VexLocal_WaitForResultToken
 */
static int
VexLocal_WaitForResultToken(
        Device_Handle_t Device,
        uint8_t MailboxNumber)
{
    int SkipSleep = VEX_POLLING_SKIP_FIRST_DELAYS;
    unsigned int LoopsLeft = VEX_POLLING_MAXLOOPS;

    msg_print(LOG_HIGH_LEVEL, "[security] VexLocal_WaitForResultToken \r\n");

    // Poll for output token available with sleep
    while (LoopsLeft > 0)
    {
        if (EIP130_MailboxCanReadToken(Device, MailboxNumber))
        {
            // Output token is available!
            //LOG_INFO(VEX_LOG_PREFIX "%s: Token received\n", __func__);
            msg_print(LOG_HIGH_LEVEL, "[security] VexLocal_WaitForResultToken, Output token is available \r\n");
            return 0;
        }

        if (SkipSleep > 0)
        {
            // First few rounds are without sleep
            // this avoids sleeping unnecessarily for fast tokens
            SkipSleep--;
        }
        else
        {
            // Sleep a bit
            Adapter_SleepMS(VEX_POLLING_DELAY_MS);
            LoopsLeft--;
            SkipSleep = VEX_POLLING_SKIP_FIRST_DELAYS;
        }
    }

    // Reached the wait limit
    LOG_WARN(VEX_LOG_PREFIX "%s: TIMEOUT!\n", __func__);
    return -1;
}

/*----------------------------------------------------------------------------
 * vex_PhysicalTokenExchange
 */
VexStatus_t
vex_PhysicalTokenExchange(
        Eip130Token_Command_t * const CommandToken_p,
        Eip130Token_Result_t * const ResultToken_p)
{
    int funcres;
    int funcres2;
    Device_Handle_t Device;
    uint32_t Identity = 0;
    uint8_t MailboxNumber;

    msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange \r\n");

    // Get identity of calling process
/*
    Identity = vex_IdentityGet();
    if (Identity == 0)
    {
        return VEX_NO_IDENTITY;
    }
*/

    // Get mailbox number to use
    MailboxNumber = vex_MailboxGet(Identity);
    if (MailboxNumber == 0)
    {
        msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, MailboxNumber == 0 \r\n");
        return VEX_NO_MAILBOX;
    }

    // Set identity in token if not the Provision Random HUK token
    if ((CommandToken_p->W[0] & (MASK_8_BITS << 24)) !=
        (uint32_t)((EIP130TOKEN_OPCODE_ASSETMANAGEMENT     << 24) |
                   (EIP130TOKEN_SUBCODE_PROVISIONRANDOMHUK << 28)))
    {
        Eip130Token_Command_Identity(CommandToken_p, Identity);
    }

    // Get device reference
    Device = vex_DeviceGetHandle();
    if (Device == NULL)
    {
        msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, Device == NULL \r\n");
        return VEX_NOT_CONNECTED;
    }

    // Link the mailbox for use
    funcres = vex_DeviceLinkMailbox(MailboxNumber, Identity);

    msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, vex_DeviceLinkMailbox funcres:%d \r\n", funcres);
    
    if (funcres != 0)
    {
        if (funcres >= -2)
        {
            return VEX_INTERNAL_ERROR;
        }
        LOG_CRIT(VEX_LOG_PREFIX "%s: vex_DeviceLinkMailbox()=%d\n", __func__, funcres);
        return VEX_MAILBOX_IN_USE;
    }

#if 0
#ifdef TOKENS_VERBOSE
    LOG_CRIT(VEX_LOG_PREFIX "CommandToken (non-zero words only):\n");
    for (funcres2 = 0; funcres2 < EIP130TOKEN_COMMAND_WORDS; funcres2++)
    {
        if (CommandToken_p->W[funcres2])
        {
            LOG_CRIT("\tW%02d=0x%08X\n", funcres2, CommandToken_p->W[funcres2]);
        }
    }
#endif
#endif

    // Write the command token to the IN mailbox
    // Note: It also checks that the mailbox is empty
    funcres = EIP130_MailboxWriteAndSubmitToken(Device, MailboxNumber, CommandToken_p);

    msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, EIP130_MailboxWriteAndSubmitToken funcres:%d \r\n", funcres);

    switch (funcres)
    {
    case 0:
        break;
    case -3:
        funcres = VEX_MAILBOX_IN_USE;
        goto func_return;
    case -4:
        vex_DeviceStateIssue();     // Report device state issue detected
        funcres = VEX_POWER_STATE_ERROR;
        goto func_return;
    default:
        funcres = VEX_INTERNAL_ERROR;
        goto func_return;
    }

    // Loop to handle a possible previous timeout situation
    do
    {
        // Wait for the result token to be available
        funcres = VexLocal_WaitForResultToken(Device, MailboxNumber);
        if (funcres != 0)
        {
            msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, VexLocal_WaitForResultToken response timeout, funcres:%d \r\n", funcres);
            
            funcres = VEX_REPONSE_TIMEOUT;
            goto func_return;
        }

        // Read the result token from the OUT mailbox
        funcres = EIP130_MailboxReadToken(Device, MailboxNumber, ResultToken_p);
        if (funcres != 0)
        {
            msg_print(LOG_HIGH_LEVEL, "[security] vex_PhysicalTokenExchange, EIP130_MailboxReadToken fail, funcres:%d \r\n", funcres);
            
            funcres = VEX_INTERNAL_ERROR;
            goto func_return;
        }
    } while ((CommandToken_p->W[0] & MASK_16_BITS) != (ResultToken_p->W[0] & MASK_16_BITS));

#if 0
#ifdef TOKENS_VERBOSE
    if (ResultToken_p->W[0] & BIT_31)
    {
        // Error - only first word is relevant
        LOG_CRIT(VEX_LOG_PREFIX "Error ResultToken W00=0x%08X\n",
                 ResultToken_p->W[0]);
    }
    else
    {
        LOG_CRIT(VEX_LOG_PREFIX "ResultToken (non-zero words only):\n");
        for (funcres2 = 0; funcres2 < EIP130TOKEN_RESULT_WORDS; funcres2++)
        {
            if (ResultToken_p->W[funcres2])
            {
                LOG_CRIT("\tW%02d=0x%08X\n", funcres2, ResultToken_p->W[funcres2]);
            }
        }
    }
#endif
#endif

func_return:
    funcres2 = vex_DeviceUnlinkMailbox(MailboxNumber, Identity);
    if (funcres2 != 0)
    {
        LOG_CRIT(VEX_LOG_PREFIX "%s: vex_DeviceUnlinkMailbox()=%d\n", __func__, funcres2);
        if (funcres == 0)
        {
            funcres = VEX_INTERNAL_ERROR;
        }
    }

    return funcres;
}


/* end of file adapter_vex_physicaltoken.c */
