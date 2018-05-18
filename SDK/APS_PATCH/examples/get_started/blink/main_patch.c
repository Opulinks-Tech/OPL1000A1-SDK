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


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


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
static osSemaphoreId g_tAppSemaphoreId;
static osMutexId g_tAppMutexId;
static osTimerId g_tAppTimerId;


// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
void Main_AppInit_patch(void);
static void Main_AppThread_1(void *argu);
static void Main_AppThread_2(void *argu);
static void Main_AppTimer(void const *argu);


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
    osSemaphoreDef_t tSemaphoreDef;
    osMutexDef_t tMutexDef;
    osTimerDef_t tTimerDef;
    
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
    
    // create the semaphore
    tSemaphoreDef.dummy = 0;                            // reserved, it is no used
    g_tAppSemaphoreId = osSemaphoreCreate(&tSemaphoreDef, 1);
    if (g_tAppSemaphoreId == NULL)
    {
        printf("To create the semaphore for AppSemaphore is fail.\n");
    }
    
    // create the mutex
    tMutexDef.dummy = 0;                                // reserved, it is no used
    g_tAppMutexId = osMutexCreate(&tMutexDef);
    if (g_tAppMutexId == NULL)
    {
        printf("To create the mutex for AppMutex is fail.\n");
    }

    // create the timer
    tTimerDef.ptimer = Main_AppTimer;
    g_tAppTimerId = osTimerCreate(&tTimerDef, osTimerPeriodic, NULL);
    if (g_tAppTimerId == NULL)
    {
        printf("To create the timer for AppTimer is fail.\n");
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
    // after the initialization, the state of semaphore is released
    // please take the first token, if want to lock the semaphore
    osSemaphoreWait(g_tAppSemaphoreId, osWaitForever);
    
    // start the timer
    osTimerStart(g_tAppTimerId, 2000);      // 2 sec
    
    while (1)
    {
        // wait the semaphore
        osSemaphoreWait(g_tAppSemaphoreId, osWaitForever);
        
        // release the mutex
        osMutexRelease(g_tAppMutexId);
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
    // after the initialization, the state of mutex is released
    // please take the first token, if want to lock the mutex
    osMutexWait(g_tAppMutexId, osWaitForever);
    
    while (1)
    {
        // wait the mutex
        osMutexWait(g_tAppMutexId, osWaitForever);
        
        // output the current tick
        printf("Current tick %d\n", osKernelSysTick());
    }
}

/*************************************************************************
* FUNCTION:
*   Main_AppTimer
*
* DESCRIPTION:
*   the timeout function of AppTimer
*
* PARAMETERS
*   1. argu     : [In] the pointer of argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppTimer(void const *argu)
{
    // release the semaphore
    osSemaphoreRelease(g_tAppSemaphoreId);
}
