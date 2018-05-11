/* adapter_vex_intern_asym.h
 *
 * VaultIP Exchange: Internal interfaces and definitions specific fro
 *                   Asymmetric Crypto.
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

#ifndef INCLUDE_GUARD_ADAPTER_VEX_INTERN_ASYM_H
#define INCLUDE_GUARD_ADAPTER_VEX_INTERN_ASYM_H

#include "adapter_vex_internal.h"   // Generic internal interfaces and definitions


/*----------------------------------------------------------------------------
 * vex_Asym_AssetSign
 *
 * This function handles the signature generate operation for ECDSA, DSA and
 * RSA based on a token using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetSign(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_Asym_AssetVerify
 *
 * This function handles the signature verify operation for ECDSA, DSA and
 * RSA based on a token using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetVerify(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

#if 0
/*----------------------------------------------------------------------------
 * vex_Asym_AssetEncrypt
 *
 * This function handles an asymmetric encrypt or decrypt operation based on
 * a token using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetEncrypt(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Asym_AssetGenKeyPair
 *
 * This function handles the key pair generation operation based on a token
 * using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetGenKeyPair(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Asym_AssetGenKeyPublic
 *
 * This function handles the public key generation operation based on a token
 * using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetGenKeyPublic(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Asym_AssetKeyCheck
 *
 * This function handles the key check operation for ECDH/ECDSA based on
 * a token using Assets.
 *
 * CommandToken_p
 *     Pointer to the buffer with the service request.
 *
 * ResultToken_p
 *     Pointer to the buffer in which the service result needs to be returned.
 *
 * Return Value:
 *     One of the VexStatus_t values.
 */
VexStatus_t
vex_Asym_AssetKeyCheck(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#endif /* Include Guard */

/* end of file adapter_vex_intern_asym.h */
