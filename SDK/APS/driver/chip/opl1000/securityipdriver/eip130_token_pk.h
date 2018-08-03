/* eip130_token_pk.h
 *
 * Security Module Token helper functions
 * - Public key tokens related functions and definitions
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_PK_H
#define INCLUDE_GUARD_EIP130TOKEN_PK_H

#include "basic_defs.h"             // uint32_t, bool, inline, etc.
#include "clib.h"                   // memset, memcpy
#include "eip130_token_common.h"    // Eip130Token_Command/Result_t

#define EIP130TOKEN_PK_CMD_MAX_HASH_SIZE  4095

// Commands for PK operations with assets
enum
{
    EIP130TOKEN_PK_ASSET_CMD_ECDH_ECDSA_KEY_CHECK = 0x01,
    EIP130TOKEN_PK_ASSET_CMD_DH_DSA_KEY_CHECK,
    EIP130TOKEN_PK_ASSET_CMD_ECDSA_SIGN = 0x06,
    EIP130TOKEN_PK_ASSET_CMD_ECDSA_VERIFY,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PKCS_SIGN = 0x08,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PKCS_VER,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PKCS_SIGN_CRT,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PSS_SIGN = 0x0C,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PSS_VER,
    EIP130TOKEN_PK_ASSET_CMD_RSA_PSS_SIGN_CRT,
    EIP130TOKEN_PK_ASSET_CMD_DH_GEN_PUB_KEY = 0x10,
    EIP130TOKEN_PK_ASSET_CMD_DH_GEN_PRIV_PUB_KEY,
    EIP130TOKEN_PK_ASSET_CMD_DH_GEN_SINGLE_SH_SECR,
    EIP130TOKEN_PK_ASSET_CMD_DH_GEN_DUAL_SH_SECR,
    EIP130TOKEN_PK_ASSET_CMD_ECDH_ECDSA_GEN_PUB_KEY,
    EIP130TOKEN_PK_ASSET_CMD_ECDH_ECDSA_GEN_PRIV_PUB_KEY,
    EIP130TOKEN_PK_ASSET_CMD_ECDH_GEN_SINGLE_SH_SECR,
    EIP130TOKEN_PK_ASSET_CMD_ECDH_GEN_DUAL_SH_SECR,
    EIP130TOKEN_PK_ASSET_CMD_RSA_OAEP_WRAP_STRING = 0x18,
    EIP130TOKEN_PK_ASSET_CMD_RSA_OAEP_WRAP_HASHED,
    EIP130TOKEN_PK_ASSET_CMD_RSA_OAEP_UNWRAP_STRING,
    EIP130TOKEN_PK_ASSET_CMD_RSA_OAEP_UNWRAP_HASHED,
    EIP130TOKEN_PK_ASSET_CMD_ELGAMAL_ECC_ENCRYPT = 0x24,
    EIP130TOKEN_PK_ASSET_CMD_ELGAMAL_ECC_DECRYPT,
    EIP130TOKEN_PK_ASSET_CMD_CURVE25519_GEN_PUBKEY = 0x28,
    EIP130TOKEN_PK_ASSET_CMD_CURVE25519_GEN_KEYPAIR,
    EIP130TOKEN_PK_ASSET_CMD_CURVE25519_GEN_SHARED_SECRET,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_GEN_PUBKEY,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_GEN_KEYPAIR,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_SIGN_INITIAL,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_SIGN_UPDATE,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_SIGN_FINAL,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_VERIFY_INITIAL,
    EIP130TOKEN_PK_ASSET_CMD_EDDSA_VERIFY_FINAL,
};


// Commands for PK operations without assets
enum
{
    EIP130TOKEN_PK_CMD_NUMLOAD = 0x01,
    EIP130TOKEN_PK_CMD_NUMSETN = 0x03,
    EIP130TOKEN_PK_CMD_MODEXPE = 0x04,
    EIP130TOKEN_PK_CMD_MODEXPD,
    EIP130TOKEN_PK_CMD_MODEXPCRT,
    EIP130TOKEN_PK_CMD_ECMONTMUL = 0x0A,
    EIP130TOKEN_PK_CMD_ECCMUL,
    EIP130TOKEN_PK_CMD_ECCADD,
    EIP130TOKEN_PK_CMD_DSA_SIGN,
    EIP130TOKEN_PK_CMD_DSA_VERIFY,
    EIP130TOKEN_PK_CMD_ECDSA_SIGN,
    EIP130TOKEN_PK_CMD_ECDSA_VERIFY,
};


// Public Key sub-vector structure (Intended for reference)
struct Eip130Token_PK_SubVector
{
    uint16_t  SubVectorLength;
    uint8_t   SubVectorIndex;
    uint8_t   NrOfSubVectors;
    uint8_t   Data[4];                  // Placeholder has flexible length
};


#endif /* Include Guard */

/* end of file eip130_token_pk.h */
