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
*  OPL1000 series
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
#include "hal_flash_internal.h"
#include "cmsis_os.h"
#include "boot_sequence.h"

#include "hal_flash_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define DUMMY          0x00
#define FLASH_TIMEOUT  0x5000

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

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
extern uint8_t g_u8aHalFlashID[SPI_IDX_MAX];

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
uint32_t Hal_Flash_AddrProgram_Internal_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data)
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

    if (g_u8aHalFlashID[u32SpiIdx] == NO_FLASH)
        return 1;
    
    if (u32Size == 0)
        return 1;
    
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

        if(g_u8aHalFlashID[u32SpiIdx] == MACRONIX_ID)
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
        else
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
            return 1;
        
        // update the next size and address
        u32DataIdx += u32DataSize;
        u32DataAddr += u32DataSize;
        u32Size -= u32DataSize;
        if (u32Size > 256)
            u32DataSize = 256;
        else
            u32DataSize = u32Size;
    }
    
    return 0;
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
uint32_t Hal_Flash_AddrRead_Internal_patch(E_SpiIdx_t u32SpiIdx, uint32_t u32StartAddr, uint8_t u8UseQuadMode, uint32_t u32Size, uint8_t *pu8Data)
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

    if (g_u8aHalFlashID[u32SpiIdx] == NO_FLASH)
        return 1;
    
    if ((u32Size == 0) || (pu8Data == NULL))
        return 1;

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

    return 0;
}
