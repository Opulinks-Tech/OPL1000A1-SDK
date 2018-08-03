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
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "mw_fim_default.h"
#include "mw_fim_default_group01.h"
#include "mw_fim_default_group02.h"
#include "mw_fim_default_group03.h"
#include "mw_fim_default_group04.h"
#include "mw_fim_default_group05.h"
#include "mw_fim_default_group06.h"
#include "mw_fim_default_group07.h"
#include "mw_fim_default_group08.h"

#include "mw_fim_default_patch.h"
#include "mw_fim_default_group01_patch.h"
#include "mw_fim_default_group02_patch.h"
#include "mw_fim_default_group03_patch.h"
#include "mw_fim_default_group04_patch.h"
#include "mw_fim_default_group05_patch.h"
#include "mw_fim_default_group06_patch.h"
#include "mw_fim_default_group07_patch.h"
#include "mw_fim_default_group08_patch.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
// the version table
uint8_t g_ubaMwFimVersionTable_patch[MW_FIM_ZONE_MAX][MW_FIM_GROUP_MAX] =
{
    {
        MW_FIM_VER00_PATCH,
        MW_FIM_VER01_PATCH,
        MW_FIM_VER02_PATCH,
        MW_FIM_VER03_PATCH,
        MW_FIM_VER04_PATCH,
        MW_FIM_VER05_PATCH,
        MW_FIM_VER06_PATCH,
        MW_FIM_VER07_PATCH,
        MW_FIM_VER08_PATCH
    },
    {
        MW_FIM_VER10_PATCH,
        MW_FIM_VER11_PATCH,
        MW_FIM_VER12_PATCH,
        MW_FIM_VER13_PATCH,
        MW_FIM_VER14_PATCH,
        MW_FIM_VER15_PATCH,
        MW_FIM_VER16_PATCH,
        MW_FIM_VER17_PATCH,
        MW_FIM_VER18_PATCH
    },
    {
        MW_FIM_VER20_PATCH,
        MW_FIM_VER21_PATCH,
        MW_FIM_VER22_PATCH,
        MW_FIM_VER23_PATCH,
        MW_FIM_VER24_PATCH,
        MW_FIM_VER25_PATCH,
        MW_FIM_VER26_PATCH,
        MW_FIM_VER27_PATCH,
        MW_FIM_VER28_PATCH
    },
    {
        MW_FIM_VER30_PATCH,
        MW_FIM_VER31_PATCH,
        MW_FIM_VER32_PATCH,
        MW_FIM_VER33_PATCH,
        MW_FIM_VER34_PATCH,
        MW_FIM_VER35_PATCH,
        MW_FIM_VER36_PATCH,
        MW_FIM_VER37_PATCH,
        MW_FIM_VER38_PATCH
    }
};

// the information table of all zone
T_MwFimZoneInfo g_taMwFimZoneInfoTable_patch[MW_FIM_ZONE_MAX] =
{
    {
        MW_FIM_ZONE0_BASE_ADDR_PATCH,
        MW_FIM_ZONE0_BLOCK_SIZE_PATCH,
        MW_FIM_ZONE0_BLOCK_NUM_PATCH,
        g_ubaMwFimVersionTable_patch[0]
    },
    {
        MW_FIM_ZONE1_BASE_ADDR_PATCH,
        MW_FIM_ZONE1_BLOCK_SIZE_PATCH,
        MW_FIM_ZONE1_BLOCK_NUM_PATCH,
        g_ubaMwFimVersionTable_patch[1]
    },
    {
        MW_FIM_ZONE2_BASE_ADDR_PATCH,
        MW_FIM_ZONE2_BLOCK_SIZE_PATCH,
        MW_FIM_ZONE2_BLOCK_NUM_PATCH,
        g_ubaMwFimVersionTable_patch[2]
    },
    {
        MW_FIM_ZONE3_BASE_ADDR_PATCH,
        MW_FIM_ZONE3_BLOCK_SIZE_PATCH,
        MW_FIM_ZONE3_BLOCK_NUM_PATCH,
        g_ubaMwFimVersionTable_patch[3]
    }
};

// the information table of all group
extern const T_MwFimFileInfo g_taMwFimGroupTableNull[];
T_MwFimFileInfo* g_ptaMwFimGroupInfoTable_patch[MW_FIM_ZONE_MAX][MW_FIM_GROUP_MAX] =
{
    // zone 0
    {
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,      // reserve for swap
        (T_MwFimFileInfo*)g_taMwFimGroupTable01_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable02_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable03_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable04_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable05_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable06_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable07_patch,
        (T_MwFimFileInfo*)g_taMwFimGroupTable08_patch
    },
    // zone 1
    {
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,      // reserve for swap
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull
    },
    // zone 2
    {
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,      // reserve for swap
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull
    },
    // zone 3
    {
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,      // reserve for swap
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull,
        (T_MwFimFileInfo*)g_taMwFimGroupTableNull
    }
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

void mw_fim_default_patch_init(void)
{
    // the version table
    memcpy(g_ubaMwFimVersionTable, g_ubaMwFimVersionTable_patch, sizeof(uint8_t) * MW_FIM_ZONE_MAX * MW_FIM_GROUP_MAX);

    // the information table of all zone
    memcpy(g_taMwFimZoneInfoTable, g_taMwFimZoneInfoTable_patch, sizeof(T_MwFimZoneInfo) * MW_FIM_ZONE_MAX);
    
    // the information table of all group
    memcpy(g_ptaMwFimGroupInfoTable, g_ptaMwFimGroupInfoTable_patch, sizeof(T_MwFimFileInfo*) * MW_FIM_ZONE_MAX * MW_FIM_GROUP_MAX);
}
