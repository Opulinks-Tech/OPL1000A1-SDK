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
*  hal_tmr_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series patch
*
*  Description:
*  ------------
*  This include file defines the patch proto-types of Timer.
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

#ifndef __HAL_TMR_PATCH_H__
#define __HAL_TMR_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include <stdint.h>
#include "hal_tmr.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef void (*T_Tmr_CallBack)(uint32_t u32TmrIdx);

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
extern T_Tmr_CallBack Tmr_CallBack[2];

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
// Patch
void Hal_Tmr_Init_patch(uint32_t u32Timer);
void Hal_Tmr_Start_patch(uint32_t u32Timer, uint32_t u32Us);
uint32_t Hal_Tmr_ReadCurrValue_patch(uint32_t u32Timer);

// Extend functions
void Hal_Tmr_Reload(uint32_t u32Timer, uint32_t u32Us);
void Hal_Tmr_IntClear(uint32_t u32Timer);
void Hal_Tmr_CallBackFuncSet(uint32_t u32Timer, T_Tmr_CallBack tFunc);

#endif
