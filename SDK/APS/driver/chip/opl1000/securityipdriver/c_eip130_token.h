/* c_eip130_token.h
 *
 * Configuration options for the Security Module Token helper functions.
 * The project-specific cs_eip130_token.h file is included, whereafter
 * defaults are provided for missing parameters and checks for illegal
 * and conflicting settings can be performed.
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

#ifndef INCLUDE_GUARD_C_EIP130_TOKEN_H
#define INCLUDE_GUARD_C_EIP130_TOKEN_H

/*----------------------------------------------------------------
 * Defines that can be used in the cs_eip130_token.h file
 */

// This option enables function call parameter checking
// disable it to reduce code size and reduce overhead
// make sure upper layer does not rely on these checks!
// #define EIP130TOKEN_STRICT_ARGS

// Use the options below to selectively enable specific functions
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_F8
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM
//#define EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20
//#define EIP130TOKEN_ENABLE_ENCRYPTED_VECTOR



/*----------------------------------------------------------------
 * inclusion of cs_eip130_token.h
 */
#include "cs_eip130_token.h"


/*----------------------------------------------------------------
 * provide backup values for all missing configuration parameters
 */


/*----------------------------------------------------------------
 * check for conflicting settings and illegal values
 */


/*----------------------------------------------------------------
 * other configuration parameters that cannot be set in cs_xxx.h
 * but are considered product-configurable anyway
 */


#endif /* INCLUDE_GUARD_C_EIP130_TOKEN_H */

/* end of file c_eip130_token.h */
