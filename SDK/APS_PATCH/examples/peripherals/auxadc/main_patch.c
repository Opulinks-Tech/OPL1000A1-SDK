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
#include "hal_auxadc.h"

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
    uint32_t ulCount;
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

// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
void Main_AppInit_patch(void);


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
static osThreadId g_tAppThread_1;
static void Main_AppThread_1(void *argu);

void Main_AppInit_patch(void)
{
    osThreadDef_t tThreadDef;
    
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

}

#define AUX_GPIO_IDX 2

static void Main_AppThread_1(void *argu)
{
    float fVbat;
    float fIoVoltage;
    
    Hal_Aux_Init();
    
    while (1) {
        
        uint8_t ret = Hal_Aux_VbatGet(&fVbat);
        if (ret == HAL_AUX_OK) {
            printf("Get bat voltage : %f \r\n", fVbat);
        } else {
            printf("error bat ...\r\n");
        }
        osDelay(2000);
        
        ret = Hal_Aux_IoVoltageGet(AUX_GPIO_IDX, &fIoVoltage);
        if (ret == HAL_AUX_OK) {
            printf("Get io %d voltage : %f \r\n", AUX_GPIO_IDX, fIoVoltage);
        } else {
            printf("error io ...\r\n");
        }
        osDelay(2000);
        
        printf("\r\n");
    }
}
