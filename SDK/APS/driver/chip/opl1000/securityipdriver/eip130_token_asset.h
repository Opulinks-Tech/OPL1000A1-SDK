/* eip130_token_asset.h
 *
 * Security Module Token helper functions
 * - Asset Management token related functions and definitions
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_ASSET_H
#define INCLUDE_GUARD_EIP130TOKEN_ASSET_H

#include "basic_defs.h"             // uint32_t, bool, inline, etc.
#include "eip130_token_common.h"    // Eip130Token_Command/Result_t

// SM accepts a 6-bit number when searching for a static asset.
#define EIP130TOKEN_STATIC_ASSET_NUMBER_MAX (MASK_6_BITS)

#define EIP130TOKEN_ASSET_ADD_MINIMUM_LENGTH  33
#define EIP130TOKEN_ASSET_ADD_PADDING_VALUE   0

/*----------------------------------------------------------------------------
 * EIP130TOKEN_ASSET_POLICY_*
 *
 * The defines below define the asset policy bits.
 */
// Asset policies related to hash/HMAC and CMAC algorithms
#define EIP130TOKEN_ASSET_POLICY_SHA1                       0x0000000000000001ULL
#define EIP130TOKEN_ASSET_POLICY_SHA224                     0x0000000000000002ULL
#define EIP130TOKEN_ASSET_POLICY_SHA256                     0x0000000000000004ULL
#define EIP130TOKEN_ASSET_POLICY_SHA384                     0x0000000000000008ULL
#define EIP130TOKEN_ASSET_POLICY_SHA512                     0x0000000000000010ULL
#define EIP130TOKEN_ASSET_POLICY_CMAC                       0x0000000000000020ULL

// Asset policies related to cipher algorithms
#define EIP130TOKEN_ASSET_POLICY_ALGO_CIPHER_MASK           0x0000000000000300ULL
#define EIP130TOKEN_ASSET_POLICY_ALGO_CIPHER_AES            0x0000000000000100ULL
#define EIP130TOKEN_ASSET_POLICY_ALGO_CIPHER_TRIPLE_DES     0x0000000000000200ULL

// Asset policies related to cipher modes
#define EIP130TOKEN_ASSET_POLICY_MODE1                      0x0000000000010000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE2                      0x0000000000020000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE3                      0x0000000000040000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE4                      0x0000000000080000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE5                      0x0000000000100000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE6                      0x0000000000200000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE7                      0x0000000000400000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE8                      0x0000000000800000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE9                      0x0000000001000000ULL
#define EIP130TOKEN_ASSET_POLICY_MODE10                     0x0000000002000000ULL

// Asset policies related to cipher/MAC operations
#define EIP130TOKEN_ASSET_POLICY_MAC_GENERATE               0x0000000004000000ULL
#define EIP130TOKEN_ASSET_POLICY_MAC_VERIFY                 0x0000000008000000ULL
#define EIP130TOKEN_ASSET_POLICY_ENCRYPT                    0x0000000010000000ULL
#define EIP130TOKEN_ASSET_POLICY_DECRYPT                    0x0000000020000000ULL

// Asset policies related to temporary values
#define EIP130TOKEN_ASSET_POLICY_TEMP_IV                    0x0001000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_TEMP_COUNTER               0x0002000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_TEMP_MAC                   0x0004000000000000ULL // includes intermedaite digest
#define EIP130TOKEN_ASSET_POLICY_TEMP_AUTH_STATE            0x0010000000000000ULL

// Asset policy related to monotonic counter
#define EIP130TOKEN_ASSET_POLICY_MONOTONIC                  0x0000000100000000ULL

// Asset policies related to key derive functionality
#define EIP130TOKEN_ASSET_POLICY_TRUSTED_ROOT_KEY           0x0000000200000000ULL
#define EIP130TOKEN_ASSET_POLICY_TRUSTED_KEY_DERIVE         0x0000000400000000ULL
#define EIP130TOKEN_ASSET_POLICY_KEY_DERIVE                 0x0000000800000000ULL

// Asset policies related to AES key wrap functionality
#define EIP130TOKEN_ASSET_POLICY_TRUSTED_WRAP               0x0000001000000000ULL
#define EIP130TOKEN_ASSET_POLICY_AES_WRAP                   0x0000002000000000ULL

// Asset policies related to PK operations
#define EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY                 0x0000000080000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_RSA_OAEP_WRAP           0x0000004000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_RSA_PKCS1_SIGN          0x0000020000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_RSA_PSS_SIGN            0x0000040000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_DSA_SIGN                0x0000080000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN          0x0000100000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_DH_KEY                  0x0000200000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_ECDH_KEY                0x0000400000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY_PARAM           0x0000800000000000ULL
#define EIP130TOKEN_ASSET_POLICY_EMMC_AUTH_KEY              0x0400000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_AUTH_KEY                   0x8000000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PK_ECC_ELGAMAL_ENC         (EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN|EIP130TOKEN_ASSET_POLICY_PK_ECDH_KEY)

// Asset policies related to domain
#define EIP130TOKEN_ASSET_POLICY_SOURCE_NON_SECURE          0x0100000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_CROSS_DOMAIN               0x0200000000000000ULL

// Asset policies related to export functionality
#define EIP130TOKEN_ASSET_POLICY_PRIVATE_DATA               0x0800000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_PUBLIC_DATA                0x1000000000000000ULL

// Asset policies related to export functionality
#define EIP130TOKEN_ASSET_POLICY_EXPORT                     0x2000000000000000ULL
#define EIP130TOKEN_ASSET_POLICY_TRUSTED_EXPORT             0x4000000000000000ULL


#endif /* Include Guard */

/* end of file eip130_token_asset.h */
