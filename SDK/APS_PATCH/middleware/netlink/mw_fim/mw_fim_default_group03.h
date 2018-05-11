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
*  mw_fim_default_group03.h
*
*  Project:
*  --------
*  NL1000 Project - the Flash Item Management (FIM) definition file
*
*  Description:
*  ------------
*  This include file is the Flash Item Management (FIM) definition file
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
#ifndef _MW_FIM_DEFAULT_GROUP03_H_
#define _MW_FIM_DEFAULT_GROUP03_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim.h"
#include "hal_auxadc.h"
#include "hal_temperature.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the file ID
// xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx
// ^^^^ ^^^^ Zone (0~3)
//           ^^^^ ^^^^ Group (0~8), 0 is reserved for swap
//                     ^^^^ ^^^^ ^^^^ ^^^^ File ID, start from 0
typedef enum
{
    MW_FIM_IDX_GP03_START = 0x00030000,             // the start IDX of group 03
    MW_FIM_IDX_GP03_CAL_AUXADC,
    MW_FIM_IDX_GP03_CAL_TEMPERATURE,
    
    MW_FIM_IDX_GP03_MAX
} E_MwFimIdxGroup03;


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
// the calibration data of AUXADC
#define MW_FIM_CAL_AUXADC_SIZE      sizeof(T_HalAuxCalData)
#define MW_FIM_CAL_AUXADC_NUM       1

// the calibration data of Temperature Sensor
#define MW_FIM_CAL_TEMPERATURE_SIZE     sizeof(T_HalTmprCalData)
#define MW_FIM_CAL_TEMPERATURE_NUM      1


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern const T_MwFimFileInfo g_taMwFimGroupTable03[];

extern const T_HalAuxCalData g_tMwFimDefaultCalAuxadc;
extern const T_HalTmprCalData g_tMwFimDefaultCalTmpr;

// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_FIM_DEFAULT_GROUP03_H_
