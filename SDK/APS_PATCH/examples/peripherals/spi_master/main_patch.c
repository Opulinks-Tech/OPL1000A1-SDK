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
*  SH SDK
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
*  spi_flash_test() is an example of using SPI0 to access SPI flash. The operation is *   (1) write a block data to certain area. 
*   (2) then read it back and compare with original data.  
*   Flash area address begins from 0x00090000, size 1600 bytes. 
*       
*  spi_send_data() is an example of access an external SPI slave device through 
           SPI1 and SPI2 port 
*  
*  SPI1 and SPI2 signal pin and parameters are defined by OPL1000_periph.spi
* 
******************************************************************************/


// Sec 1: Include File
#include <stdio.h>
#include <string.h>
#include "sys_init_patch.h"
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "Hal_pinmux_spi.h"
#include "hal_flash_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define DUMMY          0x00
#define SPI1_IDX       0 
#define SPI2_IDX       1
#define TEST_SPI       SPI2_IDX

#define FLASH_START_ADDR  0x90   // 0x00090000
#define BLOCK_DATA_SIZE   1600   // bytes
#define SECTION_INDEX     0      // one section is 4 kbytes  
#define MIN_DATA_VALUE    50 
#define MAX_DATA_VALUE    200    // note:(MAX_DATA_VALUE + MIN_DATA_VALUE) < 255 

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, union, enum, linked list


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
static void spi_send_data(int idx);



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
    printf("SPI initialization  \r\n");
    Hal_Pinmux_Spi_Init(&OPL1000_periph.spi[SPI1_IDX]);
    Hal_Pinmux_Spi_Init(&OPL1000_periph.spi[SPI2_IDX]);    
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
    
    // call spi_flash_test, use SPI0 to access on board SPI flash 
    spi_flash_test();
    
    spi_send_data(SPI1_IDX);
    
    spi_send_data(SPI2_IDX);    
    
    while(1);
}

/*************************************************************************
* FUNCTION:
*   spi_flash_test
*
* DESCRIPTION:
*   an example that read and write data on SPI0 to access on board spi flash.
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
    uint8_t u8BlockData[BLOCK_DATA_SIZE],u8ReadData[BLOCK_DATA_SIZE] = {0};
    uint32_t i,u32Length,u32SectorAddr32bit;
    uint16_t u16SectorAddr;
    bool match_flag = true; 
    
    u32Length = BLOCK_DATA_SIZE;
      
    // prepare test block data 
    for (i=0; i<u32Length; i++)
        u8BlockData[i] = (i%MAX_DATA_VALUE) + MIN_DATA_VALUE;
      
    u16SectorAddr = FLASH_START_ADDR + SECTION_INDEX; // one section is 4k Bytes 
      
    u32SectorAddr32bit =  (((uint32_t)u16SectorAddr) << 12) & 0x000ff000;
    // Erase flash firstly  
    Hal_Flash_4KSectorAddrErase_patch(SPI_IDX_0, u32SectorAddr32bit);
    // Write u8BlockData into flash   
    Hal_Flash_AddrProgram(SPI_IDX_0, u32SectorAddr32bit, 0, u32Length, u8BlockData);
    // Read flash content from SectorAddr32bit

    Hal_Flash_AddrRead(SPI_IDX_0, u32SectorAddr32bit, 0, u32Length, u8ReadData );
    for(i=0; i<u32Length; i++)
    {
        if(u8BlockData[i] != u8ReadData[i] )
        {
            printf("No.%d data error. write %x, read %x \r\n",i+1, u8BlockData[i],u8ReadData[i]);
            match_flag = false;
        }
    }
    if (match_flag == true)
    {
        printf("Write and read %d bytes data on flash @%x successfully \r\n",u32Length, u32SectorAddr32bit); 
    }
}



static void spi_send_data(int idx)
{
    char output_str[32]= {0};   
    uint32_t u32Data, i;
    T_OPL1000_Spi *spi;

    sprintf(output_str,"Hello from SPI%d \r\n",idx+1);
    spi = &OPL1000_periph.spi[idx];
    printf("Send data to external SPI%d slave device \r\n",idx+1);    
    for(i=0;i<strlen(output_str);i++)
    {
        u32Data = output_str[i];
        Hal_Spi_Data_Send(spi->index,u32Data);
    }    
}
