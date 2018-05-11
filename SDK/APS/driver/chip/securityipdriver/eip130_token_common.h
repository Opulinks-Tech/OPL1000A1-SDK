/* eip130_token_common.h
 *
 * Security Module Token helper functions
 * - Common token related functions and definitions
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_COMMON_H
#define INCLUDE_GUARD_EIP130TOKEN_COMMON_H

#include "basic_defs.h"             // uint32_t, bool, inline, etc.

// Command Token
#define EIP130TOKEN_COMMAND_WORDS   64
typedef struct
{
    uint32_t W[EIP130TOKEN_COMMAND_WORDS];
} Eip130Token_Command_t;

// Result Token
#define EIP130TOKEN_RESULT_WORDS    64
typedef struct
{
    uint32_t W[EIP130TOKEN_RESULT_WORDS];
} Eip130Token_Result_t;

// Token operation codes
#define EIP130TOKEN_OPCODE_NOP                    0
#define EIP130TOKEN_OPCODE_ENCRYPTION             1
#define EIP130TOKEN_OPCODE_HASH                   2
#define EIP130TOKEN_OPCODE_MAC                    3
#define EIP130TOKEN_OPCODE_TRNG                   4
#define EIP130TOKEN_OPCODE_RESERVED5              5
#define EIP130TOKEN_OPCODE_AESWRAP                6
#define EIP130TOKEN_OPCODE_ASSETMANAGEMENT        7
#define EIP130TOKEN_OPCODE_AUTH_UNLOCK            8
#define EIP130TOKEN_OPCODE_PUBLIC_KEY             9
#define EIP130TOKEN_OPCODE_EMMC                   10
#define EIP130TOKEN_OPCODE_EXT_SERVICE            11
#define EIP130TOKEN_OPCODE_RESERVED12             12
#define EIP130TOKEN_OPCODE_RESERVED13             13
#define EIP130TOKEN_OPCODE_SERVICE                14
#define EIP130TOKEN_OPCODE_SYSTEM                 15

// Token sub codes
// TRNG operations
#define EIP130TOKEN_SUBCODE_RANDOMNUMBER          0
#define EIP130TOKEN_SUBCODE_TRNGCONFIG            1
#define EIP130TOKEN_SUBCODE_VERIFYDRBG            2
#define EIP130TOKEN_SUBCODE_VERIFYNRBG            3
// Asset Management operations
#define EIP130TOKEN_SUBCODE_ASSETSEARCH           0
#define EIP130TOKEN_SUBCODE_ASSETCREATE           1
#define EIP130TOKEN_SUBCODE_ASSETLOAD             2
#define EIP130TOKEN_SUBCODE_ASSETDELETE           3
#define EIP130TOKEN_SUBCODE_PUBLICDATA            4
#define EIP130TOKEN_SUBCODE_MONOTONICREAD         5
#define EIP130TOKEN_SUBCODE_MONOTONICINCR         6
#define EIP130TOKEN_SUBCODE_OTPDATAWRITE          7
#define EIP130TOKEN_SUBCODE_SECURETIMER           8
#define EIP130TOKEN_SUBCODE_PROVISIONRANDOMHUK    9
// KeyWrap and Encrypt vector operations
#define EIP130TOKEN_SUBCODE_AESKEYWRAP            0
#define EIP130TOKEN_SUBCODE_ENCRYPTVECTOR         1
// Authenticated Unlock operations
#define EIP130TOKEN_SUBCODE_AUNLOCKSTART          0
#define EIP130TOKEN_SUBCODE_AUNLOCKVERIFY         1
#define EIP130TOKEN_SUBCODE_SETSECUREDEBUG        2
// Public key operations
#define EIP130TOKEN_SUBCODE_PK_NOASSETS           0
#define EIP130TOKEN_SUBCODE_PK_WITHASSETS         1
// eMMC operations
#define EIP130TOKEN_SUBCODE_EMMC_RDREQ            0
#define EIP130TOKEN_SUBCODE_EMMC_RDVER            1
#define EIP130TOKEN_SUBCODE_EMMC_RDWRCNTREQ       2
#define EIP130TOKEN_SUBCODE_EMMC_RDWRCNTVER       3
#define EIP130TOKEN_SUBCODE_EMMC_WRREQ            4
#define EIP130TOKEN_SUBCODE_EMMC_WRVER            5
// Service operations
#define EIP130TOKEN_SUBCODE_REGISTERREAD          0
#define EIP130TOKEN_SUBCODE_REGISTERWRITE         1
#define EIP130TOKEN_SUBCODE_CLOCKSWITCH           2
#define EIP130TOKEN_SUBCODE_ZEROOUTMAILBOX        3
#define EIP130TOKEN_SUBCODE_SELECTOTPZERO         4
#define EIP130TOKEN_SUBCODE_ZEROIZEOTP            5
// System operations
#define EIP130TOKEN_SUBCODE_SYSTEMINFO            0
#define EIP130TOKEN_SUBCODE_SELFTEST              1
#define EIP130TOKEN_SUBCODE_RESET                 2
#define EIP130TOKEN_SUBCODE_DEFINEUSERS           3
#define EIP130TOKEN_SUBCODE_SLEEP                 4
#define EIP130TOKEN_SUBCODE_RESUMEFROMSLEEP       5
#define EIP130TOKEN_SUBCODE_HIBERNATION           6
#define EIP130TOKEN_SUBCODE_RESUMEFROMHIBERNATION 7

// Token/HW/Algorithm related limits
#define EIP130TOKEN_DMA_MAXLENGTH           0x001FFFFF  // 2 MB - 1 bytes
#define EIP130TOKEN_DMA_TOKENID_SIZE        4           // bytes
#define EIP130TOKEN_DMA_ARC4_STATE_BUF_SIZE 256         // bytes

// DMA data block must be an integer multiple of a work block size (in bytes)
#define EIP130TOKEN_DMA_ALGO_BLOCKSIZE_HASH 64
#define EIP130TOKEN_DMA_ALGO_BLOCKSIZE_AES  16
#define EIP130TOKEN_DMA_ALGO_BLOCKSIZE_DES  8
#define EIP130TOKEN_DMA_ALGO_BLOCKSIZE_ARC4 4
#define EIP130TOKEN_DMA_ALGO_BLOCKSIZE_NOP  4


/*----------------------------------------------------------------------------
 * Eip130Token_Command_WriteByteArray
 *
 * This function fills a consecutive number of words in the command token with
 * bytes from an array. Four consecutive bytes form a 32bit word, LSB-first.
 *
 * CommandToken_p
 *      Pointer to the command token buffer.
 *
 * StartWord
 *      Start index in command token.
 *
 * Data_p
 *      Pointer to the data to write to the command token.
 *
 * DataLenInBytes
 *      Number of bytes to write.
 */
void
Eip130Token_Command_WriteByteArray(
        Eip130Token_Command_t * const CommandToken_p,
        unsigned int StartWord,
        const uint8_t * const Data_p,
        const unsigned int DataLenInBytes);


/*----------------------------------------------------------------------------
 * Eip130Token_Result_ReadByteArray
 *
 * This function reads a number of consecutive words from the result token
 * and writes these to a byte array, breaking down each word into bytes, LSB
 * first.
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * StartWord
 *      Start index in result token.
 *
 * DestLenInBytes
 *      Number of bytes to write.
 *
 * Dest_p
 *      Pointer to the data buffer to copy the data from the result token into.
 */
void
Eip130Token_Result_ReadByteArray(
        const Eip130Token_Result_t * const ResultToken_p,
        unsigned int StartWord,
        unsigned int DestLenOutBytes,
        uint8_t * Dest_p);


#endif /* Include Guard */

/* end of file eip130_token_common.h */
