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
#define SYS_SPARE_0_M0_FLASH_ACCESS_DONE    6 // bit 6

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, union, enum, linked list...
typedef enum 
{
    // ASP_CLKTREE_SRC_RC_BB, 
    // ASP_CLKTREE_SRC_XTAL, 
    // ASP_CLKTREE_SRC_XTAL_X2, 
    // ASP_CLKTREE_SRC_XTAL_X4,
    // ASP_CLKTREE_SRC_DECI,
    // ASP_CLKTREE_SRC_1P2G_DIV,
    // ASP_CLKTREE_SRC_EXTERNAL
    
    ASP_CLKTREE_SRC_1P2G_MIN = 7,
    ASP_CLKTREE_SRC_1P2G_078MHZ = ASP_CLKTREE_SRC_1P2G_MIN,
    ASP_CLKTREE_SRC_1P2G_081MHZ,
    ASP_CLKTREE_SRC_1P2G_084MHZ,
    ASP_CLKTREE_SRC_1P2G_087MHZ,
    ASP_CLKTREE_SRC_1P2G_090MHZ,
    ASP_CLKTREE_SRC_1P2G_093MHZ,
    ASP_CLKTREE_SRC_1P2G_097MHZ,
    ASP_CLKTREE_SRC_1P2G_101MHZ,
    ASP_CLKTREE_SRC_1P2G_106MHZ,
    ASP_CLKTREE_SRC_1P2G_110MHZ,
    ASP_CLKTREE_SRC_1P2G_116MHZ,
    ASP_CLKTREE_SRC_1P2G_122MHZ,
    ASP_CLKTREE_SRC_1P2G_128MHZ,
    ASP_CLKTREE_SRC_1P2G_135MHZ,
    ASP_CLKTREE_SRC_1P2G_143MHZ,
    ASP_CLKTREE_SRC_1P2G_152MHZ,
    ASP_CLKTREE_SRC_1P2G_MAX = ASP_CLKTREE_SRC_1P2G_152MHZ
} E_ApsClkTreeSrc_Ext_t;

typedef void (*T_Hal_SysPinMuxM3UartSwitch)(void);
typedef void (*T_Hal_Sys_DisableClock)(void);
typedef uint8_t *(*T_Hal_Sys_OtpRead)(uint16_t u16Offset, uint8_t *u8aBuf, uint16_t u16BufSize);

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
uint32_t Hal_Sys_PowerDefaultSettings_patch(void);

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

void Hal_Sys_ApsClkStore( void );
void Hal_Sys_ApsClkResume( void );

/* Remap relative */

/* Miscellaneous */
uint8_t *Hal_Sys_OtpRead_impl(uint16_t u16Offset, uint8_t *u8aBuf, uint16_t u16BufSize);
extern T_Hal_Sys_OtpRead Hal_Sys_OtpRead;

#endif
