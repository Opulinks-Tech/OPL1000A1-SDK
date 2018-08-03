/* adapter_vex_sym_hash.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the symmetric crypto hash services.
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
#include "log.h"

#include "adapter_vex_internal.h"   // API implementation
#include "adapter_bufmanager.h"     // BufManager_*()
#include "eip130_token_hash.h"      // Eip130Token_Command_Hash*()


/*----------------------------------------------------------------------------
 * vex_SymHash
 */
VexStatus_t
vex_SymHash(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    uint64_t DataAddr = 0;
    bool fInitWithDefault;
    bool fFinalize;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    switch (CommandToken_p->Service.Hash.Mode)
    {
    default:
    case VEXTOKEN_MODE_HASH_MAC_INIT2FINAL:
        fInitWithDefault = true;
        fFinalize = true;
        break;

    case VEXTOKEN_MODE_HASH_MAC_CONT2FINAL:
        fInitWithDefault = false;
        fFinalize = true;
        break;

    case VEXTOKEN_MODE_HASH_MAC_INIT2CONT:
        fInitWithDefault = true;
        fFinalize = false;
        break;

    case VEXTOKEN_MODE_HASH_MAC_CONT2CONT:
        fInitWithDefault = false;
        fFinalize = false;
        break;
    }

/*
    if (CommandToken_p->Service.Hash.DataSize != 0)
    {
        // Convert input buffer and get address for token
        DataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                  BUFMANAGER_BUFFERTYPE_IN,
                                  CommandToken_p->Service.Hash.Data_p,
                                  CommandToken_p->Service.Hash.DataSize,
                                  NULL);
        if (DataAddr == 0)
        {
            return VEX_NO_MEMORY;
        }
    }
    else if (!fInitWithDefault || !fFinalize)
    {
        return VEX_INVALID_LENGTH;
    }
*/
    DataAddr = (uint64_t) CommandToken_p->Service.Hash.Data_p;

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Hash(&CommandToken,
                             CommandToken_p->Service.Hash.Algorithm,
                             fInitWithDefault,
                             fFinalize,
                             DataAddr,
                             CommandToken_p->Service.Hash.DataSize);
    Eip130Token_Command_Hash_SetTempDigestASID(
        &CommandToken,
        CommandToken_p->Service.Hash.TempAssetId);
    Eip130Token_Command_Hash_SetTotalMessageLength(
        &CommandToken,
        CommandToken_p->Service.Hash.TotalMessageLength);
    Eip130Token_Command_SetTokenID(&CommandToken,
                                   vex_DeviceGetTokenID(),
                                   false);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if (fFinalize && (ResultToken_p->Result >= 0))
        {
            // Copy digest from result token
            Eip130Token_Result_Hash_CopyDigest(&ResultToken,
                                               64,
                                               ResultToken_p->Service.Hash.Digest);
        }
    }

/*
    // Release used buffer, if needed
    (void)BufManager_Unmap(DataAddr, false);
*/

    return funcres;
}


/* end of file adapter_vex_sym_hash.c */
