/* adapter_val_claim.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
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

#include "c_adapter_val.h"              // configuration

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "api_val_claim.h"              // the API to implement
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t


/*----------------------------------------------------------------------------
 * val_Claim
 */
#ifndef VAL_REMOVE_CLAIM
ValStatus_t
val_Claim(void)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_CLAIMCONTROL;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE;

    // Exchange service request with VaultIP
    return val_ExchangeToken(&t_cmd, &t_res);
}
#endif /* !VAL_REMOVE_CLAIM */


/*----------------------------------------------------------------------------
 * val_ClaimOverrule
 */
#ifndef VAL_REMOVE_CLAIM
ValStatus_t
val_ClaimOverrule(void)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_CLAIMCONTROL;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE_OVERRULE;

    // Exchange service request with VaultIP
    return val_ExchangeToken(&t_cmd, &t_res);
}
#endif /* !VAL_REMOVE_CLAIM */


/*----------------------------------------------------------------------------
 * val_ClaimRelease
 */
#ifndef VAL_REMOVE_CLAIM
ValStatus_t
val_ClaimRelease(void)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_CLAIMCONTROL;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_CLAIM_RELEASE;

    // Exchange service request with VaultIP
    return val_ExchangeToken(&t_cmd, &t_res);
}
#endif /* !VAL_REMOVE_CLAIM */


/* end of file adapter_val_claim.c */
