#include "eip130_token_publicdata.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Publicdata_Read
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * AssetID
 *      ID of Asset of the public data object to read
 *
 * DataAddress
 *      Output data address
 *
 * DataLengthInBytes
 *      Output data length
 *      Must be a multiple of 4.
 */
void
Eip130Token_Command_PublicData_Read(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetID,
        const uint64_t DataAddress,
        const uint32_t DataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_PUBLICDATA << 28);
    CommandToken_p->W[2] = AssetID;
    CommandToken_p->W[3] = DataLengthInBytes;
    CommandToken_p->W[4] = (uint32_t)(DataAddress);
    CommandToken_p->W[5] = (uint32_t)(DataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_Publicdata_Read
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * DataLengthInBytes_p
 *      Pointer to the variable in which the data length must be returned.
 */
void
Eip130Token_Result_Publicdata_Read(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const DataLengthInBytes_p)
{
    *DataLengthInBytes_p = ResultToken_p->W[1] & MASK_10_BITS;
}


