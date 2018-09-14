/******************************************************************************
*  Copyright 2017, Netlink Communication Corp.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/

/******************************************************************************
*  Filename:
*  ---------
*  hal_system_patch.h
*
*  Project:
*  --------
*  NL1000_A1 series
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

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef void (*T_Hal_SysPinMuxM3UartSwitch)(void);
typedef void (*T_Hal_Sys_DisableClock)(void);

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

/* Sleep Mode relative */
void Hal_Sys_SleepInit_patch(void);

/* Pin-Mux relative*/
void Hal_SysPinMuxAppInit_patch(void);
void Hal_SysPinMuxDownloadInit_patch(void);
void Hal_SysPinMuxSpiFlashInit_patch(void);
void Hal_SysPinMuxM3UartSwitch_impl(void);
extern T_Hal_SysPinMuxM3UartSwitch Hal_SysPinMuxM3UartSwitch;

/* Ret RAM relative*/

/* Xtal fast starup relative */

/* SW reset relative */

/* Clock relative */
uint32_t Hal_Sys_ApsClkTreeSetup_patch(E_ApsClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn, uint8_t u8PclkDivEn );
uint32_t Hal_Sys_MsqClkTreeSetup_patch(E_MsqClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn );
void Hal_Sys_ApsClkChangeApply_patch(void);
void Hal_Sys_DisableClock_impl(void);
extern T_Hal_Sys_DisableClock Hal_Sys_DisableClock;

/* Remap relative */

/* Miscellaneous */

#endif
