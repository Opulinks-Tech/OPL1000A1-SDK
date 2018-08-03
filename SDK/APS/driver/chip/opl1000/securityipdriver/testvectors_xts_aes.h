/* testvectors_xts_aes.h
 *
 * Description: Test vectors for XTS-AES-128 and XTS-AES-256.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_XTS_AES_H
#define INCLUDE_GUARD_TESTVECTORS_XTS_AES_H

#include "basic_defs.h"

typedef struct
{
    const uint8_t * Key_p;              // AES-128 or AES-256 keys.
    const uint8_t * Ptx_p;
    const uint8_t * Ctx_p;
    const uint8_t * Tweak_p;            // always 128-bit
    uint32_t KeyLen;                    // 256-bit or 512-bit
    uint32_t PtxLen;
} TestVector_XTS_AES_Rec_t;

typedef const TestVector_XTS_AES_Rec_t * TestVector_XTS_AES_t;


/* The function API for accessing the vectors. */

int
test_vectors_xts_aes_num(void);

TestVector_XTS_AES_t
test_vectors_xts_aes_get(
    int Index);

void
test_vectors_xts_aes_release(
    TestVector_XTS_AES_t Vector_p);

#endif /* Include guard */

/* end of file testvectors_xts_aes.h */
