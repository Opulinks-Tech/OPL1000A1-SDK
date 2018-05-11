/* adapter_vex_claim.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the services with which the exclusive mailbox locking
 * can be controlled.
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

#include "adapter_vex_internal.h"   // API implementation

#if 0
/*----------------------------------------------------------------------------
 * vex_Claim
 */
VexStatus_t
vex_Claim(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    uint32_t Identity;
    uint8_t MailboxNumber;
    int funcres;

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

    switch (CommandToken_p->SubCode)
    {
    case VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE:
        funcres = vex_DeviceLinkMailbox(MailboxNumber, Identity);
        if (funcres != 0)
        {
            if (funcres == -5)
            {
                return VEX_MAILBOX_IN_USE;
            }
            return VEX_INTERNAL_ERROR;
        }
        break;

    case VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE_OVERRULE:
        funcres = vex_DeviceLinkMailboxOverrule(MailboxNumber, Identity);
        if (funcres != 0)
        {
            return VEX_INTERNAL_ERROR;
        }
        break;

    case VEXTOKEN_SUBCODE_CLAIM_RELEASE:
        funcres = vex_DeviceUnlinkMailbox(MailboxNumber, Identity);
        if (funcres != 0)
        {
            return VEX_INTERNAL_ERROR;
        }
        break;

    default:
        return VEX_UNSUPPORTED;
    }

    ResultToken_p->Result = 0;
    return VEX_SUCCESS;
}
#endif

/* end of file adapter_vex_claim.c */
