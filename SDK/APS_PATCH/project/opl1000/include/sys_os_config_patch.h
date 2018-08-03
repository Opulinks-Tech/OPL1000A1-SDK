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

/***********************
Head Block of The File
***********************/
#ifndef _SYS_OS_CONFIG_PATCH_H_
#define _SYS_OS_CONFIG_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "sys_os_config.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// Task - Priority, the type of cmsis_os priority
#define OS_TASK_PRIORITY_AGENT          osPriorityLow

// Task - Stack Size, the count of 4 bytes
#define OS_TASK_STACK_SIZE_TRACER_PATCH (128)
#define OS_TASK_STACK_SIZE_AGENT        (128)


// Task - Name (max length is 15 bytes (not including '\0'))
#define OS_TASK_NAME_AGENT              "opl_agent"


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _SYS_OS_CONFIG_PATCH_H_
