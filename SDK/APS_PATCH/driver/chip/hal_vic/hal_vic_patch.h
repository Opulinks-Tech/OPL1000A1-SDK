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
*  hal_vic_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of vic patch functions.
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

#ifndef __HAL_VIC_PATCH_H__
#define __HAL_VIC_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_vic.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef enum
{
    GPIO_OUTPUT     = 0,
    GPIO_INPUT      = 1
} E_GpioDirection_t;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable

// Sec 5: declaration of global function prototype

/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable
typedef void (*T_Gpio_CallBack)(E_GpioIdx_t eIdx);

// Sec 7: declaration of static function prototype

/***********
C Functions
***********/
// Sec 8: C Functions
extern T_Gpio_CallBack GpioCallBack[GPIO_IDX_MAX];
void Hal_Vic_GpioInit(void);
uint8_t Hal_Vic_GpioInput(E_GpioIdx_t eIdx);                    // 0: low       1: high
void Hal_Vic_GpioOutput(E_GpioIdx_t eIdx, uint8_t ubLevel);     // 0: low       1: high
void Hal_Vic_GpioDirection(E_GpioIdx_t eIdx, E_GpioDirection_t tDirection);
void Hal_Vic_GpioCallBackFuncSet(E_GpioIdx_t eIdx, T_Gpio_CallBack tFunc);

#endif
