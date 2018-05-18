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
*  main_patch.c
*
*  Project:
*  --------
*  NL1000 Project - the main patch implement file
*
*  Description:
*  ------------
*  This implement file is include the main patch function and api.
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
/******************************************************************************
*  Test code brief
*  These examples show how to configure spi settings and use spi driver APIs.
*
*  spi_flash_test() is an example that read and write data on SPI2 to get the
*  Manufacturer ID of spi flash.
*  - port: SPI2
*  - interrupt: off
*  - pin assignment: cs(io5), clk(io4), mosi(io3), miso(io2)
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_spi.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define DUMMY          0x00


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
typedef void (*T_Main_AppInit_fp)(void);
extern T_Main_AppInit_fp Main_AppInit;


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype
static void __Patch_EntryPoint(void) __attribute__((section(".ARM.__at_0x00420000")));
static void __Patch_EntryPoint(void) __attribute__((used));
void Main_AppInit_patch(void);
static void spi_flash_test(void);
static uint32_t spi_Flash_ManufDeviceId(E_SpiIdx_t u32SpiIdx, uint32_t *pu32Manufacturer, uint32_t *pu32MemoryType, uint32_t *pu32MemoryDensity);


/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   __Patch_EntryPoint
*
* DESCRIPTION:
*   the entry point of SW patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void __Patch_EntryPoint(void)
{
    // don't remove this code
    SysInit_EntryPoint();
    
    // application init
    Main_AppInit = Main_AppInit_patch;
}

/*************************************************************************
* FUNCTION:
*   App_Pin_InitConfig
*
* DESCRIPTION:
*   init the pin assignment
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void App_Pin_InitConfig(void)
{
    Hal_Pinmux_Spi_Init(&OPL1000_periph.spi[0]);
}

/*************************************************************************
* FUNCTION:
*   Main_AppInit_patch
*
* DESCRIPTION:
*   the initial of application
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Main_AppInit_patch(void)
{
    // init the pin assignment
    App_Pin_InitConfig();
    
    // do the spi_flash_test
    spi_flash_test();
}

/*************************************************************************
* FUNCTION:
*   spi_flash_test
*
* DESCRIPTION:
*   an example that read and write data on SPI2 to get the Manufacturer
*   ID of spi flash.
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void spi_flash_test(void)
{
    uint32_t u32Manufacturer;
    uint32_t u32MemoryType;
    uint32_t u32MemoryDensity;
    
    // init spi2
    Hal_Spi_Init(SPI_IDX_2, SystemCoreClockGet()/2, SPI_CLK_PLOAR_HIGH_ACT,
                 SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);

    // get the Manufacturer ID, Memory Type and Memory Density
    spi_Flash_ManufDeviceId(SPI_IDX_2, &u32Manufacturer, &u32MemoryType, &u32MemoryDensity);
    
    printf("Manufacturer ID[0x%X] Type[0x%X] Density[0x%X]\n", u32Manufacturer, u32MemoryType, u32MemoryDensity);
}

static uint32_t spi_Flash_ManufDeviceId(E_SpiIdx_t u32SpiIdx, uint32_t *pu32Manufacturer, uint32_t *pu32MemoryType, uint32_t *pu32MemoryDensity)
{
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;
    
    // Command: 0x9F
    // Tx MOSI: (w)8 bits command | (w)8 bits dummy | (w)8 bits dummy | (w)8 bits dummy
    // Rx MISO: (r)8 bits dummy   | (r)8 bits data  | (r)8 bits data  | (r)8 bits data

    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x9F;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | DUMMY;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | DUMMY;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | DUMMY;  // compelete
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, pu32Manufacturer);
    Hal_Spi_Data_Recv(u32SpiIdx, pu32MemoryType);
    Hal_Spi_Data_Recv(u32SpiIdx, pu32MemoryDensity);

    return 0;
}


