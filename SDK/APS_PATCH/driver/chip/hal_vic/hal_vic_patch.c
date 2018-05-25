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
*  hal_vic_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the patch functions of vic.
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
#include "nl1000.h"
#include "hal_vic.h"
#include "hal_vic_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define PIN     ((S_App_Pin_Config_t *) (AOS_BASE + 0x090))


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef struct
{
    volatile uint32_t RG_PD_IE;           // 0x090
    volatile uint32_t RG_PD_PE;           // 0x094
    volatile uint32_t RG_PD_O_INV;        // 0x098
    volatile uint32_t RG_PD_DS;           // 0x09C
    volatile uint32_t RG_GPO;             // 0x0A0
    volatile uint32_t RG_PD_I_INV;        // 0x0A4
    volatile uint32_t RG_PDOV_MODE;       // 0x0A8
    volatile uint32_t RG_PD_DIR;          // 0x0AC
    volatile uint32_t RG_PD_OENP_INV;     // 0x0B0
    volatile uint32_t RG_PDOC_MODE;       // 0x0B4
    volatile uint32_t RG_GPI;             // 0x0B8
    volatile uint32_t reserve_0bc;        // 0x0BC, reserved
    volatile uint32_t RG_PDI_SRC_IO_A;    // 0x0C0
    volatile uint32_t RG_PDI_SRC_IO_B;    // 0x0C4
    volatile uint32_t RG_PDI_SRC_IO_C;    // 0x0C8
    volatile uint32_t RG_PDI_SRC_IO_D;    // 0x0CC
    volatile uint32_t RG_PTS_INMUX_A;     // 0x0D0
    volatile uint32_t RG_PTS_INMUX_B;     // 0x0D4
    volatile uint32_t RG_PTS_INMUX_C;     // 0x0D8
    volatile uint32_t RG_PTS_INMUX_D;     // 0x0DC
} S_App_Pin_Config_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
RET_DATA T_Gpio_CallBack g_taHalVicGpioCallBack[GPIO_IDX_MAX];

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
*  Hal_Vic_GpioCallBackFuncSet
*
* DESCRIPTION:
*   1. Setup GPIO callback function
*
* CALLS
*
* PARAMETERS
*   1. eIdx : Index of call-back GPIO
*   2. tFunc: Callback function
*
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioCallBackFuncSet(E_GpioIdx_t eIdx, T_Gpio_CallBack tFunc)
{
    if(eIdx >= GPIO_IDX_MAX)
        return;
    
    g_taHalVicGpioCallBack[eIdx] = tFunc;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_Init
*
* DESCRIPTION:
*   1. Inital of all GPIO setting. (ie.enable interrupt)
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioInit(void)
{
    // VIC 1) Clear interrupt
    Hal_Vic_IntClear(GPIO_IRQn);
    // VIC 2) Enable VIC
    Hal_Vic_IntEn(GPIO_IRQn, 1);
    // VIC 3) un-Mask VIC
    Hal_Vic_IntMask(GPIO_IRQn, 0);

    // NVIC 1) Clean NVIC
    NVIC_ClearPendingIRQ(GPIO_IRQn);
    // NVIC 2) Enable NVIC
    NVIC_EnableIRQ(GPIO_IRQn);
    // NVIC 3) Set prority
    NVIC_SetPriority(GPIO_IRQn, IRQ_PRIORITY_GPIO);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioInput
*
* DESCRIPTION:
*   get the input value
*
* CALLS
*
* PARAMETERS
*   1. eIdx : Index of call-back GPIO
*
* RETURNS
*   GPIO_LEVEL_LOW  : low
*   GPIO_LEVEL_HIGH : high
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
E_GpioLevel_t Hal_Vic_GpioInput(E_GpioIdx_t eIdx)
{
    volatile uint32_t ulRet;
    
    ulRet = PIN->RG_GPI;
    ulRet &= (0x1 << eIdx);
    ulRet = ulRet >> eIdx;
    
    return (E_GpioLevel_t)ulRet;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioOutput
*
* DESCRIPTION:
*   set the output level
*
* CALLS
*
* PARAMETERS
*   1. eIdx     : Index of call-back GPIO
*   2. tLevel   : the output level
*
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioOutput(E_GpioIdx_t eIdx, E_GpioLevel_t tLevel)
{
    volatile uint32_t tmp;
    
    tmp = PIN->RG_GPO;
    tmp &= ~(0x1 << eIdx);
    tmp |= (tLevel << eIdx);
    PIN->RG_GPO = tmp;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioDirection
*
* DESCRIPTION:
*   set the GPIO direction
*
* CALLS
*
* PARAMETERS
*   1. eIdx         : Index of call-back GPIO
*   2. tDirection   : the GPIO direction
*
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioDirection(E_GpioIdx_t eIdx, E_GpioDirection_t tDirection)
{
    volatile uint32_t tmp;
    
    tmp = PIN->RG_PD_DIR;
    tmp &= ~(0x1 << eIdx);
    tmp |= (tDirection << eIdx);
    PIN->RG_PD_DIR = tmp;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioPinmux
*
* DESCRIPTION:
*   set the pinmux to input or output
*
* CALLS
*
* PARAMETERS
*   1. eIdx           : Index of call-back GPIO
*   2. tDirection     : the GPIO direction
*   2. tOutputLevel   : the output level
*
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioPinmux(E_GpioIdx_t eIdx, E_GpioDirection_t tDirection, E_GpioLevel_t tOutputLevel)
{
    volatile uint32_t tmp;
    
    // input enable
    tmp = PIN->RG_PD_IE;
    tmp |= (0x1 << eIdx);
    PIN->RG_PD_IE = tmp;

    // pull-up / pull-down disable
    tmp = PIN->RG_PD_PE;
    tmp &= ~(0x1 << eIdx);
    PIN->RG_PD_PE = tmp;

    // output level
    tmp = PIN->RG_GPO;
    tmp &= ~(0x1 << eIdx);
    tmp |= (tOutputLevel << eIdx);
    PIN->RG_GPO = tmp;

    // GPIO direction
    tmp = PIN->RG_PD_DIR;
    tmp &= ~(0x1 << eIdx);
    tmp |= (tDirection << eIdx);
    PIN->RG_PD_DIR = tmp;

    // GPIO pin
    tmp = PIN->RG_PDOC_MODE;
    tmp &= ~(0x1 << eIdx);
    PIN->RG_PDOC_MODE = tmp;

    tmp = PIN->RG_PDOV_MODE;
    tmp &= ~(0x1 << eIdx);
    PIN->RG_PDOV_MODE = tmp;
}
