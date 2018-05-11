/* eip130_token_crypto.h
 *
 * Security Module Token helper functions
 * - Crypto token related functions and definitions
 *
 * This module can convert a set of parameters into a Security Module Command
 * token, or parses a set of parameters from a Security Module Result token.
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_CRYPTO_H
#define INCLUDE_GUARD_EIP130TOKEN_CRYPTO_H

#include "basic_defs.h"             // uint32_t, bool, inline, etc.
#include "cs_eip130_token.h"        // EIP130TOKEN_ENABLE_*
#include "eip130_token_common.h"    // Eip130Token_Command/Result_t


enum
{
    EIP130TOKEN_CRYPTO_ALGO_AES      = 0,
    EIP130TOKEN_CRYPTO_ALGO_DES      = 1,
    EIP130TOKEN_CRYPTO_ALGO_3DES     = 2,
    EIP130TOKEN_CRYPTO_ALGO_CHACHA20 = 7,
};

enum
{
    EIP130TOKEN_CRYPTO_MODE_ECB     = 0,
    EIP130TOKEN_CRYPTO_MODE_CBC     = 1,
    EIP130TOKEN_CRYPTO_MODE_CTR     = 2,
    EIP130TOKEN_CRYPTO_MODE_AES_ICM = 3,
    EIP130TOKEN_CRYPTO_MODE_AES_f8  = 4,
    EIP130TOKEN_CRYPTO_MODE_AES_CCM = 5,
    EIP130TOKEN_CRYPTO_MODE_AES_XTS = 6,
    EIP130TOKEN_CRYPTO_MODE_AES_GCM = 7,
    EIP130TOKEN_CRYPTO_MODE_ENCRYPT = 0,
    EIP130TOKEN_CRYPTO_MODE_AEAD    = 1,
};



#endif /* Include Guard */

/* end of file eip130_token_crypto.h */
