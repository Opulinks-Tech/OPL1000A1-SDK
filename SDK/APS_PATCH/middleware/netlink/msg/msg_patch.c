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

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "opl1000.h"
#include "msg.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "cmsis_os.h"
#include "diag_task.h"
#include "sys_os_config.h"
#include "sys_os_config_patch.h"
#include "mw_fim.h"
#include "mw_fim_default_group01_patch.h"


#define TRACER_GET_MSG_LEN


const T_TracerTaskInfoExt g_taTracerDefIntTaskInfoBody[TRACER_INT_TASK_NUM_MAX] = 
{
    {TRACER_ISR_NAME_PREFIX_PATCH,  LOG_NONE_LEVEL, 0, 0},

    {OS_TASK_NAME_DIAG,             LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_WIFI_MAC,         LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_SUPPLICANT,       LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_CONTROLLER,       LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_LE,               LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_EVENT_LOOP,       LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_TCPIP,            LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_PING,             LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_IPERF,            LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AGENT,            LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AT_WIFI_APP,      LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AT,               LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AT_TX_DATA,       LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AT_SOCKET_CLIENT, LOG_NONE_LEVEL, 0, 0},
    {OS_TASK_NAME_AT_SOCKET_SERVER, LOG_NONE_LEVEL, 0, 0},

    {"",                            LOG_NONE_LEVEL, 0, 0}, // end
};

T_TracerTaskInfoExt g_taTracerIntTaskInfoBodyExt[TRACER_INT_TASK_NUM_MAX] = {0};
T_TracerTaskInfoExt g_taTracerExtTaskInfoBodyExt[TRACER_EXT_TASK_NUM_MAX] = {
    {"demo_app1", LOG_ALL_LEVEL, 0, 0},
    {"demo_app2", LOG_ALL_LEVEL, 0, 0},
		{"",  LOG_NONE_LEVEL, 0, 0}, // end
};

extern osPoolId g_tTracerPoolId;
extern osMessageQId g_tTracerQueueId;
extern osThreadId g_tTracerThreadId;

extern uint8_t g_bTracerInit;
extern uint8_t g_bTracerNameDisplay;

#if 0
extern void tracer_proc_impl(char *sString);
extern void tracer_msg_free_impl(T_TracerMsg *ptMsg);
extern void tracer_opt_entry_add_impl(uint32_t dwHandle, uint8_t bLevel);
extern void tracer_task_name_get_impl(uint32_t dwHandle, char *baName, uint32_t dwSize);
extern void tracer_task_main_impl(void *pParam);
extern uint32_t tracer_task_handle_get_impl(uint8_t *pbIsr);
extern int tracer_level_get_impl(uint32_t dwHandle, uint8_t *pbLevel);
#endif


// internal
extern RET_DATA uint8_t g_bTracerLogMode;
extern RET_DATA int g_iTracerPriority;
extern RET_DATA uint32_t g_dwTracerStackSize;
extern RET_DATA uint32_t g_dwTracerQueueNum;
extern RET_DATA uint32_t g_dwTracerQueueSize;
extern RET_DATA uint8_t g_bTracerIntTaskNum;
extern RET_DATA uint8_t g_bTracerExtTaskNum;
extern RET_DATA uint8_t g_bTracerIntTaskDefLevel;
extern RET_DATA uint8_t g_bTracerExtTaskDefLevel;
//extern RET_DATA T_TracerTaskInfo *g_ptTracerIntTaskInfo;
//extern RET_DATA T_TracerTaskInfo *g_ptTracerExtTaskInfo;

RET_DATA T_TracerTaskInfoExt *g_ptTracerDefIntTaskInfo;
RET_DATA T_TracerTaskInfoExt *g_ptTracerIntTaskInfoExt;
RET_DATA T_TracerTaskInfoExt *g_ptTracerExtTaskInfoExt;

extern RET_DATA T_TracerProcFp tracer_proc;
extern RET_DATA T_TracerMsgFreeFp tracer_msg_free;
extern RET_DATA T_TracerOptEntryAddFp tracer_opt_entry_add;
extern RET_DATA T_TracerNameGetFp tracer_task_name_get;
extern RET_DATA T_TracerTaskMainFp tracer_task_main;
extern RET_DATA T_TracerHandleGetFp tracer_task_handle_get;
//extern RET_DATA T_TracerLevelGetFp tracer_level_get;
extern RET_DATA T_TracerStringSendFp USART_SendString;
extern RET_DATA T_TracerStringSendFp USART_SendStringII;

RET_DATA T_TracerLevelGetFp tracer_level_get_ext;
RET_DATA T_TracerCommonFp tracer_load;
RET_DATA T_TracerTaskCfgSaveFp tracer_cfg_save;
RET_DATA T_TracerTaskInfoSaveFp tracer_int_task_info_save;
RET_DATA T_TracerTaskInfoSaveFp tracer_ext_task_info_save;
RET_DATA T_TracerCommonFp tracer_cfg_reset;
RET_DATA T_TracerCommonFp tracer_int_task_reset;
RET_DATA T_TracerCommonFp tracer_ext_task_reset;

// external
extern RET_DATA T_TracerCommonFp tracer_init;
//extern RET_DATA T_TracerLogLevelSetFp tracer_log_level_set;
extern RET_DATA T_TracerOptSetFp tracer_log_mode_set;
extern RET_DATA T_TracerOptGetFp tracer_log_mode_get;
extern RET_DATA T_TracerPrioritySetFp tracer_priority_set;
extern RET_DATA T_TracerCommonFp tracer_dump;
extern RET_DATA T_TracerNameDisplayFp tracer_name_display;
extern RET_DATA T_TracerPrintfFp tracer_drct_printf;
extern RET_DATA T_TracerMsgFp tracer_msg;
//extern RET_DATA T_TracerTaskInfoGetFp tracer_task_info_get;
//extern RET_DATA T_TracerDefLevelFp tracer_def_level_set;
extern RET_DATA T_TracerCmdFp tracer_cmd;

RET_DATA T_TracerTaskInfoGetExtFp tracer_task_info_get_ext;
RET_DATA T_TracerLogLevelSetFp tracer_log_level_set_ext;


T_TracerTaskInfoExt *tracer_task_info_get_ext_patch(char *baName, T_TracerTaskInfoExt *taTaskInfo, uint8_t bTaskNum)
{
    T_TracerTaskInfoExt *ptInfo = NULL;
    uint8_t i = 0;

    for(i = 0; i < bTaskNum; i++)
    {
        if(taTaskInfo[i].baName[0])
        {
            if(!strncmp(baName, taTaskInfo[i].baName, strlen(taTaskInfo[i].baName)))
            {
                ptInfo = &(taTaskInfo[i]);
                break;
            }
        }
    }

    return ptInfo;
}

void tracer_opt_entry_add_patch(uint32_t dwHandle, uint8_t bLevel)
{
    char baName[TRACER_TASK_NAME_LEN_PATCH] = {0};
    T_TracerTaskInfoExt *ptInfo = NULL;
    uint8_t i = 0;
    int iUnusedIdx = -1;

    tracer_task_name_get(dwHandle, baName, sizeof(baName));

    ptInfo = tracer_task_info_get_ext(baName, g_ptTracerIntTaskInfoExt, g_bTracerIntTaskNum);

    if(ptInfo)
    {
        // internal task
        goto done;
    }

    for(i = 0; i < g_bTracerExtTaskNum; i++)
    {
        if(g_ptTracerExtTaskInfoExt[i].baName[0])
        {
            if(!strncmp(baName, g_ptTracerExtTaskInfoExt[i].baName, strlen(g_ptTracerExtTaskInfoExt[i].baName)))
            {
                ptInfo = &(g_ptTracerExtTaskInfoExt[i]);
                break;
            }
        }
        else
        {
            if(iUnusedIdx == -1)
            {
                iUnusedIdx = i;
            }
        }
    }

    if((!ptInfo) && (iUnusedIdx > -1))
    {
        // add external task info
        snprintf(g_ptTracerExtTaskInfoExt[iUnusedIdx].baName, sizeof(g_ptTracerExtTaskInfoExt[iUnusedIdx].baName), "%s", baName);
        g_ptTracerExtTaskInfoExt[iUnusedIdx].bLevel = bLevel;
        g_ptTracerExtTaskInfoExt[iUnusedIdx].bStatus = 0;
    }

done:
    return;
}

void tracer_task_name_get_patch(uint32_t dwHandle, char *baName, uint32_t dwSize)
{
    if((dwHandle & (~TRACER_ISR_HANDLE_MASK)) == (~TRACER_ISR_HANDLE_MASK))
    {
        // ISR
        snprintf(baName, dwSize, "%s%u", TRACER_ISR_NAME_PREFIX_PATCH, dwHandle & TRACER_ISR_HANDLE_MASK);
    }
    else
    {
        // Task
        snprintf(baName, dwSize, "%s", pcTaskGetName((TaskHandle_t)dwHandle));
    }

    return;
}

int tracer_level_get_ext_patch(uint32_t dwHandle, uint8_t *pbLevel)
{
    int iRet = -1;
    char baName[TRACER_TASK_NAME_LEN_PATCH] = {0};
    T_TracerTaskInfoExt *ptInfo = NULL;

    tracer_task_name_get(dwHandle, baName, sizeof(baName));

    ptInfo = tracer_task_info_get_ext(baName, g_ptTracerIntTaskInfoExt, g_bTracerIntTaskNum);

    if(ptInfo)
    {
        // internal task
        *pbLevel = ptInfo->bLevel;
        iRet = 0;
    }
    else
    {
        // external task
        ptInfo = tracer_task_info_get_ext(baName, g_ptTracerExtTaskInfoExt, g_bTracerExtTaskNum);

        if(ptInfo)
        {
            *pbLevel = ptInfo->bLevel;
            iRet = 0;
        }
        else
        {
            // entry not found
            *pbLevel = g_bTracerExtTaskDefLevel;
        }
    }

    return iRet;
}

void tracer_load_patch(void)
{
    uint8_t i = 0;
    T_TracerCfg tCfg = {0};
    T_TracerTaskInfoExt tInfo = {0};

    if(MwFim_FileRead(MW_FIM_IDX_GP01_TRACER_CFG, 0, MW_FIM_TRACER_CFG_SIZE, (uint8_t *)&tCfg) != MW_FIM_OK)
    {
        TRACER_DBG("[%s %d] MwFim_FileRead fail\n", __func__, __LINE__);

        if(tracer_cfg_save())
        {
            TRACER_DBG("[%s %d] tracer_cfg_save fail\n", __func__, __LINE__);
        }
    }
    else
    {
        g_bTracerLogMode = tCfg.bMode;
        g_bTracerExtTaskDefLevel = tCfg.bExtTaskDefLevel;
        g_iTracerPriority = tCfg.iPriority;
        g_dwTracerStackSize = tCfg.dwStackSize;
        g_dwTracerQueueNum = tCfg.dwQueueNum;
        g_dwTracerQueueSize = tCfg.dwQueueSize;

        g_bTracerNameDisplay = tCfg.bNameDisplay;
    }

    for(i = 0; i < g_bTracerIntTaskNum; i++)
    {
        if(MwFim_FileRead(MW_FIM_IDX_GP01_TRACER_INT_TASK_INFO, i, MW_FIM_TRACER_INT_TASK_INFO_SIZE, (uint8_t *)&tInfo) != MW_FIM_OK)
        {
            TRACER_DBG("[%s %d] MwFim_FileRead[%d] fail\n", __func__, __LINE__, i);

            if(tracer_int_task_info_save(i))
            {
                TRACER_DBG("[%s %d] tracer_int_task_info_save[%d] fail\n", __func__, __LINE__, i);
            }
            else
            {
                g_ptTracerIntTaskInfoExt[i].bStatus = 1;
            }
        }
        else
        {
            snprintf(g_ptTracerIntTaskInfoExt[i].baName, sizeof(g_ptTracerIntTaskInfoExt[i].baName), "%s", tInfo.baName);
            g_ptTracerIntTaskInfoExt[i].bLevel = tInfo.bLevel;
            g_ptTracerIntTaskInfoExt[i].bStatus = 1;
        }
    }

    for(i = 0; i < g_bTracerExtTaskNum; i++)
    {
        if(MwFim_FileRead(MW_FIM_IDX_GP01_TRACER_EXT_TASK_INFO, i, MW_FIM_TRACER_EXT_TASK_INFO_SIZE, (uint8_t *)&tInfo) != MW_FIM_OK)
        {
            TRACER_DBG("[%s %d] MwFim_FileRead[%d] fail\n", __func__, __LINE__, i);

            if(tracer_ext_task_info_save(i))
            {
                TRACER_DBG("[%s %d] tracer_ext_task_info_save[%d] fail\n", __func__, __LINE__, i);
            }
            else
            {
                g_ptTracerExtTaskInfoExt[i].bStatus = 1;
            }
        }
        else
        {
            snprintf(g_ptTracerExtTaskInfoExt[i].baName, sizeof(g_ptTracerExtTaskInfoExt[i].baName), "%s", tInfo.baName);
            g_ptTracerExtTaskInfoExt[i].bLevel = tInfo.bLevel;
            g_ptTracerExtTaskInfoExt[i].bStatus = 1;
        }
    }

    return;
}

void tracer_init_patch(void)
{
    osPoolDef_t tPoolDef = {0};
    osThreadDef_t tThreadDef = {0};
    osMessageQDef_t tQueueDef = {0};

    if(g_bTracerInit)
    {
        TRACER_DBG("[%s %d] Tracer already init\n", __func__, __LINE__);
        goto done;
    }

    tPoolDef.pool_sz = g_dwTracerQueueNum;
    tPoolDef.item_sz = sizeof(T_TracerMsg);

    //create memory pool
    if(g_tTracerPoolId == NULL)
    {
        g_tTracerPoolId = osPoolCreate(&tPoolDef);
    
        if(g_tTracerPoolId == NULL)
        {
            TRACER_DBG("[%s %d] osPoolCreate fail\n", __func__, __LINE__);
            goto done;
        }
    }

    //create message queue
    tQueueDef.item_sz = sizeof(T_TracerMsg);
    tQueueDef.queue_sz = g_dwTracerQueueNum;

    if(g_tTracerQueueId == NULL)
    {
        g_tTracerQueueId = osMessageCreate(&tQueueDef, NULL);
    
        if(g_tTracerQueueId == NULL)
        {
            TRACER_DBG("[%s %d] osMessageCreate fail\n", __func__, __LINE__);
            goto done;
        }
    }

    //create task
    tThreadDef.name = OS_TASK_NAME_TRACER;
    tThreadDef.stacksize = g_dwTracerStackSize;
    tThreadDef.tpriority = (osPriority)g_iTracerPriority;
    tThreadDef.pthread = tracer_task_main;

    g_tTracerThreadId = osThreadCreate(&tThreadDef, (void *)&g_tTracerThreadId);

    if(g_tTracerThreadId == NULL)
    {
        TRACER_DBG("[%s %d] osThreadCreate fail\n", __func__, __LINE__);
    }
    
    g_bTracerInit = 1;

done:
    return;
}

int tracer_msg_patch(uint8_t bType, uint8_t bLevel, char *sFmt, ...)
{
    int iRet = -1;
    va_list tList;
    T_TracerMsg *ptMsg = NULL;
    int iBufSize = 0;
    uint8_t bListUsed = 0;
    uint32_t dwHandle = 0;
    uint8_t bIsr = 0;
    uint8_t bAddOpt = 0;
    uint8_t bTaskLevel = 0;
    uint8_t bTruncated = 0;

    #ifdef TRACER_GET_MSG_LEN
    char baTmp[4] = {0};
    #else
    char baTmp[TRACER_MSG_MAX_SIZE] = {0};
    #endif

    if(!g_bTracerInit)
    {
        //TRACER_DBG("[%s %d] Tracer not yet init\n", __func__, __LINE__);

        va_start(tList, sFmt);
        vprintf(sFmt, tList);
        va_end(tList);

        goto done;
    }

    if(g_bTracerLogMode == TRACER_MODE_DISABLE)
    {
        goto done;
    }

    dwHandle = tracer_task_handle_get(&bIsr);

    if(bType == TRACER_TYPE_LOG)
    {
        uint8_t bProcLog = 0;

        if(tracer_level_get_ext(dwHandle, &bTaskLevel))
        {
            bAddOpt = 1; // process opt only
        }

    	if(bLevel & bTaskLevel)
        {
            bProcLog = 1; // process log and opt
            bAddOpt = 0;
        }

        switch(g_bTracerLogMode)
        {
            case TRACER_MODE_NORMAL:
                break;
    
            case TRACER_MODE_DRCT:
                if(bProcLog)
                {
                    bProcLog = 0; // process immediately
    
                    va_start(tList, sFmt);
                    vprintf(sFmt, tList);
                    va_end(tList);
                }
    
                break;
    
            default:
                TRACER_DBG("[%s %d] invalid mode[%d]\n", __func__, __LINE__, g_bTracerLogMode);
                goto done;
        }

        if(!bProcLog)
        {
            if(!bAddOpt)
            {
                goto done;
            }
        }
    }
    else if(bType == TRACER_TYPE_CLI)
    {
        if(g_bTracerLogMode == TRACER_MODE_DRCT)
        {
            va_start(tList, sFmt);
            vprintf(sFmt, tList);
            va_end(tList);

            goto done;
        }
    }
    else
    {
        TRACER_DBG("[%s %d] invalid type[%d]\n", __func__, __LINE__, bType);
        goto done;
    }

    ptMsg = (T_TracerMsg *)osPoolCAlloc(g_tTracerPoolId);

    if(ptMsg == NULL)
    {
        TRACER_DBG("[%s %d] osPoolCAlloc fail\n", __func__, __LINE__);
        goto done;
    }

    if(bAddOpt)
    {
        ptMsg->ptCb = (T_TracerCb *)malloc(sizeof(T_TracerCb));
    
        if(ptMsg->ptCb == NULL)
        {
            TRACER_DBG("[%s %d] malloc fail\n", __func__, __LINE__);
            goto done;
        }

        ptMsg->ptCb->tInfo.bType = TRACER_TYPE_OPT_ADD;
        ptMsg->ptCb->tInfo.bLevel = bTaskLevel;
        ptMsg->ptCb->tInfo.dwHandle = dwHandle;
    }
    else
    {
        va_start(tList, sFmt);
        bListUsed = 1;
    
        #ifdef TRACER_GET_MSG_LEN
        // With C99, it should return total length of string without writing anything.
        // But on A0 FPGA, it will write string to address 0x00000000! Something wrong!
        //iBufSize = vsnprintf(NULL, 0, sFmt, tList);

        // With C99, it will write one byte ('\0') to baTmp and return total length of string.
        iBufSize = vsnprintf(baTmp, 1, sFmt, tList);
        #else
        iBufSize = vsnprintf(baTmp, TRACER_MSG_MAX_SIZE, sFmt, tList);
        #endif
    
        if(iBufSize <= 0)
        {
            //TRACER_DBG("[%s %d] vsnprintf fail\n", __func__, __LINE__);
            goto done;
        }
        
        //iBufSize += 1; // for '\0'

        if(iBufSize > g_dwTracerQueueSize)
        {
            iBufSize = g_dwTracerQueueSize;
            bTruncated = 1;
        }
    
        ptMsg->ptCb = (T_TracerCb *)malloc(sizeof(T_TracerCb) + iBufSize);
    
        if(ptMsg->ptCb == NULL)
        {
            TRACER_DBG("[%s %d] malloc fail\n", __func__, __LINE__);
            goto done;
        }

        // exact length of baBuf[]: 4 + iBufSize
    
        #ifdef TRACER_GET_MSG_LEN
        vsnprintf(ptMsg->ptCb->baBuf, iBufSize + 1, sFmt, tList);
        #else
        snprintf(ptMsg->ptCb->baBuf, iBufSize + 1, "%s", baTmp);
        #endif

        if(bTruncated)
        {
            ptMsg->ptCb->baBuf[iBufSize] = '\n';
            ptMsg->ptCb->baBuf[iBufSize + 1] = 0;
        }
    
        ptMsg->ptCb->tInfo.bType = bType;
        ptMsg->ptCb->tInfo.bLevel = bTaskLevel;
        ptMsg->ptCb->tInfo.dwHandle = dwHandle;
    }

    if(osMessagePut(g_tTracerQueueId, (uint32_t)ptMsg, 0) != osOK)
    {
        TRACER_DBG("[%s %d] osMessagePut fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    if(bListUsed)
    {
        va_end(tList);
    }

    if(iRet)
    {
        tracer_msg_free(ptMsg);
    }

    return iRet;
}

int tracer_log_level_set_ext_patch(uint8_t bIdx, uint8_t bLevel)
{
    if(bIdx == TRACER_TASK_IDX_MAX)
    {
        uint8_t i = 0;

        for(i = 0; i < g_bTracerIntTaskNum; i++)
        {
            if(bLevel != g_ptTracerIntTaskInfoExt[i].bLevel)
            {
                g_ptTracerIntTaskInfoExt[i].bLevel = bLevel;
    
                if(tracer_int_task_info_save(i))
                {
                    TRACER_DBG("[%s %d] tracer_int_task_info_save[%d] fail\n", __func__, __LINE__, i);
                }
            }
        }
    
        for(i = 0; i < g_bTracerExtTaskNum; i++)
        {
            if(bLevel != g_ptTracerExtTaskInfoExt[i].bLevel)
            {
                g_ptTracerExtTaskInfoExt[i].bLevel = bLevel;
    
                if(tracer_ext_task_info_save(i))
                {
                    TRACER_DBG("[%s %d] tracer_ext_task_info_save[%d] fail\n", __func__, __LINE__, i);
                }
            }
        }
    }
    else
    {
        if(bIdx < g_bTracerIntTaskNum)
        {
            // internal task
            if(bLevel != g_ptTracerIntTaskInfoExt[bIdx].bLevel)
            {
                g_ptTracerIntTaskInfoExt[bIdx].bLevel = bLevel;
    
                if(tracer_int_task_info_save(bIdx))
                {
                    TRACER_DBG("[%s %d] tracer_int_task_info_save[%d] fail\n", __func__, __LINE__, bIdx);
                }
            }
        }
        else
        {
            // external task
            uint8_t bExtIdx = bIdx - g_bTracerIntTaskNum;

            if(bLevel != g_ptTracerExtTaskInfoExt[bExtIdx].bLevel)
            {
                g_ptTracerExtTaskInfoExt[bExtIdx].bLevel = bLevel;
    
                if(tracer_ext_task_info_save(bExtIdx))
                {
                    TRACER_DBG("[%s %d] tracer_ext_task_info_save[%d] fail\n", __func__, __LINE__, bExtIdx);
                }
            }
        }
    }

    return 0;
}

int tracer_cfg_save_patch(void)
{
    int iRet = -1;
    T_TracerCfg tCfg = {0};

    tCfg.bMode = g_bTracerLogMode;
    tCfg.bExtTaskDefLevel = g_bTracerExtTaskDefLevel;
    tCfg.iPriority = g_iTracerPriority;
    tCfg.dwStackSize = g_dwTracerStackSize;
    tCfg.dwQueueNum = g_dwTracerQueueNum;
    tCfg.dwQueueSize = g_dwTracerQueueSize;

    tCfg.bNameDisplay = g_bTracerNameDisplay;

    if(MwFim_FileWrite(MW_FIM_IDX_GP01_TRACER_CFG, 0, MW_FIM_TRACER_CFG_SIZE, (uint8_t *)&tCfg) != MW_FIM_OK)
    {
        TRACER_DBG("[%s %d] MwFim_FileWrite fail\n", __func__, __LINE__);
        goto done;
    }

    iRet = 0;

done:
    return iRet;
}

int tracer_int_task_info_save_patch(uint8_t bIdx)
{
    int iRet = -1;

    if(MwFim_FileWrite(MW_FIM_IDX_GP01_TRACER_INT_TASK_INFO, bIdx, MW_FIM_TRACER_INT_TASK_INFO_SIZE, (uint8_t *)&(g_ptTracerIntTaskInfoExt[bIdx])) != MW_FIM_OK)
    {
        TRACER_DBG("[%s %d] MwFim_FileWrite[%d] fail\n", __func__, __LINE__, bIdx);
        goto done;
    }

    g_ptTracerIntTaskInfoExt[bIdx].bStatus = 1;

    iRet = 0;

done:
    return iRet;
}

int tracer_ext_task_info_save_patch(uint8_t bIdx)
{
    int iRet = -1;

    if(MwFim_FileWrite(MW_FIM_IDX_GP01_TRACER_EXT_TASK_INFO, bIdx, MW_FIM_TRACER_EXT_TASK_INFO_SIZE, (uint8_t *)&(g_ptTracerExtTaskInfoExt[bIdx])) != MW_FIM_OK)
    {
        TRACER_DBG("[%s %d] MwFim_FileWrite[%d] fail\n", __func__, __LINE__, bIdx);
        goto done;
    }

    g_ptTracerExtTaskInfoExt[bIdx].bStatus = 1;

    iRet = 0;

done:
    return iRet;
}

void tracer_log_mode_set_patch(uint8_t bMode)
{
    if(!g_bTracerInit)
    {
        TRACER_DBG("[%s %d] Tracer not init yet\n", __func__, __LINE__);
        //goto done;
    }

    if(bMode < TRACER_MODE_MAX)
    {
        if(bMode != g_bTracerLogMode)
        {
            g_bTracerLogMode = bMode;

            if(tracer_cfg_save())
            {
                TRACER_DBG("[%s %d] tracer_cfg_save fail\n", __func__, __LINE__);
            }
        }
    }

//done:
    return;
}

void tracer_priority_set_patch(int iPriority)
{
    if(g_tTracerThreadId != NULL)
    {
        if(iPriority != g_iTracerPriority)
        {
            g_iTracerPriority = iPriority;
    
            if(tracer_cfg_save())
            {
                TRACER_DBG("[%s %d] tracer_cfg_save fail\n", __func__, __LINE__);
            }

            if(osThreadSetPriority(g_tTracerThreadId, (osPriority)iPriority) != osOK)
            {
                TRACER_DBG("[%s %d] osThreadSetPriority fail\n", __func__, __LINE__);
            }
        }
    }
    else
    {
        TRACER_DBG("[%s %d] g_tTracerThreadId is NULL\n", __func__, __LINE__);
    }

    return;
}

void tracer_dump_patch(void)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tracer_cli(LOG_HIGH_LEVEL, "\nTracer Mode       [%d]\t0:disable/1:normal/2:print directly\n", g_bTracerLogMode);
    tracer_cli(LOG_HIGH_LEVEL, "Display Task Name [%d]\t0:disable/1:enable\n", g_bTracerNameDisplay);
    tracer_cli(LOG_HIGH_LEVEL, "Priority          [%d]\tosPriorityIdle(%d) ~ osPriorityRealtime(%d)\n", g_iTracerPriority, osPriorityIdle, osPriorityRealtime);
    tracer_cli(LOG_HIGH_LEVEL, "StackSize         [%u]\tnumber of uint_32\n", g_dwTracerStackSize);
    tracer_cli(LOG_HIGH_LEVEL, "Queue Number      [%u]\tmax number of log\n", g_dwTracerQueueNum);
    tracer_cli(LOG_HIGH_LEVEL, "Queue Size        [%u]\tmax length of log\n", g_dwTracerQueueSize);
    tracer_cli(LOG_HIGH_LEVEL, "Log Level         [0x00:None/0x01:Low/0x02:Med/0x04:High/0x07:All]\n", g_bTracerExtTaskDefLevel);
    tracer_cli(LOG_HIGH_LEVEL, "\nDefault Level for App Tasks\t[0x%02X]\n", g_bTracerExtTaskDefLevel);

    tracer_cli(LOG_HIGH_LEVEL, "\n%4s %20s: %s\n", "Index", "Name", "Level");

    tracer_cli(LOG_HIGH_LEVEL, "---------------------------------- Internal Tasks (Start from Index 0)\n");

    for(i = 0; i < g_bTracerIntTaskNum; i++)
    {
        if(g_ptTracerIntTaskInfoExt[i].baName[0])
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%2d]  %20s: 0x%02X\n", i, g_ptTracerIntTaskInfoExt[i].baName, g_ptTracerIntTaskInfoExt[i].bLevel);
        }
    }

    tracer_cli(LOG_HIGH_LEVEL, "---------------------------------- App Tasks (Start from Index %d)\n", g_bTracerIntTaskNum);

    j = g_bTracerIntTaskNum;

    for(i = 0; i < g_bTracerExtTaskNum; i++)
    {
        if(g_ptTracerExtTaskInfoExt[i].baName[0])
        {
            tracer_cli(LOG_HIGH_LEVEL, "[%2d]  %20s: 0x%02X", j, g_ptTracerExtTaskInfoExt[i].baName, g_ptTracerExtTaskInfoExt[i].bLevel);

            if(!(g_ptTracerExtTaskInfoExt[i].bStatus))
            {
                tracer_cli(LOG_HIGH_LEVEL, "  not saved yet");
            }
    
            tracer_cli(LOG_HIGH_LEVEL, "\n");
        }
        
        ++j;
    }

    tracer_cli(LOG_HIGH_LEVEL, "\n\n");
    
    return;
}

int tracer_def_level_set_patch(uint8_t bType, uint8_t bLevel)
{
    int iRet = -1;

    if(bType >= TRACER_TASK_TYPE_MAX)
    {
        goto done;
    }

    if(bType == TRACER_TASK_TYPE_INTERNAL)
    {
        // ignore for internal task
        goto done;
    }
    else if(bType == TRACER_TASK_TYPE_APP)
    {
        // for external task
        if(bLevel != g_bTracerExtTaskDefLevel)
        {
            g_bTracerExtTaskDefLevel = bLevel;

            if(tracer_cfg_save())
            {
                TRACER_DBG("[%s %d] tracer_cfg_save fail\n", __func__, __LINE__);
            }
        }
    }

    iRet = 0;

done:
    return iRet;
}

void tracer_cmd_patch(char *sCmd)
{
    char *baParam[8] = {0};
    uint32_t dwNum = 8;
    uint32_t dwParamNum = 0;
    uint8_t i = 0;

    dwParamNum = ParseParam(sCmd, baParam, dwNum);

    tracer_cli(LOG_HIGH_LEVEL, "\n");

    if(!strcmp(baParam[1], "level"))
    {
        if(dwParamNum != 4)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Usage: tracer level <task_index> <task_level:hex>\n");
            tracer_cli(LOG_HIGH_LEVEL, "\ttask_index: 0 ~ %d. Set %d to apply level to all tasks\n", TRACER_TASK_NUM_MAX - 1, TRACER_TASK_IDX_MAX);
            tracer_cli(LOG_HIGH_LEVEL, "\ttask_level: 0x00 ~ 0xFF\n");
            goto done;
        }
        else
        {
            uint8_t bIdx = (uint8_t)strtoul(baParam[2], NULL, 10);
            uint8_t bLevel = (uint8_t)strtoul(baParam[3], NULL, 16);

            if(tracer_log_level_set_ext(bIdx, bLevel))
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_log_level_set_ext fail\n");
                goto done;
            }

            tracer_cli(LOG_HIGH_LEVEL, "Set level of task[%d] to [%02x]\n", bIdx, bLevel);
        }
    }
    else if(!strcmp(baParam[1], "disp_name"))
    {
        uint8_t bDisplay = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "Usage: tracer disp_name <0:not display/1:display>\n");
            goto done;
        }

        bDisplay = (uint8_t)strtoul(baParam[2], NULL, 10);

        if(bDisplay)
        {
            bDisplay = 1;
        }

        tracer_name_display(bDisplay);

        if(bDisplay)
        {
            tracer_cli(LOG_HIGH_LEVEL, "enable task name display\n");
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "disable task name display\n");
        }
    }
    else if(!strcmp(baParam[1], "mode"))
    {
        uint8_t bMode = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "log mode[%u]\n", tracer_log_mode_get());
            goto done;
        }

        bMode = (uint8_t)strtoul(baParam[2], NULL, 10);
        tracer_log_mode_set(bMode);

        tracer_cli(LOG_HIGH_LEVEL, "set mode to [%d]\n", bMode);
    }
    else if(!strcmp(baParam[1], "def_level"))
    {
        uint8_t bLevel = 0;
        uint8_t bType = 0;

        if(dwParamNum < 4)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        bType = (uint8_t)strtoul(baParam[2], NULL, 10);
        bLevel = (uint8_t)strtoul(baParam[3], NULL, 16);
        tracer_def_level_set(bType, bLevel);

        tracer_cli(LOG_HIGH_LEVEL, "set default level to [%d]\n", bLevel);
    }
    else if(!strcmp(baParam[1], "pri"))
    {
        int iPriority = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        iPriority = atoi(baParam[2]);

        if((iPriority < osPriorityIdle) || (iPriority > osPriorityRealtime))
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid priority[%d]\n", iPriority);
            goto done;
        }

        tracer_priority_set(iPriority);
        tracer_cli(LOG_HIGH_LEVEL, "set priority to [%d]\n", iPriority);
    }
    else if(!strcmp(baParam[1], "stack"))
    {
        uint32_t dwStackSize = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        dwStackSize = strtoul(baParam[2], NULL, 10);

        if(dwStackSize < TRACER_TASK_STACK_SIZE_MIN)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid stack size[%u]\n", dwStackSize);
            goto done;
        }

        if(dwStackSize != g_dwTracerStackSize)
        {
            g_dwTracerStackSize = dwStackSize;
    
            if(tracer_cfg_save())
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
            }
        }

        tracer_cli(LOG_HIGH_LEVEL, "set stacksize to [%u]\n", g_dwTracerStackSize);
    }
    else if(!strcmp(baParam[1], "qnum"))
    {
        uint32_t dwNum = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        dwNum = strtoul(baParam[2], NULL, 10);

        if(!dwNum)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid queue number[%u]\n", dwNum);
            goto done;
        }

        if(dwNum != g_dwTracerQueueNum)
        {
            g_dwTracerQueueNum = dwNum;
    
            if(tracer_cfg_save())
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
            }
        }

        tracer_cli(LOG_HIGH_LEVEL, "set queue number to [%u]\n", g_dwTracerQueueNum);
    }
    else if(!strcmp(baParam[1], "qsize"))
    {
        uint32_t dwSize = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        dwSize = strtoul(baParam[2], NULL, 10);

        if(!dwSize)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid queue size[%u]\n", dwSize);
            goto done;
        }

        #ifdef TRACER_GET_MSG_LEN
        #else
        if(dwSize >= TRACER_MSG_MAX_SIZE)
        {
            dwSize = TRACER_MSG_MAX_SIZE - 1;
        }
        #endif

        if(dwSize != g_dwTracerQueueSize)
        {
            g_dwTracerQueueSize = dwSize;
    
            if(tracer_cfg_save())
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
            }
        }

        tracer_cli(LOG_HIGH_LEVEL, "set queue size to [%u]\n", g_dwTracerQueueSize);
    }
    else if(!strcmp(baParam[1], "app_level"))
    {
        uint8_t bIdx = 0;
        uint8_t bLevel = 0;
        uint8_t bExtIdx = 0;
        char *sName = NULL;

        if(dwParamNum < 5)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        bIdx = (uint8_t)strtoul(baParam[2], NULL, 10);

        if((bIdx < g_bTracerIntTaskNum) || (bIdx >= g_bTracerIntTaskNum + g_bTracerExtTaskNum))
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid index[%d] for app task\n", bIdx);
            goto done;
        }

        bLevel = (uint8_t)strtoul(baParam[4], NULL, 16);

        bExtIdx = bIdx - g_bTracerIntTaskNum;

        sName = baParam[3];

        if((sName[0] == '0') && (sName[1] == 0))
        {
            // clear entry
            g_ptTracerExtTaskInfoExt[bExtIdx].baName[0] = 0;
            bLevel = 0;
        }
        else
        {
            for(i = 0; i < g_bTracerIntTaskNum; i++)
            {
                if(g_ptTracerIntTaskInfoExt[i].baName[0])
                {
                    if(!strncmp(sName, g_ptTracerIntTaskInfoExt[i].baName, strlen(g_ptTracerIntTaskInfoExt[i].baName)))
                    {
                        tracer_cli(LOG_HIGH_LEVEL, "invalid name[%s] for app task\n", sName);
                        goto done;
                    }
                }
            }

            snprintf(g_ptTracerExtTaskInfoExt[bExtIdx].baName, sizeof(g_ptTracerExtTaskInfoExt[bExtIdx].baName), "%s", sName);
        }

        g_ptTracerExtTaskInfoExt[bExtIdx].bLevel = bLevel;

        if(tracer_ext_task_info_save(bExtIdx))
        {
            TRACER_DBG("[%s %d] tracer_ext_task_info_save[%d] fail\n", __func__, __LINE__, bExtIdx);
        }

        tracer_cli(LOG_HIGH_LEVEL, "set entry[%d] for app task\n", bIdx);
    }
    else if(!strcmp(baParam[1], "save"))
    {
        if(tracer_cfg_save())
        {
            tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
        }

        for(i = 0; i < g_bTracerIntTaskNum; i++)
        {
            if(tracer_int_task_info_save(i))
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_int_task_info_save[%d] fail\n", i);
            }
        }

        for(i = 0; i < g_bTracerExtTaskNum; i++)
        {
            if(tracer_ext_task_info_save(i))
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_ext_task_info_save[%d] fail\n", i);
            }
        }

        tracer_cli(LOG_HIGH_LEVEL, "done\n");
    }
    else if(!strcmp(baParam[1], "default"))
    {
        uint32_t dwType = 0;

        if(dwParamNum < 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        dwType = strtoul(baParam[2], NULL, 10);

        if(dwType > 3)
        {
            tracer_cli(LOG_HIGH_LEVEL, "invalid parameter\n");
            goto done;
        }

        switch(dwType)
        {
        case 0:
            tracer_cfg_reset();

            if(tracer_cfg_save())
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
            }

            tracer_int_task_reset();

            for(i = 0; i < g_bTracerIntTaskNum; i++)
            {
                if(tracer_int_task_info_save(i))
                {
                    tracer_cli(LOG_HIGH_LEVEL, "tracer_int_task_info_save[%d] fail\n", i);
                }
            }
            
            tracer_ext_task_reset();

            for(i = 0; i < g_bTracerExtTaskNum; i++)
            {
                if(tracer_ext_task_info_save(i))
                {
                    tracer_cli(LOG_HIGH_LEVEL, "tracer_ext_task_info_save[%d] fail\n", i);
                }
            }
            
            tracer_cli(LOG_HIGH_LEVEL, "reset all\n");
            break;

        case 1:
            tracer_cfg_reset();

            if(tracer_cfg_save())
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_cfg_save fail\n");
            }

            tracer_cli(LOG_HIGH_LEVEL, "reset tracer cfg\n");
            break;

        case 2:
            tracer_int_task_reset();

            for(i = 0; i < g_bTracerIntTaskNum; i++)
            {
                if(tracer_int_task_info_save(i))
                {
                    tracer_cli(LOG_HIGH_LEVEL, "tracer_int_task_info_save[%d] fail\n", i);
                }
            }

            tracer_cli(LOG_HIGH_LEVEL, "reset internal tasks\n");
            break;

        case 3:
            tracer_ext_task_reset();

            for(i = 0; i < g_bTracerExtTaskNum; i++)
            {
                if(tracer_ext_task_info_save(i))
                {
                    tracer_cli(LOG_HIGH_LEVEL, "tracer_ext_task_info_save[%d] fail\n", i);
                }
            }

            tracer_cli(LOG_HIGH_LEVEL, "reset external tasks\n");
            break;

        default:
            tracer_cli(LOG_HIGH_LEVEL, "unknown type[%u]\n", dwType);
            goto done;
        }
    }
    else if(!strcmp(baParam[1], "cmd"))
    {
        tracer_cli(LOG_HIGH_LEVEL, "Tracer Command List:\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer mode <0:disable/1:normal/2:print directly>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer def_level <1:app tasks> <level:hex>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer level <task_index> <level:hex>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer app_level <index:%d ~ %d> <name:0 for empty string> <level:hex>\n", g_bTracerIntTaskNum, g_bTracerIntTaskNum + g_bTracerExtTaskNum - 1);
        tracer_cli(LOG_HIGH_LEVEL, "tracer save\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer default <0:reset all/1:reset config/2:reset internal tasks/3:reset external tasks>\n");

        tracer_cli(LOG_HIGH_LEVEL, "tracer pri <osPriority:%d ~ %d>\n", osPriorityIdle, osPriorityRealtime);
        tracer_cli(LOG_HIGH_LEVEL, "tracer stack <number of uint32>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer qnum <queue number>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer qsize <queue size>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer disp_name <0:disable/1:enable>\n");
    }
    #ifdef TRACER_SUT
    else if(!strcmp(baParam[1], "sut"))
    {
        uint8_t bEnableCliAt = 0;

        if(dwParamNum > 2)
        {
            bEnableCliAt = (uint8_t)strtoul(baParam[2], NULL, 10);
        }

        tracer_cli(LOG_HIGH_LEVEL, "Toggle tracer sut task\n");
        tracer_sut_task_create(bEnableCliAt);
    }
    #endif //#ifdef TRACER_SUT
    else
    {
        tracer_dump();
    }

done:
    return;
}

void tracer_name_display_patch(uint8_t bDisplay)
{
    if(bDisplay != g_bTracerNameDisplay)
    {
        g_bTracerNameDisplay = bDisplay;

        if(tracer_cfg_save())
        {
            TRACER_DBG("[%s %d] tracer_cfg_save fail\n", __func__, __LINE__);
        }
    }

    return;
}

void tracer_cfg_reset_patch(void)
{
    g_bTracerExtTaskDefLevel = LOG_ALL_LEVEL;

    g_bTracerLogMode = TRACER_MODE_NORMAL;
    g_iTracerPriority = OS_TASK_PRIORITY_TRACER;
    g_dwTracerStackSize = OS_TASK_STACK_SIZE_TRACER_PATCH;
    g_dwTracerQueueNum = TRACER_QUEUE_NUM_PATCH;
    g_dwTracerQueueSize = TRACER_QUEUE_SIZE_PATCH;

    g_bTracerNameDisplay = 0;
    return;
}

void tracer_int_task_reset_patch(void)
{
    memcpy(g_ptTracerIntTaskInfoExt, g_ptTracerDefIntTaskInfo, sizeof(T_TracerTaskInfoExt) * g_bTracerIntTaskNum);
    return;
}

void tracer_ext_task_reset_patch(void)
{
    memset(g_ptTracerExtTaskInfoExt, 0, sizeof(T_TracerTaskInfoExt) * g_bTracerExtTaskNum);
    return;
}

void Tracer_PatchInit(void)
{
    // internal
    /*
    g_bTracerIntTaskNum = TRACER_INT_TASK_NUM_MAX;
    g_bTracerExtTaskNum = TRACER_EXT_TASK_NUM_MAX;
    g_bTracerIntTaskDefLevel = LOG_NONE_LEVEL;
    g_bTracerExtTaskDefLevel = LOG_ALL_LEVEL;
    g_ptTracerIntTaskInfo = g_taTracerIntTaskInfoBody;
    g_ptTracerExtTaskInfo = g_taTracerExtTaskInfoBody;

    g_bTracerLogMode = TRACER_MODE_NORMAL;
    g_iTracerPriority = TRACER_TASK_PRIORITY;
    g_dwTracerStackSize = TRACER_TASK_STACK_SIZE;
    g_dwTracerQueueNum = TRACER_QUEUE_NUM;
    g_dwTracerQueueSize = TRACER_QUEUE_SIZE;
    */

    g_ptTracerDefIntTaskInfo = (T_TracerTaskInfoExt *)g_taTracerDefIntTaskInfoBody;
    g_ptTracerIntTaskInfoExt = g_taTracerIntTaskInfoBodyExt;
    g_ptTracerExtTaskInfoExt = g_taTracerExtTaskInfoBodyExt;

    /*
    tracer_proc = tracer_proc_impl;
    tracer_msg_free = tracer_msg_free_impl;
    */
    tracer_opt_entry_add = tracer_opt_entry_add_patch;
    /*
    tracer_task_name_get = tracer_task_name_get_impl;
    tracer_task_main = tracer_task_main_impl;
    tracer_task_handle_get = tracer_task_handle_get_impl;
    tracer_level_get = tracer_level_get_impl;
    tracer_task_info_get = tracer_task_info_get_impl;
    USART_SendString = USART_SendString_Impl;
    USART_SendStringII = USART_SendStringII_Impl;
    */

    tracer_level_get_ext = tracer_level_get_ext_patch;
    tracer_task_info_get_ext = tracer_task_info_get_ext_patch;
    tracer_load = tracer_load_patch;
    tracer_cfg_save = tracer_cfg_save_patch;
    tracer_int_task_info_save = tracer_int_task_info_save_patch;
    tracer_ext_task_info_save = tracer_ext_task_info_save_patch;
    tracer_cfg_reset = tracer_cfg_reset_patch;
    tracer_int_task_reset = tracer_int_task_reset_patch;
    tracer_ext_task_reset = tracer_ext_task_reset_patch;
    
    // external
    tracer_init = tracer_init_patch;
    //tracer_log_level_set = tracer_log_level_set_impl;
    tracer_log_mode_set = tracer_log_mode_set_patch;
    //tracer_log_mode_get = tracer_log_mode_get_impl;
    tracer_priority_set = tracer_priority_set_patch;
    tracer_dump = tracer_dump_patch;
    tracer_name_display = tracer_name_display_patch;
    //tracer_drct_printf = tracer_drct_printf_impl;
    tracer_msg = tracer_msg_patch;
    tracer_def_level_set = tracer_def_level_set_patch;
    tracer_cmd = tracer_cmd_patch;

    //msg_printII = msg_printII_impl;

    tracer_log_level_set_ext = tracer_log_level_set_ext_patch;

    tracer_cfg_reset();
    tracer_int_task_reset();
    tracer_ext_task_reset();
    return;
}

