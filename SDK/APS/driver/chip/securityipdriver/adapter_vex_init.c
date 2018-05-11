/* adapter_vex_init.c
 *
 * Adapter VEX initialization module.
 *
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */

// VEX Adapter API
#include "adapter_vex.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

// Default configuration
#include "c_adapter_vex.h"
#include "adapter_vex_internal.h"   // vex_DeviceInit/Exit, vex_InitBufManager
#include "log.h"
#include "msg.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Global constants
 */


/*----------------------------------------------------------------------------
 * Local variables
 */


/*----------------------------------------------------------------------------
 * Local prototypes
 */



/*----------------------------------------------------------------------------
 * vex_Init
 */
int
vex_Init(void)
{
    int rc;

#if 0
    rc = vex_LockInit();
    if (rc < 0)
    {
        LOG_CRIT(VEX_LOG_PREFIX "FAILED to initialize lock (%d)\n", rc);
        return -1;
    }
#endif

    msg_print(LOG_HIGH_LEVEL, "[security] vex_Init, call vex_DeviceInit() \r\n");

    rc = vex_DeviceInit();

    msg_print(LOG_HIGH_LEVEL, "[security] vex_Init, rc:%d \r\n", rc);

#if 0
    if (rc < 0)
    {
        LOG_CRIT(VEX_LOG_PREFIX "FAILED to initialize device (%d)\n", rc);
        if (rc == -1)
        {
        vex_LockExit();
        return -2;
    }

    vex_InitBufManager();
#endif

    return 0;
}


/*----------------------------------------------------------------------------
 * vex_UnInit
 */
void
vex_UnInit(void)
{
    vex_DeviceExit();
    //vex_LockExit();

    return;
}


/* end of file adapter_vex_init.c */

