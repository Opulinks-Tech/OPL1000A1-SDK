/* adapter_vex_service.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the service services.
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
#include "eip130_token_service.h"   // Eip130Token_Command_RegisterRead/Write()
                                    // Eip130Token_Command_ZeroizeOutputMailbox()
#include "eip130_token_otp.h"       // Eip130Token_Command_OTP(Select)Zeroize()


/*----------------------------------------------------------------------------
 * vex_Service
 */
VexStatus_t
vex_Service(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Format command token
    ZEROINIT(CommandToken);
    switch (CommandToken_p->SubCode)
    {
    case VEXTOKEN_SUBCODE_REGISTERREAD:
        Eip130Token_Command_RegisterRead(
            &CommandToken,
            CommandToken_p->Service.Register.Address);
        break;

    case VEXTOKEN_SUBCODE_REGISTERWRITE:
        Eip130Token_Command_RegisterWrite(
            &CommandToken,
            CommandToken_p->Service.Register.Address,
            CommandToken_p->Service.Register.Value);
        break;

    case VEXTOKEN_SUBCODE_CLOCKSWITCH:
        Eip130Token_Command_ClockSwitch(
            &CommandToken,
            CommandToken_p->Service.ClockSwitch.On,
            CommandToken_p->Service.ClockSwitch.Off);
        break;

    case VEXTOKEN_SUBCODE_ZEROOUTMAILBOX:
        Eip130Token_Command_ZeroizeOutputMailbox(&CommandToken);
        break;

    case VEXTOKEN_SUBCODE_SELECTOTPZERO:
        Eip130Token_Command_OTPSelectZeroize(&CommandToken);
        break;

    case VEXTOKEN_SUBCODE_ZEROIZEOTP:
        Eip130Token_Command_OTPZeroize(&CommandToken);
        break;

    default:
        return VEX_UNSUPPORTED;
    }
    Eip130Token_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID(), false);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if ((ResultToken_p->Result >= 0) &&
            (CommandToken_p->SubCode == VEXTOKEN_SUBCODE_REGISTERREAD))
        {
            // Success
            ResultToken_p->Service.Register.Value = ResultToken.W[1];
        }
    }

    return funcres;
}


/* end of file adapter_vex_service.c */
