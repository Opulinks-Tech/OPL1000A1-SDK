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
*  hal_patch.h
*
*  Project:
*  --------
*  OPL1000 Project - the patch definition file
*
*  Description:
*  ------------
*  This include file is the patch definition file
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
#ifndef _HAL_PATCH_H_
#define _HAL_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include <stdio.h>
#include <stdint.h>


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// Patch mark
#define HAL_PATCH_MARK          0x50544348      // PTCH

// Function type
#define HAL_PATCH_FUNC_HW       0x0001
#define HAL_PATCH_FUNC_SW       0x0002
#define HAL_PATCH_FUNC_CODE     0x0004
#define HAL_PATCH_FUNC_REMAP    0x0008
#define HAL_PATCH_FUNC_MASK     0x000F

// Apply type
#define HAL_PATCH_APPLY_COLD    0x0010
#define HAL_PATCH_APPLY_WARM    0x0020
#define HAL_PATCH_APPLY_MASK    0x00F0

// CPU type
#define HAL_PATCH_CPU_M3        0x0100
#define HAL_PATCH_CPU_M0        0x0200
#define HAL_PATCH_CPU_MASK      0x0F00

// Data Length
#define HAL_PATCH_LENGTH_1      0x00010000
#define HAL_PATCH_LENGTH_2      0x00020000
#define HAL_PATCH_LENGTH_4      0x00040000
#define HAL_PATCH_LENGTH_MASK   0x000F0000

// Size
#define HAL_PATCH_SIZE_HEADER                       (sizeof(S_Hal_Patch_Header_t))
#define HAL_PATCH_SIZE_HEADER_WITHOUT_CHECKSUM      (HAL_PATCH_SIZE_HEADER - 4)
#define HAL_PATCH_SIZE_HW                           8
#define HAL_PATCH_SIZE_SW                           8
#define HAL_PATCH_SIZE_CODE_ADDR                    4
#define HAL_PATCH_SIZE_CODE_DATA                    4
#define HAL_PATCH_SIZE_REMAP                        12
#define HAL_PATCH_SIZE_CHECK_SUM                    4
#define HAL_PATCH_SIZE_SKIP                         1

// the max size of input data
#define HAL_PATCH_IN_SIZE_MAX           256     // it is a limitation about stack size

// the state
#define HAL_PATCH_STATE_HEADER          0
#define HAL_PATCH_STATE_HW_PATCH        1
#define HAL_PATCH_STATE_SW_PATCH        2
#define HAL_PATCH_STATE_CODE_ADDR       3
#define HAL_PATCH_STATE_CODE_DATA       4
#define HAL_PATCH_STATE_REMAP           5
#define HAL_PATCH_STATE_DATA_CHECK_SUM  6
#define HAL_PATCH_STATE_SKIP            7
#define HAL_PATCH_STATE_MAX             8


// Return
#define HAL_PATCH_RET_FAIL          0
#define HAL_PATCH_RET_NEXT          1
#define HAL_PATCH_RET_FINISH        2
#define HAL_PATCH_RET_LOOP          3       // for internal usage


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
// Case 1: Header(16)
//              MagicNum(4)     = patch mark
//              PatchType(4)    = HW / SW
//              Size(4)         = size (8*n)
//              CheckSum(4)     = check sum
//         Data(8*n+4)
//              Addr(4)         = address       // repeat "Addr and Data"
//              Data(4)         = value
//              CheckSum(4)     = check sum
// Case 2: Header(16)
//              MagicNum(4)     = patch mark
//              PatchType(4)    = Code
//              Size(4)         = size (4+4*n)
//              CheckSum(4)     = check sum
//         Data(4+4*n+4)
//              Addr(4)         = address
//              Data(4)         = value         // repeat "Data"
//              CheckSum(4)     = check sum
// Case 3: Header(16)
//              MagicNum(4)     = patch mark
//              PatchType(4)    = Remap
//              Size(4)         = size (12*n)
//              CheckSum(4)     = check sum
//         Data(12*n+4)
//              ROM Addr(4)     = ROM address   // repeat "ROM Addr, RAM Addr and Mask"
//              RAM Addr(4)     = RAM address
//              Mask(4)         = mask
//              CheckSum(4)     = check sum
typedef struct
{
    uint32_t ulMagicNum;        // patch mark
    uint32_t ulPatchType;
    uint32_t ulSize;
    uint32_t ulCheckSum;
} S_Hal_Patch_Header_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
void Hal_Patch_PreInitCold(void);

void Hal_Patch_Init(void);
uint8_t Hal_Patch_DataIn(uint32_t *pulAddr, uint32_t ulSize);
void Hal_Patch_ApplyHwPatchM3(void);
void Hal_Patch_ApplyHwPatchM0(void);

// for Sw patch test, need to remove
typedef void (*T_Test_ForSwPatchFp)(void);
extern T_Test_ForSwPatchFp Test_ForSwPatch;


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _HAL_PATCH_H_
