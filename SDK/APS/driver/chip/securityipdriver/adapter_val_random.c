/* adapter_val_random.c
 *
 * Implementation of the VaultIP Abstraction Layer API.
 *
 * This file implements the random data related services.
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

#include "c_adapter_val.h"              // configuration

#include "basic_defs.h"
#include "clib.h"
#include "log.h"

#include "api_val_random.h"             // the API to implement
#include "adapter_val_internal.h"       // val_ExchangeToken()
#include "adapter_vex.h"                // VexToken_Command_t, VexToken_Result_t
#include "msg.h"

/*----------------------------------------------------------------------------
 * val_RandomData
 */
ValStatus_t
val_RandomData(
        const ValSize_t DataSize,
        ValOctetsOut_t * const  Data_p)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    msg_print(LOG_HIGH_LEVEL, "[security] val_RandomData, DataSize:%d \r\n", DataSize);

#ifdef VAL_STRICT_ARGS
    if ((Data_p == NULL) ||
        (DataSize == 0) || (DataSize > 65535))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_TRNG;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_RANDOMNUMBER;
    t_cmd.Service.Random.Data_p = (uint8_t *)Data_p;
    t_cmd.Service.Random.Size = (uint32_t)DataSize;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);

    msg_print(LOG_HIGH_LEVEL, "[security] val_RandomData, funcres:%d \r\n", funcres);
    
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        //LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        msg_print(LOG_HIGH_LEVEL, "[security] val_RandomData, Abort - %s()=%d \r\n", __func__, t_res.Result);

        if (t_res.Result == VEXTOKEN_RESULT_SEQ_INVALID_PARAMETER)
        {
            funcres = VAL_NOT_INITIALIZED;
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_RandomReseed
 */
ValStatus_t
val_RandomReseed(void)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_TRNG;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_TRNGCONFIG;
    t_cmd.Service.TrngConfig.fLoadStart = 0;
    t_cmd.Service.TrngConfig.fReseed = 1;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        //LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);

        if (t_res.Result == VEXTOKEN_RESULT_SEQ_INVALID_PARAMETER)
        {
            funcres = VAL_NOT_INITIALIZED;
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_TrngConfig
 */
#ifndef VAL_REMOVE_TRNG_CONFIG
ValStatus_t
val_TrngConfig(
        uint8_t  AutoSeed,
        uint16_t SampleCycles,
        uint8_t  SampleDiv,
        uint8_t  NoiseBlocks,
        bool     fReseed)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_TRNG;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_TRNGCONFIG;
    t_cmd.Service.TrngConfig.fLoadStart = 1;
    t_cmd.Service.TrngConfig.fReseed = fReseed ? 1 : 0;
    t_cmd.Service.TrngConfig.AutoSeed = AutoSeed;
    t_cmd.Service.TrngConfig.SampleCycles = SampleCycles;
    t_cmd.Service.TrngConfig.SampleDiv = SampleDiv;
    t_cmd.Service.TrngConfig.NoiseBlocks = NoiseBlocks;
    t_cmd.Service.TrngConfig.FroBlockKey = 0;
    t_res.Result = VEXTOKEN_RESULT_NO_ERROR;

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if ((funcres == VAL_SUCCESS) &&
        (t_res.Result != VEXTOKEN_RESULT_NO_ERROR))
    {
        // Error
        LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);

        if (t_res.Result == VEXTOKEN_RESULT_SEQ_INVALID_PARAMETER)
        {
            funcres = VAL_NOT_INITIALIZED;
        }
        else
        {
            funcres = (ValStatus_t)t_res.Result;
        }
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * val_TrngDrbgPostProcessing
 */
#ifndef VAL_REMOVE_VERIFY_DRBG_POSTPROCCESSING
ValStatus_t
val_TrngDrbgPostProcessing(
        const ValTrngDrbgPPTest_t Test,
        const uint16_t Pattern,
        const ValSize_t DataSize,
        ValOctetsIn_t * const InputData_p,
        const ValSize_t InputDataSize,
        ValOctetsOut_t * const OutputData_p,
        const ValSize_t OutputDataSize)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_TRNG;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_VERIFYDRBG;
    switch (Test)
    {
    case VAL_TRNG_DRBGPP_CF_12B_REPEAT_PATTERN:
#ifdef VAL_STRICT_ARGS
        if ((DataSize == 0) || (DataSize > 255) ||
            (OutputData_p == NULL))
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.DrbgPP.Pattern = Pattern;
        t_cmd.Service.DrbgPP.Size = DataSize;
        t_cmd.Service.DrbgPP.InputData_p = 0;
        t_cmd.Service.DrbgPP.InputDataSize = 0;
        break;

    case VAL_TRNG_DRBGPP_CF_EXTERNAL_NOISE:
#ifdef VAL_STRICT_ARGS
        if ((DataSize == 0) || (DataSize > 255) ||
            (InputData_p == NULL) ||
            (OutputData_p == NULL))
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.DrbgPP.Pattern = 0;
        t_cmd.Service.DrbgPP.Size = DataSize;
        t_cmd.Service.DrbgPP.InputData_p = InputData_p;
        t_cmd.Service.DrbgPP.InputDataSize = InputDataSize;
        break;

    case VAL_TRNG_DRBGPP_KNOWN_ANSWER:
#ifdef VAL_STRICT_ARGS
        if (((DataSize & MASK_4_BITS) == 0) ||
            (InputData_p == NULL) ||
            (OutputData_p == NULL))
        {
            return VAL_BAD_ARGUMENT;
        }
#endif
        t_cmd.Service.DrbgPP.Pattern = 0;
        t_cmd.Service.DrbgPP.Size = DataSize;
        t_cmd.Service.DrbgPP.InputData_p = InputData_p;
        t_cmd.Service.DrbgPP.InputDataSize = InputDataSize;
        break;

    default:
        return VAL_BAD_ARGUMENT;
    }
    t_cmd.Service.DrbgPP.Test = (uint8_t)Test;
    t_cmd.Service.DrbgPP.OutputData_p = OutputData_p;
    t_cmd.Service.DrbgPP.OutputDataSize = OutputDataSize;
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
#endif /* !VAL_REMOVE_VERIFY_DRBG_POSTPROCCESSING */


/*----------------------------------------------------------------------------
 * val_TrngNrbgSelfTest
 */
#ifndef VAL_REMOVE_VERIFY_NRBG_SELFTEST
ValStatus_t
val_TrngNrbgSelfTest(
        ValOctetsIn_t * const InputData_p,
        const ValSize_t Size,
        uint8_t * const RepCntCutoff_p,
        uint8_t * const RepCntCount_p,
        uint8_t * const RepCntData_p,
        uint8_t * const AdaptProp64Cutoff_p,
        uint8_t * const AdaptProp64Count_p,
        uint8_t * const AdaptProp64Data_p,
        uint16_t * const AdaptProp4kCutoff_p,
        uint16_t * const AdaptProp4kCount_p,
        uint8_t * const AdaptProp4kData_p,
        uint8_t * const AdaptProp64Fail_p,
        uint8_t * const AdaptProp4kFail_p)
{
    VexToken_Command_t t_cmd;
    VexToken_Result_t t_res;
    ValStatus_t funcres;

#ifdef VAL_STRICT_ARGS
    if ((InputData_p == NULL) ||
        (RepCntCutoff_p == NULL) ||
        (RepCntCount_p == NULL) ||
        (RepCntData_p == NULL) ||
        (AdaptProp64Cutoff_p == NULL) ||
        (AdaptProp64Count_p == NULL) ||
        (AdaptProp64Data_p == NULL) ||
        (AdaptProp64Fail_p == NULL) ||
        (AdaptProp4kCutoff_p == NULL) ||
        (AdaptProp4kCount_p == NULL) ||
        (AdaptProp4kData_p == NULL) ||
        (AdaptProp4kFail_p == NULL) ||
        ((*RepCntCutoff_p & ~(MASK_6_BITS)) != 0) ||
        ((*AdaptProp64Cutoff_p & ~(MASK_6_BITS)) != 0) ||
        ((*AdaptProp4kCutoff_p & ~(MASK_12_BITS)) != 0) ||
        ((*RepCntCount_p & ~(MASK_6_BITS)) != 0) ||
        ((*AdaptProp64Count_p & ~(MASK_6_BITS)) != 0) ||
        ((*AdaptProp4kCount_p & ~(MASK_12_BITS)) != 0) ||
        (Size == 0) ||
        ((Size & ~(MASK_16_BITS)) != 0))
    {
        return VAL_BAD_ARGUMENT;
    }
#endif

    // Format service request
    t_cmd.OpCode = VEXTOKEN_OPCODE_TRNG;
    t_cmd.SubCode = VEXTOKEN_SUBCODE_VERIFYNRBG;

    t_cmd.Service.NrbgVerify.RepCntCutoff = *RepCntCutoff_p;
    t_cmd.Service.NrbgVerify.RepCntCount = *RepCntCount_p;
    t_cmd.Service.NrbgVerify.RepCntData = *RepCntData_p;

    t_cmd.Service.NrbgVerify.AdaptProp64Cutoff = *AdaptProp64Cutoff_p;
    t_cmd.Service.NrbgVerify.AdaptProp64Count = *AdaptProp64Count_p;
    t_cmd.Service.NrbgVerify.AdaptProp64Data = *AdaptProp64Data_p;

    t_cmd.Service.NrbgVerify.AdaptProp4kCutoff = *AdaptProp4kCutoff_p;
    t_cmd.Service.NrbgVerify.AdaptProp4kCount = *AdaptProp4kCount_p;
    t_cmd.Service.NrbgVerify.AdaptProp4kData = *AdaptProp4kData_p;

    t_cmd.Service.NrbgVerify.InputData_p = InputData_p;
    t_cmd.Service.NrbgVerify.Size = (uint16_t)Size;
    ZEROINIT(t_res);

    // Exchange service request with VaultIP
    funcres = val_ExchangeToken(&t_cmd, &t_res);
    if (funcres == VAL_SUCCESS)
    {
        if (t_res.Result == VEXTOKEN_RESULT_NO_ERROR)
        {
            *RepCntCutoff_p = t_res.Service.NrbgVerify.RepCntCutoff;
            *RepCntCount_p = t_res.Service.NrbgVerify.RepCntCount;
            *RepCntData_p = t_res.Service.NrbgVerify.RepCntData;

            *AdaptProp64Cutoff_p = t_res.Service.NrbgVerify.AdaptProp64Cutoff;
            *AdaptProp64Count_p = t_res.Service.NrbgVerify.AdaptProp64Count;
            *AdaptProp64Data_p = t_res.Service.NrbgVerify.AdaptProp64Data;
            *AdaptProp64Fail_p = t_res.Service.NrbgVerify.AdaptProp64Fail;

            *AdaptProp4kCutoff_p = t_res.Service.NrbgVerify.AdaptProp4kCutoff;
            *AdaptProp4kCount_p = t_res.Service.NrbgVerify.AdaptProp4kCount;
            *AdaptProp4kData_p = t_res.Service.NrbgVerify.AdaptProp4kData;
            *AdaptProp4kFail_p = t_res.Service.NrbgVerify.AdaptProp4kFail;
        }
        else
        {
            // Error
            funcres = (ValStatus_t)t_res.Result;
            LOG_WARN("Abort - %s()=%d\n", __func__, t_res.Result);
        }
    }

    return funcres;
}
#endif /* !VAL_REMOVE_VERIFY_NRBG_SELFTEST */

#endif //#if 0

/* end of file adapter_val_random.c */
