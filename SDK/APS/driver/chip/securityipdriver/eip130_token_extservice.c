#include "eip130_token_extservice.h"


#if 0
/*----------------------------------------------------------------------------
 * Eip130Token_Command_ExtService
 *
 * This function initializes the External Service token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * MacAlgo
 *     Mac algorithm selection. Use one of EIP130TOKEN_MAC_ALGORITHM_*.
 *
 * InputDataAddress
 *     Address of data to be MACed.
 *
 * InputDataLengthInBytes
 *     Size/Length of the data block to be MACed.
 */
void
Eip130Token_Command_ExtService(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t MacAlgo,
        const uint64_t InputDataAddress,
        const uint32_t InputDataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_EXT_SERVICE << 24);
    CommandToken_p->W[2] = InputDataLengthInBytes;
    CommandToken_p->W[3] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[5] = InputDataLengthInBytes;
    CommandToken_p->W[6] = (MASK_4_BITS & MacAlgo);
    CommandToken_p->W[24] = (uint32_t)(InputDataLengthInBytes);
    CommandToken_p->W[25] = 0;
}

/*----------------------------------------------------------------------------
 * Eip130Token_Command_ExtService_SetKeyAssetIDAndKeyLength
 *
 * This function sets the KEK Asset ID.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *     Asset ID of the KEK asset.
 *
 * KeyLengthInBytes
 *     The size of the key.
 */
void
Eip130Token_Command_ExtService_SetKeyAssetIDAndKeyLength(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId,
        const uint32_t KeyLengthInBytes)
{
    CommandToken_p->W[6] |= ((MASK_7_BITS & KeyLengthInBytes) << 16);
    CommandToken_p->W[7] = AssetId;
}

/*----------------------------------------------------------------------------
 * Eip130Token_Command_ExtService_SetStateAssetID
 *
 * This function sets the eMMC State Asset ID.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId
 *     Asset ID of the eMMC State asset.
 */
void
Eip130Token_Command_ExtService_SetStateAssetID(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId)
{
    CommandToken_p->W[28] = AssetId;
}

/*----------------------------------------------------------------------------
 * Eip130Token_Command_ExtService_SetAssociatedData
 *
 * This function sets associated data for the keyblob import functionality.
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
Eip130Token_Command_ExtService_SetAssociatedData(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t * const AssociatedData_p,
        const uint32_t AssociatedDataSizeInBytes)
{
    CommandToken_p->W[29] |= (AssociatedDataSizeInBytes << 16);

    Eip130Token_Command_WriteByteArray(CommandToken_p, 30,
                                       AssociatedData_p,
                                       AssociatedDataSizeInBytes);
}

/*----------------------------------------------------------------------------
 * Eip130Token_Result_ExtService_CopyMAC
 *
 * This function copies the MAC from the result token to the buffer provided
 * by the caller. The requested number of bytes are copied (length depends on
 * the algorithm that was used).
 *
 * The MAC is written to the destination buffer, Byte 0 first.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * MACLenInBytes
 *     The size of the MAC to copy.
 *
 * MAC_p
 *     Pointer to the MAC buffer.
 */
void
Eip130Token_Result_ExtService_CopyMAC(
        Eip130Token_Result_t * const ResultToken_p,
        const uint32_t MACLenInBytes,
        uint8_t * MAC_p)
{
    Eip130Token_Result_ReadByteArray(ResultToken_p, 2, MACLenInBytes, MAC_p);
}
#endif


