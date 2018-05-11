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
*  These examples show how to configure uart settings and use uart driver APIs.
*
*  uart_int_test() is an example that read and write data on UART0, and send
*  the rx data to thread when the rx data is received by UART0 interrupt.
*  - port: UART0
*  - interrupt: on
*  - flow control: off
*  - pin assignment: tx(io2), rx(io3)
*
*  uart_echo_test() is an example that read and write data on UART1, and the
*  hardware flow control is turning on.
*  - port: UART1
*  - interrupt: off
*  - flow control: on
*  - pin assignment: tx(io8), rx(io9), rts(io6), cts(io7)
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_uart.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the number of elements in the message queue
#define APP_MESSAGE_Q_SIZE  16

// the event type of message
#define APP_EVENT_UART_RX   0x01


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the content of message queue
typedef struct
{
    uint32_t ulEvent;
    uint32_t ulData;
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
static void Main_AppInit_patch(void);
static void Main_AppThread_1(void *argu);
static void Main_AppThread_2(void *argu);
static void Main_AppMessageQSend(uint32_t ulData);
static void uart_int_test(void);
static void uart_echo_test(void);


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
    //Hal_PinMux_Uart_Init(&OPL1000_periph.uart[0]);
    Hal_PinMux_Uart_Init(&OPL1000_periph.uart[1]);
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
static void Main_AppInit_patch(void)
{
    // init the pin assignment
    App_Pin_InitConfig();
    
    // do the uart_int_test
    uart_int_test();
    
    // do the uart_echo_test
    uart_echo_test();
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
        switch (ptMsgPool->ulEvent)
        {
            case APP_EVENT_UART_RX:
                // debug log
                printf("uart0 read: %c\n", ptMsgPool->ulData);
                
                // write data to uart0
                Hal_Uart_DataSend(UART_IDX_0, ptMsgPool->ulData);
                break;
            
            default:
                break;
        }
        
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
    uint32_t ulData;
    
    while (1)
    {
        // read data from uart1
        if (0 == Hal_Uart_DataRecv(UART_IDX_1, &ulData))
        {
            // debug log
            printf("uart1 read: %c\n", ulData);

            // write data to uart1
            Hal_Uart_DataSend(UART_IDX_1, ulData);
        }
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
*   1. ulData   : [In] the data from uart rx
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppMessageQSend(uint32_t ulData)
{
    S_MessageQ *ptMsgPool;
    
    // allocate the memory pool
    ptMsgPool = (S_MessageQ *)osPoolCAlloc(g_tAppMemPoolId);
    if (ptMsgPool == NULL)
    {
        printf("To allocate the memory pool for AppMessageQ is fail.\n");
        goto done;
    }
    
    // copy the message content
    ptMsgPool->ulEvent = APP_EVENT_UART_RX;
    ptMsgPool->ulData = ulData;
    
    // send the message
    if (osOK != osMessagePut(g_tAppMessageQ, (uint32_t)ptMsgPool, osWaitForever))
    {
        printf("To send the message for AppMessageQ is fail.\n");
        
        // free the memory pool
        osPoolFree(g_tAppMemPoolId, ptMsgPool);
        goto done;
    }

done:
    return;
}

/*************************************************************************
* FUNCTION:
*   uart_int_test
*
* DESCRIPTION:
*   an example that read and write data on UART0, and send the rx data to
*   thread when the rx data is received in UART0 interrupt.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void uart_int_test(void)
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
    g_tAppMessageQ = osMessageCreate(&tMessageDef, g_tAppThread_2);
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
    
    // set the rx callback function and enable the rx interrupt
    Hal_Uart_RxCallBackFuncSet(UART_IDX_0, Main_AppMessageQSend);
    Hal_Uart_RxIntEn(UART_IDX_0, 1);
}

/*************************************************************************
* FUNCTION:
*   uart_echo_test
*
* DESCRIPTION:
*   an example that read and write data on UART1, and the hardware flow
*   control is turning on.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void uart_echo_test(void)
{
    osThreadDef_t tThreadDef;
    
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
    
}
