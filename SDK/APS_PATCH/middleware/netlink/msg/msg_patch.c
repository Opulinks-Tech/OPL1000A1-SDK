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
#include "nl1000.h"
#include "msg.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "cmsis_os.h"
#include "msg_patch.h"
#include "diag_task_patch.h"
#include "sys_os_config.h"


#define TRACER_TASK_PRIORITY        OS_TASK_PRIORITY_TRACER
#define TRACER_TASK_STACK_SIZE      OS_TASK_STACK_SIZE_TRACER // number of uint32_t


T_TracerTaskInfo g_taTracerIntTaskInfoBody[TRACER_INT_TASK_NUM_MAX] = 
{
    // ROM
    {"diag",            LOG_NONE_LEVEL},
    {"wifi_mac",        LOG_NONE_LEVEL},
    {"supplicant",      LOG_NONE_LEVEL},
    {"controller_task", LOG_NONE_LEVEL},
    {"le",              LOG_NONE_LEVEL},
    {"event_loop",      LOG_NONE_LEVEL},
    {"tcpip_thread",    LOG_NONE_LEVEL},
    {"ping_thread",     LOG_NONE_LEVEL},
    {"iperf",           LOG_NONE_LEVEL},
    {TRACER_ISR_NAME_PREFIX,    LOG_NONE_LEVEL},
    //{"tracer",        LOG_NONE_LEVEL},

    // Patch
    {"at_wifi_app",     LOG_NONE_LEVEL},
    {"AT",              LOG_NONE_LEVEL},
    {"at_tx_data",      LOG_NONE_LEVEL},
    {"socket",          LOG_NONE_LEVEL},
    {"server",          LOG_NONE_LEVEL},

    {"",                LOG_NONE_LEVEL}, // end
};

T_TracerTaskInfo g_taTracerExtTaskInfoBody[TRACER_EXT_TASK_NUM_MAX] = {0};

RET_DATA uint8_t g_bTracerIntTaskNum;
RET_DATA uint8_t g_bTracerExtTaskNum;
RET_DATA uint8_t g_bTracerIntTaskDefLevel;
RET_DATA uint8_t g_bTracerExtTaskDefLevel;
RET_DATA T_TracerTaskInfo *g_ptTracerIntTaskInfo;
RET_DATA T_TracerTaskInfo *g_ptTracerExtTaskInfo;

RET_DATA T_TracerTaskInfoGetFp tracer_task_info_get;
RET_DATA T_TracerDefLevelFp tracer_def_level_set;
RET_DATA T_TracerCmdFp tracer_cmd;

extern osPoolId g_tTracerPoolId;
extern osMessageQId g_tTracerQueueId;
extern osThreadId g_tTracerThreadId;

extern uint8_t g_bTracerInit;
extern uint8_t g_bTracerNameDisplay;
extern uint8_t g_bTracerOptFull;

extern T_TracerOpt g_taTracerOptBody[TRACER_TASK_NUM_MAX];


// internal
extern RET_DATA uint8_t g_bTracerLogMode;
extern RET_DATA uint8_t g_bTracerLogDefLevel;
extern RET_DATA int g_iTracerPriority;
extern RET_DATA uint32_t g_dwTracerStackSize;
extern RET_DATA uint32_t g_dwTracerQueueNum;
extern RET_DATA uint32_t g_dwTracerQueueSize;
extern RET_DATA T_TracerOpt *g_ptTracerOpt;

extern RET_DATA T_TracerProcFp tracer_proc;
extern RET_DATA T_TracerMsgFreeFp tracer_msg_free;
extern RET_DATA T_TracerOptEntryAddFp tracer_opt_entry_add;
extern RET_DATA T_TracerNameGetFp tracer_task_name_get;
extern RET_DATA T_TracerTaskMainFp tracer_task_main;
extern RET_DATA T_TracerHandleGetFp tracer_task_handle_get;
extern RET_DATA T_TracerLevelGetFp tracer_level_get;
extern RET_DATA T_TracerStringSendFp USART_SendString;
extern RET_DATA T_TracerStringSendFp USART_SendStringII;

// external
extern RET_DATA T_TracerCommonFp tracer_init;
extern RET_DATA T_TracerLogLevelSetFp tracer_log_level_set;
extern RET_DATA T_TracerOptSetFp tracer_log_mode_set;
extern RET_DATA T_TracerOptGetFp tracer_log_mode_get;
extern RET_DATA T_TracerOptSetFp tracer_log_def_level_set;
extern RET_DATA T_TracerOptGetFp tracer_log_def_level_get;
extern RET_DATA T_TracerPrioritySetFp tracer_priority_set;
extern RET_DATA T_TracerCommonFp tracer_dump;
extern RET_DATA T_TracerNameDisplayFp tracer_name_display;
extern RET_DATA T_TracerPrintfFp tracer_drct_printf;
extern RET_DATA T_TracerMsgFp tracer_msg;


T_TracerTaskInfo *tracer_task_info_get_patch(char *baName, T_TracerTaskInfo *taTaskInfo, uint8_t bTaskNum)
{
    T_TracerTaskInfo *ptInfo = NULL;
    uint8_t i = 0;

    for(i = 0; i < bTaskNum; i++)
    {
        if(taTaskInfo[i].baName[0] == 0)
        {
            break;
        }

        if(!strncmp(baName, taTaskInfo[i].baName, strlen(taTaskInfo[i].baName)))
        {
            ptInfo = &(taTaskInfo[i]);
            break;
        }
    }

    return ptInfo;
}

void tracer_opt_entry_add_patch(uint32_t dwHandle, uint8_t bLevel)
{
    char baName[TRACER_TASK_NAME_LEN] = {0};
    T_TracerTaskInfo *ptInfo = NULL;
    uint8_t i = 0;
    int iUnusedIdx = -1;

    tracer_task_name_get(dwHandle, baName, sizeof(baName));

    ptInfo = tracer_task_info_get(baName, g_ptTracerIntTaskInfo, g_bTracerIntTaskNum);

    if(ptInfo)
    {
        // internal task
        goto done;
    }

    for(i = 0; i < g_bTracerExtTaskNum; i++)
    {
        if(g_ptTracerExtTaskInfo[i].baName[0] == 0)
        {
            iUnusedIdx = i;
            break;
        }

        if(!strncmp(baName, g_ptTracerExtTaskInfo[i].baName, strlen(g_ptTracerExtTaskInfo[i].baName)))
        {
            ptInfo = &(g_ptTracerExtTaskInfo[i]);
            break;
        }
    }

    if((!ptInfo) && (iUnusedIdx > -1))
    {
        // add external task info
        g_ptTracerExtTaskInfo[iUnusedIdx].bLevel = bLevel;
        snprintf(g_ptTracerExtTaskInfo[iUnusedIdx].baName, sizeof(g_ptTracerExtTaskInfo[iUnusedIdx].baName), "%s", baName);
    }

done:
    return;
}

int tracer_level_get_patch(uint32_t dwHandle, uint8_t *pbLevel)
{
    int iRet = -1;
    char baName[TRACER_TASK_NAME_LEN] = {0};
    T_TracerTaskInfo *ptInfo = NULL;

    tracer_task_name_get(dwHandle, baName, sizeof(baName));

    ptInfo = tracer_task_info_get(baName, g_ptTracerIntTaskInfo, g_bTracerIntTaskNum);

    if(ptInfo)
    {
        // internal task
        *pbLevel = ptInfo->bLevel;
        iRet = 0;
    }
    else
    {
        // external task
        ptInfo = tracer_task_info_get(baName, g_ptTracerExtTaskInfo, g_bTracerExtTaskNum);

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

void tracer_init_patch(void)
{
    osPoolDef_t tPoolDef = {0};
    //osPoolDef(TracerPool, TRACER_QUEUE_NUM, T_TracerMsg);
    osThreadDef_t tThreadDef = {0};
    osMessageQDef_t tQueueDef = {0};
    uint8_t i = 0;

    if(g_bTracerInit)
    {
        TRACER_DBG("[%s %d] Tracer already init\n", __func__, __LINE__);
        goto done;
    }

    for(i = 0; i < g_bTracerIntTaskNum; i++)
    {
        g_ptTracerIntTaskInfo[i].bLevel = g_bTracerIntTaskDefLevel;
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
    tThreadDef.name = TRACER_TASK_NAME;
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
    osStatus tStatus = osErrorOS;
    int iBufSize = 0;
    uint8_t bListUsed = 0;
    uint32_t dwHandle = 0;
    uint8_t bIsr = 0;
    uint8_t bAddOpt = 0;
    uint8_t bTaskLevel = 0;
    uint8_t bTruncated = 0;

    #ifdef TRACER_GET_MSG_LEN
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

        if(tracer_level_get(dwHandle, &bTaskLevel))
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
        // it should be supported by C99
        iBufSize = vsnprintf(NULL, 0, sFmt, tList);
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
    
        va_end(tList);
        bListUsed = 0;

        ptMsg->ptCb->tInfo.bType = bType;
        ptMsg->ptCb->tInfo.bLevel = bTaskLevel;
        ptMsg->ptCb->tInfo.dwHandle = dwHandle;
    }

    tStatus = osMessagePut(g_tTracerQueueId, (uint32_t)ptMsg, 0);

    if(tStatus != osOK)
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

int tracer_log_level_set_patch(uint8_t bIdx, uint8_t bLevel)
{
    if(bIdx == TRACER_TASK_IDX_MAX)
    {
        uint8_t i = 0;

        for(i = 0; i < g_bTracerIntTaskNum; i++)
        {
            g_ptTracerIntTaskInfo[i].bLevel = bLevel;
        }
    
        for(i = 0; i < g_bTracerExtTaskNum; i++)
        {
            g_ptTracerExtTaskInfo[i].bLevel = bLevel;
        }
    }
    else
    {
        if(bIdx < TRACER_INT_TASK_NUM_MAX)
        {
            // internal task
            g_ptTracerIntTaskInfo[bIdx].bLevel = bLevel;
        }
        else
        {
            // external task
            g_ptTracerExtTaskInfo[bIdx - TRACER_INT_TASK_NUM_MAX].bLevel = bLevel;
        }
    }

    return 0;
}

void tracer_dump_patch(void)
{
    uint8_t i = 0;
    uint8_t j = 0;

    tracer_cli(LOG_HIGH_LEVEL, "\nTracer Mode       [%d]\t0:disable/1:normal/2:print directly\n", g_bTracerLogMode);
    tracer_cli(LOG_HIGH_LEVEL, "Display Task Name [%d]\t0:not display/1:display\n", g_bTracerNameDisplay);
    tracer_cli(LOG_HIGH_LEVEL, "Tracer Priority   [%d]\tosPriorityIdle(%d) ~ osPriorityRealtime(%d)\n\n", g_iTracerPriority, osPriorityIdle, osPriorityRealtime);
    tracer_cli(LOG_HIGH_LEVEL, "Default Level for Internal Tasks  [0x%02X]\n", g_bTracerIntTaskDefLevel);
    tracer_cli(LOG_HIGH_LEVEL, "Default Level for App Tasks       [0x%02X]\n", g_bTracerExtTaskDefLevel);

    tracer_cli(LOG_HIGH_LEVEL, "\n%4s %20s: %s\n", "Index", "Name", "Level");

    for(i = 0; i < g_bTracerIntTaskNum; i++)
    {
        if(g_ptTracerIntTaskInfo[i].baName[0] == 0)
        {
            break;
        }

        tracer_cli(LOG_HIGH_LEVEL, "[%2d]  %20s: 0x%02X\n", i, g_ptTracerIntTaskInfo[i].baName, g_ptTracerIntTaskInfo[i].bLevel);
    }

    if(g_ptTracerExtTaskInfo[0].baName[0])
    {
        tracer_cli(LOG_HIGH_LEVEL, "\n------ Start of App Tasks ----------\n\n");
    }

    j = TRACER_INT_TASK_NUM_MAX;

    for(i = 0; i < g_bTracerExtTaskNum; i++)
    {
        if(g_ptTracerExtTaskInfo[i].baName[0] == 0)
        {
            break;
        }

        tracer_cli(LOG_HIGH_LEVEL, "[%2d]  %20s: 0x%02X\n", j, g_ptTracerExtTaskInfo[i].baName, g_ptTracerExtTaskInfo[i].bLevel);
        ++j;
    }

    tracer_cli(LOG_HIGH_LEVEL, "\n");

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
        // for internal task
        g_bTracerIntTaskDefLevel = bLevel;
    }
    else if(bType == TRACER_TASK_TYPE_APP)
    {
        // for external task
        g_bTracerExtTaskDefLevel = bLevel;
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

    dwParamNum = ParseParam(sCmd, baParam, dwNum);

    tracer_cli(LOG_HIGH_LEVEL, "\n");

    if(!strcmp(baParam[1], "level"))
    {
        if(dwParamNum != 4)
        {
            tracer_dump();
            tracer_cli(LOG_HIGH_LEVEL, "Usage: tracer level <task_index> <task_level:hex>\n");
            tracer_cli(LOG_HIGH_LEVEL, "\ttask_index: 0 ~ %d. Set %d to apply level to all tasks\n", TRACER_TASK_NUM_MAX - 1, TRACER_TASK_IDX_MAX);
            tracer_cli(LOG_HIGH_LEVEL, "\ttask_level: 0x00 ~ 0xFF\n");
            goto done;
        }
        else
        {
            uint8_t bIdx = (uint8_t)strtoul(baParam[2], NULL, 10);
            uint8_t bLevel = (uint8_t)strtoul(baParam[3], NULL, 16);

            if(tracer_log_level_set(bIdx, bLevel))
            {
                tracer_cli(LOG_HIGH_LEVEL, "tracer_log_level_set fail\n");
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
        tracer_name_display(bDisplay);

        if(bDisplay)
        {
            tracer_cli(LOG_HIGH_LEVEL, "display task name\n");
        }
        else
        {
            tracer_cli(LOG_HIGH_LEVEL, "do not display task name\n");
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
        tracer_priority_set(iPriority);
        tracer_cli(LOG_HIGH_LEVEL, "set priority to [%d]\n", iPriority);
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
        tracer_cli(LOG_HIGH_LEVEL, "\nTracer Command List:\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer mode <0:disable/1:normal/2:print directly>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer disp_name <0:not display/1:display>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer def_level <0:internal tasks/1:app tasks> <level:hex>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer level <task_index> <level:hex>\n");
        tracer_cli(LOG_HIGH_LEVEL, "tracer pri <osPriority:%d ~ %d>\n", osPriorityIdle, osPriorityRealtime);
    }

done:
    return;
}

void Tracer_PatchInit(void)
{
    // internal
    g_bTracerIntTaskNum = TRACER_INT_TASK_NUM_MAX;
    g_bTracerExtTaskNum = TRACER_EXT_TASK_NUM_MAX;
    g_bTracerIntTaskDefLevel = LOG_NONE_LEVEL;
    g_bTracerExtTaskDefLevel = LOG_ALL_LEVEL;
    g_ptTracerIntTaskInfo = g_taTracerIntTaskInfoBody;
    g_ptTracerExtTaskInfo = g_taTracerExtTaskInfoBody;

    g_bTracerLogMode = TRACER_MODE_NORMAL;
    g_iTracerPriority = TRACER_TASK_PRIORITY;
    g_dwTracerStackSize = TRACER_TASK_STACK_SIZE;
    g_dwTracerQueueNum = TRACER_QUEUE_NUM_PATCH;
    g_dwTracerQueueSize = TRACER_QUEUE_SIZE_PATCH;

    tracer_opt_entry_add = tracer_opt_entry_add_patch;
    tracer_level_get = tracer_level_get_patch;
    tracer_task_info_get = tracer_task_info_get_patch;
    
    // external
    tracer_init = tracer_init_patch;
    tracer_log_level_set = tracer_log_level_set_patch;
    tracer_dump = tracer_dump_patch;
    tracer_msg = tracer_msg_patch;
    tracer_def_level_set = tracer_def_level_set_patch;
    tracer_cmd = tracer_cmd_patch;

    return;
}

