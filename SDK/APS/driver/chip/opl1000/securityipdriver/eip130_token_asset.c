#include "eip130_token_asset.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetSearch
 *
 * Request to return the ID for a static asset with the given index.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetNumber
 *      Asset number of the Static Asset to search for.
 */
void
Eip130Token_Command_AssetSearch(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetNumber)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETSEARCH << 28);
    CommandToken_p->W[4] = (MASK_6_BITS & AssetNumber) << 16;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_AssetSearch
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * AssetId_p
 *      Pointer to the variable in which the AssetId must be returned.
 *
 * DataLength_p
 *      Optional pointer to the variable in which the data length must be
 *      returned.
 */
void
Eip130Token_Result_AssetSearch(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const AssetId_p,
        uint32_t * const DataLength_p)  // Optional
{
    *AssetId_p = ResultToken_p->W[1];

    if (DataLength_p)
    {
        *DataLength_p = (ResultToken_p->W[2] & MASK_10_BITS);
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetCreate
 *
 * Request to create an asset with the given policy and length.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Policy
 *      Policy of the Asset to create.
 *
 * LengthInBytes
 *      Length of the Asset in bytes.
 */
void
Eip130Token_Command_AssetCreate(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t Policy,
        const uint32_t LengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETCREATE << 28);
    CommandToken_p->W[2] = (Policy)&0xFFFFFFFF;
    CommandToken_p->W[3] = (Policy >> 32) & 0xFFFFFFFF;
    CommandToken_p->W[4] = (LengthInBytes & MASK_10_BITS);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_AssetCreate
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * AssetId_p
 *      Pointer to the variable in which the AssetId must be returned.
 */
void
Eip130Token_Result_AssetCreate(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const AssetId_p)
{
    *AssetId_p = ResultToken_p->W[1];
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetDelete
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to delete.
 */
void
Eip130Token_Command_AssetDelete(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETDELETE << 28);
    CommandToken_p->W[2] = AssetId;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_Derive
 *
 * Request to setup the target asset content by derivation.
 *
 * Notes:
 * - Use Eip130Token_Command_AssetLoad_SetAad to setup the additional data to
 *   be used as input for the key derivation process.
 * - Use Eip130Token_Command_AssetLoad_SetInput for Salt related information
 * - Use Eip130Token_Command_AssetLoad_SetOutput for IV related information
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to load (initialize).
 *
 * KdkAssetId
 *      Asset ID of the Key Derivation Key Asset to use.
 *
 * Counter
 *      When set, the key derivation is performed with the KDF in counter mode
 *      as defined in NIST SP800-108.
 *
 * RFC5869
 *      When set, the key derivation is performed as defined in RFC 5869.
 *      Note: When this bit is set in FIPS mode, an invalid parameter error is
 *      returned.
 */
void
Eip130Token_Command_AssetLoad_Derive(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId,
        const uint32_t KdkAssetId,
        const bool Counter,
        const bool RFC5869)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETLOAD << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = BIT_24;      // Derive
    if (Counter)
    {
        CommandToken_p->W[3] |= BIT_14; // Counter mode
    }
    if (RFC5869)
    {
        CommandToken_p->W[3] |= BIT_15; // RFC5869 method
    }
    CommandToken_p->W[4] = 0;
    CommandToken_p->W[5] = 0;
    CommandToken_p->W[6] = 0;
    CommandToken_p->W[7] = 0;
    CommandToken_p->W[8] = 0;
    CommandToken_p->W[9] = KdkAssetId;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_Random
 *
 * Request to setup the target asset content with random data. The asset
 * size was already specified when the asset was created.
 *
 * Notes:
 * - Use Eip130Token_Command_AssetLoad_Export to also request the export of
 *   the asset as key blob.
 * - Use Eip130Token_Command_AssetLoad_SetAad to setup the additional data to
 *   be needed for generating the key blob.
 * - Use Eip130Token_Command_AssetLoad_SetOutput for key blob data related
 *   information
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to load (initialize).
 */
void
Eip130Token_Command_AssetLoad_Random(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETLOAD << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = BIT_25;      // Random
    CommandToken_p->W[6] = 0;
    CommandToken_p->W[7] = 0;
    CommandToken_p->W[8] = 0;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_Import
 *
 * Request to setup the target asset content from a key blob.
 *
 * Notes:
 * - Use Eip130Token_Command_AssetLoad_SetAad to setup the additional data to
 *   be needed to read the key blob.
 * - Use Eip130Token_Command_AssetLoad_SetInput for key blob data related
 *   information
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to load (initialize).
 *
 * KekAssetId
 *      Asset ID of the Key Encryption Key Asset to use.
 */
void
Eip130Token_Command_AssetLoad_Import(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId,
        const uint32_t KekAssetId)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETLOAD << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = BIT_26;      // Import
    CommandToken_p->W[4] = 0;
    CommandToken_p->W[5] = 0;
    CommandToken_p->W[9] = KekAssetId;  // AES-SIV Key Encryption Key
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_PlainText
 *
 * Request to setup the target asset content from plain text.
 *
 * Notes:
 * - Use Eip130Token_Command_AssetLoad_Export to also request the export of
 *   the asset as key blob.
 * - Use Eip130Token_Command_AssetLoad_SetAad to setup the additional data to
 *   be needed for generating the key blob.
 * - Use Eip130Token_Command_AssetLoad_SetOutput for key blob data related
 *   information
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to load (initialize).
 */
void
Eip130Token_Command_AssetLoad_Plaintext(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETLOAD << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = BIT_27;     // Plaintext
    CommandToken_p->W[4] = 0;
    CommandToken_p->W[5] = 0;
    CommandToken_p->W[6] = 0;
    CommandToken_p->W[7] = 0;
    CommandToken_p->W[8] = 0;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_AesUnwrap
 *
 * Request to setup the target asset content from an AES key wrapped key blob.
 *
 * Notes:
 * - Use Eip130Token_Command_AssetLoad_SetInput for key blob data related
 *   information
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *      Asset ID of the Asset to load (initialize).
 *
 * KekAssetId
 *      Asset ID of the Key Encryption Key Asset to use.
 */
void
Eip130Token_Command_AssetLoad_AesUnwrap(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId,
        const uint32_t KekAssetId)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_ASSETLOAD << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = BIT_28;       // AES Unwrap
    CommandToken_p->W[4] = 0;
    CommandToken_p->W[5] = 0;
    CommandToken_p->W[9] = KekAssetId;  // AES Wrap Key Encryption Key
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_SetAad
 *
 * Setup additional data for
 * - AssetLoad Derive
 * - AssetLoad Unwrap / Import
 * - AssetLoad Plaintext / Generate with request to produce a keyblob ('Wrap')
 *
 * Minimum AAD length is enforced by this function by padding with zero bytes.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssociatedData_p
 *      Associated Data address.
 *
 * AssociatedDataSizeInBytes
 *      Associated Data length.
 */
void
Eip130Token_Command_AssetLoad_SetAad(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const AssociatedData_p,
        const uint32_t AssociatedDataSizeInBytes)
{
    CommandToken_p->W[3] |= (AssociatedDataSizeInBytes << 16);

    Eip130Token_Command_WriteByteArray(CommandToken_p, 10,
                                       AssociatedData_p,
                                       AssociatedDataSizeInBytes);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_SetInput
 *
 * Request to setup the asset related input data information.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * DataAddress
 *      Input Data address.
 *
 * DataLengthInBytes
 *      Input Data length.
 */
void
Eip130Token_Command_AssetLoad_SetInput(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t DataAddress,
        const uint32_t DataLengthInBytes)
{
    CommandToken_p->W[3] |= (DataLengthInBytes & MASK_10_BITS);
    CommandToken_p->W[4]  = (uint32_t)(DataAddress);
    CommandToken_p->W[5]  = (uint32_t)(DataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_SetOutput
 *
 * Request to setup the asset related output data information.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * DataAddress
 *      Output Data address.
 *
 * DataLengthInBytes
 *      Output Data length.
 */
void
Eip130Token_Command_AssetLoad_SetOutput(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t DataAddress,
        const uint32_t DataLengthInBytes)
{
    CommandToken_p->W[8] = (DataLengthInBytes & MASK_10_BITS);
    CommandToken_p->W[6] = (uint32_t)(DataAddress);
    CommandToken_p->W[7] = (uint32_t)(DataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_AssetLoad_Export
 *
 * Request to export an asset as key blob after its contents have been setup
 * with either random or plain text data.
 * Use Eip130Token_Command_AssetLoad_SetAad to setup the additional data to be
 * used when generating the key blob.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * KekAssetId
 *      Asset ID of the Key Encryption Key Asset to use.
 */
void
Eip130Token_Command_AssetLoad_Export(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t KekAssetId)
{
    CommandToken_p->W[3] |= BIT_31;     // KeyBlob
    CommandToken_p->W[9]  = KekAssetId; // AES-SIV Key Encryption Key
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_AssetLoad_OutputSize
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * OutputSizeInBytes_p
 *      Pointer to the variable in which the ouput size must be returned.
 */
void
Eip130Token_Result_AssetLoad_OutputSize(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const OutputSizeInBytes_p)
{
    *OutputSizeInBytes_p = ResultToken_p->W[1] & MASK_10_BITS;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_NeedsAppID
 *
 * Returns true when the provided token is an Asset Load token with the
 * Derive flag set.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
bool
Eip130Token_Command_NeedsAppID(
        const Eip130Token_Command_t * const CommandToken_p)
{
    if (((CommandToken_p->W[0] >> 24) & MASK_6_BITS) ==
        ((EIP130TOKEN_OPCODE_ASSETMANAGEMENT) | (EIP130TOKEN_SUBCODE_ASSETLOAD << 4)))
    {
        // Token = Asset Management - Asset Load
        if (CommandToken_p->W[3] & BIT_24)
        {
            // Derive command
            return true;
        }
    }
    return false;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_InsertAppID
 *
 * This function is called the insert the AppID at the start of the AAD area
 * in the AssetLoad command. This is needed when Eip130Token_Command_NeedsAppID
 * returns true.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_InsertAppID(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const AssociatedData_p,
        uint32_t AssociatedDataSizeInBytes)
{
    if (Eip130Token_Command_NeedsAppID(CommandToken_p))
    {
        unsigned int AADMax = (EIP130TOKEN_COMMAND_WORDS - 8) * 4; // Max AAD bytes in token
        unsigned int AADLen = (CommandToken_p->W[3] >> 16) & MASK_8_BITS;

        // ensure additional data by itself fits
        if (AssociatedDataSizeInBytes > AADMax)
        {
            AssociatedDataSizeInBytes = AADMax;
        }

        // calculate how much of the current AAD data can remain
        if (AADLen + AssociatedDataSizeInBytes > AADMax)
        {
             AADLen = AADMax - AssociatedDataSizeInBytes;
        }

        // move the current AAD data to make space for the new data
        // move is done on byte-array, assuming LSB-first
        {
            uint8_t * AAD_Src_p = (uint8_t *)(CommandToken_p->W + 8);
            uint8_t * AAD_Dst_p = AAD_Src_p + AssociatedDataSizeInBytes;
            unsigned int i;

            for (i = AADLen; i > 0; i--)
            {
                 AAD_Dst_p[i - 1] = AAD_Src_p[i - 1];
            }
        }

        // now write the new AAD data
        Eip130Token_Command_WriteByteArray(CommandToken_p, 10,
                                           AssociatedData_p,
                                           AssociatedDataSizeInBytes);

        // overwrite the length field
        // (do not try to update it due to size limiters above)
        AADLen += AssociatedDataSizeInBytes;
        CommandToken_p->W[3] &= ~(MASK_8_BITS << 16);
        CommandToken_p->W[3] |= (AADLen << 16);
    }
}


