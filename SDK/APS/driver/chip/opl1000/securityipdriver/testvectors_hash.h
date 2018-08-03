/* testvectors_hash.h
 *
 * Description: Test vectors for various hash test vectors.
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

#ifndef INCLUDE_GUARD_TESTVECTORS_HASH_H
#define INCLUDE_GUARD_TESTVECTORS_HASH_H

#include "basic_defs.h"

typedef enum
{
    TESTVECTORS_HASH_MD5 = 0,
    TESTVECTORS_HASH_SHA160 = 1,
    TESTVECTORS_HASH_SHA224,
    TESTVECTORS_HASH_SHA256,
    TESTVECTORS_HASH_SHA384,
    TESTVECTORS_HASH_SHA512
} TestVectors_HashAlgo_t;

typedef struct
{
    TestVectors_HashAlgo_t Algorithm;
    const uint8_t * Msg_p;
    const uint8_t * Digest_p;
    uint32_t MsgLen;
    uint32_t DigestLen;
} TestVector_HASH_Rec_t;

typedef const TestVector_HASH_Rec_t * TestVector_HASH_t;

/* The function API for accessing the vectors. */

int
test_vectors_hash_num(void);

TestVector_HASH_t
test_vectors_hash_get(
        int Index);

void
test_vectors_hash_release(
        TestVector_HASH_t Vector_p);

#endif /* Include guard */

/* end of file testvectors_hash.h */
