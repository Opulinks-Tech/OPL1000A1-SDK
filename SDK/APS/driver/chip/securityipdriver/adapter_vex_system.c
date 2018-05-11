/* adapter_vex_system.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the system services.
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
#include "log.h"

#include "adapter_vex_internal.h"   // API implementation
#include "eip130_token_system.h"    // Eip130Token_Command_System*()
#include "eip130_token_service.h"   // Eip130Token_Command_ZeroizeOutputMailbox()

#if 0
#ifdef VEX_ENABLE_FIRMWARE_HIBERATION
#include "adapter_bufmanager.h"     // BufManager_*()
#endif
#endif //#if 0

/*----------------------------------------------------------------------------
 * vex_System
 */
VexStatus_t
vex_System(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

#if 0
#ifdef VEX_ENABLE_FIRMWARE_HIBERATION
    uint64_t DataBlobAddress = 0;
    bool fCopy = false;
#endif
#endif //#if 0

    uint16_t TokenID = vex_DeviceGetTokenID();
    int NrCommands = 1;

    // Format command token
    ZEROINIT(CommandToken);
    switch (CommandToken_p->SubCode)
    {
    case VEXTOKEN_SUBCODE_SYSTEMINFO:
        Eip130Token_Command_SystemInfo(&CommandToken);
        break;

    case VEXTOKEN_SUBCODE_SELFTEST:
        Eip130Token_Command_SystemSelfTest(&CommandToken);
        break;

    case VEXTOKEN_SUBCODE_RESET:
        Eip130Token_Command_SystemReset(&CommandToken);
        NrCommands = 2;                 // Perform additional verify command
        break;

#if 0
    case VEXTOKEN_SUBCODE_DEFINEUSERS:
        Eip130Token_Command_SystemDefineUsers(&CommandToken,
                                              CommandToken_p->Service.SystemDefineUsers.User1,
                                              CommandToken_p->Service.SystemDefineUsers.User2,
                                              CommandToken_p->Service.SystemDefineUsers.User3,
                                              CommandToken_p->Service.SystemDefineUsers.User4);
        break;
#endif

#if 0
#ifdef VEX_ENABLE_FIRMWARE_SLEEP
    case VEXTOKEN_SUBCODE_SLEEP:
    case VEXTOKEN_SUBCODE_RESUMEFROMSLEEP:
        {
            int rc;

            if (CommandToken_p->SubCode == VEXTOKEN_SUBCODE_SLEEP)
            {
                rc = vex_DeviceSleep();
            }
            else
            {
                rc = vex_DeviceResumeFromSleep();
            }
            switch (rc)
            {
            default:
                if (rc < -256)
                {
                    ResultToken_p->Result = rc + 256;
                }
                else
                {
                    return VEX_INTERNAL_ERROR;
                }
                // Successful but firmware error
            case 0:
                break;
            case -1:
                return VEX_UNSUPPORTED;
            case -2:
                return VEX_OPERATION_NOT_ALLOWED;
            case -3:
                return VEX_OPERATION_FAILED;
            case -4:
                return VEX_POWER_STATE_ERROR;
            }
            return VEX_SUCCESS;
        }
#endif
#endif


#if 0
#ifdef VEX_ENABLE_FIRMWARE_HIBERATION
    case VEXTOKEN_SUBCODE_HIBERNATION:
/*
        DataBlobAddress = BufManager_Map(CommandToken_p->fFromUserSpace,
                                         BUFMANAGER_BUFFERTYPE_OUT,
                                         CommandToken_p->Service.SystemHibernation.DataBlob_p,
                                         CommandToken_p->Service.SystemHibernation.DataBlobSize,
                                         (void *)&TokenID);
        if (DataBlobAddress == 0)
        {
            return VEX_NO_MEMORY;
        }

        Eip130Token_Command_SystemHibernation(&CommandToken,
                                              DataBlobAddress,
                                              BufManager_GetSize(DataBlobAddress));
*/
        DataBlobAddress = (uint64_t) CommandToken_p->Service.SystemHibernation.DataBlob_p;
        Eip130Token_Command_SystemHibernation(&CommandToken,
                                              DataBlobAddress,
                                              CommandToken_p->Service.SystemHibernation.DataBlobSize);


        
        Eip130Token_Command_SystemHibernationInfomation(&CommandToken,
                                                        CommandToken_p->Service.SystemHibernation.StateAssetId,
                                                        CommandToken_p->Service.SystemHibernation.KeyAssetId,
                                                        CommandToken_p->Service.SystemHibernation.AssociatedData,
                                                        CommandToken_p->Service.SystemHibernation.AssociatedDataSize);

        break;

    case VEXTOKEN_SUBCODE_RESUMEFROMHIBERNATION:
/*
        // Convert input buffer and get address for token
        DataBlobAddress = BufManager_Map(CommandToken_p->fFromUserSpace,
                                         BUFMANAGER_BUFFERTYPE_IN,
                                         CommandToken_p->Service.SystemHibernation.DataBlob_p,
                                         CommandToken_p->Service.SystemHibernation.DataBlobSize,
                                         NULL);
        if (DataBlobAddress == 0)
        {
            return VEX_NO_MEMORY;
        }
*/
        DataBlobAddress = (uint64_t) CommandToken_p->Service.SystemHibernation.DataBlob_p;

        Eip130Token_Command_SystemResumeFromHibernation(&CommandToken,
                                                        DataBlobAddress,
                                                        CommandToken_p->Service.SystemHibernation.DataBlobSize);
        Eip130Token_Command_SystemHibernationInfomation(&CommandToken,
                                                        CommandToken_p->Service.SystemHibernation.StateAssetId,
                                                        CommandToken_p->Service.SystemHibernation.KeyAssetId,
                                                        CommandToken_p->Service.SystemHibernation.AssociatedData,
                                                        CommandToken_p->Service.SystemHibernation.AssociatedDataSize);
        break;
#endif
#endif //#if 0


    default:
        return VEX_UNSUPPORTED;
    }

    // Initialize result token
    ZEROINIT(ResultToken);

    for (; NrCommands > 0; NrCommands--)
    {
        Vex_Command_SetTokenID(&CommandToken, TokenID);

        // Exchange token with VaultIP
        funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
        if (funcres == VEX_SUCCESS)
        {
            ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
            if (ResultToken_p->Result >= 0)
            {
                // Success
                if (CommandToken_p->SubCode == VEXTOKEN_SUBCODE_SYSTEMINFO)
                {
                    ResultToken_p->Service.SystemInfo.Firmware.Major = (uint8_t)(ResultToken.W[1] >> 16);
                    ResultToken_p->Service.SystemInfo.Firmware.Minor = (uint8_t)(ResultToken.W[1] >> 8);
                    ResultToken_p->Service.SystemInfo.Firmware.Patch = (uint8_t)(ResultToken.W[1] >> 0);

                    ResultToken_p->Service.SystemInfo.Hardware.Major = (uint8_t)(ResultToken.W[2] >> 16);
                    ResultToken_p->Service.SystemInfo.Hardware.Minor = (uint8_t)(ResultToken.W[2] >> 8);
                    ResultToken_p->Service.SystemInfo.Hardware.Patch = (uint8_t)(ResultToken.W[2] >> 0);
                    ResultToken_p->Service.SystemInfo.Hardware.MemorySizeInBytes = (uint16_t)ResultToken.W[3];

                    ResultToken_p->Service.SystemInfo.OTP.ErrorLocation = (uint16_t)(ResultToken.W[5] & MASK_12_BITS);
                    ResultToken_p->Service.SystemInfo.OTP.ErrorCode = (uint8_t)((ResultToken.W[5] >> 12) & MASK_4_BITS);

                    ResultToken_p->Service.SystemInfo.Self.Identity = ResultToken.W[4];
                    ResultToken_p->Service.SystemInfo.Self.HostID = (uint8_t)((ResultToken.W[3] >> 16) & MASK_3_BITS);
                    ResultToken_p->Service.SystemInfo.Self.NonSecure = (uint8_t)((ResultToken.W[3] >> 19) & MASK_1_BIT);
                    ResultToken_p->Service.SystemInfo.Self.CryptoOfficer = (uint8_t)((ResultToken.W[3] >> 27) & MASK_1_BIT);
                    ResultToken_p->Service.SystemInfo.Self.Mode = (uint8_t)((ResultToken.W[3] >> 28) & MASK_4_BITS);
                }
                else if (CommandToken_p->SubCode == VEXTOKEN_SUBCODE_RESET)
                {
                    // Wait until reset is finished
                    Eip130Token_Command_ZeroizeOutputMailbox(&CommandToken);
                    TokenID = vex_DeviceGetTokenID();
                }

#if 0
#ifdef VEX_ENABLE_FIRMWARE_HIBERATION
                else if (CommandToken_p->SubCode == VEXTOKEN_SUBCODE_HIBERNATION)
                {
                    Eip130Token_Result_SystemHibernation_BlobSize(&ResultToken,
                                                                  &ResultToken_p->Service.SystemHibernation.DataBlobSize);
                    fCopy = true;
                }
#endif
#endif //#if 0

            }
        }
    }

#if 0
#ifdef VEX_ENABLE_FIRMWARE_HIBERATION
/*
    if (DataBlobAddress != 0)
    {
        if (BufManager_Unmap(DataBlobAddress, fCopy) != 0)
        {
            funcres = VEX_INTERNAL_ERROR;
        }
    }
*/
#endif
#endif //#if 0

    return funcres;
}


/* end of file adapter_vex_system.c */
