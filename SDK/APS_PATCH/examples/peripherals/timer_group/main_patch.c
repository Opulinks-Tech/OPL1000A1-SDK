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

/******************************************************************************
*  Filename:
*  ---------
*  main_patch.c
*
*  Project:
*  --------
*  NL1000 Project - the main patch implement file
*
*  Description:
*  ------------
*  This implement file is include the main patch function and api.
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file
/******************************************************************************
*  Test code brief
*  These examples show how to configure timer settings and use timer driver APIs.
*
*  timer_reload_test() is an example that reload the settings of Timer0 when
*  the timer is timeout.
*  - port: Timer0
*  - interrupt: on
*
*  timer_periodic_test() is an example that keep the settings of Timer1 when
*  the timer is timeout.
*  - port: Timer1
*  - interrupt: on
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "hal_tmr.h"
#include "hal_tmr_patch.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the number of elements in the message queue
#define APP_MESSAGE_Q_SIZE  16

// the timeout value of Timer0 (us)
#define TIMEOUT_0_MAX       10000000    //  10000000us = 10sec
#define TIMEOUT_0_MIN       1000000     //  1000000us = 1sec
#define TIMEOUT_0_DELTA     1000000     //  1000000us = 1sec

// the timeout value of Timer1 (us)
#define TIMEOUT_1_TIME      1000000     //  1000000us = 1sec

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the content of message queue
typedef struct
{
    uint32_t ulTimerIdx;
    uint32_t ulTickCount;
    uint32_t ulTimeout;
} S_MessageQ;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
typedef void (*T_Main_AppInit_fp)(void);
extern T_Main_AppInit_fp Main_AppInit;


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable
static osThreadId g_tAppThread_1;
static osMessageQId g_tAppMessageQ;
static osPoolId g_tAppMemPoolId;

static uint32_t g_ulTimer0Timeout;
static uint8_t g_ulTimer0DeltaDirection;    // 0: up    1: down


// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
void Main_AppInit_patch(void);
static void Main_AppThread_1(void *argu);
static osStatus Main_AppMessageQSend(S_MessageQ *ptMsg);
static void timer_reload_test(void);
static void timer_periodic_test(void);
static void timer_reload_callback(uint32_t ulTimerIdx);
static void timer_periodic_callback(uint32_t ulTimerIdx);


/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   __Patch_EntryPoint
*
* DESCRIPTION:
*   the entry point of SW patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void __Patch_EntryPoint(void)
{
    // don't remove this code
    SysInit_EntryPoint();
    
    // application init
    Main_AppInit = Main_AppInit_patch;
}

/*************************************************************************
* FUNCTION:
*   Main_AppInit_patch
*
* DESCRIPTION:
*   the initial of application
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Main_AppInit_patch(void)
{
    osThreadDef_t tThreadDef;
    osMessageQDef_t tMessageDef;
    osPoolDef_t tMemPoolDef;
    
    // create the thread for AppThread_1
    tThreadDef.name = "App_1";
    tThreadDef.pthread = Main_AppThread_1;
    tThreadDef.tpriority = OS_TASK_PRIORITY_APP;        // osPriorityNormal
    tThreadDef.instances = 0;                           // reserved, it is no used
    tThreadDef.stacksize = OS_TASK_STACK_SIZE_APP;      // (512), unit: 4-byte, the size is 512*4 bytes
    g_tAppThread_1 = osThreadCreate(&tThreadDef, NULL);
    if (g_tAppThread_1 == NULL)
    {
        printf("To create the thread for AppThread_1 is fail.\n");
    }
    
    // create the message queue for AppMessageQ
    tMessageDef.queue_sz = APP_MESSAGE_Q_SIZE;          // number of elements in the queue
    tMessageDef.item_sz = sizeof(S_MessageQ);           // size of an item
    tMessageDef.pool = NULL;                            // reserved, it is no used
    g_tAppMessageQ = osMessageCreate(&tMessageDef, g_tAppThread_1);
    if (g_tAppMessageQ == NULL)
    {
        printf("To create the message queue for AppMessageQ is fail.\n");
    }
    
    // create the memory pool for AppMessageQ
    tMemPoolDef.pool_sz = APP_MESSAGE_Q_SIZE;           // number of items (elements) in the pool
    tMemPoolDef.item_sz = sizeof(S_MessageQ);           // size of an item
    tMemPoolDef.pool = NULL;                            // reserved, it is no used
    g_tAppMemPoolId = osPoolCreate(&tMemPoolDef);
    if (g_tAppMemPoolId == NULL)
    {
        printf("To create the memory pool for AppMessageQ is fail.\n");
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppThread_1
*
* DESCRIPTION:
*   the application thread 1
*
* PARAMETERS
*   1. argu     : [In] the input argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppThread_1(void *argu)
{
    osEvent tEvent;
    S_MessageQ *ptMsgPool;
    
    // do the timer_reload_test
    timer_reload_test();
    
    osDelay(500);      // delay 500 ms
    
    // do the timer_periodic_test
    timer_periodic_test();
    
    while (1)
    {
        // receive the message from AppMessageQ
        tEvent = osMessageGet(g_tAppMessageQ, osWaitForever);
        if (tEvent.status != osEventMessage)
        {
            printf("To receive the message from AppMessageQ is fail.\n");
            continue;
        }
        
        // get the content of message
        ptMsgPool = (S_MessageQ *)tEvent.value.p;
        
        // output the content of message
        printf("Timer[%d] Timeout[%d] Tick[%d]\n", ptMsgPool->ulTimerIdx, ptMsgPool->ulTimeout, ptMsgPool->ulTickCount);
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppMessageQSend
*
* DESCRIPTION:
*   send the message into AppMessageQ
*
* PARAMETERS
*   1. ptMsg    : [In] the pointer of message content
*
* RETURNS
*   osOK        : successful
*   osErrorOS   : fail
*
*************************************************************************/
static osStatus Main_AppMessageQSend(S_MessageQ *ptMsg)
{
    osStatus tRet = osErrorOS;
    S_MessageQ *ptMsgPool;
    
    // allocate the memory pool
    ptMsgPool = (S_MessageQ *)osPoolCAlloc(g_tAppMemPoolId);
    if (ptMsgPool == NULL)
    {
        printf("To allocate the memory pool for AppMessageQ is fail.\n");
        goto done;
    }
    
    // copy the message content
    memcpy(ptMsgPool, ptMsg, sizeof(S_MessageQ));
    
    // send the message
    if (osOK != osMessagePut(g_tAppMessageQ, (uint32_t)ptMsgPool, osWaitForever))
    {
        printf("To send the message for AppMessageQ is fail.\n");
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
        goto done;
    }
    
    tRet = osOK;

done:
    return tRet;
}

/*************************************************************************
* FUNCTION:
*   timer_reload_test
*
* DESCRIPTION:
*   an example that reload the settings of Timer0 when the timer is timeout.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void timer_reload_test(void)
{
    // init Timer0
    Hal_Tmr_Init(0);
    
    // set the callback function
    Hal_Tmr_CallBackFuncSet(0, timer_reload_callback);
    
    // start Timer0
    g_ulTimer0Timeout = TIMEOUT_0_MIN;
    g_ulTimer0DeltaDirection = 0;
    Hal_Tmr_Start(0, g_ulTimer0Timeout);
}

/*************************************************************************
* FUNCTION:
*   timer_periodic_test
*
* DESCRIPTION:
*   an example that keep the settings of Timer1 when the timer is timeout.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void timer_periodic_test(void)
{
    // init Timer1
    Hal_Tmr_Init(1);
    
    // set the callback function
    Hal_Tmr_CallBackFuncSet(1, timer_periodic_callback);
    
    // start Timer0
    Hal_Tmr_Start(1, 1000000);      // 1000000us = 1sec
}

/*************************************************************************
* FUNCTION:
*   timer_reload_callback
*
* DESCRIPTION:
*   the callback function of an example that reload the settings of Timer0
*   when the timer is timeout.
*
* PARAMETERS
*   ulTimerIdx  : [In] the timer index
*
* RETURNS
*   none
*
*************************************************************************/
static void timer_reload_callback(uint32_t ulTimerIdx)
{
    S_MessageQ tMsg;
    
    // send the result to AppThread_1
    tMsg.ulTimerIdx = ulTimerIdx;
    tMsg.ulTimeout = g_ulTimer0Timeout;
    tMsg.ulTickCount = osKernelSysTick();
    Main_AppMessageQSend(&tMsg);
    
    // reload the settings of Timer0
    if (g_ulTimer0DeltaDirection == 0)
    {
        g_ulTimer0Timeout = g_ulTimer0Timeout + TIMEOUT_0_DELTA;
        if (g_ulTimer0Timeout == TIMEOUT_0_MAX)
        {
            g_ulTimer0DeltaDirection = 1;
        }
    }
    else if (g_ulTimer0DeltaDirection == 1)
    {
        g_ulTimer0Timeout = g_ulTimer0Timeout - TIMEOUT_0_DELTA;
        if (g_ulTimer0Timeout == TIMEOUT_0_MIN)
        {
            g_ulTimer0DeltaDirection = 0;
        }
    }
    Hal_Tmr_Stop(0);
    Hal_Tmr_Start(0, g_ulTimer0Timeout);
}

/*************************************************************************
* FUNCTION:
*   timer_periodic_callback
*
* DESCRIPTION:
*   the callback function of an example that keep the settings of Timer1
*   when the timer is timeout.
*
* PARAMETERS
*   ulTimerIdx  : [In] the timer index
*
* RETURNS
*   none
*
*************************************************************************/
static void timer_periodic_callback(uint32_t ulTimerIdx)
{
    S_MessageQ tMsg;
    
    // send the result to AppThread_1
    tMsg.ulTimerIdx = ulTimerIdx;
    tMsg.ulTimeout = TIMEOUT_1_TIME;
    tMsg.ulTickCount = osKernelSysTick();
    Main_AppMessageQSend(&tMsg);
}
