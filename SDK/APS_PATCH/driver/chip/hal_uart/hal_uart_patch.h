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
*  hal_uart_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of uart .
*
*  Author:
*  -------
*  Chung-chun Wang
******************************************************************************/

#ifndef __HAL_UART_PATCH_H__
#define __HAL_UART_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include <stdint.h>
#include "hal_uart.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
// the information of Uart config
typedef struct
{
    uint32_t ulBuadrate;
    uint8_t ubDataBit;
    uint8_t ubStopBit;
    uint8_t ubParity;
    uint8_t ubFlowCtrl;
} T_HalUartConfig;


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
uint32_t Hal_Uart_Init_patch(E_UartIdx_t eUartIdx, uint32_t u32Baud, E_UartDataBit_t u8Bits, E_UartParity_t u8Parity, E_UartStopBit_t u8StopBit, uint8_t u8EnFlowCtl);
uint32_t Hal_Uart_BaudRateSet_patch(E_UartIdx_t eUartIdx, uint32_t u32Baud);
uint32_t Hal_Uart_ConfigGet(E_UartIdx_t eUartIdx, T_HalUartConfig *ptConfig);
uint32_t Hal_Uart_ConfigSet(E_UartIdx_t eUartIdx, T_HalUartConfig *ptConfig);

#endif

