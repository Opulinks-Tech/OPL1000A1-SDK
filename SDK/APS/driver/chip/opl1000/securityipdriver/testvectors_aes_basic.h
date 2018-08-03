/* testvectors_aes_basic.h
 *
 * Description: Test vectors for AES for modes ECB, CBC and CTR.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_AES_BASIC_H
#define INCLUDE_GUARD_TESTVECTORS_AES_BASIC_H

#include "basic_defs.h"

/* Beware that the following mode definitions are not guaranteed to be
   numerically identical to the corresponding SFZCRYPTO_MODE_xxx macros. */
typedef enum
{
    TESTVECTORS_MODE_ECB = 0,
    TESTVECTORS_MODE_CBC,
    TESTVECTORS_MODE_CTR,
    TESTVECTORS_MODE_ICM,
    TESTVECTORS_MODE_F8,
    TESTVECTORS_MODE_XTS_AES,
    TESTVECTORS_MODE_NUM        // must be last
} TestVectors_Mode_t;

typedef struct
{
    TestVectors_Mode_t Mode;
    const uint8_t * Key_p;
    const uint8_t * Ptx_p;
    const uint8_t * Ctx_p;
    const uint8_t * Iv_p;
    uint32_t KeyLen;
    uint32_t PtxLen;
} TestVector_AES_BASIC_Rec_t;

typedef const TestVector_AES_BASIC_Rec_t * TestVector_AES_BASIC_t;

/* The function API for accessing the vectors. */

int
test_vectors_aes_basic_num(void);

TestVector_AES_BASIC_t
test_vectors_aes_basic_get(int Index);

void
test_vectors_aes_basic_release(TestVector_AES_BASIC_t Vector_p);

#endif /* Include guard */

/* end of file testvectors_aes_basic.h */
