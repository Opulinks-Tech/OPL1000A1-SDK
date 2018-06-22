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

#define SPI_0     ((S_Spi_Reg_t *) SPI0_BASE)
#define SPI_1     ((S_Spi_Reg_t *) SPI1_BASE)
#define SPI_2     ((S_Spi_Reg_t *) SPI2_BASE)

#define SPI_SR_RX_NOT_EMPTY          (1<<3)
#define SPI_SR_TX_NOT_FULL           (1<<1)
#define SPI_TIMEOUT 0x5000

typedef struct
{
    volatile uint32_t CTRLR0;  // 0x00
	volatile uint32_t CTRLR1;  // 0x04
	volatile uint32_t SSIENR;  // 0x08
	volatile uint32_t MWCR;    // 0x0C
	volatile uint32_t SER;     // 0x10
	volatile uint32_t BAUDR;   // 0x14
	volatile uint32_t TXFTLR;  // 0x18
	volatile uint32_t RXFTLR;  // 0x1C
	volatile uint32_t TXFLR;   // 0x20
	volatile uint32_t RXFLR;   // 0x24
	volatile uint32_t SR;      // 0x28
	volatile uint32_t IMR;     // 0x2C
	volatile uint32_t ISR;     // 0x30
	volatile uint32_t PISR;    // 0x34
	volatile uint32_t TXOICR;  // 0x38
	volatile uint32_t RXOICR;  // 0x3C
	volatile uint32_t RXUICR;  // 0x40
	volatile uint32_t MSTICR;  // 0x44
	volatile uint32_t ICR;     // 0x48
	volatile uint32_t DMACR;   // 0x4C
	volatile uint32_t DMATDLR; // 0x50
	volatile uint32_t DMARDLR; // 0x54
	volatile uint32_t IDR;     // 0x58
	volatile uint32_t SSI_VER; // 0x5C
	volatile uint32_t DR[36];  // 0x60 ~ 0xEC
} S_Spi_Reg_t;

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
    S_Spi_Reg_t *pSpi = 0;
    uint32_t u32Idx = 0;    
    uint32_t u32Temp = 0;
    uint32_t u32QuadBit = 0;
    uint32_t u32DataAddr = 0;
    uint32_t u32DataSize = 0;
    uint32_t u32DataIdx = 0;
    
    uint32_t u32TimeOut;
    uint32_t i;

    uint8_t ubRet = 1;

    if (u32SpiIdx == SPI_IDX_0)
	{
        pSpi = SPI_0;
	}
    else if (u32SpiIdx == SPI_IDX_1)
    {
        pSpi = SPI_1;
    }
    else if (u32SpiIdx == SPI_IDX_2)
    {
        pSpi = SPI_2;
    }
    else
    {
        return ubRet;
    }

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
            pSpi->DR[0] = u32Temp;
            // Addr
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
            pSpi->DR[0] = u32Temp;
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
            pSpi->DR[0] = u32Temp;
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
            pSpi->DR[0] = u32Temp;
        }
        if(u8aFlashID[u32SpiIdx] == MACRONIX_ID)
        {
            if (u8UseQuadMode)
            {
                // Cmd
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x38;
                pSpi->DR[0] = u32Temp;
                // Addr
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
                pSpi->DR[0] = u32Temp;
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
                pSpi->DR[0] = u32Temp;
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_4_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
                pSpi->DR[0] = u32Temp;
            }
            else
            {
                // Cmd
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x02;
                pSpi->DR[0] = u32Temp;
                // Addr
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>16) & 0xFF );
                pSpi->DR[0] = u32Temp;
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32DataAddr>>8) & 0xFF );
                pSpi->DR[0] = u32Temp;
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32DataAddr & 0xFF);
                pSpi->DR[0] = u32Temp;
                
            }
        }

        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        u32Temp = pSpi->DR[0];  // dummy
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        u32Temp = pSpi->DR[0];  // dummy
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        u32Temp = pSpi->DR[0];  // dummy
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        u32Temp = pSpi->DR[0];  // dummy
        
        
        // programe data
        // decide the quad mode or standard mode
        if (u8UseQuadMode)
            u32QuadBit = TAG_4_BIT;
        else
            u32QuadBit = TAG_1_BIT;
            
        // 4 bytes per round
        for (u32Idx=0; (u32Idx+4)<u32DataSize; u32Idx+=4)
        {
            pSpi->DR[0] = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+u32Idx];
            pSpi->DR[0] = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+u32Idx+1];
            pSpi->DR[0] = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+u32Idx+2];
            pSpi->DR[0] = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+u32Idx+3];
            
            u32TimeOut = 0;
            while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
            {
                // Wait for RX not Empty...
                
                //Time-out check
                if( u32TimeOut > SPI_TIMEOUT )
                    return 1;
                u32TimeOut++;
            }
            u32Temp = pSpi->DR[0];  // dummy
            
            u32TimeOut = 0;
            while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
            {
                // Wait for RX not Empty...
                
                //Time-out check
                if( u32TimeOut > SPI_TIMEOUT )
                    return 1;
                u32TimeOut++;
            }
            u32Temp = pSpi->DR[0];  // dummy
            
            u32TimeOut = 0;
            while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
            {
                // Wait for RX not Empty...
                
                //Time-out check
                if( u32TimeOut > SPI_TIMEOUT )
                    return 1;
                u32TimeOut++;
            }
            u32Temp = pSpi->DR[0];  // dummy
            
            u32TimeOut = 0;
            while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
            {
                // Wait for RX not Empty...
                
                //Time-out check
                if( u32TimeOut > SPI_TIMEOUT )
                    return 1;
                u32TimeOut++;
            }
            u32Temp = pSpi->DR[0];  // dummy
        }
        
        // the rest data
        for (i=u32Idx; i<u32DataSize; i++)
        {
            if (i != (u32DataSize-1))
                u32Temp = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+i];
            else
                u32Temp = TAG_DFS_08 | TAG_CS_COMP | u32QuadBit | TAG_WRITE | pu8Data[u32DataIdx+i]; // complete
            pSpi->DR[0] = u32Temp;
        }
        for (i=u32Idx; i<u32DataSize; i++)
        {
            u32TimeOut = 0;
            while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
            {
                // Wait for RX not Empty...
                
                //Time-out check
                if( u32TimeOut > SPI_TIMEOUT )
                    return 1;
                u32TimeOut++;
            }
            u32Temp = pSpi->DR[0];  // dummy
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
    S_Spi_Reg_t *pSpi = 0;
    uint32_t u32Idx = 0;
    uint32_t u32Temp = 0;
    uint32_t u32QuadBit = 0;
    
    uint32_t u32TimeOut;
    uint32_t i;

    if (u32SpiIdx == SPI_IDX_0)
	{
        pSpi = SPI_0;
	}
    else if (u32SpiIdx == SPI_IDX_1)
    {
        pSpi = SPI_1;
    }
    else if (u32SpiIdx == SPI_IDX_2)
    {
        pSpi = SPI_2;
    }
    else
    {
        return 1;
    }

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
        pSpi->DR[0] = u32Temp;
    }
    else
    {
        u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | 0x0B;
        pSpi->DR[0] = u32Temp;
    }
    u32TimeOut = 0;
    while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
    {
        // Wait for RX not Empty...
        
        //Time-out check
        if( u32TimeOut > SPI_TIMEOUT )
            return 1;
        u32TimeOut++;
    }
    u32Temp = pSpi->DR[0];  // dummy
    
    // Addr
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32StartAddr>>16) & 0xFF );
    pSpi->DR[0] = u32Temp;
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | ( (u32StartAddr>>8) & 0xFF );
    pSpi->DR[0] = u32Temp;
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | (u32StartAddr & 0xFF);
    pSpi->DR[0] = u32Temp;
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | TAG_1_BIT | TAG_WRITE | DUMMY;
    pSpi->DR[0] = u32Temp;

    u32TimeOut = 0;
    while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
    {
        // Wait for RX not Empty...
        
        //Time-out check
        if( u32TimeOut > SPI_TIMEOUT )
            return 1;
        u32TimeOut++;
    }
    u32Temp = pSpi->DR[0];  // dummy
    
    u32TimeOut = 0;
    while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
    {
        // Wait for RX not Empty...
        
        //Time-out check
        if( u32TimeOut > SPI_TIMEOUT )
            return 1;
        u32TimeOut++;
    }
    u32Temp = pSpi->DR[0];  // dummy
    
    u32TimeOut = 0;
    while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
    {
        // Wait for RX not Empty...
        
        //Time-out check
        if( u32TimeOut > SPI_TIMEOUT )
            return 1;
        u32TimeOut++;
    }
    u32Temp = pSpi->DR[0];  // dummy
    
    u32TimeOut = 0;
    while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
    {
        // Wait for RX not Empty...
        
        //Time-out check
        if( u32TimeOut > SPI_TIMEOUT )
            return 1;
        u32TimeOut++;
    }
    u32Temp = pSpi->DR[0];  // dummy
    
    
    // read data
    // decide the quad mode or standard mode
    if (u8UseQuadMode)
        u32QuadBit = TAG_4_BIT;
    else
        u32QuadBit = TAG_1_BIT;

    // 4 bytes per round
    u32Temp = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_READ | DUMMY;
    for (u32Idx=0; (u32Idx+4)<u32Size; u32Idx+=4)
    {
        pSpi->DR[0] = u32Temp;
        pSpi->DR[0] = u32Temp;
        pSpi->DR[0] = u32Temp;
        pSpi->DR[0] = u32Temp;
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        pu8Data[u32Idx] = (uint8_t)( pSpi->DR[0] & 0xFF );
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        pu8Data[u32Idx+1] = (uint8_t)( pSpi->DR[0] & 0xFF );
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        pu8Data[u32Idx+2] = (uint8_t)( pSpi->DR[0] & 0xFF );
        
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        pu8Data[u32Idx+3] = (uint8_t)( pSpi->DR[0] & 0xFF );
    }

    // the rest data
    for (i=u32Idx; i<u32Size; i++)
    {
        if (i != (u32Size-1))
            u32Temp = TAG_DFS_08 | TAG_CS_CONT | u32QuadBit | TAG_READ | DUMMY;
        else
            u32Temp = TAG_DFS_08 | TAG_CS_COMP | u32QuadBit | TAG_READ | DUMMY; // complete
        pSpi->DR[0] = u32Temp;
    }
    for (i=u32Idx; i<u32Size; i++)
    {
        u32TimeOut = 0;
        while( !(pSpi->SR & SPI_SR_RX_NOT_EMPTY) )
        {
            // Wait for RX not Empty...
            
            //Time-out check
            if( u32TimeOut > SPI_TIMEOUT )
                return 1;
            u32TimeOut++;
        }
        pu8Data[i] = (uint8_t)( pSpi->DR[0] & 0xFF );
    }
    
    // release the semaphore
    osSemaphoreRelease(g_taHalFlashSemaphoreId[u32SpiIdx]);

    return 0;
}
