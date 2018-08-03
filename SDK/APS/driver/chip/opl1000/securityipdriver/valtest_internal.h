/* valtest_internal.h
 *
 * Description: Internal utility functions
 */

/*****************************************************************************
* Copyright (c) 2014-2016 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_VAL_TEST_INTERNAL_H
#define INCLUDE_GUARD_VAL_TEST_INTERNAL_H

//#include "c_test_val.h"                 // configuration

#include "basic_defs.h"
#include "clib.h"

//#include "sfzutf_internal.h"            // test environment
#include "api_val.h"                    // API to test

// VAL_TEST_MAX_* defines is used in several places in the test suite,
// including in test vectors
#define VAL_TEST_MAX_BUFLEN         512 /** Maximum general buffer length. */
#define VAL_TEST_MAX_VERSION_LENGTH 256 /** Maximum version information buffer length. */

/* Associated Data for general use */
extern const char * g_ValTestAssociatedData_p;

/* Reset allowed indication */
extern bool g_ResetAllowed;

/* TRNG sample cycles to use */
extern uint16_t g_TrngSampleCycles;

/* Each suite needs to provide this interface */
int build_suite(void);

#if 0
int suite_add_test_System(struct TestSuite * TestSuite_p);
int suite_add_test_Nop(struct TestSuite * TestSuite_p);
int suite_add_test_Random(struct TestSuite * TestSuite_p);
int suite_add_test_AssetManagement(struct TestSuite * TestSuite_p);
int suite_add_test_SymHashHMac(struct TestSuite * TestSuite_p);
int suite_add_test_SymCrypto(struct TestSuite * TestSuite_p);
int suite_add_test_SymAuthCrypto(struct TestSuite * TestSuite_p);
int suite_add_test_SymCipherMac(struct TestSuite * TestSuite_p);
int suite_add_test_SymKeyWrap(struct TestSuite * TestSuite_p);
int suite_add_test_AsymDh(struct TestSuite * TestSuite_p);
int suite_add_test_AsymEccElGamal(struct TestSuite * TestSuite_p);
int suite_add_test_AsymEcdh(struct TestSuite * TestSuite_p);
int suite_add_test_AsymEcdsa(struct TestSuite * TestSuite_p);
int suite_add_test_AsymCurve25519(struct TestSuite * TestSuite_p);
int suite_add_test_AsymRsa(struct TestSuite * TestSuite_p);
int suite_add_test_AsymPk(struct TestSuite * TestSuite_p);
int suite_add_test_SecureTimer(struct TestSuite * TestSuite_p);
int suite_add_test_Service(struct TestSuite * TestSuite_p);
int suite_add_test_FIPS(struct TestSuite * TestSuite_p);
#endif

/*
void valtest_initialize(void);
void valtest_terminate(void);
*/
bool valtest_StrictArgsCheck(void);
ValStatus_t valtest_DefaultTrngConfig(void);
//bool valtest_IsTrngActive(const bool fActivate);

/*----------------------------------------------------------------------------
 * asn1get
 *
 * Simple ASN.1 decoder. Reads the next item (Tag-Length-Value triplet) from
 * 'octets_p'. The actual Tag must match 'tag'. Length must be in 1..0xFFFF.
 * Returns a pointer to the item's Value.
 * Integer values may start with 0x00 to avoid being interpreted as a negative
 * value, but that interpretation is up to the caller.
 */
uint8_t *
asn1get(
        const uint8_t * octets_p,
        size_t * itemlen_p,
        uint8_t tag);

/*----------------------------------------------------------------------------
 * asn1put
 *
 * Simple ASN.1 encoder. Stores the next item (Tag-Length-Value triplet) at
 * 'octets_p'.  Tag must be 0x30 (Sequence) or 0x02 (Integer). Length must be
 * in 0..0xFFFF. Uses 'c_memmove' to copy the item, unless item_p is NULL, e.g
 * for a Sequence.
 * Returns a pointer to the location right after the stored item ('item_p != NULL)
 * or to the start of the Value ('item_p' == NULL).
 */
uint8_t *
asn1put(
        uint8_t * octets_p,
        uint8_t * item_p,
        size_t itemlen,
        uint8_t tag);

bool
valtest_IsCOIDAvailable(void);


#endif /* Include Guard */

/* end of file valtest_internal.h */
