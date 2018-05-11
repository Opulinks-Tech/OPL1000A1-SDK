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
*  hal_system_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the patch proto-types of system functions
*  Include sys-reg and AOS domains.
*
*  Author:
*  -------
*  Chung-chun Wang
******************************************************************************/

#ifndef __HAL_SYSTEM_PATCH_H__
#define __HAL_SYSTEM_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include <stdint.h>
#include "hal_system.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous

// 0x80
#define XTAL_32K_GM_POS   3
#define XTAL_32K_GM_MAX   16
#define XTAL_32K_GM_MASK  (0xF << XTAL_32K_GM_POS)
#define XTAL_32K_CAP_POS  11
#define XTAL_32K_CAP_MAX   16
#define XTAL_32K_CAP_MASK  (0xF << XTAL_32K_CAP_POS)

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
// 0x24
typedef enum 
{
    PMU_SF_VAL_0P65V = 0,
    PMU_SF_VAL_0P73V = 1, 
    PMU_SF_VAL_0P95V = 4, 
    PMU_SF_VAL_1P10V = 6,
    
    PMU_SF_VAL_MASK = 0x7
} E_PmuSfVal_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable

// Sec 5: declaration of global function prototype

/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable

// Sec 7: declaration of static function prototype

/***********
C Functions
***********/
// Sec 8: C Functions
/* Power relative */
uint32_t Hal_Sys_PmuSfVolSet(E_PmuSfVal_t eVol);
uint32_t Hal_Sys_PwrDefaultSettings(void);
uint32_t Hal_Sys_PtatEn(uint8_t u8Enable);

/* Sleep Mode relative */
uint64_t Hal_Sys_SleepTimerGet_patch(void);

/* Pin-Mux relative*/
void Hal_SysPinMuxAppInit_patch(void);
void Hal_SysPinMuxDownloadInit(void);
void Hal_SysPinMuxSpiFlashInit(void);

/* Ret RAM relative*/

/* Xtal fast starup relative */
uint32_t Hal_Sys_RcCal(void);
uint32_t Hal_Sys_FastStartUpEn(uint8_t u8Enable);

/* SW reset relative */
uint32_t Hal_Sys_ApsModuleRst_patch(E_ApsRstModule_t eModule);
uint32_t Hal_Sys_MsqModuleRst_patch(E_MsqRstModule_t eModule);

/* Clock relative */
uint32_t Hal_Sys_PwmSrcSelect_patch(E_PwmClkSrc_t eSrc);
uint32_t Hal_Sys_32kXtalGainSet(uint8_t u8Gain);
uint32_t Hal_Sys_32kXtalCapSet(uint8_t u8Cap);
void Hal_Sys_ApsClkChangeApply_patch(void);
/* Remap relative */

/* Miscellaneous */

#endif
