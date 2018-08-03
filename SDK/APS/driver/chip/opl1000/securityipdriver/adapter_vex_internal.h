/* adapter_vex_internal.h
 *
 * VaultIP Exchange: Internal interfaces and definitions.
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

#ifndef INCLUDE_GUARD_ADAPTER_VEX_INTERNAL_H
#define INCLUDE_GUARD_ADAPTER_VEX_INTERNAL_H

#include "adapter_vex.h"            // VexToken_Command_t, VexToken_Result_t
#include "eip130_token_common.h"    // Eip130Token_Command/Result_t
#include "device_types.h"           // Device_Handle_t

// Set TokenID operation
#ifdef VEX_CHECK_DMA_WITH_TOKEN_ID
#define Vex_Command_SetTokenID(a,b) Eip130Token_Command_SetTokenID(a,b,true)
#else
#define Vex_Command_SetTokenID(a,b) Eip130Token_Command_SetTokenID(a,b,false)
#endif

// Device power states
#define VEX_DEVICE_POWER_UNKNOWN        0
#define VEX_DEVICE_POWER_ACTIVE         1
#define VEX_DEVICE_POWER_SLEEP          2
#define VEX_DEVICE_POWER_HIBERATION     4

#if 0
/*----------------------------------------------------------------------------
 * vex_InitBufManager
 *
 * This function installs the callback functions for the BufManager.
 *
 * Return Value:
 *     Not applicable.
 */
void
vex_InitBufManager(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Nop
 *
 * This function handles the NOP based token.
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
vex_Nop(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif



/*----------------------------------------------------------------------------
 * vex_SymCipher
 *
 * This function handles the Symmetric Crypto Cipher based token.
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
vex_SymCipher(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_SymCipherAE
 *
 * This function handles the Authenticated Symmetric Crypto Cipher based token.
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
vex_SymCipherAE(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_SymHash
 *
 * This function handles the Symmetric Crypto Hash based token.
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
vex_SymHash(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_SymMac
 *
 * This function handles the Symmetric Crypto MAC based token.
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
vex_SymMac(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_SymKeyWrap
 *
 * This function handles the Symmetric Crypto key wrap token.
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
vex_SymKeyWrap(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_EncryptedVector
 *
 * This function handles the Encrypted Vector token.
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
vex_EncryptedVector(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

#if 0
/*----------------------------------------------------------------------------
 * vex_Trng
 *
 * This function handles the TRNG based token.
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
vex_Trng(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Asset
 *
 * This function handles the Asset Management based tokens.
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
vex_Asset(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_Asym
 *
 * This function handles the Asymmetric Crypto based tokens.
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
vex_Asym(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

/*----------------------------------------------------------------------------
 * vex_AUnlock
 *
 * This function handles the Authenticated Unlock based tokens.
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
vex_AUnlock(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_eMMC
 *
 * This function handles the eMMC based tokens.
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
vex_eMMC(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

/*----------------------------------------------------------------------------
 * vex_ExtService
 *
 * This function handles the External Service based tokens.
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
vex_ExtService(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

#if 0
/*----------------------------------------------------------------------------
 * vex_Service
 *
 * This function handles the Service based tokens.
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
vex_Service(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

/*----------------------------------------------------------------------------
 * vex_System
 *
 * This function handles the System based tokens.
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
vex_System(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);

#if 0
/*----------------------------------------------------------------------------
 * vex_Claim
 *
 * This function handles the services with which the exclusive mailbox locking
 * can be controlled.
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
vex_Claim(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);
#endif

/*----------------------------------------------------------------------------
 * vex_PhysicalTokenExchange
 *
 * This function exchanges the physical tokens with the VaultIP hardware.
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
vex_PhysicalTokenExchange(
        Eip130Token_Command_t * const CommandToken_p,
        Eip130Token_Result_t * const ResultToken_p);


/*----------------------------------------------------------------------------
 * vex_DeviceInit
 *
 * This function initializes the connection to the VaultIP hardware.
 *
 * Return Value:
 *      0 : Successful.
 *     -1 : Could not find device.
 *     -2 : Could not retrieve device information.
 *     -3 : Firmware acquiring failed.
 *     -4 : Firmware load failed.
 *     -5 : Firmware use issue.
 *     -6 : Mailbox linking failed.
 *     -7 : OUT mailbox is unexpectedly FULL.
 *     -8 : IN mailbox is unexpectedly FULL.
 *     -9 : Mailbox unlinking failed.
 */
int
vex_DeviceInit(void);

/*----------------------------------------------------------------------------
 * vex_DeviceExit
 *
 * This function releases the connection with the VaultIP hardware.
 *
 * Return Value:
 *      Not applicable.
 */
void
vex_DeviceExit(void);

/*----------------------------------------------------------------------------
 * vex_DeviceGetHandle
 *
 * This function provides the device handle, if available.
 *
 * Return Value:
 *      The device handle.
 *      Note that NULL means not available.
 */
Device_Handle_t
vex_DeviceGetHandle(void);

/*----------------------------------------------------------------------------
 * vex_DeviceIsConnected
 *
 * This function returns the connection state.
 *
 * Return Value:
 *      true  : Connected (Device handle is available).
 *      false : Not connected.
 */
bool
vex_DeviceIsConnected(void);

/*----------------------------------------------------------------------------
 * vex_DeviceIsSecureConnected
 *
 * This function returns the secure connection state.
 * Note this means that the host has its protection set when accessing the
 * VaultIP registers and mailboxes.
 *
 * Return Value:
 *      true  : Device handle is available and protection set when accessing
 *              VaultIP.
 *      false : Not connected (or device handle not available).
 */
bool
vex_DeviceIsSecureConnected(void);

/*----------------------------------------------------------------------------
 * vex_DeviceStateIssue
 *
 * This function must be called when a device connection/state issue is
 * detected. The device/firmware will be re-initialized automatically if a
 * service is requested and the device connection/state issue is solved.
 *
 * Return Value:
 *      N/A
 */
void
vex_DeviceStateIssue(void);

/*----------------------------------------------------------------------------
 * vex_DeviceLinkMailbox
 *
 * This function links a mailbox.
 *
 * MailboxNr
 *     Number of the mailbox to link.
 *
 * Identity
 *     Identity of calling process to claim mailbox.
 *
 * Return Value:
 *      0 : Successful.
 *     -1 : No device connection.
 *     -2 : Invalid mailbox number.
 *     -3 : Mailbox linking failed.
 *     -4 : Mailbox already linked by another process.
 */
int
vex_DeviceLinkMailbox(
           const uint8_t MailboxNr,
           uint32_t Identity);

/*----------------------------------------------------------------------------
 * vex_DeviceLinkMailboxOverrule
 *
 * This function overrules a mailbox identity link.
 *
 * MailboxNr
 *     Number of the mailbox to link.
 *
 * Identity
 *     Identity of calling process to claim mailbox.
 *
 * Return Value:
 *      0 : Successful.
 *     -1 : No device connection.
 *     -2 : Invalid mailbox number.
 *     -3 : Mailbox linking failed.
 *     -4 : Mailbox already linked by another process.
 */
int
vex_DeviceLinkMailboxOverrule(
           const uint8_t MailboxNr,
           uint32_t Identity);

/*----------------------------------------------------------------------------
 * vex_DeviceUnlinkMailbox
 *
 * This function unlinks a mailbox.
 *
 * MailboxNr
 *     Number of the mailbox to link.
 *
 * Identity
 *     Identity of calling process to release mailbox.
 *
 * Return Value:
 *      0 : Successful.
 *     -1 : No device connection.
 *     -2 : Invalid mailbox number.
 *     -3 : Mailbox unlinking failed.
 *     -4 : Not allowed to unlink the mailbox that is locked by another process
 */
int
vex_DeviceUnlinkMailbox(
           const uint8_t MailboxNr,
           uint32_t Identity);

/*----------------------------------------------------------------------------
 * vex_DeviceGetTokenID
 *
 * This function return a unique token id since startup.
 *
 * Return Value:
 *     A unique token id.
 */
uint16_t
vex_DeviceGetTokenID(void);

/*----------------------------------------------------------------------------
 * vex_DeviceSleep
 *
 * This function puts the device in Sleep mode after first checking that the
 * firmware is accepted and the calling process is on the master host. If that
 * is the case it writes the Sleep token and waits for the result token.
 *
 * Return Value
 *     0      Firmware is set in Sleep mode
 *     <0     Error code:
 *     -1     Not supported - Check if VaultIP active
 *     -2     This Host is NOT allowed to do the download
 *     -3     Firmware is not in accepted state
 *     -4     Invalid power state
 *     <-256  Firmware error from token
 */
int
vex_DeviceSleep(void);

/*----------------------------------------------------------------------------
 * vex_DeviceResumeFromSleep
 *
 * This function resumes the device that is Sleep mode to active state after
 * checking that the firmware is in the correct state and the calling process
 * is on the master host. If that is the case it writes the Resume from Sleep
 * token and waits for the result token.
 *
 * Return Value
 *     0      Firmware is in active (usable) state
 *     <0     Error code:
 *     -1     Not supported - Check if VaultIP active
 *     -2     This Host is NOT allowed to do the download
 *     -3     Firmware is not in accepted state
 *     <-256  Firmware error from token
 */
int
vex_DeviceResumeFromSleep(void);


#if 0
/*----------------------------------------------------------------------------
 * vex_LockInit
 *
 * This function initializes the lock mechanism.
 *
 * Return Value:
 *     0  Successful
 *     -1 Failed
 */
int
vex_LockInit(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockExit
 *
 * This function removes lock mechanism.
 *
 * Return Value:
 *      Not applicable.
 */
void
vex_LockExit(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockAcquire
 *
 * This function acquires a lock.
 *
 * Return Value:
 *     0  Successful
 *     -1 Failed
 */
int
vex_LockAcquire(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_LockRelease
 *
 * This function releases the acquired lock.
 *
 * Return Value:
 *     -
 */
void
vex_LockRelease(void);
#endif

/*----------------------------------------------------------------------------
 * vex_MailboxGet
 *
 * This function returns the mailbox number to use after verifying/linking
 * the mailbox first.
 *
 * Identity
 *     Identity of calling process.
 *
 * Return Value:
 *     1..8 : Mailbox to use.
 *        0 : Invalid mailbox or mailbox linking failed.
 */
uint8_t
vex_MailboxGet(
        uint32_t Identity);

#if 0
/*----------------------------------------------------------------------------
 * vex_IdentityGet
 *
 * This function returns identity to use.
 *
 * Return Value:
 *     The identity to use.
 */
uint32_t
vex_IdentityGet(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_IdentityCryptoOfficer
 *
 * This function set the process id and optional the identity of the Crypto
 * Officer.
 *
 * CryptoOfficerId
 *     Identity of the Crypto Officer.
 *     Note: in case 0 is provided the configured one in cs/c_adapter_vex.h
 *           will be used.
 *
 * Return Value:
 *     -
 */
void
vex_IdentityCryptoOfficer(
        uint32_t CryptoOfficerId);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_IdentityUserAdd
 *
 * This function adds the process id from the calling process to identified a
 * user.
 *
 * Return Value:
 *     -
 */
uint32_t
vex_IdentityUserAdd(void);
#endif

#if 0
/*----------------------------------------------------------------------------
 * vex_IdentityUserRemove
 *
 * This function removes the process id from the calling process (user).
 *
 * Return Value:
 *     -
 */
void
vex_IdentityUserRemove(void);
#endif

#endif /* Include Guard */

/* end of file adapter_vex_internal.h */
