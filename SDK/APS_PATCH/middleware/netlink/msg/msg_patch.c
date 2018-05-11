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
#include "sys_os_config.h"
#include "msg_patch.h"


#define TRACER_DBG(...)


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
            /*
            if(!g_bTracerOptFull)
            {
                // add opt entry if entry not found and table not full
                bAddOpt = 1;
            }
            */
        }

    	if(bLevel & bTaskLevel)
        {
            bProcLog = 1;
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
        ptMsg->ptCb->tInfo.bLevel = bLevel;
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

void Tracer_PatchInit(void)
{
    // internal
    g_bTracerLogMode = TRACER_MODE_NORMAL;
    g_dwTracerQueueNum = TRACER_QUEUE_NUM_PATCH;
    g_dwTracerQueueSize = TRACER_QUEUE_SIZE_PATCH;
    
    // external
    tracer_msg = tracer_msg_patch;
    return;
}

