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
*  OPL1000 series patch
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
#include "hal_flash_internal.h"
#include "hal_i2c.h"
#include "hal_tmr.h"
#include "hal_wdt.h"
#include "hal_pwm.h"
#include "hal_dma.h"
#include "hal_auxadc.h"
#include "hal_auxadc_internal.h"

#include "hal_flash_patch.h"
#include "hal_system_patch.h"
#include "hal_auxadc_patch.h"
#include "hal_i2c_patch.h"
#include "hal_pwm_patch.h"
#include "hal_vic_patch.h"
#include "hal_pin.h"
#include <string.h>
#include "mw_fim_default_group03.h"
#include "mw_fim_default_group03_patch.h"


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
    // vic and GPIO
    Hal_Vic_Func_Patch();

    // system (AOS+sys_reg)
    Hal_Sys_SleepInit         = Hal_Sys_SleepInit_patch;
    Hal_Sys_ApsClkTreeSetup   = Hal_Sys_ApsClkTreeSetup_patch;
    Hal_Sys_MsqClkTreeSetup   = Hal_Sys_MsqClkTreeSetup_patch;
    Hal_Sys_ApsClkChangeApply = Hal_Sys_ApsClkChangeApply_patch;
    Hal_SysPinMuxAppInit      = Hal_SysPinMuxAppInit_patch;
    Hal_SysPinMuxDownloadInit = Hal_SysPinMuxDownloadInit_patch;
    Hal_SysPinMuxSpiFlashInit = Hal_SysPinMuxSpiFlashInit_patch;
    Hal_SysPinMuxM3UartSwitch = Hal_SysPinMuxM3UartSwitch_impl;
    Hal_Sys_DisableClock      = Hal_Sys_DisableClock_impl;
    // dbg_uart

    // uart

    // spi

    // flash
    Hal_Flash_AddrProgram_Internal = Hal_Flash_AddrProgram_Internal_patch;
    Hal_Flash_AddrRead_Internal    = Hal_Flash_AddrRead_Internal_patch;

    // i2c
    _Hal_I2c_Eanble       = _Hal_I2c_Eanble_patch;
    Hal_I2c_MasterInit    = Hal_I2c_MasterInit_patch;
    Hal_I2c_TargetAddrSet = Hal_I2c_TargetAddrSet_patch;
    Hal_I2c_MasterTrasmit = Hal_I2c_MasterTrasmit_patch;
    Hal_I2c_MasterReceive = Hal_I2c_MasterReceive_patch;
    Hal_I2c_SpeedSet      = Hal_I2c_SpeedSet_patch;

    // tmr

    // wdt

    // pwm
    Hal_Pwm_Init = Hal_Pwm_Init_patch;

    // dma
    
    // auxadc
    g_ulHalAux_AverageCount = HAL_AUX_AVERAGE_COUNT;
    memcpy(&g_tHalAux_CalData_patch, &g_tMwFimDefaultCalAuxadc_patch, MW_FIM_CAL_AUXADC_SIZE);
    Hal_Aux_Init = Hal_Aux_Init_patch;
    Hal_Aux_AdcValueGet = Hal_Aux_AdcValueGet_patch;
    Hal_Aux_VbatGet = Hal_Aux_VbatGet_patch;
    Hal_Aux_IoVoltageGet = Hal_Aux_IoVoltageGet_patch;
    Hal_Aux_VbatCalibration = Hal_Aux_VbatCalibration_impl;
    Hal_Aux_IoVoltageCalibration = Hal_Aux_IoVoltageCalibration_impl;

    // pin-mux
    Hal_Pin_PreInitCold();
}
