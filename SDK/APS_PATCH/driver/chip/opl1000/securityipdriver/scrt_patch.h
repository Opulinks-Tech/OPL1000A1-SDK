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


#define SCRT_AES_CMAC_OUTPUT_LEN        16

#define SCRT_SHA_1_OUTPUT_LEN           20
#define SCRT_SHA_224_OUTPUT_LEN         32
#define SCRT_SHA_256_OUTPUT_LEN         32
#define SCRT_SHA_384_OUTPUT_LEN         64
#define SCRT_SHA_512_OUTPUT_LEN         64

#define SCRT_SHA_1_STEP_SIZE            64  // must be multiple of 64-bytes
#define SCRT_SHA_224_STEP_SIZE          64  // must be multiple of 64-bytes
#define SCRT_SHA_256_STEP_SIZE          64  // must be multiple of 64-bytes
#define SCRT_SHA_384_STEP_SIZE          128 // must be multiple of 128-bytes
#define SCRT_SHA_512_STEP_SIZE          128 // must be multiple of 128-bytes


typedef enum
{
    SCRT_STEP_NEW = 0,
    SCRT_STEP_CONTINUE,
    SCRT_STEP_FINAL,

    SCRT_STEP_MAX
} T_ScrtStep;

typedef enum
{
    SCRT_TYPE_SHA_1 = 0,
    SCRT_TYPE_SHA_224,
    SCRT_TYPE_SHA_256,
    SCRT_TYPE_SHA_384,
    SCRT_TYPE_SHA_512,

    SCRT_TYPE_MAX
} T_ScrtShaType;

typedef int (*nl_scrt_aes_cmac_fp_t)(uint8_t *u8aKey, uint8_t u8KeyLen, uint8_t *u8aInputBuf, uint32_t u32BufSize, uint32_t u32InputLen, uint8_t *u8aMac);
typedef int (*nl_scrt_hmac_sha_1_step_fp_t)(uint8_t type, uint32_t total_len, uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac);
typedef int (*nl_scrt_sha_fp_t)(uint8_t u8Type, uint8_t u8Step, uint32_t u32TotalLen, uint8_t *u8aData, uint32_t u32DataLen, uint8_t u8HasInterMac, uint8_t *u8aMac);


extern nl_scrt_aes_cmac_fp_t nl_scrt_aes_cmac_get;
extern nl_scrt_sha_fp_t nl_scrt_sha;

extern void scrt_drv_func_init_patch(void);


#endif //#ifndef __SCRT_PATCH_H__

