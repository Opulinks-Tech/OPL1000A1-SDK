/* adapter_vex_asym_asset_genshared.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for key generation.
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
 * vex_Asym_AssetGenSharedSecret
 */
VexStatus_t
vex_Asym_AssetGenSharedSecret(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint64_t OtherInfoAddr = 0;
    size_t OtherInfoSize = 0;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if ((CommandToken_p->Service.PkAssetGenSharedSecret.OtherInfo_p != NULL) &&
        (CommandToken_p->Service.PkAssetGenSharedSecret.OtherInfoSize != 0))
    {
        // Get OtherInfo input address for token
        OtherInfoSize = CommandToken_p->Service.PkAssetGenSharedSecret.OtherInfoSize;

/*
        OtherInfoAddr = BufManager_Map(
                            CommandToken_p->fFromUserSpace,
                            BUFMANAGER_BUFFERTYPE_IN,
                            CommandToken_p->Service.PkAssetGenSharedSecret.OtherInfo_p,
                            OtherInfoSize,
                            NULL);
        if (OtherInfoAddr == 0)
        {
            goto error_func_exit;
        }
*/
        OtherInfoAddr = (uint64_t)CommandToken_p->Service.PkAssetGenSharedSecret.OtherInfo_p;
        
    }

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetGenSharedSecret.Method,
        ((CommandToken_p->Service.PkAssetGenSharedSecret.ModulusSizeInBits+31)/32),
        ((CommandToken_p->Service.PkAssetGenSharedSecret.DivisorSizeInBits+31)/32),
        CommandToken_p->Service.PkAssetGenSharedSecret.AssetIdListSize,
        CommandToken_p->Service.PkAssetGenSharedSecret.PrivKeyAssetId,
        CommandToken_p->Service.PkAssetGenSharedSecret.DomainAssetId,
        CommandToken_p->Service.PkAssetGenSharedSecret.PubKeyAssetId,
        OtherInfoAddr, OtherInfoSize,
        0, 0);

    if ((CommandToken_p->Service.PkAssetGenSharedSecret.Method == VEXTOKEN_PKASSET_DH_GEN_SHARED_SECRET_DKEYPAIR) ||
        (CommandToken_p->Service.PkAssetGenSharedSecret.Method == VEXTOKEN_PKASSET_ECDH_GEN_SHARED_SECRET_DKEYPAIR))
    {
        if ((CommandToken_p->Service.PkAssetGenSharedSecret.PubKey2AssetId == 0) ||
            (CommandToken_p->Service.PkAssetGenSharedSecret.PrivKey2AssetId == 0))
        {
            funcres = VEX_BAD_ARGUMENT;
            goto error_func_exit;
        }

        Eip130Token_Command_Pk_Asset_SetAdditional(
            &CommandToken,
            (uint8_t *)&CommandToken_p->Service.PkAssetGenSharedSecret.PrivKey2AssetId,
            sizeof(CommandToken_p->Service.PkAssetGenSharedSecret.PrivKey2AssetId));
        Eip130Token_Command_Pk_Asset_SetAdditional(
            &CommandToken,
            (uint8_t *)&CommandToken_p->Service.PkAssetGenSharedSecret.PubKey2AssetId,
            sizeof(CommandToken_p->Service.PkAssetGenSharedSecret.PubKey2AssetId));
    }

    {
        Eip130Token_Command_Pk_Asset_SetAdditional(
            &CommandToken,
            (const uint8_t *)CommandToken_p->Service.PkAssetGenSharedSecret.AssetIdList_p,
            sizeof(uint32_t) * CommandToken_p->Service.PkAssetGenSharedSecret.AssetIdListSize);
    }

    Eip130Token_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID(), false);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
    }

error_func_exit:

/*
    // Release used buffers, if needed
    if (OtherInfoAddr != 0)
    {
        (void)BufManager_Unmap(OtherInfoAddr, false);
    }
*/

    return funcres;
}


/* end of file adapter_vex_asym_asset_genshared.c */
