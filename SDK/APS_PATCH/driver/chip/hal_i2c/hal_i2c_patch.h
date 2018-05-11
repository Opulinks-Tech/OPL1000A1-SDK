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
*  hal_i2c_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of I2C .
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

/***********************
Head Block of The File
***********************/
#ifndef __HAL_I2C_PATCH_H__
#define __HAL_I2C_PATCH_H__

// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_i2c.h"

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


/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable


// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions

/* Internal */

/* Master mode relative */
extern uint32_t Hal_I2c_MasterInit_patch(E_I2cAddrMode_t eAddrMode, E_I2cSpeed_t eSpeed);

/* Slave mode relative */

#endif
