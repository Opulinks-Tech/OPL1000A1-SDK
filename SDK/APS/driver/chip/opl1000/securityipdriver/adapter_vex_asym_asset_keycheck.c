/* adapter_vex_asym_asset_keycheck.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for key checking.
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

#include "c_adapter_vex.h"              // configuration

#include "basic_defs.h"
#include "clib.h"

#include "adapter_vex_intern_asym.h"   // API implementation
#include "adapter_bufmanager.h"        // BufManager_*()
#include "eip130_token_pk.h"           // Eip130Token_Command_Pk_Asset*()


/*----------------------------------------------------------------------------
 * vex_Asym_AssetKeyCheck
 */
VexStatus_t
vex_Asym_AssetKeyCheck(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetKeyCheck.Method,
        ((CommandToken_p->Service.PkAssetKeyCheck.ModulusSizeInBits+31)/32),
        ((CommandToken_p->Service.PkAssetKeyCheck.DivisorSizeInBits+31)/32),
        0,
        CommandToken_p->Service.PkAssetKeyCheck.PubKeyAssetId,
        CommandToken_p->Service.PkAssetKeyCheck.DomainAssetId,
        CommandToken_p->Service.PkAssetKeyCheck.PrivKeyAssetId,
        0, 0,
        0, 0);
    Eip130Token_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID(), false);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
    }

    return funcres;
}


/* end of file adapter_vex_asym_asset_keycheck.c */
