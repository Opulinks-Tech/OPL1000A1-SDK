/* testvectors_aes_f8.h
 *
 * Description: Test vectors for AES-f8.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_AES_F8_H
#define INCLUDE_GUARD_TESTVECTORS_AES_F8_H

#include "basic_defs.h"

typedef struct
{
    const uint8_t * Key_p;
    const uint8_t * Ptx_p;
    const uint8_t * Ctx_p;
    const uint8_t * Iv_p;
    const uint8_t * SaltKey_p;
    uint32_t KeyLen;
    uint32_t PtxLen;
    uint32_t SaltKeyLen;
} TestVector_AES_f8_Rec_t;

typedef const TestVector_AES_f8_Rec_t * TestVector_AES_f8_t;

/* The function API for accessing the vectors. */

int
test_vectors_aes_f8_num(void);

TestVector_AES_f8_t
test_vectors_aes_f8_get(int Index);

void
test_vectors_aes_f8_release(TestVector_AES_f8_t Vector_p);

#endif /* Include guard */

/* end of file testvectors_aes_f8.h */
