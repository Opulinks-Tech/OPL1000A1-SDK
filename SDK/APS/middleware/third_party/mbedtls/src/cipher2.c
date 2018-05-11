/**
 * \file cipher.c
 *
 * \brief Generic cipher wrapper for mbed TLS
 *
 * \author Adriaan de Jong <dejong@fox-it.com>
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_CIPHER_C)

#include "mbedtls/cipher.h"
#include "mbedtls/cipher_internal.h"

#include <stdlib.h>
#include <string.h>


#if defined(MBEDTLS_CCM_C)
#include "mbedtls/ccm.h"
#endif

#if defined(MBEDTLS_AES_C)
#include "mbedtls/aes.h"
#endif


#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_calloc    calloc
#define mbedtls_free       free
#endif





/*=====================
test
=====================*/

#if defined(MBEDTLS_CCM_C)
/* shared by all CCM ciphers */
static void *ccm_ctx_alloc( void )
{
    void *ctx = mbedtls_calloc( 1, sizeof( mbedtls_ccm_context ) );

    if( ctx != NULL )
        mbedtls_ccm_init( (mbedtls_ccm_context *) ctx );

    return( ctx );
}

static void ccm_ctx_free( void *ctx )
{
    mbedtls_ccm_free( ctx );
    mbedtls_free( ctx );
}
#endif /* MBEDTLS_CCM_C */

#if defined(MBEDTLS_AES_C)

static int aes_crypt_ecb_wrap( void *ctx, mbedtls_operation_t operation,
        const unsigned char *input, unsigned char *output )
{
    return mbedtls_aes_crypt_ecb( (mbedtls_aes_context *) ctx, operation, input, output );
}

static int aes_setkey_dec_wrap( void *ctx, const unsigned char *key,
                                unsigned int key_bitlen )
{
    return mbedtls_aes_setkey_dec( (mbedtls_aes_context *) ctx, key, key_bitlen );
}

static int aes_setkey_enc_wrap( void *ctx, const unsigned char *key,
                                unsigned int key_bitlen )
{
    return mbedtls_aes_setkey_enc( (mbedtls_aes_context *) ctx, key, key_bitlen );
}

static void * aes_ctx_alloc( void )
{
    mbedtls_aes_context *aes = mbedtls_calloc( 1, sizeof( mbedtls_aes_context ) );

    if( aes == NULL )
        return( NULL );

    mbedtls_aes_init( aes );

    return( aes );
}

static void aes_ctx_free( void *ctx )
{
    mbedtls_aes_free( (mbedtls_aes_context *) ctx );
    mbedtls_free( ctx );
}

static const mbedtls_cipher_base_t aes_info = {
    MBEDTLS_CIPHER_ID_AES,
    aes_crypt_ecb_wrap,
    aes_setkey_enc_wrap,
    aes_setkey_dec_wrap,
    aes_ctx_alloc,
    aes_ctx_free
};

static const mbedtls_cipher_info_t aes_128_ecb_info = {
    MBEDTLS_CIPHER_AES_128_ECB,
    MBEDTLS_MODE_ECB,
    128,
    "AES-128-ECB",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_192_ecb_info = {
    MBEDTLS_CIPHER_AES_192_ECB,
    MBEDTLS_MODE_ECB,
    192,
    "AES-192-ECB",
    16,
    0,
    16,
    &aes_info
};

static const mbedtls_cipher_info_t aes_256_ecb_info = {
    MBEDTLS_CIPHER_AES_256_ECB,
    MBEDTLS_MODE_ECB,
    256,
    "AES-256-ECB",
    16,
    0,
    16,
    &aes_info
};


#if defined(MBEDTLS_CCM_C)
static int ccm_aes_setkey_wrap( void *ctx, const unsigned char *key,
                                unsigned int key_bitlen )
{
    return mbedtls_ccm_setkey( (mbedtls_ccm_context *) ctx, MBEDTLS_CIPHER_ID_AES,
                     key, key_bitlen );
}

static const mbedtls_cipher_base_t ccm_aes_info = {
    MBEDTLS_CIPHER_ID_AES,
    NULL,
    ccm_aes_setkey_wrap,
    ccm_aes_setkey_wrap,
    ccm_ctx_alloc,
    ccm_ctx_free,
};

static const mbedtls_cipher_info_t aes_128_ccm_info = {
    MBEDTLS_CIPHER_AES_128_CCM,
    MBEDTLS_MODE_CCM,
    128,
    "AES-128-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};

static const mbedtls_cipher_info_t aes_192_ccm_info = {
    MBEDTLS_CIPHER_AES_192_CCM,
    MBEDTLS_MODE_CCM,
    192,
    "AES-192-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};

static const mbedtls_cipher_info_t aes_256_ccm_info = {
    MBEDTLS_CIPHER_AES_256_CCM,
    MBEDTLS_MODE_CCM,
    256,
    "AES-256-CCM",
    12,
    MBEDTLS_CIPHER_VARIABLE_IV_LEN,
    16,
    &ccm_aes_info
};
#endif /* MBEDTLS_CCM_C */

#endif /* MBEDTLS_AES_C */


const mbedtls_cipher_definition_t mbedtls_cipher_definitions[] =
{
#if defined(MBEDTLS_AES_C)
    { MBEDTLS_CIPHER_AES_128_ECB,          &aes_128_ecb_info },
    { MBEDTLS_CIPHER_AES_192_ECB,          &aes_192_ecb_info },
    { MBEDTLS_CIPHER_AES_256_ECB,          &aes_256_ecb_info },
#if defined(MBEDTLS_CCM_C)
    { MBEDTLS_CIPHER_AES_128_CCM,          &aes_128_ccm_info },
    { MBEDTLS_CIPHER_AES_192_CCM,          &aes_192_ccm_info },
    { MBEDTLS_CIPHER_AES_256_CCM,          &aes_256_ccm_info },
#endif
#endif /* MBEDTLS_AES_C */

    { MBEDTLS_CIPHER_NONE, NULL }
};

#define NUM_CIPHERS sizeof mbedtls_cipher_definitions / sizeof mbedtls_cipher_definitions[0]
int mbedtls_cipher_supported[NUM_CIPHERS];

/*=====================
test end
=====================*/




/* Implementation that should never be optimized out by the compiler */
static void mbedtls_zeroize( void *v, size_t n ) {
    volatile unsigned char *p = v; while( n-- ) *p++ = 0;
}



const mbedtls_cipher_info_t *mbedtls_cipher_info_from_values( const mbedtls_cipher_id_t cipher_id,
                                              int key_bitlen,
                                              const mbedtls_cipher_mode_t mode )
{
    const mbedtls_cipher_definition_t *def;

    for( def = mbedtls_cipher_definitions; def->info != NULL; def++ )
        if( def->info->base->cipher == cipher_id &&
            def->info->key_bitlen == (unsigned) key_bitlen &&
            def->info->mode == mode )
            return( def->info );

    return( NULL );
}


void mbedtls_cipher_free( mbedtls_cipher_context_t *ctx )
{
    if( ctx == NULL )
        return;

    if( ctx->cipher_ctx )
        ctx->cipher_info->base->ctx_free_func( ctx->cipher_ctx );

    mbedtls_zeroize( ctx, sizeof(mbedtls_cipher_context_t) );
}

int mbedtls_cipher_setup( mbedtls_cipher_context_t *ctx, const mbedtls_cipher_info_t *cipher_info )
{
    if( NULL == cipher_info || NULL == ctx )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    memset( ctx, 0, sizeof( mbedtls_cipher_context_t ) );

    if( NULL == ( ctx->cipher_ctx = cipher_info->base->ctx_alloc_func() ) )
        return( MBEDTLS_ERR_CIPHER_ALLOC_FAILED );

    ctx->cipher_info = cipher_info;

#if defined(MBEDTLS_CIPHER_MODE_WITH_PADDING)
    /*
     * Ignore possible errors caused by a cipher mode that doesn't use padding
     */
#if defined(MBEDTLS_CIPHER_PADDING_PKCS7)
//    (void) mbedtls_cipher_set_padding_mode( ctx, MBEDTLS_PADDING_PKCS7 );
#else
    (void) mbedtls_cipher_set_padding_mode( ctx, MBEDTLS_PADDING_NONE );
#endif
#endif /* MBEDTLS_CIPHER_MODE_WITH_PADDING */

    return( 0 );
}

int mbedtls_cipher_setkey( mbedtls_cipher_context_t *ctx, const unsigned char *key,
        int key_bitlen, const mbedtls_operation_t operation )
{
    if( NULL == ctx || NULL == ctx->cipher_info )
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );

    if( ( ctx->cipher_info->flags & MBEDTLS_CIPHER_VARIABLE_KEY_LEN ) == 0 &&
        (int) ctx->cipher_info->key_bitlen != key_bitlen )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    ctx->key_bitlen = key_bitlen;
    ctx->operation = operation;

    /*
     * For CFB and CTR mode always use the encryption key schedule
     */
    if( MBEDTLS_ENCRYPT == operation ||
        MBEDTLS_MODE_CFB == ctx->cipher_info->mode ||
        MBEDTLS_MODE_CTR == ctx->cipher_info->mode )
    {
        return ctx->cipher_info->base->setkey_enc_func( ctx->cipher_ctx, key,
                ctx->key_bitlen );
    }

    if( MBEDTLS_DECRYPT == operation )
        return ctx->cipher_info->base->setkey_dec_func( ctx->cipher_ctx, key,
                ctx->key_bitlen );

    return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
}


int mbedtls_cipher_update( mbedtls_cipher_context_t *ctx, const unsigned char *input,
                   size_t ilen, unsigned char *output, size_t *olen )
{
    int ret;

    if( NULL == ctx || NULL == ctx->cipher_info || NULL == olen )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    *olen = 0;

    if( ctx->cipher_info->mode == MBEDTLS_MODE_ECB )
    {
        if( ilen != mbedtls_cipher_get_block_size( ctx ) )
            return( MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED );

        *olen = ilen;

        if( 0 != ( ret = ctx->cipher_info->base->ecb_func( ctx->cipher_ctx,
                    ctx->operation, input, output ) ) )
        {
            return( ret );
        }

        return( 0 );
    }

    if( input == output &&
       ( ctx->unprocessed_len != 0 || ilen % mbedtls_cipher_get_block_size( ctx ) ) )
    {
        return( MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA );
    }

    return( MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE );
}


#endif /* MBEDTLS_CIPHER_C */
