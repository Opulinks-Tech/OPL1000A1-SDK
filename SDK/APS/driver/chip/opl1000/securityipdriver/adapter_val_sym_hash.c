/* adapter_val_sym_hash.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
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

#include "c_adapter_val.h"              // configuration

#ifndef VAL_REMOVE_SYM_HASH

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "api_val_sym.h"                // the API to implement
#include "api_val_asset.h"              // Asset Management related information
#include "api_val_system.h"             // val_IsAccessSecure()
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t


/*----------------------------------------------------------------------------
 * val_SymHashUpdate
 */
ValStatus_t
val_SymHashUpdate(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        (Data_p == NULL) ||
        (DataSize == 0))
    {
        return VAL_BAD_ARGUMENT;
    }

    switch (SymContext_p->Algorithm)
    {
    case VAL_SYM_ALGO_HASH_SHA1:
    case VAL_SYM_ALGO_HASH_SHA224:
    case VAL_SYM_ALGO_HASH_SHA256:
        if ((DataSize & (VAL_SYM_ALGO_HASH_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
        break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_HASH_SHA384:
    case VAL_SYM_ALGO_HASH_SHA512:
        if ((DataSize & (VAL_SYM_ALGO_HASH_SHA512_BLOCK_SIZE-1)) != 0)
        {
            return VAL_BAD_ARGUMENT;
        }
        break;
#endif

    default:
        break;
    }
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_HASH;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_NOT_USED;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Set hash algorithm & mode
    if (!SymContext_p->InitDone)
    {
        //ValPolicyMask_t AssetPolicy = VAL_POLICY_TEMP_MAC;
        ValSize_t DigestNBytes = 0;

        switch (SymContext_p->Algorithm)
        {
        case VAL_SYM_ALGO_HASH_SHA1:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA1;
            DigestNBytes = (160 / 8);
            //AssetPolicy |= VAL_POLICY_SHA1;
            break;

        case VAL_SYM_ALGO_HASH_SHA224:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA224;
            DigestNBytes = (256 / 8);
            // Note: intermediate digest is always 256 bits, final is 224 bits
            //AssetPolicy |= VAL_POLICY_SHA224;
            break;

        case VAL_SYM_ALGO_HASH_SHA256:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA256;
            DigestNBytes = (256 / 8);
            //AssetPolicy |= VAL_POLICY_SHA256;
            break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
        case VAL_SYM_ALGO_HASH_SHA384:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA384;
            DigestNBytes = (512 / 8);
            //AssetPolicy |= VAL_POLICY_SHA384;
            break;

        case VAL_SYM_ALGO_HASH_SHA512:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA512;
            DigestNBytes = (512 / 8);
            //AssetPolicy |= VAL_POLICY_SHA512;
            break;
#endif

        default:
            return VAL_INVALID_ALGORITHM;
        }

#if 0
        if (SymContext_p->Service.Hash.TempAssetId == VAL_ASSETID_INVALID)
        {
            // Allocate Asset for intermediate digest
            if (!val_IsAccessSecure())
            {
                AssetPolicy |= VAL_POLICY_SOURCE_NON_SECURE;
            }
            funcres = val_AssetAlloc(AssetPolicy,
                                     DigestNBytes,
                                     &SymContext_p->Service.Hash.TempAssetId);
            if (funcres != VAL_SUCCESS)
            {
                LOG_WARN("%s: Abort - AssetAlloc()=%d\n", __func__, funcres);
                return funcres;
            }
        }
#endif

        // Set hash mode
        t_cmd.Service.Hash.Mode = VEXTOKEN_MODE_HASH_MAC_INIT2CONT;
    }
    else
    {
        switch (SymContext_p->Algorithm)
        {
        case VAL_SYM_ALGO_HASH_SHA1:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA1;
            break;

        case VAL_SYM_ALGO_HASH_SHA224:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA224;
            break;

        case VAL_SYM_ALGO_HASH_SHA256:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA256;
            break;

#ifndef VAL_REMOVE_SYM_ALGO_SHA512
        case VAL_SYM_ALGO_HASH_SHA384:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA384;
            break;

        case VAL_SYM_ALGO_HASH_SHA512:
            t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA512;
            break;
#endif

        default:
            return VAL_INVALID_ALGORITHM;
        }

        // Set hash mode
        t_cmd.Service.Hash.Mode = VEXTOKEN_MODE_HASH_MAC_CONT2CONT;
    }

    //t_cmd.Service.Hash.TempAssetId = SymContext_p->Service.Hash.TempAssetId;
    t_cmd.Service.Hash.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Hash.DataSize = (uint32_t)DataSize;
    t_cmd.Service.Hash.TotalMessageLength = 0;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            SymContext_p->Service.Hash.TotalMessageLength += (uint64_t)DataSize;

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
 * val_SymHashFinal
 */
ValStatus_t
val_SymHashFinal(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize,
        ValOctetsOut_t * const Digest_p,
        ValSize_t * const DigestSize_p)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    uint32_t DigestNBytes = 0;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        (Digest_p == NULL) ||
        (DigestSize_p == NULL) ||
        ((Data_p == NULL) && (DataSize != 0)))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_HASH;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_NOT_USED;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
    ZEROINIT(t_res.Service.Hash.Digest);

    // Set hash algorithm
    switch (SymContext_p->Algorithm)
    {
	    case VAL_SYM_ALGO_HASH_SHA1:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA1;
	        DigestNBytes = (160 / 8);
	        break;

	    case VAL_SYM_ALGO_HASH_SHA224:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA224;
	        DigestNBytes = (224 / 8);
	        break;

	    case VAL_SYM_ALGO_HASH_SHA256:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA256;
	        DigestNBytes = (256 / 8);
	        break;
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_HASH_SHA384:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA384;
	        DigestNBytes = (384 / 8);
	        break;
	    case VAL_SYM_ALGO_HASH_SHA512:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA512;
	        DigestNBytes = (512 / 8);
	        break;
#endif

	    default:
	        return VAL_INVALID_ALGORITHM;
    }

    if (DigestNBytes > *DigestSize_p)
    {
        // Final digest does not fit in buffer
        *DigestSize_p = DigestNBytes;
        return VAL_BUFFER_TOO_SMALL;
    }

    // Set hash mode
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

    t_cmd.Service.Hash.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Hash.DataSize = (uint32_t)DataSize;
    t_cmd.Service.Hash.TotalMessageLength =
        SymContext_p->Service.Hash.TotalMessageLength + (uint64_t)DataSize;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            // Copy final digest
            memcpy(Digest_p, t_res.Service.Hash.Digest, DigestNBytes);
            *DigestSize_p = DigestNBytes;

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
 * val_SymHashGet
 */
ValStatus_t
val_SymHashGet(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const Data_p,
        const ValSize_t DataSize,
        ValOctetsOut_t * const Digest_p,
        ValSize_t * const DigestSize_p)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    uint32_t DigestNBytes = 0;
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_HASH;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_NOT_USED;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
	
    ZEROINIT(t_res.Service.Hash.Digest);

    // Set hash algorithm
    switch (SymContext_p->Algorithm)
    {
	    case VAL_SYM_ALGO_HASH_SHA1:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA1;
	        DigestNBytes = (160 / 8);
	        break;

	    case VAL_SYM_ALGO_HASH_SHA224:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA224;
	        DigestNBytes = (224 / 8);
	        break;

	    case VAL_SYM_ALGO_HASH_SHA256:
	        t_cmd.Service.Hash.Algorithm = VEXTOKEN_ALGO_HASH_SHA256;
	        DigestNBytes = (256 / 8);
	        break;

	    default:
	        return VAL_INVALID_ALGORITHM;
    }

    if (DigestNBytes > *DigestSize_p)
    {
        // Final digest does not fit in buffer
        *DigestSize_p = DigestNBytes;
        return VAL_BUFFER_TOO_SMALL;
    }

    // Set hash mode
    t_cmd.Service.Hash.Mode = VEXTOKEN_MODE_HASH_MAC_INIT2FINAL;
    t_cmd.Service.Hash.TempAssetId = VAL_ASSETID_INVALID;

    t_cmd.Service.Hash.Data_p = (const uint8_t *)Data_p;
    t_cmd.Service.Hash.DataSize = (uint32_t)DataSize;
    t_cmd.Service.Hash.TotalMessageLength = SymContext_p->Service.Hash.TotalMessageLength + (uint64_t)DataSize;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            // Copy final digest
            memcpy(Digest_p, t_res.Service.Hash.Digest, DigestNBytes);
            *DigestSize_p = DigestNBytes;

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


#endif /* !VAL_REMOVE_SYM_HASH */

/* end of file adapter_val_sym_hash.c */
