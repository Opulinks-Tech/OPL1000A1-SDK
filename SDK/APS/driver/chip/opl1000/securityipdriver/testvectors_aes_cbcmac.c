/* testvectors_aes_cbcmac.c
 *
 * Description: Test vectors for AES CBC-MAC.
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

#include "basic_defs.h"

#include "testvectors_aes_cbcmac.h"

/* Use macro to build cbcmac_test_vectors. */
#define TEST_VECTOR_AES_CBCMAC(key, keylen, msg, msglen, mac, maclen) \
    {                                                      \
        test_vector_keys + (key),                          \
        test_vector_msgs + (msg),                          \
        test_vector_macs + (mac),                          \
        keylen,                                            \
        msglen,                                            \
        maclen,                                            \
    }

/* Actual vectors, from automatically generated file. */
#include "testvectors_aes_cbcmac_data.h"

/* The function API for accessing the vectors. */

int
test_vectors_aes_cbcmac_num(void)
{
    return sizeof(cbcmac_test_vectors) / sizeof(cbcmac_test_vectors[0]);
}

TestVector_AES_CBCMAC_t
test_vectors_aes_cbcmac_get(int Index)
{
    if (Index >= test_vectors_aes_cbcmac_num()) return NULL;
    return &cbcmac_test_vectors[Index];
}

void
test_vectors_aes_cbcmac_release(TestVector_AES_CBCMAC_t Vector_p)
{
    /* Test vectors are statically defined => nothing to do. */
    IDENTIFIER_NOT_USED(Vector_p);
}

/* Search test vector with specific properties.
   The (inline) helper function provides an test vector with
   KeyMin <= KeyLen <= KeyMax
   and
   MsgMin <= MsgLen <= MsgMax.

   Returns NULL if no suitable vector was found.
 */
TestVector_AES_CBCMAC_t test_vectors_aes_cbcmac_search(
        uint32_t KeyMin,
        uint32_t KeyMax,
        uint32_t MsgMin,
        uint32_t MsgMax)
{
    TestVector_AES_CBCMAC_t vector_p;
    int i;

    for (i = 0; ; i++)
    {
        vector_p = test_vectors_aes_cbcmac_get(i);

        if (!vector_p)
            break;

        if (vector_p->KeyLen >= KeyMin &&
            vector_p->KeyLen <= KeyMax &&
            vector_p->MsgLen >= MsgMin &&
            vector_p->KeyLen <= MsgMax)
        {
            break;
        }
    }

    return vector_p;
}


/* end of file testvectors_aes_cbcmac.c */
