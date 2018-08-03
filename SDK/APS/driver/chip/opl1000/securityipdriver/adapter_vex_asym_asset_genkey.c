/* adapter_vex_asym_asset_genkey.c
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
 * vex_Asym_AssetGenKeyPair
 */
VexStatus_t
vex_Asym_AssetGenKeyPair(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    bool fCopy = false;
    uint64_t KeyBlobAddr = 0;
    size_t KeyBlobSize = 0;
    uint64_t PubKeyAddr = 0;
    size_t PubKeySize = 0;
    uint16_t TokenID;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    TokenID = vex_DeviceGetTokenID();

    if (CommandToken_p->Service.PkAssetGenKey.KekAssetId != 0)
    {
        if ((CommandToken_p->Service.PkAssetGenKey.KeyBlob_p == NULL) ||
            (CommandToken_p->Service.PkAssetGenKey.AssociatedDataSize >= (256 - 4)))
        {
            return VEX_BAD_ARGUMENT;
        }

        // Get Key output address for token
        KeyBlobSize = CommandToken_p->Service.PkAssetGenKey.KeyBlobSize;

/*
        KeyBlobAddr = BufManager_Map(
                          CommandToken_p->fFromUserSpace,
                          BUFMANAGER_BUFFERTYPE_OUT,
                          CommandToken_p->Service.PkAssetGenKey.KeyBlob_p,
                          KeyBlobSize,
                          (void *)&TokenID);
        if (KeyBlobAddr == 0)
        {
            goto error_func_exit;
        }
*/
        KeyBlobAddr = (uint64_t) CommandToken_p->Service.PkAssetGenKey.KeyBlob_p;
        
    }

    if (CommandToken_p->Service.PkAssetGenKey.PubKey_p != NULL)
    {
        // Get public key output address for token
        PubKeySize = CommandToken_p->Service.PkAssetGenKey.PubKeySize;

        /*
        PubKeyAddr = BufManager_Map(
                            CommandToken_p->fFromUserSpace,
                            BUFMANAGER_BUFFERTYPE_OUT,
                            CommandToken_p->Service.PkAssetGenKey.PubKey_p,
                            PubKeySize,
                            (void *)&TokenID);
        if (PubKeyAddr == 0)
        {
            goto error_func_exit;
        }*/
        PubKeyAddr = (uint64_t) CommandToken_p->Service.PkAssetGenKey.PubKey_p;

        
    }

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetGenKey.Method,
        ((CommandToken_p->Service.PkAssetGenKey.ModulusSizeInBits+31)/32),
        ((CommandToken_p->Service.PkAssetGenKey.DivisorSizeInBits+31)/32),
        0,
        CommandToken_p->Service.PkAssetGenKey.PrivKeyAssetId,
        CommandToken_p->Service.PkAssetGenKey.DomainAssetId,
        CommandToken_p->Service.PkAssetGenKey.PubKeyAssetId,
        KeyBlobAddr, KeyBlobSize,
        PubKeyAddr, PubKeySize);
    if (KeyBlobAddr != 0)
    {
        Eip130Token_Command_Pk_Asset_SetAdditional(
            &CommandToken,
            (uint8_t *)&CommandToken_p->Service.PkAssetGenKey.KekAssetId,
            sizeof(CommandToken_p->Service.PkAssetGenKey.KekAssetId));
        Eip130Token_Command_Pk_Asset_SetAdditional(
            &CommandToken,
            CommandToken_p->Service.PkAssetGenKey.AssociatedData,
            CommandToken_p->Service.PkAssetGenKey.AssociatedDataSize);
    }

    if ((KeyBlobAddr == 0) && (PubKeyAddr == 0))
    {
        Eip130Token_Command_SetTokenID(&CommandToken, TokenID, false);
    }
    else
    {
        Vex_Command_SetTokenID(&CommandToken, TokenID);
    }

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
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (KeyBlobAddr != 0)
    {
        int rc = BufManager_Unmap(KeyBlobAddr, fCopy);
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
    if (PubKeyAddr != 0)
    {
        int rc = BufManager_Unmap(PubKeyAddr, fCopy);
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
 * vex_Asym_AssetGenKeyPublic
 */
VexStatus_t
vex_Asym_AssetGenKeyPublic(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    bool fCopy = false;
    uint64_t PubKeyAddr = 0;
    size_t PubKeySize = 0;
    uint16_t TokenID;
    uint8_t ModulusWords;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    TokenID = vex_DeviceGetTokenID();

    if (CommandToken_p->Service.PkAssetGenKey.PubKey_p != NULL)
    {
/*
        // Get public key output address for token
        PubKeyAddr = BufManager_Map(
                            CommandToken_p->fFromUserSpace,
                            BUFMANAGER_BUFFERTYPE_OUT,
                            CommandToken_p->Service.PkAssetGenKey.PubKey_p,
                            CommandToken_p->Service.PkAssetGenKey.PubKeySize,
                            (void *)&TokenID);
        if (PubKeyAddr == 0)
        {
            return VEX_NO_MEMORY;
        }
*/
        PubKeyAddr = (uint64_t) CommandToken_p->Service.PkAssetGenKey.PubKey_p;

        PubKeySize = CommandToken_p->Service.PkAssetGenKey.PubKeySize;
    }

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetGenKey.Method,
        ((CommandToken_p->Service.PkAssetGenKey.ModulusSizeInBits+31)/32),
        ((CommandToken_p->Service.PkAssetGenKey.DivisorSizeInBits+31)/32),
        0,
        CommandToken_p->Service.PkAssetGenKey.PrivKeyAssetId,
        CommandToken_p->Service.PkAssetGenKey.DomainAssetId,
        CommandToken_p->Service.PkAssetGenKey.PubKeyAssetId,
        0, 0,
        PubKeyAddr, PubKeySize);
    if (PubKeyAddr == 0)
    {
        Eip130Token_Command_SetTokenID(&CommandToken, TokenID, false);
    }
    else
    {
        Vex_Command_SetTokenID(&CommandToken, TokenID);
    }

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
        }
    }

/*
    // Release used buffer, if needed
    if (PubKeyAddr != 0)
    {
        int rc = BufManager_Unmap(PubKeyAddr, fCopy);
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

/* end of file adapter_vex_asym_asset_genkey.c */
