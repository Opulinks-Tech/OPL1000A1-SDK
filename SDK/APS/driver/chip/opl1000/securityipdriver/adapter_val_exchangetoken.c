/* adapter_val_exchangetoken.c
 *
 * Implementation of the VaultIP Abstraction Layer.
 *
 * This file contains the token exchange with the VaultIP Exchange.
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

#include "basic_defs.h"                 // uint16_t
#include "log.h"

#include "adapter_val_internal.h"       // the API to implement
#include "adapter_val_internal_ext.h"   // User or Kernel mode indication
#include "adapter_vex.h"                // VexStatus_t, vex_ExchangeToken()
#include "msg.h"

/*----------------------------------------------------------------------------
 * val_ExchangeToken
 *
 * Allocate an asset and set its policy. Its content is setup later.
 */
ValStatus_t
val_ExchangeToken(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p)
{
    VexStatus_t funcres;

#ifdef VAL_STRICT_ARGS
    // In principle an internal function, so do not check arguments
    if (CommandToken_p == NULL ||
        ResultToken_p == NULL)
    {
        return VAL_INTERNAL_ERROR;
    }
#endif

    CommandToken_p->fFromUserSpace = ADAPTER_VAL_INTERNAL_USER_SPACE_FLAG;

    msg_print(LOG_HIGH_LEVEL, "[security] val_ExchangeToken \r\n");

    funcres = vex_LogicalToken(CommandToken_p, ResultToken_p);
    
    msg_print(LOG_HIGH_LEVEL, "[security] val_ExchangeToken, funcres:%d \r\n", funcres);

    if (funcres != VEX_SUCCESS)
    {
        LOG_WARN("%s::vex_LogicalToken()=%d\n", __func__, funcres);
        if ((funcres == VEX_REPONSE_TIMEOUT) ||
            (funcres == VEX_DATA_TIMEOUT))
        {
            return VAL_TIMEOUT_ERROR;
        }
        return VAL_INTERNAL_ERROR;
    }

    return VAL_SUCCESS;
}


/* end of file adapter_val_exchangetoken.c */
