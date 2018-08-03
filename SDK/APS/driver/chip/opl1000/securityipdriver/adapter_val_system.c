/* adapter_val_system.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
 *
 * This file implements the system services.
 */

/*****************************************************************************
* Copyright (c) 2014-2017 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include "c_adapter_val.h"              // configuration

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "api_val.h"                    // the API to implement
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t


/*----------------------------------------------------------------------------
 * val_SystemGetVersion
 */
#ifndef VAL_REMOVE_SYSTEM_GETVERSION
ValStatus_t
val_SystemGetVersion(
        ValOctetsOut_t * const Version_p,
        ValSize_t * const VersionSize_p)
{
    // Only single-digit major/minor/patch numbers are supported
    static const char gl_APIVersion[] = "VaultIP APIv" VAL_API_VERSION;
    static const char gl_DeviceVersionTemplate[] = "{HWv0.0p0 FWv0.0p0 0x0000 bytes}";
    //                                                        1         2         3
    //                                              01234567890123456789012345678901

    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    if (VersionSize_p == NULL)
    {
        return VAL_BAD_ARGUMENT;
    }

    if (*VersionSize_p < (sizeof(gl_APIVersion) + sizeof(gl_DeviceVersionTemplate)))
    {
        *VersionSize_p = (sizeof(gl_APIVersion) + sizeof(gl_DeviceVersionTemplate));
        return VAL_BUFFER_TOO_SMALL;
    }

    if (Version_p == NULL)
    {
        return VAL_BAD_ARGUMENT;
    }

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_SYSTEMINFO;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
    ZEROINIT(t_res.Service.SystemInfo);

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            int i;
            uint16_t MemorySize;
            uint8_t * ptr;

            // Copy version template
            *VersionSize_p = (sizeof(gl_APIVersion) + sizeof(gl_DeviceVersionTemplate));
            ptr = Version_p;
            memcpy(ptr, gl_APIVersion, sizeof(gl_APIVersion));
            ptr += sizeof(gl_APIVersion) -1;
            *ptr++ = ' ';
            memcpy(ptr, gl_DeviceVersionTemplate, sizeof(gl_DeviceVersionTemplate));

            // Add the HW/FW version details
            ptr[4]  += t_res.Service.SystemInfo.Hardware.Major;
            ptr[6]  += t_res.Service.SystemInfo.Hardware.Minor;
            ptr[8]  += t_res.Service.SystemInfo.Hardware.Patch;
            ptr[13] += t_res.Service.SystemInfo.Firmware.Major;
            ptr[15] += t_res.Service.SystemInfo.Firmware.Minor;
            ptr[17] += t_res.Service.SystemInfo.Firmware.Patch;

            // Add memory size
            ptr += 24;
            MemorySize = t_res.Service.SystemInfo.Hardware.MemorySizeInBytes;
            for (i = 3; i >= 0; i--)
            {
                uint16_t Value = MemorySize & MASK_4_BITS;

                if (Value <= 9)
                {
                    *ptr += Value;
                }
                else
                {
                    *ptr += (Value + 7);
                }

                MemorySize >>= 4;
                ptr--;
            }
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}
#endif /* !VAL_REMOVE_SYSTEM_GETVERSION */


/*----------------------------------------------------------------------------
 * val_SystemGetState
 */
ValStatus_t
val_SystemGetState(
        uint8_t * const OtpErrorCode_p,
        uint16_t * const OtpErrorLocation_p,
        uint8_t * const Mode_p,
        uint8_t * const CryptoOfficer_p,
        uint8_t * const HostID_p,
        uint8_t * const NonSecure_p,
        uint32_t * const Identity_p)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_SYSTEMINFO;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
    ZEROINIT(t_res.Service.SystemInfo);

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            // return requested information
            if (OtpErrorCode_p != NULL)
            {
                *OtpErrorCode_p = t_res.Service.SystemInfo.OTP.ErrorCode;
            }
            if (OtpErrorLocation_p != NULL)
            {
                *OtpErrorLocation_p = t_res.Service.SystemInfo.OTP.ErrorLocation;
            }
            if (Mode_p != NULL)
            {
                *Mode_p = t_res.Service.SystemInfo.Self.Mode;
            }
            if (CryptoOfficer_p != NULL)
            {
                *CryptoOfficer_p = t_res.Service.SystemInfo.Self.CryptoOfficer;
            }
            if (HostID_p != NULL)
            {
                *HostID_p = t_res.Service.SystemInfo.Self.HostID;
            }
            if (NonSecure_p != NULL)
            {
                *NonSecure_p = t_res.Service.SystemInfo.Self.NonSecure;
            }
            if (Identity_p != NULL)
            {
                *Identity_p = t_res.Service.SystemInfo.Self.Identity;
            }
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_SystemDefineUsers
 */
#ifndef VAL_REMOVE_SYSTEM_DEFINE_USERS
ValStatus_t
val_SystemDefineUsers(
        const uint32_t User1,
        const uint32_t User2,
        const uint32_t User3,
        const uint32_t User4)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_DEFINEUSERS;
    t_cmd.Service.SystemDefineUsers.User1 = User1;
    t_cmd.Service.SystemDefineUsers.User2 = User2;
    t_cmd.Service.SystemDefineUsers.User3 = User3;
    t_cmd.Service.SystemDefineUsers.User4 = User4;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        funcres = (ValStatus_t)t_res.Result;
        LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
    }

    return funcres;
}
#endif /* !VAL_REMOVE_SYSTEM_DEFINE_USERS */


#if !defined(VAL_REMOVE_SYSTEM_RESET) || \
    !defined(VAL_REMOVE_SYSTEM_SELFTEST) || \
    !defined(VAL_REMOVE_SYSTEM_SLEEP)
static ValStatus_t
val_SystemGeneralCommand(VexTokenSubCode_t SubCode)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = SubCode;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        funcres = (ValStatus_t)t_res.Result;
        LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
    }

    return funcres;
}
#endif


/*----------------------------------------------------------------------------
 * val_SystemReset
 */
#ifndef VAL_REMOVE_SYSTEM_RESET
ValStatus_t
val_SystemReset(void)
{
    return val_SystemGeneralCommand(VEXTOKEN_SUBCODE_RESET);
}
#endif /* !VAL_REMOVE_SYSTEM_RESET */


/*----------------------------------------------------------------------------
 * val_SystemSelfTest
 */
#ifndef VAL_REMOVE_SYSTEM_SELFTEST
ValStatus_t
val_SystemSelfTest(void)
{
    return val_SystemGeneralCommand(VEXTOKEN_SUBCODE_SELFTEST);
}
#endif /* !VAL_REMOVE_SYSTEM_SELFTEST */


/*----------------------------------------------------------------------------
 * val_SystemSleep
 */
#ifndef VAL_REMOVE_SYSTEM_SLEEP
ValStatus_t
val_SystemSleep(void)
{
    return val_SystemGeneralCommand(VEXTOKEN_SUBCODE_SLEEP);
}
#endif /* !VAL_REMOVE_SYSTEM_SLEEP */


/*----------------------------------------------------------------------------
 * val_SystemResumeFromSleep
 */
#ifndef VAL_REMOVE_SYSTEM_SLEEP
ValStatus_t
val_SystemResumeFromSleep(void)
{
    return val_SystemGeneralCommand(VEXTOKEN_SUBCODE_RESUMEFROMSLEEP);
}
#endif /* !VAL_REMOVE_SYSTEM_SLEEP */


/*----------------------------------------------------------------------------
 * val_SystemHibernation
 */
#ifndef VAL_REMOVE_SYSTEM_HIBERNATION
ValStatus_t
val_SystemHibernation(
        const ValAssetId_t StateAssetId,
        const ValAssetId_t KEKAssetId,
        ValOctetsIn_t * const AssociatedData_p,
        const ValSize_t AssociatedDataSize,
        ValOctetsOut_t * const DataBlob_p,
        ValSize_t * const DataBlobSize_p)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#if 0
#ifdef VAL_STRICT_ARGS
    if ((StateAssetId == VAL_ASSETID_INVALID) ||
        (KEKAssetId == VAL_ASSETID_INVALID) ||
        (AssociatedData_p == NULL) ||
        (AssociatedDataSize < VAL_KEYBLOB_AAD_MIN_SIZE) ||
        (AssociatedDataSize > VAL_KEYBLOB_AAD_MAX_SIZE) ||
        (DataBlob_p == NULL) ||
        (DataBlobSize_p == NULL) ||
        (*DataBlobSize_p <  VAL_KEYBLOB_SIZE(1024)))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif
#endif

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_HIBERNATION;
    t_cmd.Service.SystemHibernation.StateAssetId = (uint32_t)StateAssetId;
    //t_cmd.Service.SystemHibernation.KeyAssetId = (uint32_t)KEKAssetId;
    memcpy(t_cmd.Service.SystemHibernation.AssociatedData, AssociatedData_p, AssociatedDataSize);
    t_cmd.Service.SystemHibernation.AssociatedDataSize = (uint32_t)AssociatedDataSize;
    t_cmd.Service.SystemHibernation.DataBlob_p = (const uint8_t *)DataBlob_p;
    t_cmd.Service.SystemHibernation.DataBlobSize = (uint32_t)*DataBlobSize_p;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;
    t_res.Service.SystemHibernation.DataBlobSize = 0;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        // Check for errors
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            *DataBlobSize_p = t_res.Service.SystemHibernation.DataBlobSize;
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}
#endif /* !VAL_REMOVE_SYSTEM_HIBERNATION */


/*----------------------------------------------------------------------------
 * val_SystemResumeFromHibernation
 */
#ifndef VAL_REMOVE_SYSTEM_HIBERNATION
ValStatus_t
val_SystemResumeFromHibernation(
        const ValAssetId_t StateAssetId,
        const ValAssetId_t KEKAssetId,
        ValOctetsIn_t * const AssociatedData_p,
        const ValSize_t AssociatedDataSize,
        ValOctetsIn_t * const DataBlob_p,
        const ValSize_t DataBlobSize)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#if 0
#ifdef VAL_STRICT_ARGS
    if ((StateAssetId == VAL_ASSETID_INVALID) ||
        (KEKAssetId == VAL_ASSETID_INVALID) ||
        (AssociatedData_p == NULL) ||
        (AssociatedDataSize < VAL_KEYBLOB_AAD_MIN_SIZE) ||
        (AssociatedDataSize > VAL_KEYBLOB_AAD_MAX_SIZE) ||
        (DataBlob_p == NULL) ||
        (DataBlobSize == 0))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif
#endif

    // Format service request
    t_cmd.OpCode  = VEXTOKEN_OPCODE_SYSTEM;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_RESUMEFROMHIBERNATION;
    t_cmd.Service.SystemHibernation.StateAssetId = (uint32_t)StateAssetId;
    //t_cmd.Service.SystemHibernation.KeyAssetId = (uint32_t)KEKAssetId;
    memcpy(t_cmd.Service.SystemHibernation.AssociatedData, AssociatedData_p, AssociatedDataSize);
    t_cmd.Service.SystemHibernation.AssociatedDataSize = (uint32_t)AssociatedDataSize;
    t_cmd.Service.SystemHibernation.DataBlob_p = (const uint8_t *)DataBlob_p;
    t_cmd.Service.SystemHibernation.DataBlobSize = (uint32_t)DataBlobSize;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        funcres = (ValStatus_t)t_res.Result;
        LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
    }

    return funcres;
}
#endif /* !VAL_REMOVE_SYSTEM_HIBERNATION */


/*----------------------------------------------------------------------------
 * val_IsAccessSecure
 */
bool
val_IsAccessSecure(void)
{
    static int gl_ValAccessType = 0;    // Secure/Non-secure access indication

    if ((gl_ValAccessType != 0x55AA55) && (gl_ValAccessType != 0x882288))
    {
        ValStatus_t FuncRes;
        uint8_t OtpErrorCode = 0;
        uint16_t OtpErrorLocation =0 ;
        uint8_t Mode = 0;
        uint8_t CryptoOfficer = 0;
        uint8_t HostID = 0;
        uint8_t NonSecure = 0;
        uint32_t Identity = 0;

        FuncRes = val_SystemGetState(&OtpErrorCode, &OtpErrorLocation,
                                     &Mode, &CryptoOfficer,
                                     &HostID, &NonSecure, &Identity);
        if(FuncRes == VAL_SUCCESS)
        {
            if (NonSecure)
            {
                // Set Non-secure indication
                gl_ValAccessType = 0x55AA55;
            }
            else
            {
                // Set Secure indication
                gl_ValAccessType = 0x882288;
            }
        }
    }

    if (gl_ValAccessType == 0x882288)
    {
        // Secure
        return true;
    }

    // Non-secure
    return false;
}

/* end of file adapter_val_system.c */
