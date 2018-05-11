/* valtest_system.c
 *
 * Description: System related tests
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

#include "valtest_internal.h"



void test_system_reset()
{
    char versionString[VAL_TEST_MAX_VERSION_LENGTH + 1];
    ValSize_t versionStringLen;
    ValStatus_t Status;

    Status = val_SystemReset();
    //fail_if(Status != VAL_SUCCESS, "val_SystemReset()=", Status);

    versionStringLen = VAL_TEST_MAX_VERSION_LENGTH;
    Status = val_SystemGetVersion((ValOctetsOut_t *)versionString,
                                  &versionStringLen);
    //fail_if(Status != VAL_SUCCESS, "val_SystemGetVersion()=", Status);
}


/* end of file valtest_system.c */
