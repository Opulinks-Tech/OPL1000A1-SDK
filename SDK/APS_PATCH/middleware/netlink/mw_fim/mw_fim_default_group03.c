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
*  mw_fim_default_group03.c
*
*  Project:
*  --------
*  NL1000 Project - the Flash Item Management (FIM) implement file
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
#include "mw_fim_default_group03.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
// the calibration data of AUXADC
const T_HalAuxCalData g_tMwFimDefaultCalAuxadc =
{
    0.003333,   // float fSlopeVbat;
    0.003333,   // float fSlopeIo;
    105,        // uint16_t uwDcOffsetVbat;            // 0V
    92          // uint16_t uwDcOffsetIo;              // 0V
};
// the address buffer of AUXADC
uint32_t g_ulaMwFimAddrBufferCalAuxadc[MW_FIM_CAL_AUXADC_NUM];

// the calibration data of Temperature Sensor
const T_HalTmprCalData g_tMwFimDefaultCalTmpr =
{
    25.0,       // float fBaseTemperature;                  // 25
    {           // float faThermistor[HAL_TMPR_STEP_MAX];   // 25 ~ 48
        49.5908, 47.7780, 45.9120, 43.9080,
        42.1360, 40.4133, 38.6646, 37.0968,
        35.6130, 34.1773, 32.6134, 31.4780,
        30.1557, 29.0165, 27.8149, 26.7361,
        25.6579, 24.7377, 23.5434, 22.8032,
        21.9060, 21.0696, 20.2292, 19.4776
    },
    30.0        // float fVolDivResistor;                   // Voltage divider resistor
};
// the address buffer of Temperature Sensor
uint32_t g_ulaMwFimAddrBufferCalTmpr[MW_FIM_CAL_TEMPERATURE_NUM];

// the information table of group 03
const T_MwFimFileInfo g_taMwFimGroupTable03[] =
{
    {MW_FIM_IDX_GP03_CAL_AUXADC,      MW_FIM_CAL_AUXADC_NUM,      MW_FIM_CAL_AUXADC_SIZE,      (uint8_t*)&g_tMwFimDefaultCalAuxadc, g_ulaMwFimAddrBufferCalAuxadc},
    {MW_FIM_IDX_GP03_CAL_TEMPERATURE, MW_FIM_CAL_TEMPERATURE_NUM, MW_FIM_CAL_TEMPERATURE_SIZE, (uint8_t*)&g_tMwFimDefaultCalTmpr,   g_ulaMwFimAddrBufferCalTmpr},

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
