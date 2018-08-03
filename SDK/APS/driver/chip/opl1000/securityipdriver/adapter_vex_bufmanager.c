/* adapter_vex_bufmanager.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the buffer manager specific functions.
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
#include "adapter_sleep.h"          // Adapter_SleepMS()

#define VEX_DMA_TOKEN_ID_SIZE   sizeof(uint32_t)

#if 0
/*----------------------------------------------------------------------------
 * vexLocal_CB_SizeAlignment
 */
static size_t
vexLocal_CB_SizeAlignment(
        const size_t Size)
{
#ifdef VEX_CHECK_DMA_WITH_TOKEN_ID
    // 32-bit alignment + extra size for the TokenID
    return (((Size + 3) & ~3) + VEX_DMA_TOKEN_ID_SIZE);
#else
    // 32-bit alignment
    return ((Size + 3) & ~3);
#endif
}
#endif

#if 0
/*----------------------------------------------------------------------------
 * vexLocal_CB_CheckClear
 */
#ifdef VEX_CHECK_DMA_WITH_TOKEN_ID
static int
vexLocal_CB_CheckClear(
        void * Buffer_p,
        const size_t Size,
        void * UserData_p)
{
    if (UserData_p != NULL)
    {
        // Zeroize TokenID area
        uint8_t * Ptr_p = (uint8_t *)Buffer_p;
        Ptr_p += (Size - VEX_DMA_TOKEN_ID_SIZE);
        memset(Ptr_p, 0, sizeof(uint32_t));
    }
    return 0;
}
#endif
#endif

#if 0
/*----------------------------------------------------------------------------
 * vexLocal_CB_CheckClear
 */
#ifdef VEX_CHECK_DMA_WITH_TOKEN_ID
static int
vexLocal_CB_CheckReady(
        void * Buffer_p,
        const size_t Size,
        void * UserData_p)
{
    if (UserData_p != NULL)
    {
        int SkipSleep = VEX_POLLING_SKIP_FIRST_DELAYS;
        unsigned int LoopsLeft = VEX_POLLING_MAXLOOPS;
        uint32_t TokenID = (uint32_t)*(uint16_t *)UserData_p;
        uint8_t * Ptr_p = (uint8_t *)Buffer_p;

        // Initialize pointer for check
        Ptr_p += (Size - VEX_DMA_TOKEN_ID_SIZE);

        // Poll for TokenID available
        while (memcmp(Ptr_p, &TokenID, sizeof(uint32_t)) != 0)
        {
            if (SkipSleep > 0)
            {
                // First few rounds are without sleep
                // this avoids sleeping unnecessarily for fast tokens
                SkipSleep--;
            }
            else
            {
                // Sleep a bit
                Adapter_SleepMS(VEX_POLLING_DELAY_MS);
                LoopsLeft--;
                if (LoopsLeft == 0)
                {
                    LOG_WARN(VEX_LOG_PREFIX "%s: TIMEOUT!\n", __func__);
                    return -1;
                }
            }
        }
        LOG_INFO(VEX_LOG_PREFIX "%s: Data ready\n", __func__);
    }
    else
    {
        // Force some delay to increase the chance that the data is available
        Adapter_SleepMS(VEX_POLLING_DELAY_MS);
        Adapter_SleepMS(VEX_POLLING_DELAY_MS);
    }
    return 0;
}
#endif
#endif

#if 0
void
vex_InitBufManager(void)
{
#ifdef VEX_CHECK_DMA_WITH_TOKEN_ID
    (void)BufManager_Register(vexLocal_CB_SizeAlignment,
                              vexLocal_CB_CheckClear,
                              vexLocal_CB_CheckReady);
#else
    (void)BufManager_Register(vexLocal_CB_SizeAlignment,
                              NULL,
                              NULL);
#endif
}
#endif

/* end of file adapter_vex_bufmanager.c */
