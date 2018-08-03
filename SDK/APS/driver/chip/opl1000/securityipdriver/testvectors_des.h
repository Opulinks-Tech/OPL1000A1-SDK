/* testvectors_des.h
 *
 * Description: Test vectors for DES.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_DES_H
#define INCLUDE_GUARD_TESTVECTORS_DES_H

#include "basic_defs.h"
#include "testvectors_aes_basic.h"

/* Re-use AES basic test vector type for DES. */
typedef const TestVector_AES_BASIC_Rec_t * TestVector_DES_t;


#if 0
/* The function API for accessing the vectors. */

int
test_vectors_des_num(void);

TestVector_AES_BASIC_t
test_vectors_des_get(int Index);

void
test_vectors_des_release(TestVector_AES_BASIC_t Vector_p);
#endif


#endif /* Include guard */

/* end of file testvectors_des.h */
