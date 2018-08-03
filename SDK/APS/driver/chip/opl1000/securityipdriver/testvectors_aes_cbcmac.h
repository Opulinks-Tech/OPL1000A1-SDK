/* testvectors_aes_cbcmac.h
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

#ifndef INCLUDE_GUARD_TESTVECTORS_AES_CBCMAC_H
#define INCLUDE_GUARD_TESTVECTORS_AES_CBCMAC_H

#include "basic_defs.h"

/* Structure for Test Vectors.
   AES-CBCMAC uses same structure than cmac tests, to allow
   easier usage of the test vectors together. */

#include "testvectors_aes_cmac.h"

typedef TestVector_AES_CMAC_Rec_t TestVector_AES_CBCMAC_Rec_t;
typedef TestVector_AES_CMAC_t     TestVector_AES_CBCMAC_t;

/* API for using AES CBCMAC test vectors. */

/* Request number of AES CBCMAC test vectors available. */
int test_vectors_aes_cbcmac_num(void);

/* Request test vector by index.
   If Index >= test_vectors_aes_cbcmac_num(), the function returns NULL.

   Note: The function returns zero only when invalid vector has been requested.
   Note: The received vector is constant and must not be altered by caller.
   The test vector is described by structure TestVector_AES_CBCMAC_t and
   the structure shall be accessed directly by the user of the test vector.
*/
TestVector_AES_CBCMAC_t test_vectors_aes_cbcmac_get(int Index);

/* Release a vector.
   It is mandatory to release a vector once user of test vector is finished
   with the vector. */
void test_vectors_aes_cbcmac_release(TestVector_AES_CBCMAC_t Vector_p);



#endif /* Include guard */

/* end of file testvectors_aes_cbcmac.h */
