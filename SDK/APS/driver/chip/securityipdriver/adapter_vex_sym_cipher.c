/* adapter_vex_sym_cipher.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the symmetric crypto cipher services.
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

#include "adapter_vex_internal.h"   // API implementation
#include "adapter_bufmanager.h"     // BufManager_*()
#include "eip130_token_crypto.h"    // Eip130Token_Command_Crypto*()


/*----------------------------------------------------------------------------
 * vex_SymCipher
 */
VexStatus_t
vex_SymCipher(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint16_t TokenID;
    uint32_t SrcDataLength;
    uint64_t SrcDataAddr = 0;
    uint64_t DstDataAddr = 0;
    bool fCopy = false;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;
    uint32_t DstDataLength;

    SrcDataAddr = (uint64_t)CommandToken_p->Service.Cipher.SrcData_p;
    DstDataAddr = (uint64_t)CommandToken_p->Service.Cipher.DstData_p;
    DstDataLength = CommandToken_p->Service.Cipher.DataSize;

    SrcDataLength = CommandToken_p->Service.Cipher.DataSize;
    switch (CommandToken_p->Service.Cipher.Algorithm)
    {
    case VEXTOKEN_ALGO_CIPHER_AES:
        if ((SrcDataLength == 0) || (SrcDataLength & ((128/8) - 1)))
        {
            return VEX_INVALID_LENGTH;
        }
        break;

#if defined(VEX_ENABLE_SYM_ALGO_DES) || defined(VEX_ENABLE_SYM_ALGO_3DES)
#ifdef VEX_ENABLE_SYM_ALGO_DES
    case VEXTOKEN_ALGO_CIPHER_DES:
#endif
#ifdef VEX_ENABLE_SYM_ALGO_3DES
    case VEXTOKEN_ALGO_CIPHER_3DES:
#endif
        if ((SrcDataLength == 0) || (SrcDataLength & ((64/8) - 1)))
        {
            return VEX_INVALID_LENGTH;
        }
        break;
#endif

#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
    case VEXTOKEN_ALGO_CIPHER_CHACHA20:
        if (SrcDataLength & ((512/8) - 1))
        {
            return VEX_INVALID_LENGTH;
        }
        if (SrcDataLength == 0)
        {
            if (CommandToken_p->Service.Cipher.SrcData_p == NULL)
            {
                // The output buffer size is assumed at least 32-byte.
                SrcDataLength = (256 / 8);
            }
            else
            {
                return VEX_BAD_ARGUMENT;
            }
        }
        break;
#endif

    default:
        return VEX_UNSUPPORTED;
    }

/*
    // Convert input buffer and get address for token
    if (CommandToken_p->Service.Cipher.DataSize != 0)
    {
        SrcDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                     BUFMANAGER_BUFFERTYPE_IN,
                                     CommandToken_p->Service.Cipher.SrcData_p,
                                     SrcDataLength,
                                     NULL);
        if (SrcDataAddr == 0)
        {
            goto error_func_exit;
        }
    }

    // Get output address for token
    TokenID = vex_DeviceGetTokenID();
    DstDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                 BUFMANAGER_BUFFERTYPE_OUT,
                                 CommandToken_p->Service.Cipher.DstData_p,
                                 SrcDataLength,
                                 (void *)&TokenID);
    if (DstDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
	
    // Format command token
    ZEROINIT(CommandToken);
    switch (CommandToken_p->Service.Cipher.Algorithm)
    {
    case VEXTOKEN_ALGO_CIPHER_AES:
        Eip130Token_Command_Crypto_Operation(
            &CommandToken,
            EIP130TOKEN_CRYPTO_ALGO_AES,
            CommandToken_p->Service.Cipher.Mode,
            CommandToken_p->Service.Cipher.fEncrypt,
            SrcDataLength);


/*
        if (CommandToken_p->Service.Cipher.Mode == VEXTOKEN_MODE_CIPHER_XTS)
        {
            Eip130Token_Command_Crypto_AES_SetKeyLength(
                &CommandToken,
                CommandToken_p->Service.Cipher.KeySize/2);
        }
        else
        {
*/
        	Eip130Token_Command_Crypto_AES_SetKeyLength(
	            &CommandToken,
	            CommandToken_p->Service.Cipher.KeySize);

/*	
            if (CommandToken_p->Service.Cipher.Mode == VEXTOKEN_MODE_CIPHER_F8)
            {
                // f8 IV
                Eip130Token_Command_Crypto_Copyf8IV(
                    &CommandToken,
                    CommandToken_p->Service.Cipher.f8_IV);

                // f8 SaltKey
                Eip130Token_Command_Crypto_Copyf8SaltKey(
                    &CommandToken,
                    CommandToken_p->Service.Cipher.f8_SaltKey,
                    (uint32_t)CommandToken_p->Service.Cipher.f8_SaltKeySize);
            }
            
        }
*/
        break;

#ifdef VEX_ENABLE_SYM_ALGO_DES
    case VEXTOKEN_ALGO_CIPHER_DES:
        Eip130Token_Command_Crypto_Operation(
            &CommandToken,
            EIP130TOKEN_CRYPTO_ALGO_DES,
            CommandToken_p->Service.Cipher.Mode,
            CommandToken_p->Service.Cipher.fEncrypt,
            SrcDataLength);
        break;
#endif

#ifdef VEX_ENABLE_SYM_ALGO_3DES
    case VEXTOKEN_ALGO_CIPHER_3DES:
        Eip130Token_Command_Crypto_Operation(
            &CommandToken,
            EIP130TOKEN_CRYPTO_ALGO_3DES,
            CommandToken_p->Service.Cipher.Mode,
            CommandToken_p->Service.Cipher.fEncrypt,
            SrcDataLength);
        break;
#endif

#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
    case VEXTOKEN_ALGO_CIPHER_CHACHA20:
        Eip130Token_Command_Crypto_Operation(
            &CommandToken,
            EIP130TOKEN_CRYPTO_ALGO_CHACHA20,
            CommandToken_p->Service.Cipher.Mode,
            CommandToken_p->Service.Cipher.fEncrypt,
            CommandToken_p->Service.Cipher.DataSize);
        Eip130Token_Command_Crypto_ChaCha20_SetKeyLength(
            &CommandToken,
            CommandToken_p->Service.Cipher.KeySize);
        Eip130Token_Command_Crypto_SetNonceLength(
            &CommandToken,
            CommandToken_p->Service.Cipher.NonceLength);
        break;
#endif


    default:
        return VEX_UNSUPPORTED;
    }

    // Key
    if (CommandToken_p->Service.Cipher.KeyAssetId)
    {
        // From Asset Store
        Eip130Token_Command_Crypto_SetASLoadKey(
            &CommandToken,
            CommandToken_p->Service.Cipher.KeyAssetId);
    }
    else
    {
        // From token
        Eip130Token_Command_Crypto_CopyKey(
            &CommandToken,
            CommandToken_p->Service.Cipher.Key,
            CommandToken_p->Service.Cipher.KeySize);
    }

    // From token
    Eip130Token_Command_Crypto_CopyKey(
        &CommandToken,
        CommandToken_p->Service.Cipher.Key,
        CommandToken_p->Service.Cipher.KeySize);


    // IV
#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
    if ((CommandToken_p->Service.Cipher.Algorithm == VEXTOKEN_ALGO_CIPHER_CHACHA20) ||
        (CommandToken_p->Service.Cipher.Mode != VEXTOKEN_MODE_CIPHER_ECB))
#else
        if (CommandToken_p->Service.Cipher.Mode != VEXTOKEN_MODE_CIPHER_ECB)
#endif
    {

        if (CommandToken_p->Service.Cipher.TempAssetId != 0)
        {
            // From Asset Store
            Eip130Token_Command_Crypto_SetASLoadIV(
                &CommandToken,
                CommandToken_p->Service.Cipher.TempAssetId);
            Eip130Token_Command_Crypto_SetASSaveIV(
                &CommandToken,
                CommandToken_p->Service.Cipher.TempAssetId);
        }
        else
        {
            // From token
            Eip130Token_Command_Crypto_CopyIV(
                &CommandToken,
                CommandToken_p->Service.Cipher.IV);
        }

        // From token
        Eip130Token_Command_Crypto_CopyIV(
            &CommandToken,
            CommandToken_p->Service.Cipher.IV);

    }

/*
    Eip130Token_Command_Crypto_SetDataAddresses(
        &CommandToken,
        SrcDataAddr, SrcDataLength,
        DstDataAddr, BufManager_GetSize(DstDataAddr));
*/
    Eip130Token_Command_Crypto_SetDataAddresses(
        &CommandToken,
        SrcDataAddr, SrcDataLength,
        DstDataAddr, DstDataLength);


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

            if (CommandToken_p->Service.Cipher.TempAssetId == 0)
            {
                switch (CommandToken_p->Service.Cipher.Algorithm)
                {
                default:
                case VEXTOKEN_ALGO_CIPHER_AES:
#ifdef VEX_ENABLE_SYM_ALGO_DES
                case VEXTOKEN_ALGO_CIPHER_DES:
#endif
#ifdef VEX_ENABLE_SYM_ALGO_3DES
                case VEXTOKEN_ALGO_CIPHER_3DES:
#endif
                    if (CommandToken_p->Service.Cipher.Mode == VEXTOKEN_MODE_CIPHER_ECB)
                    {
                        break;
                    }
#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
                case VEXTOKEN_ALGO_CIPHER_CHACHA20:
#endif
                    // From token
                    Eip130Token_Result_Crypto_CopyIV(&ResultToken,
                                                     ResultToken_p->Service.Cipher.IV);
                    break;
                }
            }
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
        if (BufManager_Unmap(DstDataAddr, fCopy) != 0)
        {
            funcres = VEX_INTERNAL_ERROR;
        }
    }
*/

    return funcres;
}

/*----------------------------------------------------------------------------
 * vex_SymCipherAE
 */
VexStatus_t
vex_SymCipherAE(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint16_t TokenID;
    uint8_t Algorithm = EIP130TOKEN_CRYPTO_ALGO_AES;
    uint32_t ActualDataLength;
    uint32_t SrcDataLength;
    uint32_t DstDataLength = 0;
    uint64_t AADAddr = 0;
    uint64_t SrcDataAddr = 0;
    uint64_t DstDataAddr = 0;
    bool fCopy = false;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    ActualDataLength = CommandToken_p->Service.CipherAE.DataSize;
    switch (CommandToken_p->Service.CipherAE.Algorithm)
    {
    case VEXTOKEN_ALGO_CIPHER_AES:
        SrcDataLength = (ActualDataLength + ((128 / 8) - 1)) & ~((128 / 8) - 1);
#ifdef VEX_ENABLE_SYM_ALGO_AES_CGM
        if (CommandToken_p->Service.CipherAE.Mode != VEXTOKEN_MODE_CIPHER_GCM)
        {
            ActualDataLength = SrcDataLength;
        }
#else
        ActualDataLength = SrcDataLength;
#endif
        break;

#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
    case VEXTOKEN_ALGO_CIPHER_CHACHA20:
        Algorithm = EIP130TOKEN_CRYPTO_ALGO_CHACHA20;
        SrcDataLength = (ActualDataLength + ((512 / 8) - 1)) & ~((512 / 8) - 1);
        break;
#endif

    default:
        return VEX_BAD_ARGUMENT;
    }


/*
    // Convert input buffers and get addresses for token
    if (CommandToken_p->Service.CipherAE.AADSize > 0)
    {
        AADAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                 BUFMANAGER_BUFFERTYPE_IN,
                                 CommandToken_p->Service.CipherAE.AAD_p,
                                 CommandToken_p->Service.CipherAE.AADSize,
                                 NULL);
        if (AADAddr == 0)
        {
            goto error_func_exit;
        }
    }

    TokenID = vex_DeviceGetTokenID();

    if (SrcDataLength > 0)
    {
        SrcDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                     BUFMANAGER_BUFFERTYPE_IN,
                                     CommandToken_p->Service.CipherAE.SrcData_p,
                                     ActualDataLength,
                                     NULL);
        if (SrcDataAddr == 0)
        {
            goto error_func_exit;
        }

        if (CommandToken_p->Service.CipherAE.DstData_p != NULL)
        {
            // Get output address for token
            DstDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                         BUFMANAGER_BUFFERTYPE_OUT,
                                         CommandToken_p->Service.CipherAE.DstData_p,
                                         SrcDataLength,
                                         (void *)&TokenID);
            if (DstDataAddr == 0)
            {
                goto error_func_exit;
            }
            DstDataLength = BufManager_GetSize(DstDataAddr);
        }
    }
*/
    AADAddr = (uint64_t) CommandToken_p->Service.CipherAE.AAD_p;
    TokenID = vex_DeviceGetTokenID();
    if (SrcDataLength > 0)
    {
        SrcDataAddr = (uint64_t) CommandToken_p->Service.CipherAE.SrcData_p;

        if (CommandToken_p->Service.CipherAE.DstData_p != NULL)
        {
            DstDataAddr = (uint64_t) CommandToken_p->Service.CipherAE.DstData_p;
            DstDataLength = SrcDataLength;
        }
    }


    // Format command token
    ZEROINIT(CommandToken);

    Eip130Token_Command_Crypto_Operation(
        &CommandToken,
        Algorithm,
        CommandToken_p->Service.CipherAE.Mode,
        CommandToken_p->Service.CipherAE.fEncrypt,
        ActualDataLength);

    // Key
    switch (CommandToken_p->Service.CipherAE.Algorithm)
    {
    default:
    case VEXTOKEN_ALGO_CIPHER_AES:
        Eip130Token_Command_Crypto_AES_SetKeyLength(
            &CommandToken,
            CommandToken_p->Service.CipherAE.KeySize);
        break;

#ifdef VEX_ENABLE_SYM_ALGO_CHACHA20
    case VEXTOKEN_ALGO_CIPHER_CHACHA20:
        Eip130Token_Command_Crypto_ChaCha20_SetKeyLength(
            &CommandToken,
            CommandToken_p->Service.CipherAE.KeySize);
        break;
#endif
    }

    if (CommandToken_p->Service.CipherAE.KeyAssetId)
    {
        // From Asset Store
        Eip130Token_Command_Crypto_SetASLoadKey(
            &CommandToken,
            CommandToken_p->Service.CipherAE.KeyAssetId);
    }
    else
    {
        // From token
        Eip130Token_Command_Crypto_CopyKey(
            &CommandToken,
            CommandToken_p->Service.CipherAE.Key,
            CommandToken_p->Service.CipherAE.KeySize);
    }

#ifdef VEX_ENABLE_SYM_ALGO_AES_CGM
    if (CommandToken_p->Service.CipherAE.Mode == VEXTOKEN_MODE_CIPHER_GCM)
    {
        // Hash Key
        Eip130Token_Command_Crypto_CopyHashKey(
            &CommandToken,
            CommandToken_p->Service.CipherAE.NonceHashKey,
            (uint32_t)CommandToken_p->Service.CipherAE.NonceHashKeySize);
        Eip130Token_Command_Crypto_SetGcmMode(
            &CommandToken,
            CommandToken_p->Service.CipherAE.GCMMode);
        Eip130Token_Command_Crypto_CopyIV(
            &CommandToken,
            CommandToken_p->Service.CipherAE.IV);
    }
    else
#endif
    {
        // Nonce
        Eip130Token_Command_Crypto_CopyNonce(
            &CommandToken,
            CommandToken_p->Service.CipherAE.NonceHashKey,
            (uint32_t)CommandToken_p->Service.CipherAE.NonceHashKeySize);
    }

    // Set data addresses
    Eip130Token_Command_Crypto_SetADAddress(
        &CommandToken,
        AADAddr, (uint16_t)CommandToken_p->Service.CipherAE.AADSize);
    Eip130Token_Command_Crypto_SetDataAddresses(
        &CommandToken,
        SrcDataAddr, SrcDataLength,
        DstDataAddr, DstDataLength);
    Eip130Token_Command_Crypto_CopyTag(
        &CommandToken,
        CommandToken_p->Service.CipherAE.Tag,
        CommandToken_p->Service.CipherAE.TagSize);
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

            if (CommandToken_p->Service.CipherAE.fEncrypt)
            {
                // From token
                Eip130Token_Result_Crypto_CopyTag(
                    &ResultToken,
                    ResultToken_p->Service.CipherAE.Tag);
            }
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (AADAddr != 0)
    {
        (void)BufManager_Unmap(AADAddr, false);
    }
    if (SrcDataAddr != 0)
    {
        (void)BufManager_Unmap(SrcDataAddr, false);
    }
    if (DstDataAddr != 0)
    {
        if (BufManager_Unmap(DstDataAddr, fCopy) != 0)
        {
            funcres = VEX_INTERNAL_ERROR;
        }
    }
*/

    return funcres;
}


/* end of file adapter_vex_sym_cipher.c */
