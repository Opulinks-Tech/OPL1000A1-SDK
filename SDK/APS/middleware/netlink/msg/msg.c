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

/*===========================================================================

                   msg.c

DESCRIPTION
  This file implement main task 


       
Copyright (c) 2006 by Paul, Incorporated.  All Rights Reserved.

============================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/30/13   Paul     initial
===========================================================================*/
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

/********************************************************************************************************
*                                           VARIABLES
********************************************************************************************************/

RET_DATA T_MsgPringIIFp msg_printII;

void USART_SendString_Impl(char* pszData);
void USART_SendStringII_Impl(char* pszData);
void msg_printII_impl(bool bFlag, int iLevel, char* pszData, ...);


#ifdef TRACER

//#define TRACER_GET_MSG_LEN // not supported for A0 FPGA

#include "cmsis_os.h"
#include "sys_os_config.h"

#define TRACER_TASK_NAME        "tracer"
#define TRACER_TASK_PRIORITY    OS_TASK_PRIORITY_TRACER
#define TRACER_TASK_STACK_SIZE  OS_TASK_STACK_SIZE_TRACER // number of uint32_t

#define TRACER_ISR_HANDLE_MASK  0xFF

#define TRACER_DEBUG

#ifdef TRACER_DEBUG
    #ifdef printf
    #undef printf
    #endif

    #define TRACER_DBG          printf
#else
    #define TRACER_DBG(...)
#endif


osPoolId g_tTracerPoolId = NULL;
osMessageQId g_tTracerQueueId = NULL;
osThreadId g_tTracerThreadId = NULL;

uint8_t g_bTracerInit = 0;
uint8_t g_bTracerNameDisplay = 0;
uint8_t g_bTracerOptFull = 0;

T_TracerOpt g_taTracerOptBody[TRACER_TASK_NUM_MAX] = {0};

void tracer_proc_impl(char *sString);
void tracer_msg_free_impl(T_TracerMsg *ptMsg);
void tracer_opt_entry_add_impl(uint32_t dwHandle, uint8_t bLevel);
void tracer_task_name_get_impl(uint32_t dwHandle, char *baName, uint32_t dwSize);
void tracer_task_main_impl(void *pParam);
uint32_t tracer_task_handle_get_impl(uint8_t *pbIsr);
int tracer_level_get_impl(uint32_t dwHandle, uint8_t *pbLevel);


// internal
RET_DATA uint8_t g_bTracerLogMode;
RET_DATA uint8_t g_bTracerLogDefLevel;
RET_DATA int g_iTracerPriority;
RET_DATA uint32_t g_dwTracerStackSize;
RET_DATA uint32_t g_dwTracerQueueNum;
RET_DATA uint32_t g_dwTracerQueueSize;
RET_DATA T_TracerOpt *g_ptTracerOpt;

RET_DATA T_TracerProcFp tracer_proc;
RET_DATA T_TracerMsgFreeFp tracer_msg_free;
RET_DATA T_TracerOptEntryAddFp tracer_opt_entry_add;
RET_DATA T_TracerNameGetFp tracer_task_name_get;
RET_DATA T_TracerTaskMainFp tracer_task_main;
RET_DATA T_TracerHandleGetFp tracer_task_handle_get;
RET_DATA T_TracerLevelGetFp tracer_level_get;
RET_DATA T_TracerStringSendFp USART_SendString;
RET_DATA T_TracerStringSendFp USART_SendStringII;

// external
RET_DATA T_TracerCommonFp tracer_init;
RET_DATA T_TracerLogLevelSetFp tracer_log_level_set;
RET_DATA T_TracerOptSetFp tracer_log_mode_set;
RET_DATA T_TracerOptGetFp tracer_log_mode_get;
RET_DATA T_TracerOptSetFp tracer_log_def_level_set;
RET_DATA T_TracerOptGetFp tracer_log_def_level_get;
RET_DATA T_TracerPrioritySetFp tracer_priority_set;
RET_DATA T_TracerCommonFp tracer_dump;
RET_DATA T_TracerNameDisplayFp tracer_name_display;
RET_DATA T_TracerPrintfFp tracer_drct_printf;
RET_DATA T_TracerMsgFp tracer_msg;


void tracer_proc_impl(char *sString)
{
    char *pt;

#if defined(__SIMULATOR__)
    pt = &sString[0];
    
    while(*pt != '\0')
    {
        ITM_SendChar(*pt++);
    }
#else
    pt = &sString[0];
   
    while(*pt != '\0')
    {
        if(*pt == '\n') {
            Hal_DbgUart_DataSend('\r');
        }

        Hal_DbgUart_DataSend(*pt++);
    }
#endif

    return;
}

void tracer_msg_free_impl(T_TracerMsg *ptMsg)
{
    if(ptMsg == NULL)
    {
        goto done;
    }

    if(ptMsg->ptCb)
    {
        free(ptMsg->ptCb);
        ptMsg->ptCb = NULL;
    }

    if(osPoolFree(g_tTracerPoolId, ptMsg) != osOK)
    {
        TRACER_DBG("[%s %d] osPoolFree fail\n", __func__, __LINE__);
    }

done:
    return;
}

void tracer_opt_entry_add_impl(uint32_t dwHandle, uint8_t bLevel)
{
    uint8_t i = 0;
    int iIdx = -1;

    for(i = 0; i < TRACER_TASK_NUM_MAX; i++)
    {
        if(g_ptTracerOpt[i].bUsed)
        {
            // entry already exist
            if(g_ptTracerOpt[i].dwHandle == dwHandle)
            {
                goto done;
            }
        }
        else
        {
            if(iIdx == -1)
            {
                iIdx = i;
            }
        }
    }

    if(iIdx == -1)
    {
        TRACER_DBG("[%s %d] g_taOpt not enough\n", __func__, __LINE__);
        g_bTracerOptFull = 1;
        goto done;
    }

    // add new entry
    g_ptTracerOpt[iIdx].dwHandle = dwHandle;
    g_ptTracerOpt[iIdx].bLevel = g_bTracerLogDefLevel;
    g_ptTracerOpt[iIdx].bUsed = 1;

done:
    return;
}

void tracer_task_name_get_impl(uint32_t dwHandle, char *baName, uint32_t dwSize)
{
    if((dwHandle & (~TRACER_ISR_HANDLE_MASK)) == (~TRACER_ISR_HANDLE_MASK))
    {
        // ISR
        snprintf(baName, dwSize, "ISR_%u", dwHandle & TRACER_ISR_HANDLE_MASK);
    }
    else
    {
        // Task
        snprintf(baName, dwSize, "%s", pcTaskGetName((TaskHandle_t)dwHandle));
    }

    return;
}

void tracer_task_main_impl(void *pParam)
{
    osEvent tEvent;
    T_TracerMsg *ptMsg = NULL;
    //osThreadId *ptThreadId = (osThreadId *)pParam;

    while(1)
    {
        tEvent = osMessageGet(g_tTracerQueueId, osWaitForever);

        if(tEvent.status != osEventMessage)
        {
            continue;
        }

        ptMsg = (T_TracerMsg *)tEvent.value.p;

        if(ptMsg == NULL)
        {
            TRACER_DBG("[%s %d] ptMsg is NULL\n", __func__, __LINE__);
            goto done;
        }

        if(ptMsg->ptCb == NULL)
        {
            TRACER_DBG("[%s %d] ptMsg->ptCb is NULL\n", __func__, __LINE__);
            goto done;
        }

        switch(ptMsg->ptCb->tInfo.bType)
        {
            case TRACER_TYPE_LOG:
                tracer_opt_entry_add(ptMsg->ptCb->tInfo.dwHandle, ptMsg->ptCb->tInfo.bLevel);

                if(g_bTracerLogMode != TRACER_MODE_NORMAL)
                {
                    //TRACER_DBG("[%s %d] normal mode disabled\n", __func__, __LINE__);
                    goto done;
                }

                if(g_bTracerNameDisplay)
                {
                    char baName[32] = {0};

                    tracer_task_name_get(ptMsg->ptCb->tInfo.dwHandle, baName, sizeof(baName));
                    strcat(baName, " -> ");
                    tracer_proc(baName);
                }

                tracer_proc(ptMsg->ptCb->baBuf);
                break;
                
            case TRACER_TYPE_CLI:
                tracer_proc(ptMsg->ptCb->baBuf);
                break;

            case TRACER_TYPE_OPT_ADD:
                tracer_opt_entry_add(ptMsg->ptCb->tInfo.dwHandle, ptMsg->ptCb->tInfo.bLevel);
                break;
    
            default:
                 TRACER_DBG("[%s %d] unknown type[%d]\n", __func__, __LINE__, ptMsg->ptCb->tInfo.bType);
                 goto done;
        }

    done:
        tracer_msg_free(ptMsg);
    }
}

uint32_t tracer_task_handle_get_impl(uint8_t *pbIsr)
{
    uint32_t dwHandle = 0;
    uint32_t dwVecAct = SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk;

    if(dwVecAct)
    {
        // ISR
        dwHandle = (dwVecAct - 16) | (~TRACER_ISR_HANDLE_MASK);
        *pbIsr = 1;
    }
    else
    {
        // Task
        dwHandle = (uint32_t)xTaskGetCurrentTaskHandle();
        *pbIsr = 0;
    }

    return dwHandle;
}

int tracer_level_get_impl(uint32_t dwHandle, uint8_t *pbLevel)
{
    int iRet = -1;
    uint8_t i = 0;

    *pbLevel = g_bTracerLogDefLevel;

    for(i = 0; i < TRACER_TASK_NUM_MAX; i++)
    {
        if(g_ptTracerOpt[i].bUsed)
        {
            if(g_ptTracerOpt[i].dwHandle == dwHandle)
            {
                *pbLevel = g_ptTracerOpt[i].bLevel;
                iRet = 0;
                goto done;
            }
        }
    }

done:
    return iRet;
}

void tracer_init_impl(void)
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

    for(i = 0; i < TRACER_TASK_NUM_MAX; i++)
    {
        g_ptTracerOpt[i].dwHandle = 0;
        g_ptTracerOpt[i].bLevel = LOG_NONE_LEVEL;
        g_ptTracerOpt[i].bUsed = 0;
    }

    tPoolDef.pool_sz = g_dwTracerQueueNum;
    tPoolDef.item_sz = sizeof(T_TracerMsg);

    //create memory pool
    g_tTracerPoolId = osPoolCreate(&tPoolDef);

    if(g_tTracerPoolId == NULL)
    {
        TRACER_DBG("[%s %d] osPoolCreate fail\n", __func__, __LINE__);
        goto done;
    }

    //create message queue
    tQueueDef.item_sz = sizeof(T_TracerMsg);
    tQueueDef.queue_sz = g_dwTracerQueueNum;

    g_tTracerQueueId = osMessageCreate(&tQueueDef, NULL);

    if(g_tTracerQueueId == NULL)
    {
        TRACER_DBG("[%s %d] osMessageCreate fail\n", __func__, __LINE__);
        goto done;
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

int tracer_drct_printf_impl(const char *sFmt, ...)
{
    va_list tList;

    va_start(tList, sFmt);
    vprintf(sFmt, tList);
    va_end(tList);
    return 0;
}

int tracer_msg_impl(uint8_t bType, uint8_t bLevel, char *sFmt, ...)
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
        iBufSize = vsnprintf(baTmp, g_dwTracerQueueSize, sFmt, tList);
        #endif
    
        if(iBufSize <= 0)
        {
            //TRACER_DBG("[%s %d] vsnprintf fail\n", __func__, __LINE__);
            goto done;
        }
        
        iBufSize += 1; // for '\0'
    
        if(iBufSize > g_dwTracerQueueSize)
        {
            iBufSize = g_dwTracerQueueSize;
        }
    
        ptMsg->ptCb = (T_TracerCb *)malloc(sizeof(T_TracerCb) + iBufSize);
    
        if(ptMsg->ptCb == NULL)
        {
            TRACER_DBG("[%s %d] malloc fail\n", __func__, __LINE__);
            goto done;
        }
    
        #ifdef TRACER_GET_MSG_LEN
        vsnprintf(ptMsg->ptCb->baBuf, iBufSize, sFmt, tList);
        #else
        snprintf(ptMsg->ptCb->baBuf, iBufSize, "%s", baTmp);
        #endif
    
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

int tracer_log_level_set_impl(uint8_t bIdx, uint8_t bLevel)
{
    int iRet = -1;
    uint8_t i = 0;

    if(bIdx == TRACER_TASK_IDX_MAX)
    {
        for(i = 0; i < TRACER_TASK_NUM_MAX; i++)
        {
            if(g_ptTracerOpt[i].bUsed)
            {
                g_ptTracerOpt[i].bLevel = bLevel;
            }
        }

        iRet = 0;
    }
    else if(bIdx < TRACER_TASK_NUM_MAX)
    {
        if(g_ptTracerOpt[bIdx].bUsed)
        {
            g_ptTracerOpt[bIdx].bLevel = bLevel;
            iRet = 0;
        }
    }

    return iRet;
}

void tracer_log_mode_set_impl(uint8_t bMode)
{
    if(!g_bTracerInit)
    {
        TRACER_DBG("[%s %d] Tracer not init yet\n", __func__, __LINE__);
        //goto done;
    }

    if(bMode < TRACER_MODE_MAX)
    {
        g_bTracerLogMode = bMode;
    }

//done:
    return;
}

uint8_t tracer_log_mode_get_impl(void)
{
    return g_bTracerLogMode;
}

void tracer_log_def_level_set_impl(uint8_t bLevel)
{
    g_bTracerLogDefLevel = bLevel;
    return;
}

uint8_t tracer_log_def_level_get_impl(void)
{
    return g_bTracerLogDefLevel;
}

void tracer_priority_set_impl(int iPriority)
{
    if(g_tTracerThreadId != NULL)
    {
        if(osThreadSetPriority(g_tTracerThreadId, (osPriority)iPriority) == osOK)
        {
            g_iTracerPriority = iPriority;
        }
        else
        {
            TRACER_DBG("[%s %d] osThreadSetPriority fail\n", __func__, __LINE__);
        }
    }
    else
    {
        TRACER_DBG("[%s %d] g_tTracerThreadId is NULL\n", __func__, __LINE__);
    }

    return;
}

void tracer_dump_impl(void)
{
    uint8_t i = 0;

    tracer_cli(LOG_HIGH_LEVEL, "\nTracer Mode       [%d]\t0:disable/1:normal/2:print directly\n", g_bTracerLogMode);
    tracer_cli(LOG_HIGH_LEVEL, "Display Task Name [%d]\t0:not display/1:display\n", g_bTracerNameDisplay);
    tracer_cli(LOG_HIGH_LEVEL, "Tracer Priority   [%d]\tosPriorityIdle(%d) ~ osPriorityRealtime(%d)\n", g_iTracerPriority, osPriorityIdle, osPriorityRealtime);
    tracer_cli(LOG_HIGH_LEVEL, "Default Level     [0x%02X]\n", g_bTracerLogDefLevel);

    tracer_cli(LOG_HIGH_LEVEL, "\n%4s %32s: %s\n", "Index", "Name", "Level");

    for(i = 0; i < TRACER_TASK_NUM_MAX; i++)
    {
        if(g_ptTracerOpt[i].bUsed)
        {
            char baName[32] = {0};

            tracer_task_name_get(g_ptTracerOpt[i].dwHandle, baName, sizeof(baName));
            tracer_cli(LOG_HIGH_LEVEL, "[%2d]  %32s: 0x%02X\n", i, baName, g_ptTracerOpt[i].bLevel);
        }
    }

    tracer_cli(LOG_HIGH_LEVEL, "\n");

    return;
}

void tracer_name_display_impl(uint8_t bDisplay)
{
    if(bDisplay)
    {
        g_bTracerNameDisplay = 1;
    }
    else
    {
        g_bTracerNameDisplay = 0;
    }

    return;
}

void Tracer_PreInit(void)
{
    // internal
    g_bTracerLogMode = TRACER_DEF_MODE;
    g_bTracerLogDefLevel = TRACER_DEF_LEVEL;
    g_iTracerPriority = TRACER_TASK_PRIORITY;
    g_dwTracerStackSize = TRACER_TASK_STACK_SIZE;
    g_dwTracerQueueNum = TRACER_QUEUE_NUM;
    g_dwTracerQueueSize = TRACER_QUEUE_SIZE;
    g_ptTracerOpt = g_taTracerOptBody;

    tracer_proc = tracer_proc_impl;
    tracer_msg_free = tracer_msg_free_impl;
    tracer_opt_entry_add = tracer_opt_entry_add_impl;
    tracer_task_name_get = tracer_task_name_get_impl;
    tracer_task_main = tracer_task_main_impl;
    tracer_task_handle_get = tracer_task_handle_get_impl;
    tracer_level_get = tracer_level_get_impl;
    USART_SendString = USART_SendString_Impl;
    USART_SendStringII = USART_SendStringII_Impl;
    
    // external
    tracer_init = tracer_init_impl;
    tracer_log_level_set = tracer_log_level_set_impl;
    tracer_log_mode_set = tracer_log_mode_set_impl;
    tracer_log_mode_get = tracer_log_mode_get_impl;
    tracer_log_def_level_set = tracer_log_def_level_set_impl;
    tracer_log_def_level_get = tracer_log_def_level_get_impl;
    tracer_priority_set = tracer_priority_set_impl;
    tracer_dump = tracer_dump_impl;
    tracer_name_display = tracer_name_display_impl;
    tracer_drct_printf = tracer_drct_printf_impl;
    tracer_msg = tracer_msg_impl;

    msg_printII = msg_printII_impl;

    return;
}

#ifdef TRACER_SUT

#define TRACER_SUT_TASK_NUM     3
#define TRACER_SUT_TASK_DELAY   2000 // ms

static volatile uint8_t g_bSutTaksRun = 0;
static osThreadId g_taSutThreadId[TRACER_SUT_TASK_NUM] = {0};
static char *g_saSutThreadName[TRACER_SUT_TASK_NUM] = 
{
    "tracer_sut_1", 
    "tracer_sut_2", 
    "tracer_sut_3"
};

static void tracer_sut_task_main_1(void *pParam)
{
    uint8_t dwEnableCliAt = (uint32_t)pParam;

    while(g_bSutTaksRun)
    {
        tracer_log(LOG_HIGH_LEVEL, "[%s] Log: LOG_HIGH_LEVEL\n", __func__);
        tracer_log(LOG_MED_LEVEL, "[%s] Log: LOG_MED_LEVEL\n", __func__);
        tracer_log(LOG_LOW_LEVEL, "[%s] Log: LOG_LOW_LEVEL\n", __func__);

        if(dwEnableCliAt)
        {
            tracer_cli(LOG_LOW_LEVEL, "[%s] CLI output\n", __func__);
        }

        osDelay(TRACER_SUT_TASK_DELAY);
    }

    return;
}

static void tracer_sut_task_main_2(void *pParam)
{
    uint8_t dwEnableCliAt = (uint32_t)pParam;

    while(g_bSutTaksRun)
    {
        tracer_log(LOG_HIGH_LEVEL, "[%s] Log: LOG_HIGH_LEVEL\n", __func__);
        tracer_log(LOG_MED_LEVEL, "[%s] Log: LOG_MED_LEVEL\n", __func__);
        tracer_log(LOG_LOW_LEVEL, "[%s] Log: LOG_LOW_LEVEL\n", __func__);

        if(dwEnableCliAt)
        {
            tracer_cli(LOG_LOW_LEVEL, "[%s] CLI output\n", __func__);
        }

        osDelay(TRACER_SUT_TASK_DELAY);
    }

    return;
}

static void tracer_sut_task_main_3(void *pParam)
{
    uint8_t dwEnableCliAt = (uint32_t)pParam;

    while(g_bSutTaksRun)
    {
        tracer_log(LOG_HIGH_LEVEL, "[%s] Log: LOG_HIGH_LEVEL\n", __func__);
        tracer_log(LOG_MED_LEVEL, "[%s] Log: LOG_MED_LEVEL\n", __func__);
        tracer_log(LOG_LOW_LEVEL, "[%s] Log: LOG_LOW_LEVEL\n", __func__);

        if(dwEnableCliAt)
        {
            tracer_cli(LOG_LOW_LEVEL, "[%s] CLI output\n", __func__);
        }

        osDelay(TRACER_SUT_TASK_DELAY);
    }

    return;
}

static os_pthread g_fpaSutThreadMain[TRACER_SUT_TASK_NUM] = 
{
    tracer_sut_task_main_1,
    tracer_sut_task_main_2,
    tracer_sut_task_main_3
};

static void tracer_sut_task_delete(void)
{
    uint8_t i = 0;

    for(i = 0; i < TRACER_SUT_TASK_NUM; i++)
    {
        if(g_taSutThreadId[i])
        {
            osThreadTerminate(g_taSutThreadId[i]);
            g_taSutThreadId[i] = 0;
        }
    }
}

int tracer_sut_task_create(uint8_t bEnableCliAt)
{
    int iRet = -1;
    uint8_t i = 0;
    osThreadDef_t tThreadDef = {0};
    uint32_t dwValue = bEnableCliAt;

    if(g_bSutTaksRun)
    {
        TRACER_DBG("[%s %d] terminate sut tasks\n", __func__, __LINE__);
        tracer_sut_task_delete();
        goto done;
    }

    g_bSutTaksRun = 1;

    for(i = 0; i < TRACER_SUT_TASK_NUM; i++)
    {
        //create task
        tThreadDef.name = g_saSutThreadName[i];
        tThreadDef.stacksize = TRACER_TASK_STACK_SIZE;
        tThreadDef.tpriority = TRACER_TASK_PRIORITY;
        tThreadDef.pthread = g_fpaSutThreadMain[i];
    
        g_taSutThreadId[i] = osThreadCreate(&tThreadDef, (void *)dwValue);
    
        if(g_taSutThreadId[i] == NULL)
        {
            TRACER_DBG("[%s %d] osThreadCreate fail\n", __func__, __LINE__);
            goto done;
        }
    }

    iRet = 0;
    
done:
    if(iRet)
    {
        g_bSutTaksRun = 0;
    }

    return iRet;
}
#endif //#ifdef TRACER_SUT

#else //#ifdef TRACER

int log_level_now = LOG_THRESHOLD;

void msg_init_impl()
{
	log_level_now = LOG_HIGH_LEVEL;
	msg_printII = msg_printII_impl;
}

void msg_print2_impl(char *fmt,...)
{
	va_list ap;
    char string[256];
    char *pt;
	
	//if(level == log_level_now)
	{
		va_start(ap,fmt);
    	vsprintf(string,fmt,ap);
#if defined(__SIMULATOR__)
        pt = &string[0];
        while(*pt != '\0')
        {
            ITM_SendChar(*pt++);
        }
#else
	    pt = &string[0];
        while(*pt != '\0')
        {
            if(*pt == '\n') {
                Hal_DbgUart_DataSend('\r');
            }

            Hal_DbgUart_DataSend(*pt++);
        }
#endif
    	va_end(ap);
	}
}

void msg_print_impl(int level, char *fmt,...)
{
	va_list ap;
    char string[256];
    char *pt;
	
    //memset(string, 0, 256);
	if(level == log_level_now)
	{
		va_start(ap,fmt);
    	vsprintf(string,fmt,ap);
#if defined(__SIMULATOR__)
        pt = &string[0];
        /*while(*pt != '\n')
        {
            if(*pt == '\r')
            {
                ITM_SendChar(0x0A);
                pt++;
            }
            else
                ITM_SendChar(*pt++);
        }
        ITM_SendChar(0x0D);*/
        while(*pt != '\0')
        {
            ITM_SendChar(*pt++);
        }
#else
        pt = &string[0];
       /* while(*pt != '\n')
        {
            //nl1000_uart_send_data(UARTDBG, *pt++);
            if(*pt == '\r')
            {
                Hal_DbgUart_DataSend(0x0A);
                pt++;
            }
            else
                Hal_DbgUart_DataSend(*pt++);
        }
        Hal_DbgUart_DataSend(0x0D);*/
        while(*pt != '\0')
        {
            if(*pt == '\n') {
                Hal_DbgUart_DataSend('\r');
            }

            Hal_DbgUart_DataSend(*pt++);
        }
#endif
    	va_end(ap);
	}
}

void msg_set_level_impl(int level)
{
	log_level_now = level;
}

T_MsgInitFp msg_init = msg_init_impl;
T_MsgPrintFp msg_print = msg_print_impl;
T_MsgPrint2Fp msg_print2 = msg_print2_impl;
T_MsgLevelSetFp msg_set_level = msg_set_level_impl;
T_MsgStringSendFp USART_SendString = USART_SendString_Impl;
T_MsgStringSendFp USART_SendStringII = USART_SendStringII_Impl;

#endif //#ifdef TRACER

//Brian
void msg_printII_impl(bool bFlag, int iLevel, char* pszData, ...)
{
    va_list ap;
    char szString[256] = {0};

#if defined(__SIMULATOR__)
    char *pszString;
#endif	
	
    //if(iLevel == log_level_now)
    {
        va_start(ap, pszData);
        vsprintf(szString, pszData, ap);
        strcat(szString, "\r\n");		
					
#if defined(__SIMULATOR__)
        pszString = &szString[0];
        while(*pszString != '\n')
        {
            ITM_SendChar(*pszString++);
        }
#else			
		 if(bFlag == true)
		    USART_SendString(szString);
        else
		    USART_SendStringII(szString);
#endif 
        va_end(ap);
    }
}

#if 1 // to be removed
void USART_SendStringII_Impl(char* sString)
{	
	while(*sString != '\n')
	{
			if(*sString == '\r')
			{
					Hal_DbgUart_DataSend(0x0A);
					sString++;
			}
			else
					Hal_DbgUart_DataSend(*sString++);
	}
	Hal_DbgUart_DataSend(0x0D);
}

void USART_SendString_Impl(char *sString)
{
	while(*sString != '\n')
	{
			if(*sString == '\r')
			{
					Hal_DbgUart_DataSend(0x0A);
					sString++;
			}
			else
					Hal_DbgUart_DataSend(*sString++);
	}
	Hal_DbgUart_DataSend(0x0D);
	Hal_DbgUart_DataSend('>');	
}
#else
void USART_SendStringII_Impl(char* sString)
{	
	while(*sString != '\n')
	{
			if(*sString == '\r')
			{
					Hal_Uart_DataSend(g_tTracerUartIdx, 0x0A);
					sString++;
			}
			else
					Hal_Uart_DataSend(g_tTracerUartIdx, *sString++);
	}
	Hal_Uart_DataSend(g_tTracerUartIdx, 0x0D);
}

void USART_SendString_Impl(char *sString)
{
	while(*sString != '\n')
	{
			if(*sString == '\r')
			{
					Hal_Uart_DataSend(g_tTracerUartIdx, 0x0A);
					sString++;
			}
			else
					Hal_Uart_DataSend(g_tTracerUartIdx, *sString++);
	}
	Hal_Uart_DataSend(g_tTracerUartIdx, 0x0D);
	Hal_Uart_DataSend(g_tTracerUartIdx, '>');	
}
#endif

/* for printf() */
int fputc(int ch, FILE *f) 
{
#if defined(__SIMULATOR__)
    return ITM_SendChar(ch);
#else
    if(ch == '\n') {
        Hal_DbgUart_DataSend('\r');
    }

    Hal_DbgUart_DataSend(ch);
    return ch;
#endif    
}

#if defined(__SIMULATOR__)
volatile int ITM_RxBuffer = ITM_RXBUFFER_EMPTY;       /* used for Debug Input */

int fgetc(FILE *f)
{
    //while (ITM_CheckChar() != 1){ __NOP();}
    return (ITM_ReceiveChar());
}
#endif

int kbhit(void)
{
#if defined(__SIMULATOR__)
    return (ITM_CheckChar());
#else
    //return ((MEM_READ_DW(REG_UARTDBG_STATE) & UARTDBG_STATE_RX_BUFF_FULL) == UARTDBG_STATE_RX_BUFF_FULL);
    return ((MEM_READ_DW(0x30001004) & 0x2) == 0x2);
#endif
}

uint8_t uart_getchar(void)
{
#if defined(__SIMULATOR__)
    return (ITM_ReceiveChar());
#else
	//return MEM_READ_DW(REG_UARTDBG_DATA);
    return MEM_READ_DW(0x30001000);
#endif
}
