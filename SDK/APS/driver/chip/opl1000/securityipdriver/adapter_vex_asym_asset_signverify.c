/* adapter_vex_asym_asset_signverify.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for Sign/Verify.
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
#include "eip130_token_asset.h"        // Eip130Token_Result_AssetCreate()
                                       // for StateAssetId

/*----------------------------------------------------------------------------
 * vex_Asym_AssetSign
 */
VexStatus_t
vex_Asym_AssetSign(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    bool fCopy = false;
    uint64_t HashDataAddr = 0;
    uint64_t SignAddr = 0;
    uint16_t TokenID;
    uint8_t ModulusWords;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if ((CommandToken_p->Service.PkAssetSignVerify.HashData_p == NULL) ||
        ((CommandToken_p->Service.PkAssetSignVerify.Sign_p == NULL) &&
         (CommandToken_p->Service.PkAssetSignVerify.Method != VEXTOKEN_PKASSET_EDDSA_SIGN_INITIAL) &&
         (CommandToken_p->Service.PkAssetSignVerify.Method != VEXTOKEN_PKASSET_EDDSA_SIGN_UPDATE)))
    {
        return VEX_BAD_ARGUMENT;
    }

/*
    // Get hash data address for token
    HashDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                  BUFMANAGER_BUFFERTYPE_IN,
                                  CommandToken_p->Service.PkAssetSignVerify.HashData_p,
                                  CommandToken_p->Service.PkAssetSignVerify.HashDataSize,
                                  NULL);
    if (HashDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
    HashDataAddr = (uint64_t) CommandToken_p->Service.PkAssetSignVerify.HashData_p;

    // Get signature output address for token
    TokenID = vex_DeviceGetTokenID();

/*
    if (CommandToken_p->Service.PkAssetSignVerify.Sign_p != NULL)
    {
        SignAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                  BUFMANAGER_BUFFERTYPE_OUT,
                                  CommandToken_p->Service.PkAssetSignVerify.Sign_p,
                                  CommandToken_p->Service.PkAssetSignVerify.SignSize,
                                  (void *)&TokenID);
        if (SignAddr == 0)
        {
            goto error_func_exit;
        }
    }
*/
    SignAddr = (uint64_t) CommandToken_p->Service.PkAssetSignVerify.Sign_p;


    ModulusWords = ((CommandToken_p->Service.PkAssetSignVerify.ModulusSizeInBits+31)/32);

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetSignVerify.Method,
        ModulusWords, ModulusWords,
        CommandToken_p->Service.PkAssetSignVerify.SaltSize,
        CommandToken_p->Service.PkAssetSignVerify.KeyAssetId,
        CommandToken_p->Service.PkAssetSignVerify.DomainAssetId,
        CommandToken_p->Service.PkAssetSignVerify.DigestAssetId,
        HashDataAddr, CommandToken_p->Service.PkAssetSignVerify.HashDataSize,
        SignAddr, CommandToken_p->Service.PkAssetSignVerify.SignSize);
    Eip130Token_Command_Pk_Asset_SetAdditional(
        &CommandToken,
        (uint8_t *)&CommandToken_p->Service.PkAssetSignVerify.TotalMessageSize,
        sizeof(CommandToken_p->Service.PkAssetSignVerify.TotalMessageSize));
    Vex_Command_SetTokenID(&CommandToken, TokenID);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if (ResultToken_p->Result >= 0)
        {
            // Copy output data
            fCopy = true;

            if (CommandToken_p->Service.PkAssetSignVerify.Method == VEXTOKEN_PKASSET_EDDSA_SIGN_INITIAL)
            {
                // Get StateAssetId
                Eip130Token_Result_AssetCreate(
                    &ResultToken,
                    &ResultToken_p->Service.PkAssetSignVerify.StateAssetId);
            }
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (HashDataAddr != 0)
    {
        (void)BufManager_Unmap(HashDataAddr, false);
    }
    if (SignAddr != 0)
    {
        int rc = BufManager_Unmap(SignAddr, fCopy);
        if (rc != 0)
        {
            if (rc == -3)
            {
                funcres = VEX_DATA_TIMEOUT;
            }
            else
            {
                funcres = VEX_INTERNAL_ERROR;
            }
        }
    }
*/
    return funcres;
}


/*----------------------------------------------------------------------------
 * vex_Asym_AssetVerify
 */
VexStatus_t
vex_Asym_AssetVerify(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint64_t HashDataAddr = 0;
    uint64_t SignAddr = 0;
    uint8_t ModulusWords;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if ((CommandToken_p->Service.PkAssetSignVerify.HashData_p == NULL) ||
        ((CommandToken_p->Service.PkAssetSignVerify.Sign_p == NULL) &&
         (CommandToken_p->Service.PkAssetSignVerify.Method != VEXTOKEN_PKASSET_EDDSA_VERIFY_FINAL)))
    {
        return VEX_BAD_ARGUMENT;
    }

/*
    // Get hash data address for token
    HashDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                  BUFMANAGER_BUFFERTYPE_IN,
                                  CommandToken_p->Service.PkAssetSignVerify.HashData_p,
                                  CommandToken_p->Service.PkAssetSignVerify.HashDataSize,
                                  NULL);
    if (HashDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
    HashDataAddr = (uint64_t) CommandToken_p->Service.PkAssetSignVerify.HashData_p;

    // Get signature output address for token
    if (CommandToken_p->Service.PkAssetSignVerify.Sign_p != NULL)
    {

/*
        SignAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                  BUFMANAGER_BUFFERTYPE_IN,
                                  CommandToken_p->Service.PkAssetSignVerify.Sign_p,
                                  CommandToken_p->Service.PkAssetSignVerify.SignSize,
                                  NULL);
        if (SignAddr == 0)
        {
            goto error_func_exit;
        }
*/
        SignAddr = (uint64_t) CommandToken_p->Service.PkAssetSignVerify.Sign_p;

    }

    ModulusWords = ((CommandToken_p->Service.PkAssetSignVerify.ModulusSizeInBits+31)/32);

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetSignVerify.Method,
        ModulusWords, ModulusWords,
        CommandToken_p->Service.PkAssetSignVerify.SaltSize,
        CommandToken_p->Service.PkAssetSignVerify.KeyAssetId,
        CommandToken_p->Service.PkAssetSignVerify.DomainAssetId,
        CommandToken_p->Service.PkAssetSignVerify.DigestAssetId,
        HashDataAddr, CommandToken_p->Service.PkAssetSignVerify.HashDataSize,
        SignAddr, CommandToken_p->Service.PkAssetSignVerify.SignSize);
    Eip130Token_Command_Pk_Asset_SetAdditional(
        &CommandToken,
        (uint8_t *)&CommandToken_p->Service.PkAssetSignVerify.TotalMessageSize,
        sizeof(CommandToken_p->Service.PkAssetSignVerify.TotalMessageSize));
    Eip130Token_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID(), false);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if ((ResultToken_p->Result >= 0) &&
            (CommandToken_p->Service.PkAssetSignVerify.Method == VEXTOKEN_PKASSET_EDDSA_VERIFY_INITIAL))
        {
            // Get StateAssetId
            Eip130Token_Result_AssetCreate(
                &ResultToken,
                &ResultToken_p->Service.PkAssetSignVerify.StateAssetId);
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (HashDataAddr != 0)
    {
        (void)BufManager_Unmap(HashDataAddr, false);
    }
    if (SignAddr != 0)
    {
        (void)BufManager_Unmap(SignAddr, false);
    }
*/
    return funcres;
}


/* end of file adapter_vex_asym_asset_signverify.c */
