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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "scrt.h"
#include "scrt_patch.h"
#include "ipc.h"
#include "scrt_cmd.h"


// idx: T_ScrtMbIdx
#define SCRT_BASE_ADDR(idx)             (SCRT0_BASE_ADDR + ((idx & 0x03) << 10))

#define SCRT_CTRL_WRITE_MSK(idx)        (0x01 << (idx * 4))
#define SCRT_CTRL_READ_MSK(idx)         (0x02 << (idx * 4))
#define SCRT_CTRL_LINK_MSK(idx)         (0x04 << (idx * 4))
#define SCRT_CTRL_UNLINK_MSK(idx)       (0x08 << (idx * 4))
#define SCRT_STATUS_WRITE_MSK           SCRT_CTRL_WRITE_MSK
#define SCRT_STATUS_READ_MSK            SCRT_CTRL_READ_MSK
#define SCRT_STATUS_LINK_MSK            SCRT_CTRL_LINK_MSK
#define SCRT_STATUS_AVAIL_MSK           SCRT_CTRL_UNLINK_MSK

#define SCRT_STAT_CTRL_ADDR             (SCRT0_BASE_ADDR + EIP130_REGISTEROFFSET_MAILBOX_STAT)
#define SCRT_OTP_STATUS_ADDR            (0x30002400)

#define SCRT_ID                         0x4F5A3647
#define SCRT_PRIV_KEY_LEN               32
#define SCRT_PUB_KEY_VEC_LEN            32
#define SCRT_KEY_PARAM_FMT_LEN          224
#define SCRT_PRIV_KEY_FMT_LEN           36
#define SCRT_PUB_KEY_FMT_LEN            72
#define SCRT_SHARED_SRCRET_LEN          32

#define SCRT_WAIT_RES_SEM               osWaitForever
#define SCRT_WAIT_ECDH_SEM              osWaitForever

#define SCRT_WAIT_RSP_CNT               (20000000)


//#define SCRT_ACCESS_SEM

#ifdef SCRT_ACCESS_SEM
    #define SCRT_WAIT_ACCESS_SEM        (2000) // ms
#else
    #define SCRT_WAIT_RES               10 // ms
    #define SCRT_WAIT_RES_MAX_CNT       200
#endif

#define SCRT_MALLOC                 malloc
#define SCRT_FREE                   free

#define SCRT_DEBUG

#ifdef SCRT_DEBUG
    #define SCRT_LOGI(...)
    //#define SCRT_LOGI(args...)      tracer_log(LOG_LOW_LEVEL, args)
    #define SCRT_LOGE(args...)      tracer_log(LOG_HIGH_LEVEL, args)
#else
    #define SCRT_LOGI(...)
    #define SCRT_LOGE(...)
#endif

#define SCRT_CHECK

#ifdef SCRT_CHECK
    #define SCRT_ASSERT(a)    \
    if(!(a))    \
    {     \
        tracer_drct_printf("TS_ASSERT FAILED '" #a "' %s %s:%d\n", __func__, __FILE__, __LINE__);   \
        while(1);    \
    }
#else
    #define SCRT_ASSERT(...)
#endif


typedef enum
{
    SCRT_MB_IDX_0 = 0,
    SCRT_MB_IDX_1,
    SCRT_MB_IDX_2,
    SCRT_MB_IDX_3,

    SCRT_MB_IDX_MAX
} T_ScrtMbIdx;

typedef enum
{
    SCRT_TOKEN_ID_RESET = 0xE000,
    SCRT_TOKEN_ID_TRNG_CFG = 0xE010,
    SCRT_TOKEN_ID_ASSET_LOAD = 0xE020,
    SCRT_TOKEN_ID_ASSET_CREATE_KEY_PARAM = 0xE030,
    SCRT_TOKEN_ID_ASSET_CREATE_PRIV_KEY = 0xE040,
    SCRT_TOKEN_ID_ASSET_CREATE_PUB_KEY = 0xE050,
    SCRT_TOKEN_ID_ASSET_CREATE_SHARED_SECRET = 0xE060,
    SCRT_TOKEN_ID_SHARED_SECRET_GEN = 0xE070,
    SCRT_TOKEN_ID_PUB_DATA_READ = 0xE080,
    SCRT_TOKEN_ID_ASSET_DELETE = 0xE090,
    SCRT_TOKEN_ID_KEY_PAIR_GEN = 0xE0A0,
    SCRT_TOKEN_ID_AES_CCM_ENCRYPT = 0xE0B0,
    SCRT_TOKEN_ID_AES_CCM_DECRYPT = 0xE0C0,
    SCRT_TOKEN_ID_HMAC_SHA_1 = 0xE0D0,
    SCRT_TOKEN_ID_AES_ECB = 0xE0E0,

    SCRT_TOKEN_ID_MAX = 0xEFFF
} T_ScrtTokenId;

typedef struct
{
    uint8_t u8Used;
    uint8_t u8MbIdx;
} T_ScrtRes;


// internal
RET_DATA nl_scrt_common_fp_t scrt_param_init;
RET_DATA nl_scrt_common_fp_t scrt_mb_init;
RET_DATA scrt_cmd_fp_t scrt_trng_init;
RET_DATA scrt_cmd_fp_t scrt_eng_init;
RET_DATA scrt_status_chk_fp_t scrt_status_chk;
RET_DATA scrt_asset_create_fp_t scrt_ecdh_key_param_init;
RET_DATA scrt_asset_create_fp_t scrt_ecdh_priv_key_asset_create;
RET_DATA scrt_asset_create_fp_t scrt_ecdh_pub_key_asset_create;
RET_DATA scrt_asset_create_fp_t scrt_ecdh_shared_secret_asset_create;
RET_DATA scrt_asset_delete_fp_t scrt_asset_delete;
RET_DATA scrt_asset_load_fp_t scrt_asset_load;
RET_DATA scrt_asset_get_fp_t scrt_asset_get;
RET_DATA scrt_cmd_fp_t scrt_reset;
RET_DATA nl_scrt_common_fp_t scrt_internal_reset;
RET_DATA scrt_cmd_fp_t scrt_link;

#ifdef SCRT_ENABLE_UNLINK
RET_DATA scrt_cmd_fp_t scrt_unlink;
#endif

#ifdef SCRT_ACCESS_SEM
RET_DATA nl_scrt_common_fp_t scrt_access_lock;
RET_DATA nl_scrt_common_fp_t scrt_access_unlock;
#endif

RET_DATA nl_scrt_common_fp_t scrt_res_lock;
RET_DATA nl_scrt_common_fp_t scrt_res_unlock;
RET_DATA nl_scrt_common_fp_t scrt_ecdh_lock;
RET_DATA nl_scrt_common_fp_t scrt_ecdh_unlock;
RET_DATA nl_scrt_common_fp_t scrt_sem_create;
RET_DATA scrt_res_alloc_fp_t scrt_res_alloc;
RET_DATA scrt_res_free_fp_t scrt_res_free;

// external
RET_DATA nl_scrt_common_fp_t nl_scrt_otp_status_get;
RET_DATA nl_scrt_ecdh_key_pair_gen_fp_t nl_scrt_ecdh_key_pair_gen;
RET_DATA nl_scrt_ecdh_dhkey_gen_fp_t nl_scrt_ecdh_dhkey_gen;
RET_DATA nl_scrt_key_delete_fp_t nl_scrt_key_delete;


T_ScrtRes g_tScrtRes[SCRT_MB_IDX_MAX] = {0};
osSemaphoreId g_tScrtResSem = NULL;
osSemaphoreId g_tScrtEcdhSem = NULL;

#ifdef SCRT_ACCESS_SEM
osSemaphoreId g_tScrtAccessSem = NULL;
#endif

//uint8_t g_u8ScrtOtpReady = 0;
//uint32_t g_u32ScrtKeyParamAsId = 0;
uint32_t g_u32ScrtWaitResCnt = 0;
uint32_t g_u32ScrtWaitRspCnt = 0;

uint32_t g_u32aScrtKeyParamFmt[] __attribute__((aligned(16))) = 
{
    0x07000100, 
    // Curve_P
    0xffffffff, 0xffffffff, 0xffffffff, 0x00000000, 
    0x00000000, 0x00000000, 0x00000001, 0xffffffff, 

    0x07010100,
    0xfffffffc, 0xffffffff, 0xffffffff, 0x00000000, 
    0x00000000, 0x00000000, 0x00000001, 0xffffffff, 

    0x07020100,
    // Curve_B
    0x27d2604b, 0x3bce3c3e, 0xcc53b0f6, 0x651d06b0, 
    0x769886bc, 0xb3ebbd55, 0xaa3a93e7, 0x5ac635d8, 

    0x07030100,
    // Curve_N
    0xfc632551, 0xf3b9cac2, 0xa7179e84, 0xbce6faad,
    0xffffffff, 0xffffffff, 0x00000000, 0xffffffff,

    0x07040100, 
    // Curve_G1
    0xd898c296, 0xf4a13945, 0x2deb33a0, 0x77037d81, 
    0x63a440f2, 0xf8bce6e5, 0xe12c4247, 0x6b17d1f2, 

    0x07050100,
    // Curve_G2
    0x37bf51f5, 0xcbb64068, 0x6b315ece, 0x2bce3357, 
    0x7c0f9e16, 0x8ee7eb4a, 0xfe1a7f9b, 0x4fe342e2, 

    0x07060001, 
    0x00000001
};


int scrt_status_chk_impl(uint32_t u32Mask, uint32_t u32Value)
{
    int iRet = -1;
    volatile uint32_t *pu32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Cnt = 0;

    while(u32Cnt < SCRT_WAIT_RSP_CNT)
    {
        if((*pu32Status & u32Mask) == u32Value)
        {
            iRet = 0;
            break;
        }

        ++u32Cnt;
    }

    if(u32Cnt > g_u32ScrtWaitRspCnt)
    {
        g_u32ScrtWaitRspCnt = u32Cnt;
    }

    return iRet;
}

int scrt_ecdh_priv_key_asset_create_impl(uint8_t u8Idx, uint32_t *pu32PrivKeyAsId)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint16_t u16TokenId = SCRT_TOKEN_ID_ASSET_CREATE_PRIV_KEY + u8Idx;
    uint32_t u32aOutput[2] = {0};

    // Asset Create (for private key): start
    u32aBase[0] = 0x17000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x00000000;
    u32aBase[3] = 0x00004000;
    u32aBase[4] = 0x00000024;
    u32aBase[5] = 0x00000000;
    u32aBase[6] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Create (for private key): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Create (for private key) output: start
    memcpy(u32aOutput, (void *)u32aBase, 8);

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32aOutput[0] != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_PRIV_KEY[%08X]\n", __func__, __LINE__, u32aOutput[0], u16TokenId);
        goto done;
    }

    *pu32PrivKeyAsId = u32aOutput[1];
    // Asset Create (for private key) output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_ecdh_pub_key_asset_create_impl(uint8_t u8Idx, uint32_t *pu32PubKeyAsId)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint16_t u16TokenId = SCRT_TOKEN_ID_ASSET_CREATE_PUB_KEY + u8Idx;
    uint32_t u32aOutput[2] = {0};

    // Asset Create (for public key): start
    u32aBase[0] = 0x17000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x80000000;
    u32aBase[3] = 0x00004000;
    u32aBase[4] = 0x00000048;
    u32aBase[5] = 0x00000000;
    u32aBase[6] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Create (for public key): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Create (for public key) output: start
    memcpy(u32aOutput, (void *)u32aBase, 8);

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32aOutput[0] != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_PUB_KEY[%08X]\n", __func__, __LINE__, u32aOutput[0], u16TokenId);
        goto done;
    }

    *pu32PubKeyAsId = u32aOutput[1];
    // Asset Create (for public key) output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_ecdh_shared_secret_asset_create_impl(uint8_t u8Idx, uint32_t *pu32SharedSecretAsId)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint16_t u16TokenId = SCRT_TOKEN_ID_ASSET_CREATE_SHARED_SECRET + u8Idx;
    uint32_t u32aOutput[2] = {0};

    // Asset Create (for shared secret): start
    u32aBase[0] = 0x17000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;

    // PUBLIC-DATA
    u32aBase[2] = 0x00000000;
    u32aBase[3] = 0x10000000;

    // AES-CMAC
    //u32aBase[2] = 0x00000120;
    //u32aBase[3] = 0x00000008;

    // SHA-256
    //u32aBase[2] = 0x00000004;
    //u32aBase[3] = 0x00000008;

    u32aBase[4] = 0x00000020;
    u32aBase[5] = 0x00000000;
    u32aBase[6] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Create (for shared secret): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Create (for shared secret) output: start
    memcpy(u32aOutput, (void *)u32aBase, 8);

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32aOutput[0] != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_SHARED_SECRET[%08X]\n", __func__, __LINE__, u32aOutput[0], u16TokenId);
        goto done;
    }

    *pu32SharedSecretAsId = u32aOutput[1];
    // Asset Create (for shared secret) output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_asset_delete_impl(uint8_t u8Idx, uint32_t u32AsId)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint16_t u16TokenId = SCRT_TOKEN_ID_ASSET_DELETE + u8Idx;

    SCRT_LOGI("[%s %d] u32AsId[%#x]\n", __func__, __LINE__, u32AsId);

    if(!u32AsId)
    {
        iRet = 0;
        goto done;
    }

    // Asset Delete: start
    u32aBase[0] = 0x37000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32AsId;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Delete: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Delete output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_ASSET_DELETE[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }
    // Asset Delete output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_asset_load_impl(uint8_t u8Idx, uint32_t u32AsId, uint8_t *u8aAddr, uint16_t u16Len)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint16_t u16TokenId = SCRT_TOKEN_ID_ASSET_LOAD + u8Idx;

    // Asset Load: start
    u32aBase[0] = 0x27000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32AsId;
    u32aBase[3] = 0x08000000 | (u16Len & 0x3FF);
    u32aBase[4] = (uint32_t)u8aAddr;
    u32aBase[5] = 0x00000000;
    u32aBase[6] = 0x00000000;
    u32aBase[7] = 0x00000000;
    u32aBase[8] = 0x00000000;
    u32aBase[9] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Load: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Load output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for u16TokenId[%#x]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }
    // Asset Load output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_asset_get_impl(uint8_t u8Idx, uint32_t u32AsId, uint8_t *u8aBuf, uint32_t u32BufLen, uint32_t u32OutputLen)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint32_t u32OutputTokenId = 0;
    uint16_t u16TokenId = SCRT_TOKEN_ID_PUB_DATA_READ + u8Idx;

    // Public Data Read: start
    u32aBase[0] = 0x47040000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32AsId;
    u32aBase[3] = u32BufLen & 0x000003FF;
    u32aBase[4] = (uint32_t)u8aBuf;
    u32aBase[5] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Public Data Read: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Public Data Read output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_PUB_DATA_READ[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }

    memcpy(&u32OutputTokenId, &(u8aBuf[u32OutputLen]), 4);

    SCRT_LOGI("[%s %d] output token id[%08X] SCRT_TOKEN_ID_PUB_DATA_READ[%08X]\n", __func__, __LINE__, 
              u32OutputTokenId, u16TokenId);

    if(u32OutputTokenId != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_PUB_DATA_READ[%08X]\n", __func__, __LINE__, 
                  u32OutputTokenId, u16TokenId);
        goto done;
    }
    // Public Data Read output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_reset_impl(uint8_t u8Idx)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint16_t u16TokenId = SCRT_TOKEN_ID_RESET + u8Idx;

    // Reset: start
    u32aBase[0] = 0x2F020000 | u16TokenId;
    u32aBase[1] = SCRT_ID;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Reset: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Reset output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_RESET[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }
    // Reset output: end

    iRet = 0;

done:

    return iRet;
}

int scrt_internal_reset_impl(void)
{
    int iRet = -1;
    volatile uint32_t *u32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Mask = 0;
    uint8_t i = 0;

    for(i = 0; i < SCRT_MB_IDX_MAX; i++)
    {
        *u32Status = SCRT_CTRL_LINK_MSK(i);
        u32Mask = SCRT_STATUS_LINK_MSK(i);

        if(scrt_status_chk(u32Mask, SCRT_STATUS_LINK_MSK(i)))
        {
            SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);
        }

        *u32Status = SCRT_CTRL_READ_MSK(i);

        u32Mask = SCRT_STATUS_WRITE_MSK(i) | SCRT_STATUS_READ_MSK (i) | SCRT_STATUS_LINK_MSK(i) | SCRT_STATUS_AVAIL_MSK(i);

        if((*u32Status & u32Mask) == SCRT_STATUS_LINK_MSK(i))
        {
            if(!scrt_reset(i))
            {
                SCRT_LOGI("[%s %d] scrt_reset success, mb_id[%d]\n", __func__, __LINE__, i);
                iRet = 0;
                break;
            }
            else
            {
                SCRT_LOGE("[%s %d] scrt_reset fail, mb_id[%d]\n", __func__, __LINE__, i);
            }
        }
    }

    if(!iRet)
    {
        for(i = 0; i < SCRT_MB_IDX_MAX; i++)
        {
            *u32Status = SCRT_CTRL_UNLINK_MSK(i);
        }
    }
    
    return iRet;
}

int scrt_link_impl(uint8_t u8Idx)
{
    int iRet = -1;
    volatile uint32_t *u32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Mask = SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK (u8Idx) | SCRT_STATUS_LINK_MSK(u8Idx) | SCRT_STATUS_AVAIL_MSK(u8Idx);

    #ifdef SCRT_ENABLE_UNLINK
    if((*u32Status & u32Mask) != SCRT_STATUS_AVAIL_MSK(u8Idx))
    {
        SCRT_LOGE("[%s %d] idx[%d] s[%08X] not ready to link\n", __func__, __LINE__, u8Idx, *u32Status);
        //goto done;
    }
    #endif

    *u32Status = SCRT_CTRL_LINK_MSK(u8Idx);

    if(scrt_status_chk(u32Mask, SCRT_STATUS_LINK_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

#ifdef SCRT_ENABLE_UNLINK
int scrt_unlink_impl(uint8_t u8Idx)
{
    int iRet = -1;
    volatile uint32_t *u32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Mask = SCRT_STATUS_LINK_MSK(u8Idx) | SCRT_STATUS_AVAIL_MSK(u8Idx);

    if((*u32Status & u32Mask) == SCRT_STATUS_AVAIL_MSK(u8Idx))
    {
        SCRT_LOGE("[%s %d] idx[%d] s[%08X] already available\n", __func__, __LINE__, u8Idx, *u32Status);
        iRet = 0;
        goto done;
    }

    *u32Status = SCRT_CTRL_UNLINK_MSK(u8Idx);

    if(scrt_status_chk(u32Mask, SCRT_STATUS_AVAIL_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}
#endif

int scrt_mb_init_impl(void)
{
    int iRet = -1;
    volatile uint32_t *u32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint8_t i = 0;

    for(i = 0; i < SCRT_MB_IDX_MAX; i++)
    {
        uint32_t u32Mask = SCRT_STATUS_WRITE_MSK(i) | SCRT_STATUS_READ_MSK (i) | SCRT_STATUS_LINK_MSK(i) | SCRT_STATUS_AVAIL_MSK(i);

        if((*u32Status & u32Mask) != SCRT_STATUS_AVAIL_MSK(i))
        {
            if(scrt_internal_reset())
            {
                SCRT_LOGE("[%s %d] scrt_internal_reset fail, please reset device\n", __func__, __LINE__);

                #ifdef SCRT_CHECK
                SCRT_ASSERT(0);
                #else
                goto done;
                #endif
            }

            break;
        }
    }

    #ifdef SCRT_PRE_LINK
    for(i = 0; i < SCRT_MB_IDX_MAX; i++)
    {
        if(scrt_link(i))
        {
            SCRT_LOGE("[%s %d] scrt_link fail, please reset device\n", __func__, __LINE__);

            #ifdef SCRT_CHECK
            SCRT_ASSERT(0);
            #else
            goto done;
            #endif
        }
    }
    #endif

    iRet = 0;

#ifdef SCRT_CHECK
#else
done:
#endif
    return iRet;
}

#ifdef SCRT_ACCESS_SEM
int scrt_access_lock_impl(void)
{
    int iRet = -1;
    int iStatus = 0;

    if(!g_tScrtAccessSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }

    iStatus = osSemaphoreWait(g_tScrtAccessSem, SCRT_WAIT_ACCESS_SEM);

    if(iStatus != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail, status[%d]\n", __func__, __LINE__, iStatus);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_access_unlock_impl(void)
{
    int iRet = -1;

    if(!g_tScrtAccessSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }
    
    if(osSemaphoreRelease(g_tScrtAccessSem) != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}
#endif //#ifdef SCRT_ACCESS_SEM

int scrt_res_lock_impl(void)
{
    int iRet = -1;

    if(!g_tScrtResSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }
    
    if(osSemaphoreWait(g_tScrtResSem, SCRT_WAIT_RES_SEM) != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_res_unlock_impl(void)
{
    int iRet = -1;

    if(!g_tScrtResSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }
    
    if(osSemaphoreRelease(g_tScrtResSem) != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_ecdh_lock_impl(void)
{
    int iRet = -1;

    if(!g_tScrtEcdhSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }
    
    if(osSemaphoreWait(g_tScrtEcdhSem, SCRT_WAIT_ECDH_SEM) != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_ecdh_unlock_impl(void)
{
    int iRet = -1;

    if(!g_tScrtEcdhSem)
    {
        SCRT_LOGE("[%s %d] sem is null\n", __func__, __LINE__);
        goto done;
    }
    
    if(osSemaphoreRelease(g_tScrtEcdhSem) != osOK)
    {
        SCRT_LOGE("[%s %d] osSemaphoreWait fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_sem_create_impl(void)
{
    int iRet = -1;
    osSemaphoreDef_t tSemDef = {0};

    #ifdef SCRT_ACCESS_SEM
    if(g_tScrtAccessSem == NULL)
    {
        g_tScrtAccessSem = osSemaphoreCreate(&tSemDef, SCRT_MB_IDX_MAX);
    
        if(g_tScrtAccessSem == NULL)
        {
            SCRT_LOGE("[%s %d] osSemaphoreCreate fail\n", __func__, __LINE__);
        }
    }
    #endif

    if(g_tScrtResSem == NULL)
    {
        g_tScrtResSem = osSemaphoreCreate(&tSemDef, 1);
    
        if(g_tScrtResSem == NULL)
        {
            SCRT_LOGE("[%s %d] osSemaphoreCreate fail\n", __func__, __LINE__);
        }
    }

    if(g_tScrtEcdhSem == NULL)
    {
        g_tScrtEcdhSem = osSemaphoreCreate(&tSemDef, 1);
    
        if(g_tScrtEcdhSem == NULL)
        {
            SCRT_LOGE("[%s %d] osSemaphoreCreate fail\n", __func__, __LINE__);
        }
    }

    iRet = 0;

    return iRet;
}

#ifdef SCRT_ACCESS_SEM
uint8_t scrt_res_alloc_impl(void)
{
    uint8_t u8Idx = SCRT_MB_IDX_MAX;
    uint8_t u8ResDone = 0;
    uint8_t i = 0;

    #ifdef SCRT_ACCESS_SEM
    if(scrt_access_lock())
    {
        SCRT_LOGE("[%s %d] scrt_access_lock fail\n", __func__, __LINE__);
        goto done;
    }
    #endif

    if(scrt_res_lock())
    {
        SCRT_LOGE("[%s %d] scrt_res_lock fail\n", __func__, __LINE__);
        goto done;
    }

    u8ResDone = 1;

    for(i = 0; i < SCRT_MB_IDX_MAX; i++)
    {
        if(g_tScrtRes[i].u8Used == 0)
        {
            g_tScrtRes[i].u8Used = 1;
            u8Idx = i;
            break;
        }
    }

    SCRT_ASSERT(u8Idx <= SCRT_MB_IDX_MAX);

done:
    if(u8ResDone)
    {
        if(scrt_res_unlock())
        {
            SCRT_LOGE("[%s %d] scrt_res_unlock fail\n", __func__, __LINE__);

            SCRT_ASSERT(0);
        }
    }

    return u8Idx;
}
#else
uint8_t scrt_res_alloc_impl(void)
{
    uint8_t u8Idx = SCRT_MB_IDX_MAX;
    uint8_t i = 0;
    uint32_t u32Cnt = 0;

    while(u32Cnt < SCRT_WAIT_RES_MAX_CNT)
    {
        if(scrt_res_lock())
        {
            SCRT_LOGE("[%s %d] scrt_res_lock fail\n", __func__, __LINE__);
            break;
        }

        for(i = 0; i < SCRT_MB_IDX_MAX; i++)
        {
            if(g_tScrtRes[i].u8Used == 0)
            {
                g_tScrtRes[i].u8Used = 1;
                u8Idx = i;
                break;
            }
        }

        if(scrt_res_unlock())
        {
            SCRT_LOGE("[%s %d] scrt_res_unlock fail\n", __func__, __LINE__);
            break;
        }

        if(u8Idx != SCRT_MB_IDX_MAX)
        {
            break;
        }

        ++u32Cnt;
        osDelay(SCRT_WAIT_RES);
    }

    if(u32Cnt >g_u32ScrtWaitResCnt)
    {
        g_u32ScrtWaitResCnt = u32Cnt;
    }

    return u8Idx;
}
#endif //#ifdef SCRT_ACCESS_SEM

void scrt_res_free_impl(uint8_t u8Idx)
{
    //uint8_t u8ResDone = 0;

    if(u8Idx >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGI("[%s %d] invalid res_id[%d]\n", __func__, __LINE__, u8Idx);
        goto done;
    }
    /*
    if(scrt_res_lock())
    {
        SCRT_LOGE("[%s %d] scrt_res_lock fail\n", __func__, __LINE__);
        goto done;
    }
    
    u8ResDone = 1;
    */
    g_tScrtRes[u8Idx].u8Used = 0;

done:
    /*
    if(u8ResDone)
    {
        if(scrt_res_unlock())
        {
            SCRT_LOGE("[%s %d] scrt_res_unlock fail\n", __func__, __LINE__);

            SCRT_ASSERT(0);
        }
    }
    */
    #ifdef SCRT_ACCESS_SEM
    if(scrt_access_unlock())
    {
        SCRT_LOGE("[%s %d] scrt_access_unlock fail\n", __func__, __LINE__);

        SCRT_ASSERT(0);
    }
    #endif

    return;
}

int scrt_trng_init_impl(uint8_t u8Idx)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint16_t u16TokenId = 0;

    SCRT_LOGI("[%s %d] u32aBase at[%08X]\n", __func__, __LINE__, u32aBase);

    if(!nl_scrt_otp_status_get())
    {
        SCRT_LOGE("[%s %d] OTP not ready\n", __func__, __LINE__);
        goto done;
    }

    u16TokenId = SCRT_TOKEN_ID_TRNG_CFG + u8Idx;

    // TRNG configure: start
    u32aBase[0] = 0x14000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x00000001;
    u32aBase[3] = 0x00030101;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // TRNG configure: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // TRNG configure output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_TRNG_CFG[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }
    // TRNG configure output: end

    iRet = 0;

done:
    return iRet;
}

int scrt_ecdh_key_param_init_impl(uint8_t u8Idx, uint32_t *pu32KeyParamAsId)
{
    int iRet = -1;
    volatile uint32_t *u32aBase = (uint32_t *)SCRT_BASE_ADDR(u8Idx);
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32aOutput[2] = {0};
    uint16_t u16TokenId = 0;

    SCRT_LOGI("[%s %d] u32aBase at[%08X]\n", __func__, __LINE__, u32aBase);

    u16TokenId = SCRT_TOKEN_ID_ASSET_CREATE_KEY_PARAM + u8Idx;

    // Asset Create (for key param): start
    u32aBase[0] = 0x17000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x00000000;
    u32aBase[3] = 0x00008000;
    u32aBase[4] = 0x000000E0;
    u32aBase[5] = 0x00000000;
    u32aBase[6] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(u8Idx);
    // Asset Create (for key param): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(u8Idx) | SCRT_STATUS_READ_MSK(u8Idx), 
                       SCRT_STATUS_READ_MSK(u8Idx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Asset Create (for key param) output: start
    memcpy(u32aOutput, (void *)u32aBase, 8);

    *u32aStatus = SCRT_CTRL_READ_MSK(u8Idx);

    if(u32aOutput[0] != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_PUB_KEY_PARAM\n", __func__, __LINE__, u32aOutput[0], u16TokenId);
        goto done;
    }

    *pu32KeyParamAsId = u32aOutput[1];
    // Asset Create (for key param) output: end

    SCRT_LOGI("[%s %d] load key param\n", __func__, __LINE__);
    
    if(scrt_asset_load(u8Idx, *pu32KeyParamAsId, (uint8_t *)g_u32aScrtKeyParamFmt, SCRT_KEY_PARAM_FMT_LEN))
    {
        SCRT_LOGE("[%s %d] scrt_asset_load fail for key param\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_eng_init_impl(uint8_t u8Idx)
{
    int iRet = -1;

    if(scrt_trng_init(u8Idx))
    {
        SCRT_LOGE("[%s %d] scrt_trng_init fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int scrt_param_init_impl(void)
{
    uint32_t i = 0;

    // init scrt resource
    for(i = 0; i < SCRT_MB_IDX_MAX; i++)
    {
        g_tScrtRes[i].u8Used = 0;
        g_tScrtRes[i].u8MbIdx = i;
    }

    //g_u8ScrtOtpReady = 0;
    //g_tScrtResSem = NULL;

    #ifdef SCRT_ACCESS_SEM
    //g_tScrtAccessSem = NULL;
    #endif
    
    //g_u32ScrtKeyParamAsId = 0;

    g_u32ScrtWaitResCnt = 0;
    g_u32ScrtWaitRspCnt = 0;
    return 0;
}

/*
 * nl_scrt_otp_status_get_impl - Get OTP status.
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
int nl_scrt_otp_status_get_impl(void)
{
    int iRet = 0;
    volatile uint32_t *pu32OtpStatus = (uint32_t *)SCRT_OTP_STATUS_ADDR;

    if(*pu32OtpStatus == 0)
    {
        SCRT_LOGI("[%s %d] OTP not ready\n", __func__, __LINE__);

        goto done;
    }

    iRet = 1;

done:
    return iRet;
}

/*
 * nl_scrt_ecdh_key_pair_gen_impl - Generate ECDH key pair.
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
int nl_scrt_ecdh_key_pair_gen_impl(void *pPubKey, uint32_t *u32aPrivKey, uint32_t *pu32PrivKeyId)
{
    int iRet = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint32_t u32OutputTokenId = 0;
    uint8_t *u8aPubKey = (uint8_t *)pPubKey;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint32_t u32KeyParamAsId = 0;
    uint32_t u32PrivKeyAsId = 0;
    uint32_t u32PubKeyAsId = 0;
    uint16_t u16TokenId = 0;
    uint32_t u32OutputBufLen = 0;
    uint8_t *u8aOutputBuf = NULL;
    uint8_t *u8aAlignBuf = NULL;
    uint8_t u8EcdhLock = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if(!nl_scrt_otp_status_get())
    {
        SCRT_LOGE("[%s %d] OTP not ready\n", __func__, __LINE__);
        goto done;
    }

    if((pPubKey == NULL) || (pu32PrivKeyId == NULL))
    {
        goto done;
    }

    if(scrt_ecdh_lock())
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_lock fail\n", __func__, __LINE__);
        goto done;
    }

    u8EcdhLock = 1;

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    SCRT_LOGI("[%s %d] u8ResId[%d]\n", __func__, __LINE__, u8ResId);

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    if(scrt_eng_init(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_eng_init fail\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_ecdh_key_param_init(g_tScrtRes[u8ResId].u8MbIdx, &u32KeyParamAsId))
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_key_param_init fail\n", __func__, __LINE__);
        goto done;
    }

    if(*pu32PrivKeyId != 0)
    {
        if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, *pu32PrivKeyId))
        {
            SCRT_LOGE("[%s %d] scrt_asset_delete fail for private key\n", __func__, __LINE__);
        }
    }

    if(scrt_ecdh_priv_key_asset_create(g_tScrtRes[u8ResId].u8MbIdx, &u32PrivKeyAsId))
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_priv_key_asset_create fail\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_ecdh_pub_key_asset_create(g_tScrtRes[u8ResId].u8MbIdx, &u32PubKeyAsId))
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_pub_key_asset_create fail\n", __func__, __LINE__);
        goto done;
    }

    u16TokenId = SCRT_TOKEN_ID_KEY_PAIR_GEN + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    SCRT_LOGI("[%s %d] u32aBase at[%08X]\n", __func__, __LINE__, u32aBase);

    u32OutputBufLen = SCRT_PUB_KEY_FMT_LEN + 4 + 16;
    u8aOutputBuf = (uint8_t *)SCRT_MALLOC(u32OutputBufLen);

    if(!u8aOutputBuf)
    {
        SCRT_LOGE("[%s %d] SCRT_MALLOC(%u) fail\n", __func__, __LINE__, u32OutputBufLen);
        goto done;
    }

    u8aAlignBuf = (uint8_t *)IPC_ADDR_ALIGN(u8aOutputBuf, 16);
    
    // Public Key Token (ECDH/ECDSA/ElGamal ECC generate private and public key): start
    u32aBase[0] = 0x19040000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x08080015;
    u32aBase[3] = 0x00000000;
    u32aBase[4] = u32PrivKeyAsId;
    u32aBase[5] = u32KeyParamAsId;
    u32aBase[6] = u32PubKeyAsId;
    u32aBase[7] = (SCRT_PUB_KEY_FMT_LEN & 0x0FFF) << 16;
    u32aBase[8] = 0x00000000;
    u32aBase[9] = 0x00000000;
    u32aBase[10] = (uint32_t)u8aAlignBuf;
    u32aBase[11] = 0x00000000;
    u32aBase[12] = 0x00000000;

    SCRT_LOGI("[%s %d] param[%08X] priv[%08X] pub[%08X]\n", __func__, __LINE__, 
              u32KeyParamAsId, u32PrivKeyAsId, u32PubKeyAsId);

    *u32aStatus = SCRT_CTRL_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    // Public Key Token (ECDH/ECDSA/ElGamal ECC generate private and public key): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx) | SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx), 
                       SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Public Key Token output (ECDH/ECDSA/ElGamal ECC generate private and public key): start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_KEY_PAIR_GEN[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }

    memcpy(&u32OutputTokenId, &(u8aAlignBuf[SCRT_PUB_KEY_FMT_LEN]), 4);

    SCRT_LOGI("[%s %d] output token id[%08X] SCRT_TOKEN_ID_KEY_PAIR_GEN[%08X]\n", __func__, __LINE__, 
              u32OutputTokenId, u16TokenId);

    if(u32OutputTokenId != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_KEY_PAIR_GEN[%08X]\n", __func__, __LINE__, 
                  u32OutputTokenId, u16TokenId);
        goto done;
    }
    // Public Key Token output (ECDH/ECDSA/ElGamal ECC generate private and public key): end

    memcpy(&(u8aPubKey[0]), &(u8aAlignBuf[4]), SCRT_PUB_KEY_VEC_LEN);
    memcpy(&(u8aPubKey[SCRT_PUB_KEY_VEC_LEN]), &(u8aAlignBuf[40]), SCRT_PUB_KEY_VEC_LEN);

    *pu32PrivKeyId = u32PrivKeyAsId;

    iRet = 1;

done:
    if(u8aOutputBuf)
    {
        SCRT_FREE(u8aOutputBuf);
    }

    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        if(u32KeyParamAsId)
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32KeyParamAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for key param\n", __func__, __LINE__);
            }
        }
        
        if(u32PubKeyAsId)
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32PubKeyAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for public key\n", __func__, __LINE__);
            }
        }

        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    if(u8EcdhLock)
    {
        if(scrt_ecdh_unlock())
        {
            SCRT_LOGE("[%s %d] scrt_ecdh_unlock fail\n", __func__, __LINE__);
        }
    }

    return iRet;
}

/*
 * nl_scrt_ecdh_dhkey_gen_impl - Generate ECDH shared secret (DHKey).
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
int nl_scrt_ecdh_dhkey_gen_impl(uint8_t *u8aPubKeyX, uint8_t *u8aPubKeyY, uint32_t *u32aPrivKey, void *pDhKey, uint32_t u32PrivKeyId)
{
    int iRet = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint8_t *u8aDhKey = (uint8_t *)pDhKey;
    uint32_t i = 0;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint32_t u32KeyParamAsId = 0;
    uint32_t u32PrivKeyAsId = 0;
    uint32_t u32PubKeyAsId = 0;
    uint32_t u32SharedSecretAsId = 0;
    uint16_t u16TokenId = 0;
    uint32_t u32OutputBufLen = 0;
    uint8_t *u8aOutputBuf = NULL;
    uint8_t *u8aAlignBuf = NULL;
    uint8_t u8EcdhLock = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if(!nl_scrt_otp_status_get())
    {
        SCRT_LOGE("[%s %d] OTP not ready\n", __func__, __LINE__);
        goto done;
    }

    if((u8aPubKeyX == NULL) || (u8aPubKeyY == NULL) || 
       (pDhKey == NULL))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_ecdh_lock())
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_lock fail\n", __func__, __LINE__);
        goto done;
    }

    u8EcdhLock = 1;

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    SCRT_LOGI("[%s %d] u8ResId[%d]\n", __func__, __LINE__, u8ResId);

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    if(scrt_eng_init(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_eng_init fail\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_ecdh_key_param_init(g_tScrtRes[u8ResId].u8MbIdx, &u32KeyParamAsId))
    {
        SCRT_LOGE("[%s %d] scrt_ecdh_key_param_init fail\n", __func__, __LINE__);
        goto done;
    }

    u32OutputBufLen = SCRT_PUB_KEY_FMT_LEN + 16; // >= SCRT_SHARED_SRCRET_LEN + 4 + 16;
    u8aOutputBuf = (uint8_t *)SCRT_MALLOC(u32OutputBufLen);

    if(!u8aOutputBuf)
    {
        SCRT_LOGE("[%s %d] SCRT_MALLOC(%u) fail\n", __func__, __LINE__, u32OutputBufLen);
        goto done;
    }

    u8aAlignBuf = (uint8_t *)IPC_ADDR_ALIGN(u8aOutputBuf, 16);

    if(u32PrivKeyId) // use previous private key
    {
        u32PrivKeyAsId = u32PrivKeyId;

        SCRT_LOGI("[%s %d] use previous private key[%08X]\n", __func__, __LINE__, u32PrivKeyAsId);
    }
    else // create and load private key
    {
        if(u32aPrivKey == NULL)
        {
            SCRT_LOGE("[%s %d] invalid private key\n", __func__, __LINE__);
            goto done;
        }

        if(scrt_ecdh_priv_key_asset_create(g_tScrtRes[u8ResId].u8MbIdx, &u32PrivKeyAsId))
        {
            SCRT_LOGE("[%s %d] scrt_ecdh_priv_key_asset_create fail\n", __func__, __LINE__);
            goto done;
        }

        SCRT_LOGI("[%s %d] create private key[%08X]\n", __func__, __LINE__, u32PrivKeyAsId);

        u8aAlignBuf[0] = 0x00;
        u8aAlignBuf[1] = 0x01;
        u8aAlignBuf[2] = 0x00;
        u8aAlignBuf[3] = 0x01;

        memcpy(&(u8aAlignBuf[4]), u32aPrivKey, SCRT_PRIV_KEY_LEN);

        SCRT_LOGI("[%s %d] load private key[%08X]\n", __func__, __LINE__, u32PrivKeyAsId);
    
        if(scrt_asset_load(g_tScrtRes[u8ResId].u8MbIdx, u32PrivKeyAsId, u8aAlignBuf, SCRT_PRIV_KEY_FMT_LEN))
        {
            SCRT_LOGE("[%s %d] scrt_asset_load fail for private key\n", __func__, __LINE__);
            goto done;
        }
    }

    SCRT_LOGI("[%s %d] u32PrivKeyAsId[%08X]\n", __func__, __LINE__, u32PrivKeyAsId);

    if(scrt_ecdh_pub_key_asset_create(g_tScrtRes[u8ResId].u8MbIdx, &u32PubKeyAsId))
    {
        SCRT_LOGE("[%s %d] scrt_asset_load fail for private key\n", __func__, __LINE__);
        goto done;
    }
    
    // 0x02000100
    u8aAlignBuf[0] = 0x00;
    u8aAlignBuf[1] = 0x01;
    u8aAlignBuf[2] = 0x00;
    u8aAlignBuf[3] = 0x02;
    
    memcpy(&(u8aAlignBuf[4]), u8aPubKeyX, SCRT_PUB_KEY_VEC_LEN);

    // 0x02010100
    u8aAlignBuf[36] = 0x00;
    u8aAlignBuf[37] = 0x01;
    u8aAlignBuf[38] = 0x01;
    u8aAlignBuf[39] = 0x02;
    
    memcpy(&(u8aAlignBuf[40]), u8aPubKeyY, SCRT_PUB_KEY_VEC_LEN);

    SCRT_LOGI("[%s %d] load public key\n", __func__, __LINE__);

    if(scrt_asset_load(g_tScrtRes[u8ResId].u8MbIdx, u32PubKeyAsId, u8aAlignBuf, SCRT_PUB_KEY_FMT_LEN))
    {
        SCRT_LOGE("[%s %d] scrt_asset_load fail for public key\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_ecdh_shared_secret_asset_create(g_tScrtRes[u8ResId].u8MbIdx, &u32SharedSecretAsId))
    {
        SCRT_LOGE("[%s %d] scrt_asset_load fail for private key\n", __func__, __LINE__);
        goto done;
    }

    u16TokenId = SCRT_TOKEN_ID_SHARED_SECRET_GEN + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    SCRT_LOGI("[%s %d] u32aBase at[%08X]\n", __func__, __LINE__, u32aBase);

    // Public Key Token (ECDH generate shared secrets (single key-pair)): start
    u32aBase[0] = 0x19000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = 0x00080016;
    u32aBase[3] = 0x80000100;
    u32aBase[4] = u32PrivKeyAsId;
    u32aBase[5] = u32KeyParamAsId;
    u32aBase[6] = u32PubKeyAsId;
    u32aBase[7] = 0x00000000;
    u32aBase[8] = 0x00000000;
    u32aBase[9] = 0x00000000;
    u32aBase[10] = 0x00000000;
    u32aBase[11] = 0x00000000;
    u32aBase[12] = u32SharedSecretAsId;

    SCRT_LOGI("[%s %d] param[%08X] priv[%08X] pub[%08X] ss[%08X]\n", __func__, __LINE__, 
              u32KeyParamAsId, u32PrivKeyAsId, 
              u32PubKeyAsId, u32SharedSecretAsId);

    *u32aStatus = SCRT_CTRL_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    // Public Key Token (ECDH generate shared secrets (single key-pair)): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx) | SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx), 
                       SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // Public Key Token (ECDH generate shared secrets (single key-pair)) output: start
    u32Output = u32aBase[0];

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] invalid output[%08X] for SCRT_TOKEN_ID_SHARED_SECRET[%08X]\n", __func__, __LINE__, u32Output, u16TokenId);
        goto done;
    }
    // Public Key Token (ECDH generate shared secrets (single key-pair)) output: end
    
    if(scrt_asset_get(g_tScrtRes[u8ResId].u8MbIdx, u32SharedSecretAsId, u8aAlignBuf, SCRT_SHARED_SRCRET_LEN + 4, SCRT_SHARED_SRCRET_LEN))
    {
        SCRT_LOGE("[%s %d] scrt_asset_get fail for shared secret\n", __func__, __LINE__);
        goto done;
    }

    for(i = 0; i < SCRT_SHARED_SRCRET_LEN; i++)
    {
        u8aDhKey[i] = u8aAlignBuf[SCRT_SHARED_SRCRET_LEN - 1 - i];
    }

    iRet = 1;

done:
    if(u8aOutputBuf)
    {
        SCRT_FREE(u8aOutputBuf);
    }

    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        if(u32KeyParamAsId)
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32KeyParamAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for key param\n", __func__, __LINE__);
            }
        }

        if((u32PrivKeyAsId) && (u32PrivKeyAsId != u32PrivKeyId))
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32PrivKeyAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for private key\n", __func__, __LINE__);
            }
        }

        if(u32PubKeyAsId)
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32PubKeyAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for public key\n", __func__, __LINE__);
            }
        }

        if(u32SharedSecretAsId)
        {
            if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32SharedSecretAsId))
            {
                SCRT_LOGE("[%s %d] scrt_asset_delete fail for shared secret\n", __func__, __LINE__);
            }
        }

        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    if(u8EcdhLock)
    {
        if(scrt_ecdh_unlock())
        {
            SCRT_LOGE("[%s %d] scrt_ecdh_unlock fail\n", __func__, __LINE__);
        }
    }

    return iRet;
}

/*
 * nl_scrt_key_delete_impl - Delete key.
 *
 * @param [in] u32KeyId
 *    [in]: ID of key
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_key_delete_impl(uint32_t u32KeyId)
{
    int iRet = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if(!u32KeyId)
    {
        SCRT_LOGE("[%s %d] invalid id\n", __func__, __LINE__);
        goto done;
    }

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    if(scrt_asset_delete(g_tScrtRes[u8ResId].u8MbIdx, u32KeyId))
    {
        SCRT_LOGE("[%s %d] scrt_asset_delete fail\n", __func__, __LINE__);
    }

    iRet = 1;

done:

    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    return iRet;
}

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
int nl_scrt_init_patch(void)
{
    int iRet = 0;

    if(scrt_param_init())
    {
        SCRT_LOGE("[%s %d] scrt_param_init fail\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_sem_create())
    {
        SCRT_LOGE("[%s %d] scrt_sem_create fail\n", __func__, __LINE__);
        goto done;
    }

    if(scrt_mb_init())
    {
        SCRT_LOGE("[%s %d] scrt_mb_init fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 1;

done:
    return iRet;
}

/*
 * nl_scrt_aes_ccm_patch - Perform the AES CCM Operation.
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
int nl_scrt_aes_ccm_patch(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *nonce, int nonce_len, unsigned char *adata, int adata_len, unsigned char *plain_text, unsigned char *encrypted_text, int text_len, unsigned char *tag, int tag_len)
{
    int status = 1;
    unsigned int word_11 = 0;
    int buf_len = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t *u8aInput = NULL;
    uint8_t *u8aOutput = NULL;
    uint8_t u8NeedToClr = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif
    
    if((!sk) || (!sk_len) || 
       (!nonce) || (!nonce_len) || 
       (!adata) || (!adata_len) || 
       (!plain_text) || (!encrypted_text) || (!text_len) || 
       (!tag) || (!tag_len))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    //SCRT_LOGI("[%s %d] u8ResId[%d]\n", __func__, __LINE__, u8ResId);

    #if 0 // Todo
    {
        uint32_t *pu32Reg = (uint32_t *)0x40001134;
        uint32_t u32Value = *pu32Reg;

        *pu32Reg = u32Value | (0x01000000);
    }
    #endif

    buf_len = text_len & (~0x0F);

    if(text_len & 0x0F)
    {
        buf_len += 16;
    }

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    if(bEncrypt)
    {
        u16TokenId = SCRT_TOKEN_ID_AES_CCM_ENCRYPT + g_tScrtRes[u8ResId].u8MbIdx;
        u8aInput = plain_text;
        u8aOutput = encrypted_text;
    }
    else
    {
        u16TokenId = SCRT_TOKEN_ID_AES_CCM_DECRYPT + g_tScrtRes[u8ResId].u8MbIdx;
        u8aInput = encrypted_text;
        u8aOutput = plain_text;
    }

    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    // Encryption/Decryption (AES-CCM): start
    u32aBase[0] = 0x01040000 | u16TokenId;
    //u32aBase[0] = 0x01000000 | u16TokenId;
    u32aBase[1] = 0x00000000;
    u32aBase[2] = text_len;                                 //The length of the data transfer
    u32aBase[3] = (uint32_t)u8aInput;                       //Input Data
    u32aBase[4] = 0x00000000;                               //Input Data
    u32aBase[5] = buf_len;                                  //Input Data Length
    u32aBase[6] = (uint32_t)u8aOutput;                      //Output Data
    u32aBase[7] = 0x00000000;                               //Output Data
    u32aBase[8] = (buf_len + 4);                            //Output Data Length
    //u32aBase[8] = buf_len;                            //Output Data Length
    u32aBase[9] = (uint32_t)adata;                          //Associated Data
    u32aBase[10] = 0x00000000;                              //Associated Data
    //u32aBase[11] = 0x10d18050;                                                //0x10d18050;           //Nonce length               => word 11[23:20]  => 13  => 0xd
                                                                                //Key length             => word 11[19:16]           => 1 (1 means 128 bits key)
                                                                                //Tag length             => word 11[28:24] 5 bits  => 0x10  => 16
                                                                                //Encrypt or Decrypt  => word11[15]                => 1:encrypt 0:decrypt

    if (bEncrypt == 1) {
        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x80) << 8) | ((0x50) << 0);
    } else {
        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x0) << 8) | ((0x50) << 0);
    }

    //printf("\r\n text_len:%d word_11:%08x \r\n", text_len, word_11);

    u32aBase[11] = word_11;
    u32aBase[12] = 0x00000000;
    u32aBase[13] = 0x00000000;
    u32aBase[14] = 0x00000000;
    u32aBase[15] = 0x00000000;
    u32aBase[16] = 0x00000000;

    memcpy((void *)&(u32aBase[17]), sk, sk_len);

    u32aBase[21] = 0x00000000;          //key
    u32aBase[22] = 0x00000000;          //key
    u32aBase[23] = 0x00000000;          //key
    u32aBase[24] = 0x00000000;          //key
    u32aBase[25] = adata_len;           //Associated Data Length
    u32aBase[26] = 0x00000000;
    u32aBase[27] = 0x00000000;
    u32aBase[28] = 0x00000000;

    memcpy((void *)&(u32aBase[29]), nonce, nonce_len);

    if (bEncrypt == 1) {
        u32aBase[33] = 0x00000000;
        u32aBase[34] = 0x00000000;
        u32aBase[35] = 0x00000000;
        u32aBase[36] = 0x00000000;
    } else {
        memcpy((void *)&(u32aBase[33]), tag, tag_len);
    }

    *u32aStatus = SCRT_CTRL_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    // Encryption/Decryption (AES-CCM): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx) | SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx), 
                       SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    u8NeedToClr = 1;

    // Encryption/Decryption (AES-CCM) output: start
    u32Output = u32aBase[0];

    if(u32Output != u16TokenId)
    {
        SCRT_LOGI("[%s %d] enc[%d] invalid output[%08X] for SCRT_TOKEN_ID_AES_CCM[%08X]\n", __func__, __LINE__, bEncrypt, u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        if(bEncrypt)
        {
            SCRT_ASSERT(0);
        }
        #endif
        
        goto done;
    }

    u32Output = *((volatile unsigned int *)(u8aOutput + buf_len));

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_AES_CCM[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    if (bEncrypt == 1) {
        /* Copy the output tag data to the buffer "tag" */
        memcpy((void *)tag, (void *)&(u32aBase[6]), tag_len);
    }

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    u8NeedToClr = 0;
    // Encryption/Decryption (AES-CCM) output: end

    status = 1;

done:
    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        if(u8NeedToClr)
        {
            *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
        }

        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    return status;
}

/*
 * nl_aes_ccm_patch  - AES CCM Operation. [deprecated]
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
int nl_aes_ccm_patch(bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len)
{
    int iRet = 0;
    
    if(!nl_scrt_aes_ccm((bEncrypt)?(1):(0), sk, sk_len, nonce, nonce_len, (unsigned char *)adata, adata_len, 
                        (unsigned char *)plain_text, (unsigned char *)encrypted_text, text_len, tag, tag_len))
    {
        goto done;
    }

    iRet = 1;

done:
    return iRet;
}

#if 1
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
int nl_hmac_sha_1_patch(uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac)
{
    int status = 0;
    unsigned int word_6 = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t u8NeedToClr = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if((!sk) || (!sk_len) || 
       (!in_data) || (!in_data_len) || 
       (!mac))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    u16TokenId = SCRT_TOKEN_ID_HMAC_SHA_1 + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    // MAC: start
    u32aBase[0] = 0x03000000 | u16TokenId;
    u32aBase[1] = 0x00000000;
    u32aBase[2] = in_data_len; //The length of the input data (in bytes).
    u32aBase[3] = (uint32_t)in_data;
    u32aBase[4] = 0x00000000;
    u32aBase[5] = in_data_len;

    //*(volatile unsigned int *)0x60000018 = 0x00200001;
    word_6 = ((sk_len & 0xff) << 16) | ((0x1) << 0);
    u32aBase[6] = word_6;      //key length: 0x08  ;  [3:0] Algorithm  HMAC-SHA-1, 160-bit MAC, block size is 64 Bytes

    u32aBase[7] = 0x00000000;
    u32aBase[8] = 0x00000000;
    u32aBase[9] = 0x00000000;
    u32aBase[10] = 0x00000000;
    u32aBase[11] = 0x00000000;
    u32aBase[12] = 0x00000000;
    u32aBase[13] = 0x00000000;
    u32aBase[14] = 0x00000000;
    u32aBase[15] = 0x00000000;
    u32aBase[16] = 0x00000000;
    u32aBase[17] = 0x00000000;
    u32aBase[18] = 0x00000000;
    u32aBase[19] = 0x00000000;
    u32aBase[20] = 0x00000000;
    u32aBase[21] = 0x00000000;
    u32aBase[22] = 0x00000000;
    u32aBase[23] = 0x00000000;

    //word 24
    u32aBase[24] = in_data_len;

    u32aBase[25] = 0x00000000;
    u32aBase[26] = 0x00000000;
    u32aBase[27] = 0x00000000;

    //msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, in_data_len:%d word_6:%08x \r\n", in_data_len, word_6);

    /* Key */
    memcpy((void *)&(u32aBase[28]), sk, sk_len);

    /* Write a word */
    *u32aStatus = SCRT_CTRL_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    // MAC: end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx) | SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx), 
                       SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    u8NeedToClr = 1;

    // MAC output: start
    u32Output = u32aBase[0];

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_HMAC_SHA_1[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    /* Copy the output MAC data */
    memcpy((void *)mac, (void *)&(u32aBase[2]), 20);

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    u8NeedToClr = 0;
    // MAC output: end

    status = 1;

done:
    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        if(u8NeedToClr)
        {
            *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
        }

        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    return status;
}

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
int nl_scrt_aes_ecb_patch(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len)
{
    int status = 1;
    unsigned int word_11 = 0;
    int buf_len = 0;
    int key_len = 0;
    int fieldEncrypt = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t u8NeedToClr = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if((!sk) || (!sk_len) || (!data_in) || (!data_out) || (!data_len))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    if(bEncrypt)
    {
        fieldEncrypt = 1;
    }

    switch(sk_len)
    {
        case 16:
            key_len = 1;
            break;
        case 24:
            key_len = 2;
            break;
        case 32:
            key_len = 3;
            break;
        default:
            SCRT_LOGE("[%s %d] invalid sk_len[%d]\n", __func__, __LINE__, sk_len);
            goto done;
    }

    u8ResId = scrt_res_alloc();

    if(u8ResId >= SCRT_MB_IDX_MAX)
    {
        SCRT_LOGE("[%s %d] scrt_res_alloc fail\n", __func__, __LINE__);
        goto done;
    }

    buf_len = data_len & (~0x0F);

    if(data_len & 0x0F)
    {
        buf_len += 16;
    }

    #ifdef SCRT_PRE_LINK
    #else
    // Link: start
    if(scrt_link(g_tScrtRes[u8ResId].u8MbIdx))
    {
        SCRT_LOGE("[%s %d] scrt_link fail\n", __func__, __LINE__);
        goto done;
    }

    #ifdef SCRT_ENABLE_UNLINK
    u8Link = 1;
    #endif
    // Link: end
    #endif

    u16TokenId = SCRT_TOKEN_ID_AES_ECB + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    // Encryption/Decryption (AES-ECB): start
    u32aBase[0] = 0x01040000 | u16TokenId;
    u32aBase[1] = 0x00000000;
    u32aBase[2] = buf_len;
    u32aBase[3] = (uint32_t)data_in;
    u32aBase[4] = 0x00000000;
    u32aBase[5] = buf_len;
    u32aBase[6] = (uint32_t)data_out;
    u32aBase[7] = 0x00000000;
    u32aBase[8] = buf_len + 4;
    u32aBase[9] = 0x00000000;
    u32aBase[10] = 0x00000000;

    word_11 = ((key_len << 16) | (fieldEncrypt << 15));
    u32aBase[11] = word_11;

    u32aBase[12] = 0x00000000;
    u32aBase[13] = 0x00000000;
    u32aBase[14] = 0x00000000;
    u32aBase[15] = 0x00000000;
    u32aBase[16] = 0x00000000;

    memcpy((void *)&(u32aBase[17]), (void *)sk, 16);

    u32aBase[21] = 0x00000000;
    u32aBase[22] = 0x00000000;
    u32aBase[23] = 0x00000000;
    u32aBase[24] = 0x00000000;

    *u32aStatus = SCRT_CTRL_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    // Encryption/Decryption (AES-ECB): end

    // write done and ready to read
    if(scrt_status_chk(SCRT_STATUS_WRITE_MSK(g_tScrtRes[u8ResId].u8MbIdx) | SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx), 
                       SCRT_STATUS_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx)))
    {
        SCRT_LOGE("[%s %d] scrt_status_chk fail\n", __func__, __LINE__);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    u8NeedToClr = 1;

    // Encryption/Decryption (AES-ECB) output: start
    u32Output = u32aBase[0];

    if(u32Output != u16TokenId)
    {
        SCRT_LOGI("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_AES_ECB[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        if(bEncrypt)
        {
            SCRT_ASSERT(0);
        }
        #endif
        
        goto done;
    }

    u32Output = *((volatile unsigned int *)(data_out + buf_len));

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_AES_ECB[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    u8NeedToClr = 0;
    // Encryption/Decryption (AES-ECB) output: end

    status = 1;

done:
    if(u8ResId < SCRT_MB_IDX_MAX)
    {
        if(u8NeedToClr)
        {
            *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
        }

        #ifdef SCRT_ENABLE_UNLINK
        // Unlink: start
        if(u8Link)
        {
            if(scrt_unlink(g_tScrtRes[u8ResId].u8MbIdx))
            {
                SCRT_LOGE("[%s %d] scrt_unlink fail\n", __func__, __LINE__);
            }
        }
        // Unlink: end
        #endif
    
        // free resource
        scrt_res_free(u8ResId);
    }

    return status;
}

/*
 * scrt_drv_func_init - Interface Initialization: SCRT
 *
 */
void scrt_drv_func_init_patch(void)
{
    // internal
    scrt_param_init = scrt_param_init_impl;
    scrt_mb_init = scrt_mb_init_impl;
    scrt_trng_init = scrt_trng_init_impl;
    scrt_eng_init = scrt_eng_init_impl;
    scrt_status_chk = scrt_status_chk_impl;
    scrt_ecdh_key_param_init = scrt_ecdh_key_param_init_impl;
    scrt_ecdh_priv_key_asset_create = scrt_ecdh_priv_key_asset_create_impl;
    scrt_ecdh_pub_key_asset_create = scrt_ecdh_pub_key_asset_create_impl;
    scrt_ecdh_shared_secret_asset_create = scrt_ecdh_shared_secret_asset_create_impl;
    scrt_asset_delete = scrt_asset_delete_impl;
    scrt_asset_load = scrt_asset_load_impl;
    scrt_asset_get = scrt_asset_get_impl;
    scrt_reset = scrt_reset_impl;
    scrt_internal_reset = scrt_internal_reset_impl;
    scrt_link = scrt_link_impl;

    #ifdef SCRT_ENABLE_UNLINK
    scrt_unlink = scrt_unlink_impl;
    #endif

    #ifdef SCRT_ACCESS_SEM
    scrt_access_lock = scrt_access_lock_impl;
    scrt_access_unlock = scrt_access_unlock_impl;
    #endif

    scrt_res_lock = scrt_res_lock_impl;
    scrt_res_unlock = scrt_res_unlock_impl;
    scrt_ecdh_lock = scrt_ecdh_lock_impl;
    scrt_ecdh_unlock = scrt_ecdh_unlock_impl;
    scrt_sem_create = scrt_sem_create_impl;
    scrt_res_alloc = scrt_res_alloc_impl;
    scrt_res_free = scrt_res_free_impl;

    // external
    nl_scrt_Init = nl_scrt_init_patch;
    nl_aes_ccm = nl_aes_ccm_patch;
    nl_scrt_aes_ccm = nl_scrt_aes_ccm_patch;
    nl_hmac_sha_1 = nl_hmac_sha_1_patch;
    nl_scrt_aes_ecb = nl_scrt_aes_ecb_patch;
    nl_scrt_otp_status_get = nl_scrt_otp_status_get_impl;
    nl_scrt_ecdh_key_pair_gen = nl_scrt_ecdh_key_pair_gen_impl;
    nl_scrt_ecdh_dhkey_gen = nl_scrt_ecdh_dhkey_gen_impl;
    nl_scrt_key_delete = nl_scrt_key_delete_impl;

    #ifdef SCRT_CMD
    nl_scrt_cmd_func_init();
    #endif
    
    return;
}

#else

/*
 * delay_ - Perform a delay.
 *
 * @param [in] times  The times to do __NOP
 *
 */
void delay_(int times){
    int i;
    for(i=0; i<times; i++){
        __NOP();
    }
}

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
int nl_hmac_sha_1_patch(uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac)
{
    unsigned int data                   = 0;
    unsigned int c_output_token_id      = 0x9d37;
    unsigned int r_output_token_data    = 0;
    bool status = true;
    unsigned int word_6;
    int i;
    int cnt = 0;

    /*
        SCRT Control- Link Mailbox
      */
    *(volatile unsigned int *)0x60003f00 = 0x4;

    /*
        SCRT Control- Check Mailbox Status
      */
    while(1){
        delay_(1);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    /* Write Token to mailbox */
    *(volatile unsigned int *)0x60000000 = 0x03009d37;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = in_data_len; //The length of the input data (in bytes).
    *(volatile unsigned int *)0x6000000c = (unsigned int) in_data;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = in_data_len;

    //*(volatile unsigned int *)0x60000018 = 0x00200001;
    word_6 = ((sk_len & 0xff) << 16) | ((0x1) << 0);
    *(volatile unsigned int *)0x60000018 = word_6;      //key length: 0x08  ;  [3:0] Algorithm  HMAC-SHA-1, 160-bit MAC, block size is 64 Bytes

    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = 0x00000000;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;
    *(volatile unsigned int *)0x6000002c = 0x00000000;
    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;
    *(volatile unsigned int *)0x60000044 = 0x00000000;
    *(volatile unsigned int *)0x60000048 = 0x00000000;
    *(volatile unsigned int *)0x6000004c = 0x00000000;
    *(volatile unsigned int *)0x60000050 = 0x00000000;
    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;

    //word 24
    *(volatile unsigned int *)0x60000060 = in_data_len;

    *(volatile unsigned int *)0x60000064 = 0x00000000;
    *(volatile unsigned int *)0x60000068 = 0x00000000;
    *(volatile unsigned int *)0x6000006c = 0x00000000;

    //msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, in_data_len:%d word_6:%08x \r\n", in_data_len, word_6);

    /* Key */
     for (i=0; i<sk_len; i++) {
         *(volatile unsigned int *)(0x60000070 + i) = *(sk + i);
     }

    /* Write a word */
    *(volatile unsigned int *)0x60003f00 = 0x1;

    /*
            SCRT Control- Check Operation Status
        */
    while(1){
        cnt++;
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
        delay_(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    /*
        SCRT Control- Check output token status
       */
    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = false;
        //msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, output token status fail \r\n");
    }

    /* Copy the output MAC data */
    //os_memcpy((void *)mac, (void *)0x60000008, 20);
    memcpy((void *)mac, (void *)0x60000008, 20);

    /*
            SCRT Control- Clear Status
       */
    *(volatile unsigned int *)0x60003f00 = 0x2;

    return status;
func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}

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
int nl_scrt_aes_ecb_patch(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len)
{
    unsigned int data;
    unsigned int r_output_token_data;
    unsigned int c_output_token_id = 0xd82c;
    int status = 1;
    //int i;
    unsigned int word_11;
    int quo, rem;
    int buf_len;
    int key_len = 1;
    int fieldEncrypt;
    unsigned int sk_m;
    unsigned int data_in_m;
    unsigned int data_out_m;
    //unsigned char w_key[4] = {0};
    int cnt = 0;

    //printf("\r\n nl_scrt_aes_ecb, sk:%lx data_in:%lx data_out:%lx \r\n", sk, data_in, data_out); //sk:602b28 data_in:602b38 data_out:602b18

#if 0
    //Remapping
    sk_m = (unsigned int)sk | 0x20000000;
    data_in_m = (unsigned int)data_in | 0x20000000;
    data_out_m = (unsigned int)data_out | 0x20000000;
#else
    sk_m = (unsigned int)sk;
    data_in_m = (unsigned int)data_in;
    data_out_m = (unsigned int)data_out;
#endif

    if(sk == 0||
       data_in == 0 ||
       data_out == 0){
        return 0;
    }

    //printf("\r\n nl_scrt_aes_ecb, sk_m:%lx \r\n", sk_m);
    //printf("\r\n nl_scrt_aes_ecb, data_in_m:%lx \r\n", data_in_m);
    //printf("\r\n nl_scrt_aes_ecb, data_out_m:%lx \r\n", data_out_m);

    if(sk_len != 16 &&
       sk_len != 24 &&
       sk_len != 32) {
        return 0;
    }

    if(data_len <= 0) {
        return 0;
    }

    if(bEncrypt == 1) {
        fieldEncrypt = 1;
    } else {
        fieldEncrypt = 0;
    }

    switch (sk_len)
    {
        case 16:
            key_len = 1;
            break;
        case 24:
            key_len = 2;
            break;
        case 32:
            key_len = 3;
            break;
    }

    //It should be a multiple of block size (16 bytes)
    quo = (data_len / 16);
    rem = (data_len % 16);
    if(rem == 0) {
        buf_len = (quo * 16);
    } else {
        buf_len = ((quo + 1) * 16);
    }

    *(volatile unsigned int *)0x60003f00 = 0x4;

    //printf("\r\n nl_scrt_aes_ecb, 444, fieldEncrypt:%d key_len:%d buf_len:%d \r\n", fieldEncrypt, key_len, buf_len);

    while(1){
        delay_(3);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    //printf("\r\n nl_scrt_aes_ecb, 555 \r\n");

    *(volatile unsigned int *)0x60000000 = 0x0104d82c;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = buf_len;
    *(volatile unsigned int *)0x6000000c = data_in_m;//(unsigned int) data_in;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = buf_len;
    *(volatile unsigned int *)0x60000018 = data_out_m;//(unsigned int) data_out;
    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = buf_len + 4;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;

    word_11 = ((key_len << 16) | (fieldEncrypt << 15) | (0x0));
    *(volatile unsigned int *)0x6000002c = word_11;
    //printf("\r\n word_11:0x%lx \r\n", word_11);

    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;

    memcpy((void *)0x60000044, (void *)sk_m, 16);

    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;
    *(volatile unsigned int *)0x60000060 = 0x00000000;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    while(1){
        cnt++;
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
        delay_(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    //printf("\r\n nl_scrt_aes_ecb, 777 \r\n");

    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = 0;
        //printf("\r\n nl_scrt_aes_ecb, 888 \r\n");
    }

    r_output_token_data = *((volatile unsigned int *) (data_out + buf_len));
    if ( r_output_token_data != c_output_token_id )
    {
       status = 0;
       //printf("\r\n nl_scrt_aes_ecb, 999 \r\n");
    }

    *(volatile unsigned int *)0x60003f00 = 0x2;
    //printf("\r\n nl_scrt_aes_ecb, ret:%d \r\n", ret);

    return status;

func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}


/*
 * nl_scrt_aes_cmac
 *
 * AES-CMAC Message Authentication Code operation.
 *
 * @param [in] sk
 *    Pointer to the key structure of the key.
 *
 * @param [in] sk_len
 *    Size of the key.
 *
 * @param [in] data_in
 *    Pointer to the input message data.
 *
 * @param [in] data_in_len
 *    Size of the input message data.
 *
 * @param [out] data_out
 *    Pointer to the output MAC data.
 *
 * @param [in] data_out_len
 *    Size of the output MAC data.
 *
 */
int
nl_scrt_aes_cmac( unsigned char *sk, int sk_len, unsigned char *data_in, int data_in_len, unsigned char *data_out, int data_out_len)
{ //not finish yet
    unsigned int data                   = 0;
    unsigned int c_output_token_id      = 0x9d37;
    unsigned int r_output_token_data    = 0;

    //unsigned int sk_m;
    //unsigned int data_in_m;

    int quo, rem;
    int buf_in_len;
    //int key_len = 1;
    //int buf_out_len;

    int status = 1;
    int cnt = 0;

    unsigned int word_6;

    //printf("\r\n nl_scrt_aes_cmac, sk:%lx data_in:%lx data_out:%lx \r\n", sk, data_in, data_out);

    //sk_m = (unsigned int)sk;
    //data_in_m = (unsigned int)data_in;

    if(sk == 0||
       data_in == 0 ||
       data_out == 0){
        return 0;
    }

    //printf("\r\n nl_scrt_aes_cmac, sk_m:%lx \r\n", sk_m);
    //printf("\r\n nl_scrt_aes_cmac, data_in_m:%lx \r\n", data_in_m);
    //printf("\r\n nl_scrt_aes_cmac, data_out_m:%lx \r\n", data_out_m);

    if(sk_len != 16 &&
       sk_len != 24 &&
       sk_len != 32) {
        return 0;
    }

    if(data_in_len <= 0 || data_out_len <= 0) {
        return 0;
    }

    //It should be a multiple of block size (16 bytes)
    quo = (data_in_len / 16);
    rem = (data_in_len % 16);
    if(rem == 0) {
        buf_in_len = (quo * 16);
    } else {
        buf_in_len = ((quo + 1) * 16);
    }

    //printf("\r\n nl_scrt_aes_cmac, data_in_len:%d \r\n", data_in_len);
    //printf("\r\n nl_scrt_aes_cmac, buf_in_len:%d \r\n", buf_in_len);

    *(volatile unsigned int *)0x60003f00 = 0x4;

    while(1){
        delay_(3);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    *(volatile unsigned int *)0x60000000 = 0x03009d37;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = buf_in_len;
    *(volatile unsigned int *)0x6000000c = (unsigned int)data_in;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = buf_in_len;

    //*(volatile unsigned int *)0x60000018 = 0x00100008;
    word_6 = (sk_len << 16) | 0x0008;
    *(volatile unsigned int *)0x60000018 = word_6;
    //printf("\r\n nl_scrt_aes_cmac, word_6:%lx \r\n", word_6);

    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = 0x00000000;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;
    *(volatile unsigned int *)0x6000002c = 0x00000000;
    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;
    *(volatile unsigned int *)0x60000044 = 0x00000000;
    *(volatile unsigned int *)0x60000048 = 0x00000000;
    *(volatile unsigned int *)0x6000004c = 0x00000000;
    *(volatile unsigned int *)0x60000050 = 0x00000000;
    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;
    *(volatile unsigned int *)0x60000060 = 0x00000000;
    *(volatile unsigned int *)0x60000064 = 0x00000000;
    *(volatile unsigned int *)0x60000068 = 0x00000000;
    *(volatile unsigned int *)0x6000006c = 0x00000000;

    //key, word 28~57
    memcpy((void *)0x60000070, (void *)sk, sk_len);

    if (sk_len == 16) {
        *(volatile unsigned int *)0x60000080 = 0x00000000;
        *(volatile unsigned int *)0x60000084 = 0x00000000;
        *(volatile unsigned int *)0x60000088 = 0x00000000;
        *(volatile unsigned int *)0x6000008c = 0x00000000;
    } else if (sk_len == 24) {
        *(volatile unsigned int *)0x60000088 = 0x00000000;
        *(volatile unsigned int *)0x6000008c = 0x00000000;
    }

    *(volatile unsigned int *)0x60000090 = 0x00000000;
    *(volatile unsigned int *)0x60000094 = 0x00000000;
    *(volatile unsigned int *)0x60000098 = 0x00000000;
    *(volatile unsigned int *)0x6000009c = 0x00000000;
    *(volatile unsigned int *)0x600000a0 = 0x00000000;
    *(volatile unsigned int *)0x600000a4 = 0x00000000;
    *(volatile unsigned int *)0x600000a8 = 0x00000000;
    *(volatile unsigned int *)0x600000ac = 0x00000000;
    *(volatile unsigned int *)0x600000b0 = 0x00000000;
    *(volatile unsigned int *)0x600000b4 = 0x00000000;
    *(volatile unsigned int *)0x600000b8 = 0x00000000;
    *(volatile unsigned int *)0x600000bc = 0x00000000;
    *(volatile unsigned int *)0x600000c0 = 0x00000000;
    *(volatile unsigned int *)0x600000c4 = 0x00000000;
    *(volatile unsigned int *)0x600000c8 = 0x00000000;
    *(volatile unsigned int *)0x600000cc = 0x00000000;
    *(volatile unsigned int *)0x600000d0 = 0x00000000;
    *(volatile unsigned int *)0x600000d4 = 0x00000000;
    *(volatile unsigned int *)0x600000d8 = 0x00000000;
    *(volatile unsigned int *)0x600000dc = 0x00000000;
    *(volatile unsigned int *)0x600000e0 = 0x00000000;
    *(volatile unsigned int *)0x600000e4 = 0x00000000;
    *(volatile unsigned int *)0x600000e8 = 0x00000000;
    *(volatile unsigned int *)0x600000ec = 0x00000000;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    while(1){
        cnt++;
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
        delay_(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = 0;
    }

    /* Copy the output mac data */
    memcpy((void *)data_out, (void *)0x60000008, data_out_len);

    //Write
    *(volatile unsigned int *)0x60003f00 = 0x2;

    //printf("\r\n nl_scrt_aes_cmac, cnt:%d status:%d \r\n", cnt, status);

    return status;

func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}

/*
 * nl_scrt_trng
 *
 * Generate True Random Number
 *
 * @param [in] size
 *    Size of the random number
 *
 * @param [out] trng_out
 *    Pointer to the output random number.
 *
 * @return 1 success
 *
 * @return 0 fail
 */
int nl_scrt_trng(int size, unsigned char *trng_out)
{
    unsigned int c_output_token_id_1    = 0xc8a7;
    unsigned int c_output_token_id_2    = 0x4da5;
    unsigned int r_output_token_data    = 0;
    unsigned int data                   = 0;
    int status = 1;
    int cnt = 0;

    // Todo
    if(!nl_scrt_otp_status_get())
    {
        SCRT_LOGE("[%s %d] OTP not ready\n", __func__, __LINE__);
        return 0;
    }

    if(trng_out == 0) return 0;

    //It should be a multiple of 4 bytes
    if( (size <= 0) || (size > 256) || ((size % 4) != 0)) return 0;

    *(volatile unsigned int *)0x60003f00 = 0x4;

    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    //Write Token - Configure Random Number
    *(volatile unsigned int *)0x60000000 = 0x1402c8a7;
    *(volatile unsigned int *)0x60000004 = 0x4f5a3647;
    *(volatile unsigned int *)0x60000008 = 0x00000003;
    *(volatile unsigned int *)0x6000000c = 0x00030801;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    while(1){
        cnt++;
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
        delay_(10);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id_1) {
        status = 0;
    }

    *(volatile unsigned int *)0x60003f00 = 0x2;

    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    //Write Token - Get Random Number
    *(volatile unsigned int *)0x60000000 = 0x04044da5;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = size;
    *(volatile unsigned int *)0x6000000c = (unsigned int) trng_out;
    *(volatile unsigned int *)0x60000010 = 0x00000000;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    cnt = 0;
    while(1){
        cnt++;
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
        delay_(10);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id_2) {
        status = 0;
    }

    //Write a word
    *(volatile unsigned int *)0x60003f00 = 0x2;

    //printf("\r\n cnt:%d status:%d \r\n", cnt, status);
    return status;
func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;
    return status;
}

#if 0
/*
 * hmac_sha_1_sample - Sample code to do HMAC SHA1
 *
 */
void hmac_sha_1_sample(void)
{
    uint8_t passphrase[8] = {0x32, 0x32, 0x32, 0x32,
                             0x32, 0x32, 0x32, 0x32};
    uint8_t data[20] = {0xad, 0x82, 0x16, 0xc9,
                        0x63, 0x59, 0x33, 0xfd,
                        0x82, 0xa7, 0x6b, 0xad,
                        0x69, 0xe4, 0x51, 0x81,
                        0xa1, 0x18, 0x37, 0xb7
                        };
    uint8_t mac[20] = {0};
    int i;

    msg_print(LOG_HIGH_LEVEL, "\r\nhmac_sha_1_sample\r\n");
    os_memset(mac, 0, sizeof(mac));

    nl_hmac_sha_1(passphrase, sizeof(passphrase), data, sizeof(data), mac);

    /* Show output MAC */
    for(i=0; i < sizeof(mac) ;i++) {
        msg_print(LOG_HIGH_LEVEL, "hmac_sha_1_sample, output mac[%d]:%02x \r\n", i, mac[i]);
    }
}
#endif

#endif

