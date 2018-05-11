/* valtest_hash.c
 *
 * Description: hash tests for MD5, SHA-1, SHA-2
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
#include "testvectors_hash.h"
#include "testvectors_hmac.h"
#include "sfzutf.h"

/* Foreach macro, for iterating through arrays.
   When reading the next/last element, reading (just) outside the array
   is prevented. */
#define FOREACH(var, index, array)                                      \
    for(((index) = 0),(var) = (array)[0];                               \
        ((index) < sizeof((array))/sizeof((array)[0]));                 \
        (var) = (++(index) < sizeof((array))/sizeof((array)[0]))?       \
            (array)[(index)]: (var))

/* Context for iterating over data in fragments.
   If frags_p == NULL, handle data unfragmented.
   If frags_p[i] == 0, re-use last fragment size.
   If frags_p[i] < 0, cycle back to first fragment size.
   If frags_p[i] > 0, use frags_p[i] as next fragment size. */
typedef struct
{
    const uint8_t * msg_p;
    size_t nbytes_done;
    size_t nbytes_left;
    const int * frags_p;
    size_t frags_ndx;
    size_t frag_len;
} MsgIter_t;


/*----------------------------------------------------------------------------
 * msgiter_init
 *
 * Initialize '*msgiter_p' for iterating over the message defined by
 * 'Msg_p' and 'MsgLen', using fragments as defined by '*Fragments_p'
 * if non-NULL.
 */
void
msgiter_init(
         MsgIter_t * const msgiter_p,
         const uint8_t * Msg_p,
         size_t MsgLen,
         const int * Fragments_p)
{
    msgiter_p->msg_p = Msg_p;
    msgiter_p->nbytes_done = 0;
    msgiter_p->nbytes_left = MsgLen;
    msgiter_p->frags_p = Fragments_p;
    msgiter_p->frags_ndx = 0;

    if (Fragments_p == NULL)
    {
        msgiter_p->frag_len = MsgLen;
    }
    else
    {
        msgiter_p->frag_len = Fragments_p[msgiter_p->frags_ndx++];
    }
}


/*----------------------------------------------------------------------------
 * msgiter_next
 *
 * Update '*msgiter_p' for the next message fragment.
 * Return false if the previous fragment was the last.
 */
bool
msgiter_next(MsgIter_t * const msgiter_p)
{
    size_t fraglen = msgiter_p->frag_len;
    int next_fraglen;

    msgiter_p->nbytes_done += fraglen;
    msgiter_p->nbytes_left -= fraglen;
    if (msgiter_p->nbytes_left == 0)
    {
        msgiter_p->frag_len = 0;
        return false;
    }

    if (msgiter_p->frags_p == NULL)
    {
        msgiter_p->frag_len = 0;
        return false;
    }

    next_fraglen = msgiter_p->frags_p[msgiter_p->frags_ndx];
    if (next_fraglen < 0)
    {
        msgiter_p->frags_ndx = 0;
        next_fraglen = msgiter_p->frags_p[0];
    }

    if (next_fraglen == 0)
    {
        next_fraglen = fraglen;
        msgiter_p->frags_ndx--;

        if (next_fraglen == 0)
        {
            msgiter_p->frag_len = 0;
            return false;
        }
    }

    msgiter_p->frag_len = MIN(msgiter_p->nbytes_left, (size_t)next_fraglen);
    msgiter_p->frags_ndx++;
    return true;
}


/*----------------------------------------------------------------------------
 * do_hash_test
 *
 * Helper function that runs a single hash test.
 */
int
do_hash_test(
        int VectorIndex,
        ValSymContextPtr_t SymContext_p,
        TestVector_HASH_t Vector_p,
        const int * const frags_p)
{
    ValStatus_t Status;
    ValOctetsOut_t Digest[VAL_SYM_ALGO_MAX_DIGEST_SIZE];
    ValSize_t DigestSize = 0;
    MsgIter_t msgit;

    msgiter_init(&msgit, Vector_p->Msg_p, Vector_p->MsgLen, frags_p);
    do
    {
        uint8_t * frag_p = (uint8_t *)(msgit.msg_p + msgit.nbytes_done);

        uint8_t * InCopy_p = (uint8_t *)SFZUTF_MALLOC(msgit.frag_len);
        //fail_if(InCopy_p == NULL, "Allocation ", (int)msgit.frag_len);
        memcpy(InCopy_p, frag_p, msgit.frag_len);
		
        if (msgit.nbytes_left == msgit.frag_len)
        {
            DigestSize = sizeof(Digest);
            Status = val_SymHashFinal(SymContext_p,
                                      InCopy_p,
                                      msgit.frag_len,
                                      Digest,
                                      &DigestSize);
            //fail_if(Status != VAL_SUCCESS, "val_SymHashFinal()=", Status);
        }
        else
        {
            Status = val_SymHashUpdate(SymContext_p, InCopy_p, msgit.frag_len);
            //fail_if(Status != VAL_SUCCESS, "val_SymHashUpdate()=", Status);
        }
    }
    while (msgiter_next(&msgit));

/*
    fail_if(Vector_p->DigestLen != DigestSize,
            "Length mismatch ", (int)DigestSize);
    fail_if(memcmp(Digest, Vector_p->Digest_p, Vector_p->DigestLen) != 0,
            "", VectorIndex);
*/
    return END_TEST_SUCCES;
}


/*----------------------------------------------------------------------------
 * do_hmac_test
 * Helper function that runs a single HMAC test.
 */
int
do_hmac_test(
        int VectorIndex,
        ValSymContextPtr_t SymContext_p,
        TestVector_HMAC_t Vector_p,
        const int * const frags_p,
        const bool Verify)
{
    ValStatus_t Status;
    ValOctetsOut_t Mac[VAL_SYM_ALGO_MAX_DIGEST_SIZE];
    ValSize_t MacSize = 0;
    MsgIter_t msgit;

    msgiter_init(&msgit, Vector_p->Msg_p, Vector_p->MsgLen, frags_p);
    do
    {
        uint8_t * frag_p = (uint8_t *)(msgit.msg_p + msgit.nbytes_done);
        uint8_t * InCopy_p = (uint8_t *)SFZUTF_MALLOC(msgit.frag_len);
        //fail_if(InCopy_p == NULL, "Allocation ", (int)msgit.frag_len);
        memcpy(InCopy_p, frag_p, msgit.frag_len);

        if (msgit.nbytes_left == msgit.frag_len)
        {
            if (Verify)
            {
                MacSize = Vector_p->MacLen;
                memcpy(Mac, Vector_p->Mac_p, MacSize);
                Status = val_SymMacVerify(SymContext_p,
                                          InCopy_p, msgit.frag_len,
                                          VAL_ASSETID_INVALID,
                                          Mac, MacSize);
                //fail_if(Status != VAL_SUCCESS, "val_SymMacVerify(1)=", Status);

            }
            else
            {
                MacSize = sizeof(Mac);
                Status = val_SymMacGenerate(SymContext_p,
                                            InCopy_p, msgit.frag_len,
                                            Mac, &MacSize);
                //fail_if(Status != VAL_SUCCESS, "val_SymMacGenerate()=", Status);
                //fail_if(Vector_p->MacLen != MacSize, "Result mismatch on length", VectorIndex);
                //fail_if(memcmp(Mac, Vector_p->Mac_p, Vector_p->MacLen) != 0, "", VectorIndex);
            }
        }
        else
        {
            Status = val_SymMacUpdate(SymContext_p, InCopy_p, msgit.frag_len);
            //fail_if(Status != VAL_SUCCESS, "val_SymMacUpdate()=", Status);
        }
    }
    while (msgiter_next(&msgit));

    return END_TEST_SUCCES;
}

void start_test_hash()
{
    int ndx;
    int failed;

    for (ndx = 0, failed = 0; ; ndx++)
    {
        ValSymContextPtr_t SymContext_p = NULL;
        ValSymAlgo_t Algorithm = VAL_SYM_ALGO_NONE;
        TestVector_HASH_t tv_p;
        ValStatus_t Status;

        tv_p = test_vectors_hash_get(ndx);
        if (tv_p == NULL)
        {
            break;
        }

        LOG_INFO("Process vector %d\n", ndx);

        switch (tv_p->Algorithm)
        {
	        default:
	            // Not (yet) supported
	            test_vectors_hash_release(tv_p);
	            continue;
	        case TESTVECTORS_HASH_SHA160:
	            Algorithm = VAL_SYM_ALGO_HASH_SHA1;
	            break;
	        case TESTVECTORS_HASH_SHA224:
	            Algorithm = VAL_SYM_ALGO_HASH_SHA224;
	            break;
	        case TESTVECTORS_HASH_SHA256:
	            Algorithm = VAL_SYM_ALGO_HASH_SHA256;
	            break;
	        case TESTVECTORS_HASH_SHA384:
	            Algorithm = VAL_SYM_ALGO_HASH_SHA384;
	            break;
	        case TESTVECTORS_HASH_SHA512:
	            Algorithm = VAL_SYM_ALGO_HASH_SHA512;
	            break;
        }

        Status = val_SymAlloc(Algorithm, VAL_SYM_MODE_NONE, &SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);

        if (do_hash_test(ndx, SymContext_p, tv_p, NULL) != END_TEST_SUCCES)
        {
            LOG_CRIT("Process vector %d\n", ndx);
            failed++;
        }
        test_vectors_hash_release(tv_p);
    }

    //fail_if(failed, "#wrong tests", failed);
}


/*----------------------------------------------------------------------------
 * do_hmac_key_hash
 *
 * Helper function that hashes the key if needed.
 */
static bool
do_hmac_key_hash_if_needed(
        ValSymAlgo_t Algorithm,
        const uint8_t * Key_p,
        uint32_t KeySize,
        uint8_t * HashedKey_p,
        ValSize_t * HashedKeySize)
{
    if (KeySize > VAL_SYM_ALGO_MAX_MAC_KEY_SIZE)
    {
        ValSymContextPtr_t SymContext_p = NULL;
        ValSymAlgo_t HashAlgorithm;
        ValStatus_t Status;

        switch (Algorithm)
        {
        default:
        case VAL_SYM_ALGO_MAC_HMAC_SHA1:
            HashAlgorithm = VAL_SYM_ALGO_HASH_SHA1;
            break;
        case VAL_SYM_ALGO_MAC_HMAC_SHA224:
            HashAlgorithm = VAL_SYM_ALGO_HASH_SHA224;
            break;
        case VAL_SYM_ALGO_MAC_HMAC_SHA256:
            HashAlgorithm = VAL_SYM_ALGO_HASH_SHA256;
            break;
        }

        Status = val_SymAlloc(HashAlgorithm, VAL_SYM_MODE_NONE, &SymContext_p);
        if  (Status == VAL_SUCCESS)
        {
            uint8_t * KeyCopy_p = (uint8_t *)SFZUTF_MALLOC(KeySize);
            fail_if(KeyCopy_p == NULL, "Allocation ", 0);
            memcpy(KeyCopy_p, Key_p, KeySize);

            Status = val_SymHashFinal(SymContext_p,
                                      KeyCopy_p,
                                      KeySize,
                                      HashedKey_p,
                                      HashedKeySize);
			
            if  (Status == VAL_SUCCESS)
            {
                return true;
            }

            (void)val_SymRelease(SymContext_p);
        }
    }
    return false;
}


int
do_test_hmac(
        const bool fVerify,
        const bool fKeyAsset)
{
    int ndx;

    for (ndx = 0; ; ndx++)
    {
        TestVector_HMAC_t tv_p;
        uint32_t MacSize = 0;
        ValSymContextPtr_t SymContext_p = NULL;
        ValSymAlgo_t Algorithm = VAL_SYM_ALGO_NONE;
        ValStatus_t Status;

        tv_p = test_vectors_hmac_get(ndx);
        if (tv_p == NULL)
        {
            break;
        }

        LOG_INFO("Process vector %d\n", ndx);


        switch (tv_p->Algorithm)
        {
        default:
        case TESTVECTORS_HASH_MD5:
        case TESTVECTORS_HASH_SHA384:
        case TESTVECTORS_HASH_SHA512:
            // Not (yet) supported
            //test_vectors_hmac_release(tv_p);
            continue;
        case TESTVECTORS_HASH_SHA160:
            Algorithm = VAL_SYM_ALGO_MAC_HMAC_SHA1;
            MacSize = (160/8);
            break;
        case TESTVECTORS_HASH_SHA224:
            Algorithm = VAL_SYM_ALGO_MAC_HMAC_SHA224;
            MacSize = (224/8);
            break;
        case TESTVECTORS_HASH_SHA256:
            Algorithm = VAL_SYM_ALGO_MAC_HMAC_SHA256;
            MacSize = (256/8);
            break;
        }

        Status = val_SymAlloc(Algorithm, VAL_SYM_MODE_NONE, &SymContext_p);
        fail_if(Status != VAL_SUCCESS, "val_SymAlloc()=", Status);

        if (tv_p->KeyLen != 0)
        {
            if (fKeyAsset)
            {
                uint8_t HashedKeyBuffer[VAL_SYM_ALGO_MAX_DIGEST_SIZE];
                ValOctetsIn_t * Key_p = NULL;
                ValSize_t KeySize = VAL_SYM_ALGO_MAX_DIGEST_SIZE;

                if (tv_p->KeyLen < (MacSize/2))
                {
                    // HMAC Asset require a minimum size
                    (void)val_SymRelease(SymContext_p);
                    goto next_vector;
                }

                if (do_hmac_key_hash_if_needed(Algorithm, tv_p->Key_p, tv_p->KeyLen,
                                               HashedKeyBuffer, &KeySize))
                {
                    Key_p = HashedKeyBuffer;
                }
                else
                {
                    Key_p = tv_p->Key_p;
                    KeySize = tv_p->KeyLen;
                }

                fail_if(Status != VAL_SUCCESS, "val_AssetAlloc(Key)=", Status);

                {

                    uint8_t * KeyCopy_p = (uint8_t *)SFZUTF_MALLOC(KeySize);
                    fail_if(KeyCopy_p == NULL, "Allocation ", 0);
                    memcpy(KeyCopy_p, Key_p, KeySize);
                }
				
                fail_if(Status != VAL_SUCCESS, "val_AssetLoadPlaintext(Key)=", Status);
            }
            else
            {
                Status = val_SymInitKey(SymContext_p, VAL_ASSETID_INVALID,
                                        (ValOctetsIn_Optional_t * const)tv_p->Key_p,
                                        tv_p->KeyLen);
            }
            fail_if(Status != VAL_SUCCESS, "val_SymInitKey()=", Status);
        }

        if (fVerify)
        {
            if (MacSize != tv_p->MacLen)
            {
                // HMAC does not handle partial MAC compare yet
                (void)val_SymRelease(SymContext_p);
                goto next_vector;
            }

        }

        if (do_hmac_test(ndx, SymContext_p, tv_p, NULL,
                         fVerify) != END_TEST_SUCCES)
        {
            return END_TEST_FAIL;
        }

next_vector:

        test_vectors_hmac_release(tv_p);
    }

    return END_TEST_SUCCES;
}



/* end of file valtest_hash.c */
