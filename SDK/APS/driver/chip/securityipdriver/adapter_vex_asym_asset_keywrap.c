/* adapter_vex_asym_asset_keywrap.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for key wrap and unwrap.
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
 * vex_Asym_AssetKeyWrap
 */
VexStatus_t
vex_Asym_AssetKeyWrap(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    bool fCopy = false;
    uint64_t InDataBlobAddr = 0;
    uint64_t OutDataBlobAddr = 0;
    uint16_t InBlobSize = 0;
    uint16_t OutBlobSize = 0;
    uint16_t TokenID;
    uint8_t ModulusWords;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if (CommandToken_p->Service.PkAssetWrap.AdditionalInputSize > 208)
    {
        return VEX_BAD_ARGUMENT;
    }

    // Get key blob data address for token can be input or output
    TokenID = vex_DeviceGetTokenID();
    if ((CommandToken_p->Service.PkAssetWrap.Method == VEXTOKEN_PKASSET_RSA_OAEP_WRAP_STRING) ||
        (CommandToken_p->Service.PkAssetWrap.Method == VEXTOKEN_PKASSET_RSA_OAEP_WRAP_HASH))
    {

/*
        OutDataBlobAddr = BufManager_Map(
                              CommandToken_p->fFromUserSpace,
                              BUFMANAGER_BUFFERTYPE_OUT,
                              CommandToken_p->Service.PkAssetWrap.Data_p,
                              CommandToken_p->Service.PkAssetWrap.DataSize,
                              (void *)&TokenID);
        if (OutDataBlobAddr == 0)
        {
            goto error_func_exit;
        }
        OutBlobSize = BufManager_GetSize(OutDataBlobAddr);
*/
        OutDataBlobAddr = (uint64_t) CommandToken_p->Service.PkAssetWrap.Data_p;
        OutBlobSize = (uint16_t) CommandToken_p->Service.PkAssetWrap.DataSize;
    }
    else
    {
        InBlobSize = CommandToken_p->Service.PkAssetWrap.DataSize;

/*
        InDataBlobAddr = BufManager_Map(
                             CommandToken_p->fFromUserSpace,
                             BUFMANAGER_BUFFERTYPE_IN,
                             CommandToken_p->Service.PkAssetWrap.Data_p,
                             InBlobSize,
                             NULL);
        if (InDataBlobAddr == 0)
        {
            goto error_func_exit;
        }
*/
        InDataBlobAddr = (uint64_t) CommandToken_p->Service.PkAssetWrap.Data_p;
        
    }

    ModulusWords = ((CommandToken_p->Service.PkAssetWrap.ModulusSizeInBits + 31) / 32);

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetWrap.Method,
        ModulusWords, 0,
        0,
        CommandToken_p->Service.PkAssetWrap.KeyAssetId,
        0,
        CommandToken_p->Service.PkAssetWrap.AssetId,
        InDataBlobAddr, InBlobSize,
        OutDataBlobAddr, OutBlobSize);
    Eip130Token_Command_Pk_Asset_SetAdditional(
        &CommandToken,
        CommandToken_p->Service.PkAssetWrap.AdditionalInput,
        (uint8_t)CommandToken_p->Service.PkAssetWrap.AdditionalInputSize);
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
            // Copy output data if needed
            fCopy = true;
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (InDataBlobAddr != 0)
    {
        (void)BufManager_Unmap(InDataBlobAddr, false);
    }
    if (OutDataBlobAddr != 0)
    {
        int rc = BufManager_Unmap(OutDataBlobAddr, fCopy);
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


/* end of file adapter_vex_asym_asset_keywrap.c */
