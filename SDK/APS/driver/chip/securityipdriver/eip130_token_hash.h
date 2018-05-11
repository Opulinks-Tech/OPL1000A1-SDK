/* eip130_token_hash.h
 *
 * Security Module Token helper functions
 * - Hash token related functions and definitions
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_HASH_H
#define INCLUDE_GUARD_EIP130TOKEN_HASH_H

#include "c_eip130_token.h"         // configuration options
#include "basic_defs.h"             // uint32_t, bool, inline, etc.
#include "eip130_token_common.h"    // Eip130Token_Command/Result_t


enum
{
    EIP130TOKEN_HASH_ALGORITHM_SHA1 = 1,
    EIP130TOKEN_HASH_ALGORITHM_SHA224,
    EIP130TOKEN_HASH_ALGORITHM_SHA256,
    EIP130TOKEN_HASH_ALGORITHM_SHA384,
    EIP130TOKEN_HASH_ALGORITHM_SHA512,
};


#endif /* Include Guard */

/* end of file eip130_token_hash.h */
