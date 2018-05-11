/* testvectors_aes_cmac.c
 *
 * Description: Test vectors for AES CMAC.
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

#include "testvectors_aes_cmac.h"

/* Use macro to build cmac_test_vectors. */
#define TEST_VECTOR_AES_CMAC(key, keylen, msg, msglen, mac, maclen) \
    {                                                      \
        test_vector_keys + (key),                          \
        test_vector_msgs + (msg),                          \
        test_vector_macs + (mac),                          \
        keylen,                                            \
        msglen,                                            \
        maclen,                                            \
    }

/* Actual vectors, from automatically generated file. */
#include "testvectors_aes_cmac_data.h"

/* The function API for accessing the vectors. */

int
test_vectors_aes_cmac_num(void)
{
    return sizeof(cmac_test_vectors) / sizeof(cmac_test_vectors[0]);
}

TestVector_AES_CMAC_t
test_vectors_aes_cmac_get(int Index)
{
    if (Index >= test_vectors_aes_cmac_num())
    {
         return NULL;
    }
    return &cmac_test_vectors[Index];
}

void
test_vectors_aes_cmac_release(TestVector_AES_CMAC_t Vector_p)
{
    /* Test vectors are statically defined => nothing to do. */
    IDENTIFIER_NOT_USED(Vector_p);
}

/* end of file testvectors_aes_cmac.c */
