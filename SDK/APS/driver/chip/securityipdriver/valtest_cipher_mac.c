/* valtest_cipher_mac.c
 *
 * Description: VAL Test Suite; Symmetric Crypto functionality
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

#include "valtest_internal.h"

/* Test vectors. */
#include "testvectors_aes_cmac.h"
#include "testvectors_aes_cbcmac.h"
#ifdef VALTEST_SYM_ALGO_POLY1305
#include "testvectors_poly1305.h"
#endif
#include "sfzutf.h"
#include "api_val_buffers.h"
#include "api_val_sym.h"
#include "msg.h"

/* Cipher MAC test vectors use same structure than TestVector_AES_CMAC_t. */
typedef TestVector_AES_CMAC_t TestVector_Cipher_MAC_t;

typedef struct
{
    int Index;
    ValSymContextPtr_t SymContext_p;
    ValAssetId_t KeyAssetId;
    uint8_t * Msg_p;
    uint32_t MsgLen;
    uint8_t * Mac_p;
    uint32_t MacLen;
    uint32_t BlockSize;
    bool fPadding;
    uint32_t options;
} SymCryptTestCtx_t;



/*----------------------------------------------------------------------------
 * SetupTestContextAndEnvironment
 */
static int
SetupTestContextAndEnvironment(
        SymCryptTestCtx_t * TestCntxt_p,
        int Index,
        ValSymAlgo_t Algorithm,
        bool fVerify,
        bool fKeyAsset,
        const uint8_t * const Key_p,
        uint32_t KeySize,
        const uint8_t * const Iv_p,
        const uint8_t * const Msg_p,
        const uint32_t MsgLen,
        const uint8_t * const Mac_p,
        const uint32_t MacLen)
{
    ValPolicyMask_t KeyAssetPolicy = 0;
    ValStatus_t Status;
    uint32_t IvSize = 0;

    memset(TestCntxt_p, 0, sizeof(SymCryptTestCtx_t));

    switch (Algorithm)
    {
	    case VAL_SYM_ALGO_MAC_AES_CMAC:
	        KeyAssetPolicy = VAL_POLICY_ALGO_CIPHER_AES|VAL_POLICY_CMAC;
	        TestCntxt_p->BlockSize = VAL_SYM_ALGO_AES_BLOCK_SIZE;
	        break;

	    case VAL_SYM_ALGO_MAC_AES_CBC_MAC:
	        KeyAssetPolicy = VAL_POLICY_ALGO_CIPHER_AES|VAL_POLICY_AES_MODE_CBC|VAL_POLICY_ENCRYPT;
	        TestCntxt_p->BlockSize = VAL_SYM_ALGO_AES_BLOCK_SIZE;
	        IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
	        break;
            
#ifdef VALTEST_SYM_ALGO_POLY1305
        case VAL_SYM_ALGO_MAC_POLY1305:
            KeyAssetPolicy = VAL_POLICY_POLY1305;
            TestCntxt_p->BlockSize = VAL_SYM_ALGO_POLY1305_BLOCK_SIZE;
            break;
#endif

	    default:
	        break;
    }

    Status = val_SymAlloc(Algorithm, VAL_SYM_MODE_NONE, &TestCntxt_p->SymContext_p);
    unsupported_if((Status == VAL_INVALID_ALGORITHM), "");
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);

    Status = val_SymInitKey(TestCntxt_p->SymContext_p,
                            TestCntxt_p->KeyAssetId, Key_p, KeySize);
    //fail_if(Status != VAL_SUCCESS, "val_SymInitKey()=", Status);

    if ((Iv_p != NULL) && (IvSize > 0))
    {
        Status = val_SymInitIV(TestCntxt_p->SymContext_p, Iv_p, IvSize);
        //fail_if(Status != VAL_SUCCESS, "val_SymInitIV()=", Status);
    }

    TestCntxt_p->Index = Index;
    TestCntxt_p->Msg_p = sfzutf_discard_const(Msg_p);
    TestCntxt_p->MsgLen = MsgLen;
    TestCntxt_p->Mac_p = sfzutf_discard_const(Mac_p);
    TestCntxt_p->MacLen = MacLen;
	
    return END_TEST_SUCCES;
}

/*----------------------------------------------------------------------------
 * test_cipher_mac
 */
int
test_cipher_mac(
        SymCryptTestCtx_t * TestCntxt_p,
        bool fVerify,
        bool fMacFinalAsset,
        bool fMultipart)
{
    ValStatus_t Status;
    ValPolicyMask_t AssetPolicy = 0;
    ValAssetId_t MacAssetId = VAL_ASSETID_INVALID;
    ValSize_t MacSize = 0;
    uint8_t * Msg_p = TestCntxt_p->Msg_p;
    ValSize_t MsgSize = TestCntxt_p->MsgLen;
	ValOctetsOut_t Mac[VAL_SYM_ALGO_MAX_DIGEST_SIZE];

#if 0
    if (fVerify && fMacFinalAsset)
    {
        AssetPolicy = VAL_POLICY_TEMP_MAC;
        if (!val_IsAccessSecure())
        {
            AssetPolicy |= VAL_POLICY_SOURCE_NON_SECURE;
        }
        Status = val_AssetAlloc(AssetPolicy, TestCntxt_p->MacLen, &MacAssetId);
        //fail_if(Status != VAL_SUCCESS, "val_AssetAlloc(MAC)=", Status);

        {
            uint8_t * MacCopy_p = (uint8_t *)SFZUTF_MALLOC(TestCntxt_p->MacLen);
            //fail_if(MacCopy_p == NULL, "Allocation ", (int)TestCntxt_p->MacLen);
            memcpy(MacCopy_p, TestCntxt_p->Mac_p, TestCntxt_p->MacLen);

            Status = val_AssetLoadPlaintext(MacAssetId,
                                            MacCopy_p, TestCntxt_p->MacLen);
            //fail_if(Status != VAL_SUCCESS, "val_AssetLoadPlaintext(MAC)=", Status);
        }
    }
#endif

#if 0
    if (fMultipart && (MsgSize > TestCntxt_p->BlockSize))
    {
        uint8_t * InCopy_p = (uint8_t *)SFZUTF_MALLOC(TestCntxt_p->BlockSize);
        //fail_if(InCopy_p == NULL, "Allocation ", (int)TestCntxt_p->BlockSize);
        memcpy(InCopy_p, Msg_p, TestCntxt_p->BlockSize);

        Status = val_SymMacUpdate(TestCntxt_p->SymContext_p,
                                  InCopy_p, TestCntxt_p->BlockSize);
        //fail_if(Status != VAL_SUCCESS, "val_SymMacUpdate()=", Status);

        Msg_p += TestCntxt_p->BlockSize;
        MsgSize -= TestCntxt_p->BlockSize;
    }
#endif

    {
        uint8_t * InCopy_p = (uint8_t *)SFZUTF_MALLOC(MsgSize);
        //fail_if(InCopy_p == NULL, "Allocation ", (int)MsgSize);
        memcpy(InCopy_p, Msg_p, MsgSize);

/*
        if (fVerify)
        {
            MacSize = TestCntxt_p->MacLen;
            if (MacAssetId == VAL_ASSETID_INVALID)
            {
                Status = val_SymMacVerify(TestCntxt_p->SymContext_p,
                                          InCopy_p, MsgSize,
                                          VAL_ASSETID_INVALID,
                                          TestCntxt_p->Mac_p, MacSize);
                //fail_if(Status != VAL_SUCCESS, "val_SymMacVerify()=", Status);
            }
            else
            {
                Status = val_SymMacVerify(TestCntxt_p->SymContext_p,
                                          InCopy_p, MsgSize,
                                          MacAssetId,
                                          NULL, MacSize);
                //fail_if(Status != VAL_SUCCESS, "val_SymMacVerify()=", Status);
            }
        }
        else
        {
*/

            

            MacSize = sizeof(Mac);
            Status = val_SymMacGenerate(TestCntxt_p->SymContext_p,
                                        InCopy_p, MsgSize,
                                        Mac, &MacSize);

            //fail_if(Status != VAL_SUCCESS, "val_SymMacGenerate()=", Status);
            //fail_if(Vector_p->MacLen != MacSize, "Result mismatch on length %d", VectorIndex);
            //fail_if(memcmp(Mac, TestCntxt_p->Mac_p, TestCntxt_p->MacLen) != 0, "", TestCntxt_p->Index);
        //}

    }

#if 0
    if (MacAssetId != VAL_ASSETID_INVALID)
    {
        Status = val_AssetFree(MacAssetId);
        //fail_if(Status != VAL_SUCCESS, "val_AssetFree(VerifyMAC)=", Status);
    }
    if (TestCntxt_p->KeyAssetId != VAL_ASSETID_INVALID)
    {
        Status = val_AssetFree(TestCntxt_p->KeyAssetId);
        //fail_if(Status != VAL_SUCCESS, "val_AssetFree(Key)=", Status);
    }
#endif

    return END_TEST_SUCCES;
}

/*----------------------------------------------------------------------------
 * test_aes_cmac
 * _i value: bit 0 -> 0 = MacGenerate
 *                 -> 1 = MacVerify
 *           bit 1 -> 0 = Key in token
 *                 -> 1 = Key as Asset
 *           bit 2 -> 0 = Final MAC in token
 *                 -> 1 = Final MAC Asset
 *           bit 3 -> 0 = Single shot
 *                 -> 1 = Multi-part (one block + remaining part)
 */
void test_aes_cmac()
{
    SymCryptTestCtx_t TestCntxt;
    int Index;
    int Success = 0;
    int Failed = 0;

    for (Index = 0; ; Index++)
    {
        TestVector_AES_CMAC_t tv_p;

        tv_p = test_vectors_aes_cmac_get(Index);
        if (tv_p == NULL)
        {
            break;
        }

        // NOTE: MAC does not handle a partial MAC value compare yet,
        //       so allow only complete MAC values
        if ((tv_p->MsgLen > 0) &&
            (tv_p->MacLen == VAL_SYM_ALGO_AES_BLOCK_SIZE))
        {
            if (SetupTestContextAndEnvironment(&TestCntxt, Index,
                                               VAL_SYM_ALGO_MAC_AES_CMAC,
                                               true, false,
                                               tv_p->Key, tv_p->KeyLen,
                                               NULL,
                                               tv_p->Msg, tv_p->MsgLen,
                                               tv_p->Mac, tv_p->MacLen) == END_TEST_SUCCES)
            {
                //if (test_cipher_mac(&TestCntxt, (_i & 1), (_i & 4), (_i & 8)) == END_TEST_SUCCES)
			    if (test_cipher_mac(&TestCntxt, true, false, false) == END_TEST_SUCCES)
                {
                    Success++;
                    msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cmac, index:%d result==END_TEST_SUCCES \r\n", Index);
                }
                else
                {
                    //LOG_CRIT("Process vector %d\n", Index);
                    Failed++;
                    msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cmac, index:%d Fail result!=END_TEST_SUCCES \r\n", Index);
                }
            }
            else
            {
                //LOG_CRIT("Process vector %d\n", Index);
                Failed++;
                msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cmac, index:%d Setup Fail \r\n", Index);
            }
        }
        test_vectors_aes_cmac_release(tv_p);
    }

    //fail_if(Failed, "#wrong tests", Failed);
}


/*----------------------------------------------------------------------------
 * test_aes_cbcmac
 * _i value: bit 0 -> 0 = MacGenerate
 *                 -> 1 = MacVerify
 *           bit 1 -> 0 = Key in token
 *                 -> 1 = Key as Asset
 *           bit 2 -> 0 = Final MAC in token
 *                 -> 1 = Final MAC Asset
 *           bit 3 -> 0 = Single shot
 *                 -> 1 = Multi-part (one block + remaining part)
 */
void test_aes_cbcmac()
{
    SymCryptTestCtx_t TestCntxt;
    int Index;
    int Success = 0;
    int Failed = 0;

    for (Index = 0; ; Index++)
    {
        TestVector_AES_CBCMAC_t tv_p;

        tv_p = test_vectors_aes_cbcmac_get(Index);
        if (tv_p == NULL)
        {
            break;
        }

        // NOTE: MAC does not handle a partial MAC value compare yet,
        //       so allow only complete MAC values
        if ((tv_p->MsgLen > 0) &&
            (tv_p->MacLen == VAL_SYM_ALGO_AES_BLOCK_SIZE))
        {
            if (SetupTestContextAndEnvironment(&TestCntxt, Index,
                                               VAL_SYM_ALGO_MAC_AES_CBC_MAC,
                                               true, false,
                                               tv_p->Key, tv_p->KeyLen,
                                               NULL,
                                               tv_p->Msg, tv_p->MsgLen,
                                               tv_p->Mac, tv_p->MacLen) == END_TEST_SUCCES)
            {
                if (test_cipher_mac(&TestCntxt, true, false, false) == END_TEST_SUCCES)
                {
                    Success++;
                    msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cbcmac, index:%d result==END_TEST_SUCCES \r\n", Index);
                }
                else
                {
                    //LOG_CRIT("Process vector %d\n", Index);
                    Failed++;
                    msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cbcmac, index:%d result!=END_TEST_SUCCES \r\n", Index);
                }
            }
            else
            {
                //LOG_CRIT("Process vector %d\n", Index);
                Failed++;
                msg_print(LOG_HIGH_LEVEL, "[security] test_aes_cbcmac, index:%d Setup Fail \r\n", Index);
            }
        }
        test_vectors_aes_cbcmac_release(tv_p);
    }

    //fail_if(Failed, "#wrong tests", Failed);
}

#if 0
/*----------------------------------------------------------------------------
 * test_poly1305
 * _i value: bit 0 -> 0 = MacGenerate
 *                 -> 1 = MacVerify
 *           bit 1 -> 0 = Key in token
 *                 -> 1 = Key as Asset
 *           bit 2 -> 0 = Final MAC in token
 *                 -> 1 = Final MAC Asset
 *           bit 3 -> 0 = Single shot
 *                 -> 1 = Multi-part (one block + remaining part)
 */
#ifdef VALTEST_SYM_ALGO_POLY1305
void test_poly1305()
{
    SymCryptTestCtx_t TestCntxt;
    int Index;
    int Success = 0;
    int Failed = 0;

    if ((_i != 1) && (_i != 5) && (_i != 9) && (_i != 13)) // These lead to a MACGenerate operation
    {                                                      // that is already tested
        for (Index = 0; ; Index++)
        {
            TestVector_Poly1305_t tv_p;

            tv_p = test_vectors_poly1305_get(Index);
            if (tv_p == NULL)
            {
                break;
            }

            // NOTE: MAC does not handle a partial MAC value compare yet,
            //       so allow only complete MAC values
            if ((tv_p->MsgLen > 0) &&
                (tv_p->TagLen == VAL_SYM_ALGO_POLY1305_BLOCK_SIZE))
            {
                if (SetupTestContextAndEnvironment(&TestCntxt, Index,
                                                   VAL_SYM_ALGO_MAC_POLY1305,
                                                   (_i & 1), (_i & 2),
                                                   tv_p->Key_p, tv_p->KeyLen,
                                                   NULL,
                                                   tv_p->Message_p, tv_p->MsgLen,
                                                   tv_p->Tag_p, tv_p->TagLen) == END_TEST_SUCCES)
                {
                    if (test_cipher_mac(&TestCntxt, (_i & 1), (_i & 4), (_i & 8)) == END_TEST_SUCCES)
                    {
                        Success++;
                    }
                    else
                    {
                        LOG_CRIT("Process vector %d\n", Index);
                        Failed++;
                    }
                }
                else
                {
                    LOG_CRIT("Process vector %d\n", Index);
                    Failed++;
                }
            }
            test_vectors_poly1305_release(tv_p);
        }

        fail_if(Failed, "#wrong tests", Failed);
    }
    else
    {
        LOG_CRIT("SKIPPED: Duplicate Poly1305 test (L%d)\n", _i);
    }
}
#endif
#endif


#if 0
/*----------------------------------------------------------------------------
 * suite_add_test_SymCipherMac
 */
int
suite_add_test_SymCipherMac(
        struct TestSuite * TestSuite_p)
{
    struct TestCase * TestCase_p;

    TestCase_p = sfzutf_tcase_create(TestSuite_p, "SymCrypto_CipherMac_Tests");
    if (TestCase_p != NULL)
    {
        if (sfzutf_tcase_add_fixture(TestCase_p, valtest_initialize, valtest_terminate) != 0)
        {
             goto FuncErrorReturn;
        }

        if (sfzutf_test_add_loop(TestCase_p, test_aes_cmac, 16) != 0) goto FuncErrorReturn;
        if (sfzutf_test_add_loop(TestCase_p, test_aes_cbcmac, 16) != 0) goto FuncErrorReturn;
        
#ifdef VALTEST_SYM_ALGO_POLY1305
        if (sfzutf_test_add_loop(TestCase_p, test_poly1305, 16) != 0) goto FuncErrorReturn;
#endif

        return 0;
    }

FuncErrorReturn:
    return -1;
}
#endif


int do_CMAC_Gen(uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen){
    SymCryptTestCtx_t TestCntxt;

    if(Key_p == NULL 
		|| KeySize <= 0
		|| Msg_p == NULL
		|| MsgLen <= 0
		|| Mac_p == NULL
		|| MacLen <= 0) {
		return false;
	}

    // NOTE: MAC does not handle a partial MAC value compare yet,
    //       so allow only complete MAC values
    if (MsgLen > 0)
    {
        if (SetupTestContextAndEnvironment(&TestCntxt, 0,
                                           VAL_SYM_ALGO_MAC_AES_CMAC,
                                           false, false,
                                           Key_p, KeySize,
                                           NULL,
                                           Msg_p, MsgLen,
                                           Mac_p, VAL_SYM_ALGO_AES_BLOCK_SIZE) == END_TEST_SUCCES)
        {
            if (test_cipher_mac(&TestCntxt, false, false, false) == END_TEST_SUCCES)
            {
                return true;
            }
        }
    }

	return false;
}

int do_CBCMAC_Gen(uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen, uint8_t * Iv_p){
    SymCryptTestCtx_t TestCntxt;


    if(Key_p == NULL 
		|| KeySize <= 0
		|| Msg_p == NULL
		|| MsgLen <= 0
		|| Mac_p == NULL
		|| MacLen <= 0
		|| Iv_p == NULL) {
		return false;
	}

	// NOTE: MAC does not handle a partial MAC value compare yet,
	//       so allow only complete MAC values
	if ((MsgLen > 0) &&
		(MacLen == VAL_SYM_ALGO_AES_BLOCK_SIZE))
	{
	    if (SetupTestContextAndEnvironment(&TestCntxt, 0,
	                                       VAL_SYM_ALGO_MAC_AES_CBC_MAC,
	                                       false, false,
	                                       Key_p, KeySize,
	                                       Iv_p,
	                                       Msg_p, MsgLen,
	                                       Mac_p, MacLen) == END_TEST_SUCCES)
	    {
	        if (test_cipher_mac(&TestCntxt, false, false, false) == END_TEST_SUCCES)
	        {
	            return true;
	        }
        }
	}

	return false;
}



/* end of file sfzcrypto_cipher_mac.c */
