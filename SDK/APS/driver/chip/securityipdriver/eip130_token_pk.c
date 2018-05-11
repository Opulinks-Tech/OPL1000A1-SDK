#include "eip130_token_pk.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Pk_Asset_Command
 *
 * Request to perform a PK operation with assets
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Command
 *      Public Key operation (command) to perform.
 *
 * Nwords
 *      Number of words of the basic vector.
 *
 * Mwords
 *      Number of words of the alternate (shorter) vector.
 *
 * OtherLen
 *      Length of the other data in the token.
 *
 * KeyAssetId
 *     Asset ID of the main key asset.
 *
 * ParamAssetId (optional)
 *     Asset ID of the domain or curve parameters asset.
 *
 * IOAssetId (optional)
 *     Asset ID of the input and/or output of result asset.
 *
 * InputDataAddress
 *      Input data address.
 *
 * InputDataLengthInBytes
 *      Input data length.
 *
 * OutputDataAddress
 *      Output data address.
 *
 * OutputDataLengthInBytes
 *      Output data length.
 */
void
Eip130Token_Command_Pk_Asset_Command(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Command,
        const uint8_t Nwords,
        const uint8_t Mwords,
        const uint8_t OtherLen,
        const uint32_t KeyAssetId,
        const uint32_t ParamAssetId,
        const uint32_t IOAssetId,
        const uint64_t InputDataAddress,
        const uint16_t InputDataLengthInBytes,
        const uint64_t OutputDataAddress,       // or Signature address
        const uint16_t OutputDataLengthInBytes) // or Signature length
{
    CommandToken_p->W[0]  = (EIP130TOKEN_OPCODE_PUBLIC_KEY << 24) |
                            (EIP130TOKEN_SUBCODE_PK_WITHASSETS << 28);
    CommandToken_p->W[2]  = Command |   // PK operation to perform
                            (Nwords << 16) |
                            (Mwords << 24);
    CommandToken_p->W[3]  = (OtherLen << 8);
    CommandToken_p->W[4]  = KeyAssetId; // asset containing x and y coordinates of pk
    CommandToken_p->W[5]  = ParamAssetId; // public key parameters:
                                          // p, a, b, n, base x, base y[, h]
    CommandToken_p->W[6]  = IOAssetId;
    CommandToken_p->W[7]  = ((MASK_12_BITS & OutputDataLengthInBytes) << 16 ) |
                             (MASK_12_BITS & InputDataLengthInBytes);
    CommandToken_p->W[8]  = (uint32_t)(InputDataAddress);
    CommandToken_p->W[9]  = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[10] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[11] = (uint32_t)(OutputDataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Pk_Asset_SetAdditional
 *
 * This function copies the additional data from the buffer provided by the
 * caller into the command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AddData_p
 *      Additional input data address
 *
 * AddDataLengthInBytes
 *      Additional input data length
 */
void
Eip130Token_Command_Pk_Asset_SetAdditional(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const AddData_p,
        const uint8_t AddDataLengthInBytes)
{
    uint8_t offset = ((CommandToken_p->W[3] & 0xFF) + 3) & ~3;

    CommandToken_p->W[3] &= ~0xFF;
    CommandToken_p->W[3] |= (offset + AddDataLengthInBytes);
    Eip130Token_Command_WriteByteArray(CommandToken_p, ((offset/4) + 12),
                                       AddData_p,
                                       (uint32_t)AddDataLengthInBytes);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Pk_Claim
 *
 * Request to claim the PKA engine for PK operations without assets
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Nwords
 *      Number of words of the basic vector for NumSetN.
 *
 * Mwords
 *      Number of words of the alternate (shorter) vector for NumSetN.
 *
 * Mmask
 *      Bitmask specifying (with 1b’s) which of the first 8 vectors in Number
 *      Array are Mwords long for NumSetN.
 */
void
Eip130Token_Command_Pk_Claim(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Nwords,
        const uint8_t Mwords,
        const uint8_t Mmask)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_PUBLIC_KEY << 24) |
                           (EIP130TOKEN_SUBCODE_PK_NOASSETS << 28);
    CommandToken_p->W[2] = EIP130TOKEN_PK_CMD_NUMSETN |
                          (Mmask << 8) |
                          (Nwords << 16) |
                          (Mwords << 24);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Pk_NumLoad
 *
 * Request to load vectors for PK operations without assets
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Index
 *     Selects the location(s) in which to store the input vector(s) for
 *     NumLoad.
 *
 * InputDataAddress
 *      Input data address.
 *
 * InputDataLengthInBytes
 *      Input data length.
 */
void
Eip130Token_Command_Pk_NumLoad(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Index,
        const uint64_t InputDataAddress,
        const uint32_t InputDataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_PUBLIC_KEY << 24) |
                           (EIP130TOKEN_SUBCODE_PK_NOASSETS << 28);
    CommandToken_p->W[2] = EIP130TOKEN_PK_CMD_NUMLOAD |
                          ((MASK_4_BITS & Index) << 24);
    CommandToken_p->W[5] = (MASK_12_BITS & InputDataLengthInBytes);
    CommandToken_p->W[6] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[7] = (uint32_t)(InputDataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_ECDSA_Verify
 *
 * Request to perform a PK operations without assets
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Command
 *      Public Key operation (command) to perform.
 *
 * PublicExponent
 *      Specifies exponent for RSA public key operation.
 *
 * InputDataAddress
 *      Input data address.
 *
 * InputDataLengthInBytes
 *      Input data length.
 *
 * OutputDataAddress
 *      Output data address.
 *
 * OutputDataLengthInBytes
 *      Output data length.
 */
void
Eip130Token_Command_Pk_Operation(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Command,
        const uint32_t PublicExponent,
        const uint64_t InputDataAddress,
        const uint32_t InputDataLengthInBytes,
        const uint64_t OutputDataAddress,
        const uint32_t OutputDataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_PUBLIC_KEY << 24) |
                           (EIP130TOKEN_SUBCODE_PK_NOASSETS << 28);
    CommandToken_p->W[2] = (MASK_5_BITS & Command); // PK operation to perform
    CommandToken_p->W[3] = PublicExponent;
    CommandToken_p->W[5] = ((OutputDataLengthInBytes & MASK_12_BITS) << 16) |
                           (InputDataLengthInBytes & MASK_12_BITS);
    CommandToken_p->W[6] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[7] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[8] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[9] = (uint32_t)(OutputDataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Pk_Relaese
 *
 * Request to release the PKA engine for PK operations without assets
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_Pk_Release(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_PUBLIC_KEY << 24) |
                           (EIP130TOKEN_SUBCODE_PK_NOASSETS << 28);
    CommandToken_p->W[2] = EIP130TOKEN_PK_CMD_NUMSETN;
                                        // Note: Mmask M/Nwords are zeroized
}


