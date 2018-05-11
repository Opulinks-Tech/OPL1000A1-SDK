#include "eip130_token_random.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_TRNG_Configure
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AutoSeed
 *     Setting that defines the automatic reseed after <AutoSeed> times
 *     64K Bytes of DRBG random generation.
 *
 * SampleCycles
 *     Setting that controls the number of (XOR-ed) FRO samples XOR-ed
 *     together to generate a single ?oise?bit. This value must be
 *     set such that the total amount of entropy in 8 ?oise?bits
 *     equals at least 1 bit.
 *
 * SampleDiv
 *     Setting that controls the number of module clock cycles between
 *     samples taken from the FROs.
 *
 * NoiseBlocks
 *     Setting that defines number of 512 bit ?oise?blocks to process
 *     through the SHA-256 Conditioning function to generate a single
 *     256 bits ?ull entropy?result for (re-)seeding the DRBG.
 */
void
Eip130Token_Command_TRNG_Configure(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t  AutoSeed,
        const uint16_t SampleCycles,
        const uint8_t  SampleDiv,
        const uint8_t  NoiseBlocks)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_TRNG << 24) |
                           (EIP130TOKEN_SUBCODE_TRNGCONFIG << 28);
    CommandToken_p->W[2] = (AutoSeed << 8) | BIT_0/* LST */;
    CommandToken_p->W[3] = (SampleCycles << 16) |
                           ((SampleDiv & 0x0F) << 8) |
                            NoiseBlocks;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_PRNG_ReseedNow
 *
 * This function creates a command token that requests to reseed the TRNG.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_PRNG_ReseedNow(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_TRNG << 24) |
                           (EIP130TOKEN_SUBCODE_TRNGCONFIG << 28);
    CommandToken_p->W[2] = BIT_1;   // RRD = Reseed post-processor
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_RandomNumber_Generate
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * NumberLengthInBytes
 *     The number of random bytes to generate.
 *
 * OutputDataAddress
 *     DMA address of the buffer where the random number bytes will be written
 *     to. The size of the buffer must be an integer number of 32bit words,
 *     equal or larger than NumberLengthInBytes.
 *     Note: When WriteTokenID is used, one more 32bit word will be written.
 */
void
Eip130Token_Command_RandomNumber_Generate(
        Eip130Token_Command_t * const CommandToken_p,
        const uint16_t NumberLengthInBytes,
        const uint64_t OutputDataAddress)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_TRNG << 24) |
                           (EIP130TOKEN_SUBCODE_RANDOMNUMBER << 28);
    CommandToken_p->W[2] = NumberLengthInBytes;
    CommandToken_p->W[3] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(OutputDataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_RandomNumber_Generate
 *
 * This function extracts 'quality warning' details from a TRNG random number
 * generate result token.
 *
 * ResultToken_p
 *     Pointer to the result token buffer this function will read from.
 *
 * Return Value
 *     0    no warnings
 *     <0   Error code
 *     >0   the 5-bit Result code indicating some statistic anomaly in
 *          the generated random data.
 */
int
Eip130Token_Result_RandomNumber_Generate(
        Eip130Token_Result_t * const ResultToken_p)
{
    int rv = ResultToken_p->W[0] >> 24;
    if (rv != 0)
    {
        if (rv & BIT_7)
        {
            return -rv;
        }
        if ((rv & (BIT_6 | BIT_5)) != BIT_6)
        {
            return -rv;
        }
        rv = rv & MASK_5_BITS;
    }

    return rv;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_TRNG_PP_Verification
 *
 * This function initializes the TRNG post-processing verification token that
 * can be used to verify the SHA-2 Conditioning Function and AES-256 based
 * CTR-DRBG random number generator.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * Test
 *     Test to perform:
 *     0 - Conditioning Function with 12 bits repeating pattern
 *     1 - Conditioning Function with externally supplied ?oise?bits
 *     2 - DRBG data generation using known input data
 *
 * Pattern
 *     Repeating input pattern for the 'Conditioning Function with 12 bits
 *     repeating pattern' test. Please, read the VaultIP Firmware Reference
 *     Manual for details .
 *
 * Size
 *     The actual value of Size depends on the test that is performed:
 *     0 - Number of 384 bits V & Key output blocks to generate in range 1-255.
 *     1 - Number of 384 bits V & Key output blocks to generate in range 1-255;
 *         the number of 512 bit ?oise blocks?to read equals the value given
 *         here times the NoiseBlocks value in the TRNG configuration token.
 *     2 - Splits the DataSize field in separate sub-fields:
 *         Bits [3:0]   = Total number of 384 bits seed blocks to read (range
 *                        1-15). The first is used for an ?nstantiate
 *                        function, the others for a ?e-seed?function.
 *         Bits [7:4]   = Number of 128-bit DRBG output words to generate in a
 *                        single data block (values 0-15 = 1-16 words).
 *         Bits [11:8]  = Number of data blocks to generate before each re-seed
 *                        (values 0-14 = 1-15 blocks, value 15 = 0 blocks).
 *         Bits [15:12] = Number of data blocks to generate after the last
 *                        re-seed (or ?nstantiate?if bits [3:0] are value 1).
 *                        Values 0-15 generate 1-16 blocks.
 *         Note: Using these sub-fields, several test-scenarios can be
 *               executed. Value 0x0002 performs an ?nstantiate-generate-
 *               reseed-generate?sequence while value 0x1F02 performs an
 *               ?nstantiate-reseed-generate-generate?sequence.
 *
 * InputDataAddress
 *     DMA address of the buffer with the input data.
 *
 * OutputDataAddress
 *     DMA address of the buffer  in which the output data must be written.
 *
 * @return One of the ValStatus_t values.
 */
void
Eip130Token_Command_TRNG_PP_Verification(
        Eip130Token_Command_t * const CommandToken_p,
        const uint8_t Test,
        const uint16_t Pattern,
        const uint16_t Size,
        const uint64_t InputDataAddress,
        const uint64_t OutputDataAddress)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_TRNG << 24) |
                           (EIP130TOKEN_SUBCODE_VERIFYDRBG << 28);
    CommandToken_p->W[2] = ((Test & MASK_4_BITS) << 28) |
                           ((Pattern & MASK_12_BITS) << 16) |
                           Size;
    CommandToken_p->W[3] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(InputDataAddress >> 32);
    CommandToken_p->W[5] = (uint32_t)(OutputDataAddress);
    CommandToken_p->W[6] = (uint32_t)(OutputDataAddress >> 32);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_TRNG_HW_SelfTest
 *
 * This function initializes the TRNG hardware self-test verification token.
 * The self-test focuses on the following hardware-related functions:
 * - The NIST SP800-90B proposed ?epetition Count?test on the noise source.
 * - The NIST SP800-90B proposed ?daptive Proportion?tests (with 64 and 4K
 *   window sizes) on the noise source. and starts the True Random Number
 *   Generator (TRNG) and Deterministic Random Bit Generator (DRBG).
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * InputDataAddress
 *     DMA address of the buffer with the input data.
 *
 * SizeInBytes
 *     Size of the input data in bytes.
 *
 * RepCntCutoff
 *     ?utoff?value for the NIST SP800-90B ?epetition Count?test. The
 *     default setting for this value is 31, unless overruled here.
 *     Setting this value to zero disables the ?epetition Count?test.
 *
 * RepCntCount
 *     Initial counter value for the NIST SP800-90B ?epetition Count?test.
 *     Although it is possible to pre-load this counter, it will be reset to
 *     one (1) at the start of the test run.
 *
 * RepCntData
 *     Initial compare data value for the NIST SP800-90B ?epetition Count
 *     test. At the start of the test, this value indicates the last 8-bits
 *     ?oise sample?value detected for repetition counting.
 *
 * AdaptProp64Cutoff
 *     ?utoff?value for the NIST SP800-90B ?daptive Proportion?test with a
 *     64 ?oise samples?window. The default setting for this value is 51,
 *     unless overruled here. Setting this value to zero disables the
 *     ?daptive Proportion?test with a 64 ?oise samples?window.
 *
 * AdaptProp64Count
 *     Initial counter value for the NIST SP800-90B ?daptive Proportion?test
 *     with a 64 ?oise samples?window. Although it is possible to pre-load
 *     this counter, it will be reset to zero at the start of the test run.
 *
 * AdaptProp64Data
 *     Initial compare data value for the NIST SP800-90B ?daptive Proportion
 *     test with a 64 ?oise samples?window. Although this value is loaded
 *     into the HW compare register before the test run start, it will be
 *     overwritten immediately with the first 8-bits ?oise sample?in the
 *     test data stream.
 *
 * AdaptProp4kCutoff
 *     ?utoff?value for the NIST SP800-90B ?daptive Proportion?test with a
 *     4096 ?oise samples?window. The default setting for this value is 2240,
 *     unless overruled here. Setting this value to zero disables the ?daptive
 *     Proportion?test with a 4096 ?oise samples?window..
 *
 * AdaptProp4kCount
 *     Initial counter value for the NIST SP800-90B ?daptive Proportion?test
 *     with a 4096 ?oise samples?window. Although it is possible to pre-load
 *     this counter, it will be reset to zero at the start of the test run.
 *
 * AdaptProp4kData
 *     Initial compare data value for the NIST SP800-90B ?daptive Proportion
 *     test with a 4096 ?oise samples?window. Although this value is loaded
 *     into the HW compare register before the test run start, it will be
 *     overwritten immediately with the first 8-bits ?oise sample?in the
 *     test data stream.
 */
void
Eip130Token_Command_TRNG_HW_SelfTest(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t InputDataAddress,
        const uint16_t SizeInBytes,
        const uint8_t RepCntCutoff,
        const uint8_t RepCntCount,
        const uint8_t RepCntData,
        const uint8_t AdaptProp64Cutoff,
        const uint8_t AdaptProp64Count,
        const uint8_t AdaptProp64Data,
        const uint16_t AdaptProp4kCutoff,
        const uint16_t AdaptProp4kCount,
        const uint8_t AdaptProp4kData)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_TRNG << 24) |
                           (EIP130TOKEN_SUBCODE_VERIFYNRBG << 28);
    CommandToken_p->W[2] = SizeInBytes;
    CommandToken_p->W[3] = (uint32_t)(InputDataAddress);
    CommandToken_p->W[4] = (uint32_t)(InputDataAddress >> 32);
    if ((RepCntCutoff != 0) || (AdaptProp64Cutoff != 0) || (AdaptProp4kCutoff != 0))
    {
        CommandToken_p->W[2] |= BIT_29;
        CommandToken_p->W[5] = ((AdaptProp4kCutoff & MASK_12_BITS) << 16) |
                               ((AdaptProp64Cutoff & MASK_6_BITS) << 8) |
                               (RepCntCutoff & MASK_6_BITS);
    }
    if ((RepCntCount != 0) || (AdaptProp64Count != 0) || (AdaptProp4kCount != 0))
    {
        CommandToken_p->W[2] |= BIT_30;
        CommandToken_p->W[6] = ((AdaptProp4kCount & MASK_12_BITS) << 16) |
                               ((AdaptProp64Count & MASK_6_BITS) << 8) |
                               (RepCntCount & MASK_6_BITS);
    }
    if ((RepCntData != 0) || (AdaptProp64Data != 0) || (AdaptProp4kData != 0))
    {
        CommandToken_p->W[2] |= BIT_31;
        CommandToken_p->W[7] = (AdaptProp4kData << 16) |
                               (AdaptProp64Data << 8) |
                               (RepCntData);
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_TRNG_HW_SelfTest
 *
 * This function extracts the TRNG hardware self-test verification information.
 *
 * ResultToken_p
 *     Pointer to the result token buffer this function will read from.
 *
 * RepCntCutoff
 *     Pointer to a buffer where the ?utoff?value for the NIST SP800-90B
 *     ?epetition Count?test must be written, as present in the engine at
 *     the end of the test run.
 *
 * RepCntCount
 *     Pointer to a buffer where the counter value for the NIST SP800-90B
 *     ?epetition Count?test must be written, as present in the engine at
 *     the end of the test run.
 *
 * RepCntData
 *     Pointer to a buffer where the compare data value for the NIST SP800-90B
 *     ?epetition Count?test must be written, as present in the engine at
 *     the end of the test run.
 *
 * AdaptProp64Cutoff
 *     Pointer to a buffer where the ?utoff?value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 64 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp64Count
 *     Pointer to a buffer where the counter value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 64 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp64Data
 *     Pointer to a buffer where the compare data value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 64 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp64Fail
 *     Pointer to a buffer where the failure indication for the NIST SP800-90B
 *     ?daptive Proportion?test with a 64 ?oise samples?window must be
 *     written.
 *
 * AdaptProp4kCutoff
 *     Pointer to a buffer where the ?utoff?value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 4096 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp4kCount
 *     Pointer to a buffer where the counter value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 4096 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp4kData
 *     Pointer to a buffer where the compare data value for the NIST SP800-90B
 *     ?daptive Proportion?test with a 4096 ?oise samples?window must be
 *     written, as present in the engine at the end of the test run.
 *
 * AdaptProp4kFail
 *     Pointer to a buffer where the failure indication for the NIST SP800-90B
 *     ?daptive Proportion?test with a 4096 ?oise samples?window must be
 *     written.
 */
void
Eip130Token_Result_TRNG_HW_SelfTest(
        Eip130Token_Result_t * const ResultToken_p,
        uint8_t * const RepCntCutoff,
        uint8_t * const RepCntCount,
        uint8_t * const RepCntData,
        uint8_t * const AdaptProp64Cutoff,
        uint8_t * const AdaptProp64Count,
        uint8_t * const AdaptProp64Data,
        uint8_t * const AdaptProp64Fail,
        uint16_t * const AdaptProp4kCutoff,
        uint16_t * const AdaptProp4kCount,
        uint8_t * const AdaptProp4kData,
        uint8_t * const AdaptProp4kFail)
{
    *RepCntCutoff = (uint8_t)(ResultToken_p->W[1] & MASK_6_BITS);
    *RepCntCount = (uint8_t)(ResultToken_p->W[2] & MASK_6_BITS);
    *RepCntData = (uint8_t)ResultToken_p->W[3];
    *AdaptProp64Cutoff = (uint8_t)((ResultToken_p->W[1] >> 8) & MASK_6_BITS);
    *AdaptProp64Count = (uint8_t)((ResultToken_p->W[2] >> 8) & MASK_6_BITS);
    *AdaptProp64Data = (uint8_t)(ResultToken_p->W[3] >> 8);
    *AdaptProp64Fail = (uint8_t)((ResultToken_p->W[3] >> 30) & MASK_1_BIT);
    *AdaptProp4kCutoff = (uint16_t)((ResultToken_p->W[1] >> 16) & MASK_12_BITS);
    *AdaptProp4kCount = (uint16_t)((ResultToken_p->W[2] >> 16) & MASK_12_BITS);
    *AdaptProp4kData = (uint8_t)(ResultToken_p->W[3] >> 16);
    *AdaptProp4kFail = (uint8_t)((ResultToken_p->W[3] >> 31) & MASK_1_BIT);
}


