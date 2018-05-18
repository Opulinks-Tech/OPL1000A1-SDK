/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/
#include "basic_defs.h"

/** Bit Define */
#define BIT_0   0x00000001U
#define BIT_1   0x00000002U
#define BIT_2   0x00000004U
#define BIT_3   0x00000008U
#define BIT_4   0x00000010U
#define BIT_5   0x00000020U
#define BIT_6   0x00000040U
#define BIT_7   0x00000080U
#define BIT_8   0x00000100U
#define BIT_9   0x00000200U
#define BIT_10  0x00000400U
#define BIT_11  0x00000800U
#define BIT_12  0x00001000U
#define BIT_13  0x00002000U
#define BIT_14  0x00004000U
#define BIT_15  0x00008000U
#define BIT_16  0x00010000U
#define BIT_17  0x00020000U
#define BIT_18  0x00040000U
#define BIT_19  0x00080000U
#define BIT_20  0x00100000U
#define BIT_21  0x00200000U
#define BIT_22  0x00400000U
#define BIT_23  0x00800000U
#define BIT_24  0x01000000U
#define BIT_25  0x02000000U
#define BIT_26  0x04000000U
#define BIT_27  0x08000000U
#define BIT_28  0x10000000U
#define BIT_29  0x20000000U
#define BIT_30  0x40000000U
#define BIT_31  0x80000000U

/** VaultIP Core Register Basic & Offset Address  */
#define EIP130_MAILBOX_IN_BASE                  0x0000
#define EIP130_MAILBOX_OUT_BASE                 0x0000
#define EIP130_REGISTEROFFSET_MAILBOX_STAT      0x3F00
#define EIP130_REGISTEROFFSET_MAILBOX_CTRL      0x3F00
#define EIP130_REGISTEROFFSET_MAILBOX_RAWSTAT   0x3F04
#define EIP130_REGISTEROFFSET_MAILBOX_RESET     0x3F04
#define EIP130_REGISTEROFFSET_MAILBOX_LINKID    0x3F08
#define EIP130_REGISTEROFFSET_MAILBOX_OUTID     0x3F0C
#define EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT   0x3F10
#define EIP130_REGISTEROFFSET_MODULE_STATUS     0x3FE0
#define EIP130_REGISTEROFFSET_EIP_OPTIONS2      0x3FF4
#define EIP130_REGISTEROFFSET_EIP_OPTIONS       0x3FF8
#define EIP130_REGISTEROFFSET_EIP_VERSION       0x3FFC

/** ITP */
#define ITP_SCRT_SRAM_BASE_ADDR                 0x0041a000

/** SCRT0 Core Register Adddress */
#define SCRT0_BASE_ADDR                         0x60000000

/** SCRT1 User-Defined Register Adddress */
#define SCRT1_BASE_ADDR                         0x6e000000

/** Interval for One Checking */
#define INTERVAL_EACH_CHECKING                  10

/** Maximum Checking times */
#define MAX_TIMES_CHECKING                      ( 2000 / INTERVAL_EACH_CHECKING )

/** SCRT Initialization */
int nl_scrt_Init_impl(void);

/** AES CCM [deprecated] */
bool nl_aes_ccm_impl (bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);

/** AES CCM */
int nl_scrt_aes_ccm_impl (int bEncrypt, unsigned char *sk, int sk_len, unsigned char *nonce, int nonce_len, unsigned char *adata, int adata_len, unsigned char *plain_text, unsigned char *encrypted_text, int text_len, unsigned char *tag, int tag_len);

/** AES CCM Sample */
void aes_ccm_sample(void);

/** HMAC SHA-1 */
bool nl_hmac_sha_1_impl(uint8_t *sk, int sk_len, uint8_t *in_data, int data_len, uint8_t *mac);

/** HMAC SHA-1 Sample */
void hmac_sha_1_sample(void);

/** AES ECB */
int nl_scrt_aes_ecb_impl(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len);

/** AES ECB Sample */
void aes_ecb_sample(void);

typedef int (*nl_scrt_Init_fp_t)(void);
typedef int (*nl_aes_ccm_fp_t)(bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);
typedef int (*nl_scrt_aes_ccm_fp_t)(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *nonce, int nonce_len, unsigned char *adata, int adata_len, unsigned char *plain_text, unsigned char *encrypted_text, int text_len, unsigned char *tag, int tag_len);
typedef int (*nl_hmac_sha_1_fp_t)(uint8_t *sk, int sk_len, uint8_t *in_data, int data_len, uint8_t *mac);
typedef int (*nl_scrt_aes_ecb_fp_t)(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len);

extern nl_scrt_Init_fp_t nl_scrt_Init;
extern nl_aes_ccm_fp_t nl_aes_ccm; //degraded
extern nl_scrt_aes_ccm_fp_t nl_scrt_aes_ccm;
extern nl_hmac_sha_1_fp_t nl_hmac_sha_1;
extern nl_scrt_aes_ecb_fp_t nl_scrt_aes_ecb;

/*
 * scrt_drv_func_init
 *
 * Interface Initialization: SCRT
 *
 */
void scrt_drv_func_init(void);


#if 0
#include "clib.h"
#include "api_val_sym.h"
#include "api_val_asset.h"
#include "api_val_result.h"
#include "api_val_buffers.h"
#include "testvectors_aes_basic.h"
#include "sfzutf.h"

#define SYM_CRYPT_ENCRYPT true
#define SYM_CRYPT_DECRYPT false
#define SYM_CRYPT_MAX_BUFLEN       2048 /** Maximum buffer length. */
#define SYM_CRYPT_IN_PLACE        0x0001
#define SYM_CRYPT_MULTIPART       0x0002
#define SYM_CRYPT_UNALIGNED       0x0004

typedef struct
{
    //int Index;
    ValSymContextPtr_t SymContext_p;
    ValAssetId_t KeyAssetId;
    uint8_t * ptx_p;
    uint8_t * ctx_p;
    uint32_t txt_len;
    uint32_t BlockSize;
    bool fPadding;
    uint8_t PaddingByte;
    uint32_t options;
    int misalign;
} SymCryptCtx_t;

/* Beware that the following mode definitions are not guaranteed to be
   numerically identical to the corresponding SFZCRYPTO_MODE_xxx macros. */
typedef enum
{
    SFZCRYPTO_MODE_ECB = 0,
    SFZCRYPTO_MODE_CBC,
    SFZCRYPTO_MODE_CTR,
    SFZCRYPTO_MODE_ICM,
    SFZCRYPTO_MODE_F8,
    SFZCRYPTO_MODE_XTS_AES,
    SFZCRYPTO_MODE_NUM        // must be last
} SfzCrypto_Mode_t;

int nl_scrt_Init(void);
int nl_scrt_aes_cbc(Nl_Scrt_Usr_t user, bool fDecrypt, const uint8_t *key, uint32_t keySize, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data);
int nl_scrt_aes_ctr(Nl_Scrt_Usr_t user, bool fDecrypt, const uint8_t *key, uint32_t keySize, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data);
int nl_scrt_aes_cmac(Nl_Scrt_Usr_t user, uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen);
int nl_scrt_aes_cbcmac(Nl_Scrt_Usr_t user, uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen, uint8_t * Iv_p);
ValStatus_t nl_scrt_trng_gen(Nl_Scrt_Usr_t user, const ValSize_t DataSize, ValOctetsOut_t * const  Data_p);
ValStatus_t nl_scrt_asym_ecc_gen_keypair(uint32_t KeySize, uint8_t * PubKey_p, uint8_t * PrivKey_p);
void nl_se_sample_test(void);
#endif


