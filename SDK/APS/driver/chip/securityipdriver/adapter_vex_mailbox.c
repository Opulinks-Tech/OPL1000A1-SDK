/* adapter_vex_mailbox.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the mailbox related functionality.
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


/*----------------------------------------------------------------------------
 * vex_MailboxGet
 */
uint8_t
vex_MailboxGet(
        uint32_t Identity)
{
    //IDENTIFIER_NOT_USED(Identity);

    // Future extension: Link mailbox based on identity.

    return VEX_MAILBOX_NR;
}


/* end of file adapter_vex_mailbox.c */
