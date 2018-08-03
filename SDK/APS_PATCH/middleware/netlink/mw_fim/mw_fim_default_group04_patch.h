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
#ifndef _MW_FIM_DEFAULT_GROUP04_PATCH_H_
#define _MW_FIM_DEFAULT_GROUP04_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim.h"
#include "mw_fim_default_group04.h"
#include "le_ctrl_patch.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
// the file ID
// xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx_xxxx
// ^^^^ ^^^^ Zone (0~3)
//           ^^^^ ^^^^ Group (0~8), 0 is reserved for swap
//                     ^^^^ ^^^^ ^^^^ ^^^^ File ID, start from 0
typedef enum
{
    MW_FIM_IDX_GP04_PATCH_START = 0x00040000,             // the start IDX of group 04
    MW_FIM_IDX_GP04_PATCH_LE_CFG,
    
    MW_FIM_IDX_GP04_PATCH_MAX
} E_MwFimIdxGroup04_Patch;

#define MW_FIM_IDX_LE_CFG_SIZE_PATCH   sizeof(le_cfg_patch_t)

#define FIM_HCI_Version          0x08
#define FIM_HCI_Revision         0x0001
#define FIM_LMP_PAL_Version      0x08
#define FIM_Manufacturer_Name    0x0000  // TODO: not manufacturer name currently
#define FIM_LMP_PAL_Subversion   0x0001

/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern const T_MwFimFileInfo g_taMwFimGroupTable04_patch[];


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _MW_FIM_DEFAULT_GROUP04_PATCH_H_
