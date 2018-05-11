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
*  hal_flash_patch.c
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
*  Jeff Kuo
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File 
#include "hal_flash_patch.h"
#include "cmsis_os.h"
#include "boot_sequence.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define NO_FLASH       0
#define GIGADEVICE_ID  0xC8 /* GigaDevice, tested in GD25Q80C */
#define MACRONIX_ID    0xC2 /* Macronix (MX), tested in MX25V8035F */
#define WINBOND_NEX_ID 0xEF	/* Winbond (ex Nexcom) serial flashes, tested in W25Q80DV */

#define DUMMY          0x00

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef uint32_t (*T__Hal_Flash_WriteDoneCheck)(E_SpiIdx_t u32SpiIdx);
typedef void (*T__Hal_Flash_WriteEn)(E_SpiIdx_t u32SpiIdx);

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
extern uint8_t u8aFlashID[SPI_IDX_MAX];
osSemaphoreId g_taHalFlashSemaphoreId[SPI_IDX_MAX];

extern T__Hal_Flash_WriteDoneCheck _Hal_Flash_WriteDoneCheck;
extern T__Hal_Flash_WriteEn        _Hal_Flash_WriteEn;

// Sec 5: declaration of global function prototype
uint32_t Hal_Flash_Init_patch(E_SpiIdx_t u32SpiIdx);
uint32_t Hal_Flash_4KSectorAddrErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr);
uint32_t Hal_Flash_4KSectorIdxErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecIdx);
uint32_t Hal_Flash_PageAddrProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
uint32_t Hal_Flash_PageIdxProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageIdx, uint8_t u8UseQuadMode, uint8_t *pu8Data);
uint32_t Hal_Flash_PageAddrRead_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
void Hal_Flash_Reset_patch(E_SpiIdx_t u32SpiIdx);

uint32_t Hal_Flash_AddrProgram(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);
uint32_t Hal_Flash_AddrRead(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data);

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
extern uint32_t Hal_Flash_Init_impl(E_SpiIdx_t u32SpiIdx);
uint32_t Hal_Flash_Init_patch(E_SpiIdx_t u32SpiIdx)
{
    osSemaphoreDef_t tSemaphoreDef;

    // 0. check for cold boot only
    if (0 != Boot_CheckWarmBoot())
        return 0;   // the init is done in the cold boot

    if (0 != Hal_Flash_Init_impl(u32SpiIdx))
    {
        return 1;
    }

    // create the semaphore
    tSemaphoreDef.dummy = 0;                            // reserved, it is no used
    g_taHalFlashSemaphoreId[u32SpiIdx] = osSemaphoreCreate(&tSemaphoreDef, 1);
    if (g_taHalFlashSemaphoreId[u32SpiIdx] == NULL)
    {
        printf("To create the semaphore for Hal_Flash is fail.\n");
        return 1;
    }

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
extern uint32_t Hal_Flash_4KSectorAddrErase_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr);
uint32_t Hal_Flash_4KSectorAddrErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecAddr)
{
    uint8_t ubRet = 1;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    if (0 != Hal_Flash_4KSectorAddrErase_impl(u32SpiIdx, u32SecAddr))
    {
        goto done;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
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
extern uint32_t Hal_Flash_4KSectorIdxErase_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32SecIdx);
uint32_t Hal_Flash_4KSectorIdxErase_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32SecIdx)
{
    uint8_t ubRet = 1;

    if(u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    if (0 != Hal_Flash_4KSectorIdxErase_impl(u32SpiIdx, u32SecIdx))
    {
        goto done;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
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
extern uint32_t Hal_Flash_PageAddrProgram_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
uint32_t Hal_Flash_PageAddrProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    uint8_t ubRet = 1;
    
    if(u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    if (0 != Hal_Flash_PageAddrProgram_impl(u32SpiIdx, u32PageAddr, u8UseQuadMode, pu8Data))
    {
        goto done;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
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
extern uint32_t Hal_Flash_PageIdxProgram_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageIdx, uint8_t u8UseQuadMode, uint8_t *pu8Data);
uint32_t Hal_Flash_PageIdxProgram_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageIdx, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    uint8_t ubRet = 1;
    
    if(u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    if (0 != Hal_Flash_PageIdxProgram_impl(u32SpiIdx, u32PageIdx, u8UseQuadMode, pu8Data))
    {
        goto done;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
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
extern uint32_t Hal_Flash_PageAddrRead_impl(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data);
uint32_t Hal_Flash_PageAddrRead_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32PageAddr, uint8_t u8UseQuadMode, uint8_t *pu8Data)
{
    uint8_t ubRet = 1;
    
    if(u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    if (0 != Hal_Flash_PageAddrRead_impl(u32SpiIdx, u32PageAddr, u8UseQuadMode, pu8Data))
    {
        goto done;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
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
extern void Hal_Flash_Reset_impl(E_SpiIdx_t u32SpiIdx);
void Hal_Flash_Reset_patch(E_SpiIdx_t u32SpiIdx)
{
    if(u32SpiIdx >= SPI_IDX_MAX)
        return;

    if(u8aFlashID[u32SpiIdx] == NO_FLASH)
        return;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

    Hal_Flash_Reset_impl(u32SpiIdx);

    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_AddrProgram
*
* DESCRIPTION:
*   1. Program(write) n bytes from the start address
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*   2. u32StartAddr : Start address
*   3. u8UseQuadMode: Qaud-mode select. 1 for enable/0 for disable
*   4. u32Size      : Data size
*   5. pu8Data      : Data buffer
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_AddrProgram(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data)
{
    uint32_t u32Idx = 0;    
    uint32_t u32Temp = 0;
    uint32_t u32DataAddr = 0;
    uint32_t u32DataSize = 0;
    uint32_t u32DataIdx = 0;

    uint8_t ubRet = 1;

    if (u32SpiIdx >= SPI_IDX_MAX)
        return ubRet;

    if (u8aFlashID[u32SpiIdx] == NO_FLASH)
        return ubRet;
    
    if (u32Size == 0)
        return ubRet;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);
    
    // 1. handle the data size of the first page if the start address is not aligned
    u32DataAddr = u32StartAddr;
    u32DataSize = 256 - (u32DataAddr & 0xFF);
    if (u32DataSize > u32Size)
        u32DataSize = u32Size;
    u32DataIdx = 0;
    
    // 2. program the data
    while (u32Size > 0)
    {
        _Hal_Flash_WriteEn(u32SpiIdx);

        if( (u8aFlashID[u32SpiIdx] == GIGADEVICE_ID) || (u8aFlashID[u32SpiIdx] == WINBOND_NEX_ID))
        {
            // Cmd
            if (u8UseQuadMode)
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x32;
            else
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x02;
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            // Addr
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
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
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            }
            else
            {
                // Cmd
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x02;
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                // Addr
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
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
            for (u32Idx=0; u32Idx<u32DataSize; u32Idx++)
            {
                if (u32Idx != (u32DataSize-1))
                    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | pu8Data[u32DataIdx+u32Idx];
                else
                    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_4_BIT | TAG_WRITE | pu8Data[u32DataIdx+u32Idx]; // complete
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                
                Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
            }
        }
        else
        {
            for (u32Idx=0; u32Idx<u32DataSize; u32Idx++)
            {
                if (u32Idx != (u32DataSize-1))
                    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | pu8Data[u32DataIdx+u32Idx];
                else
                    u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_WRITE | pu8Data[u32DataIdx+u32Idx]; // complete
                Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
                
                Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp); // dummy
            }
        }
        
        if (0 != _Hal_Flash_WriteDoneCheck(u32SpiIdx))
            goto done;
        
        // update the next size and address
        u32DataIdx += u32DataSize;
        u32DataAddr += u32DataSize;
        u32Size -= u32DataSize;
        if (u32Size > 256)
            u32DataSize = 256;
        else
            u32DataSize = u32Size;
    }

    ubRet = 0;

done:
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);
    return ubRet;
}

/*************************************************************************
* FUNCTION:
*  Hal_Flash_AddrRead
*
* DESCRIPTION:
*   1. Read n bytes from the start address
*
* CALLS
*
* PARAMETERS
*   1. eSpiIdx      : Index of SPI. refert to E_SpiIdx_t
*   2. u32StartAddr : Start address
*   3. u8UseQuadMode: Qaud-mode select. 1 for enable/0 for disable
*   4. u32Size      : Data size
*   5. pu8Data      : Data buffer
*
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Flash_AddrRead(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data)
{
    uint32_t u32Idx = 0;
    uint32_t u32Temp = 0;

    if (u32SpiIdx >= SPI_IDX_MAX)
        return 1;

    if (u8aFlashID[u32SpiIdx] == NO_FLASH)
        return 1;
    
    if ((u32Size == 0) || (pu8Data == NULL))
        return 1;

    // wait the semaphore
    osSemaphoreWait(g_taHalFlashSemaphoreId[u32SpiIdx], osWaitForever);

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
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32StartAddr>>16) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32StartAddr>>8) & 0xFF );
    Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32StartAddr & 0xFF);
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
        for (u32Idx=0; u32Idx<u32Size; u32Idx++)
        {
            if (u32Idx != (u32Size-1))
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
        for (u32Idx=0; u32Idx<u32Size; u32Idx++)
        {
            if (u32Idx != (u32Size-1))
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_READ | DUMMY;
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | TAG_1_BIT | TAG_READ | DUMMY; // complete
            Hal_Spi_Data_Send(u32SpiIdx, u32Temp);
            
            Hal_Spi_Data_Recv(u32SpiIdx, &u32Temp);
            pu8Data[u32Idx] = (uint8_t)( u32Temp&0xFF );
        }
    }
    
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);

    return 0;
}
