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
*  SH SDK
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init.h"
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "example_mdns.h"
#include "msg_patch.h"
#include "mw_fim.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the number of elements in the message queue


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the content of message queue


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern T_TracerTaskInfoExt g_taTracerDefIntTaskInfoBody[TRACER_INT_TASK_NUM_MAX];
extern T_TracerLogLevelSetFp tracer_log_level_set_ext;
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
static void Main_FlashLayoutUpdate(void);

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

void Internal_Module_Log_Config(char* module_name, bool on_off_set)
{
	  uint8_t log_level_set,i,module_index = TRACER_INT_TASK_NUM_MAX; 	
	
    if(on_off_set == true) 
        log_level_set = LOG_ALL_LEVEL;
    else
        log_level_set = LOG_NONE_LEVEL;	
    
    for (i = 0; i < TRACER_INT_TASK_NUM_MAX; i++) 
    {
        if (strcmp(module_name,g_taTracerDefIntTaskInfoBody[i].baName) == 0)
        {
            module_index = i;
            break;
        }
    }
    if(module_index < TRACER_INT_TASK_NUM_MAX) 
    {
        tracer_log_level_set_ext(module_index, log_level_set);
    } 
} 

void Main_AppInit_patch(void)
{
    Internal_Module_Log_Config("opl_wifi_mac",true);
    Internal_Module_Log_Config("opl_controller_task",true);
    Internal_Module_Log_Config("opl_event_loop",true);
    
    WifiAppInit();
}
