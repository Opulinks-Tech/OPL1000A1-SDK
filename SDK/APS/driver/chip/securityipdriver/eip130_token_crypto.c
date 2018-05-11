#include "eip130_token_crypto.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_Operation
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Algorithm
 *     Operation algorithm. Must be one of EIP130TOKEN_CRYPTO_ALGO_*
 *
 * Mode
 *     Mode of operation. Must be one of EIP130TOKEN_CRYPTO_MODE_*
 *
 * fEncrypt
 *     true = Encrypt
 *     false = Decrypt
 *
 * DataLength
 *     Number of bytes to process.
 *     Must be a multiple of the blocksize.
 */
void
Eip130Token_Command_Crypto_Operation(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Algorithm,
        const uint8_t Mode,
        const bool fEncrypt,
        const uint32_t DataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ENCRYPTION << 24);
    CommandToken_p->W[2] = DataLengthInBytes;

    // Algorithm, Mode and direction
    CommandToken_p->W[11] = (MASK_4_BITS & Algorithm) +
                            ((MASK_4_BITS & Mode) << 4);
    if (fEncrypt)
    {
        CommandToken_p->W[11] |= BIT_15;
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetDataAddresses
 *
 * This function sets the Data address for Input and Output to use.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * InputDataAddress
 *     Input data Address.
 *
 * InputDataLengthInBytes
 *     Input data length.
 *
 * OutputDataAddress
 *     Output data Address.
 *
 * OutputDataLengthInBytes
 *     Output data length.
 */
void
Eip130Token_Command_Crypto_SetDataAddresses(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t InputDataAddress,
        const uint32_t InputDataLengthInBytes,
        const uint64_t OutputDataAddress,
        const uint32_t OutputDataLengthInBytes)
{
    CommandToken_p->W[3] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[5] = InputDataLengthInBytes;
    CommandToken_p->W[6] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[7] = (uint32_t)(OutputDataAddress >> 32);
    CommandToken_p->W[8] = OutputDataLengthInBytes;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_AES_SetKeyLength
 *
 * This function sets the coded length of the AES key.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * KeyLengthInBytes
 *     Key length.
 */
void
Eip130Token_Command_Crypto_AES_SetKeyLength(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t KeyLengthInBytes)
{
    uint8_t CodedKeyLen = 0;

    // key length only needed for AES
    switch (KeyLengthInBytes)
    {
    case (128 / 8):
        CodedKeyLen = 1;
        break;

    case (192 / 8):
        CodedKeyLen = 2;
        break;

    case (256 / 8):
        CodedKeyLen = 3;
        break;

    default:
        break;
    }
    CommandToken_p->W[11] |= (CodedKeyLen << 16);
}


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_ChaCha20_SetKeyLength
 *
 * This function sets the coded length of the ChaCha20 key.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * KeyLengthInBytes
 *     Key length.
 */
void
Eip130Token_Command_Crypto_ChaCha20_SetKeyLength(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t KeyLengthInBytes)
{
    uint8_t CodedKeyLen = 0;

    // Key length for ChaCha20
    if (KeyLengthInBytes == (128 / 8))
    {
        CodedKeyLen = 1;
    }
    CommandToken_p->W[11] |= (CodedKeyLen << 16);
}
#endif


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetASLoadKey
 *
 * This function sets the Asset Store Load location for the key and activates
 * its use. This also disables the use of the key via the token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *     Key Asset Identifier.
 */
void
Eip130Token_Command_Crypto_SetASLoadKey(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[11] |= BIT_8;
    CommandToken_p->W[17]  = AssetId;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetASLoadIV
 *
 * This function sets the Asset Store Load location for the IV and
 * activates its use. This also disables the use of the IV via the token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *     Input IV Asset Identifier.
 */
void
Eip130Token_Command_Crypto_SetASLoadIV(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[11] |= BIT_9;
    CommandToken_p->W[13]  = AssetId;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetASSaveIV
 *
 * This function sets the Asset Store Save location for the IV and
 * activates its use.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *     Output IV Asset Identifier.
 */
void
Eip130Token_Command_Crypto_SetASSaveIV(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[11] |= BIT_12;
    CommandToken_p->W[12]  = AssetId;
}


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetGcmMode
 *
 * This function sets the GCM operation mode to use.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * GCMMode
 *     GCM operation mode.
 */
void
Eip130Token_Command_Crypto_SetGcmMode(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t GCMMode)
{
    CommandToken_p->W[11] |= (GCMMode << 13);
}
#endif


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetNonceLength
 *
 * This function sets the Nonce length for ChaCha20.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * NonceLengthInBytes
 *     Nonce length.
 */
void
Eip130Token_Command_Crypto_SetNonceLength(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t NonceLengthInBytes)
{
    CommandToken_p->W[11] |= ((MASK_4_BITS & NonceLengthInBytes) << 20);
}
#endif


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_CopyKey
 *
 * This function copies the key from the buffer provided by the caller into
 * the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Key_p
 *     Pointer to the key buffer.
 *
 * KeyLengthInBytes
 *     Key length.
 */
void
Eip130Token_Command_Crypto_CopyKey(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const Key_p,
        const uint32_t KeyLengthInBytes)
{
    Eip130Token_Command_WriteByteArray(CommandToken_p, 17,
                                       Key_p, KeyLengthInBytes);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_CopyIV
 *
 * This function copies the IV from the buffer provided by the caller into
 * the command token. The IV length is always 16 bytes.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * IV_p
 *     Pointer to the IV buffer.
 */
void
Eip130Token_Command_Crypto_CopyIV(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const IV_p)
{
    Eip130Token_Command_WriteByteArray(CommandToken_p, 13, IV_p, 16);
}


#if defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20)
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_SetADAddress
 *
 * This function sets the Associated Data address and length provided by the
 * caller into the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * InputDataAddress
 *     Input data address of the Associated Data (AAD).
 *
 * DataLengthInBytes
 *     Associated Data length.
 */
void
Eip130Token_Command_Crypto_SetADAddress(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t InputDataAddress,
        const uint16_t DataLengthInBytes)
{
    CommandToken_p->W[9]  = (uint32_t)(InputDataAddress);
    CommandToken_p->W[10] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[25] = DataLengthInBytes;
}
#endif


#if defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20)
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_CopyNonce
 *
 * This function copies the Nonce from the buffer provided by the caller into
 * the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Nonce_p
 *     Pointer to the nonce buffer.
 *
 * NonceLengthInBytes
 *     Nonce length.
 */
void
Eip130Token_Command_Crypto_CopyNonce(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const Nonce_p,
        const uint32_t NonceLengthInBytes)
{
    CommandToken_p->W[11] |= ((MASK_4_BITS & NonceLengthInBytes) << 20);

    Eip130Token_Command_WriteByteArray(CommandToken_p, 29,
                                       Nonce_p, NonceLengthInBytes);
}
#endif


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_CopyHashKey
 *
 * This function copies the Hash Key from the buffer provided by the caller
 * into the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * HashKey_p
 *     Pointer to the hash key buffer.
 *
 * HashKeyLengthInBytes
 *     Hash key length.
 */
void
Eip130Token_Command_Crypto_CopyHashKey(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const HashKey_p,
        const uint32_t HashKeyLengthInBytes)
{
    Eip130Token_Command_WriteByteArray(CommandToken_p, 26,
                                       HashKey_p, HashKeyLengthInBytes);
}
#endif


#if defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20)
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_CopyTag
 *
 * This function copies the Tag from the buffer provided by the caller into
 * the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Tag_p
 *     Pointer to the TAG buffer.
 *
 * TagLengthInBytes
 *     TAG length.
 */
void
Eip130Token_Command_Crypto_CopyTag(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const Tag_p,
        const uint32_t TagLengthInBytes)
{
    CommandToken_p->W[11] |= ((MASK_5_BITS & TagLengthInBytes) << 24);

    if ((CommandToken_p->W[11] & BIT_15) == 0)
    {
        // Decrypt operation, so tag is input
        Eip130Token_Command_WriteByteArray(CommandToken_p, 33,
                                           Tag_p, TagLengthInBytes);
    }
}
#endif


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_AES_F8
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_Copyf8SaltKey
 *
 * This function copies the f8 salt key from the buffer provided by the caller
 * into the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Key_p
 *     Pointer to the f8 salt key buffer.
 *
 * KeyLengthInBytes
 *     The f8 salt key length.
 */
void
Eip130Token_Command_Crypto_Copyf8SaltKey(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const Key_p,
        const uint32_t KeyLengthInBytes)
{
    CommandToken_p->W[11] |= ((MASK_5_BITS & KeyLengthInBytes) << 24);

    Eip130Token_Command_WriteByteArray(CommandToken_p, 29,
                                       Key_p, KeyLengthInBytes);
}
#endif


#ifdef EIP130TOKEN_ENABLE_SYM_ALGO_AES_F8
/*----------------------------------------------------------------------------
 * Eip130Token_Command_Crypto_Copyf8IV
 *
 * This function copies the f8 IV from the buffer provided by the caller into
 * the command token. The f8 IV length is always 16 bytes.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * f8IV_p
 *     Pointer to the f8 IV buffer.
 */
void
Eip130Token_Command_Crypto_Copyf8IV(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const f8IV_p)
{
    Eip130Token_Command_WriteByteArray(CommandToken_p, 25, f8IV_p, 16);
}
#endif


/*----------------------------------------------------------------------------
 * Eip130Token_Result_Crypto_CopyIV
 *
 * This function copies the IV from the result token to the buffer
 * provided by the caller. The IV length is always 16 bytes.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * Dest_p
 *      Pointer to the buffer to copy the IV to.
 */
void
Eip130Token_Result_Crypto_CopyIV(
        const Eip130Token_Result_t * const ResultToken_p,
        uint8_t * Dest_p)
{
    Eip130Token_Result_ReadByteArray(ResultToken_p, 2, 16, Dest_p);
}


#if defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM) || \
    defined(EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20)
/*----------------------------------------------------------------------------
 * Eip130Token_Result_Crypto_CopyTag
 *
 * This function copies the Tag from the result token to the buffer
 * provided by the caller. The Tag length is always 16 bytes.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * Dest_p
 *      Pointer to the buffer to copy the Tag to.
 */
void
Eip130Token_Result_Crypto_CopyTag(
        const Eip130Token_Result_t * const ResultToken_p,
        uint8_t * Dest_p)
{
    Eip130Token_Result_ReadByteArray(ResultToken_p, 6, 16, Dest_p);
}
#endif




