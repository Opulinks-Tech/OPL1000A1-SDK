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
#include <stdarg.h>
#include "cmsis_os.h"
#include "opl1000.h"
#include "basic_defs.h"
#include "msg.h"
#include "scrt.h"
#include "ipc.h"
#include "scrt_cmd.h"
#include "scrt_patch.h"
#include "scrt_cmd_patch.h"
#include "hal_system.h"


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

//#define SCRT_CHECK

#ifdef SCRT_CHECK
    #define SCRT_ASSERT(a)    \
    if(!(a))    \
    {     \
        tracer_drct_printf("SCRT_ASSERT FAILED '" #a "' %s %s:%d\n", __func__, __FILE__, __LINE__);   \
        while(1);    \
    }
#else
    #define SCRT_ASSERT(...)
#endif

#define SCRT_IP_CLK_MSK     (0x00000001)
#define SCRT_OTP_CLK_MSK    (0x00000002)


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
    SCRT_TOKEN_ID_AES_CMAC = 0xE0F0,
    SCRT_TOKEN_ID_SHA = 0xE100,

    SCRT_TOKEN_ID_MAX = 0xEFFF
} T_ScrtTokenId;

typedef struct
{
    uint8_t u8Used;
    uint8_t u8MbIdx;
} T_ScrtRes;


// internal
extern RET_DATA nl_scrt_common_fp_t scrt_param_init;
extern RET_DATA nl_scrt_common_fp_t scrt_mb_init;
extern RET_DATA scrt_cmd_fp_t scrt_trng_init;
extern RET_DATA scrt_cmd_fp_t scrt_eng_init;
extern RET_DATA scrt_status_chk_fp_t scrt_status_chk;
extern RET_DATA scrt_asset_create_fp_t scrt_ecdh_key_param_init;
extern RET_DATA scrt_asset_create_fp_t scrt_ecdh_priv_key_asset_create;
extern RET_DATA scrt_asset_create_fp_t scrt_ecdh_pub_key_asset_create;
extern RET_DATA scrt_asset_create_fp_t scrt_ecdh_shared_secret_asset_create;
extern RET_DATA scrt_asset_delete_fp_t scrt_asset_delete;
extern RET_DATA scrt_asset_load_fp_t scrt_asset_load;
extern RET_DATA scrt_asset_get_fp_t scrt_asset_get;
extern RET_DATA scrt_cmd_fp_t scrt_reset;
extern RET_DATA nl_scrt_common_fp_t scrt_internal_reset;
extern RET_DATA scrt_cmd_fp_t scrt_link;

#ifdef SCRT_ENABLE_UNLINK
extern RET_DATA scrt_cmd_fp_t scrt_unlink;
#endif

#ifdef SCRT_ACCESS_SEM
extern RET_DATA nl_scrt_common_fp_t scrt_access_lock;
extern RET_DATA nl_scrt_common_fp_t scrt_access_unlock;
#endif

extern RET_DATA nl_scrt_common_fp_t scrt_res_lock;
extern RET_DATA nl_scrt_common_fp_t scrt_res_unlock;
extern RET_DATA nl_scrt_common_fp_t scrt_ecdh_lock;
extern RET_DATA nl_scrt_common_fp_t scrt_ecdh_unlock;
extern RET_DATA nl_scrt_common_fp_t scrt_sem_create;
extern RET_DATA scrt_res_alloc_fp_t scrt_res_alloc;
extern RET_DATA scrt_res_free_fp_t scrt_res_free;

// external
extern RET_DATA nl_scrt_common_fp_t nl_scrt_Init;
extern RET_DATA nl_aes_ccm_fp_t nl_aes_ccm;
extern RET_DATA nl_scrt_aes_ccm_fp_t nl_scrt_aes_ccm;
extern RET_DATA nl_hmac_sha_1_fp_t nl_hmac_sha_1;
extern RET_DATA nl_scrt_aes_ecb_fp_t nl_scrt_aes_ecb;
extern RET_DATA nl_scrt_common_fp_t nl_scrt_otp_status_get;
extern RET_DATA nl_scrt_ecdh_key_pair_gen_fp_t nl_scrt_ecdh_key_pair_gen;
extern RET_DATA nl_scrt_ecdh_dhkey_gen_fp_t nl_scrt_ecdh_dhkey_gen;
extern RET_DATA nl_scrt_key_delete_fp_t nl_scrt_key_delete;

RET_DATA nl_scrt_aes_cmac_fp_t nl_scrt_aes_cmac_get;
//RET_DATA nl_scrt_hmac_sha_1_step_fp_t nl_scrt_hmac_sha_1_step;
//RET_DATA nl_scrt_sha_1_fp_t nl_scrt_sha_1;
//RET_DATA nl_scrt_sha_256_fp_t nl_scrt_sha_256;

RET_DATA nl_scrt_sha_fp_t nl_scrt_sha;


extern T_ScrtRes g_tScrtRes[SCRT_MB_IDX_MAX];
extern osSemaphoreId g_tScrtResSem;
extern osSemaphoreId g_tScrtEcdhSem;

#ifdef SCRT_ACCESS_SEM
extern osSemaphoreId g_tScrtAccessSem;
#endif

//uint8_t g_u8ScrtOtpReady = 0;
//uint32_t g_u32ScrtKeyParamAsId = 0;
extern uint32_t g_u32ScrtWaitResCnt;
extern uint32_t g_u32ScrtWaitRspCnt;

extern uint32_t g_u32aScrtKeyParamFmt[56];


/*
 * nl_aes_cmac_patch - Perform the AES CMAC Operation
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
int nl_scrt_aes_cmac_patch(uint8_t *u8aKey, uint8_t u8KeyLen, uint8_t *u8aInputBuf, uint32_t u32BufSize, uint32_t u32InputLen, uint8_t *u8aMac)
{
    int status = 0;
    unsigned int word_6 = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t u8NeedToClr = 0;
    uint32_t u32BlkSize = 16;
    uint32_t u32DataLen = 0;
    uint32_t u32PadLen = 0;
    uint8_t *u8aInput = NULL;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if((!u8aKey) || (!u8KeyLen) || 
       (!u8aMac))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    if(u32InputLen)
    {
        if(!u8aInputBuf)
        {
            SCRT_LOGE("[%s %d] invalid input buffer\n", __func__, __LINE__);
            goto done;
        }
    }

    if((u8KeyLen != 16) && (u8KeyLen != 24) && (u8KeyLen != 32))
    {
        SCRT_LOGE("[%s %d] invalid key length[%d]\n", __func__, __LINE__, u8KeyLen);
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

    if(u32InputLen)
    {
        uint32_t u32Remain = u32InputLen & (u32BlkSize - 1);

        u32DataLen = u32InputLen & (~(u32BlkSize - 1));
        
        if(u32Remain)
        {
            u32DataLen += u32BlkSize;
            u32PadLen = u32BlkSize - u32Remain;
        }
    }
    else
    {
        u32DataLen = u32BlkSize;
        u32PadLen = u32BlkSize;
    }

    // MAC: start
    if(u32PadLen)
    {
        uint8_t u8Alloc = 1;

        if(u8aInputBuf)
        {
            if(u32BufSize >= u32DataLen)
            {
                u8aInput = u8aInputBuf;
                u8Alloc = 0;
            }
        }

        if(u8Alloc)
        {
            u8aInput = SCRT_MALLOC(u32DataLen);

            if(u8aInput == NULL)
            {
                SCRT_LOGE("[%s %d] SCRT_MALLOC fail\n", __func__, __LINE__);
                goto done;
            }

            if(u8aInputBuf && u32InputLen)
            {
                memcpy(u8aInput, u8aInputBuf, u32InputLen);
            }
        }

        // padding
        u8aInput[u32InputLen] = 0x80;
        memset(&(u8aInput[u32InputLen + 1]), 0x00, u32PadLen - 1);
    }
    else // multiple of 16
    {
        u8aInput = u8aInputBuf;
    }

    u16TokenId = SCRT_TOKEN_ID_AES_CMAC + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    u32aBase[0] = 0x03000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32DataLen;
    u32aBase[3] = (uint32_t)u8aInput;
    u32aBase[4] = 0x00000000;
    u32aBase[5] = u32DataLen;

    word_6 = ((u8KeyLen & 0xff) << 16) | 0x08;
    u32aBase[6] = word_6;

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
    if(u32PadLen < u32BlkSize)
    {
        u32aBase[24] = u32PadLen & 0x0F;
    }
    else
    {
        u32aBase[24] = 0x0F;
    }

    u32aBase[25] = 0x00000000;
    u32aBase[26] = 0x00000000;
    u32aBase[27] = 0x00000000;

    //msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, in_data_len:%d word_6:%08x \r\n", in_data_len, word_6);

    /* Key */
    memcpy((void *)&(u32aBase[28]), u8aKey, u8KeyLen);

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
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_AES_CMAC[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    /* Copy the output MAC data */
    memcpy((void *)u8aMac, (void *)&(u32aBase[2]), SCRT_AES_CMAC_OUTPUT_LEN);

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    u8NeedToClr = 0;
    // MAC output: end

    status = 1;

done:
    if((u8aInput) && (u8aInput != u8aInputBuf))
    {
        SCRT_FREE(u8aInput);
    }

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

#if 0
/*
 * nl_hmac_sha_1_step - Perform the HMAC SHA1 Operation
 *
 * @param [in] type
 *      Operation type: 0: new/1: continue/2:final
 *
 * @param [in] total length
 *      Total Length. It's necessary for final operation.
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
int nl_scrt_hmac_sha_1_step_patch(uint8_t type, uint32_t total_len, uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac)
{
    int status = 0;
    unsigned int word_6 = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t u8NeedToClr = 0;

    uint8_t u8Mode = 0;
    uint32_t u32DataLen = 0;
    uint32_t u32BlkSize = 64; // must be power of 2

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if(type >= SCRT_STEP_MAX)
    {
        SCRT_LOGE("[%s %d] invalid type[%d]\n", __func__, __LINE__, type);
        goto done;
    }

    if((!sk) || (!sk_len) || 
       (!in_data) || (!in_data_len) || 
       (!mac))
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    if(type == SCRT_STEP_FINAL)
    {
        u32DataLen = in_data_len;
    }
    else
    {
        if(in_data_len & (u32BlkSize - 1))
        {
            SCRT_LOGE("[%s %d] invalid length[%u] for non-final block\n", __func__, __LINE__, in_data_len);
            goto done;
        }

        u32DataLen = in_data_len;
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
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32DataLen;
    u32aBase[3] = (uint32_t)in_data;
    u32aBase[4] = 0x00000000;
    u32aBase[5] = in_data_len;

    switch(type)
    {
    case SCRT_STEP_NEW:
        u8Mode = 0x02;
        break;

    case SCRT_STEP_CONTINUE:
        u8Mode = 0x03;
        break;

    case SCRT_STEP_FINAL:
        u8Mode = 0x01;
        break;

    default:
        SCRT_ASSERT(0);
    }

    word_6 = ((sk_len & 0xff) << 16) | ((u8Mode & 0x03) << 4) | ((0x1) << 0);
    
    u32aBase[6] = word_6;      //key length: 0x08  ;  [3:0] Algorithm  HMAC-SHA-1, 160-bit MAC, block size is 64 Bytes
    u32aBase[7] = 0x00000000;

    if(type == SCRT_STEP_NEW)
    {
        // clear intermediate MAC (word 8 ~ 15)
        memset((void *)&(u32aBase[8]), 0, SCRT_SHA_1_INTER_MAC_LEN);
    }
    else
    {
        // copy intermediate MAC (word 8 ~ 15)
        memcpy((void *)&(u32aBase[8]), mac, SCRT_SHA_1_INTER_MAC_LEN);
    }

    u32aBase[16] = 0x00000000;
    u32aBase[17] = 0x00000000;
    u32aBase[18] = 0x00000000;
    u32aBase[19] = 0x00000000;
    u32aBase[20] = 0x00000000;
    u32aBase[21] = 0x00000000;
    u32aBase[22] = 0x00000000;
    u32aBase[23] = 0x00000000;

    //word 24
    if(type == SCRT_STEP_FINAL)
    {
        u32aBase[24] = total_len;
    }
    else
    {
        u32aBase[24] = 0x00000000;
    }

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

    if(type == SCRT_STEP_FINAL)
    {
        /* Copy the output MAC data */
        memcpy((void *)mac, (void *)&(u32aBase[2]), SCRT_SHA_1_OUTPUT_LEN);
    }
    else
    {
        memcpy((void *)mac, (void *)&(u32aBase[2]), SCRT_SHA_1_INTER_MAC_LEN);
    }

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
#endif

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
    int status = 0;
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
    int status = 0;
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

int nl_scrt_sha_patch(uint8_t u8Type, uint8_t u8Step, uint32_t u32TotalLen, uint8_t *u8aData, uint32_t u32DataLen, uint8_t u8HasInterMac, uint8_t *u8aMac)
{
    int status = 0;
    volatile uint32_t *u32aBase = NULL;
    volatile uint32_t *u32aStatus = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint32_t u32Output = 0;
    uint8_t u8ResId = SCRT_MB_IDX_MAX;
    uint16_t u16TokenId = 0;
    uint8_t u8NeedToClr = 0;
    uint8_t u8Mode = 0;
    uint8_t u8Alg = 0;
    uint32_t u32StepSize = 0;
    uint32_t u32OutputLen = 0;

    #ifdef SCRT_ENABLE_UNLINK
    uint8_t u8Link = 0;
    #endif

    if(!u8aMac)
    {
        SCRT_LOGE("[%s %d] invalid param\n", __func__, __LINE__);
        goto done;
    }

    if((u32DataLen) && (!u8aData))
    {
        SCRT_LOGE("[%s %d] invalid data buffer\n", __func__, __LINE__);
        goto done;
    }

    switch(u8Type)
    {
        case SCRT_TYPE_SHA_1:
            u8Alg = 1;
            u32StepSize = SCRT_SHA_1_STEP_SIZE;
            u32OutputLen = SCRT_SHA_1_OUTPUT_LEN;
            break;
    
        case SCRT_TYPE_SHA_224:
            u8Alg = 2;
            u32StepSize = SCRT_SHA_224_STEP_SIZE;
            u32OutputLen = SCRT_SHA_224_OUTPUT_LEN;
            break;
    
        case SCRT_TYPE_SHA_256:
            u8Alg = 3;
            u32StepSize = SCRT_SHA_256_STEP_SIZE;
            u32OutputLen = SCRT_SHA_256_OUTPUT_LEN;
            break;
    
        case SCRT_TYPE_SHA_384:
            u8Alg = 4;
            u32StepSize = SCRT_SHA_384_STEP_SIZE;
            u32OutputLen = SCRT_SHA_384_OUTPUT_LEN;
            break;
    
        case SCRT_TYPE_SHA_512:
            u8Alg = 5;
            u32StepSize = SCRT_SHA_512_STEP_SIZE;
            u32OutputLen = SCRT_SHA_512_OUTPUT_LEN;
            break;
    
        default:
            SCRT_LOGE("[%s %d] unknown type[%d]\n", __func__, __LINE__, u8Type);
            goto done;
    }

    if(u8Step)
    {
        if(u32DataLen > u32StepSize)
        {
            SCRT_LOGE("[%s %d] invalid data_len[%u] > [%u]\n", __func__, __LINE__, u32DataLen, u32StepSize);
            goto done;
        }

        if(u8HasInterMac)
        {
            if(u32DataLen < u32StepSize)
            {
                // final
                u8Mode = 0x01;
            }
            else
            {
                // continue
                u8Mode = 0x03;
            }
        }
        else
        {
            if(u32DataLen < u32StepSize)
            {
                // initial and final
                u8Mode = 0x00;
            }
            else
            {
                // initial
                u8Mode = 0x02;
            }
        }
    }
    else
    {
        // initial and final
        u8Mode = 0x00;
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

    u16TokenId = SCRT_TOKEN_ID_SHA + g_tScrtRes[u8ResId].u8MbIdx;
    u32aBase = (volatile uint32_t *)SCRT_BASE_ADDR(g_tScrtRes[u8ResId].u8MbIdx);

    // Hash: start
    u32aBase[0] = 0x02000000 | u16TokenId;
    u32aBase[1] = SCRT_ID;
    u32aBase[2] = u32DataLen;
    u32aBase[3] = (uint32_t)u8aData;
    u32aBase[4] = 0x00000000;
    u32aBase[5] = u32DataLen;
    u32aBase[6] = ((u8Mode & 0x03) << 4) | (u8Alg & 0x0F);
    u32aBase[7] = 0x00000000;

    if(u8Mode & 0x01) // continue
    {
        // copy intermediate MAC
        memcpy((void *)&(u32aBase[8]), u8aMac, u32OutputLen);
    }
    
    u32aBase[24] = u32TotalLen;
    u32aBase[25] = 0x00000000;

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

    // Hash output: start
    u32Output = u32aBase[0];

    if(u32Output != u16TokenId)
    {
        SCRT_LOGE("[%s %d] output token id[%08X] != SCRT_TOKEN_ID_SHA[%08X]\n", __func__, __LINE__, 
                  u32Output, u16TokenId);

        #ifdef SCRT_CHECK
        SCRT_ASSERT(0);
        #else
        goto done;
        #endif
    }

    // intermediate or final mac
    memcpy((void *)u8aMac, (void *)&(u32aBase[2]), u32OutputLen);

    *u32aStatus = SCRT_CTRL_READ_MSK(g_tScrtRes[u8ResId].u8MbIdx);
    u8NeedToClr = 0;
    // Hash output: end

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

void scrt_clk_enable(uint8_t u8Enable, uint32_t u32Msk)
{
    if(u32Msk & SCRT_IP_CLK_MSK)
    {
        Hal_Sys_ApsClkEn(u8Enable, APS_CLK_SCRT);
    }

    if(u32Msk & SCRT_OTP_CLK_MSK)
    {
        Hal_Sys_ApsClkEn(u8Enable, APS_CLK_OTP);
    }

    return;
}

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

    scrt_clk_enable(1, (SCRT_OTP_CLK_MSK | SCRT_IP_CLK_MSK));

    if(scrt_mb_init())
    {
        SCRT_LOGE("[%s %d] scrt_mb_init fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 1;

done:
    scrt_clk_enable(0, (SCRT_OTP_CLK_MSK | SCRT_IP_CLK_MSK));
    return iRet;
}

int nl_scrt_otp_status_get_patch(void)
{
    int iRet = 0;
    volatile uint32_t *pu32OtpStatus = NULL;

    scrt_clk_enable(1, SCRT_OTP_CLK_MSK);

    pu32OtpStatus = (uint32_t *)SCRT_OTP_STATUS_ADDR;

    if(*pu32OtpStatus == 0)
    {
        SCRT_LOGI("[%s %d] OTP not ready\n", __func__, __LINE__);

        //scrt_clk_enable(0, SCRT_OTP_CLK_MSK);
        goto done;
    }

    iRet = 1;

done:
    return iRet;
}

uint8_t scrt_res_alloc_patch(void)
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

        // reserve SCRT_MB_IDX_0 for M0 usage
        for(i = SCRT_MB_IDX_1; i < SCRT_MB_IDX_MAX; i++)
        {
            if(g_tScrtRes[i].u8Used == 0)
            {
                scrt_clk_enable(1, (SCRT_OTP_CLK_MSK | SCRT_IP_CLK_MSK));

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

int scrt_mb_init_patch(void)
{
    int iRet = -1;
    volatile uint32_t *u32Status = (uint32_t *)SCRT_STAT_CTRL_ADDR;
    uint8_t u8AvailCnt = 0;
    uint8_t i = 0;

    for(i = SCRT_MB_IDX_0; i < SCRT_MB_IDX_MAX; i++)
    {
        uint32_t u32Mask = SCRT_STATUS_WRITE_MSK(i) | SCRT_STATUS_READ_MSK (i) | SCRT_STATUS_LINK_MSK(i) | SCRT_STATUS_AVAIL_MSK(i);

        if((*u32Status & u32Mask) != SCRT_STATUS_AVAIL_MSK(i))
        {
            if(!Hal_Sys_ApsModuleRst(ASP_RST_SCRT))
            {
                iRet = 0;
                break;
            }

            SCRT_LOGE("[%s %d] Hal_Sys_ApsModuleRst(ASP_RST_SCRT) fail\n", __func__, __LINE__);
        }
        else
        {
            ++u8AvailCnt;
        }
    }

    if(u8AvailCnt == SCRT_MB_IDX_MAX)
    {
        iRet = 0;
    }

    return iRet;
}

/*
 * scrt_drv_func_init - Interface Initialization: SCRT
 *
 */
void scrt_drv_func_init_patch(void)
{
    nl_scrt_aes_cmac_get = nl_scrt_aes_cmac_patch;
    nl_scrt_aes_ccm = nl_scrt_aes_ccm_patch;
    nl_scrt_aes_ecb = nl_scrt_aes_ecb_patch;
    nl_scrt_sha = nl_scrt_sha_patch;
    nl_scrt_Init = nl_scrt_init_patch;
    nl_scrt_otp_status_get = nl_scrt_otp_status_get_patch;
    scrt_res_alloc = scrt_res_alloc_patch;
    scrt_mb_init = scrt_mb_init_patch;

    #ifdef SCRT_CMD_PATCH
    nl_scrt_cmd_func_init_patch();
    #endif
    
    return;
}

