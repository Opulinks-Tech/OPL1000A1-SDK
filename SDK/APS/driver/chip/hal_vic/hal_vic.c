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
*  hal_vic.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of vic functions
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
#include "hal_vic.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define VIC             ((S_Vic_Reg_t *) VIC_BASE)

#define IPC_STA_MASK    0xF
#define IPC_STA_SHIFT   16

#define INT_TYPE_MASK   0x3

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef struct
{
    volatile uint32_t IPC_INT; // 0x000, IPC interrupt
    volatile uint32_t CTRL0;   // 0x004, Interrupt invert
    volatile uint32_t CTRL1;   // 0x008, Interrupt disable
    volatile uint32_t CTRL2;   // 0x00C, Interrupt type IRQ 0~15 
    volatile uint32_t CTRL3;   // 0x010, Interrupt type IRQ 16~31 
    volatile uint32_t CTRL4;   // 0x014, Interrupt clear
    volatile uint32_t CTRL5;   // 0x018, Interrupt mask
    volatile uint32_t CTRL6;   // 0x01C, Interrupt status
    volatile uint32_t CTRL7;   // 0x020, GPIO interrupt invert
    volatile uint32_t CTRL8;   // 0x024, GPIO interrupt disable
    volatile uint32_t CTRL9;   // 0x028, GPIO interrupt type IRQ 0~15
    volatile uint32_t CTRLA;   // 0x02C, GPIO interrupt type IRQ 16~31
    volatile uint32_t CTRLB;   // 0x030, GPIO interrupt clear
    volatile uint32_t CTRLC;   // 0x034, GPIO interrupt mask
    volatile uint32_t CTRLD;   // 0x038, GPIO interrupt status
    volatile uint32_t CTRLE;   // 0x03C, GPIO interrupt to resume mmfactor
    volatile uint32_t CTRLF;   // 0x040, IPC interrupt to resume mmfactor
} S_Vic_Reg_t;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
// VIC moudle
T_Hal_Vic_IntEn       Hal_Vic_IntEn;
T_Hal_Vic_IntClear    Hal_Vic_IntClear;
T_Hal_Vic_IntMask     Hal_Vic_IntMask;
T_Hal_Vic_IntTypeSel  Hal_Vic_IntTypeSel;
T_Hal_Vic_IntInv      Hal_Vic_IntInv;
T_Hal_Vic_IntStatRead Hal_Vic_IntStatRead;
T_Hal_Vic_Init        Hal_Vic_Init;


// IPC module
T_Hal_Vic_IpcIntTrig  Hal_Vic_IpcIntTrig;
T_Hal_Vic_IpcIntEn    Hal_Vic_IpcIntEn;
T_Hal_Vic_IpcIntClear Hal_Vic_IpcIntClear;

// GPIO module
T_Hal_Vic_GpioIntEn       Hal_Vic_GpioIntEn;
T_Hal_Vic_GpioIntClear    Hal_Vic_GpioIntClear;
T_Hal_Vic_GpioIntMask     Hal_Vic_GpioIntMask;
T_Hal_Vic_GpioIntTypeSel  Hal_Vic_GpioIntTypeSel;
T_Hal_Vic_GpioIntInv      Hal_Vic_GpioIntInv;
T_Hal_Vic_GpioIntStatRead Hal_Vic_GpioIntStatRead;

// MmFactor relative
T_Hal_Vic_MmFactorResumeByIpcIntEn        Hal_Vic_MmFactorResumeByIpcIntEn;
T_Hal_Vic_MmFactorResumeByIpcIntStatRead  Hal_Vic_MmFactorResumeByIpcIntStatRead;
T_Hal_Vic_MmFactorResumeByGpioIntEn       Hal_Vic_MmFactorResumeByGpioIntEn;
T_Hal_Vic_MmFactorResumeByGpioIntStatRead Hal_Vic_MmFactorResumeByGpioIntStatRead;

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
*  Hal_Vic_IpcIntEn
*
* DESCRIPTION:
*   1. Enable IPC interrupt
* CALLS
*
* PARAMETERS
*   1. eIpc    : Index of IPC. refert to E_IpcIdx_t
*   2. u8Enable: 1 for Enable/0 for Disable 
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IpcIntEn_impl(E_IpcIdx_t eIpc, uint8_t u8Enable)
{
    IRQn_Type eIrq = IRQn_MAX;
    uint32_t u32IrqPriorty = 0xFF;
    
    switch(eIpc)
    {
        case IPC_IDX_0:
            eIrq = IPC0_IRQn;
            u32IrqPriorty = IRQ_PRIORITY_IPC0;
            break;
        case IPC_IDX_1: 
            eIrq = IPC1_IRQn;
            u32IrqPriorty = IRQ_PRIORITY_IPC1;
            break;
        case IPC_IDX_2: 
            eIrq = IPC2_IRQn;
            u32IrqPriorty = IRQ_PRIORITY_IPC2;
            break;
        case IPC_IDX_3: 
            eIrq = IPC3_IRQn;
            u32IrqPriorty = IRQ_PRIORITY_IPC3;
            break;
        default:
            return;
    }
    
    if(u8Enable)
    {
        // VIC 1) Enable VIC
        Hal_Vic_IntEn(eIrq, 1);
        // VIC 2) un-Mask VIC
        Hal_Vic_IntMask(eIrq, 0);
        // VIC 3) Clear interrupt
        Hal_Vic_IntClear(eIrq);

        // NVIC 1) Clean NVIC
        NVIC_ClearPendingIRQ(eIrq);
        // NVIC 2) Enable NVIC
        NVIC_EnableIRQ(eIrq);
        // NVIC 3) Set prority
        NVIC_SetPriority(eIrq, u32IrqPriorty);

        // Module part: Enable module
        // No need do anything in IPC ...
    }else{
        // Module part: Disable module
        // No need do anything in IPC ...

        // VIC part: Disable VIC
        Hal_Vic_IntEn(eIrq, 0);

        // NVIC part: Disable NVIC
        NVIC_DisableIRQ(eIrq);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IpcIntTrig
*
* DESCRIPTION:
*   1. Trigger the IPC interrupt
* CALLS
*
* PARAMETERS
*   1. eIpc    : Index of IPC. refert to E_IpcIdx_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IpcIntTrig_impl(E_IpcIdx_t eIpc)
{
    if(eIpc >= IPC_IDX_MAX)
        return;
    
    VIC->IPC_INT |= (1 << eIpc);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IpcIntClear
*
* DESCRIPTION:
*   1. Clear the IPC interrupt
* CALLS
*
* PARAMETERS
*   1. eIpc    : Index of IPC. refert to E_IpcIdx_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IpcIntClear_impl(E_IpcIdx_t eIpc)
{
    IRQn_Type eIrq = IRQn_MAX;
    
    switch(eIpc)
    {
        case IPC_IDX_0: eIrq = IPC0_IRQn; break;
        case IPC_IDX_1: eIrq = IPC1_IRQn; break;
        case IPC_IDX_2: eIrq = IPC2_IRQn; break;
        case IPC_IDX_3: eIrq = IPC3_IRQn; break;
        default:
            return;
    }

    // Clean module parts
    // No need do anything in IPC ...

    // Clear VIC part
    Hal_Vic_IntClear(eIrq);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IpcIntOtherCoreStatRead
*
* DESCRIPTION:
*   1. Get the IPC interrupt status of the other core
* CALLS
*
* PARAMETERS
*   None 
*
* RETURNS
*   IPC interrupt status.  bit index refert to E_IpcIdx_t
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Vic_IpcIntOtherCoreStatRead_impl(void)
{
    return (VIC->IPC_INT >> IPC_STA_SHIFT) & IPC_STA_MASK;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntInv
*
* DESCRIPTION:
*   1. Invert interrupt signal
* CALLS
*
* PARAMETERS
*   1. eInterrupt: Index of interrupt. refert to IRQn_Type
*   2. u8Enable  : 1 for Enable/0 for Disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IntInv_impl(IRQn_Type eInterrupt, uint8_t u8Enable)
{
    if( (eInterrupt < 0) || (eInterrupt >= IRQn_MAX) )
        return;

    if(u8Enable) 
    {
        VIC->CTRL0 |= (1 << eInterrupt);
    }else{
        VIC->CTRL0 &= ~(1 << eInterrupt);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntEn
*
* DESCRIPTION:
*   1. Enable or disable interrupt signal
* CALLS
*
* PARAMETERS
*   1. eInterrupt: Index of interrupt. refert to IRQn_Type
*   2. u8Enable  : 1 for Enable/0 for Disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IntEn_impl(IRQn_Type eInterrupt, uint8_t u8Enable)
{
    if( (eInterrupt < 0) || (eInterrupt >= IRQn_MAX) )
        return;
    
    // Note: regster function is "disable"
    if(!u8Enable)
		{
        VIC->CTRL1 |= (1 << eInterrupt);
    }else{
        VIC->CTRL1 &= ~(1 << eInterrupt);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntTypeSel
*
* DESCRIPTION:
*   1. Select interrupt type
* CALLS
*
* PARAMETERS
*   1. eInterrupt: Index of interrupt. refert to IRQn_Type
*   2. eType     : Type of interrupt. refert to E_ItrType_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IntTypeSel_impl(IRQn_Type eInterrupt, E_ItrType_t eType)
{
    uint32_t u32Tmp = 0;
    uint32_t u32Shift = 0;

    if( (eInterrupt < 0) || (eInterrupt >= IRQn_MAX) )
        return;

    if(eInterrupt < 16) 
		{
        u32Shift = eInterrupt * 2;
        
        u32Tmp = VIC->CTRL2;
        u32Tmp &= ~(INT_TYPE_MASK << u32Shift);
        u32Tmp |= (eType << u32Shift);
        VIC->CTRL2 = u32Tmp;
    }else{
        u32Shift = (eInterrupt - 16) * 2;
        
        u32Tmp = VIC->CTRL3;
        u32Tmp &= ~(INT_TYPE_MASK << u32Shift);
        u32Tmp |= (eType << u32Shift);
        VIC->CTRL3 = u32Tmp;
	}
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntClear
*
* DESCRIPTION:
*   1. Clear interrupt
* CALLS
*
* PARAMETERS
*   1. eInterrupt: Index of interrupt. refert to IRQn_Type
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IntClear_impl(IRQn_Type eInterrupt)
{
    if( (eInterrupt < 0) || (eInterrupt >= IRQn_MAX) )
        return;
    
    VIC->CTRL4 |= (1 << eInterrupt);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntMask
*
* DESCRIPTION:
*   1. Mask interrupt
* CALLS
*
* PARAMETERS
*   1. eInterrupt: Index of interrupt. refert to IRQn_Type
*   2. u8Enable  : 1 for Enable/0 for Disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_IntMask_impl(IRQn_Type eInterrupt, uint8_t u8Enable)
{
    if( (eInterrupt < 0) || (eInterrupt >= IRQn_MAX) )
        return;
    
    if(u8Enable)
    {
        VIC->CTRL5 |= (1 << eInterrupt);
    }else{
        VIC->CTRL5 &= ~(1 << eInterrupt);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_IntStatRead
*
* DESCRIPTION:
*   1. Return interrupt status
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   interrupt status, each bit index refer to IRQn_Type
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Vic_IntStatRead_impl(void)
{
    return VIC->CTRL6;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntInv
*
* DESCRIPTION:
*   1. Invert gpio interrupt signal
* CALLS
*
* PARAMETERS
*   1. eGpio    : Index of GPIO. refer to E_GpioIdx_t
*   2. u8Enable : 1 for enable/ 0 for disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioIntInv_impl(E_GpioIdx_t eGpio, uint8_t u8Enable)
{
    if(eGpio >= GPIO_IDX_MAX)
        return;
    
    if(u8Enable) {
        VIC->CTRL7 |= (1 << eGpio);
    }else{
        VIC->CTRL7 &= ~(1 << eGpio);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntEn
*
* DESCRIPTION:
*   1. Enable or disable gpio interrupt signal
* CALLS
*
* PARAMETERS
*   1. eGpio    : Index of GPIO. refer to E_GpioIdx_t
*   2. u8Enable : 1 for enable/ 0 for disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioIntEn_impl(E_GpioIdx_t eGpio, uint8_t u8Enable)
{
    if(eGpio >= GPIO_IDX_MAX)
        return;
    
    // Note: regster function is "disable"
    if(!u8Enable) {
        VIC->CTRL8 |= (1 << eGpio);
    }else{
        VIC->CTRL8 &= ~(1 << eGpio);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntTypeSel
*
* DESCRIPTION:
*   1. Select gpio interrupt type
* CALLS
*
* PARAMETERS
*   1. eGpioIdx: Gpio index. Refer to E_GpioIdx_t
*   2. eType   : Type of interrupt. refert to E_ItrType_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioIntTypeSel_impl(E_GpioIdx_t eGpio, E_ItrType_t eType)
{
    uint32_t u32Tmp = 0;
    uint32_t u32Shift = 0;

    if(eGpio >= GPIO_IDX_MAX)
        return;

    if(eGpio < 16) {
        u32Shift = eGpio * 2;
        
        u32Tmp = VIC->CTRL9;
        u32Tmp &= ~(INT_TYPE_MASK << u32Shift);
        u32Tmp |= (eType << u32Shift);
        VIC->CTRL9 = u32Tmp;
    }else{
        u32Shift = (eGpio - 16) * 2;
        
        u32Tmp = VIC->CTRLA;
        u32Tmp &= ~(INT_TYPE_MASK << u32Shift);
        u32Tmp |= (eType << u32Shift);
        VIC->CTRLA = u32Tmp;
	}
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntClear
*
* DESCRIPTION:
*   1. Clear gpio interrupt
* CALLS
*
* PARAMETERS
*   1. eGpioIdx: Gpio index. Refer to E_GpioIdx_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioIntClear_impl(E_GpioIdx_t eGpio)
{
    VIC->CTRLB |= (1 << eGpio);
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntMask
*
* DESCRIPTION:
*   1. Mask gpio interrupt
* CALLS
*
* PARAMETERS
*   1. eGpioIdx: Gpio index. Refer to E_GpioIdx_t
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_GpioIntMask_impl(E_GpioIdx_t eGpio, uint8_t u8Enable)
{
    if(eGpio >= GPIO_IDX_MAX)
        return;
    
    if(u8Enable) {
        VIC->CTRLC |= (1 << eGpio);
    }else{
        VIC->CTRLC &= ~(1 << eGpio);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_GpioIntStatus
*
* DESCRIPTION:
*   1. Return gpio interrupt status
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   gpio interrupt status. refert to E_GpioIdx_t
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Vic_GpioIntStatRead_impl(void)
{
    return VIC->CTRLD;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_MmFactorResumeByGpioIntEn
*
* DESCRIPTION:
*   1. gpio interrupt notify clock modulator at full speed enable.
*      Used with Hal_Sys_ApsClkMmFactorSet
* CALLS
*
* PARAMETERS
*   1. eGpioIdx : Gpio index. Refer to E_GpioIdx_t
*   2. u8Enable : 1 for enable/ 0 for disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_MmFactorResumeByGpioIntEn_impl(E_GpioIdx_t eGpio, uint8_t u8Enable)
{
    if(eGpio >= GPIO_IDX_MAX)
        return;
    
    if(u8Enable) {
        VIC->CTRLE |= (1 << eGpio);
    }else{
        VIC->CTRLE &= ~(1 << eGpio);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_MmFactorResumeByGpioIntStatRead
*
* DESCRIPTION:
*   1. which gpio interrupt will notify clock modulator at full speed.
*      Used with Hal_Sys_ApsClkMmFactorSet
* CALLS
*
* PARAMETERS
*  None
* RETURNS
*  Gpio index. Refer to E_GpioIdx_t
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Vic_MmFactorResumeByGpioIntStatRead_impl(void)
{
    return VIC->CTRLE;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_MmFactorResumeByIpcIntEn
*
* DESCRIPTION:
*   1. ipc interrupt notify clock modulator at full speed enable
*      Used with Hal_Sys_ApsClkMmFactorSet
* CALLS
*
* PARAMETERS
*   1. eIpc       : Index of IPC. refert to E_IpcIdx_t
*   2. u8Enable : 1 for enable/ 0 for disable
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Vic_MmFactorResumeByIpcIntEn_impl(E_IpcIdx_t eIpc, uint8_t u8Enable)
{
    if(eIpc>= IPC_IDX_MAX)
        return;
    
    if(u8Enable) {
        VIC->CTRLF |= (1 << eIpc);
    }else{
        VIC->CTRLF &= ~(1 << eIpc);
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_MmFactorResumeByIpcIntStatRead
*
* DESCRIPTION:
*   1. Which ipc interrupt will notify clock modulator at full speed
*      Used with Hal_Sys_ApsClkMmFactorSet
* CALLS
*
* PARAMETERS
*  None
* RETURNS
*  Index of IPC. refert to E_IpcIdx_t
* GLOBALS AFFECTED
* 
*************************************************************************/

uint32_t Hal_Vic_MmFactorResumeByIpcIntStatRead_impl(void)
{
    return VIC->CTRLF;
}

/*************************************************************************
* FUNCTION:
*  Hal_Vic_Init
*
* DESCRIPTION:
*   1. Inital of all VIC setting. Disable and masked all interrupt, level sensitive.
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
void Hal_Vic_Init_impl(void)
{
    // Set all disable
    VIC->CTRL8 = 0xFFFFFFFF; // GPIO
    VIC->CTRL1 = 0xFFFFFFFF; // VIC

    // Set all invert disable
    VIC->CTRL7 = 0; // GPIO
    VIC->CTRL0 = 0; // VIC

    // Set all type level
    VIC->CTRL9 = 0; // GPIO 00~15
    VIC->CTRLA = 0; // GPIO 16~31
    VIC->CTRL2 = 0; // VIC 00~15
    VIC->CTRL3 = 0; // VIC 16~31

    // Clear all interrupt
    VIC->CTRLB = 0xFFFFFFFF; // GPIO
    VIC->CTRL4 = 0xFFFFFFFF; // VIC

    // Set all mask
    VIC->CTRLC = 0xFFFFFFFF; // GPIO
    VIC->CTRL5 = 0xFFFFFFFF; // VIC

    // Disable all mmfactor resume
    VIC->CTRLE = 0; // GPIO
    VIC->CTRLF = 0; // VIC
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
void Hal_Vic_Pre_Init(void)
{
    // VIC moudle
    Hal_Vic_IntEn       = Hal_Vic_IntEn_impl;
    Hal_Vic_IntClear    = Hal_Vic_IntClear_impl;
    Hal_Vic_IntMask     = Hal_Vic_IntMask_impl;
    Hal_Vic_IntTypeSel  = Hal_Vic_IntTypeSel_impl;
    Hal_Vic_IntInv      = Hal_Vic_IntInv_impl;
    Hal_Vic_IntStatRead = Hal_Vic_IntStatRead_impl;
    Hal_Vic_Init        = Hal_Vic_Init_impl;

    // IPC module
    Hal_Vic_IpcIntTrig  = Hal_Vic_IpcIntTrig_impl;
    Hal_Vic_IpcIntEn    = Hal_Vic_IpcIntEn_impl;
    Hal_Vic_IpcIntClear = Hal_Vic_IpcIntClear_impl;

    // GPIO module
    Hal_Vic_GpioIntEn       = Hal_Vic_GpioIntEn_impl;
    Hal_Vic_GpioIntClear    = Hal_Vic_GpioIntClear_impl;
    Hal_Vic_GpioIntMask     = Hal_Vic_GpioIntMask_impl;
    Hal_Vic_GpioIntTypeSel  = Hal_Vic_GpioIntTypeSel_impl;
    Hal_Vic_GpioIntInv      = Hal_Vic_GpioIntInv_impl;
    Hal_Vic_GpioIntStatRead = Hal_Vic_GpioIntStatRead_impl;

    // MmFactor relative
    Hal_Vic_MmFactorResumeByIpcIntEn        = Hal_Vic_MmFactorResumeByIpcIntEn_impl;
    Hal_Vic_MmFactorResumeByIpcIntStatRead  = Hal_Vic_MmFactorResumeByIpcIntStatRead_impl;
    Hal_Vic_MmFactorResumeByGpioIntEn       = Hal_Vic_MmFactorResumeByGpioIntEn_impl;
    Hal_Vic_MmFactorResumeByGpioIntStatRead = Hal_Vic_MmFactorResumeByGpioIntStatRead_impl;
}
