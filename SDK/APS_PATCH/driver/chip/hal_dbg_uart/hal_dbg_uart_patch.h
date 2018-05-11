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
*  hal_dbg_uart_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of debug UART.
*
*  Author:
*  -------
*  Jeff Kuo
******************************************************************************/

#ifndef __HAL_DBG_UART_PATCH_H__
#define __HAL_DBG_UART_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_dbg_uart.h"

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
extern uint32_t Hal_DbgUart_BaudRateSet_patch(uint32_t u32Baud);
extern uint32_t Hal_DbgUart_BaudRateGet(void);
extern uint32_t Hal_DbgUart_RxIntEn_patch(uint8_t u8Enable);
extern uint8_t Hal_DbgUart_RxIntEnStatusGet(void);

/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable

// Sec 7: declaration of static function prototype

/***********
C Functions
***********/
// Sec 8: C Functions

#endif

