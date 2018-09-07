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

#include "scrt_cmd_patch.h"


#ifdef SCRT_CMD_PATCH


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scrt.h"
#include "le_smp_util.h"
#include "diag_task.h"
#include "scrt_patch.h"


#define SCRT_CMD_ECDH
#define SCRT_CMD_AES_CCM
#define SCRT_CMD_HMAC_SHA_1
#define SCRT_CMD_AES_ECB
#define SCRT_CMD_SUT_TASK
//#define SCRT_CMD_HMAC_SHA_1_STEP
#define SCRT_CMD_AES_CMAC
#define SCRT_CMD_SHA

//#define SCRT_KEY_PAIR_1
#define SCRT_DHKEY_1
//#define SCRT_AES_CCM_ENC_1
//#define SCRT_AES_CCM_DEC_1
//#define SCRT_CMAC_SHA1_1
//#define SCRT_AES_ECB_ENC_1
//#define SCRT_AES_ECB_DEC_1

//#define SCRT_KEY_PAIR_2
//#define SCRT_DHKEY_2
#define SCRT_AES_CCM_ENC_2
#define SCRT_AES_CCM_DEC_2
#define SCRT_CMAC_SHA1_2
#define SCRT_AES_ECB_ENC_2
#define SCRT_AES_ECB_DEC_2

//#define SCRT_KEY_PAIR_3
//#define SCRT_DHKEY_3
#define SCRT_AES_CCM_ENC_3
#define SCRT_AES_CCM_DEC_3
#define SCRT_CMAC_SHA1_3
#define SCRT_AES_ECB_ENC_3
#define SCRT_AES_ECB_DEC_3

//#define SCRT_KEY_PAIR_4
//#define SCRT_DHKEY_4
#define SCRT_AES_CCM_ENC_4
#define SCRT_AES_CCM_DEC_4
#define SCRT_CMAC_SHA1_4
#define SCRT_AES_ECB_ENC_4
#define SCRT_AES_ECB_DEC_4

#define SCRT_SUT_TASK_NUM           4
#define SCRT_SUT_TASK_DELAY         200 // ms
#define SCRT_SUT_TASK_STACK_SIZE    512

#define SCRT_CURR_TIME      (*(volatile uint32_t *)(0x40003044))


// internal
extern RET_DATA os_pthread scrt_sut_task_main_1;
extern RET_DATA os_pthread scrt_sut_task_main_2;
extern RET_DATA os_pthread scrt_sut_task_main_3;
extern RET_DATA os_pthread scrt_sut_task_main_4;
extern RET_DATA scrt_sut_task_delete_fp_t scrt_sut_task_delete;
extern RET_DATA scrt_sut_task_create_fp_t scrt_sut_task_create;

// external
extern RET_DATA nl_scrt_cmd_fp_t nl_scrt_cmd;


// 7.1.2.1 P-256 Data Set 1
// Private key of A
extern const uint32_t g_u32aAPrivKey1[8];

// Public key of B
extern const uint8_t g_u8aBPubKeyX1[32];

extern const uint8_t g_u8aBPubKeyY1[32];
// --------------------------------------------------------------------
// Private key of B
extern const uint32_t g_u32aBPrivKey1[8];

// Public key of A
extern const uint8_t g_u8aAPubKeyX1[32];

extern const uint8_t g_u8aAPubKeyY1[32];

// 7.1.2.1 P-256 Data Set 2
// Private key of A
extern const uint32_t g_u32aAPrivKey2[8];

// Public key of B
extern const uint8_t g_u8aBPubKeyX2[32];

extern const uint8_t g_u8aBPubKeyY2[32];

// --------------------------------------------------------------------
// Private key of B
extern const uint32_t g_u32aBPrivKey2[8];

// Public key of A
extern const uint8_t g_u8aAPubKeyX2[32];

extern const uint8_t g_u8aAPubKeyY2[32];

extern const uint8_t g_u8aScrtAesCcmSk[16];

extern const uint8_t g_u8aScrtAesCcmNonce[13];

extern const uint8_t g_u8aScrtAesCcmAdata[22];

extern const uint8_t g_u8aScrtAesCcmTag[8];

extern const uint8_t g_u8ScrtAesCcmPlainText[286 + 6];

extern const uint8_t g_u8aScrtAesCcmCipher[286 + 6];

extern const uint8_t g_u8aCmacSha1Sk[8];

extern const uint8_t g_u8aCmacSha1Data[20];

extern const uint8_t g_u8aCmacSha1Mac[20];

extern const uint8_t g_u8aAesEcbSk[16];

extern const uint8_t g_u8aAesEcbInput[16];

extern const uint8_t g_u8aAesEcbOutput[16];

extern uint32_t g_u32aHwPubKey[16];
extern uint32_t g_u32aSwPrivKey[8];
extern uint32_t g_u32aSwPubKey[16];

extern volatile uint8_t g_bScrtSutTaskRun;
extern osThreadId g_taScrtSutThreadId[SCRT_SUT_TASK_NUM];

extern const char *g_saScrtSutTaskName[SCRT_SUT_TASK_NUM];

extern uint32_t g_baScrtError[SCRT_SUT_TASK_NUM];
extern uint32_t g_baScrtCnt[SCRT_SUT_TASK_NUM];

#ifdef SCRT_CMD_AES_CMAC
const uint8_t g_u8aScrtAesCmacSk[] = 
{
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F 
};

const uint8_t g_u8aScrtAesCmacData[128] = 
{
    0x38, 0xD7, 0xCB, 0xF0, 0xE7, 0x88, 0x2A, 0x76, 0xE1, 0x97, 0x6B, 0x6F, 0xFA, 0x68, 0x80, 0xB3, 
    0x0F, 0x12, 0x73, 0x00, 0xAF, 0xC5, 0x88, 0x2F, 0xE8, 0x4A, 0x40, 0xA1, 0xED, 0x6C, 0x34, 0xD7, 
    0xDF, 0x31, 0xE9, 0x75, 0x3C, 0x64, 0xA4, 0x78, 0x17, 0x1E, 0x80, 0x0A, 0x1C, 0xFE, 0xA0, 0xF6, 
    0x5E, 0x58, 0x47, 0xBF, 0x66, 0x75, 0x00, 0x4B, 0x3C, 0x90, 0xD6, 0x6D, 0x37, 0x13, 0x66, 0xF1, 
    0x4C, 0x93, 0xF3, 0xEC, 0xD0, 0x00, 0x57, 0x55, 0x3A, 0xB4, 0x46, 0x52, 0x37, 0xBC, 0x5A, 0xDE, 
};
#endif //#ifdef SCRT_CMD_AES_CMAC

#ifdef SCRT_CMD_HMAC_SHA_1_STEP
const uint8_t g_u8aScrtHmacSha1SkStep[] = 
{
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38
};
#endif //#ifdef SCRT_CMD_HMAC_SHA_1_STEP

#ifdef SCRT_CMD_SHA
const uint8_t g_u8aScrtShaData[] = 
{
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
    0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 
    
    0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
    0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 

    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
    0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 
    0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 
    
    0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 
    0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 
    
};
#endif //#ifdef SCRT_CMD_SHA

void nl_scrt_cmd_patch(char *sCmd)
{
    char *baParam[8] = {0};
    uint32_t dwNum = 8;
    uint32_t dwParamNum = 0;
    uint32_t dwCase = 0;
    uint32_t i = 0;
    uint32_t u32Start = 0;
    uint32_t u32End = 0;

    #ifdef SCRT_CMD_ECDH
    int32_t i32Overflow = 0;
    uint32_t u32HwTime = 0;
    uint32_t u32SwTime = 0;
    uint32_t u32PeerIdx = 0;
    #endif

    extern uint32_t g_u32ScrtWaitResCnt;
    extern uint32_t g_u32ScrtWaitRspCnt;

    (void)i;

    dwParamNum = ParseParam(sCmd, baParam, dwNum);
    (void)dwParamNum;

    tracer_cli(LOG_HIGH_LEVEL, "\n\n");

    if(!nl_scrt_otp_status_get())
    {
        tracer_cli(LOG_HIGH_LEVEL, "OTP not ready\n");
        //goto done;
    }
    else
    {
        tracer_cli(LOG_HIGH_LEVEL, "OTP ready\n");
    }

    if(dwParamNum > 1)
    {
        dwCase = strtoul((char *)baParam[1], NULL, 10);
    }

    if(dwCase == 0)
    {
        #ifdef SCRT_CMD_SUT_TASK
        tracer_cli(LOG_HIGH_LEVEL, "count [%u] [%u] [%u] [%u]\n", 
                   g_baScrtCnt[0], g_baScrtCnt[1], g_baScrtCnt[2], g_baScrtCnt[3]);
        tracer_cli(LOG_HIGH_LEVEL, "error [%u] [%u] [%u] [%u]\n", 
                   g_baScrtError[0], g_baScrtError[1], g_baScrtError[2], g_baScrtError[3]);
        #endif //#ifdef SCRT_CMD_SUT_TASK
    }
    #ifdef SCRT_CMD_ECDH
    else if(dwCase == 1) // load private key
    {
        uint8_t u8aDhKey1[32] = {0};
        uint8_t u8aDhKey2[32] = {0};
    
        uint32_t u32StartUs = 0;
        uint32_t u32EndUs = 0;
        uint32_t u32HwTimeUs = 0;
        uint32_t u32SwTimeUs = 0;
    
        uint8_t u8aAPubKeyX1[32] = {0};
        uint8_t u8aAPubKeyY1[32] = {0};
        uint32_t u32aBPrivKey1[32] = {0};

        memcpy(u8aAPubKeyX1, g_u8aAPubKeyX1, sizeof(u8aAPubKeyX1));
        memcpy(u8aAPubKeyY1, g_u8aAPubKeyY1, sizeof(u8aAPubKeyY1));
        memcpy(u32aBPrivKey1, g_u32aBPrivKey1, sizeof(u32aBPrivKey1));
    
        osKernelSysTickEx(&u32Start, &i32Overflow);
        u32StartUs = SCRT_CURR_TIME;
    
        if(!nl_scrt_ecdh_dhkey_gen(u8aAPubKeyX1, u8aAPubKeyY1, u32aBPrivKey1, u8aDhKey1, 0))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] nl_scrt_dhkey_gen fail\n", __func__, __LINE__);
            goto done;
        }
    
        u32EndUs = SCRT_CURR_TIME;
        osKernelSysTickEx(&u32End, &i32Overflow);

        u32HwTime = u32End - u32Start;
        u32Start = u32End;
    
        u32HwTimeUs = u32EndUs - u32StartUs;

        u32StartUs = SCRT_CURR_TIME;
    
        if(LeSmpUtilGenDhkey(u8aAPubKeyX1, u8aAPubKeyY1, u32aBPrivKey1, u8aDhKey2))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] LeSmpUtilGenDhkey fail\n", __func__, __LINE__);
            goto done;
        }
    
        u32EndUs = SCRT_CURR_TIME;
        osKernelSysTickEx(&u32End, &i32Overflow);

        u32SwTime = u32End - u32Start;
        u32SwTimeUs = u32EndUs - u32StartUs;
    
        tracer_cli(LOG_HIGH_LEVEL, "HW proc_time: %u ms, %u us\n", u32HwTime, u32HwTimeUs);
    
        for(i = 0; i < 32; i++)
        {
            if((i % 8) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%02X]", u8aDhKey1[i]);
        }
    
        tracer_cli(LOG_HIGH_LEVEL, "\n");
    
        tracer_cli(LOG_HIGH_LEVEL, "\nSW proc_time: %u ms, %u us\n", u32SwTime, u32SwTimeUs);
    
        for(i = 0; i < 32; i++)
        {
            if((i % 8) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%02X]", u8aDhKey2[i]);
        }
    
        tracer_cli(LOG_HIGH_LEVEL, "\n\n");
    
        if(memcmp(u8aDhKey1, u8aDhKey2, 32))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] FAIL: DHKey not matched\n\n", __func__, __LINE__);
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] SUCCESS\n\n", __func__, __LINE__);
        }
    }
    else if(dwCase == 2) // generate key pair
    {
        uint32_t u32aHwPrivKey[8] = {0};
        uint32_t u32PrivKeyId = 0;

        if(dwParamNum > 2)
        {
            u32PrivKeyId = strtoul((char *)baParam[2], NULL, 16);
        }

        osKernelSysTickEx(&u32Start, &i32Overflow);

        if(!nl_scrt_ecdh_key_pair_gen(g_u32aHwPubKey, u32aHwPrivKey, &u32PrivKeyId))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] nl_scrt_ecdh_key_pair_gen fail\n", __func__, __LINE__);
            //goto done;
        }

        osKernelSysTickEx(&u32End, &i32Overflow);
        u32HwTime = u32End - u32Start;

        osKernelSysTickEx(&u32Start, &i32Overflow);

        if(LeSmpUtilGenKeyPair(g_u32aSwPubKey, g_u32aSwPrivKey))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] LeSmpUtilGenKeyPair fail\n", __func__, __LINE__);
            //goto done;
        }

        osKernelSysTickEx(&u32End, &i32Overflow);
        u32SwTime = u32End - u32Start;

        tracer_cli(LOG_HIGH_LEVEL, "HW proc_time: %u ms    u32PrivKeyId[%08X]\n", u32HwTime, u32PrivKeyId);

        for(i = 0; i < 8; i++)
        {
            if((i % 4) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%08X]", u32aHwPrivKey[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n\nPublic Key: ");

        for(i = 0; i < 16; i++)
        {
            if((i % 4) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%08X]", g_u32aHwPubKey[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n\n");

        tracer_cli(LOG_HIGH_LEVEL, "SW proc_time: %u ms\n", u32SwTime);

        for(i = 0; i < 8; i++)
        {
            if((i % 4) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%08X]", g_u32aSwPrivKey[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n\nPublic Key: ");

        for(i = 0; i < 16; i++)
        {
            if((i % 4) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%08X]", g_u32aSwPubKey[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n\n");
    }
    else if(dwCase == 3) // use prevous private key (by AS_ID)
    {
        uint8_t u8aDhKey1[32] = {0};
        uint8_t u8aDhKey2[32] = {0};

        uint8_t *pu8PubKeyX = (uint8_t *)&(g_u32aHwPubKey[0]);
        uint8_t *pu8PubKeyY = (uint8_t *)&(g_u32aHwPubKey[8]);

        uint8_t u8aPeerPubKeyX[32] = {0};
        uint8_t u8aPeerPubKeyY[32] = {0};
        uint32_t u32aPeerPrivKey[8] = {0};

        uint32_t u32PrivKeyId = 0;

        if(dwParamNum > 2)
        {
            u32PrivKeyId = strtoul((char *)baParam[2], NULL, 16);
        }

        if(dwParamNum > 3)
        {
            u32PeerIdx = strtoul((char *)baParam[3], NULL, 10);
    
            tracer_cli(LOG_HIGH_LEVEL, "u32PeerIdx[%u]\n", u32PeerIdx);
        }

        if(u32PeerIdx == 0)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Data1: A\n");

            memcpy(u8aPeerPubKeyX, g_u8aAPubKeyX1, sizeof(u8aPeerPubKeyX));
            memcpy(u8aPeerPubKeyY, g_u8aAPubKeyY1, sizeof(u8aPeerPubKeyY));
            memcpy(u32aPeerPrivKey, g_u32aAPrivKey1, sizeof(u32aPeerPrivKey));
            /*
            u8aPeerPubKeyX = g_u8aAPubKeyX1;
            u8aPeerPubKeyY = g_u8aAPubKeyY1;
            u32aPeerPrivKey = g_u32aAPrivKey1;
            */
        }
        else if(u32PeerIdx == 1)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Data1: B\n");

            memcpy(u8aPeerPubKeyX, g_u8aBPubKeyX1, sizeof(u8aPeerPubKeyX));
            memcpy(u8aPeerPubKeyY, g_u8aBPubKeyY1, sizeof(u8aPeerPubKeyY));
            memcpy(u32aPeerPrivKey, g_u32aBPrivKey1, sizeof(u32aPeerPrivKey));
            /*
            u8aPeerPubKeyX = g_u8aBPubKeyX1;
            u8aPeerPubKeyY = g_u8aBPubKeyY1;
            u32aPeerPrivKey = g_u32aBPrivKey1;
            */
        }
        else if(u32PeerIdx == 2)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Data2: A\n");

            memcpy(u8aPeerPubKeyX, g_u8aAPubKeyX2, sizeof(u8aPeerPubKeyX));
            memcpy(u8aPeerPubKeyY, g_u8aAPubKeyY2, sizeof(u8aPeerPubKeyY));
            memcpy(u32aPeerPrivKey, g_u32aAPrivKey2, sizeof(u32aPeerPrivKey));

            /*
            u8aPeerPubKeyX = g_u8aAPubKeyX2;
            u8aPeerPubKeyY = g_u8aAPubKeyY2;
            u32aPeerPrivKey = g_u32aAPrivKey2;
            */
        }
        else if(u32PeerIdx == 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Data2: B\n");

            memcpy(u8aPeerPubKeyX, g_u8aBPubKeyX2, sizeof(u8aPeerPubKeyX));
            memcpy(u8aPeerPubKeyY, g_u8aBPubKeyY2, sizeof(u8aPeerPubKeyY));
            memcpy(u32aPeerPrivKey, g_u32aBPrivKey2, sizeof(u32aPeerPrivKey));

            /*
            u8aPeerPubKeyX = g_u8aBPubKeyX2;
            u8aPeerPubKeyY = g_u8aBPubKeyY2;
            u32aPeerPrivKey = g_u32aBPrivKey2;
            */
        }
        else if(u32PeerIdx == 4)
        {
            tracer_cli(LOG_HIGH_LEVEL, "SW\n");

            memcpy(u8aPeerPubKeyX, &g_u32aSwPubKey[0], sizeof(u8aPeerPubKeyX));
            memcpy(u8aPeerPubKeyY, &g_u32aSwPubKey[8], sizeof(u8aPeerPubKeyY));
            memcpy(u32aPeerPrivKey, g_u32aSwPrivKey, sizeof(u32aPeerPrivKey));

            /*
            u8aPeerPubKeyX = (uint8_t *)&g_u32aSwPubKey[0];
            u8aPeerPubKeyY = (uint8_t *)&g_u32aSwPubKey[8];
            u32aPeerPrivKey = g_u32aSwPrivKey;
            */
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "unknown\n");
            goto done;
        }
    
        osKernelSysTickEx(&u32Start, &i32Overflow);
    
        if(!nl_scrt_ecdh_dhkey_gen(u8aPeerPubKeyX, u8aPeerPubKeyY, NULL, u8aDhKey1, u32PrivKeyId))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] nl_scrt_dhkey_gen fail\n", __func__, __LINE__);
            goto done;
        }
    
        osKernelSysTickEx(&u32End, &i32Overflow);
        u32HwTime = u32End - u32Start;
        u32Start = u32End;
    
        if(LeSmpUtilGenDhkey(pu8PubKeyX, pu8PubKeyY, u32aPeerPrivKey, u8aDhKey2))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] LeSmpUtilGenDhkey fail\n", __func__, __LINE__);
            goto done;
        }
    
        osKernelSysTickEx(&u32End, &i32Overflow);
        u32SwTime = u32End - u32Start;
    
        tracer_cli(LOG_HIGH_LEVEL, "HW proc_time: %u ms\n", u32HwTime);
    
        for(i = 0; i < 32; i++)
        {
            if((i % 8) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%02X]", u8aDhKey1[i]);
        }
    
        tracer_cli(LOG_HIGH_LEVEL, "\n");
    
        tracer_cli(LOG_HIGH_LEVEL, "\nSW proc_time: %u ms\n", u32SwTime);
    
        for(i = 0; i < 32; i++)
        {
            if((i % 8) == 0)
            {
                tracer_cli(LOG_HIGH_LEVEL, "\n");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, " [%02X]", u8aDhKey2[i]);
        }
    
        tracer_cli(LOG_HIGH_LEVEL, "\n\n");
    
        if(memcmp(u8aDhKey1, u8aDhKey2, 32))
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] FAIL: DHKey not matched\n\n", __func__, __LINE__);
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%s %d] SUCCESS\n\n", __func__, __LINE__);
        }
    }
    #endif //#ifdef SCRT_CMD_ECDH
    #ifdef SCRT_CMD_SUT_TASK
    else if(dwCase == 4)
    {
        tracer_cli(LOG_HIGH_LEVEL, "Toggle scrt sut task\n");
        scrt_sut_task_create(0);
    }
    #endif //#ifdef SCRT_CMD_SUT_TASK
    #ifdef SCRT_CMD_AES_CCM
    else if(dwCase == 5)
    {
        uint32_t u32SkLen = 16;
        uint32_t u32NonceLen = 13;
        uint32_t u32AdataLen = 22;
        uint32_t u32TagLen = 8;
        uint32_t u32DataLen = 286;
        uint8_t u8aInput[286 + 6] = {0};
        uint8_t u8aBuf[286 + 6] = {0};
        uint8_t u8aTag[8] = {0};
        //uint32_t u32Old = 0;
        uint32_t u32New = 0;

        uint8_t u8aScrtAesCcmSk[16] = {0};
        uint8_t u8aScrtAesCcmNonce[13] = {0};
        uint8_t u8aScrtAesCcmAdata[22] = {0};
        uint8_t u8aScrtAesCcmTag[8] = {0};
    
        memcpy(u8aScrtAesCcmSk, g_u8aScrtAesCcmSk, sizeof(u8aScrtAesCcmSk));
        memcpy(u8aScrtAesCcmNonce, g_u8aScrtAesCcmNonce, sizeof(u8aScrtAesCcmNonce));
        memcpy(u8aScrtAesCcmAdata, g_u8aScrtAesCcmAdata, sizeof(u8aScrtAesCcmAdata));
        memcpy(u8aScrtAesCcmTag, g_u8aScrtAesCcmTag, sizeof(g_u8aScrtAesCcmTag));

        #if 0 // Todo: disable scrt clock
        {
            uint32_t *pu32Reg = (uint32_t *)0x40001134;
            uint32_t u32Value = *pu32Reg;
    
            tracer_cli(LOG_HIGH_LEVEL, "before [%08X]\n", *pu32Reg);
            *pu32Reg = u32Value & (~(0x01000000));
            tracer_cli(LOG_HIGH_LEVEL, "after  [%08X]\n", *pu32Reg);
        }
        #endif

        memset(u8aTag, 0, sizeof(u8aTag));
        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        memcpy(u8aInput, g_u8ScrtAesCcmPlainText, sizeof(u8aInput));

        u32Start = SCRT_CURR_TIME;

        if(!nl_scrt_aes_ccm(1, 
                            u8aScrtAesCcmSk, u32SkLen, 
                            u8aScrtAesCcmNonce, u32NonceLen, 
                            u8aScrtAesCcmAdata, u32AdataLen, 
                            u8aInput, u8aBuf, u32DataLen, 
                            u8aTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32New = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm enc proc_time: %u us\n", u32New);

        if(memcmp(u8aBuf, g_u8aScrtAesCcmCipher, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "data not matched for nl_scrt_aes_ccm enc1\n");
            //goto done;
        }

        if(memcmp(u8aTag, g_u8aScrtAesCcmTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "tag not matched for nl_scrt_aes_ccm enc\n");
        }
        
        memset(u8aTag, 0, sizeof(u8aTag));
        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        memcpy(u8aInput, g_u8aScrtAesCcmCipher, sizeof(u8aInput));

        u32Start = SCRT_CURR_TIME;

        if(!nl_scrt_aes_ccm(0, 
                            u8aScrtAesCcmSk, u32SkLen, 
                            u8aScrtAesCcmNonce, u32NonceLen, 
                            u8aScrtAesCcmAdata, u32AdataLen, 
                            u8aBuf, u8aInput, u32DataLen, 
                            u8aScrtAesCcmTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32New = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm dec proc_time: %u us\n", u32New);

        if(memcmp(u8aBuf, g_u8ScrtAesCcmPlainText, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "data not matched for nl_scrt_aes_ccm dec\n");
            //goto done;
        }
        
        memset(u8aTag, 0, sizeof(u8aTag));
        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        memcpy(u8aInput, g_u8ScrtAesCcmPlainText, sizeof(u8aInput));

        #if 0
        u32Start = SCRT_CURR_TIME;
        
        if(!nl_scrt_aes_ccm(1, 
                            u8aScrtAesCcmSk, u32SkLen, 
                            u8aScrtAesCcmNonce, u32NonceLen, 
                            u8aScrtAesCcmAdata, u32AdataLen, 
                            u8aInput, u8aBuf, u32DataLen, 
                            u8aTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm_impl fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Old = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm_impl enc proc_time: %u us\n", u32Old);

        if(memcmp(u8aBuf, g_u8aScrtAesCcmCipher, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "data not matched for nl_scrt_aes_ccm_impl enc\n");
            //goto done;
        }

        if(memcmp(u8aTag, g_u8aScrtAesCcmTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "tag not matched for nl_scrt_aes_ccm enc\n");
        }
        
        memset(u8aTag, 0, sizeof(u8aTag));
        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        memcpy(u8aInput, g_u8aScrtAesCcmCipher, sizeof(u8aInput));

        u32Start = SCRT_CURR_TIME;

        if(!nl_scrt_aes_ccm(0, 
                            u8aScrtAesCcmSk, u32SkLen, 
                            u8aScrtAesCcmNonce, u32NonceLen, 
                            u8aScrtAesCcmAdata, u32AdataLen, 
                            u8aBuf, u8aInput, u32DataLen, 
                            u8aScrtAesCcmTag, u32TagLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm_impl fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Old = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ccm_impl dec proc_time: %u us\n", u32Old);

        if(memcmp(u8aBuf, g_u8ScrtAesCcmPlainText, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "data not matched for nl_scrt_aes_ccm_impl dec\n");
            //goto done;
        }
        #endif
    }
    #endif //#ifdef SCRT_CMD_AES_CCM
    #ifdef SCRT_CMD_HMAC_SHA_1
    else if(dwCase == 6)
    {
        uint8_t u8aCmacSha1Sk[8] = {0};
        uint8_t u8aCmacSha1Data[20] = {0};
        uint8_t u8aOutput[20] = {0};

        uint32_t u32SkLen = 8;
        uint32_t u32DataLen = 20;
        uint32_t u32Time = 0;

        memcpy(u8aCmacSha1Sk, g_u8aCmacSha1Sk, sizeof(u8aCmacSha1Sk));
        memcpy(u8aCmacSha1Data, g_u8aCmacSha1Data, sizeof(u8aCmacSha1Data));

        memset(u8aOutput, 0, u32DataLen);

        u32Start = SCRT_CURR_TIME;

        if(!nl_hmac_sha_1(u8aCmacSha1Sk, u32SkLen, u8aCmacSha1Data, u32DataLen, u8aOutput))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1 fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1 proc_time: %u us\n", u32Time);

        if(memcmp(u8aOutput, g_u8aCmacSha1Mac, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "mac not matched for nl_hmac_sha_1\n");
            //goto done;
        }
    }
    #endif
    #ifdef SCRT_CMD_AES_ECB
    else if(dwCase == 7)
    {
        uint8_t u8aAesEcbSk[16] = {0};
        uint8_t u8aAesEcbInput[16] = {0};
        uint8_t u8aAesEcbOutput[16] = {0};
        uint32_t u32SkLen = 16;
        uint32_t u32DataLen = 16;
        uint8_t u8aBuf[64] = {0};
        uint32_t u32Time = 0;

        memcpy(u8aAesEcbSk, g_u8aAesEcbSk, sizeof(u8aAesEcbSk));
        memcpy(u8aAesEcbInput, g_u8aAesEcbInput, sizeof(u8aAesEcbInput));
        memcpy(u8aAesEcbOutput, g_u8aAesEcbOutput, sizeof(u8aAesEcbOutput));

        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        u32Start = SCRT_CURR_TIME;
        
        if(!nl_scrt_aes_ecb(1, 
                            u8aAesEcbSk, u32SkLen, 
                            u8aAesEcbInput, u8aBuf, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ecb fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ecb enc proc_time: %u us\n", u32Time);

        if(memcmp(u8aBuf, g_u8aAesEcbOutput, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "output not matched\n");
            goto done;
        }

        memset(u8aBuf, 0xEE, sizeof(u8aBuf));

        u32Start = SCRT_CURR_TIME;
        
        if(!nl_scrt_aes_ecb(0, 
                            u8aAesEcbSk, u32SkLen, 
                            u8aAesEcbOutput, u8aBuf, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ecb fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_ecb dec proc_time: %u us\n", u32Time);

        if(memcmp(u8aBuf, g_u8aAesEcbInput, u32DataLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "output not matched\n");
            goto done;
        }
    }
    #endif
    #ifdef SCRT_CMD_SHA
    else if(dwCase == 8) // SHA-1
    {
        uint8_t u8aOutput[SCRT_SHA_1_OUTPUT_LEN] = {0};
        uint32_t u32BlkSize = SCRT_SHA_1_STEP_SIZE;
        uint32_t u32Total = 64;
        uint32_t u32Offset = 0;
        uint32_t u32Input = 0;
        uint32_t u32DataLen = SCRT_SHA_1_OUTPUT_LEN;
        uint32_t u32Time = 0;
        uint8_t u8IsInterMac = 0;
        //uint8_t u8aInterMac[32] = {0};
        uint8_t u8aScrtShaData[256] = {0};
        uint8_t u8Step = 1;

        if(dwParamNum > 2)
        {
            u32Total = strtoul((char *)baParam[2], NULL, 10);
        }

        if(dwParamNum > 3)
        {
            u8Step = (uint8_t)strtoul((char *)baParam[3], NULL, 10);
        }

        tracer_cli(LOG_HIGH_LEVEL, "u32Total[%u]\n", u32Total);

        memcpy(u8aScrtShaData, g_u8aScrtShaData, u32Total);

        while(1)
        {
            if(u8Step)
            {
                if(u32Offset + u32BlkSize > u32Total)
                {
                    u32Input = u32Total - u32Offset;
                }
                else
                {
                    u32Input = u32BlkSize;
                }
            }
            else
            {
                u32Input = u32Total;
            }
            

            tracer_cli(LOG_HIGH_LEVEL, "step[%u] interM[%u] total[%u] offset[%u] input[%u]\n", u8Step, u8IsInterMac, u32Total, u32Offset, u32Input);

            u32Start = SCRT_CURR_TIME;

            if(!nl_scrt_sha(SCRT_TYPE_SHA_1, u8Step, u32Total, &(u8aScrtShaData[u32Offset]), u32Input, u8IsInterMac, u8aOutput))
            {
                tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_sha_1_step fail\n");
                goto done;
            }

            u32End = SCRT_CURR_TIME;
            u32Time = u32End - u32Start;

            if((!u8Step) || (u32Input < u32BlkSize))
            {
                tracer_cli(LOG_HIGH_LEVEL, "Output:");
            }
            else
            {
                tracer_cli(LOG_HIGH_LEVEL, "InterM:");
            }

            for(i = 0; i < u32DataLen; i++)
            {
                tracer_cli(LOG_HIGH_LEVEL, " %02x", u8aOutput[i]);
            }

            tracer_cli(LOG_HIGH_LEVEL, "\n");

            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_sha_1_step proc_time: %u us\n", u32Time);

            if(u8Step)
            {
                u8IsInterMac = 1;
            }
            else
            {
                break;
            }

            if(u32Input < u32BlkSize)
            {
                break;
            }

            u32Offset += u32Input;
        }
    }
    else if(dwCase == 9)
    {
        extern void mbedtls_sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );

        uint8_t u8aOutput[SCRT_SHA_1_OUTPUT_LEN] = {0};
        uint32_t u32DataLen = SCRT_SHA_1_OUTPUT_LEN;
        uint8_t u8aScrtShaData[256] = {0};
        uint32_t u32Total = 64;
        uint32_t u32Time = 0;

        if(dwParamNum > 2)
        {
            u32Total = strtoul((char *)baParam[2], NULL, 10);
        }

        tracer_cli(LOG_HIGH_LEVEL, "u32Total[%u]\n", u32Total);

        memcpy(u8aScrtShaData, g_u8aScrtShaData, u32Total);

        u32Start = SCRT_CURR_TIME;

        mbedtls_sha1(u8aScrtShaData, u32Total, u8aOutput);

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;

        tracer_cli(LOG_HIGH_LEVEL, "mbedtls_sha1\nOutput:", u32Total);

        for(i = 0; i < u32DataLen; i++)
        {
            tracer_cli(LOG_HIGH_LEVEL, " %02x", u8aOutput[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n");

        tracer_cli(LOG_HIGH_LEVEL, "mbedtls_sha1 proc_time: %u us\n", u32Time);
    }
    #endif //#ifdef SCRT_CMD_SHA
    #ifdef SCRT_CMD_AES_CMAC
    else if(dwCase == 20) // AES-CMAC
    {
        uint8_t u8aSk[16] = {0};
        uint8_t u8aSwOutput[16] = {0};
        uint8_t u8aHwOutput[16] = {0};
        uint8_t u8aScrtAesCmacData[128] = {0};
        uint32_t u32BufSize = sizeof(g_u8aScrtAesCmacData);
        uint32_t u32DataLen = u32BufSize;
        uint32_t u32SkLen = sizeof(u8aSk);
        uint32_t u32MacLen = 16;
        uint32_t u32Time = 0;
        //T_LeAesCmacFp fpSwAesCmac = (T_LeAesCmacFp)(0x00023401); //LeSmpUtilAesCmac_Impl

        if(dwParamNum > 2)
        {
            u32BufSize = strtoul((char *)baParam[2], NULL, 10);
            tracer_cli(LOG_HIGH_LEVEL, "BufSize[%u]\n", u32BufSize);
        }

        if(dwParamNum > 3)
        {
            u32DataLen = strtoul((char *)baParam[3], NULL, 10);
            tracer_cli(LOG_HIGH_LEVEL, "DataLen[%u]\n", u32DataLen);
        }

        memcpy(u8aSk, g_u8aScrtAesCmacSk, u32SkLen);
        memcpy(u8aScrtAesCmacData, g_u8aScrtAesCmacData, u32DataLen);

        u32Start = SCRT_CURR_TIME;

        if(LeSmpUtilAesCmac(u8aSk, &(u8aScrtAesCmacData[0]), u32DataLen, u8aSwOutput))
        {
            tracer_cli(LOG_HIGH_LEVEL, "LeSmpUtilAesCmac fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "\nLeSmpUtilAesCmac_Impl proc_time: %u us\n\n", u32Time);

        for(i = 0; i < u32MacLen; i++)
        {
            tracer_cli(LOG_HIGH_LEVEL, "%02x", u8aSwOutput[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n");


        u32Start = SCRT_CURR_TIME;

        if(!nl_scrt_aes_cmac_get(u8aSk, u32SkLen, &(u8aScrtAesCmacData[0]), u32BufSize, u32DataLen, u8aHwOutput))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_cmac fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "\nnl_aes_cmac_patch proc_time: %u us\n\n", u32Time);

        for(i = 0; i < u32MacLen; i++)
        {
            tracer_cli(LOG_HIGH_LEVEL, "%02x", u8aHwOutput[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n");

        if(memcmp(u8aHwOutput, u8aSwOutput, u32MacLen))
        {
            tracer_cli(LOG_HIGH_LEVEL, "\nFAIL: NOT Match\n\n");
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "\nSUCCESS\n\n");
        }
    }
    else if(dwCase == 21) // AES-CMAC
    {
        uint8_t u8aSk[16] = {0};
        uint8_t u8aSwOutput[16] = {0};
        uint8_t u8aHwOutput[16] = {0};
        uint8_t u8aScrtAesCmacData[128] = {0};
        uint32_t u32BufSize = sizeof(g_u8aScrtAesCmacData);
        uint32_t u32DataLen = u32BufSize;
        uint32_t u32SkLen = sizeof(u8aSk);
        uint32_t u32MacLen = 16;
        uint32_t u32Time = 0;
        //T_LeAesCmacFp fpSwAesCmac = (T_LeAesCmacFp)(0x00023401); //LeSmpUtilAesCmac_Impl

        if(dwParamNum > 2)
        {
            u32BufSize = strtoul((char *)baParam[2], NULL, 10);
            tracer_cli(LOG_HIGH_LEVEL, "BufSize[%u]\n", u32BufSize);
        }

        if(dwParamNum > 3)
        {
            u32DataLen = strtoul((char *)baParam[3], NULL, 10);
            tracer_cli(LOG_HIGH_LEVEL, "DataLen[%u]\n", u32DataLen);
        }

        memcpy(u8aSk, g_u8aScrtAesCmacSk, u32SkLen);
        memcpy(u8aScrtAesCmacData, g_u8aScrtAesCmacData, u32DataLen);

        for(i = 0; i <= u32DataLen; i++)
        {
            u32Start = SCRT_CURR_TIME;

            if(LeSmpUtilAesCmac(u8aSk, &(u8aScrtAesCmacData[0]), i, u8aSwOutput))
            {
                tracer_cli(LOG_HIGH_LEVEL, "LeSmpUtilAesCmac fail\n");
                goto done;
            }
    
            u32End = SCRT_CURR_TIME;
            u32Time = u32End - u32Start;
            tracer_cli(LOG_HIGH_LEVEL, "Input[%u] SW[%u] ", i, u32Time);
    
            u32Start = SCRT_CURR_TIME;
    
            if(!nl_scrt_aes_cmac_get(u8aSk, u32SkLen, &(u8aScrtAesCmacData[0]), u32BufSize, i, u8aHwOutput))
            {
                tracer_cli(LOG_HIGH_LEVEL, "nl_scrt_aes_cmac fail\n");
                goto done;
            }
    
            u32End = SCRT_CURR_TIME;
            u32Time = u32End - u32Start;
            tracer_cli(LOG_HIGH_LEVEL, "HW[%u]\n", u32Time);

            if(memcmp(u8aHwOutput, u8aSwOutput, u32MacLen))
            {
                tracer_cli(LOG_HIGH_LEVEL, "\nFAIL: NOT Match\n\n");
                goto done;
            }
        }

        tracer_cli(LOG_HIGH_LEVEL, "\nSUCCESS\n\n");
    }
    #endif //#ifdef SCRT_CMD_AES_CMAC
    #ifdef SCRT_CMD_HMAC_SHA_1_STEP
    else if(dwCase == 22) // HMAC-SHA-1
    {
        uint8_t u8aHmacSha1Sk[8] = {0};
        uint8_t u8aOutput[32] = {0};
        uint8_t u8ScrtHmacSha1Data[256] = {0};
        uint32_t u32Total = sizeof(u8ScrtHmacSha1Data);
        uint32_t u32SkLen = 8;
        uint32_t u32DataLen = 20;
        uint32_t u32Time = 0;

        memcpy(u8aHmacSha1Sk, g_u8aScrtHmacSha1SkStep, sizeof(u8aHmacSha1Sk));
        memcpy(u8ScrtHmacSha1Data, g_u8aScrtSha1Data, u32Total);

        u32Start = SCRT_CURR_TIME;

        if(!nl_hmac_sha_1(u8aHmacSha1Sk, u32SkLen, &(u8ScrtHmacSha1Data[0]), u32Total, u8aOutput))
        {
            tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1 fail\n");
            goto done;
        }

        u32End = SCRT_CURR_TIME;
        u32Time = u32End - u32Start;
        tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1 proc_time: %u us\n", u32Time);

        for(i = 0; i < u32DataLen; i++)
        {
            tracer_cli(LOG_HIGH_LEVEL, "%02x", u8aOutput[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n");
    }
    else if(dwCase == 23) // HMAC-SHA-1
    {
        uint8_t u8aHmacSha1Sk[8] = {0};
        uint8_t u8aOutput[32] = {0};
        uint8_t u8ScrtHmacSha1Data[256] = {0};
        uint32_t u32Total = sizeof(u8ScrtHmacSha1Data);
        uint32_t u32Offset = 0;
        uint32_t u32Input = 64;

        uint32_t u32SkLen = 8;
        uint32_t u32DataLen = 20;
        uint32_t u32Time = 0;

        memcpy(u8aHmacSha1Sk, g_u8aScrtHmacSha1SkStep, sizeof(u8aHmacSha1Sk));
        memcpy(u8ScrtHmacSha1Data, g_u8aScrtSha1Data, u32Total);

        for(u32Offset = 0; u32Offset < u32Total; )
        {
            uint8_t u8Type = 0;

            /*
            if(u32Input == 64)
            {
                u32Input = 128;
            }
            else
            {
                u32Input = 64;
            }
            */

            if(u32Offset == 0)
            {
                u8Type = 0;
            }
            else if(u32Offset + u32Input >= u32Total)
            {
                u8Type = 2;
                u32Input = u32Total - u32Offset;
            }
            else
            {
                u8Type = 1;
            }

            tracer_cli(LOG_HIGH_LEVEL, "type[%d]\n", u8Type);

            u32Start = SCRT_CURR_TIME;

            if(!nl_scrt_hmac_sha_1_step(u8Type, u32Total, u8aHmacSha1Sk, u32SkLen, &(u8ScrtHmacSha1Data[u32Offset]), u32Input, u8aOutput))
            {
                tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1 fail\n");
                goto done;
            }

            u32End = SCRT_CURR_TIME;
            u32Time = u32End - u32Start;
            tracer_cli(LOG_HIGH_LEVEL, "nl_hmac_sha_1_step proc_time: %u us\n", u32Time);

            u32Offset += u32Input;
        }
        
        for(i = 0; i < u32DataLen; i++)
        {
            tracer_cli(LOG_HIGH_LEVEL, "%02x", u8aOutput[i]);
        }

        tracer_cli(LOG_HIGH_LEVEL, "\n");
        
    }
    #endif //#ifdef SCRT_CMD_HMAC_SHA_1_STEP
    else
    {
        #ifdef SCRT_CMD_SUT_TASK
        tracer_cli(LOG_HIGH_LEVEL, "count [%u] [%u] [%u] [%u]\n", 
                   g_baScrtCnt[0], g_baScrtCnt[1], g_baScrtCnt[2], g_baScrtCnt[3]);
        tracer_cli(LOG_HIGH_LEVEL, "error [%u] [%u] [%u] [%u]\n", 
                   g_baScrtError[0], g_baScrtError[1], g_baScrtError[2], g_baScrtError[3]);
        #endif //#ifdef SCRT_CMD_SUT_TASK
    }

    tracer_cli(LOG_HIGH_LEVEL, "WaitResCnt[%u] WaitRspCnt[%u]\n\n", g_u32ScrtWaitResCnt, g_u32ScrtWaitRspCnt);

done:
    return;
}

void nl_scrt_cmd_func_init_patch(void)
{
    nl_scrt_cmd = nl_scrt_cmd_patch;
    return;
}

#endif //#ifdef SCRT_CMD_PATCH

