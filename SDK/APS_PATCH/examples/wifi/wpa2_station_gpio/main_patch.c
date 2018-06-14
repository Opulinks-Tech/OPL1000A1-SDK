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
#include "wpa2_station_app.h"
#include "msg_patch.h"


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
extern T_TracerTaskInfo g_taTracerIntTaskInfoBody[];

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
    /*UART0 Init*/
    Hal_PinMux_Uart_Init(&OPL1000_periph.uart[0]);
    
    /*IO4 Init*/
    Hal_Pinmux_Gpio_Init(&OPL1000_periph.gpio[0]);
}


void Internal_Module_Log_Config(char* module_name, bool on_off_set)
{
	  uint8_t log_level_set,i,module_index; 	
	
    if(on_off_set == true) 
        log_level_set = LOG_ALL_LEVEL;
    else
        log_level_set = LOG_NONE_LEVEL;	
    
		for (i = 0; i < TRACER_INT_TASK_NUM_MAX; i++) 
		{
			if (strcmp(module_name,g_taTracerIntTaskInfoBody[i].baName) == 0)
			{
				module_index = i;
				break;
			}
		}
		if(module_index < TRACER_INT_TASK_NUM_MAX) 
		{
		    g_taTracerIntTaskInfoBody[module_index].bLevel = log_level_set;
    } 
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

    Internal_Module_Log_Config("wifi_mac",true);			
    Internal_Module_Log_Config("controller_task",true);
    Internal_Module_Log_Config("event_loop",true);	
	    
    // wifi init
    WifiAppInit();
}





