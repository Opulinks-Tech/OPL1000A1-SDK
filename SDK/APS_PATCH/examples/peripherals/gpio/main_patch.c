/******************************************************************************
*  Copyright 2018, Opulinks Technology Ltd. 
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd.  (C) 2018
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
*  These examples show how to configure gpio settings and use gpio driver APIs.
*
*  gpio_int_test() is an example that generate the output pulses from one gpio
*  to another, then the input side will be triggered the interrupt.
*  - port: GPIO2(io2), GPIO3(io3), GPIO4(io4), GPIO5(io5)
*  - interrupt: on
*  - connection:
*    connect GPIO2 with GPIO4
*    connect GPIO3 with GPIO5
*  - behavior:
*    GPIO2: output
*    GPIO3: output
*    GPIO4: input, interrupt from rising edge and falling edge
*    GPIO5: input, interrupt from rising edge
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_gpio.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the number of elements in the message queue
#define APP_MESSAGE_Q_SIZE  16


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the content of message queue
typedef struct
{
    uint32_t ulGpioIdx;
    uint32_t ulLevel;
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
static osThreadId g_tAppThread_2;
static osMessageQId g_tAppMessageQ;
static osPoolId g_tAppMemPoolId;


// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
void Main_AppInit_patch(void);
static void Main_AppThread_1(void *argu);
static void Main_AppThread_2(void *argu);
static osStatus Main_AppMessageQSend(S_MessageQ *ptMsg);
static void gpio_int_test(void);
static void gpio_int_callback(E_GpioIdx_t tGpioIdx);


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
*   App_Pin_InitConfig
*
* DESCRIPTION:
*   init the pin assignment
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void App_Pin_InitConfig(void)
{
    // GPIO2  OUTPUT
    Hal_Pinmux_Gpio_Init(&OPL1000_periph.gpio[0]);
    // GPIO3  OUTPUT
    Hal_Pinmux_Gpio_Init(&OPL1000_periph.gpio[1]);
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
    
    // init the pin assignment
    App_Pin_InitConfig();
    
    // do the gpio_int_test
    gpio_int_test();
    
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
    
    // create the thread for AppThread_2
    tThreadDef.name = "App_2";
    tThreadDef.pthread = Main_AppThread_2;
    tThreadDef.tpriority = OS_TASK_PRIORITY_APP;        // osPriorityNormal
    tThreadDef.instances = 0;                           // reserved, it is no used
    tThreadDef.stacksize = OS_TASK_STACK_SIZE_APP;      // (512), unit: 4-byte, the size is 512*4 bytes
    g_tAppThread_2 = osThreadCreate(&tThreadDef, NULL);
    if (g_tAppThread_2 == NULL)
    {
        printf("To create the thread for AppThread_2 is fail.\n");
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
        printf("GPIO[%d] Level[%d]\n", ptMsgPool->ulGpioIdx, ptMsgPool->ulLevel);
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppThread_2
*
* DESCRIPTION:
*   the application thread 2
*
* PARAMETERS
*   1. argu     : [In] the input argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppThread_2(void *argu)
{
    uint32_t ulCount = 1;

    while (1)
    {
        printf("Count = %d\n", ulCount);
        Hal_Vic_GpioOutput(GPIO_IDX_02, (E_GpioLevel_t)(ulCount % 2));
        Hal_Vic_GpioOutput(GPIO_IDX_03,  (E_GpioLevel_t)(ulCount % 2));
        ulCount++;
        
        osDelay(2000);      // delay 2000 ms
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
*   gpio_int_test
*
* DESCRIPTION:
*   an example that generate the output pulses from one gpio to another,
*   then the input side will be triggered the interrupt.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void gpio_int_test(void)
{

    // GPIO4
    Hal_Vic_GpioDirection(GPIO_IDX_04, GPIO_INPUT);
    Hal_Vic_GpioCallBackFuncSet(GPIO_IDX_04, gpio_int_callback);
    Hal_Vic_GpioIntTypeSel(GPIO_IDX_04, INT_TYPE_BOTH_EDGE);
    Hal_Vic_GpioIntInv(GPIO_IDX_04, 0);
    Hal_Vic_GpioIntMask(GPIO_IDX_04, 0);
    Hal_Vic_GpioIntEn(GPIO_IDX_04, 1);
    
    // GPIO5
    Hal_Vic_GpioDirection(GPIO_IDX_05, GPIO_INPUT);
    Hal_Vic_GpioCallBackFuncSet(GPIO_IDX_05, gpio_int_callback);
    Hal_Vic_GpioIntTypeSel(GPIO_IDX_05, INT_TYPE_RISING_EDGE);
    Hal_Vic_GpioIntInv(GPIO_IDX_05, 0);
    Hal_Vic_GpioIntMask(GPIO_IDX_05, 0);
    Hal_Vic_GpioIntEn(GPIO_IDX_05, 1);
}

/*************************************************************************
* FUNCTION:
*   gpio_int_callback
*
* DESCRIPTION:
*   an example that generate the output pulses from one gpio to another,
*   then the input side will be triggered the interrupt.
*
* PARAMETERS
*   tGpioIdx    : Index of call-back GPIO
*
* RETURNS
*   none
*
*************************************************************************/
static void gpio_int_callback(E_GpioIdx_t tGpioIdx)
{
    S_MessageQ tMsg;
    
    // send the result to AppThread_1
    tMsg.ulGpioIdx = tGpioIdx;
    tMsg.ulLevel = Hal_Vic_GpioInput(tGpioIdx);
    Main_AppMessageQSend(&tMsg);
}
