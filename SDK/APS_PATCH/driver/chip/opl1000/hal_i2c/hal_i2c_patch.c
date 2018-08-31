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
*  hal_i2c_patch.c
*
*  Project:
*  --------
*  OPL1000 series
*
*  Description:
*  ------------
*  This source file defines the functions of I2C .
*  Ref. document is << DesignWare DW_apb_i2c Databook >>
*
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include "opl1000.h"
#include "hal_system.h"
#include "hal_vic.h"
#include "hal_i2c_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define I2C       ((S_I2C_Reg_t *) I2C_BASE)

#define I2C_CLK_STD              100000
#define I2C_CLK_FAST             400000
#define I2C_CLK_HIGH             3400000
#define I2C_CLK_SUPPORT_MAX      I2C_CLK_FAST

#define I2C_CON_SLAVE_DISABLE    (1<<6)
#define I2C_CON_RESTART_EN       (1<<5)
#define I2C_CON_MASTER_07BIT     (0<<4)
#define I2C_CON_MASTER_10BIT     (1<<4)
#define I2C_CON_SLAVE_10BIT      (1<<3)
#define I2C_CON_SPEED_STD        (1<<1)
#define I2C_CON_SPEED_FAST       (2<<1)
#define I2C_CON_SPEED_HIGH       (3<<1)
#define I2C_CON_SPEED_MASK       (0x3<<1)
#define I2C_CON_MASTER_MODE      (1<<0)

#define I2C_INT_TX_ABRT          (1<<6)
#define I2C_INT_RD_REQ           (1<<5)
#define I2C_INT_RX_FULL          (1<<2)
#define I2C_DATA_RESTART_BIT     (1<<10)
#define I2C_DATA_STOP_BIT        (1<<9)
#define I2C_DATA_CMD_WRITE       (0<<8)
#define I2C_DATA_CMD_READ        (1<<8)
#define I2C_DATA_CMD_DATA_MASK   (0xFF)
#define I2C_STATUS_SLV_ACTIVITY  (1<<6)
#define I2C_STATUS_MST_ACTIVITY  (1<<5)
#define I2C_STATUS_RX_FULL       (1<<4)
#define I2C_STATUS_RX_NOT_EMPTY  (1<<3)
#define I2C_STATUS_TX_EMPTY      (1<<2)
#define I2C_STATUS_TX_NOT_FULL   (1<<1)
#define I2C_ENABLE_EN            1
#define I2C_ENABLE_STATUS_EN     1
#define I2C_TIMEOUT_COUNT_MAX    (0x30000)
#define I2C_FIFO_SIZE            8

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
typedef struct
{
    volatile uint32_t CON;               // 0x00
    volatile uint32_t TAR;               // 0x04
    volatile uint32_t SAR;               // 0x08
    volatile uint32_t HS_MADDR;          // 0x0C
    volatile uint32_t DATA_CMD;          // 0x10
    volatile uint32_t SS_SCL_HCNT;       // 0x14
    volatile uint32_t SS_SCL_LCNT;       // 0x18
    volatile uint32_t FS_SCL_HCNT;       // 0x1C
    volatile uint32_t FS_SCL_LCNT;       // 0x20
    volatile uint32_t HS_SCL_HCNT;       // 0x24
    volatile uint32_t HS_SCL_LCNT;       // 0x28
    volatile uint32_t INTR_STAT;         // 0x2C
    volatile uint32_t INTR_MASK;         // 0x30
    volatile uint32_t RAW_INTR_MASK;     // 0x34
    volatile uint32_t RX_TL;             // 0x38
    volatile uint32_t TX_TL;             // 0x3C
    volatile uint32_t CIR_INTR;          // 0x40
    volatile uint32_t CLR_RX_UNDER;      // 0x44
    volatile uint32_t CLR_RX_OVER;       // 0x48
    volatile uint32_t CLR_TX_OVER;       // 0x4C
    volatile uint32_t CLR_RD_REQ;        // 0x50
    volatile uint32_t CLR_TX_ABRT;       // 0x54
    volatile uint32_t CLR_RX_DONE;       // 0x58
    volatile uint32_t CLR_ACTIVITY;      // 0x5C
    volatile uint32_t CLR_STOP_DET;      // 0x60
    volatile uint32_t CLR_START_DET;     // 0x64
    volatile uint32_t CLR_GEN_CALL;      // 0x68
    volatile uint32_t ENABLE;            // 0x6C
    volatile uint32_t STATUS;            // 0x70
    volatile uint32_t TXFLR;             // 0x74
    volatile uint32_t RXFLR;             // 0x78
    volatile uint32_t SDA_HOLD;          // 0x7C
    volatile uint32_t TX_ABRT_SOURCE;    // 0x80
    volatile uint32_t SLV_DATA_NACK_ONLY;// 0x84
    volatile uint32_t DMA_CR;            // 0x88
    volatile uint32_t DMA_TDLR;          // 0x8C
    volatile uint32_t DMA_RDLR;          // 0x90
    volatile uint32_t SDA_SETUP;         // 0x94
    volatile uint32_t ACK_GENERAL_CALL;  // 0x98
    volatile uint32_t ENABLE_STATUS;     // 0x9C
    volatile uint32_t FS_SPKLEN;         // 0xA0
    volatile uint32_t HS_SPKLEN;         // 0xA4
} S_I2C_Reg_t;

typedef uint32_t (*T_Hal_I2c_Eanble)(uint8_t u8Enable);
typedef uint32_t (*T_Hal_I2c_WaitForMasterCompleted)(void);


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
extern T_Hal_I2c_WaitForMasterCompleted _Hal_I2c_WaitForMasterCompleted;

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
*  _Hal_I2c_Eanble
* 
* DESCRIPTION:
*   1. Enable/Disable I2C
* 
* CALLS
* 
* PARAMETERS
*   1. u8Enable: 1 for enable/0 for disable
* 
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t _Hal_I2c_Eanble_patch(uint8_t u8Enable)
{
    uint32_t u32Count = 0;
    uint32_t u32DisableTimeout = 10*( SystemCoreClockGet()/I2C_CLK_SUPPORT_MAX );
    
    if(u8Enable)
    {
        // Enable
        I2C->ENABLE |= I2C_ENABLE_EN;
    }else{
        // Disable, ref to << 3.8.3 Disabling DW_apb_i2c >>
        I2C->ENABLE &= ~I2C_ENABLE_EN;
        
        while( I2C->ENABLE_STATUS & I2C_ENABLE_STATUS_EN )
        {
            if(u32Count++ > u32DisableTimeout)
                return 1;            
        }
    }
    return 0;
}
uint32_t Hal_I2c_MasterInit_patch(E_I2cAddrMode_t eAddrMode, E_I2cSpeed_t eSpeed)
{
    Hal_Sys_ApsClkEn(1, APS_CLK_I2C);
    Hal_Sys_ApsModuleRst(ASP_RST_I2C);
    _Hal_I2c_Eanble( 0 );
    I2C->CON = I2C_CON_MASTER_MODE | I2C_CON_SLAVE_DISABLE | I2C_CON_RESTART_EN;
    if(eAddrMode == I2C_10BIT)
    {
        I2C->CON |= I2C_CON_MASTER_10BIT;
    }else{
        I2C->CON |= I2C_CON_MASTER_07BIT;
    }
    I2C->RX_TL = 0;
    I2C->TX_TL = 0;
    Hal_I2c_SpeedSet( eSpeed );
    I2C->INTR_MASK = I2C_INT_TX_ABRT;
    _Hal_I2c_Eanble( 1 );
    Hal_Vic_IntClear(I2C_IRQn);
    Hal_Vic_IntMask(I2C_IRQn, 0);
    Hal_Vic_IntEn(I2C_IRQn, 1);
    NVIC_ClearPendingIRQ(I2C_IRQn);
    NVIC_SetPriority(I2C_IRQn, IRQ_PRIORITY_I2C);
    NVIC_EnableIRQ(I2C_IRQn);
    return 0;
}
uint32_t Hal_I2c_MasterReceive_patch(uint8_t *pu8Data, uint32_t u32Length, uint8_t u8NeedStopBit)
{
    uint32_t u32Count = 0;
    uint32_t u32Temp = 0;
    uint32_t u32Idx = 0;
    uint32_t u32WrIdx = 0;
    if( _Hal_I2c_WaitForMasterCompleted() )
        return 1;
    while ((u32WrIdx < I2C_FIFO_SIZE) && (u32WrIdx < u32Length))
    {
        u32Temp = I2C_DATA_CMD_READ;
        if(u32WrIdx == u32Length-1)
        {
            if(u8NeedStopBit)
                u32Temp |= I2C_DATA_STOP_BIT;
        }
        u32Count = 0;
        while( !(I2C->STATUS & I2C_STATUS_TX_NOT_FULL) )
        {
            if(u32Count > I2C_TIMEOUT_COUNT_MAX)
                return 1;
            u32Count++;
        }
        I2C->DATA_CMD = u32Temp;
        u32WrIdx++;
    }
    while (u32WrIdx < u32Length)
    {
        u32Count = 0;
        while( !(I2C->STATUS & I2C_STATUS_RX_NOT_EMPTY) )
        {
            if(u32Count > I2C_TIMEOUT_COUNT_MAX)
                return 1;
            u32Count++;
        }
        pu8Data[u32Idx] = (uint8_t)(I2C->DATA_CMD & I2C_DATA_CMD_DATA_MASK);
        u32Idx++;
        u32Temp = I2C_DATA_CMD_READ;
        if(u32WrIdx == u32Length-1)
        {
            if(u8NeedStopBit)
                u32Temp |= I2C_DATA_STOP_BIT;
        }
        u32Count = 0;
        while( !(I2C->STATUS & I2C_STATUS_TX_NOT_FULL) )
        {
            if(u32Count > I2C_TIMEOUT_COUNT_MAX)
                return 1;
            u32Count++;
        }
        I2C->DATA_CMD = u32Temp;
        u32WrIdx++;
    }
    while (u32Idx < u32Length)
    {
        u32Count = 0;
        while( !(I2C->STATUS & I2C_STATUS_RX_NOT_EMPTY) )
        {
            if(u32Count > I2C_TIMEOUT_COUNT_MAX)
                return 1;
            u32Count++;
        }
        pu8Data[u32Idx] = (uint8_t)(I2C->DATA_CMD & I2C_DATA_CMD_DATA_MASK);
        u32Idx++;
    }
    return 0;
}
