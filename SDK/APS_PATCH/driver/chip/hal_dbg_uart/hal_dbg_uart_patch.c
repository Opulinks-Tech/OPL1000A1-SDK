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
*  hal_dbg_uart_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This source file defines the functions of debug UART.
*
*  Author:
*  -------
*  Jeff Kuo
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include "nl1000.h"
#include "hal_dbg_uart.h"
#include "hal_dbg_uart_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define DBG_UART     ((S_DbgUart_Reg_t *) UART_DBG_BASE)
#define DEFAULT_BAUD 115200

// 0x10
#define DBG_UART_BAUD_DIV_MIN 0x00010
#define DBG_UART_BAUD_DIV_MAX 0xFFFFF

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef struct
{
    volatile uint32_t DATA;         //0x00
    volatile uint32_t STATE;        //0x04
    volatile uint32_t CTRL;         //0x08
    volatile uint32_t INT_STATUS;   //0x0C
    volatile uint32_t BAUD_DIV;     //0x10
} S_DbgUart_Reg_t;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
uint32_t g_ulHalDbgUart_CurrentBaudRate = DEFAULT_BAUD;
uint8_t g_ubHalDbgUart_RxIntEnStatus = 0;

// Sec 5: declaration of global function prototype
uint32_t Hal_DbgUart_BaudRateSet_patch(uint32_t u32Baud);
uint32_t Hal_DbgUart_BaudRateGet(void);
uint32_t Hal_DbgUart_RxIntEn_patch(uint8_t u8Enable);
uint8_t Hal_DbgUart_RxIntEnStatusGet(void);

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
*  Hal_DbgUart_BaudRateSet
*
* DESCRIPTION:
*   1. Set baur-rate. (default:115200)
*
* CALLS
*
* PARAMETERS
*   1. u32Baud: Baud-rate
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_DbgUart_BaudRateSet_patch(uint32_t u32Baud)
{
    uint32_t result = 0;

    result = (SystemCoreClockGet() + u32Baud/2)/u32Baud;
    if( (result<DBG_UART_BAUD_DIV_MIN) ||(result>DBG_UART_BAUD_DIV_MAX) )
    {
        DBG_UART->BAUD_DIV = (SystemCoreClockGet() + DEFAULT_BAUD/2) / DEFAULT_BAUD;
        g_ulHalDbgUart_CurrentBaudRate = DEFAULT_BAUD;
        return 1;
    }else{
        DBG_UART->BAUD_DIV = result;
        g_ulHalDbgUart_CurrentBaudRate = u32Baud;
        return 0;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_DbgUart_BaudRateGet
*
* DESCRIPTION:
*   1. Get baur-rate.
*
* CALLS
*
* PARAMETERS
*
* RETURNS
*   Baud-rate
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_DbgUart_BaudRateGet(void)
{
    return g_ulHalDbgUart_CurrentBaudRate;
}

/*************************************************************************
* FUNCTION:
*  Hal_DbgUart_RxIntEn
*
* DESCRIPTION:
*   1. Enable rx interrupt
*
* CALLS
*
* PARAMETERS
*   1. u8Enable  : 1 for enable/0 for disable the interrupt
*
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
* 
*************************************************************************/
extern uint32_t Hal_DbgUart_RxIntEn_impl(uint8_t u8Enable);
uint32_t Hal_DbgUart_RxIntEn_patch(uint8_t u8Enable)
{
    if (0 != Hal_DbgUart_RxIntEn_impl(u8Enable))
        return 1;

    g_ubHalDbgUart_RxIntEnStatus = u8Enable;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_DbgUart_RxIntEnStatusGet
*
* DESCRIPTION:
*   1. get the status of enable rx interrupt
*
* CALLS
*
* PARAMETERS
*
* RETURNS
*   the status of enable rx interrupt
* GLOBALS AFFECTED
* 
*************************************************************************/
uint8_t Hal_DbgUart_RxIntEnStatusGet(void)
{
    return g_ubHalDbgUart_RxIntEnStatus;
}
