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
*  hal_flash.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This source file defines the functions of flash.
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_flash.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define NO_FLASH       0
#define GIGADEVICE_ID  0xC8 /* GigaDevice, tested in GD25Q80C */
#define MACRONIX_ID    0xC2 /* Macronix (MX), tested in MX25V8035F */
#define WINBOND_NEX_ID 0xEF	/* Winbond (ex Nexcom) serial flashes, tested in W25Q80DV */

#define DUMMY          0x00

#define FLASH_TIMEOUT  0x5000
/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef uint32_t (*T__Hal_Flash_ManufDeviceId)(E_SpiIdx_t u32SpiIdx, uint32_t *pu32Manufacturer, uint32_t *pu32MemoryType, uint32_t *pu32MemoryDensity );
typedef uint32_t (*T__Hal_Flash_WriteDoneCheck)(E_SpiIdx_t u32SpiIdx);
typedef void (*T__Hal_Flash_WriteEn)(E_SpiIdx_t u32SpiIdx);
typedef uint32_t (*T__Hal_Flash_QuadModeEn)(E_SpiIdx_t u32SpiIdx, uint8_t u8QModeEn);

T__Hal_Flash_ManufDeviceId  _Hal_Flash_ManufDeviceId;
T__Hal_Flash_WriteDoneCheck _Hal_Flash_WriteDoneCheck;
T__Hal_Flash_WriteEn        _Hal_Flash_WriteEn;
T__Hal_Flash_QuadModeEn     _Hal_Flash_QuadModeEn;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
static uint8_t u8aFlashID[SPI_IDX_MAX] = {NO_FLASH, NO_FLASH, NO_FLASH};

RET_DATA T__Hal_Flash_ManufDeviceId  _Hal_Flash_ManufDeviceId;
RET_DATA T__Hal_Flash_WriteDoneCheck _Hal_Flash_WriteDoneCheck;
RET_DATA T__Hal_Flash_WriteEn        _Hal_Flash_WriteEn;
RET_DATA T__Hal_Flash_QuadModeEn     _Hal_Flash_QuadModeEn;

RET_DATA T_Hal_Flash_Init              Hal_Flash_Init;
RET_DATA T_Hal_Flash_4KSectorAddrErase Hal_Flash_4KSectorAddrErase;
RET_DATA T_Hal_Flash_4KSectorIdxErase  Hal_Flash_4KSectorIdxErase;
RET_DATA T_Hal_Flash_PageAddrProgram   Hal_Flash_PageAddrProgram;
RET_DATA T_Hal_Flash_PageIdxProgram    Hal_Flash_PageIdxProgram;
RET_DATA T_Hal_Flash_PageAddrRead      Hal_Flash_PageAddrRead;
RET_DATA T_Hal_Flash_Reset             Hal_Flash_Reset;
RET_DATA T_Hal_Flash_Check             Hal_Flash_Check;

// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable

// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions
/*************************************************************************
* FUNCTION:
*  _Hal_Flash_ManufDeviceId
*
* DESCRIPTION:
*   1. Get flash Manufacturer ID, Memory Type and Memory Density
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx          : Index of SPI. refert to E_SpiIdx_t
*   2. pu32Manufacturer : Pointer to receive Manufacturer ID
*   3. pu32MemoryType   : Pointer to receive Memory Type
*   4. pu32MemoryDensity: Pointer to receive Memory Density
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t _Hal_Flash_ManufDeviceId_impl(E_SpiIdx_t u32SpiIdx, uint32_t *pu32Manufacturer, uint32_t *pu32MemoryType, uint32_t *pu32MemoryDensity )
{
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

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

/*************************************************************************
* FUNCTION:
*  _Hal_Flash_WriteDoneCheck
*
* DESCRIPTION:
*   1. Check flash write finished
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t _Hal_Flash_WriteDoneCheck_impl(E_SpiIdx_t u32SpiIdx)
{
    uint32_t u32Status = 0;
    uint32_t u32TimeOut = 0;
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    do
    {
        if(u32TimeOut > FLASH_TIMEOUT)
            return 1;
        
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x05;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | DUMMY; // compelte
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        
        Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
        Hal_Spi_Data_Recv(u32SpiIdx, &u32Status);
        
        u32TimeOut++;
    }while( u32Status & 0x01 );
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  _Hal_Flash_WriteEn
*
* DESCRIPTION:
*   1. Enable flash to write
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*
* RETURNS
*   NONE
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void _Hal_Flash_WriteEn_impl(E_SpiIdx_t u32SpiIdx)
{
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
            return;

    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x06;  // complete
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
}

/*************************************************************************
* FUNCTION:
*  _Hal_Flash_QuadModeEn
*
* DESCRIPTION:
*   1. Enable flash into Quad mode
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx   : Index of SPI. refert to E_SpiIdx_t
*   2. u8QModeEn : Qaud-mode select. 1 for enable/0 for disable
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t _Hal_Flash_QuadModeEn_impl(E_SpiIdx_t u32SpiIdx, uint8_t u8QModeEn)
{
    uint32_t u32Temp = 0;
    _Hal_Flash_WriteEn(u32SpiIdx);
    
    if( (u8aFlashID[u32SpiIdx] == GIGADEVICE_ID) || (u8aFlashID[u32SpiIdx] == WINBOND_NEX_ID))
    {
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x01;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x00;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        if (u8QModeEn)
            u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x02; // complete
        else
            u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x00; // complete
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    }
    else if(u8aFlashID[u32SpiIdx] == MACRONIX_ID)
    {
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x01;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        if (u8QModeEn == 1)
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x40;
        else
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x00;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x00; // comeplte
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    }
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    return _Hal_Flash_WriteDoneCheck(u32SpiIdx);
}


/*************************************************************************
* FUNCTION:
*  Hal_Flash_Check
*
* DESCRIPTION:
*   1. Check flash is OK or not
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*
* RETURNS
*   refert to E_FlashCheckStatus_t
* 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
E_FlashCheckStatus_t Hal_Flash_Check_impl(E_SpiIdx_t u32SpiIdx)
{
    if(u8aFlashID[u32SpiIdx] == NO_FLASH) {
        return NOT_SUPPORTED;
    } else {
        return SUPPORTED_FLASH;
    }
}
/*************************************************************************
* FUNCTION:
*  Hal_Flash_Init
*
* DESCRIPTION:
*   1. Init flash status
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*
* RETURNS
*   0: setting complete (Identified SPI flash)
*   1: error  (No SPI flash)
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_Init_impl(E_SpiIdx_t u32SpiIdx)
{
    uint32_t u32Manufacturer =0, u32MemoryType=0, u32MemoryDensity=0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    // Read flash ID
    _Hal_Flash_ManufDeviceId(u32SpiIdx, &u32Manufacturer, &u32MemoryType, &u32MemoryDensity);

    // ID Cheack
    if( (u32Manufacturer == GIGADEVICE_ID) || (u32Manufacturer == MACRONIX_ID) || (u32Manufacturer == WINBOND_NEX_ID) )
    {
        // Suppoerted 
        u8aFlashID[u32SpiIdx] = u32Manufacturer;
    }
    else
    {
        // Not supported
        u8aFlashID[u32SpiIdx] = NO_FLASH;
        return 1;
    }

    // Enable Quad mode
    _Hal_Flash_QuadModeEn(u32SpiIdx, 1);

    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_4KSectorAddrErase
*
* DESCRIPTION:
*   1. Erase a sector (4 KB)
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*   2. u32SecAddr : Address of sector (must sector aligned, LSBs truncated)
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_4KSectorAddrErase_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr)
{
    uint32_t u32Temp = 0;
    uint32_t u32Addr = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return 1;

    _Hal_Flash_WriteEn(u32SpiIdx);

    // Cmd
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x20;

    // Addr
    u32Addr = u32SecAddr & ~0xFFF; // aligned
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>16) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>8) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | (u32Addr & 0xFF); // Complete
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    return _Hal_Flash_WriteDoneCheck(u32SpiIdx);
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_4KSectorIdxErase
*
* DESCRIPTION:
*   1. Erase a sector (4 KB)
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx    : Index of SPI. refert to E_SpiIdx_t
*   2. u32SecAddr : Index of sector.
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_4KSectorIdxErase_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32SecIdx)
{
    return Hal_Flash_4KSectorAddrErase_impl(u32SpiIdx, u32SecIdx*0x1000);
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_PageAddrProgram
*
* DESCRIPTION:
*   1. Program(write) a page (256 B)
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*   2. u32PageAddr  : Address of page. (must page aligned, LSBs truncated)
*   3. u8UseQuadMode: Qaud-mode select. 1 for enable/0 for disable
*   4. pu8Data      : Data buffer
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_PageAddrProgram_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    uint32_t u32Idx = 0;    
    uint32_t u32Temp = 0;
    uint32_t u32Addr = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return 1;
    
    _Hal_Flash_WriteEn(u32SpiIdx);

    u32Addr = u32PageAddr & ~0xFF; // aligned
    if( (u8aFlashID[u32SpiIdx] == GIGADEVICE_ID) || (u8aFlashID[u32SpiIdx] == WINBOND_NEX_ID))
    {
        // Cmd
        if (u8UseQuadMode)
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x32;
        else
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x02;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        // Addr
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>16) & 0xFF );
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>8) & 0xFF );
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32Addr & 0xFF);
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    }
    if(u8aFlashID[u32SpiIdx] == MACRONIX_ID)
    {
        if (u8UseQuadMode)
        {
            // Cmd
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x38;
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            // Addr
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32Addr>>16) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32Addr>>8) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | (u32Addr & 0xFF);
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
        }
        else
        {
            // Cmd
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x02;
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            // Addr
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>16) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32Addr>>8) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32Addr & 0xFF);
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
        }
    }

    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    // programe data
    if (u8UseQuadMode)
    {
        for (u32Idx=0; u32Idx<256; u32Idx++)
        {
            if (u32Idx != 255)
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | pu8Data[u32Idx];
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_4_BIT | TAG_WRITE | pu8Data[u32Idx]; // complete
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
            Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
        }
    }
    else
    {
        for (u32Idx=0; u32Idx<256; u32Idx++)
        {
            if (u32Idx != 255)
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | pu8Data[u32Idx];
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | pu8Data[u32Idx]; // complete
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
            Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
        }
    }
    return _Hal_Flash_WriteDoneCheck(u32SpiIdx);
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_PageIdxProgram
*
* DESCRIPTION:
*   1. Program(write) a page (256 B)
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*   2. u32PageAddr  : Index of page.
*   3. u8UseQuadMode: Qaud-mode select. 1 for enable/0 for disable
*   4. pu8Data      : Data buffer
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_PageIdxProgram_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageIdx, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    return Hal_Flash_PageAddrProgram_impl(u32SpiIdx, u32PageIdx*0x100, u8UseQuadMode, pu8Data);
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_PageAddrRead
*
* DESCRIPTION:
*   1. Read a page (256 B) 
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*   2. u32PageAddr  : Address of page.
*   3. u8UseQuadMode: Qaud-mode select. 1 for enable/0 for disable
*   4. pu8Data      : Data buffer
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_PageAddrRead_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    uint32_t u32Idx = 0;
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return 1;

    // read cmd
    if (u8UseQuadMode)
    {
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x6B;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    }
    else
    {
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x0B;
        Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    }
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    // Addr
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32PageAddr>>16) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32PageAddr>>8) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32PageAddr & 0xFF);
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | DUMMY;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    // read data
    if (u8UseQuadMode)
    {
        for (u32Idx=0; u32Idx<256; u32Idx++)
        {
            if (u32Idx != 255)
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_READ | DUMMY;
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_4_BIT | TAG_READ | DUMMY; // complete
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
            Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp);
            pu8Data[u32Idx] = (uint8_t)( u32Temp&0xFF );
        }
    }
    else
    {
        for (u32Idx=0; u32Idx<256; u32Idx++)
        {
            if (u32Idx != 255)
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_READ | DUMMY;
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_READ | DUMMY; // complete
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
            Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp);
            pu8Data[u32Idx] = (uint8_t)( u32Temp&0xFF );
        }
    }
		return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_Reset
*
* DESCRIPTION:
*   1. Flash reset
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*
* RETURNS
*   None
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Flash_Reset_impl(E_SpiIdx_t u32SpiIdx)
{
    uint32_t u32Temp = 0;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return;

    // Reset enable
    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x66;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
    
    // Reset
    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | 0x99;
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    
    Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
}

/*************************************************************************
* FUNCTION:
*  _Hal_Flash_FunPoiterInit
*
* DESCRIPTION:
*   1. Inital of functuin pointers
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void _Hal_Flash_FunPoiterInit(void)
{
    _Hal_Flash_ManufDeviceId  = _Hal_Flash_ManufDeviceId_impl;
    _Hal_Flash_WriteDoneCheck = _Hal_Flash_WriteDoneCheck_impl;
    _Hal_Flash_WriteEn        = _Hal_Flash_WriteEn_impl;
    _Hal_Flash_QuadModeEn     = _Hal_Flash_QuadModeEn_impl;

    Hal_Flash_Init              = Hal_Flash_Init_impl;
    Hal_Flash_4KSectorAddrErase = Hal_Flash_4KSectorAddrErase_impl;
    Hal_Flash_4KSectorIdxErase  = Hal_Flash_4KSectorIdxErase_impl;
    Hal_Flash_PageAddrProgram   = Hal_Flash_PageAddrProgram_impl;
    Hal_Flash_PageIdxProgram    = Hal_Flash_PageIdxProgram_impl;
    Hal_Flash_PageAddrRead      = Hal_Flash_PageAddrRead_impl;
    Hal_Flash_Reset             = Hal_Flash_Reset_impl;
    Hal_Flash_Check             = Hal_Flash_Check_impl;
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_ParamInit
*
* DESCRIPTION:
*   1. Inital of glabal parameters
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Flash_ParamInit(void)
{
    u8aFlashID[ SPI_IDX_0 ] = NO_FLASH;
    u8aFlashID[ SPI_IDX_1 ] = NO_FLASH;
    u8aFlashID[ SPI_IDX_2 ] = NO_FLASH;
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_Pre_Init
*
* DESCRIPTION:
*   1. Inital for functuin pointers and glabal parameters
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_Flash_Pre_Init(void)
{
    // Param
    Hal_Flash_ParamInit();

    // Function pointers
    _Hal_Flash_FunPoiterInit();
}
