/* adapter_vex_identity.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the identity related functionality.
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

#include "adapter_getpid.h"
#include "adapter_vex_internal.h"   // API implementation

#define VEX_IDENTITY_MAX_USERS 4
#define VEX_IDENTITY_CO_INDEX  VEX_IDENTITY_MAX_USERS

struct vex_identity
{
   int      pid;
   uint32_t identity;
};

static struct vex_identity gl_Identities[VEX_IDENTITY_MAX_USERS + 1] =
{
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, VEX_CRYPTO_OFFICER_ID }
};


/*----------------------------------------------------------------------------
 * vex_IdentityGet
 */
uint32_t
vex_IdentityGet(void)
{
/*
    int pid = Adapter_GetPid();
    uint32_t identity = 0;
    int i;

    for (i = 0; i < (VEX_IDENTITY_MAX_USERS + 1); i++)
    {
        if (gl_Identities[i].pid == pid)
        {
            identity = gl_Identities[i].identity;
            break;
        }
    }

    if (identity == 0)
    {
        // Add process if not known
        identity = vex_IdentityUserAdd();
    }
*/

    /* FUTURE: Find solution for TEE environment to distinguish users */
/*
    if (vex_DeviceIsSecureConnected())
    {
        identity = VEX_CRYPTO_OFFICER_ID;
    }
    else
    {
        identity = (VEX_CRYPTO_OFFICER_ID ^ -1);
    }

    return identity;
*/

    return VEX_CRYPTO_OFFICER_ID;
}


/*----------------------------------------------------------------------------
 * vex_IdentityCryptoOfficer
 */
void
vex_IdentityCryptoOfficer(
        uint32_t CryptoOfficerId)
{
    //gl_Identities[VEX_IDENTITY_CO_INDEX].pid = Adapter_GetPid();
    if (CryptoOfficerId != 0)
    {
        gl_Identities[VEX_IDENTITY_CO_INDEX].identity = CryptoOfficerId;
    }
    else
    {
        gl_Identities[VEX_IDENTITY_CO_INDEX].identity = VEX_CRYPTO_OFFICER_ID;
    }
/*
    LOG_CRIT(VEX_LOG_PREFIX "Set CryptoOfficerId: Process ID=0x%x (%u)\n",
             gl_Identities[VEX_IDENTITY_CO_INDEX].pid,
             gl_Identities[VEX_IDENTITY_CO_INDEX].identity);
*/
}


/*----------------------------------------------------------------------------
 * vex_IdentityUserAdd
 */
uint32_t
vex_IdentityUserAdd(void)
{

    //static uint32_t gl_IdentityRef = 0x1;
    //int pid = Adapter_GetPid();
    uint32_t identity = 0;
/*
    int i;

    for (i = 0; i < VEX_IDENTITY_MAX_USERS; i++)
    {
        if (gl_Identities[i].pid == pid)
        {
            identity = gl_Identities[i].identity;
            break;
        }
    }

    if (identity == 0)
    {
        // Add process if not known
        // Note: Setting of Crypto Officer pid requires
        //       vex_IdentityCryptoOfficer()
        gl_IdentityRef++;
        gl_IdentityRef &= 0x0FFFFFFF;
        if (gl_IdentityRef == 0)
        {
            gl_IdentityRef = 0x1;
        }
        for (i = 0; i < VEX_IDENTITY_MAX_USERS; i++)
        {
            if (gl_Identities[i].pid == 0)
            {
                gl_Identities[i].pid = pid;
                identity = (uint32_t)((gl_IdentityRef << 4) + i);
                gl_Identities[i].identity = identity;

                LOG_CRIT(VEX_LOG_PREFIX "Add User%d: Process ID=0x%x (%u)\n",
                         i+1,
                         gl_Identities[i].pid,
                         gl_Identities[i].identity);
                break;
            }
        }
        if (identity == 0)
        {
            LOG_CRIT(VEX_LOG_PREFIX "FAILED to add User - No free entry\n");
        }
    }
*/

    return identity;
}


/*----------------------------------------------------------------------------
 * vex_IdentityUserRemove
 */
void
vex_IdentityUserRemove(void)
{
    int pid = Adapter_GetPid();
    int i;

    for (i = 0; i < VEX_IDENTITY_MAX_USERS; i++)
    {
        if (gl_Identities[i].pid == pid)
        {
            gl_Identities[i].pid = 0;
            gl_Identities[i].identity = 0;

            LOG_CRIT(VEX_LOG_PREFIX "Remove User%d: Process ID=0x%x\n",
                     i+1, pid);
            break;
        }
    }
}


/* end of file adapter_vex_identity.c */
