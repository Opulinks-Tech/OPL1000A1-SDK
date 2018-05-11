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
*  hal_tmr.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of Timer functions
*
*  Author:
*  -------
*  Luke Liao
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include "nl1000.h"
#include "hal_system.h"
#include "hal_tmr.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define TMR0                ((S_Tmr_Reg_t *) TIM0_BASE)
#define TMR1                ((S_Tmr_Reg_t *) TIM1_BASE)

#define TIMER_MAX_VALUE     0xFFFFFFFF
#define TIMER_TICKS_US      (SystemCoreClockGet()/1000000)

#define TIMER_CTRL_EN             (0x01 << 0)
#define TIMER_CTRL_IRQEN          (0x01 << 3)


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef struct
{
	volatile uint32_t  CTRL;              // 0x000
	volatile uint32_t  VALUE;             // 0x004
	volatile uint32_t  LOAD;              // 0x008
	volatile uint32_t  INTSTATUS;         // 0x00C
} S_Tmr_Reg_t;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
RET_DATA T_Hal_Tmr_Init Hal_Tmr_Init;
RET_DATA T_Hal_Tmr_Start Hal_Tmr_Start;
RET_DATA T_Hal_Tmr_Stop Hal_Tmr_Stop;
RET_DATA T_Hal_Tmr_ReadCurrValue Hal_Tmr_ReadCurrValue;

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
*  Hal_Tmr_Init
*
* DESCRIPTION:
* This function is mainly used to initialize the Timer0 or Timer1 hardware.
*
* CALLS
*
* PARAMETERS
*   1. u32Timer: timer number 0 or 1
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Tmr_Init_impl(uint32_t u32Timer)
{
    switch(u32Timer) {
        case 0:
			Hal_Sys_ApsModuleRst(ASP_RST_TIMER_0);
			Hal_Sys_ApsClkEn(1, APS_CLK_TIMER_0);
        	NVIC_EnableIRQ(TIM0_IRQn);
            break;
        case 1:
			Hal_Sys_ApsModuleRst(ASP_RST_TIMER_1);
			Hal_Sys_ApsClkEn(1, APS_CLK_TIMER_1);
        	NVIC_EnableIRQ(TIM1_IRQn);
            break;
        default: 
        	break;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Tmr_Start
*
* DESCRIPTION:
* This function is mainly used to start the Timer0 or Timer1 hardware.
*
* CALLS
*
* PARAMETERS
*   1. u32Timer: timer number 0 or 1
*	2. u32Us: timer reload value in us - 0 to reload to timer max value
* RETURNS
*   None
*
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Tmr_Start_impl(uint32_t u32Timer, uint32_t u32Us)
{
    uint32_t reload = 0;

        if (u32Us == 0)
            reload = TIMER_MAX_VALUE;
        else
            reload = u32Us * TIMER_TICKS_US;

    switch(u32Timer) {
        case 0:
        	TMR0->CTRL = 0;
        	TMR0->LOAD = reload;
        	TMR0->CTRL |= (TIMER_CTRL_EN | TIMER_CTRL_IRQEN);
            break;
        case 1:
        	TMR1->CTRL = 0;
        	TMR1->LOAD = reload;
        	TMR1->CTRL |= (TIMER_CTRL_EN | TIMER_CTRL_IRQEN);
            break;
        default: 
        	break;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Tmr_Stop
*
* DESCRIPTION:
* This function is mainly used to stop the Timer0 or Timer1 hardware.
*
* CALLS
*
* PARAMETERS
*   1. u32Timer: timer number 0 or 1
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Tmr_Stop_impl(uint32_t u32Timer)
{
    switch(u32Timer) {
        case 0:
        	TMR0->CTRL = 0;
            break;
        case 1:
        	TMR1->CTRL = 0;
            break;
        default: 
        	break;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Tmr_ReadCurrValue
*
* DESCRIPTION:
* This function is mainly used to read the Timer0 or Timer1 value.
*
* CALLS
*
* PARAMETERS
*   1. u32Timer: timer number 0 or 1
*
* RETURNS
*   timer value us
*
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Tmr_ReadCurrValue_impl(uint32_t u32Timer)
{
    switch(u32Timer) {
        case 0:
            return (TMR0->VALUE / TIMER_TICKS_US);
        case 1:
            return (TMR1->VALUE / TIMER_TICKS_US);
        default: 
        	break;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Tmr_Pre_Init
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
void Hal_Tmr_Pre_Init(void)
{
    Hal_Tmr_Init = Hal_Tmr_Init_impl;
    Hal_Tmr_Start = Hal_Tmr_Start_impl;
    Hal_Tmr_Stop = Hal_Tmr_Stop_impl;
    Hal_Tmr_ReadCurrValue = Hal_Tmr_ReadCurrValue_impl;
}
