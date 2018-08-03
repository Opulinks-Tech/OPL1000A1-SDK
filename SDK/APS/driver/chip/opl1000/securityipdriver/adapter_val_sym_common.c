/* adapter_val_sym_common.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
 *
 * This file implements the symmetric crypto common functions.
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

#include "api_val_sym.h"                // the API to implement
#include "api_val_asset.h"              // Asset Management related information
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t
#include "msg.h"


/*----------------------------------------------------------------------------
 * val_SymAlloc
 */
ValStatus_t
val_SymAlloc(
        ValSymAlgo_t Algorithm,
        ValSymMode_t Mode,
        ValSymContextPtr_t * Context_pp)
{
    ValSymContext_t * SymContext_p;

    if (Context_pp == NULL)
    {
        return VAL_BAD_ARGUMENT;
    }

    // Perform initialization based on specified algorithm
    switch (Algorithm)
    {
        // Hash algorithms
	    case VAL_SYM_ALGO_HASH_SHA1:
	    case VAL_SYM_ALGO_HASH_SHA224:
	    case VAL_SYM_ALGO_HASH_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_HASH_SHA384:
	    case VAL_SYM_ALGO_HASH_SHA512:
#endif
        Mode = VAL_SYM_MODE_NONE;      // Force mode
        break;

        // MAC algorithms
	    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
	    case VAL_SYM_ALGO_MAC_AES_CMAC:
	    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
        case VAL_SYM_ALGO_MAC_POLY1305:
#endif
	        Mode = VAL_SYM_MODE_NONE;      // Force mode
	        break;

        // (Block)Cipher algorithms
	    case VAL_SYM_ALGO_CIPHER_AES:
	        switch (Mode)
	        {
		        case VAL_SYM_MODE_CIPHER_ECB:
		        case VAL_SYM_MODE_CIPHER_CBC:
		        case VAL_SYM_MODE_CIPHER_CTR:
		        case VAL_SYM_MODE_CIPHER_ICM:
#ifndef VAL_REMOVE_SYM_ALGO_AES_F8
	        	case VAL_SYM_MODE_CIPHER_F8:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_AES_CCM
	        	case VAL_SYM_MODE_CIPHER_CCM:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_AES_XTS
	        	case VAL_SYM_MODE_CIPHER_XTS:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_AES_GCM
	        	case VAL_SYM_MODE_CIPHER_GCM:
#endif
		            break;

		        default:
		            return VAL_INVALID_MODE;
		        }
		        break;
#ifndef VAL_REMOVE_SYM_ALGO_DES
    	case VAL_SYM_ALGO_CIPHER_DES:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_3DES
    	case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
#endif
#if !defined(VAL_REMOVE_SYM_ALGO_DES) || !defined(VAL_REMOVE_SYM_ALGO_3DES)
	        switch (Mode)
	        {
	        case VAL_SYM_MODE_CIPHER_ECB:
	        case VAL_SYM_MODE_CIPHER_CBC:
	            break;

	        default:
	            return VAL_INVALID_MODE;
	        }
	        break;
#endif
#if !defined(VAL_REMOVE_SYM_ALGO_CHACHA20)
            case VAL_SYM_ALGO_CIPHER_CHACHA20:
                switch (Mode)
                {
                case VAL_SYM_MODE_CIPHER_CHACHA20_ENC:
                case VAL_SYM_MODE_CIPHER_CHACHA20_AEAD:
                    break;
        
                default:
                    return VAL_INVALID_MODE;
                }
                break;
#endif
	    default:
	        return VAL_INVALID_ALGORITHM;
    }

    // Allocate the symmetric crypto context
    SymContext_p = Adapter_Alloc(sizeof(ValSymContext_t));
    if (SymContext_p == NULL)
    {
        msg_print(LOG_HIGH_LEVEL, "[security] val_SymAlloc, SymContext_p == NULL \r\n");
        
        return VAL_NO_MEMORY;
    }

    // Initialize the symmetric crypto context
    memset(SymContext_p, 0, sizeof(ValSymContext_t));
    SymContext_p->Algorithm = Algorithm;
    SymContext_p->Mode = Mode;
#if !defined(VAL_REMOVE_SYM_ALGO_CHACHA20)
    if (Algorithm == VAL_SYM_ALGO_CIPHER_CHACHA20)
    {
        SymContext_p->Service.Cipher.NonceLength = 12;
    }
#endif
    SymContext_p->MagicBegin = VALMARKER_SYMCONTEXT;
    SymContext_p->MagicEnd = VALMARKER_SYMCONTEXT;
    *Context_pp = (ValSymContextPtr_t)SymContext_p;

    return VAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * val_SymInitKey
 */
ValStatus_t
val_SymInitKey(
        ValSymContextPtr_t const Context_p,
        ValAssetId_Optional_t KeyAssetId,
        ValOctetsIn_Optional_t * const Key_p,
        const ValSize_t KeySize)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    uint8_t * ContextKey_p = NULL;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) //||
        //(SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        //((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0)
        )
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Perform key initialization based on algorithm
    switch (SymContext_p->Algorithm)
    {
    default:
    case VAL_SYM_ALGO_HASH_SHA1:
    case VAL_SYM_ALGO_HASH_SHA224:
    case VAL_SYM_ALGO_HASH_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_HASH_SHA384:
    case VAL_SYM_ALGO_HASH_SHA512:
#endif
        return VAL_UNSUPPORTED;

    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
#endif
/*
        if (KeyAssetId != VAL_ASSETID_INVALID)
        {
            SymContext_p->Service.Mac.KeyAssetId = KeyAssetId;
            SymContext_p->Service.Mac.KeySize = KeySize;
            return VAL_SUCCESS;
        }
*/
        if (Key_p == NULL)
        {
            if (KeySize != 0)
            {
                return VAL_INVALID_KEYSIZE;
            }
            return VAL_SUCCESS;
        }

        switch (SymContext_p->Algorithm)
        {
        default:
        case VAL_SYM_ALGO_MAC_HMAC_SHA1:
        case VAL_SYM_ALGO_MAC_HMAC_SHA224:
        case VAL_SYM_ALGO_MAC_HMAC_SHA256:
            if (KeySize <= VAL_SYM_ALGO_MAX_SHA2_MAC_KEY_SIZE)
            {
                ContextKey_p = SymContext_p->Service.Mac.Key;
                SymContext_p->Service.Mac.KeySize = KeySize;
                goto func_return;
            }
            break;
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
        case VAL_SYM_ALGO_MAC_HMAC_SHA384:
        case VAL_SYM_ALGO_MAC_HMAC_SHA512:
            if (KeySize <= VAL_SYM_ALGO_MAX_SHA512_MAC_KEY_SIZE)
            {
                ContextKey_p = SymContext_p->Service.Mac.Key;
                SymContext_p->Service.Mac.KeySize = KeySize;
                goto func_return;
            }
            break;
#endif
        }

        // Hash key
        {
            ValSymContextPtr_t TmpContext_p = NULL;
            ValSymAlgo_t Algorithm;
            ValStatus_t Status;

            switch (SymContext_p->Algorithm)
            {
            case VAL_SYM_ALGO_MAC_HMAC_SHA1:
                Algorithm = VAL_SYM_ALGO_HASH_SHA1;
                break;
            case VAL_SYM_ALGO_MAC_HMAC_SHA224:
                Algorithm = VAL_SYM_ALGO_HASH_SHA224;
                break;
            default:
            case VAL_SYM_ALGO_MAC_HMAC_SHA256:
                Algorithm = VAL_SYM_ALGO_HASH_SHA256;
                break;
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
            case VAL_SYM_ALGO_MAC_HMAC_SHA384:
                Algorithm = VAL_SYM_ALGO_HASH_SHA384;
                break;
            case VAL_SYM_ALGO_MAC_HMAC_SHA512:
                Algorithm = VAL_SYM_ALGO_HASH_SHA512;
                break;
#endif
            }

            Status = val_SymAlloc(Algorithm, VAL_SYM_MODE_NONE, &TmpContext_p);
            if  (Status == VAL_SUCCESS)
            {
                uint8_t * CopyKey_p = Adapter_Alloc(KeySize);
                if (CopyKey_p != NULL)
                {
                    ValSize_t TmpKeySize;

                    memcpy(CopyKey_p, Key_p, KeySize);
                    TmpKeySize = sizeof(SymContext_p->Service.Mac.Key);
                    Status = val_SymHashFinal(TmpContext_p,
                                              CopyKey_p,
                                              KeySize,
                                              SymContext_p->Service.Mac.Key,
                                              &TmpKeySize);
                    Adapter_Free(CopyKey_p);
                    if  (Status == VAL_SUCCESS)
                    {
                        SymContext_p->Service.Mac.KeySize = (uint32_t)TmpKeySize;
                        return VAL_SUCCESS;
                    }
                }

                (void)val_SymRelease(TmpContext_p);
            }
            msg_print(LOG_HIGH_LEVEL, "[security] val_SymInitKey, hashkey error: VAL_INVALID_KEYSIZE \r\n");
            return VAL_INVALID_KEYSIZE;
        }
        break;

    case VAL_SYM_ALGO_MAC_AES_CMAC:
    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
        if ((KeySize != (128/8)) &&
            (KeySize != (192/8)) &&
            (KeySize != (256/8)))
        {
            msg_print(LOG_HIGH_LEVEL, "[security] val_SymInitKey, case VAL_SYM_ALGO_MAC_AES_CMAC error: VAL_INVALID_KEYSIZE \r\n");
            return VAL_INVALID_KEYSIZE;
        }
/*
        if (KeyAssetId != VAL_ASSETID_INVALID)
        {
            SymContext_p->Service.Mac.KeyAssetId = KeyAssetId;
            SymContext_p->Service.Mac.KeySize = KeySize;
            return VAL_SUCCESS;
        }
*/
        if (Key_p == NULL)
        {
            return VAL_BAD_ARGUMENT;
        }
        ContextKey_p = SymContext_p->Service.Mac.Key;
        SymContext_p->Service.Mac.KeySize = KeySize;
        break;

#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
        case VAL_SYM_ALGO_MAC_POLY1305:
            if (KeySize != (256/8))
            {
                return VAL_INVALID_KEYSIZE;
            }
            if (KeyAssetId != VAL_ASSETID_INVALID)
            {
                SymContext_p->Service.Mac.KeyAssetId = KeyAssetId;
                SymContext_p->Service.Mac.KeySize = KeySize;
                return VAL_SUCCESS;
            }
            if (Key_p == NULL)
            {
                return VAL_BAD_ARGUMENT;
            }
            ContextKey_p = SymContext_p->Service.Mac.Key;
            SymContext_p->Service.Mac.KeySize = KeySize;
            break;
#endif

    case VAL_SYM_ALGO_CIPHER_AES:
        switch (SymContext_p->Mode)
        {
#ifndef VAL_REMOVE_SYM_ALGO_AES_XTS
        case VAL_SYM_MODE_CIPHER_XTS:
            if ((KeySize != (256/8)) && // AES-128
                (KeySize != (512/8)))   // AES-256
            {
                return VAL_INVALID_KEYSIZE;
            }
            break;
#endif

#ifndef VAL_REMOVE_SYM_ALGO_AES_F8
        case VAL_SYM_MODE_CIPHER_F8:
            if (KeySize != (128/8))
            {
                return VAL_INVALID_KEYSIZE;
            }
            break;
#endif

        default:
            if ((KeySize != (128/8)) &&
                (KeySize != (192/8)) &&
                (KeySize != (256/8)))
            {
                return VAL_INVALID_KEYSIZE;
            }
            break;
        }
/*
        if (KeyAssetId != VAL_ASSETID_INVALID)
        {
            SymContext_p->Service.Cipher.KeyAssetId = KeyAssetId;
            SymContext_p->Service.Cipher.KeySize = KeySize;
            return VAL_SUCCESS;
        }
*/
        if (Key_p == NULL)
        {
            return VAL_BAD_ARGUMENT;
        }
        ContextKey_p = SymContext_p->Service.Cipher.Key;
        SymContext_p->Service.Cipher.KeySize = KeySize;
        break;

#if 0
#ifndef VAL_REMOVE_SYM_ALGO_DES
    case VAL_SYM_ALGO_CIPHER_DES:
        if (KeySize != (64/8))
        {
            return VAL_INVALID_KEYSIZE;
        }
/*
        if ((KeyAssetId != VAL_ASSETID_INVALID) ||(Key_p == NULL))
        {
            return VAL_BAD_ARGUMENT;
        }
*/
        ContextKey_p = SymContext_p->Service.Cipher.Key;
        SymContext_p->Service.Cipher.KeySize = KeySize;
        break;
#endif
#endif

#if 0
#ifndef VAL_REMOVE_SYM_ALGO_3DES
    case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
        if (KeySize != (3*(64/8)))
        {
            return VAL_INVALID_KEYSIZE;
        }
/*
        if (KeyAssetId != VAL_ASSETID_INVALID)
        {
            SymContext_p->Service.Cipher.KeyAssetId = KeyAssetId;
            SymContext_p->Service.Cipher.KeySize = KeySize;
            return VAL_SUCCESS;
        }
*/
        if (Key_p == NULL)
        {
            return VAL_BAD_ARGUMENT;
        }
        ContextKey_p = SymContext_p->Service.Cipher.Key;
        SymContext_p->Service.Cipher.KeySize = KeySize;
        break;
#endif
#endif
#ifndef VAL_REMOVE_SYM_ALGO_CHACHA20
        case VAL_SYM_ALGO_CIPHER_CHACHA20:
            switch (SymContext_p->Mode)
            {
            case VAL_SYM_MODE_CIPHER_CHACHA20_AEAD:
                if (KeySize != (256/8))
                {
                    return VAL_INVALID_KEYSIZE;
                }
                break;
    
            default:
                if ((KeySize != (128/8)) &&
                    (KeySize != (256/8)))
                {
                    return VAL_INVALID_KEYSIZE;
                }
                break;
            }
            if (KeyAssetId != VAL_ASSETID_INVALID)
            {
                SymContext_p->Service.Cipher.KeyAssetId = KeyAssetId;
                SymContext_p->Service.Cipher.KeySize = KeySize;
                return VAL_SUCCESS;
            }
            if (Key_p == NULL)
            {
                return VAL_BAD_ARGUMENT;
            }
            ContextKey_p = SymContext_p->Service.Cipher.Key;
            SymContext_p->Service.Cipher.KeySize = KeySize;
            break;
#endif

    }

func_return:
    if (ContextKey_p != NULL)
    {
        memcpy(ContextKey_p, Key_p, KeySize);
    }
    else
    {
        return VAL_BAD_ARGUMENT;
    }

    return VAL_SUCCESS;
}


/*----------------------------------------------------------------------------
 * val_SymInitIV
 */
ValStatus_t
val_SymInitIV(
        ValSymContextPtr_t const Context_p,
        ValOctetsIn_t * const IV_p,
        const ValSize_t IVSize)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    uint8_t * ContextIV_p = NULL;
    ValSize_t CheckIVSize = 0;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0) ||
        (IV_p == NULL))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Perform key initialization based on algorithm
    switch (SymContext_p->Algorithm)
    {
	    default:
	    case VAL_SYM_ALGO_HASH_SHA1:
	    case VAL_SYM_ALGO_HASH_SHA224:
	    case VAL_SYM_ALGO_HASH_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_HASH_SHA384:
	    case VAL_SYM_ALGO_HASH_SHA512:
#endif
	    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
#endif
	    case VAL_SYM_ALGO_MAC_AES_CMAC:
        case VAL_SYM_ALGO_MAC_POLY1305:
	        return VAL_UNSUPPORTED;

        case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
            ContextIV_p = SymContext_p->Service.Mac.Mac;
            CheckIVSize = VAL_SYM_ALGO_AES_IV_SIZE;
            break;

	    case VAL_SYM_ALGO_CIPHER_AES:
	        switch (SymContext_p->Mode)
	        {
	        	case VAL_SYM_MODE_CIPHER_CBC:
		        case VAL_SYM_MODE_CIPHER_CTR:
		        case VAL_SYM_MODE_CIPHER_ICM:
#ifndef VAL_REMOVE_SYM_ALGO_AES_F8
	        	case VAL_SYM_MODE_CIPHER_F8:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_AES_XTS
	        	case VAL_SYM_MODE_CIPHER_XTS:
#endif
		            ContextIV_p = SymContext_p->Service.Cipher.IV;
		            break;
                    
#ifndef VAL_REMOVE_SYM_ALGO_AES_GCM
	        	case VAL_SYM_MODE_CIPHER_GCM:
#endif
	            	ContextIV_p = SymContext_p->Service.CipherAE.IV;
	            	break;

#ifndef VAL_REMOVE_SYM_ALGO_AES_CCM
	        	case VAL_SYM_MODE_CIPHER_CCM:
#endif

	        	case VAL_SYM_MODE_CIPHER_ECB:
				
		        default:
		            return VAL_UNSUPPORTED;
        	}
			
        CheckIVSize = VAL_SYM_ALGO_AES_IV_SIZE;
        break;
        
#ifndef VAL_REMOVE_SYM_ALGO_DES
    case VAL_SYM_ALGO_CIPHER_DES:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_3DES
    case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
#endif

#if !defined(VAL_REMOVE_SYM_ALGO_DES) || !defined(VAL_REMOVE_SYM_ALGO_3DES)
        if (SymContext_p->Mode != VAL_SYM_MODE_CIPHER_CBC)
        {
            return VAL_UNSUPPORTED;
        }
        ContextIV_p = SymContext_p->Service.Cipher.IV;
        CheckIVSize = VAL_SYM_ALGO_DES_IV_SIZE;
        break;
#endif

#if !defined(VAL_REMOVE_SYM_ALGO_CHACHA20)
    case VAL_SYM_ALGO_CIPHER_CHACHA20:
        switch (SymContext_p->Mode)
        {
        case VAL_SYM_MODE_CIPHER_CHACHA20_ENC:
            ContextIV_p = SymContext_p->Service.Cipher.IV;
            break;

        case VAL_SYM_MODE_CIPHER_CHACHA20_AEAD:
        default:
            return VAL_UNSUPPORTED;
        }
        CheckIVSize = VAL_SYM_ALGO_CHACHA20_IV_SIZE;
        break;
#endif
    }

    if ((ContextIV_p != NULL) && (IV_p != NULL))
    {
        if (IVSize != CheckIVSize)
        {
            return VAL_INVALID_LENGTH;
        }

        memcpy(ContextIV_p, IV_p, IVSize);
        return VAL_SUCCESS;
    }

    return VAL_BAD_ARGUMENT;
}


/*----------------------------------------------------------------------------
 * val_SymRelease
 */
ValStatus_t
val_SymRelease(
        ValSymContextPtr_t const Context_p)
{
    ValSymContext_t * SymContext_p = (ValSymContext_t *)Context_p;
    ValAssetId_t TempAssetId = VAL_ASSETID_INVALID;

#ifdef VAL_STRICT_ARGS
    if ((SymContext_p == NULL) ||
        (SymContext_p->MagicBegin != VALMARKER_SYMCONTEXT) ||
        ((SymContext_p->MagicBegin ^ SymContext_p->MagicEnd) != 0))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Perform reset based on algorithm
    switch (SymContext_p->Algorithm)
    {
	    default:
	    case VAL_SYM_ALGO_NONE:                // Not Used/Initialized
	        break;
	        // Hash algorithms
	    case VAL_SYM_ALGO_HASH_SHA1:
	    case VAL_SYM_ALGO_HASH_SHA224:
	    case VAL_SYM_ALGO_HASH_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_HASH_SHA384:
	    case VAL_SYM_ALGO_HASH_SHA512:
#endif
	        TempAssetId = SymContext_p->Service.Hash.TempAssetId;
	        break;

	        // MAC algorithms
	    case VAL_SYM_ALGO_MAC_HMAC_SHA1:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA224:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA256:
#ifndef VAL_REMOVE_SYM_ALGO_SHA512
	    case VAL_SYM_ALGO_MAC_HMAC_SHA384:
	    case VAL_SYM_ALGO_MAC_HMAC_SHA512:
#endif
	    case VAL_SYM_ALGO_MAC_AES_CMAC:
	    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
#ifndef VAL_REMOVE_SYM_ALGO_POLY1305
        case VAL_SYM_ALGO_MAC_POLY1305:
#endif
	        TempAssetId = SymContext_p->Service.Mac.TempAssetId;
	        break;

	        // (Block)Cipher algorithms
	    case VAL_SYM_ALGO_CIPHER_AES:
#ifndef VAL_REMOVE_SYM_ALGO_DES
    	case VAL_SYM_ALGO_CIPHER_DES:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_3DES
    	case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
#endif
#ifndef VAL_REMOVE_SYM_ALGO_CHACHA20
                case VAL_SYM_ALGO_CIPHER_CHACHA20:
#endif

	        TempAssetId = SymContext_p->Service.Cipher.TempAssetId;
	        break;
    }

/*
    if (TempAssetId != VAL_ASSETID_INVALID)
    {
        ValStatus_t funcres = val_AssetFree(TempAssetId);
        if (funcres != VAL_SUCCESS)
        {
            LOG_WARN("%s: Abort - AssetFree()=%d\n", __func__, funcres);
        }
    }
*/

    memset(SymContext_p, 0, sizeof(ValSymContext_t));
    Adapter_Free(SymContext_p);

    return VAL_SUCCESS;
}


/* end of file adapter_val_sym_common.c */
