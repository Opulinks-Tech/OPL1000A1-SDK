/* testvectors_aes_cmac.h
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

#ifndef INCLUDE_GUARD_TESTVECTORS_AES_CMAC_H
#define INCLUDE_GUARD_TESTVECTORS_AES_CMAC_H

#include "basic_defs.h"

/* Structure for Test Vectors. */
typedef struct
{
    const uint8_t *Key;
    const uint8_t *Msg;
    const uint8_t *Mac;
    uint32_t KeyLen;
    uint32_t MsgLen;
    uint32_t MacLen;
} TestVector_AES_CMAC_Rec_t;

typedef const TestVector_AES_CMAC_Rec_t * TestVector_AES_CMAC_t;

/* API for using AES CMAC test vectors. */

/* Request number of AES CMAC test vectors available. */
int test_vectors_aes_cmac_num(void);

/* Request test vector by index.
   If Index >= test_vectors_aes_cmac_num(), the function returns NULL.

   Note: The function returns zero only when invalid vector has been requested.
   Note: The received vector is constant and must not be altered by caller.
   The test vector is described by structure TestVector_AES_CMAC_t and
   the structure shall be accessed directly by the user of the test vector.
*/
TestVector_AES_CMAC_t test_vectors_aes_cmac_get(int Index);

/* Release a vector.
   It is mandatory to release a vector once user of test vector is finished
   with the vector. */
void test_vectors_aes_cmac_release(TestVector_AES_CMAC_t Vector_p);

#if 0
/* Search test vector with specific properties.
   The (inline) helper function provides an test vector with
   KeyMin <= KeyLen <= KeyMax
   and
   MsgMin <= MsgLen <= MsgMax.

   Returns NULL if no suitable vector was found.
 */
TestVector_AES_CMAC_t test_vectors_aes_cmac_search(
        uint32_t KeyMin,
        uint32_t KeyMax,
        uint32_t MsgMin,
        uint32_t MsgMax)
{
    TestVector_AES_CMAC_t vector_p;
    int i;

    for (i = 0; ; i++)
    {
        vector_p = test_vectors_aes_cmac_get(i);

        if (!vector_p)
            break;

        if (vector_p->KeyLen >= KeyMin &&
            vector_p->KeyLen <= KeyMax &&
            vector_p->MsgLen >= MsgMin &&
            vector_p->MsgLen <= MsgMax)
        {
            break;
        }
    }
    return vector_p;
}
#endif


#endif /* Include guard */

/* end of file testvectors_aes_cmac.h */
