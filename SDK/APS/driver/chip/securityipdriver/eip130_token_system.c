#include "eip130_token_system.h"


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemInfo
 *
 * This function writes the System Information command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_SystemInfo(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_SYSTEMINFO  << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_SystemInfo
 *
 * This function parses the System Information result token. It can also
 * be used to query the (fixed) length of the firmware versions string this
 * function can generate.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * Info_p
 *     Pointer to the information structure that this function will populate.
 */
void
Eip130Token_Result_SystemInfo(
        const Eip130Token_Result_t * const ResultToken_p,
        Eip130Token_SystemInfo_t * const Info_p)
{
    uint32_t MaMiPa;

    memset(Info_p, 0, sizeof(Eip130Token_SystemInfo_t));

    MaMiPa = ResultToken_p->W[1];
    Info_p->Firmware.Major = (uint8_t)(MaMiPa >> 16);
    Info_p->Firmware.Minor = (uint8_t)(MaMiPa >> 8);
    Info_p->Firmware.Patch = (uint8_t)MaMiPa;
    if (MaMiPa & BIT_31)
    {
        Info_p->Firmware.fIsTestFW = true;
    }

    MaMiPa = ResultToken_p->W[2];
    Info_p->Hardware.Major = (uint8_t)(MaMiPa >> 16);
    Info_p->Hardware.Minor = (uint8_t)(MaMiPa >> 8);
    Info_p->Hardware.Patch = (uint8_t)MaMiPa;

    Info_p->Hardware.MemorySizeInBytes = (uint16_t)ResultToken_p->W[3];

    Info_p->SelfIdentity.HostID = MASK_4_BITS & (ResultToken_p->W[3] >> 16);
    Info_p->SelfIdentity.Identity = ResultToken_p->W[4];

    Info_p->OTP.ErrorCode = MASK_4_BITS & (ResultToken_p->W[5] >> 12);
    Info_p->OTP.ErrorLocation = MASK_12_BITS & ResultToken_p->W[5];
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemSelfTest
 *
 * This function writes the Self Test command token, with which the self test
 * can be selected manually and also the initiation of the transition to FIPS
 * mode.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_SystemSelfTest(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_SELFTEST  << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_Reset
 *
 * This function writes the Reset command token, with which a software wise
 * reset of the firmware can be performed.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_SystemReset(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_RESET  << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemDefineUsers
 *
 * This function writes the define users command token, with which FIPS mode
 * allowed users can be defined.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * User1/2/3/4
 *     Allowed user 1/2/3/4 identity value.
 */
void
Eip130Token_Command_SystemDefineUsers(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t User1,
        const uint32_t User2,
        const uint32_t User3,
        const uint32_t User4)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_DEFINEUSERS  << 28);
    CommandToken_p->W[2] = User1;       // User 1...
    CommandToken_p->W[3] = User2;
    CommandToken_p->W[4] = User3;
    CommandToken_p->W[5] = User4;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemSleep
 *
 * This function writes the Sleep command token, with which the VaultIP
 * (EIP130) is placed in sleep mode.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_SystemSleep(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_SLEEP  << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemResumeFromSleep
 *
 * This function writes the Resume From Sleep command token, with which the
 * VaultIP (EIP130) resumed to normal operation coming out of sleep mode.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 */
void
Eip130Token_Command_SystemResumeFromSleep(
        Eip130Token_Command_t * const CommandToken_p)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_RESUMEFROMSLEEP  << 28);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemHibernation
 *
 * This function writes the Hibernation command token, with which the VaultIP
 * (EIP130) is placed in hibernation mode.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * DataBlobAddress
 *      Data Blob address.
 *
 * DataBlobSizeInBytes
 *      Data Blob size.
 */
void
Eip130Token_Command_SystemHibernation(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t DataBlobAddress,
        const uint32_t DataBlobSizeInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_HIBERNATION  << 28);
    CommandToken_p->W[3] = BIT_31 | BIT_27;
    CommandToken_p->W[4] = 0;
    CommandToken_p->W[5] = 0;
    CommandToken_p->W[6] = (uint32_t)(DataBlobAddress);
    CommandToken_p->W[7] = (uint32_t)(DataBlobAddress >> 32);
    CommandToken_p->W[8] = (DataBlobSizeInBytes & MASK_11_BITS);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemResumeFromHibernation
 *
 * This function writes the Resume From Hibernation command token, with which
 * the VaultIP (EIP130) resumed to normal operation coming out of hibernation
 * mode.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * DataBlobAddress
 *      Data Blob address.
 *
 * DataBlobSizeInBytes
 *      Data Blob size.
 */
void
Eip130Token_Command_SystemResumeFromHibernation(
        Eip130Token_Command_t * const CommandToken_p,
        const uint64_t DataBlobAddress,
        const uint32_t DataBlobSizeInBytes)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_SYSTEM << 24) |
                           (EIP130TOKEN_SUBCODE_RESUMEFROMHIBERNATION  << 28);
    CommandToken_p->W[3] = BIT_26 | (DataBlobSizeInBytes & MASK_10_BITS);
    CommandToken_p->W[4] = (uint32_t)(DataBlobAddress);
    CommandToken_p->W[5] = (uint32_t)(DataBlobAddress >> 32);
    CommandToken_p->W[6] = 0;
    CommandToken_p->W[7] = 0;
    CommandToken_p->W[8] = 0;
}


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SystemHibernationInfomation
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * StateAssetId
 *      Asset ID of the State Asset to use.
 *
 * KeyAssetId
 *      Asset ID of the Key Encryption Key Asset to use.
 *
 * AssociatedData_p
 *      Associated Data address.
 *
 * AssociatedDataSizeInBytes
 *      Associated Data length.
 *
 */
void
Eip130Token_Command_SystemHibernationInfomation(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t StateAssetId,
        const uint32_t KeyAssetId,
        const uint8_t * const AssociatedData_p,
        const uint32_t AssociatedDataSizeInBytes)
{
    CommandToken_p->W[2] = StateAssetId;
    CommandToken_p->W[9] = KeyAssetId;  // Key Encryption Key

    CommandToken_p->W[3] |= (AssociatedDataSizeInBytes << 16);
    Eip130Token_Command_WriteByteArray(CommandToken_p, 10,
                                       AssociatedData_p,
                                       AssociatedDataSizeInBytes);
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_SystemHibernation_BlobSize
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * OutputSizeInBytes_p
 *      Pointer to the variable in which the blob size must be returned.
 */
void
Eip130Token_Result_SystemHibernation_BlobSize(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const OutputSizeInBytes_p)
{
    *OutputSizeInBytes_p = ResultToken_p->W[1] & MASK_10_BITS;
}


