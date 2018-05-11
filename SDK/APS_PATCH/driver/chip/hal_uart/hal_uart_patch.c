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
*  hal_uart_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This source file defines the functions of uart .
*  Ref. document is << DesignWare DW_apb_uart Databook >>
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
#include <string.h>
#include "nl1000.h"
#include "hal_tick.h"
#include "hal_system.h"
#include "hal_vic.h"
#include "hal_uart.h"
#include "hal_uart_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define UART_0       ((S_Uart_Reg_t *) UART0_BASE)
#define UART_1       ((S_Uart_Reg_t *) UART1_BASE)
#define SYSTEM_CLK   22000000
#define DEFAULT_BAUD 115200

/*
 * baud rate:115200 -> 100 us per char, 
 * Max core 176Mhz -> 1ms = 176*1000 tick = 0x2AF80 => 0x30000
 */
#define UART_TIMEOUT_COUNT_MAX  ( 0x30000 )
#define UART_TIMEOUT_MS_MAX     ( Hal_Tick_MilliSecMax() )

#define UART_BAUD_DIV_MAX    0xFFFF
#define UART_BAUD_DIV_L_MASK 0xFF
#define UART_BAUD_DIV_H_MASK 0xFF

#define UART_IER_PTIME_EN (1<<7)
#define UART_IER_EDSSI_EN (1<<3)
#define UART_IER_ELSI_EN  (1<<2)
#define UART_IER_ETBEI_EN (1<<1)
#define UART_IER_ERBFI_EN (1<<0)

#define UART_FCR_REVC_TRIG_ANY      (0<<6)
#define UART_FCR_REVC_TRIG_QUARTER  (1<<6)
#define UART_FCR_REVC_TRIG_HALF     (2<<6)
#define UART_FCR_REVC_TRIG_TWO_AVA  (3<<6)
#define UART_FCR_REVC_TRIG_MASK     (0x3<<6)
#define UART_FCR_EMPTY_TRIG_NONE    (0<<4)
#define UART_FCR_EMPTY_TRIG_TWO     (1<<4)
#define UART_FCR_EMPTY_TRIG_QUARTER (2<<4)
#define UART_FCR_EMPTY_TRIG_HALF    (3<<4)
#define UART_FCR_EMPTY_TRIG_MASK    (0x3<<4)
#define UART_FCR_XMIT_FIFO_RST      (1<<2)
#define UART_FCR_RECV_FIFO_RST      (1<<1)
#define UART_FCR_XMIT_FIFO_EN       (1<<0)

#define UART_IIR_INT_ID_MASK         0xF

#define UART_LCR_BAUD_DIV_LATCH  (1<<7)
#define UART_LCR_EVEN_PARITY     (1<<4)
#define UART_LCR_PARITY_EN       (1<<3)
#define UART_LCR_STOP_BIT        (1<<2)
#define UART_LCR_DATA_BITS_5     0
#define UART_LCR_DATA_BITS_6     1
#define UART_LCR_DATA_BITS_7     2
#define UART_LCR_DATA_BITS_8     3
#define UART_LCR_DATA_BITS_MASK  0x3

#define UART_LSR_XMIT_HOLD_EMPTY (1<<5)
#define UART_LSR_DATA_READY      (1<<0)

#define UART_MCR_RTS           (1<<1)
#define UART_MCR_AUTO_FLOW_CTL (1<<5)

#define UART_IDX_MAX    2   // the total count of UART module

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list¡K
typedef struct
{
    volatile uint32_t DATA;         //0x00, RBR(R) / THR(W) / DLL
    volatile uint32_t INT_EN;       //0x04, DLH / IER
    volatile uint32_t INT_STATUS;   //0x08, IIR(R) / FCR(W)
    volatile uint32_t LCR;          //0x0C, Line control
    volatile uint32_t MCR;          //0x10, Moden control
    volatile uint32_t LSR;          //0x14, Line status
    volatile uint32_t MSR;          //0x18, Moden status
    volatile uint32_t resv[24];     //0x1C ~ 0x78
    volatile uint32_t USR;          //0x7C UART status
} S_Uart_Reg_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
T_HalUartConfig g_taHalUartCurrentConfig[UART_IDX_MAX] =
{
    {
        115200,
        DATA_BIT_8,
        STOP_BIT_1,
        PARITY_NONE,
        0               // disable the flow control
    },
    
    {
        115200,
        DATA_BIT_8,
        STOP_BIT_1,
        PARITY_NONE,
        0               // disable the flow control
    }
};

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
*  Hal_Uart_Init
*
* DESCRIPTION:
*   1. uart initail function
*
* CALLS
*
* PARAMETERS
*   1. eUartIdx   : The index of UART. refert to E_UartIdx_t
*   2. u32Baud    : Baud-rate
*   3. u8Bits     : Data bits. refert to E_UartDataBit_t
*   4. u8Parity   : Parity mode. refer to E_UartParity_t
*   5. u8StopBit  : Stop bits. refert to E_UartStopBit_t 
*   6. u8EnFlowCtl: Enable/Disable CTS and RTS. 1 for enable/0 for disable 
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Uart_Init_patch(E_UartIdx_t eUartIdx, uint32_t u32Baud, E_UartDataBit_t u8Bits, E_UartParity_t u8Parity, E_UartStopBit_t u8StopBit, uint8_t u8EnFlowCtl)
{
    S_Uart_Reg_t *pUart = 0;
    T_HalUartConfig tConfig;

    // Check UART Index
    if(eUartIdx == UART_IDX_0)
    {
        pUart = UART_0;
    }
    else if(eUartIdx == UART_IDX_1)
    {
        pUart = UART_1;
    }
    else
    {
        return 1;
    }

    // Check StopBit setting
    if(u8StopBit == STOP_BIT_1P5)
    {
        // Only Bits = 5 can set StopBit =1.5
        if(u8Bits != DATA_BIT_5)
            return 1;
    }

    // Reset uart
    if(eUartIdx  == UART_IDX_0)
    {
        // Enable module
        Hal_Sys_ApsClkEn(1, APS_CLK_UART_0);
        // Reset module
        Hal_Sys_ApsModuleRst(ASP_RST_UART_0);
    }else if(eUartIdx == UART_IDX_1){
        // Enable module
        Hal_Sys_ApsClkEn(1, APS_CLK_UART_1);
        // Reset module
        Hal_Sys_ApsModuleRst(ASP_RST_UART_1);
    }

    tConfig.ulBuadrate = u32Baud;
    tConfig.ubDataBit = u8Bits;
    tConfig.ubStopBit = u8StopBit;
    tConfig.ubParity = u8Parity;
    tConfig.ubFlowCtrl = u8EnFlowCtl;
    if (0 != Hal_Uart_ConfigSet(eUartIdx, &tConfig))
        return 1;
	
    // Write FCR
    pUart->INT_STATUS = UART_FCR_REVC_TRIG_ANY | //UART_FCR_REVC_TRIG_HALF |
                        UART_FCR_EMPTY_TRIG_QUARTER |
                        UART_FCR_XMIT_FIFO_EN;

    // Write IER, disable all interrupts
    pUart->INT_EN = 0;
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Uart_BaudRateSet
*
* DESCRIPTION:
*   1. Set baur-rate
*
* CALLS
*
* PARAMETERS
*   1. eUartIdx: The index of UART. refert to E_UartIdx_t
*   2. u32Baud : Baud-rate
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
extern uint32_t Hal_Uart_BaudRateSet_impl(E_UartIdx_t eUartIdx, uint32_t u32Baud);
uint32_t Hal_Uart_BaudRateSet_patch(E_UartIdx_t eUartIdx, uint32_t u32Baud)
{
    if (0 != Hal_Uart_BaudRateSet_impl(eUartIdx, u32Baud))
        return 1;

	g_taHalUartCurrentConfig[eUartIdx].ulBuadrate = u32Baud;
	
    return 0;
}


/*************************************************************************
* FUNCTION:
*  Hal_Uart_ConfigGet
*
* DESCRIPTION:
*   1. get the current config of UART
*
* CALLS
*
* PARAMETERS
*   1. eUartIdx    : The index of UART. refert to E_UartIdx_t
*   2. ptConfig    : the config of UART
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Uart_ConfigGet(E_UartIdx_t eUartIdx, T_HalUartConfig *ptConfig)
{
    // Check UART Index
    if (eUartIdx >= UART_IDX_MAX)
        return 1;
    
    // get the current config of UART
    memcpy(ptConfig, &(g_taHalUartCurrentConfig[eUartIdx]), sizeof(T_HalUartConfig));
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Uart_ConfigSet
*
* DESCRIPTION:
*   1. set the current config of UART
*
* CALLS
*
* PARAMETERS
*   1. eUartIdx   : The index of UART. refert to E_UartIdx_t
*   2. ptConfig    : the config of UART
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Uart_ConfigSet(E_UartIdx_t eUartIdx, T_HalUartConfig *ptConfig)
{
    S_Uart_Reg_t *pUart = 0;
    uint32_t u32Temp = 0;

    // Check UART Index
    if(eUartIdx == UART_IDX_0)
    {
        pUart = UART_0;
    }
    else if(eUartIdx == UART_IDX_1)
    {
        pUart = UART_1;
    }
    else
    {
        return 1;
    }

    // Check StopBit setting
    if(ptConfig->ubStopBit == STOP_BIT_1P5)
    {
        // Only Bits = 5 can set StopBit =1.5
        if(ptConfig->ubDataBit != DATA_BIT_5)
            return 1;
    }
	
    // sequence ref to Page.134
    // Write MCR
    if(ptConfig->ubFlowCtrl)
        pUart->MCR = UART_MCR_AUTO_FLOW_CTL | UART_MCR_RTS;
    else
        pUart->MCR = 0;
	
    // Write divider ( latch is contained ) 
    Hal_Uart_BaudRateSet(eUartIdx, ptConfig->ulBuadrate);
	
    // Write LCR
    if(ptConfig->ubParity == PARITY_EVEN)
    {
        u32Temp |= UART_LCR_PARITY_EN | 
                   UART_LCR_EVEN_PARITY;
    }else if(ptConfig->ubParity == PARITY_ODD){
        u32Temp |= UART_LCR_PARITY_EN;
    }
    if(ptConfig->ubStopBit != STOP_BIT_1)
        u32Temp |= UART_LCR_STOP_BIT;
    u32Temp |= ptConfig->ubDataBit;
    pUart->LCR = u32Temp;
		
    // save the current config
    memcpy(&(g_taHalUartCurrentConfig[eUartIdx]), ptConfig, sizeof(T_HalUartConfig));
    return 0;
}
