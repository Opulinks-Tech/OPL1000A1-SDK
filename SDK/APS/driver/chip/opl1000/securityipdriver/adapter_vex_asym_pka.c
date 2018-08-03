/* adapter_vex_asym_pka.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the asymmetric crypto services for direct PKA/PKCP use.
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

#include "c_adapter_vex.h"              // configuration

#include "basic_defs.h"
#include "clib.h"

#include "adapter_vex_intern_asym.h"   // API implementation
#include "adapter_bufmanager.h"        // BufManager_*()
#include "eip130_token_pk.h"           // Eip130Token_Command_Pk_Asset*()


/*----------------------------------------------------------------------------
 * vex_Asym_PkaNumSet
 */
VexStatus_t
vex_Asym_PkaNumSet(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Claim(&CommandToken,
                                 CommandToken_p->Service.PkOperation.Nwords,
                                 CommandToken_p->Service.PkOperation.Mwords,
                                 CommandToken_p->Service.PkOperation.Mmask);
    Vex_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID());

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
 * vex_Asym_PkaNumLoad
 */
VexStatus_t
vex_Asym_PkaNumLoad(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;
    uint64_t VectorAddr = 0;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

/*
    // Get Vector input address for token
    VectorAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                BUFMANAGER_BUFFERTYPE_IN,
                                CommandToken_p->Service.PkOperation.InData_p,
                                CommandToken_p->Service.PkOperation.InDataSize,
                                NULL);
    if (VectorAddr == 0)
    {
        return VEX_NO_MEMORY;
    }
*/
    VectorAddr = (uint64_t) CommandToken_p->Service.PkOperation.InData_p;

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_NumLoad(&CommandToken,
                                   CommandToken_p->Service.PkOperation.Index,
                                   VectorAddr,
                                   CommandToken_p->Service.PkOperation.InDataSize);
    Vex_Command_SetTokenID(&CommandToken, vex_DeviceGetTokenID());

    // Initialize result token
    ZEROINIT(ResultToken);

    // Exchange token with VaultIP
    funcres = vex_PhysicalTokenExchange(&CommandToken, &ResultToken);
    if (funcres == VEX_SUCCESS)
    {
        ResultToken_p->Result = Eip130Token_Result_Code(&ResultToken);
    }

/*
    // Release used buffer
    (void)BufManager_Unmap(VectorAddr, false);
*/

    return funcres;
}


/*----------------------------------------------------------------------------
 * vex_Asym_PkaOperation
 */
VexStatus_t
vex_Asym_PkaOperation(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint64_t InputAddr = 0;
    uint64_t OutputAddr = 0;
    size_t InputSize = 0;
    size_t OutputSize = 0;
    uint16_t TokenID;
    bool fCopy = false;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

    // Get Vector input address for token
    if (CommandToken_p->Service.PkOperation.InData_p != NULL)
    {
        InputSize = CommandToken_p->Service.PkOperation.InDataSize;

/*
        InputAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                   BUFMANAGER_BUFFERTYPE_IN,
                                   CommandToken_p->Service.PkOperation.InData_p,
                                   InputSize,
                                   NULL);
        if (InputAddr == 0)
        {
            goto error_func_exit;
        }
*/
        InputAddr = (uint64_t) CommandToken_p->Service.PkOperation.InData_p;


    }

    TokenID = vex_DeviceGetTokenID();

    if (CommandToken_p->Service.PkOperation.OutData_p != NULL)
    {

/*
        OutputAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                    BUFMANAGER_BUFFERTYPE_OUT,
                                    CommandToken_p->Service.PkOperation.OutData_p,
                                    CommandToken_p->Service.PkOperation.OutDataSize,
                                    (void *)&TokenID);
        if (OutputAddr == 0)
        {
            goto error_func_exit;
        }
        OutputSize = BufManager_GetSize(OutputAddr);
*/
        OutputAddr = (uint64_t) CommandToken_p->Service.PkOperation.OutData_p;
        OutputSize = CommandToken_p->Service.PkOperation.OutDataSize;


    }

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Pk_Operation(&CommandToken,
                                     CommandToken_p->Service.PkOperation.Operation,
                                     CommandToken_p->Service.PkOperation.PublicExponent,
                                     InputAddr, InputSize,
                                     OutputAddr, OutputSize);
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
    // Release used buffer
    if (InputAddr != 0)
    {
        (void)BufManager_Unmap(InputAddr, false);
    }

    if (OutputAddr != 0)
    {
        int rc = BufManager_Unmap(OutputAddr, fCopy);
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



/* end of file adapter_vex_asym_pka.c */
