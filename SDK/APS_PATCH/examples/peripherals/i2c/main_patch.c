/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
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
*  OPL1000 Project - the main patch implement file
*
*  Description:
*  ------------
*  This implement file is include the main patch function and api.
*
*  Author:
*  -------
*  SH SDK
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file
/******************************************************************************
*  Test code brief
*  These examples show how to configure i2c settings and use i2c driver APIs.
*
*  i2c_eeprom_test() is an example that read and write data on 24aa128 EEPROM
*  - port: I2C
*  - interrupt: off
*  - mode: master
*  - pin assignment: SCL(io10), SDA(io11)
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sys_init.h"
#include "sys_init_patch.h"
#include "mw_fim.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_i2c.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define EEPROM_SIZE     0x4000      // 128Kb


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


// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
static void Main_FlashLayoutUpdate(void);
void Main_AppInit_patch(void);
static void Main_AppThread(void *argu);
static void i2c_eeprom_test(void);


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
    
    // update the flash layout
    MwFim_FlashLayoutUpdate = Main_FlashLayoutUpdate;
    
    // application init
    Sys_AppInit = Main_AppInit_patch;
}

/*************************************************************************
* FUNCTION:
*   Main_FlashLayoutUpdate
*
* DESCRIPTION:
*   update the flash layout
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_FlashLayoutUpdate(void)
{
    // update here
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

    Hal_Pinmux_I2c_Slave_Init(&OPL1000_periph.i2c[0]);

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
    // init the pin assignment
    App_Pin_InitConfig();
    
    // do the i2c_eeprom_test
    i2c_eeprom_test();
}

/*************************************************************************
* FUNCTION:
*   Main_AppThread
*
* DESCRIPTION:
*   the application thread 
*
* PARAMETERS
*   1. argu     : [In] the input argument
*
* RETURNS
*   none
*
*************************************************************************/
static void Main_AppThread(void *argu)
{
    uint16_t uwAddr = 0;
    uint8_t ubaData[4];
    
    while (1)
    {
        // SDA: (w)8 bits high byte address | (w)8 bits low byte address | (w)8 bits data
        
        printf("I2C Running\n");
			
        // write the data
        ubaData[0] = (uwAddr >> 8) & 0xFF;              // high byte address
        ubaData[1] = uwAddr & 0xFF;                     // low byte address
        ubaData[2] = 0xFF - (uwAddr & 0xFF);            // data
        Hal_I2c_MasterTrasmit(ubaData, 3, 1);
        
        // SDA: (w)8 bits high byte address | (w)8 bits low byte address
        // SDA: (r)8 bits data
        
        // read the data
        Hal_I2c_MasterTrasmit(ubaData, 2, 1);
        Hal_I2c_MasterReceive(&ubaData[3], 1, 1);
        
        // show the data
        printf("Addr[0x%X], Write[%d], Read[%d]\n", uwAddr, ubaData[2], ubaData[3]);
        
        // the next address
        uwAddr++;
        if (uwAddr >= EEPROM_SIZE)
            uwAddr = 0;
        
        osDelay(1000);      // delay 1000ms (1sec)
    }
}

/*************************************************************************
* FUNCTION:
*   i2c_eeprom_test
*
* DESCRIPTION:
*   an example that read and write data on 24aa128 EEPROM
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void i2c_eeprom_test(void)
{
    osThreadDef_t tThreadDef;
    
    // create the thread for AppThread_1
    tThreadDef.name = "App";
    tThreadDef.pthread = Main_AppThread;
    tThreadDef.tpriority = OS_TASK_PRIORITY_APP;        // osPriorityNormal
    tThreadDef.instances = 0;                           // reserved, it is no used
    tThreadDef.stacksize = OS_TASK_STACK_SIZE_APP;      // (512), unit: 4-byte, the size is 512*4 bytes
    g_tAppThread_1 = osThreadCreate(&tThreadDef, NULL);
    if (g_tAppThread_1 == NULL)
    {
        printf("To create the thread for AppThread is fail.\n");
    }
    
}
