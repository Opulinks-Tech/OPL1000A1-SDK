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

#ifndef __SCRT_PATCH_H__
#define __SCRT_PATCH_H__


#include "scrt.h"


//#define SCRT_PRE_LINK
#define SCRT_ENABLE_UNLINK


#if 1

typedef int (*scrt_status_chk_fp_t)(uint32_t u32Mask, uint32_t u32Value);
typedef int (*scrt_asset_create_fp_t)(uint8_t u8Idx, uint32_t *pu32AsId);
typedef int (*scrt_asset_delete_fp_t)(uint8_t u8Idx, uint32_t u32AsId);
typedef int (*scrt_asset_load_fp_t)(uint8_t u8Idx, uint32_t u32AsId, uint8_t *u8aAddr, uint16_t u16Len);
typedef int (*scrt_asset_get_fp_t)(uint8_t u8Idx, uint32_t u32AsId, uint8_t *u8aBuf, uint32_t u32BufLen, uint32_t u32OutputLen);
typedef int (*scrt_cmd_fp_t)(uint8_t u8Idx);
typedef uint8_t (*scrt_res_alloc_fp_t)(void);
typedef void (*scrt_res_free_fp_t)(uint8_t u8Idx);

typedef int (*nl_scrt_common_fp_t)(void);
typedef int (*nl_scrt_ecdh_key_pair_gen_fp_t)(void *pPubKey, uint32_t *u32aPrivKey, uint32_t *pu32PrivKeyId);
typedef int (*nl_scrt_ecdh_dhkey_gen_fp_t)(uint8_t *u8aPubKeyX, uint8_t *u8aPubKeyY, uint32_t *u32aPrivKey, void *pDhKey, uint32_t u32PrivKeyId);
typedef int (*nl_scrt_key_delete_fp_t)(uint32_t u32KeyId);


// internal
extern nl_scrt_common_fp_t scrt_param_init;
extern nl_scrt_common_fp_t scrt_mb_init;
extern scrt_cmd_fp_t scrt_trng_init;
extern scrt_cmd_fp_t scrt_eng_init;
extern scrt_status_chk_fp_t scrt_status_chk;
extern scrt_asset_create_fp_t scrt_ecdh_key_param_init;
extern scrt_asset_create_fp_t scrt_ecdh_priv_key_asset_create;
extern scrt_asset_create_fp_t scrt_ecdh_pub_key_asset_create;
extern scrt_asset_create_fp_t scrt_ecdh_shared_secret_asset_create;
extern scrt_asset_delete_fp_t scrt_asset_delete;
extern scrt_asset_load_fp_t scrt_asset_load;
extern scrt_asset_get_fp_t scrt_asset_get;
extern scrt_cmd_fp_t scrt_reset;
extern nl_scrt_common_fp_t scrt_internal_reset;
extern scrt_cmd_fp_t scrt_link;
extern scrt_cmd_fp_t scrt_unlink;
extern nl_scrt_common_fp_t scrt_access_lock;
extern nl_scrt_common_fp_t scrt_access_unlock;
extern nl_scrt_common_fp_t scrt_res_lock;
extern nl_scrt_common_fp_t scrt_res_unlock;
extern nl_scrt_common_fp_t scrt_ecdh_lock;
extern nl_scrt_common_fp_t scrt_ecdh_unlock;
extern nl_scrt_common_fp_t scrt_sem_create;
extern scrt_res_alloc_fp_t scrt_res_alloc;
extern scrt_res_free_fp_t scrt_res_free;

// external
extern nl_scrt_common_fp_t nl_scrt_otp_status_get;
extern nl_scrt_ecdh_key_pair_gen_fp_t nl_scrt_ecdh_key_pair_gen;
extern nl_scrt_ecdh_dhkey_gen_fp_t nl_scrt_ecdh_dhkey_gen;
extern nl_scrt_key_delete_fp_t nl_scrt_key_delete;

/*
 * scrt_drv_func_init
 *
 * Interface Initialization: SCRT
 *
 */
void scrt_drv_func_init_patch(void);

#else

/*
 * nl_scrt_init_patch - Initialize scrt module.
 *
 * @param [in] None
 *    N/A
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_init_patch(void);

/*
 * nl_scrt_otp_status_get - Get OTP status.
 *
 * @param [in] None
 *    N/A
 *
 * @return 1 success
 *    OTP ready
 *
 * @return 0 fail
 *    OTP not ready
 *
 */
int nl_scrt_otp_status_get(void);

/*
 * nl_scrt_ecdh_key_pair_gen - Generate ECDH key pair.
 *
 * @param [in] pPubKey
 *    Public Key
 *
 * @param [out] u32aPrivKey
 *    Private Key
 *
 * @param [in/out] pu32PrivKeyId
 *    [in]: ID of previous private key
 *    [out]: ID of current private key
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_ecdh_key_pair_gen(void *pPubKey, uint32_t *u32aPrivKey, uint32_t *pu32PrivKeyId);

/*
 * nl_scrt_ecdh_dhkey_gen - Generate ECDH shared secret (DHKey).
 *
 * @param [in] u8aPubKeyX
 *    Sub-vector X of Public Key
 *
 * @param [in] u8aPubKeyY
 *    Sub-vector Y of Public Key
 *
 * @param [in] u32aPrivKey
 *    Private Key
 *
 * @param [out] pDhKey
 *    DHKey
 *
 * @param [in] pu32PrivKeyId
 *    [in]: ID of private key
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_ecdh_dhkey_gen(uint8_t *u8aPubKeyX, uint8_t *u8aPubKeyY, uint32_t *u32aPrivKey, void *pDhKey, uint32_t u32PrivKeyId);

/*
 * nl_scrt_key_delete - Delete key.
 *
 * @param [in] u32KeyId
 *    [in]: ID of key
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_key_delete(uint32_t u32KeyId);

/*
 * nl_aes_ccm  - AES CCM Operation. [deprecated]
 *
 * @param [in] bEncrypt
 *    true:encrypt false:decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in] nonce
 *    Nonce
 *
 * @param [in] nonce_len
 *    Size of Nonce
 *
 * @param [in] adata
 *    Additional Associated Data
 *
 * @param [in] adata_len
 *    Size of Additional Associated Data
 *
 * @param [in/out] plain_text
 *    Plain Text Data
 *
 * @param [in/out] encrypted_text
 *    Encrypted Data
 *
 * @param [in] text_len
 *    The length of the Data Transfer
 *
 * @param [out] tag
 *    Output Tag Data
 *
 * @param [in] tag_len
 *    Size of the Output Tag Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 * @deprecated It will be removed in the future
 *
 */
int nl_aes_ccm_patch(bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);

/*
 * nl_scrt_aes_ccm - Perform the AES CCM Operation.
 *
 * @param [in] bEncrypt
 *    1:encrypt 0:decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in] nonce
 *    Nonce
 *
 * @param [in] nonce_len
 *    Size of Nonce
 *
 * @param [in] adata
 *    Additional Associated Data
 *
 * @param [in] adata_len
 *    Size of Additional Associated Data
 *
 * @param [in/out] plain_text
 *    Plain Text Data
 *
 * @param [in/out] encrypted_text
 *    Encrypted Data
 *
 * @param [in] text_len
 *    The length of the Data Transfer
 *
 * @param [out] tag
 *    Output Tag Data
 *
 * @param [in] tag_len
 *    Size of the Output Tag Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_aes_ccm_patch(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *nonce, int nonce_len, unsigned char *adata, int adata_len, unsigned char *plain_text, unsigned char *encrypted_text, int text_len, unsigned char *tag, int tag_len);

/*
 * nl_hmac_sha_1_patch - Perform the HMAC SHA1 Operation
 *
 * @param [in] sk
 *      The Key
 *
 * @param [in] sk_len
 *      The Key Length
 *
 * @param [in] in_data
 *      The Input Data which to be handled
 *
 * @param [in] in_data_len
 *      The length of Input Data
 *
 * @param [out] mac
 *      Output MAC Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_hmac_sha_1_patch(uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac);

/*
 * nl_scrt_aes_ecb_patch - Perform the AES ECB Operation.
 *
 * @param [in] bEncrypt
 *    1:Encrypt
 *    0:Decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in/out] data_in
 *    Input Data
 *
 * @param [out] data_out
 *    Output Data
 *
 * @param [in] data_len
 *    The length of the Data Transfer
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_aes_ecb_patch(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len);

/** AES CMAC */
int nl_scrt_aes_cmac( unsigned char *sk, int sk_len, unsigned char *data_in, int data_in_len, unsigned char *data_out, int data_out_len);

/** TRNG */
int nl_scrt_trng(int size, unsigned char *trng_out);
#endif // allen chu test

#endif

