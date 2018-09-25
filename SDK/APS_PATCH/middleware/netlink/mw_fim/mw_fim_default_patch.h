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

/***********************
Head Block of The File
***********************/
#ifndef _MW_FIM_DEFAULT_PATCH_H_
#define _MW_FIM_DEFAULT_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim_default.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the max of zone and group

// the version of group
#define MW_FIM_VER00_PATCH      0x01    // reserve for swap
#define MW_FIM_VER01_PATCH      0x09    // system & driver
#define MW_FIM_VER02_PATCH      0x06    // for WIFI & TCPIP
#define MW_FIM_VER03_PATCH      0x04    // calibration data
#define MW_FIM_VER04_PATCH      0x04    // for LE Controller
#define MW_FIM_VER05_PATCH      0x01
#define MW_FIM_VER06_PATCH      0x01
#define MW_FIM_VER07_PATCH      0x02	// For BLE
#define MW_FIM_VER08_PATCH      0x01

#define MW_FIM_VER10_PATCH      0x01    // reserve for swap
#define MW_FIM_VER11_PATCH      0x01    // example
#define MW_FIM_VER12_PATCH      0x01
#define MW_FIM_VER13_PATCH      0x01
#define MW_FIM_VER14_PATCH      0x01
#define MW_FIM_VER15_PATCH      0x01
#define MW_FIM_VER16_PATCH      0x01
#define MW_FIM_VER17_PATCH      0x01
#define MW_FIM_VER18_PATCH      0x01

#define MW_FIM_VER20_PATCH      0x01    // reserve for swap
#define MW_FIM_VER21_PATCH      0x01
#define MW_FIM_VER22_PATCH      0x01
#define MW_FIM_VER23_PATCH      0x01
#define MW_FIM_VER24_PATCH      0x01
#define MW_FIM_VER25_PATCH      0x01
#define MW_FIM_VER26_PATCH      0x01
#define MW_FIM_VER27_PATCH      0x01
#define MW_FIM_VER28_PATCH      0x01

#define MW_FIM_VER30_PATCH      0x01    // reserve for swap
#define MW_FIM_VER31_PATCH      0x01
#define MW_FIM_VER32_PATCH      0x01
#define MW_FIM_VER33_PATCH      0x01
#define MW_FIM_VER34_PATCH      0x01
#define MW_FIM_VER35_PATCH      0x01
#define MW_FIM_VER36_PATCH      0x01
#define MW_FIM_VER37_PATCH      0x01
#define MW_FIM_VER38_PATCH      0x01

// the information of zone
#define MW_FIM_ZONE0_BASE_ADDR_PATCH    0x00077000
#define MW_FIM_ZONE0_BLOCK_SIZE_PATCH   0x1000
#define MW_FIM_ZONE0_BLOCK_NUM_PATCH    9               // swap + used

#define MW_FIM_ZONE1_BASE_ADDR_PATCH    0x00080000
#define MW_FIM_ZONE1_BLOCK_SIZE_PATCH   0x1000
#define MW_FIM_ZONE1_BLOCK_NUM_PATCH    0               // swap + used

#define MW_FIM_ZONE2_BASE_ADDR_PATCH    0x00081000
#define MW_FIM_ZONE2_BLOCK_SIZE_PATCH   0x1000
#define MW_FIM_ZONE2_BLOCK_NUM_PATCH    0               // swap + used

#define MW_FIM_ZONE3_BASE_ADDR_PATCH    0x00082000
#define MW_FIM_ZONE3_BLOCK_SIZE_PATCH   0x1000
#define MW_FIM_ZONE3_BLOCK_NUM_PATCH    0               // swap + used


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
extern void mw_fim_default_patch_init(void);


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_FIM_DEFAULT_PATCH_H_
