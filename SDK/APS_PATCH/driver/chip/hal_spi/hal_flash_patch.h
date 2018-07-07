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
*  hal_flash_patch.h
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of flash .
*
*  Author:
*  -------
*  Jeff Kuo
******************************************************************************/

#ifndef __HAL_FLASH_PATCH_H__
#define __HAL_FLASH_PATCH_H__

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_spi.h"

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
extern uint32_t Hal_Flash_Init_patch(E_SpiIdx_t u32SpiIdx);
extern uint32_t Hal_Flash_4KSectorAddrErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr);
extern uint32_t Hal_Flash_4KSectorIdxErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecIdx);
extern uint32_t Hal_Flash_PageAddrProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
extern uint32_t Hal_Flash_PageIdxProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageIdx, uint8_t u8UseQuadMode, uint8_t *pu8Data);
extern uint32_t Hal_Flash_PageAddrRead_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
extern void Hal_Flash_Reset_patch(E_SpiIdx_t u32SpiIdx);

extern uint32_t Hal_Flash_AddrProgram(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);
extern uint32_t Hal_Flash_AddrRead(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);

extern uint32_t Hal_Flash_4KSectorAddrErase_Internal(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr);
extern uint32_t Hal_Flash_AddrProgram_Internal(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);
extern uint32_t Hal_Flash_AddrRead_Internal(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);
/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable

// Sec 7: declaration of static function prototype

/***********
C Functions
***********/
// Sec 8: C Functions

#endif

