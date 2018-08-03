/* adapter_vex_trng.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the TRNG services.
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

#include "adapter_vex_internal.h"   // API implementation
#include "adapter_bufmanager.h"     // BufManager_*()
#include "eip130_token_random.h"    // Eip130Token_Command_RandomNumber_Generate()
                                    // Eip130Token_Command_TRNG_Configure()
                                    // Eip130Token_Command_PRNG_ReseedNow()

/*----------------------------------------------------------------------------
 * vexLocal_TrngRandom
 */
static VexStatus_t
vexLocal_TrngRandom(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    uint16_t TokenID;
    uint64_t DataAddr = 0;
    bool fCopy = false;
    int rc;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Random output via DMA and setup DMA OutputAddress in token.
    TokenID = vex_DeviceGetTokenID();


/*
    DataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                              BUFMANAGER_BUFFERTYPE_OUT,
                              CommandToken_p->Service.Random.Data_p,
                              CommandToken_p->Service.Random.Size,
                              (void *)&TokenID);
    if (DataAddr == 0)
    {
        return VEX_NO_MEMORY;
    }
*/
    DataAddr = (uint64_t) CommandToken_p->Service.Random.Data_p;


    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_RandomNumber_Generate(&CommandToken,
                                              CommandToken_p->Service.Random.Size,
                                              DataAddr);
    Vex_Command_SetTokenID(&CommandToken, TokenID);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if (ResultToken_p->Result >= 0)
        {
            // Copy output data
            fCopy = true;
        }
    }

/*
    // Release used buffer, if needed
    rc = BufManager_Unmap(DataAddr, fCopy);
    if (rc != 0)
    {
        if (rc == -3)
        {
            funcres = VEX_DATA_TIMEOUT;
        }
        else
        {
            funcres = VEX_INTERNAL_ERROR;
        }
    }
*/

    return funcres;
}



/*----------------------------------------------------------------------------
 * vexLocal_TrngConfig
 */
static VexStatus_t
vexLocal_TrngConfig(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Format command token
    ZEROINIT(CommandToken);
    if (CommandToken_p->Service.TrngConfig.fReseed &&
        !CommandToken_p->Service.TrngConfig.fLoadStart)
    {
        // Only reseed
        Eip130Token_Command_PRNG_ReseedNow(&CommandToken);
    }
    else
    {
        // Configure
        Eip130Token_Command_TRNG_Configure(
            &CommandToken,
            CommandToken_p->Service.TrngConfig.AutoSeed,
            CommandToken_p->Service.TrngConfig.SampleCycles,
            CommandToken_p->Service.TrngConfig.SampleDiv,
            CommandToken_p->Service.TrngConfig.NoiseBlocks);

        if (CommandToken_p->Service.TrngConfig.fReseed)
        {
            // RRD = Reseed post-processor
            CommandToken.W[2] |= BIT_1;
        }
    }

    Eip130Token_Command_SetTokenID(&CommandToken,
                                   vex_DeviceGetTokenID(),
                                   false);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
    }

    return funcres;
}


/*----------------------------------------------------------------------------
 * vexLocal_VerifyDrbg
 */
static VexStatus_t
vexLocal_VerifyDrbg(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint16_t TokenID;
    uint64_t InputDataAddr = 0;
    uint64_t OutputDataAddr = 0;
    bool fCopy = false;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    if (CommandToken_p->Service.DrbgPP.InputData_p != NULL)
    {

/*
        // Convert input buffer and get address for token
        InputDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                       BUFMANAGER_BUFFERTYPE_IN,
                                       CommandToken_p->Service.DrbgPP.InputData_p,
                                       CommandToken_p->Service.DrbgPP.InputDataSize,
                                       NULL);
        if (InputDataAddr == 0)
        {
            goto error_func_exit;
        }
*/
        InputDataAddr = (uint64_t) CommandToken_p->Service.DrbgPP.InputData_p;

    }


    // Get output address for token
    // Note: The TRNG Post-Processing Verification does not respect the write
    //       TokenID after DMA data write
    TokenID = vex_DeviceGetTokenID();


#if 0    
    OutputDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                    BUFMANAGER_BUFFERTYPE_OUT,
                                    CommandToken_p->Service.DrbgPP.OutputData_p,
                                    CommandToken_p->Service.DrbgPP.OutputDataSize,
                                    NULL/*(void *)&TokenID*/);
    if (OutputDataAddr == 0)
    {
        goto error_func_exit;
    }
#endif
    OutputDataAddr = (uint64_t) CommandToken_p->Service.DrbgPP.OutputData_p;


    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_TRNG_PP_Verification(&CommandToken,
                                             CommandToken_p->Service.DrbgPP.Test,
                                             CommandToken_p->Service.DrbgPP.Pattern,
                                             CommandToken_p->Service.DrbgPP.Size,
                                             InputDataAddr,
                                             OutputDataAddr);
    Vex_Command_SetTokenID(&CommandToken, TokenID);

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if (ResultToken_p->Result >= 0)
        {
            // Copy output data
            fCopy = true;
        }
    }

error_func_exit:
/*
    // Release used buffers, if needed
    if (InputDataAddr != 0)
    {
        (void)BufManager_Unmap(InputDataAddr, false);
    }
    if (OutputDataAddr != 0)
    {
        int rc = BufManager_Unmap(OutputDataAddr, fCopy);
        if (rc != 0)
        {
            if (rc == -3)
            {
                funcres = VEX_DATA_TIMEOUT;
            }
            else
            {
                funcres = VEX_INTERNAL_ERROR;
            }
        }
    }
*/

    return funcres;
}


/*----------------------------------------------------------------------------
 * vexLocal_VerifyNrbg
 */
static VexStatus_t
vexLocal_VerifyNrbg(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    uint64_t DataAddr = 0;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

/*
    // Convert input buffer and get address for token
    DataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                              BUFMANAGER_BUFFERTYPE_IN,
                              CommandToken_p->Service.NrbgVerify.InputData_p,
                              CommandToken_p->Service.NrbgVerify.Size,
                              NULL);
    if (DataAddr == 0)
    {
        return VEX_NO_MEMORY;
    }
*/
    DataAddr = (uint64_t) CommandToken_p->Service.NrbgVerify.InputData_p;


    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_TRNG_HW_SelfTest(&CommandToken,
                                         DataAddr,
                                         CommandToken_p->Service.NrbgVerify.Size,
                                         CommandToken_p->Service.NrbgVerify.RepCntCutoff,
                                         CommandToken_p->Service.NrbgVerify.RepCntCount,
                                         CommandToken_p->Service.NrbgVerify.RepCntData,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp64Cutoff,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp64Count,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp64Data,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp4kCutoff,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp4kCount,
                                         CommandToken_p->Service.NrbgVerify.AdaptProp4kData);
    Vex_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID());

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
        if (ResultToken_p->Result >= 0)
        {
            Eip130Token_Result_TRNG_HW_SelfTest(&ResultToken,
                                                &ResultToken_p->Service.NrbgVerify.RepCntCutoff,
                                                &ResultToken_p->Service.NrbgVerify.RepCntCount,
                                                &ResultToken_p->Service.NrbgVerify.RepCntData,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp64Cutoff,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp64Count,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp64Data,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp64Fail,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp4kCutoff,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp4kCount,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp4kData,
                                                &ResultToken_p->Service.NrbgVerify.AdaptProp4kFail);
        }
    }

/*
    // Release used buffer, if needed
    (void)BufManager_Unmap(DataAddr, false);
*/

    return funcres;
}


/*----------------------------------------------------------------------------
 * vex_Trng
 */
VexStatus_t
vex_Trng(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_UNSUPPORTED;

    switch (CommandToken_p->SubCode)
    {
    case VEXTOKEN_SUBCODE_RANDOMNUMBER:
        funcres = vexLocal_TrngRandom(CommandToken_p, ResultToken_p);
        break;

    case VEXTOKEN_SUBCODE_TRNGCONFIG:
        //funcres = vexLocal_TrngConfig(CommandToken_p, ResultToken_p);
        break;

    case VEXTOKEN_SUBCODE_VERIFYDRBG:
        //funcres = vexLocal_VerifyDrbg(CommandToken_p, ResultToken_p);
        break;

    case VEXTOKEN_SUBCODE_VERIFYNRBG:
        //funcres = vexLocal_VerifyNrbg(CommandToken_p, ResultToken_p);
        break;

    default:
        break;
    }

    return funcres;
}


/* end of file adapter_vex_trng.c */
