/* valtest_sym_crypto.c
 *
 * Description: VAL Test Suite; Symmetric Crypto functionality
 *              for AES, AES-f8, DES and Triple-DES.
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
#include "testvectors_aes_basic.h"
#if defined(VALTEST_SYM_ALGO_DES) || defined(VALTEST_SYM_ALGO_3DES)
#include "testvectors_des.h"
#endif
#ifdef VALTEST_SYM_ALGO_AES_F8
#include "testvectors_aes_f8.h"
#endif
#ifdef VALTEST_SYM_ALGO_AES_XTS
#include "testvectors_xts_aes.h"
#endif
#ifdef VALTEST_SYM_ALGO_CHACHA20
#include "testvectors_chacha20.h"
#endif

#include "sfzutf.h"
#include "valtest_sym_crypto.h"
#include "msg.h"

/* Foreach macro, for iterating through arrays.
   When reading the next/last element, reading (just) outside the array
   is prevented. */
#define FOREACH(var, index, array)                                      \
    for(((index) = 0),(var) = (array)[0];                               \
        ((index) < sizeof((array))/sizeof((array)[0]));                 \
        (var) = (++(index) < sizeof((array))/sizeof((array)[0]))?       \
            (array)[(index)]: (var))

#define LARGE_BUF_SIZE     16 * 1024 /*It is the large buffer size of 16 KB*/


typedef struct
{
    int Index;
    ValSymContextPtr_t SymContext_p;
    ValAssetId_t KeyAssetId;
    uint8_t * ptx_p;
    uint8_t * ctx_p;
    uint32_t txt_len;
    uint32_t BlockSize;
    bool fPadding;
    uint8_t PaddingByte;
    uint32_t options;
    int misalign;
} SymCryptTestCtx_t;

#define SYM_CRYPT_TEST_IN_PLACE        0x0001
#define SYM_CRYPT_TEST_MULTIPART       0x0002
#define SYM_CRYPT_TEST_UNALIGNED       0x0004


/*----------------------------------------------------------------------------
 * SetupTestContextAndEnvironment
 *
 * Setup for the "do_SymCryptTest" test.
 */
int
SetupTestContextAndEnvironment(
        SymCryptTestCtx_t * TestCntxt_p,
        int Index,
        ValSymAlgo_t Algorithm,
        TestVectors_Mode_t TestMode,
        const uint8_t * const Key_p,
        uint32_t KeySize,
        const uint8_t * const Iv_p,
        const uint8_t * const Ptx_p,
        const uint8_t * const Ctx_p,
        const uint32_t TxtLen)
{
    ValSymMode_t Mode = VAL_SYM_MODE_NONE;
    ValStatus_t Status;
    uint32_t IvSize = 0;

    memset(TestCntxt_p, 0, sizeof(SymCryptTestCtx_t));

    switch (Algorithm)
    {
    case VAL_SYM_ALGO_CIPHER_AES:
        switch (TestMode)
        {
        case TESTVECTORS_MODE_ECB:
            Mode = VAL_SYM_MODE_CIPHER_ECB;
            break;
        case TESTVECTORS_MODE_CBC:
            Mode = VAL_SYM_MODE_CIPHER_CBC;
            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
            break;
        case TESTVECTORS_MODE_CTR:
            Mode = VAL_SYM_MODE_CIPHER_CTR;
            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
            TestCntxt_p->fPadding = true;
            TestCntxt_p->PaddingByte = 0xDC;
            break;
        case TESTVECTORS_MODE_ICM:
            Mode = VAL_SYM_MODE_CIPHER_ICM;
            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
            TestCntxt_p->fPadding = true;
            TestCntxt_p->PaddingByte = 0xDC;
            break;
        case TESTVECTORS_MODE_F8:
            Mode = VAL_SYM_MODE_CIPHER_F8;
            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
            TestCntxt_p->fPadding = true;
            TestCntxt_p->PaddingByte = 0xDC;
            break;
        case TESTVECTORS_MODE_XTS_AES:
            Mode = VAL_SYM_MODE_CIPHER_XTS;
            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
            TestCntxt_p->fPadding = true;
            //TestCntxt_p->PaddingByte = 0x00;
            break;
        default:
            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_AES_BLOCK_SIZE;
        break;
    case VAL_SYM_ALGO_CIPHER_DES:
        // Note: No key Asset is allowed
        switch (TestMode)
        {
        case TESTVECTORS_MODE_ECB:
            Mode = VAL_SYM_MODE_CIPHER_ECB;
            break;
        case TESTVECTORS_MODE_CBC:
            Mode = VAL_SYM_MODE_CIPHER_CBC;
            IvSize = VAL_SYM_ALGO_DES_IV_SIZE;
            break;
        default:
            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_DES_BLOCK_SIZE;
        break;
    case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
        switch (TestMode)
        {
        case TESTVECTORS_MODE_ECB:
            Mode = VAL_SYM_MODE_CIPHER_ECB;
            break;
        case TESTVECTORS_MODE_CBC:
            Mode = VAL_SYM_MODE_CIPHER_CBC;
            IvSize = VAL_SYM_ALGO_DES_IV_SIZE;
            break;
        default:
            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_DES_BLOCK_SIZE;
        break;
#ifdef VALTEST_SYM_ALGO_CHACHA20
    case VAL_SYM_ALGO_CIPHER_CHACHA20:
        Mode = VAL_SYM_MODE_CIPHER_CHACHA20_ENC;
        KeyAssetPolicy = VAL_POLICY_CHACHA20_ENCRYPT;
        IvSize = VAL_SYM_ALGO_CHACHA20_IV_SIZE;
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_CHACHA20_BLOCK_SIZE;
        TestCntxt_p->fPadding = true;
        //TestCntxt_p->PaddingByte = 0x00;
        break;
#endif
    default:
        break;
    }

    Status = val_SymAlloc(Algorithm, Mode, &TestCntxt_p->SymContext_p);
    //unsupported_if((Status == VAL_INVALID_ALGORITHM), "");
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
    if (Status != VAL_SUCCESS) {
        msg_print(LOG_HIGH_LEVEL, "[security] SetupTestContextAndEnvironment, val_SymAlloc error, Status:%d \r\n", Status);
    }

    Status = val_SymInitKey(TestCntxt_p->SymContext_p,
                            TestCntxt_p->KeyAssetId, Key_p, KeySize);
    //fail_if(Status != VAL_SUCCESS, "val_SymInitKey()=", Status);
    if (Status != VAL_SUCCESS) {
        msg_print(LOG_HIGH_LEVEL, "[security] SetupTestContextAndEnvironment, val_SymInitKey error, Status:%d \r\n", Status);
    }

    if (IvSize)
    {
        Status = val_SymInitIV(TestCntxt_p->SymContext_p,
                               Iv_p, IvSize);
        //fail_if(Status != VAL_SUCCESS, "val_SymInitIV()=", Status);
        if (Status != VAL_SUCCESS) {
            msg_print(LOG_HIGH_LEVEL, "[security] SetupTestContextAndEnvironment, val_SymInitIV error, Status:%d \r\n", Status);
        }
    }

    TestCntxt_p->Index = Index;
    //TestCntxt_p->ptx_p = sfzutf_discard_const(Ptx_p);
    //TestCntxt_p->ctx_p = sfzutf_discard_const(Ctx_p);
    TestCntxt_p->txt_len = TxtLen;

    return END_TEST_SUCCES;
}


/*----------------------------------------------------------------------------
 * do_SymCryptTest
 *
 * Perform a test with the 'val_SymCipher' API using the data and
 * options from the given "SymCryptTestCtx_t" test context.
 */
int
do_SymCryptTest(
        SymCryptTestCtx_t * const TestCntxt_p,
        bool fDecrypt)
{
    static uint8_t result_text[VAL_TEST_MAX_BUFLEN];
    static uint8_t input_text[VAL_TEST_MAX_BUFLEN];

    ValSize_t result_len = sizeof(result_text);
    ValStatus_t Status;
    uint8_t * TxtIn_p;
    uint8_t * TxtOut_p;
    uint8_t * Result_p = result_text;
    uint32_t padding = 0;

    if (fDecrypt)
    {//Decrypt
        TxtIn_p = TestCntxt_p->ctx_p;
        TxtOut_p = TestCntxt_p->ptx_p;
    }
    else
    {//Encrypt
        Status = val_SymCipherInitEncrypt(TestCntxt_p->SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymCipherInitEncrypt()=", Status);

        TxtIn_p = TestCntxt_p->ptx_p;
        TxtOut_p = TestCntxt_p->ctx_p;
    }

    if (TestCntxt_p->options & SYM_CRYPT_TEST_IN_PLACE)
    {
        memset(Result_p, 0xDC, result_len);
        memcpy(Result_p, TxtIn_p, TestCntxt_p->txt_len);
        TxtIn_p = Result_p;
    }

    if (TestCntxt_p->options & SYM_CRYPT_TEST_UNALIGNED)
    {
        int offset = 1 + (TestCntxt_p->misalign % 3);

        memset(input_text, 0xDC, sizeof(input_text));
        memcpy(input_text + offset, TxtIn_p, TestCntxt_p->txt_len);
        TxtIn_p = input_text + offset;
        Result_p += 1 + ((offset + 1) % 3);
        result_len -= 1 + ((offset + 1) % 3);
        TestCntxt_p->misalign += 1;
    }

    if (TestCntxt_p->txt_len & (TestCntxt_p->BlockSize - 1))
    {
        //        fail_if(TestCntxt_p->fPadding == false, "Bad length for ECB/CBC ", TestCntxt_p->txt_len);

        padding = (0 - TestCntxt_p->txt_len) & (TestCntxt_p->BlockSize - 1);
        if (!(TestCntxt_p->options & SYM_CRYPT_TEST_UNALIGNED))
        {
            memset(input_text, TestCntxt_p->PaddingByte, (TestCntxt_p->txt_len + padding));
            memcpy(input_text, TxtIn_p, TestCntxt_p->txt_len);
            TxtIn_p = input_text;
        }
    }
	
    if (TestCntxt_p->options & SYM_CRYPT_TEST_MULTIPART)
    {
        ValSize_t src_len = TestCntxt_p->txt_len + padding;
        ValSize_t dst_space = result_len;
        uint8_t * srcpart_p = TxtIn_p;
        uint8_t * dstpart_p = Result_p;

        do
        {
            ValSize_t part_len = MIN(TestCntxt_p->BlockSize, src_len);
            ValSize_t dst_len = dst_space;

            if (src_len == part_len)
            {
                uint8_t * InCopy_p;
                uint8_t * OutCopy_p;

                InCopy_p = (uint8_t *)SFZUTF_MALLOC(part_len);
                //fail_if(InCopy_p == NULL, "Allocation ", (int)part_len);
                if (InCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCryptTest, SFZUTF_MALLOC error \r\n");
                }
                
                OutCopy_p = (uint8_t *)SFZUTF_MALLOC(dst_len);
                //fail_if(OutCopy_p == NULL, "Allocation ", (int)dst_len);
                if (OutCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCryptTest, SFZUTF_MALLOC error \r\n");
                }

                memcpy(InCopy_p, srcpart_p, part_len);
                Status = val_SymCipherFinal(TestCntxt_p->SymContext_p,
                                            InCopy_p, part_len,
                                            OutCopy_p, &dst_len);
                if (Status == VAL_SUCCESS)
                {
                    memcpy(dstpart_p, OutCopy_p, dst_len);
                }

                SFZUTF_FREE(OutCopy_p);
                SFZUTF_FREE(InCopy_p);

                //fail_if(Status != VAL_SUCCESS, "val_SymCipherFinal()=", Status);
            }
            else
            {
                uint8_t * InCopy_p;
                uint8_t * OutCopy_p;

                InCopy_p = (uint8_t *)SFZUTF_MALLOC(part_len);
                //fail_if(InCopy_p == NULL, "Allocation ", (int)part_len);
                if (InCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCryptTest, SFZUTF_MALLOC error \r\n");
                }
                                
                OutCopy_p = (uint8_t *)SFZUTF_MALLOC(dst_len);
                //fail_if(OutCopy_p == NULL, "Allocation ", (int)dst_len);
                if (OutCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCryptTest, SFZUTF_MALLOC error \r\n");
                }

                memcpy(InCopy_p, srcpart_p, part_len);
                Status = val_SymCipherUpdate(TestCntxt_p->SymContext_p,
                                             InCopy_p, part_len,
                                             OutCopy_p, &dst_len);
                if (Status == VAL_SUCCESS)
                {
                    memcpy(dstpart_p, OutCopy_p, dst_len);
                }

                SFZUTF_FREE(OutCopy_p);
                SFZUTF_FREE(InCopy_p);

                //fail_if(Status != VAL_SUCCESS, "val_SymCipherUpdate()=", Status);
            }

            //fail_if(dst_len != TestCntxt_p->BlockSize,"Unexpected partial result length ",(int)dst_len);

            src_len -= part_len;
            dst_space -= dst_len;
            srcpart_p += part_len;
            dstpart_p += part_len;
        } while (src_len > 0);

        /* Let result_len = accumulated result length. */
        result_len -= dst_space;
    }
    else
    {
		{
	        uint8_t * InCopy_p;
	        uint8_t * OutCopy_p;

	        InCopy_p = (uint8_t *)SFZUTF_MALLOC(TestCntxt_p->txt_len + padding);
	        //fail_if(InCopy_p == NULL, "Allocation ", (int)(TestCntxt_p->txt_len + padding));
	        OutCopy_p = (uint8_t *)SFZUTF_MALLOC(result_len);
	        //fail_if(OutCopy_p == NULL, "Allocation ", (int)result_len);

	        memcpy(InCopy_p, TxtIn_p, (TestCntxt_p->txt_len + padding));
	        Status = val_SymCipherFinal(TestCntxt_p->SymContext_p,
	                                    InCopy_p, (TestCntxt_p->txt_len + padding),
	                                    OutCopy_p, &result_len);
	        if (Status == VAL_SUCCESS)
	        {
	            memcpy(Result_p, OutCopy_p, result_len);
	        }

	        SFZUTF_FREE(OutCopy_p);
	        SFZUTF_FREE(InCopy_p);

	        //fail_if(Status != VAL_SUCCESS, "val_SymCipherFinal()=", Status);
		}

    }

    //fail_if(result_len != (ValSize_t)(TestCntxt_p->txt_len + padding),"Unexpected result length: ", (int)result_len);
    //fail_if(memcmp(Result_p, TxtOut_p, TestCntxt_p->txt_len) != 0,"Unexpected output result ", -1);
    if (memcmp(Result_p, TxtOut_p, TestCntxt_p->txt_len) != 0) {

	} else {

	}

    return END_TEST_SUCCES;
}


/*----------------------------------------------------------------------------
 * test_sym_crypto_aes
 *
 * Test with AES test vectors. Intended to be called 6 times.
 * Uses encryption if loopcount _i is even.
 * Uses "in-place" operation when loopcount _i is 2, 3 or 5.
 * Uses "multipart" operation when loopcount is 4 or 5.
 */
void test_sym_crypto_aes()
{
    SymCryptTestCtx_t TestCntxt;
    int Index;
    int Success = 0;
    int Failed = 0;

    for (Index = 0; ; Index++)
    {
        TestVector_AES_BASIC_t tv_p;

        tv_p = test_vectors_aes_basic_get(Index);
        if (tv_p == NULL)
        {
            break;
        }

        if (SetupTestContextAndEnvironment(&TestCntxt,
                                           Index,
                                           VAL_SYM_ALGO_CIPHER_AES,
                                           tv_p->Mode,
                                           tv_p->Key_p,
                                           tv_p->KeyLen,
                                           tv_p->Iv_p,
                                           tv_p->Ptx_p,
                                           tv_p->Ctx_p,
                                           tv_p->PtxLen) == END_TEST_SUCCES)
        {

            TestCntxt.options |= SYM_CRYPT_TEST_IN_PLACE;

            if (do_SymCryptTest(&TestCntxt, true) == END_TEST_SUCCES)
            {
                Success++;
                msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_aes, index:%d result==END_TEST_SUCCES \r\n", Index);
            }
            else
            {
                //LOG_CRIT("Process vector %d\n", Index);
                Failed++;
                msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_aes, index:%d result==END_TEST_FAIL \r\n", Index);
            }
        }
        else
        {
            //LOG_CRIT("Process vector %d\n", Index);
            Failed++;
            msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_aes, index:%d SetupTestContextAndEnvironment fail \r\n", Index);
        }

        test_vectors_aes_basic_release(tv_p);
    }

    //fail_if(Failed, "#wrong tests", Failed);
		msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_aes, Failed:%d \r\n", Failed);
}


/*----------------------------------------------------------------------------
 * test_sym_crypto_chacha20
 *
 * Test with ChaCha20 test vectors. Intended to be called 6 times.
 * Uses encryption if loopcount _i is even.
 * Uses "in-place" operation when loopcount _i is 2 and 3.
 */
#ifdef VALTEST_SYM_ALGO_CHACHA20
void test_sym_crypto_chacha20()
{
    SymCryptTestCtx_t TestCntxt;
    uint8_t IV[VAL_SYM_ALGO_CHACHA20_IV_SIZE];
    int Index;
    int Success = 0;
    int Failed = 0;

    for (Index = 0; ; Index++)
    {
        TestVector_ChaCha20_t tv_p;

        tv_p = test_vectors_chacha20_get(Index);
        if (tv_p == NULL)
        {
            break;
        }

        memset(IV, 0, sizeof(IV));
        memcpy(IV, tv_p->Counter_p, tv_p->CounterLen);
        memcpy(&IV[tv_p->CounterLen], tv_p->Nonce_p, tv_p->NonceLen);

        if (SetupTestContextAndEnvironment(&TestCntxt,
                                           Index,
                                           VAL_SYM_ALGO_CIPHER_CHACHA20,
                                           0,
                                           ((_i & 4) != 0),
                                           tv_p->Key_p,
                                           tv_p->KeyLen,
                                           IV,
                                           tv_p->PlainData_p,
                                           tv_p->CipherData_p,
                                           tv_p->DataLen) == END_TEST_SUCCES)
        {
            if (val_SymCipherInitChaCha20(TestCntxt.SymContext_p,
                                          tv_p->NonceLen) == VAL_SUCCESS)
            {
                if ((_i == 2) || (_i == 3) || (_i == 5))
                {
                    TestCntxt.options |= SYM_CRYPT_TEST_IN_PLACE;
                }

                if (do_SymCryptTest(&TestCntxt, ((_i & 1) == 1)) == END_TEST_SUCCES)
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
        else
        {
            LOG_CRIT("Process vector %d\n", Index);
            Failed++;
        }

        test_vectors_chacha20_release(tv_p);
    }

    fail_if(Failed, "#wrong tests", Failed);
}
#endif


/*----------------------------------------------------------------------------
 * test_sym_crypto_chacha20_keygen
 *
 * Test the ChaCha20 key generation.
 */
#ifdef VALTEST_SYM_ALGO_CHACHA20
void test_sym_crypto_chacha20_keygen()
{
    static const uint8_t Key[] =
    {
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F
    };
    static const uint8_t Nonce[] =
    {
        0x07, 0x00, 0x00, 0x00, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47
    };
    static const uint8_t Result[] =
    {
        0x7B, 0xAC, 0x2B, 0x25, 0x2D, 0xB4, 0x47, 0xAF, 0x09, 0xB6, 0x7A, 0x55, 0xA4, 0xE9, 0x55, 0x84,
        0x0A, 0xE1, 0xD6, 0x73, 0x10, 0x75, 0xD9, 0xEB, 0x2A, 0x93, 0x75, 0x78, 0x3E, 0xD5, 0x53, 0xFF
    };
    SymCryptTestCtx_t TestCntxt;
    uint8_t IV[VAL_SYM_ALGO_CHACHA20_IV_SIZE];
    uint8_t Output[(256/8)];
    ValSize_t ResultSize = (256/8);
    ValStatus_t Status;

    memset(IV, 0, sizeof(IV));
    memcpy(&IV[4], Nonce, sizeof(Nonce));
    memset(Output, 0, sizeof(Output));

    fail_if(SetupTestContextAndEnvironment(&TestCntxt,
                                           0,
                                           VAL_SYM_ALGO_CIPHER_CHACHA20,
                                           0,
                                           false,
                                           Key,
                                           sizeof(Key),
                                           IV,
                                           NULL,
                                           Output,
                                           0) != END_TEST_SUCCES,
            "Context setup", END_TEST_FAIL);

    if (_i)
    {
        fail_if(val_SymCipherInitEncrypt(TestCntxt.SymContext_p) != VAL_SUCCESS,
                "val_SymCipherInitEncrypt()=", VAL_BAD_ARGUMENT);
    }

#ifdef SFZUTF_USERMODE
    Status = val_SymCipherFinal(TestCntxt.SymContext_p,
                                NULL, 0,
                                Output, &ResultSize);
#else
    {
        uint8_t * Output_p = (uint8_t *)SFZUTF_MALLOC(ResultSize);
        fail_if(Output_p == NULL, "Allocation ", (int)ResultSize);

        Status = val_SymCipherFinal(TestCntxt.SymContext_p,
                                    NULL, 0,
                                    Output_p, &ResultSize);
        if (Status == VAL_SUCCESS)
        {
            memcpy(Output, Output_p, ResultSize);
        }

        SFZUTF_FREE(Output_p);
    }
#endif

    fail_if(Status != VAL_SUCCESS, "val_SymCipherFinal()=", Status);
    fail_if(ResultSize != (ValSize_t)(256/8),
            "Unexpected result length: ", (int)ResultSize);
    fail_if(memcmp(Output, Result, ResultSize) != 0,
            "Unexpected output result ", -1);
}
#endif

/*----------------------------------------------------------------------------
 * test_sym_crypto_common
 */
void test_sym_crypto_common(void)
{
    uint8_t Buffer[(128/8)];
    ValSymContextPtr_t SymContext_p;
    ValStatus_t Status;

    /*Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_AES,
                          VAL_SYM_MODE_CIPHER_ECB,
                          NULL);*/
    //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymAlloc()=", Status);
    //msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

#if 0
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER,
                          VAL_SYM_MODE_CIPHER_ECB,
                          &SymContext_p);
    //fail_if(Status != VAL_INVALID_ALGORITHM, "val_SymAlloc()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_AES,
                          VAL_SYM_MODE_NONE,
                          &SymContext_p);
    //fail_if(Status != VAL_INVALID_MODE, "val_SymAlloc()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
#endif

    /*
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_TRIPLE_DES,
                          VAL_SYM_MODE_NONE,
                          &SymContext_p);*/
    //fail_if(Status != VAL_INVALID_MODE, "val_SymAlloc()=", Status);

    // AES-CBC (General cases)
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_AES,
                          VAL_SYM_MODE_CIPHER_CBC,
                          &SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);


#if 0
    Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, 0);
    //fail_if(Status != VAL_INVALID_KEYSIZE, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, (128/8));
    //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
#endif

#if 0
    Status = val_SymInitIV(SymContext_p, Buffer, 0);
    //fail_if(Status != VAL_INVALID_LENGTH, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitIV(SymContext_p, NULL, (128/8));
    //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitIV(BadArgument2)=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
#endif

    Status = val_SymRelease(SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);



    // AES-ECB (Specific case)
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_AES,
                          VAL_SYM_MODE_CIPHER_ECB,
                          &SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

#if 0
    Status = val_SymInitIV(SymContext_p, Buffer, (128/8));
    //fail_if(Status != VAL_UNSUPPORTED, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
#endif

    Status = val_SymRelease(SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);



    // AES-CBC (Specific case)
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_AES,
                          VAL_SYM_MODE_CIPHER_CBC,
                          &SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitIV(SymContext_p, Buffer, (128/8));
    //fail_if(Status != VAL_UNSUPPORTED, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymRelease(SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);



    // 3DES-CBC (General cases)
    /*
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_TRIPLE_DES,
                          VAL_SYM_MODE_CIPHER_CBC,
                          &SymContext_p);*/
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);

#if 0
    Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, 0);
    //fail_if(Status != VAL_INVALID_KEYSIZE, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, (3*(64/8)));
    //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitIV(SymContext_p, Buffer, 0);
    //fail_if(Status != VAL_INVALID_LENGTH, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymInitIV(SymContext_p, NULL, (64/8));
    //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitIV(BadArgument2)=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymRelease(SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    // 3DES-ECB (Specific case)
    /*
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_TRIPLE_DES,
                          VAL_SYM_MODE_CIPHER_ECB,
                          &SymContext_p);*/
    //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);

    Status = val_SymInitIV(SymContext_p, Buffer, (64/8));
    //fail_if(Status != VAL_UNSUPPORTED, "val_SymInitKey()=", Status);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    Status = val_SymRelease(SymContext_p);
    //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
    //msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    // DES-ECB
    Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_DES,
                          VAL_SYM_MODE_CIPHER_ECB,
                          &SymContext_p);
    msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

    if (Status == VAL_SUCCESS)
    {
        Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, 0);
        //fail_if(Status != VAL_INVALID_KEYSIZE, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, (64/8));
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitIV(SymContext_p, Buffer, (64/8));
        //fail_if(Status != VAL_UNSUPPORTED, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymRelease(SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        // DES-CBC
        Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_DES,
                              VAL_SYM_MODE_CIPHER_CBC,
                              &SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, 0);
        //fail_if(Status != VAL_INVALID_KEYSIZE, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID, NULL, (64/8));
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitIV(SymContext_p, Buffer, 0);
        //fail_if(Status != VAL_INVALID_LENGTH, "val_SymInitKey()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitIV(SymContext_p, NULL, (64/8));
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitIV(BadArgument2)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymRelease(SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
    }
    else
    {
        //fail_if(Status != VAL_UNSUPPORTED, "val_SymAlloc(DES)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
    }
#endif

#if 0
    if (valtest_StrictArgsCheck())
    {
        ValSize_t OutputSize = 10;

        Status = val_SymRelease(NULL);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymRelease(BadArgument1)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymInitIV(NULL, Buffer, (128/8));
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymInitIV(BadArgument1)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherInitEncrypt(NULL);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherInitEncrypt(BadArgument1)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymAlloc(VAL_SYM_ALGO_CIPHER_TRIPLE_DES,
                              VAL_SYM_MODE_CIPHER_CBC,
                              &SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherUpdate(NULL, Buffer, (128/8), Buffer, &OutputSize);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument1)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherUpdate(SymContext_p, NULL, (128/8), Buffer, &OutputSize );
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument2)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherUpdate(SymContext_p, Buffer, 0, Buffer, &OutputSize );
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument3)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherUpdate(SymContext_p, Buffer, (128/8), NULL, &OutputSize );
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument4)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherUpdate(SymContext_p, Buffer, (128/8), Buffer, NULL);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument5)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherFinal(NULL, Buffer, (128/8), Buffer, &OutputSize);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument1)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymCipherFinal(SymContext_p, NULL, (128/8), Buffer, &OutputSize);
        //fail_if(Status != VAL_BAD_ARGUMENT, "val_SymCipherUpdate(BadArgument2)=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);

        Status = val_SymRelease(SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymRelease()=", Status);
        msg_print(LOG_HIGH_LEVEL, "[security] test_sym_crypto_common, Status:%d \r\n", Status);
    }
#endif

    
}

/*
int
suite_add_test_SymCrypto(
    struct TestSuite * TestSuite_p)
{
    struct TestCase * TestCase_p;

    TestCase_p = sfzutf_tcase_create(TestSuite_p, "SymCrypto_Chiper_Tests");
    if (TestCase_p != NULL)
    {
        if (sfzutf_tcase_add_fixture(TestCase_p, valtest_initialize, valtest_terminate) != 0)
        {
             goto FuncErrorReturn;
        }

        if (sfzutf_test_add(TestCase_p, test_sym_crypto_common) != 0) goto FuncErrorReturn;

        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_aes, 6) != 0) goto FuncErrorReturn;
#ifdef VALTEST_SYM_ALGO_AES_XTS
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_xts_aes, 6) != 0) goto FuncErrorReturn;
#endif
#ifdef VALTEST_SYM_ALGO_AES_F8
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_aesf8, 4) != 0) goto FuncErrorReturn;
#endif
#if defined(VALTEST_SYM_ALGO_DES) || defined(VALTEST_SYM_ALGO_3DES)
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_des_3des, 6) != 0) goto FuncErrorReturn;
#endif
#ifdef VALTEST_SYM_ALGO_CHACHA20
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_chacha20, 6) != 0) goto FuncErrorReturn;
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_chacha20_keygen, 2) != 0) goto FuncErrorReturn;
#endif

#ifdef SFZUTF_USERMODE
        if (sfzutf_test_add_loop(TestCase_p, test_sym_crypto_aes_unaligned, 2) != 0) goto FuncErrorReturn;
#endif
        if (sfzutf_test_add(TestCase_p, test_sym_crypto_invalid_algorithm) != 0) goto FuncErrorReturn;
        if (sfzutf_test_add(TestCase_p, test_sym_crypto_buffer_too_small) != 0) goto FuncErrorReturn;

        return 0;
    }

FuncErrorReturn:
    return -1;
}
*/


/* end of file valtest_sym_crypto.c */
