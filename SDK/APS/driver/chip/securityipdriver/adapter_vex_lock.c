
/* adapter_vex_lock.c
 *
 * Implementation of the VaultIP Exchange.
 *
 * This file implements the locking specific functions.
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
#include "adapter_lock.h"           // Adapter_Lock_t, Adapter_Lock*()
#include "adapter_sleep.h"          // Adapter_SleepMS()

// Lock and critical section for vex_Init/Uninit()
//static ADAPTER_LOCK_DEFINE(gl_VexInitLock);
static Adapter_Lock_CS_t gl_VexInitCS;
static uint32_t gl_VexLockRefCnt = 0;

#if 0

/*----------------------------------------------------------------------------
 * vex_LockInit
 */
int
vex_LockInit(void)
{
    // Initialize lock mechanism
    Adapter_Lock_CS_Set(&gl_VexInitCS, &gl_VexInitLock);

    if (vex_LockAcquire() == 0)
    {
        // Increment reference count
        gl_VexLockRefCnt++;

        Adapter_Lock_CS_Leave(&gl_VexInitCS);
        return 0;
    }

    return -1;
}
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockExit
 */
void
vex_LockExit(void)
{
    // Make sure that the lock mechanism is initialized
    Adapter_Lock_CS_Set(&gl_VexInitCS, &gl_VexInitLock);

    if (vex_LockAcquire() == 0)
    {
        // Decrement reference count
        gl_VexLockRefCnt--;

        Adapter_Lock_CS_Leave(&gl_VexInitCS);
    }
}
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockAcquire
 */
int
vex_LockAcquire(void)
{
#if 0
    while (!Adapter_Lock_CS_Enter(&gl_VexInitCS))
    {
#ifdef MODULE
        schedule();
#else
        sched_yield();
#endif
    }
#endif
    return 0;
}
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockRelease
 */
void
vex_LockRelease(void)
{
    //Adapter_Lock_CS_Leave(&gl_VexInitCS);
}
#endif

/* end of file adapter_vex_lock.c */

