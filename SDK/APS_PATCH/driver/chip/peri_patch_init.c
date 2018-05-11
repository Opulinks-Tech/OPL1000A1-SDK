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
*  peri_patch_init.c
*
*  Project:
*  --------
*  NL1000_A0 series patch
*
*  Description:
*  ------------
*  This source file defines the patch functions for peri_patch_init.
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include "hal_vic.h"
#include "hal_system.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "hal_spi.h"
#include "hal_flash.h"
#include "hal_i2c.h"
#include "hal_tmr.h"
#include "hal_wdt.h"
#include "hal_pwm.h"
#include "hal_dma.h"

#include "hal_system_patch.h"
#include "hal_vic_patch.h"
#include "hal_tmr_patch.h"
#include "hal_dbg_uart_patch.h"
#include "hal_uart_patch.h"
#include "hal_flash_patch.h"
#include "hal_wdt_patch.h"
#include "hal_i2c_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...

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


/*************************************************************************
* FUNCTION:
*  peripheral_patch_init
* 
* DESCRIPTION:
*   1. The function implements initial of peripheral patch function.
* 
* CALLS
* 
* PARAMETERS
* 
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void peripheral_patch_init(void)
{
    E_GpioIdx_t eGpioIdx = GPIO_IDX_00;

    // vic and GPIO
    for(eGpioIdx = GPIO_IDX_00; eGpioIdx<GPIO_IDX_MAX; eGpioIdx++)
    {
        GpioCallBack[eGpioIdx] = 0;
    }

    // system (AOS+sys_reg)
    Hal_Sys_SleepTimerGet       = Hal_Sys_SleepTimerGet_patch;
    Hal_Sys_ApsModuleRst        = Hal_Sys_ApsModuleRst_patch;
    Hal_Sys_MsqModuleRst        = Hal_Sys_MsqModuleRst_patch;
    Hal_Sys_PwmSrcSelect        = Hal_Sys_PwmSrcSelect_patch;
    Hal_SysPinMuxAppInit        = Hal_SysPinMuxAppInit_patch;
    Hal_Sys_ApsClkChangeApply   = Hal_Sys_ApsClkChangeApply_patch;

    // dbg_uart
    Hal_DbgUart_BaudRateSet = Hal_DbgUart_BaudRateSet_patch;
    Hal_DbgUart_RxIntEn     = Hal_DbgUart_RxIntEn_patch;

    // uart
    Hal_Uart_Init        = Hal_Uart_Init_patch;
    Hal_Uart_BaudRateSet = Hal_Uart_BaudRateSet_patch;

    // spi

    // flash
    Hal_Flash_Init              = Hal_Flash_Init_patch;
    Hal_Flash_4KSectorAddrErase = Hal_Flash_4KSectorAddrErase_patch;
    Hal_Flash_4KSectorIdxErase  = Hal_Flash_4KSectorIdxErase_patch;
    Hal_Flash_PageAddrProgram   = Hal_Flash_PageAddrProgram_patch;
    Hal_Flash_PageIdxProgram    = Hal_Flash_PageIdxProgram_patch;
    Hal_Flash_PageAddrRead      = Hal_Flash_PageAddrRead_patch;
    Hal_Flash_Reset             = Hal_Flash_Reset_patch;

    // i2c
    Hal_I2c_MasterInit = Hal_I2c_MasterInit_patch;

    // tmr
    Tmr_CallBack[0] = NULL;
    Tmr_CallBack[1] = NULL;
    Hal_Tmr_Init          = Hal_Tmr_Init_patch;
    Hal_Tmr_Start         = Hal_Tmr_Start_patch;
    Hal_Tmr_ReadCurrValue = Hal_Tmr_ReadCurrValue_patch;

    // wdt
    Hal_Wdt_Init       = Hal_Wdt_Init_patch;
    Hal_Wdt_InitForInt = Hal_Wdt_InitForInt_patch;
    Hal_Wdt_Start      = Hal_Wdt_Start_patch;
    Hal_Wdt_Stop       = Hal_Wdt_Stop_patch;
    Hal_Wdt_Feed       = Hal_Wdt_Feed_patch;
    Hal_Wdt_Clear      = Hal_Wdt_Clear_patch;

    // pwm

    // dma

}
