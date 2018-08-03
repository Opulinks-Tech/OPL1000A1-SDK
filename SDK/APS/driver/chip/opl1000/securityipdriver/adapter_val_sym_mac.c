/* adapter_val_sym_mac.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
 *
 * This file implements the symmetric crypto MAC services.
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

#ifndef VAL_REMOVE_SYM_MAC

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "api_val_sym.h"                // the API to implement
#include "api_val_asset.h"              // Asset Management related information
#include "api_val_system.h"             // val_IsAccessSecure()
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t


/*----------------------------------------------------------------------------
 * val_SymMacUpdate
 */
ValStatus_t
val_SymMacUpdate(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;
    //ValPolicyMask_t AssetPolicy = VAL_POLICY_TEMP_MAC;
    ValSize_t MacNBytes = 0;

#if 0
#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        (Data_p == NULL) ||
        (DataSize == 0))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_MAC;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_MACUPDATE;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Set MAC algorithm
    switch (SymContext_p->Algorithm)
    {
    case VAL_SYM_ALGO_MAC_HMAC_SHA1:

#if 0
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_HASH_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
#endif

        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA1;
        MacNBytes = (160 / 8);
        //AssetPolicy |= VAL_POLICY_SHA1;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA224:

#if 0
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_HASH_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
#endif

        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA224;
        MacNBytes = (256 / 8);
        // Note: intermediate digest is always 256 bits, final is 224 bits
        //AssetPolicy |= VAL_POLICY_SHA224;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
#if 0
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_HASH_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
#endif

        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA256;
        MacNBytes = (256 / 8);
        //AssetPolicy |= VAL_POLICY_SHA256;
        break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_HASH_SHA512_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA384;
        MacNBytes = (512 / 8);
        //AssetPolicy |= VAL_POLICY_SHA384;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_HASH_SHA512_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA512;
        MacNBytes = (512 / 8);
        //AssetPolicy |= VAL_POLICY_SHA512;
        break;
#endif

    case VAL_SYM_ALGO_MAC_AES_CMAC:
#if 0
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_AES_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
#endif

        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CMAC;
        MacNBytes = VAL_SYM_ALGO_AES_IV_SIZE;
        //AssetPolicy |= VAL_POLICY_ALGO_CIPHER_AES | VAL_POLICY_CMAC;
        break;

    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
#if 0
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_AES_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
#endif

        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CBC_MAC;
        MacNBytes = VAL_SYM_ALGO_AES_IV_SIZE;
        //AssetPolicy |= VAL_POLICY_ALGO_CIPHER_AES;
        break;

#if 0
#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
    case VAL_SYM_ALGO_MAC_POLY1305:
#ifdef VAL_STRICT_ARGS
        if ((DataSize & (VAL_SYM_ALGO_POLY1305_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_POLY1305;
        MacNBytes = 24;
        AssetPolicy |= VAL_POLICY_POLY1305;
        break;
#endif
#endif

    default:
        return VAL_INVALID_ALGORITHM;
    }

#if 0
    // Set MAC mode
    if (!SymContext_p->InitDone)
    {
        if (SymContext_p->Service.Mac.TempAssetId == VAL_ASSETID_INVALID)
        {
            // Allocate Asset for intermediate MAC value
            if (!val_IsAccessSecure())
            {
                AssetPolicy |= VAL_POLICY_SOURCE_NON_SECURE;
            }
            funcres = val_AssetAlloc(AssetPolicy,
                                     MacNBytes,
                                     &SymContext_p->Service.Mac.TempAssetId);
            if (funcres != VAL_SUCCESS)
            {
                LOG_WARN("%s: Abort - AssetAlloc()=%d\n", __func__, funcres);
                return funcres;
            }

            if (SymContext_p->Algorithm == VAL_SYM_ALGO_MAC_AES_CBC_MAC)
            {
                // Initialize the intermediate MAC Asset data
                uint8_t * AssetData_p = Adapter_Alloc(MacNBytes);
                if (AssetData_p == NULL)
                {
                    funcres = VAL_NO_MEMORY;
                }
                else
                {
                    memcpy(AssetData_p, SymContext_p->Service.Mac.Mac, MacNBytes);
                    funcres = val_AssetLoadPlaintext(SymContext_p->Service.Mac.TempAssetId,
                                                     AssetData_p, MacNBytes);
                    Adapter_Free(AssetData_p);
                }
                if (funcres != VAL_SUCCESS)
                {
                    LOG_WARN("%s: Abort - AssetLoadPlaintext()=%d\n",
                             __func__, funcres);

                    // Free Asset again
                    (void)val_AssetFree(SymContext_p->Service.Mac.TempAssetId);
                    SymContext_p->Service.Mac.TempAssetId = VAL_ASSETID_INVALID;
                    return funcres;
                }
            }
        }

        t_cmd.Service.Mac.Mode = VEXTOKEN_MODE_HASH_MAC_INIT2CONT;
    }
    else
    {
        t_cmd.Service.Mac.Mode = VEXTOKEN_MODE_HASH_MAC_CONT2CONT;
    }


    t_cmd.Service.Mac.TempAssetId = SymContext_p->Service.Mac.TempAssetId;
    t_cmd.Service.Mac.KeyAssetId = SymContext_p->Service.Mac.KeyAssetId;
    if (t_cmd.Service.Mac.KeyAssetId == VAL_ASSETID_INVALID)
    {
        memcpy(t_cmd.Service.Mac.Key,
               SymContext_p->Service.Mac.Key,
               SymContext_p->Service.Mac.KeySize);
    }
#endif

    t_cmd.Service.Mac.KeySize = SymContext_p->Service.Mac.KeySize;
    t_cmd.Service.Mac.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Mac.DataSize = (uint32_t)DataSize;
    t_cmd.Service.Mac.TotalMessageLength = 0;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            SymContext_p->Service.Mac.TotalMessageLength += (uint64_t)DataSize;

            // Mark initialization done
            SymContext_p->InitDone = true;
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_SymMacGenerate
 */
ValStatus_t
val_SymMacGenerate(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize,
        ValOctetsOut_t * const Mac_p,
        ValSize_t * const MacSize_p)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;
    uint32_t MacNBytes = 0;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        (Mac_p == NULL) ||
        (MacSize_p == NULL) ||
        ((Data_p == NULL) && (DataSize != 0)))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Format service request
    ZEROINIT(t_cmd);
    t_cmd.OpCode = VEXTOKEN_OPCODE_MAC;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_MACGENERATE;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
    ZEROINIT(t_res.Service.Mac.Mac);

    // Set MAC algorithm
    switch (SymContext_p->Algorithm)
    {
    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA1;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        MacNBytes = (160 / 8);
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA224;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        MacNBytes = (224 / 8);
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA256;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        MacNBytes = (256 / 8);
        break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA384;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        MacNBytes = (384 / 8);
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA512;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        MacNBytes = (512 / 8);
        break;
#endif

    case VAL_SYM_ALGO_MAC_AES_CMAC:
        // TotalMessageLength and last block padding is handled in VEX part
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CMAC;
        MacNBytes = VAL_SYM_ALGO_AES_BLOCK_SIZE;
        break;

    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
        // TotalMessageLength and last block padding is handled in VEX part
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CBC_MAC;
        MacNBytes = VAL_SYM_ALGO_AES_BLOCK_SIZE;
        break;

#if 0
#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
    case VAL_SYM_ALGO_MAC_POLY1305:
        // TotalMessageLength is not used
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_POLY1305;
        MacNBytes = 16;
        break;
#endif
#endif

    default:
        return VAL_INVALID_ALGORITHM;
    }

    if (MacNBytes > *MacSize_p)
    {
        // Final digest does not fit in buffer
        *MacSize_p = MacNBytes;
        return VAL_BUFFER_TOO_SMALL;
    }

    // Set MAC mode
    if (SymContext_p->InitDone)
    {
        t_cmd.Service.Mac.Mode = VEXTOKEN_MODE_HASH_MAC_CONT2FINAL;
        //t_cmd.Service.Mac.TempAssetId = SymContext_p->Service.Mac.TempAssetId;
    }
    else
    {
        t_cmd.Service.Mac.Mode = VEXTOKEN_MODE_HASH_MAC_INIT2FINAL;
        //t_cmd.Service.Mac.TempAssetId = VAL_ASSETID_INVALID;
    }
	
/*
    t_cmd.Service.Mac.KeyAssetId = SymContext_p->Service.Mac.KeyAssetId;
    if (t_cmd.Service.Mac.KeyAssetId == VAL_ASSETID_INVALID)
    {
        memcpy(t_cmd.Service.Mac.Key,
               SymContext_p->Service.Mac.Key,
               SymContext_p->Service.Mac.KeySize);
        t_cmd.Service.Mac.KeySize = SymContext_p->Service.Mac.KeySize;
    }
*/
	
    t_cmd.Service.Mac.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Mac.DataSize = (uint32_t)DataSize;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            // Copy generated MAC value
            memcpy(Mac_p, t_res.Service.Mac.Mac, MacNBytes);
            *MacSize_p = MacNBytes;

            // Release symmetric crypto context
            (void)val_SymRelease(Context_p);
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_SymMacVerify
 */
ValStatus_t
val_SymMacVerify(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize,
        ValAssetId_Optional_t const MacAssetId,
        ValOctetsIn_Optional_t * const Mac_p,
        const ValSize_t MacSize)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        ((MacAssetId == VAL_ASSETID_INVALID) && (Mac_p == NULL)) ||
        ((Data_p == NULL) && (DataSize != 0)))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_MAC;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_MACVERIFY;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Set MAC algorithm
    switch (SymContext_p->Algorithm)
    {
    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA1;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA224;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA256;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA384;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        break;

    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_HMAC_SHA512;
        t_cmd.Service.Mac.TotalMessageLength =
            SymContext_p->Service.Mac.TotalMessageLength + (uint64_t)DataSize;
        break;
#endif

    case VAL_SYM_ALGO_MAC_AES_CMAC:
        // TotalMessageLength and last block padding is handled in VEX part
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CMAC;
        break;

    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
        // TotalMessageLength and last block padding is handled in VEX part
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_AES_CBC_MAC;
        break;

#if 0        
#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
    case VAL_SYM_ALGO_MAC_POLY1305:
        // TotalMessageLength is not used
        t_cmd.Service.Mac.Algorithm = VEXTOKEN_ALGO_MAC_POLY1305;
        t_cmd.Service.Mac.TotalMessageLength = 0;
        break;
#endif
#endif

    default:
        return VAL_INVALID_ALGORITHM;
    }

    // Set MAC mode
    if (SymContext_p->InitDone)
    {
        t_cmd.Service.Hash.Mode = VEXTOKEN_MODE_HASH_MAC_CONT2FINAL;
        //t_cmd.Service.Hash.TempAssetId = SymContext_p->Service.Hash.TempAssetId;
    }
    else
    {
        t_cmd.Service.Hash.Mode = VEXTOKEN_MODE_HASH_MAC_INIT2FINAL;
        //t_cmd.Service.Hash.TempAssetId = VAL_ASSETID_INVALID;
    }
	
/*
    t_cmd.Service.Mac.KeyAssetId = SymContext_p->Service.Mac.KeyAssetId;
    if (t_cmd.Service.Mac.KeyAssetId == VAL_ASSETID_INVALID)
    {
        memcpy(t_cmd.Service.Mac.Key,
               SymContext_p->Service.Mac.Key,
               SymContext_p->Service.Mac.KeySize);
        t_cmd.Service.Mac.KeySize = SymContext_p->Service.Mac.KeySize;
    }
*/
    
    t_cmd.Service.Mac.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Mac.DataSize = (uint32_t)DataSize;
    t_cmd.Service.Mac.MacAssetId = MacAssetId;
    if (t_cmd.Service.Mac.MacAssetId == VAL_ASSETID_INVALID)
    {
        memcpy(t_cmd.Service.Mac.Mac, Mac_p, MacSize);
        t_cmd.Service.Mac.MacSize = MacSize;
    }

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        funcres = (ValStatus_t)t_res.Result;
        if ((t_res.Result == VEXTOKEN_RESULT_NO_ERROR) ||
            (t_res.Result == VEXTOKEN_RESULT_SEQ_VERIFY_ERROR))
        {
            // Release symmetric crypto context
            (void)val_SymRelease(Context_p);
        }
        else
        {
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}


#endif /* !VAL_REMOVE_SYM_MAC */

/* end of file adapter_val_sym_mac.c */
