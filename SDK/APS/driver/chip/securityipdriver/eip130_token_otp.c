#include "eip130_token_otp.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_OTPDataWrite
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetNumber
 *      Asset number of the Static Asset to write.
 *
 * PolicyNumber
 *      Policy reference number for this Static Asset.
 *
 * fCRC
 *      Indication that the Asset data must protected with a CRC.
 *      Note: Must be false for Monotonic Counter initial data.
 *
 * InputDataAddress
 *      Input data address of the key blob with the Asset data.
 *
 * InputDataLengthInBytes
 *      Input data length being the size of the key blob.
 *
 * AssociatedData_p
 *      Pointer to the Associated Data needed for key blob unwrapping.
 *
 * AssociatedDataSizeInBytes
 *      Size of the Associated Data in bytes.
 */
void
Eip130Token_Command_OTPDataWrite(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t AssetNumber,
        const uint16_t PolicyNumber,
        const bool fCRC,
        const uint64_t InputDataAddress,
        const uint16_t InputDataLengthInBytes,
        const uint8_t * const AssociatedData_p,
        const uint32_t AssociatedDataSizeInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_OTPDATAWRITE << 28);
    CommandToken_p->W[2] = (AssetNumber & MASK_5_BITS) |
                           ((PolicyNumber & MASK_5_BITS) << 16);
    if (fCRC)
    {
        CommandToken_p->W[2] |= BIT_31;
    }
    CommandToken_p->W[3] = (InputDataLengthInBytes & MASK_10_BITS) |
                           ((AssociatedDataSizeInBytes & MASK_10_BITS) << 16);
    CommandToken_p->W[4]  = (uint32_t)(InputDataAddress);
    CommandToken_p->W[5]  = (uint32_t)(InputDataAddress >> 32);

    Eip130Token_Command_WriteByteArray(CommandToken_p, 6,
                                       AssociatedData_p,
                                       AssociatedDataSizeInBytes);
}

/*----------------------------------------------------------------------------
 * Eip130Token_Command_OTPSelectZeroize
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_OTPSelectZeroize(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_SELECTOTPZERO << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_OTPSelectZeroize
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_OTPZeroize(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SERVICE << 24) |
                           (EIP130TOKEN_SUBCODE_ZEROIZEOTP << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_OTPMonotonicCounter_Read
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * AssetID
 *      ID of Asset of the monotonic counter to read
 *
 * DataAddress
 *      Output data address
 *
 * DataLengthInBytes
 *      Output data length
 *      Must be a multiple of 4.
 */
void
Eip130Token_Command_OTPMonotonicCounter_Read(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetID,
        const uint64_t DataAddress,
        const uint32_t DataLengthInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_MONOTONICREAD << 28);
    CommandToken_p->W[2] = AssetID;
    CommandToken_p->W[3] = DataLengthInBytes;
    CommandToken_p->W[4] = (uint32_t)(DataAddress);
    CommandToken_p->W[5] = (uint32_t)(DataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_OTPMonotonicCounter_Read
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * DataLengthInBytes_p
 *      Pointer to the variable in which the data length must be returned.
 */
void
Eip130Token_Result_OTPMonotonicCounter_Read(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const DataLengthInBytes_p)
{
    *DataLengthInBytes_p = ResultToken_p->W[1] & MASK_10_BITS;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_OTPMonotonicCounter_Increment
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * AssetID
 *      ID of Asset of the monotonic counter to read
 */
void
Eip130Token_Command_OTPMonotonicCounter_Increment(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetID)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_MONOTONICINCR << 28);
    CommandToken_p->W[2] = AssetID;
}

/*----------------------------------------------------------------------------
 * Eip130Token_Command_ProvisionRandomHUK
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetNumber
 *      Asset number of the Static Asset to write.
 *
 * f128bit
 *      Indication that the Asset data size is 128 bit instead of 256 bit.
 *      Thus if not set, the Asset data size is 256 bit.
 *
 * fCRC
 *      Indication that the Asset data must protected with a CRC.
 *
 * AutoSeed
 *     Setting that defines the automatic reseed after <AutoSeed> times
 *     64K Bytes of DRBG random generation.
 *
 * SampleCycles
 *     Setting that controls the number of (XOR-ed) FRO samples XOR-ed
 *     together to generate a single ‘noise?bit. This value must be
 *     set such that the total amount of entropy in 8 ‘noise?bits
 *     equals at least 1 bit.
 *
 * SampleDiv
 *     Setting that controls the number of module clock cycles between
 *     samples taken from the FROs.
 *
 * NoiseBlocks
 *     Setting that defines number of 512 bit ‘noise?blocks to process
 *     through the SHA-256 Conditioning function to generate a single
 *     256 bits ‘full entropy?result for (re-)seeding the DRBG.
 *
 * OutputDataAddress
 *      Output buffer address in which the generated OTP key blob must be
 *      written.
 *
 * OutputDataLengthInBytes
 *      Output buffer size. Note when the size is zero, no OTP key blob
 *      generation is assumed.
 *
 * AssociatedData_p
 *      Pointer to the Associated Data needed for OTP key blob generation.
 *
 * AssociatedDataSizeInBytes
 *      Size of the Associated Data in bytes.
 */
void
Eip130Token_Command_ProvisionRandomHUK(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t AssetNumber,
        const bool f128bit,
        const bool fCRC,
        const uint8_t AutoSeed,
        const uint16_t SampleCycles,
        const uint8_t SampleDiv,
        const uint8_t NoiseBlocks,
        const uint64_t OutputDataAddress,
        const uint16_t OutputDataLengthInBytes,
        const uint8_t * const AssociatedData_p,
        const uint32_t AssociatedDataSizeInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_PROVISIONRANDOMHUK << 28);
    CommandToken_p->W[2] = (AssetNumber & MASK_5_BITS) |
                           ((AutoSeed & MASK_8_BITS) << 8);
    if (f128bit)
    {
        CommandToken_p->W[2] |= BIT_16;
    }
    else
    {
        CommandToken_p->W[2] |= BIT_17;
    }
    if (fCRC)
    {
        CommandToken_p->W[2] |= BIT_30;
    }
    CommandToken_p->W[3] = (NoiseBlocks) |
                           ((SampleDiv & MASK_4_BITS) << 8) |
                           (SampleCycles << 16);
    if (OutputDataLengthInBytes != 0)
    {
        CommandToken_p->W[2] |= BIT_31;
        CommandToken_p->W[4]  = (OutputDataLengthInBytes & MASK_10_BITS) |
                                ((AssociatedDataSizeInBytes & MASK_8_BITS) << 16);
        CommandToken_p->W[5]  = (uint32_t)(OutputDataAddress);
        CommandToken_p->W[6]  = (uint32_t)(OutputDataAddress >> 32);

        Eip130Token_Command_WriteByteArray(CommandToken_p, 7,
                                           AssociatedData_p,
                                           AssociatedDataSizeInBytes);
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_ProvisionRandomHUK
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * KeyBlobLengthInBytes_p
 *      Pointer to the variable in which the OTP key blob length must be
 *      returned.
 */
void
Eip130Token_Result_ProvisionRandomHUK(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const KeyBlobLengthInBytes_p)
{
    *KeyBlobLengthInBytes_p = ResultToken_p->W[1] & MASK_10_BITS;
}


