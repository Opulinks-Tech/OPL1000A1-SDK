/* testvectors_hmac.h
 *
 * Description: Test vectors for various HMAC test vectors.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_HMAC_H
#define INCLUDE_GUARD_TESTVECTORS_HMAC_H

#include "basic_defs.h"
#include "testvectors_hash.h"       // TestVectors_HashAlgo_t

typedef struct
{
    TestVectors_HashAlgo_t Algorithm;
    const uint8_t * Key_p;
    uint32_t KeyLen;
    const uint8_t * Msg_p;
    uint32_t MsgLen;
    const uint8_t * Mac_p;
    uint32_t MacLen;
} TestVector_HMAC_Rec_t;

typedef const TestVector_HMAC_Rec_t * TestVector_HMAC_t;

/* The function API for accessing the vectors. */

int
test_vectors_hmac_num(void);

TestVector_HMAC_t
test_vectors_hmac_get(int Index);

void
test_vectors_hmac_release(TestVector_HMAC_t Vector_p);

#endif /* Include guard */

/* end of file testvectors_hmac.h */
