/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
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
*  mw_fim_default_group01.c
*
*  Project:
*  --------
*  OPL1000 Project - the Flash Item Management (FIM) implement file
*
*  Description:
*  ------------
*  This implement file is include the Flash Item Management (FIM) function and api.
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim_default_group01.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable

// the default value of Uart config
const T_HalUartConfig g_tMwFimDefaultUartConfig =
{
    115200,
    DATA_BIT_8,
    STOP_BIT_1,
    PARITY_NONE,
    0               // disable the flow control
};

// the address buffer of Uart config
RET_DATA uint32_t g_ulaMwFimAddrBufferUartConfig[MW_FIM_UART_CFG_NUM];

// the information table of group 01
const T_MwFimFileInfo g_taMwFimGroupTable01[] =
{
    {MW_FIM_IDX_GP01_UART_CFG, MW_FIM_UART_CFG_NUM, MW_FIM_UART_CFG_SIZE, (uint8_t*)&g_tMwFimDefaultUartConfig, g_ulaMwFimAddrBufferUartConfig},
    // the end, don't modify and remove it
    {0xFFFFFFFF,            0x00,              0x00,               NULL,                            NULL}
};


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions
