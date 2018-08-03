/* adapter_vex_nop.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the NOP crypto aka Data Copy.
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
#include "adapter_bufmanager.h"     // BufManager_*()
#include "eip130_token_nop.h"       // Eip130Token_Command_Nop()


/*----------------------------------------------------------------------------
 * vex_Nop
 */
VexStatus_t
vex_Nop(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_NO_MEMORY;
    uint16_t TokenID;
    uint64_t SrcDataAddr = 0;
    uint64_t DstDataAddr = 0;
    bool fCopy = false;
    Eip130Token_Command_t CommandToken;
    Eip130Token_Result_t ResultToken;

/*
    // Convert input buffer and get address for token
    SrcDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                 BUFMANAGER_BUFFERTYPE_IN,
                                 CommandToken_p->Service.Nop.In_p,
                                 CommandToken_p->Service.Nop.Size,
                                 NULL);
    if (SrcDataAddr == 0)
    {
        goto error_func_exit;
    }
*/
    SrcDataAddr = (uint64_t) CommandToken_p->Service.Nop.In_p;


    // Get output address for token
    TokenID = vex_DeviceGetTokenID();


/*    
    DstDataAddr = BufManager_Map(CommandToken_p->fFromUserSpace,
                                 BUFMANAGER_BUFFERTYPE_OUT,
                                 CommandToken_p->Service.Nop.Out_p,
                                 CommandToken_p->Service.Nop.Size,
                                 (void *)&TokenID);
    if (DstDataAddr == 0)
    {
        goto error_func_exit;
    }

    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Nop(&CommandToken,
                            SrcDataAddr, CommandToken_p->Service.Nop.Size,
                            DstDataAddr, BufManager_GetSize(DstDataAddr));
*/
    DstDataAddr = (uint64_t) CommandToken_p->Service.Nop.Out_p;
    // Format command token
    ZEROINIT(CommandToken);
    Eip130Token_Command_Nop(&CommandToken,
                            SrcDataAddr, CommandToken_p->Service.Nop.Size,
                            DstDataAddr, CommandToken_p->Service.Nop.Size);



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
    if (SrcDataAddr != 0)
    {
        (void)BufManager_Unmap(SrcDataAddr, false);
    }
    if (DstDataAddr != 0)
    {
        int rc = BufManager_Unmap(DstDataAddr, fCopy);
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


/* end of file adapter_vex_nop.c */
