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
*  nl1000_it_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series patch
*
*  Description:
*  ------------
*  This source file defines the patch functions of ISRs.
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
#include "hal_system.h"
#include "hal_vic.h"
#include "hal_vic_patch.h"
#include "hal_uart.h"
#include "hal_tmr_patch.h"
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
typedef void (*T_InterruptHandler)(void);

// Sec 5: declaration of global function prototype
extern T_InterruptHandler IPC0_IRQHandler_Patch;
extern T_InterruptHandler IPC1_IRQHandler_Patch;
extern T_InterruptHandler IPC2_IRQHandler_Patch;
extern T_InterruptHandler IPC3_IRQHandler_Patch;
extern T_InterruptHandler GPIO_IRQHandler_Patch;
extern T_InterruptHandler SCRT_IRQHandler_Patch;
extern T_InterruptHandler MSQ_IRQHandler_Patch;
extern T_InterruptHandler UARTDBG_IRQHandler_Patch;
extern T_InterruptHandler UART0_IRQHandler_Patch;
extern T_InterruptHandler UART1_IRQHandler_Patch;
extern T_InterruptHandler I2C_IRQHandler_Patch;
extern T_InterruptHandler SPI0_IRQHandler_Patch;
extern T_InterruptHandler SPI1_IRQHandler_Patch;
extern T_InterruptHandler SPI2_IRQHandler_Patch;
extern T_InterruptHandler TIM0_IRQHandler_Patch;
extern T_InterruptHandler TIM1_IRQHandler_Patch;
extern T_InterruptHandler WDT_IRQHandler_Patch;
extern T_InterruptHandler JTAG_IRQHandler_Patch;
extern T_InterruptHandler DMA_IRQHandler_Patch;

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
*  TIM0_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of Timer_0 ISR function.
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
void GPIO_IRQHandler_Patch_patch(void)
{
    E_GpioIdx_t eGpioIdx = GPIO_IDX_00;
    uint32_t u32Status = 0;

    // Get status
    u32Status = Hal_Vic_GpioIntStatRead();

    for(eGpioIdx = GPIO_IDX_00; eGpioIdx<GPIO_IDX_MAX; eGpioIdx++)
    {
        if( u32Status & (1<<eGpioIdx) )
        {
            // Resume Mmfactor
            if( Hal_Vic_MmFactorResumeByGpioIntStatRead() | (1<<eGpioIdx) )
            {
                // Need this function to apply system clock change
                Hal_Sys_ApsClkMmFactorSet(16);
            }

            // Hook here...
            if(g_taHalVicGpioCallBack[eGpioIdx] != 0)
                g_taHalVicGpioCallBack[eGpioIdx](eGpioIdx);
            
            // Clear module interrupt
            Hal_Vic_GpioIntClear(eGpioIdx);
        }
    }

    // Clear VIC interrupt
    Hal_Vic_IntClear(GPIO_IRQn);
}

/*************************************************************************
* FUNCTION:
*  UART0_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of UART0 ISR function.
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
void UART0_IRQHandler_Patch_patch(void)
{
    // Note: Level-sensitive interrupt
    uint32_t u32Temp = 0;
    uint32_t u32InterruptId = 0;

    // Get status
    u32InterruptId = Hal_Uart_IntIdRead(UART_IDX_0);

    // Clear module interrupt 
    Hal_Uart_IntClear(UART_IDX_0, (E_UartIntId_t)u32InterruptId, &u32Temp);

    // Clear VIC interrupt
    Hal_Vic_IntClear(UART0_IRQn);

    if((u32InterruptId == UART_INT_RX_AVALIBLE) || (u32InterruptId == UART_INT_CHAR_TIMEOUT))
    {
        // ---- Rx interrupt case ----
        // Hook here ...
        if( Uart_0_RxCallBack != 0 )
            Uart_0_RxCallBack(u32Temp);
    }
}

/*************************************************************************
* FUNCTION:
*  UART1_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of UART1 ISR function.
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
void UART1_IRQHandler_Patch_patch(void)
{
    // Note: Level-sensitive interrupt
    uint32_t u32Temp = 0;
    uint32_t u32InterruptId = 0;

    // Get status
    u32InterruptId = Hal_Uart_IntIdRead(UART_IDX_1);

    // Clear module interrupt 
    Hal_Uart_IntClear(UART_IDX_1, (E_UartIntId_t)u32InterruptId, &u32Temp);

    // Clear VIC interrupt
    Hal_Vic_IntClear(UART1_IRQn);

    if((u32InterruptId == UART_INT_RX_AVALIBLE) || (u32InterruptId == UART_INT_CHAR_TIMEOUT))
    {
        // ---- Rx interrupt case ----
        // Hook here ...
        if( Uart_1_RxCallBack != 0 )
            Uart_1_RxCallBack(u32Temp);
    }
}

/*************************************************************************
* FUNCTION:
*  TIM0_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of Timer_0 ISR function.
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
void TIM0_IRQHandler_Patch_patch(void)
{
    // Clear module interrupt
    Hal_Tmr_IntClear(0);
    
    // Clear VIC part
    Hal_Vic_IntClear(TIM0_IRQn);
    
    // handle the timer 0 interrupt
    if(Tmr_CallBack[0] != NULL)
        Tmr_CallBack[0](0);
}

/*************************************************************************
* FUNCTION:
*  TIM1_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of Timer_1 ISR function.
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
void TIM1_IRQHandler_Patch_patch(void)
{
    // Clear module interrupt
    Hal_Tmr_IntClear(1);
    
    // Clear VIC part
    Hal_Vic_IntClear(TIM1_IRQn);
    
    // handle the timer 1 interrupt
    if(Tmr_CallBack[1] != NULL)
        Tmr_CallBack[1](1);
}

/*************************************************************************
* FUNCTION:
*  I2C_IRQHandler_Patch_patch
* 
* DESCRIPTION:
*   1. The function implements patch of I2C ISR function.
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
void I2C_IRQHandler_Patch_patch(void)
{
    // handle the I2C interrupt
    Hal_I2c_IntHandler();
    
    // Clear VIC interrupt
    Hal_Vic_IntClear(I2C_IRQn);
}

/*************************************************************************
* FUNCTION:
*  isr_patch_init
* 
* DESCRIPTION:
*   1. The function implements initial of ISR patch function.
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
void isr_patch_init(void)
{
    GPIO_IRQHandler_Patch  = GPIO_IRQHandler_Patch_patch;
    UART0_IRQHandler_Patch = UART0_IRQHandler_Patch_patch;
    UART1_IRQHandler_Patch = UART1_IRQHandler_Patch_patch;
    TIM0_IRQHandler_Patch  = TIM0_IRQHandler_Patch_patch;
    TIM1_IRQHandler_Patch  = TIM1_IRQHandler_Patch_patch;
    I2C_IRQHandler_Patch   = I2C_IRQHandler_Patch_patch;
}
