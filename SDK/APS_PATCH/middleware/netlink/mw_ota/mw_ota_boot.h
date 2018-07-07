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
*  mw_ota_boot.h
*
*  Project:
*  --------
*  OPL1000 Project - the Over The Air (OTA) definition file
*
*  Description:
*  ------------
*  This include file is the Over The Air (OTA) definition file
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
#ifndef _MW_OTA_BOOT_H_
#define _MW_OTA_BOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include <stdio.h>
#include <stdint.h>
#include "mw_ota_def.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
// the information of OTA image header (64 bytes)
typedef struct
{
    uint16_t uwProjectId;
    uint16_t uwChipId;
    uint16_t uwFirmwareId;
    uint16_t uwCheckSum;
    uint32_t ulImageSize;
    uint8_t ubaReserved[52];
} T_MwOtaImageHeader;

typedef uint8_t (*T_MwOta_Boot_Init_Fp)(void);
typedef uint8_t (*T_MwOta_Boot_CheckUartBehavior_Fp)(void);
typedef uint8_t (*T_MwOta_Boot_LoadPatchImage_Fp)(void);

// internal part
typedef uint8_t (*T_MwOta_Boot_HeaderPaser_Fp)(void);
typedef uint8_t (*T_MwOta_Boot_WritePatchImage_Fp)(void);


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
extern T_MwOta_Boot_Init_Fp MwOta_Boot_Init;
extern T_MwOta_Boot_CheckUartBehavior_Fp MwOta_Boot_CheckUartBehavior;
extern T_MwOta_Boot_LoadPatchImage_Fp MwOta_Boot_LoadPatchImage;

// internal part
extern T_MwOta_Boot_HeaderPaser_Fp MwOta_Boot_HeaderPaser;
extern T_MwOta_Boot_WritePatchImage_Fp MwOta_Boot_WritePatchImage;

void MwOta_Boot_PreInitCold(void);


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_OTA_BOOT_H_
