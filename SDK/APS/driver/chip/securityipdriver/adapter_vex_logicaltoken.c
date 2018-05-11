/* adapter_vex_logicaltoken.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file contains the logical token exchange implementation between the
 * VaultIP Abstraction Layer and VaultIP Exchange.
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

#include "basic_defs.h"             // uint16_t
#include "log.h"

#include "adapter_vex.h"            // API to implement
#include "adapter_vex_internal.h"   // vex_*() and VexToken_Command/Result_t
#include "msg.h"

/*----------------------------------------------------------------------------
 * vex_LogicalToken
 */
VexStatus_t
vex_LogicalToken(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres = VEX_UNSUPPORTED;

    msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken \r\n");


#ifdef VEX_STRICT_ARGS
    if ((CommandToken_p == NULL) ||
        (ResultToken_p == NULL))
    {
        return VEX_BAD_ARGUMENT;
    }
#endif


/*
    if (vex_LockAcquire() < 0)
    {
        return VEX_LOCK_TIMEOUT;
    }
*/	

    if (vex_DeviceGetHandle() != NULL)
	{
	    msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, vex_DeviceGetHandle() != NULL \r\n");
        
        // VaultIP connection available
        switch (CommandToken_p->OpCode)
        {
        case VEXTOKEN_OPCODE_NOP:
            funcres = vex_Nop(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_ENCRYPTION:
            if (CommandToken_p->SubCode)
            {
                funcres = vex_SymCipherAE(CommandToken_p, ResultToken_p);
            }
            else
            {
                msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymCipher() \r\n");
                
                funcres = vex_SymCipher(CommandToken_p, ResultToken_p);

                msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymCipher(), funcres:%d \r\n", funcres);
            }
            break;

        case VEXTOKEN_OPCODE_HASH:
            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymHash() \r\n");
            
            funcres = vex_SymHash(CommandToken_p, ResultToken_p);

            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymHash(), funcres:%d \r\n", funcres);
            
            break;

        case VEXTOKEN_OPCODE_MAC:
            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymMac() \r\n");
            
            funcres = vex_SymMac(CommandToken_p, ResultToken_p);

            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_SymMac(), funcres:%d \r\n", funcres);
            
            break;

        case VEXTOKEN_OPCODE_TRNG:
            funcres = vex_Trng(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_AESWRAP:
#ifdef VEX_ENABLE_ENCRYPTED_VECTOR
            //if (CommandToken_p->SubCode)
            //{
            //    funcres = vex_EncryptedVector(CommandToken_p, ResultToken_p);
            //}
            //else
#endif
            //{
            //    funcres = vex_SymKeyWrap(CommandToken_p, ResultToken_p);
            //}
            break;


        case VEXTOKEN_OPCODE_ASSETMANAGEMENT:
            //funcres = vex_Asset(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_AUTH_UNLOCK:
            //funcres = vex_AUnlock(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_PUBLIC_KEY:
            funcres = vex_Asym(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_EMMC:
            //funcres = vex_eMMC(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_EXTSERVICE:
            //funcres = vex_ExtService(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_SERVICE:
            //funcres = vex_Service(CommandToken_p, ResultToken_p);
            break;

        case VEXTOKEN_OPCODE_SYSTEM:
            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_System() \r\n");
            
            funcres = vex_System(CommandToken_p, ResultToken_p);

            msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, do vex_System(), funcres:%d \r\n", funcres);
            
            break;

        case VEXTOKEN_OPCODE_CLAIMCONTROL:
            //funcres = vex_Claim(CommandToken_p, ResultToken_p);
            break;

        default:
            funcres = VEX_INVALID_OPCODE;
            break;
        }
    }
    else
    {
        funcres = VEX_NOT_CONNECTED;
        
        msg_print(LOG_HIGH_LEVEL, "[security] vex_LogicalToken, vex_DeviceGetHandle() == NULL \r\n");
    }

    //vex_LockRelease();

    return funcres;
}


/* end of file adapter_vex_logicaltoken.c */
