/* adapter_vex.h
 *
 * The VaultIP Exchange API.
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

#ifndef INCLUDE_GUARD_ADAPTER_VEX_H
#define INCLUDE_GUARD_ADAPTER_VEX_H

#include "c_adapter_vex.h"          // configuration

#include "basic_defs.h"             // uint32_t, inline, etc.

/*----------------------------------------------------------------------------
 * VexStatus_t
 */
typedef enum
{
    /* Operation success */
    VEX_SUCCESS = 0,

    /* Not supported */
    VEX_UNSUPPORTED = -1,

    /* No connection with VaultIP HW available */
    VEX_NOT_CONNECTED = -2,

    /* Device writes (unexpectly) did not work */
    VEX_POWER_STATE_ERROR = -3,

    /* The host is not allowed to do this operation */
    VEX_OPERATION_NOT_ALLOWED = -4,

    /* Operation failed */
    VEX_OPERATION_FAILED = -5,

    /* Invalid parameter */
    VEX_INVALID_OPCODE = -6,

    /* Invalid parameter */
    VEX_INVALID_SUBCODE = -7,

    /* Invalid length */
    VEX_INVALID_LENGTH = -8,

    /* Wrong use; not depending on configuration */
    VEX_BAD_ARGUMENT = -9,

    /* No memory */
    VEX_NO_MEMORY = -10,

    /* No identity */
    VEX_NO_IDENTITY = -11,

    /* No mailbox */
    VEX_NO_MAILBOX = -12,

    /* Mailbox is in use */
    VEX_MAILBOX_IN_USE = -13,

    /* Output token response timeout */
    VEX_REPONSE_TIMEOUT = -14,

    /* Data ready check timeout */
    VEX_DATA_TIMEOUT = -15,

    /* Output token response timeout */
    VEX_LOCK_TIMEOUT = -16,

    /* Internal error */
    VEX_INTERNAL_ERROR = -17
}
VexStatus_t;


/*----------------------------------------------------------------------------
 * VexTokenOpCode_t - Token operation codes
 */
typedef enum
{
    VEXTOKEN_OPCODE_NOP             = 0,
    VEXTOKEN_OPCODE_ENCRYPTION      = 1,
    VEXTOKEN_OPCODE_HASH            = 2,
    VEXTOKEN_OPCODE_MAC             = 3,
    VEXTOKEN_OPCODE_TRNG            = 4,
    VEXTOKEN_OPCODE_RESERVED5       = 5,
    VEXTOKEN_OPCODE_AESWRAP         = 6,
    VEXTOKEN_OPCODE_ASSETMANAGEMENT = 7,
    VEXTOKEN_OPCODE_AUTH_UNLOCK     = 8,
    VEXTOKEN_OPCODE_PUBLIC_KEY      = 9,
    VEXTOKEN_OPCODE_EMMC            = 10,
    VEXTOKEN_OPCODE_EXTSERVICE      = 11,
    VEXTOKEN_OPCODE_RESERVED12      = 12,
    VEXTOKEN_OPCODE_RESERVED13      = 13,
    VEXTOKEN_OPCODE_SERVICE         = 14,
    VEXTOKEN_OPCODE_SYSTEM          = 15,
    VEXTOKEN_OPCODE_CLAIMCONTROL    = 16
} VexTokenOpCode_t;

/*----------------------------------------------------------------------------
 * VexTokenSubCode_t - Token sub codes
 */
typedef enum
{
    // General operations
    VEXTOKEN_SUBCODE_NOT_USED = 0,

    // Encryption operation
    VEXTOKEN_SUBCODE_ENCRYPT     = 0,
    VEXTOKEN_SUBCODE_AUTHENCRYPT = 1,

    // MAC operations
    VEXTOKEN_SUBCODE_MACUPDATE   = 0,
    VEXTOKEN_SUBCODE_MACGENERATE = 1,
    VEXTOKEN_SUBCODE_MACVERIFY   = 2,

    // TRNG operations
    VEXTOKEN_SUBCODE_RANDOMNUMBER = 0,
    VEXTOKEN_SUBCODE_TRNGCONFIG   = 1,
    VEXTOKEN_SUBCODE_VERIFYDRBG   = 2,
    VEXTOKEN_SUBCODE_VERIFYNRBG   = 3,

    // Asset Management operations
    VEXTOKEN_SUBCODE_ASSETSEARCH   = 0,
    VEXTOKEN_SUBCODE_ASSETCREATE   = 1,
    VEXTOKEN_SUBCODE_ASSETLOAD     = 2,
    VEXTOKEN_SUBCODE_ASSETDELETE   = 3,
    VEXTOKEN_SUBCODE_PUBLICDATA    = 4,
    VEXTOKEN_SUBCODE_MONOTONICREAD = 5,
    VEXTOKEN_SUBCODE_MONOTONICINCR = 6,
    VEXTOKEN_SUBCODE_OTPDATAWRITE  = 7,
    VEXTOKEN_SUBCODE_SECURETIMER   = 8,
    VEXTOKEN_SUBCODE_PROVISIONRANDOMHUK = 9,

    // KeyWrap and Encrypt vector operations
    VEXTOKEN_SUBCODE_AESKEYWRAP    = 0,
#ifdef VEX_ENABLE_ENCRYPTED_VECTOR
    VEXTOKEN_SUBCODE_ENCRYPTVECTOR = 1,
#endif

    // Authenticated Unlock operations
    VEXTOKEN_SUBCODE_AUNLOCKSTART   = 0,
    VEXTOKEN_SUBCODE_AUNLOCKVERIFY  = 1,
    VEXTOKEN_SUBCODE_SETSECUREDEBUG = 2,

    // Public key operations
    VEXTOKEN_SUBCODE_PK_NOASSETS   = 0,
    VEXTOKEN_SUBCODE_PK_WITHASSETS = 1,

    // eMMC operations
    VEXTOKEN_SUBCODE_EMMC_RDREQ      = 0,
    VEXTOKEN_SUBCODE_EMMC_RDVER      = 1,
    VEXTOKEN_SUBCODE_EMMC_RDWRCNTREQ = 2,
    VEXTOKEN_SUBCODE_EMMC_RDWRCNTVER = 3,
    VEXTOKEN_SUBCODE_EMMC_WRREQ      = 4,
    VEXTOKEN_SUBCODE_EMMC_WRVER      = 5,

    // Service operations
    VEXTOKEN_SUBCODE_REGISTERREAD   = 0,
    VEXTOKEN_SUBCODE_REGISTERWRITE  = 1,
    VEXTOKEN_SUBCODE_CLOCKSWITCH    = 2,
    VEXTOKEN_SUBCODE_ZEROOUTMAILBOX = 3,
    VEXTOKEN_SUBCODE_SELECTOTPZERO  = 4,
    VEXTOKEN_SUBCODE_ZEROIZEOTP     = 5,

    // System operations
    VEXTOKEN_SUBCODE_SYSTEMINFO            = 0,
    VEXTOKEN_SUBCODE_SELFTEST              = 1,
    VEXTOKEN_SUBCODE_RESET                 = 2,
    VEXTOKEN_SUBCODE_DEFINEUSERS           = 3,
    VEXTOKEN_SUBCODE_SLEEP                 = 4,
    VEXTOKEN_SUBCODE_RESUMEFROMSLEEP       = 5,
    VEXTOKEN_SUBCODE_HIBERNATION           = 6,
    VEXTOKEN_SUBCODE_RESUMEFROMHIBERNATION = 7,

    // Claim control
    VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE          = 0,
    VEXTOKEN_SUBCODE_CLAIM_EXCLUSIVE_USE_OVERRULE = 1,
    VEXTOKEN_SUBCODE_CLAIM_RELEASE                = 2,
} VexTokenSubCode_t;

/*----------------------------------------------------------------------------
 * VexTokenAlgoHash_t - Hash algorithm codes
 */
typedef enum
{
    VEXTOKEN_ALGO_HASH_SHA1 = 1,
    VEXTOKEN_ALGO_HASH_SHA224,
    VEXTOKEN_ALGO_HASH_SHA256,
    VEXTOKEN_ALGO_HASH_SHA384,
    VEXTOKEN_ALGO_HASH_SHA512,
} VexTokenAlgoHash_t;

/*----------------------------------------------------------------------------
 * VexTokenAlgoMac_t - MAC algorithm codes
 */
typedef enum
{
    VEXTOKEN_ALGO_MAC_HMAC_SHA1 = 1,
    VEXTOKEN_ALGO_MAC_HMAC_SHA224,
    VEXTOKEN_ALGO_MAC_HMAC_SHA256,
    VEXTOKEN_ALGO_MAC_HMAC_SHA384,
    VEXTOKEN_ALGO_MAC_HMAC_SHA512,
    VEXTOKEN_ALGO_MAC_AES_CMAC = 8,
    VEXTOKEN_ALGO_MAC_AES_CBC_MAC,
    VEXTOKEN_ALGO_MAC_POLY1305 = 14,
} VexTokenAlgoMac_t;


/*----------------------------------------------------------------------------
 * VexTokenAlgoCipher_t - Cipher algorithm codes
 */
typedef enum
{
    VEXTOKEN_ALGO_CIPHER_AES = 0,
    VEXTOKEN_ALGO_CIPHER_DES,
    VEXTOKEN_ALGO_CIPHER_3DES,
    VEXTOKEN_ALGO_CIPHER_CHACHA20 = 7,
} VexTokenAlgoCipher_t;

/*----------------------------------------------------------------------------
 * VexTokenModeHashMAC_t - Hash and MAC mode codes
 */
typedef enum
{
    VEXTOKEN_MODE_HASH_MAC_INIT2FINAL = 0,
    VEXTOKEN_MODE_HASH_MAC_CONT2FINAL,
    VEXTOKEN_MODE_HASH_MAC_INIT2CONT,
    VEXTOKEN_MODE_HASH_MAC_CONT2CONT,
} VexTokenModeHashMAC_t;

/*----------------------------------------------------------------------------
 * VexTokenModeCipher_t - Cipher mode codes
 */
typedef enum
{
    VEXTOKEN_MODE_CIPHER_ECB = 0,
    VEXTOKEN_MODE_CIPHER_CBC,
    VEXTOKEN_MODE_CIPHER_CTR,
    VEXTOKEN_MODE_CIPHER_ICM,
    VEXTOKEN_MODE_CIPHER_F8,
    VEXTOKEN_MODE_CIPHER_CCM,
    VEXTOKEN_MODE_CIPHER_XTS,
    VEXTOKEN_MODE_CIPHER_GCM,
    VEXTOKEN_MODE_CIPHER_CHACHA20_ENC = 0,
    VEXTOKEN_MODE_CIPHER_CHACHA20_AEAD,
} VexTokenModeCipher_t;

/*----------------------------------------------------------------------------
 * VexTokenAssetLoad_t - Asset Load codes
 */
typedef enum
{
    VEXTOKEN_ASSETLOAD_IMPORT = 0,
    VEXTOKEN_ASSETLOAD_DERIVE,
    VEXTOKEN_ASSETLOAD_PLAINTEXT,
    VEXTOKEN_ASSETLOAD_RANDOM,
    VEXTOKEN_ASSETLOAD_AESUNWRAP,
} VexTokenAssetLoad_t;

/*----------------------------------------------------------------------------
 * VexTokenPkAsset_t - PK with Assets operation codes
 */
typedef enum
{
    VEXTOKEN_PKASSET_ECDH_ECDSA_KEYCHK = 0x01,
    VEXTOKEN_PKASSET_DH_DSA_KEYCHK,
    VEXTOKEN_PKASSET_DSA_SIGN = 0x04,
    VEXTOKEN_PKASSET_DSA_VERIFY,
    VEXTOKEN_PKASSET_ECDSA_SIGN,
    VEXTOKEN_PKASSET_ECDSA_VERIFY,
    VEXTOKEN_PKASSET_RSA_PKCS1V1_5_SIGN = 0x08,
    VEXTOKEN_PKASSET_RSA_PKCS1V1_5_VERIFY,
    VEXTOKEN_PKASSET_RSA_PKCS1V1_5_SIGN_CRT,
    VEXTOKEN_PKASSET_RSA_PSS_SIGN = 0x0C,
    VEXTOKEN_PKASSET_RSA_PSS_VERIFY,
    VEXTOKEN_PKASSET_RSA_PSS_SIGN_CRT,
    VEXTOKEN_PKASSET_DH_GEN_PUBKEY = 0x10,
    VEXTOKEN_PKASSET_DH_GEN_KEYPAIR,
    VEXTOKEN_PKASSET_DH_GEN_SHARED_SECRET_SKEYPAIR,
    VEXTOKEN_PKASSET_DH_GEN_SHARED_SECRET_DKEYPAIR,
    VEXTOKEN_PKASSET_ECDH_ECDSA_GEN_PUBKEY,
    VEXTOKEN_PKASSET_ECDH_ECDSA_GEN_KEYPAIR,
    VEXTOKEN_PKASSET_ECDH_GEN_SHARED_SECRET_SKEYPAIR,
    VEXTOKEN_PKASSET_ECDH_GEN_SHARED_SECRET_DKEYPAIR,
    VEXTOKEN_PKASSET_RSA_OAEP_WRAP_STRING = 0x18,
    VEXTOKEN_PKASSET_RSA_OAEP_WRAP_HASH,
    VEXTOKEN_PKASSET_RSA_OAEP_UNWRAP_STRING,
    VEXTOKEN_PKASSET_RSA_OAEP_UNWRAP_HASH,
    VEXTOKEN_PKASSET_ECC_ELGAMAL_ENCRYPT = 0x24,
    VEXTOKEN_PKASSET_ECC_ELGAMAL_DECRYPT,
    VEXTOKEN_PKASSET_CURVE25519_GEN_PUBKEY = 0x28,
    VEXTOKEN_PKASSET_CURVE25519_GEN_KEYPAIR,
    VEXTOKEN_PKASSET_CURVE25519_GEN_SHARED_SECRET,
    VEXTOKEN_PKASSET_EDDSA_GEN_PUBKEY,
    VEXTOKEN_PKASSET_EDDSA_GEN_KEYPAIR,
    VEXTOKEN_PKASSET_EDDSA_SIGN_INITIAL,
    VEXTOKEN_PKASSET_EDDSA_SIGN_UPDATE,
    VEXTOKEN_PKASSET_EDDSA_SIGN_FINAL,
    VEXTOKEN_PKASSET_EDDSA_VERIFY_INITIAL,
    VEXTOKEN_PKASSET_EDDSA_VERIFY_FINAL,
} VexTokenPkAsset_t;

/*----------------------------------------------------------------------------
 * VexTokenPkOperation_t - PK without Assets operation codes
 */
typedef enum
{
    VEXTOKEN_PK_OP_NUMLOAD = 0x01,
    VEXTOKEN_PK_OP_NUMSETN = 0x03,
    VEXTOKEN_PK_OP_MODEXPE = 0x04,
    VEXTOKEN_PK_OP_MODEXPD,
    VEXTOKEN_PK_OP_MODEXPCRT,
    VEXTOKEN_PK_OP_ECMONTMUL = 0x0A,
    VEXTOKEN_PK_OP_ECCMUL,
    VEXTOKEN_PK_OP_ECCADD,
    VEXTOKEN_PK_OP_DSASIGN,
    VEXTOKEN_PK_OP_DSAVERIFY,
    VEXTOKEN_PK_OP_ECDSASIGN,
    VEXTOKEN_PK_OP_ECDSAVERIFY,
} VexTokenPkOperation_t;


/*----------------------------------------------------------------------------
 * VexTokenSecureTimer_t - Secure Timer operation codes
 */
typedef enum
{
    VEXTOKEN_SECURETIMER_START = 0,     // (Re)Start a timer
    VEXTOKEN_SECURETIMER_STOP,          // Read elapsed time and stop the timer
    VEXTOKEN_SECURETIMER_READ,          // Read elapsed time
} VexTokenSecureTimer_t;

/*----------------------------------------------------------------------------
 * Token/HW/Algorithm related limits
 */
#define VEXTOKEN_DMA_MAXLENGTH           0x001FFFFF  // 2 MB - 1 bytes
#define VEXTOKEN_DMA_TOKENID_SIZE        4           // bytes

/*----------------------------------------------------------------------------
 * DMA data block must be an integer multiple of a work block size (in bytes)
 */
#define VEXTOKEN_DMA_ALGO_BLOCKSIZE_HASH 64
#define VEXTOKEN_DMA_ALGO_BLOCKSIZE_AES  16
#define VEXTOKEN_DMA_ALGO_BLOCKSIZE_DES  8
#define VEXTOKEN_DMA_ALGO_BLOCKSIZE_ARC4 4
#define VEXTOKEN_DMA_ALGO_BLOCKSIZE_NOP  4

/*----------------------------------------------------------------------------
 * VexToken_Command_t - Command Token
 */
typedef struct
{
    uint32_t OpCode;                    // Service group
    uint32_t SubCode;                   // Specific service code in the group
    uint32_t fFromUserSpace;            // From User Space indication

    union
    {
        struct
        {
            const uint8_t * In_p;
            uint8_t * Out_p;
            uint32_t Size;
        } Nop;
        struct
        {
            const uint8_t * SrcData_p;
            uint8_t * DstData_p;
            uint32_t Algorithm;
            uint32_t Mode;
            uint32_t TempAssetId;
            uint32_t KeyAssetId;
            uint32_t DataSize;
            uint32_t KeySize;
            uint8_t Key[64];            // AES-XTS requires 512bit (2 * 256bit)
            uint8_t IV[16];
            uint8_t f8_IV[16];          // IV for AES f8 mode
            uint8_t f8_SaltKey[16];     // Salt key for AES f8 mode
            uint8_t f8_SaltKeySize;     // Salt key size for AES f8 mode
            uint8_t NonceLength;        // Nonce length for ChaCha20
            uint8_t fEncrypt;
        } Cipher;
        struct
        {
            const uint8_t * AAD_p;
            const uint8_t * SrcData_p;
            uint8_t * DstData_p;
            uint32_t Algorithm;
            uint32_t Mode;
            uint32_t KeyAssetId;
            uint32_t AADSize;
            uint32_t DataSize;
            uint32_t KeySize;
            uint8_t Key[32];
            uint8_t NonceHashKey[16];
            uint8_t IV[16];
            uint8_t Tag[16];
            uint8_t fEncrypt;
            uint8_t NonceHashKeySize;  // Actual Nonce/HashKey size
            uint8_t GCMMode;           // GCMMode
            uint8_t TagSize;           // Tag size
        } CipherAE;
        struct
        {
            const uint8_t * Data_p;
            uint64_t TotalMessageLength;
            uint32_t Algorithm;
            uint32_t Mode;
            uint32_t TempAssetId;
            uint32_t DataSize;
        } Hash;
        struct
        {
            const uint8_t * Data_p;
            uint64_t TotalMessageLength;
            uint32_t Algorithm;
            uint32_t Mode;
            uint32_t TempAssetId;
            uint32_t KeyAssetId;
            uint32_t MacAssetId;
            uint32_t DataSize;
            uint32_t KeySize;
            uint32_t MacSize;
            uint8_t Key[128];
            uint8_t Mac[64];
        } Mac;
        struct
        {
            const uint8_t * SrcData_p;
            uint8_t * DstData_p;
            uint32_t KeyAssetId;
            uint32_t SrcDataSize;
            uint32_t DstDataSize;
            uint32_t KeySize;
            uint8_t Key[32];
            uint8_t fWrap;
        } KeyWrap;
#ifdef VEX_ENABLE_ENCRYPTED_VECTOR
        struct
        {
            uint8_t * Data_p;
            uint32_t AssetId;
            uint32_t DataSize;
            uint16_t AssetNumber;
        } EncVector;
#endif
        struct
        {
            uint16_t FroBlockKey;
            uint16_t SampleCycles;
            uint8_t SampleDiv;
            uint8_t NoiseBlocks;
            uint8_t AutoSeed;
            uint8_t fLoadStart;
            uint8_t fReseed;
        } TrngConfig;
        struct
        {
            uint8_t * Data_p;
            uint16_t Size;
        } Random;
        struct
        {
            const uint8_t * InputData_p;
            uint8_t * OutputData_p;
            uint32_t InputDataSize;
            uint32_t OutputDataSize;
            uint16_t Size;
            uint16_t Pattern;
            uint8_t Test;
        } DrbgPP;
        struct
        {
            const uint8_t * InputData_p;
            uint16_t Size;
            uint16_t AdaptProp4kCutoff;
            uint16_t AdaptProp4kCount;
            uint8_t AdaptProp4kData;
            uint8_t AdaptProp64Cutoff;
            uint8_t AdaptProp64Count;
            uint8_t AdaptProp64Data;
            uint8_t RepCntCutoff;
            uint8_t RepCntCount;
            uint8_t RepCntData;
        } NrbgVerify;
        struct
        {
            uint64_t Policy;
            uint32_t Size;
        } AssetCreate;
        struct
        {
            uint32_t AssetId;
        } AssetDelete;
        struct
        {
            uint16_t Number;
        } AssetSearch;
        struct
        {
            uint32_t Method;
            const uint8_t * KeyBlob_p;
            uint32_t AssetId;
            uint32_t KekAssetId;
            uint32_t KeyBlobSize;
            uint32_t AssociatedDataSize;
            uint8_t AssociatedData[224];
        } AssetLoadImport;
        struct
        {
            uint32_t Method;
            const uint8_t * Salt_p;
            const uint8_t * IV_p;
            uint32_t AssetId;
            uint32_t KdkAssetId;
            uint32_t AssociatedDataSize;
            uint32_t SaltSize;
            uint32_t IVSize;
            uint8_t AssociatedData[224];
            uint8_t fCounter;
            uint8_t fRFC5869;
        } AssetLoadDerive;
        struct
        {
            uint32_t Method;
            const uint8_t * Data_p;
            uint8_t * KeyBlob_p;
            uint32_t AssetId;
            uint32_t KekAssetId;
            uint32_t DataSize;
            uint32_t AssociatedDataSize;
            uint32_t KeyBlobSize;
            uint8_t AssociatedData[224];
        } AssetLoadPlaintext;
        struct
        {
            uint32_t Method;
            uint8_t * KeyBlob_p;
            uint32_t AssetId;
            uint32_t KekAssetId;
            uint32_t AssociatedDataSize;
            uint32_t KeyBlobSize;
            uint8_t AssociatedData[224];
        } AssetLoadRandom;
        struct
        {
            uint32_t Method;
            const uint8_t * KeyBlob_p;
            uint32_t AssetId;
            uint32_t KekAssetId;
            uint32_t KeyBlobSize;
        } AssetLoadAesunwrap;
        struct
        {
            const uint8_t * KeyBlob_p;
            uint32_t KeyBlobSize;
            uint32_t AssociatedDataSize;
            uint16_t AssetNumber;
            uint16_t PolicyNumber;
            uint8_t AssociatedData[224];
            uint8_t fAddCRC;
        } OTPDataWrite;
        struct
        {
            const uint8_t * KeyBlob_p;
            uint32_t Identity;
            uint32_t KeyBlobSize;
            uint32_t AssociatedDataSize;
            uint16_t AssetNumber;
            uint16_t SampleCycles;
            uint8_t AssociatedData[224];
            uint8_t NoiseBlocks;
            uint8_t SampleDiv;
            uint8_t AutoSeed;
            uint8_t f128bit;
            uint8_t fAddCRC;
        } ProvisionRandomHUK;
        struct
        {
            uint32_t Operation;
            uint32_t AssetId;
            uint32_t fSecond;
        } SecureTimer;
        struct
        {
            const uint8_t * Data_p;
            uint32_t AssetId;
            uint32_t DataSize;
        } PublicData;
        struct
        {
            const uint8_t * Data_p;
            uint32_t AssetId;
            uint32_t DataSize;
        } MonotonicCounter;
        struct
        {
            uint32_t Method;
            const uint8_t * HashData_p;
            uint8_t * Sign_p;
            uint64_t TotalMessageSize;
            uint32_t ModulusSizeInBits;
            uint32_t KeyAssetId;
            uint32_t DomainAssetId;
            uint32_t DigestAssetId;
            uint32_t HashDataSize;
            uint32_t SaltSize;          // RSA-PSS only
            uint32_t SignSize;
        } PkAssetSignVerify;
        struct
        {
            uint32_t Method;
            const  uint8_t * SrcData_p;
            const uint8_t * DstData_p;
            uint32_t ModulusSizeInBits;
            uint32_t KeyAssetId;
            uint32_t DomainAssetId;
            uint32_t SrcDataSize;
            uint32_t DstDataSize;
        } PkAssetEncrypt;
        struct
        {
            uint32_t Method;
            const uint8_t * KeyBlob_p;
            uint8_t * PubKey_p;
            uint32_t ModulusSizeInBits;
            uint32_t DivisorSizeInBits;
            uint32_t PubKeyAssetId;
            uint32_t PrivKeyAssetId;
            uint32_t DomainAssetId;
            uint32_t KekAssetId;
            uint32_t AssociatedDataSize;
            uint32_t KeyBlobSize;
            uint32_t PubKeySize;
            uint8_t AssociatedData[224];
        } PkAssetGenKey;
        struct
        {
            uint32_t Method;
            uint32_t ModulusSizeInBits;
            uint32_t DivisorSizeInBits;
            uint32_t PubKeyAssetId;
            uint32_t PrivKeyAssetId;
            uint32_t DomainAssetId;
        } PkAssetKeyCheck;
        struct
        {
            uint32_t Method;
            const uint8_t * OtherInfo_p;
            const uint32_t * AssetIdList_p;
            uint32_t ModulusSizeInBits;
            uint32_t DivisorSizeInBits;
            uint32_t PubKeyAssetId;
            uint32_t PrivKeyAssetId;
            uint32_t DomainAssetId;
            uint32_t PubKey2AssetId;
            uint32_t PrivKey2AssetId;
            uint32_t OtherInfoSize;
            uint32_t AssetIdListSize;
        } PkAssetGenSharedSecret;
        struct
        {
            uint32_t Method;
            const uint8_t * Data_p;
            uint32_t ModulusSizeInBits;
            uint32_t KeyAssetId;
            uint32_t AssetId;
            uint32_t DataSize;
            uint32_t AdditionalInputSize;
            uint8_t AdditionalInput[208];
        } PkAssetWrap;
        struct
        {
            const uint8_t * InData_p;
            const uint8_t * OutData_p;
            uint32_t Operation;
            uint32_t PublicExponent;
            uint32_t InDataSize;
            uint32_t OutDataSize;
            uint8_t Nwords;
            uint8_t Mwords;
            uint8_t Mmask;
            uint8_t Index;
        } PkOperation;
        struct
        {
            const uint8_t * Sign_p;
            uint32_t AuthStateAssetId;
            uint32_t AuthKeyAssetId;
            uint32_t SignSize;
            uint8_t Nonce[16];
        } AuthUnlock;
        struct
        {
            uint32_t AuthStateAssetId;
            uint32_t fSet;
        } SecureDebug;
        struct
        {
            uint32_t AssetId;
        } eMMCRdRequest;
        struct
        {
            const uint8_t * Data_p;
            uint32_t StateAssetId;
            uint32_t DataSize;
            uint32_t MacSize;
            uint8_t Mac[32];
        } eMMCRdVerifyWrReqVer;
        struct
        {
            const uint8_t * Data_p;
            uint32_t Algorithm;
            uint32_t KeyAssetId;
            uint32_t StateAssetId;
            uint32_t KeySize;
            uint32_t DataSize;
            uint32_t AssociatedDataSize;
            uint8_t AssociatedData[224];
        } ExtService;
        struct
        {
            uint32_t Address;
            uint32_t Value;
        } Register;
        struct
        {
            uint32_t User1;
            uint32_t User2;
            uint32_t User3;
            uint32_t User4;
        } SystemDefineUsers;
        struct
        {
            const uint8_t * DataBlob_p;
            uint32_t StateAssetId;
            uint32_t KeyAssetId;
            uint32_t AssociatedDataSize;
            uint32_t DataBlobSize;
            uint8_t AssociatedData[224];
        } SystemHibernation;
        struct
        {
            uint16_t On;
            uint16_t Off;
        } ClockSwitch;
    } Service;
} VexToken_Command_t;

/*----------------------------------------------------------------------------
 * VexTokenResult_t - Result codes
 */
typedef enum
{
    VEXTOKEN_RESULT_NO_ERROR                 =  (0x00), // No error or warning
    VEXTOKEN_RESULT_SEQ_INVALID_TOKEN     = -(0x01), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_PARAMETER = -(0x02), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_KEYSIZE   = -(0x03), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_LENGTH    = -(0x04), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_LOCATION  = -(0x05), // Error
    VEXTOKEN_RESULT_SEQ_CLOCK_ERROR       = -(0x06), // Error
    VEXTOKEN_RESULT_SEQ_ACCESS_ERROR      = -(0x07), // Error
    VEXTOKEN_RESULT_SEQ_UNWRAP_ERROR      = -(0x0A), // Error
    VEXTOKEN_RESULT_SEQ_DATA_OVERRUN      = -(0x0B), // Error
    VEXTOKEN_RESULT_SEQ_ASSET_CHECKSUM    = -(0x0C), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_ASSET     = -(0x0D), // Error
    VEXTOKEN_RESULT_SEQ_OVERFLOW_FULL     = -(0x0E), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_ADDRESS   = -(0x0F), // Error
    VEXTOKEN_RESULT_SEQ_Z1USED            =  (0x10), // Warning
    VEXTOKEN_RESULT_SEQ_INVALID_MODULUS   = -(0x11), // Error
    VEXTOKEN_RESULT_SEQ_VERIFY_ERROR      = -(0x12), // Error
    VEXTOKEN_RESULT_SEQ_INVALID_STATE     = -(0x13), // Error
    VEXTOKEN_RESULT_SEQ_OTP_WRITE_ERROR   = -(0x14), // Error
    VEXTOKEN_RESULT_SEQ_PANIC             = -(0x1F)  // Error
} VexTokenResult_t;

/*----------------------------------------------------------------------------
 * VexToken_Result_t - Result token
 */
typedef struct
{
    int Result;

    union
    {
        struct
        {
            uint8_t IV[16];
        } Cipher;
        struct
        {
            uint8_t Tag[16];
        } CipherAE;
        struct
        {
            uint8_t Digest[64];
        } Hash;
        struct
        {
            uint8_t Mac[64];
        } Mac;
        struct
        {
            const uint8_t * InputData_p;
            uint16_t Size;
            uint16_t AdaptProp4kCutoff;
            uint16_t AdaptProp4kCount;
            uint8_t AdaptProp4kData;
            uint8_t AdaptProp4kFail;
            uint8_t AdaptProp64Cutoff;
            uint8_t AdaptProp64Count;
            uint8_t AdaptProp64Data;
            uint8_t AdaptProp64Fail;
            uint8_t RepCntCutoff;
            uint8_t RepCntCount;
            uint8_t RepCntData;
        } NrbgVerify;
        struct
        {
            uint32_t Size;
        } KeyWrap;
#ifdef VEX_ENABLE_ENCRYPTED_VECTOR
        struct
        {
            uint32_t DataSize;
        } EncVector;
#endif
        struct
        {
            uint32_t AssetId;
        } AssetCreate;
        struct
        {
            uint32_t AssetId;
            uint32_t Size;
        } AssetSearch;
        struct
        {
            uint32_t KeyBlobSize;
        } AssetLoad;
        struct
        {
            uint32_t KeyBlobSize;
        } ProvisionRandomHUK;
        struct
        {
            uint32_t AssetId;
            uint32_t ElapsedTime;
        } SecureTimer;
        struct
        {
            uint32_t DataSize;
        } PublicData;
        struct
        {
            uint32_t DataSize;
        } MonotonicCounter;
        struct
        {
            uint32_t KeyBlobSize;
        } PkAssetGenKey;
        struct
        {
            uint32_t StateAssetId;
        } PkAssetSignVerify;
        struct
        {
            uint32_t WrappedDataSize;
        } PkAssetWrap;
        struct
        {
            uint32_t OutDataSize;
        } PkOperation;
        struct
        {
            uint8_t Nonce[16];
        } AuthUnlock;
        struct
        {
            uint32_t AssetId;
            uint8_t Nonce[16];
        } eMMCRdRequest;
        struct
        {
            uint8_t Mac[32];
        } eMMCRdVerifyWrReqVer;
        struct
        {
            uint8_t Mac[32];
        } ExtService;
        struct
        {
            uint32_t Value;
        } Register;
        struct
        {
            struct
            {
                uint16_t MemorySizeInBytes;
                uint8_t Major;          // 0..9
                uint8_t Minor;          // 0..9
                uint8_t Patch;          // 0..9
            } Hardware;
            struct
            {
                uint8_t Major;          // 0..9
                uint8_t Minor;          // 0..9
                uint8_t Patch;          // 0..9
            } Firmware;
            struct
            {
                uint32_t Identity;
                uint8_t HostID;         // 0..7
                uint8_t NonSecure;      // 0=Secure, !0=Non-Secure
                uint8_t CryptoOfficer;  // 0=Not available, !0=Available
                uint8_t Mode;           // 0=Non-FIPS mode, 4..6=Error mode, 15=FIPS mode
            } Self;
            struct
            {
                uint16_t ErrorLocation;
                uint8_t ErrorCode;
            } OTP;
        } SystemInfo;
        struct
        {
            uint32_t DataBlobSize;
        } SystemHibernation;
    } Service;
} VexToken_Result_t;

/*----------------------------------------------------------------------------
 * vex_LogicalToken
 *
 * This function handles the logical tokens coming from the VaultIP Abraction
 * Layer (VAL).
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
vex_LogicalToken(
        VexToken_Command_t * const CommandToken_p,
        VexToken_Result_t * const ResultToken_p);


/*----------------------------------------------------------------------------
 * vex_Init
 *
 * This function initializes the VEX Adapter.
 *
*
 * Return Value:
 *     0 if successful, otherwise <0
 */
int
vex_Init(void);


/*----------------------------------------------------------------------------
 * vex_UnInit
 *
 * This function un-initializes the VEX Adapter.
 *
 * Return Value:
 *     None
 */
void
vex_UnInit(void);


#endif /* Include Guard */


/* end of file adapter_vex.h */
