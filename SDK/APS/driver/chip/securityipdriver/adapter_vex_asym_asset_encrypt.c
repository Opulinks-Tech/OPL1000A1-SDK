/* adapter_vex_asym_asset_encrypt.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for encrypt and decrypt.
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
 * vex_Asym_AssetEncrypt
 */
VexStatus_t
vex_Asym_AssetEncrypt(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    bool fCopy = false;
    uint64_t SrcDataAddr = 0;
    uint64_t DstDataAddr = 0;
    uint16_t TokenID;
    uint8_t ModulusWords;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if ((CommandToken_p->Service.PkAssetEncrypt.SrcData_p == NULL) ||
        (CommandToken_p->Service.PkAssetEncrypt.DstData_p == NULL))
    {
        return VEX_BAD_ARGUMENT;
    }

/*
    // Get input data address for token
    SrcDataAddr = BufManager_Map(
                      CommandToken_p->fFromUserSpace,
                      BUFMANAGER_BUFFERTYPE_IN,
                      CommandToken_p->Service.PkAssetEncrypt.SrcData_p,
                      CommandToken_p->Service.PkAssetEncrypt.SrcDataSize,
                      NULL);
    if (SrcDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
    SrcDataAddr = (uint64_t) CommandToken_p->Service.PkAssetEncrypt.SrcData_p;

    // Get output data address for token
    TokenID = vex_DeviceGetTokenID();

/*
    DstDataAddr = BufManager_Map(
                      CommandToken_p->fFromUserSpace,
                      BUFMANAGER_BUFFERTYPE_OUT,
                      CommandToken_p->Service.PkAssetEncrypt.DstData_p,
                      CommandToken_p->Service.PkAssetEncrypt.DstDataSize,
                      (void *)&TokenID);
    if (DstDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
    DstDataAddr = (uint64_t) CommandToken_p->Service.PkAssetEncrypt.DstData_p;

    ModulusWords = ((CommandToken_p->Service.PkAssetEncrypt.ModulusSizeInBits+31)/32);

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Asset_Command(
        &CommandToken,
        CommandToken_p->Service.PkAssetEncrypt.Method,
        ModulusWords, ModulusWords,
        0,
        CommandToken_p->Service.PkAssetEncrypt.KeyAssetId,
        CommandToken_p->Service.PkAssetEncrypt.DomainAssetId,
        0,
        SrcDataAddr, CommandToken_p->Service.PkAssetEncrypt.SrcDataSize,
        DstDataAddr, CommandToken_p->Service.PkAssetEncrypt.DstDataSize);
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
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (SrcDataAddr != 0)
    {
        (void)BufManager_Unmap(SrcDataAddr, false);
    }
    if (DstDataAddr != 0)
    {
        int rc = BufManager_Unmap(DstDataAddr, fCopy);
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

/* end of file adapter_vex_asym_asset_encrypt.c */
