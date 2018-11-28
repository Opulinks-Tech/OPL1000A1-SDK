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
*  hal_auxadc_patch.h
*
*  Project:
*  --------
*  OPL1000 Project - the AUXADC definition file
*
*  Description:
*  ------------
*  This include file is the AUXADC definition file
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
#ifndef _HAL_AUXADC_PATCH_H_
#define _HAL_AUXADC_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "hal_auxadc.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define HAL_AUX_AVERAGE_COUNT       30  // the average count of ADC value


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
// the calibration data of AUXADC
typedef struct
{
    float fSlopeVbat;
    float fSlopeIo;
    int16_t wDcOffsetVbat;      // 0V
    int16_t wDcOffsetIo;        // 0V
} T_HalAuxCalData_patch;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern T_HalAuxCalData_patch g_tHalAux_CalData_patch;
extern uint32_t g_ulHalAux_AverageCount;


// Sec 5: declaration of global function prototype
extern void Hal_Aux_Init_patch(void);
extern uint8_t Hal_Aux_AdcValueGet_patch(uint32_t *pulValue);
extern uint8_t Hal_Aux_VbatGet_patch(float *pfVbat);
extern uint8_t Hal_Aux_IoVoltageGet_patch(uint8_t ubGpioIdx, float *pfVoltage);

typedef uint8_t (*T_Hal_Aux_VbatCalibration_Fp)(float fVbat);
typedef uint8_t (*T_Hal_Aux_IoVoltageCalibration_Fp)(uint8_t ubGpioIdx, float fVoltage);

extern T_Hal_Aux_VbatCalibration_Fp Hal_Aux_VbatCalibration;
extern T_Hal_Aux_IoVoltageCalibration_Fp Hal_Aux_IoVoltageCalibration;

extern uint8_t Hal_Aux_VbatCalibration_impl(float fVbat);
extern uint8_t Hal_Aux_IoVoltageCalibration_impl(uint8_t ubGpioIdx, float fVoltage);


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _HAL_AUXADC_PATCH_H_
