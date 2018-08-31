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
*  OPL1000 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of vic functions
*
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_vic_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define PIN             ((S_App_Pin_Config_t *) (AOS_BASE + 0x090))


#define BITBAND_ADDR(addr, bitnum)      ( ((addr) & 0xF0000000) + 0x2000000 + (((addr) & 0xFFFFF) << 5) + ((bitnum) << 2) )
#define REG_BB_READ(addr, bitnum)       ( reg_read(BITBAND_ADDR((uint32_t)(addr), (uint32_t)(bitnum))) )
#define REG_BB_WRITE(addr, bitnum, val) ( reg_write(BITBAND_ADDR((uint32_t)(addr), (uint32_t)(bitnum)), ((uint32_t)(val) & 1)) )


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

// GPIO module


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
E_GpioLevel_t Hal_Vic_GpioInput_patch(E_GpioIdx_t eIdx)
{
    return (E_GpioLevel_t)REG_BB_READ(&PIN->RG_GPI, eIdx);
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
void Hal_Vic_GpioOutput_patch(E_GpioIdx_t eIdx, E_GpioLevel_t tLevel)
{
    REG_BB_WRITE(&PIN->RG_GPO, eIdx, tLevel);
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
void Hal_Vic_GpioDirection_patch(E_GpioIdx_t eIdx, E_GpioDirection_t tDirection)
{
    REG_BB_WRITE(&PIN->RG_PD_DIR, eIdx, tDirection);
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
void Hal_Vic_GpioPinmux_patch(E_GpioIdx_t eIdx, E_GpioDirection_t tDirection, E_GpioLevel_t tOutputLevel)
{
    //input enable
    REG_BB_WRITE(&PIN->RG_PD_IE, eIdx, 1);
    // pull-up / pull-down disable
    REG_BB_WRITE(&PIN->RG_PD_PE, eIdx, 0);
    // output level
    REG_BB_WRITE(&PIN->RG_GPO, eIdx, tOutputLevel);

    // GPIO direction
    REG_BB_WRITE(&PIN->RG_PD_DIR, eIdx, tDirection);

    // GPIO pin
    REG_BB_WRITE(&PIN->RG_PDOC_MODE, eIdx, 0);
    REG_BB_WRITE(&PIN->RG_PDOV_MODE, eIdx, 0);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_Pre_Init
*
* DESCRIPTION:
*   1. Inital of functuin pointers
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
void Hal_Vic_Func_Patch(void)
{
    Hal_Vic_GpioInput     = Hal_Vic_GpioInput_patch;
    Hal_Vic_GpioOutput    = Hal_Vic_GpioOutput_patch;
    Hal_Vic_GpioDirection = Hal_Vic_GpioDirection_patch;
    Hal_Vic_GpioPinmux    = Hal_Vic_GpioPinmux_patch;

}
