/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/

/******************************************************************************
*  Filename:
*  ---------
*  hal_i2c_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This source file defines the functions of I2C .
*  Ref. document is << DesignWare DW_apb_i2c Databook >>
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
#include "nl1000.h"
#include "hal_system.h"
#include "hal_vic.h"
#include "hal_i2c.h"
#include "hal_i2c_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define I2C       ((S_Uart_Reg_t *) I2C_BASE)

#define SYSYEM_TICK_PER_MS       ( SystemCoreClockGet()/1000000 )
#define MS_TO_NS                 1000
#define I2C_CLK_STD              100000
#define I2C_CLK_FAST             400000
#define I2C_CLK_HIGH             3400000
#define I2C_CLK_SUPPORT_MAX      I2C_CLK_FAST
#define I2C_DISABLE_TIMEOUT_TICK 10*( SystemCoreClockGet()/I2C_CLK_SUPPORT_MAX )
#define I2C_TIMEOUT_COUNT_MAX    (0x30000)
// According to Section 3.9 page 55
#define I2C_FS_SPIKE_MAX         50   /* UNIT: ns */
#define I2C_HS_SPIKE_MAX         10   /* UNIT: ns */
// According to Section 3.10.2.3 
#define I2C_STD_SCL_LOW_MIN      4700 /* UNIT: ns */
#define I2C_STD_SCL_HIGH_MIN     4000 /* UNIT: ns */
#define I2C_FS_SCL_LOW_MIN       1300 /* UNIT: ns */
#define I2C_FS_SCL_HIGH_MIN      600  /* UNIT: ns */
#define I2C_HS_SCL_LOW_MIN       120  /* UNIT: ns */
#define I2C_HS_SCL_HIGH_MIN      60   /* UNIT: ns */

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

#define I2C_TAR_TARGET_ADDR_MASK (0x3FF)

#define I2C_SAR_SLAVE_ADDR_MASK  (0x3FF)

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

#define I2C_INT_TX_ABRT          (1<<6)
#define I2C_INT_RD_REQ           (1<<5)
#define I2C_INT_RX_FULL          (1<<2)

#define I2C_ENABLE_EN            1

#define I2C_ENABLE_STATUS_EN     1

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
} S_Uart_Reg_t;

typedef uint32_t (*T_Hal_I2c_Eanble)(uint8_t u8Enable);
typedef uint32_t (*T_Hal_I2c_WaitForMasterCompleted)(void);


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
/* Internal */
extern T_Hal_I2c_Eanble                 _Hal_I2c_Eanble;
extern T_Hal_I2c_WaitForMasterCompleted _Hal_I2c_WaitForMasterCompleted;

/* Master mode relative */

/* Slave mode relative */
T_Hal_I2c_SlaveIntTxCallBack g_tHalI2cIntTxCallback;
T_Hal_I2c_SlaveIntRxCallBack g_tHalI2cIntRxCallback;

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
*   Hal_I2c_IntHandler
*
* DESCRIPTION:
*   the handler of I2C
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_I2c_IntHandler(void)
{
    uint32_t ulIrqClr;
    uint8_t u8Data;

    // slave write
	if (I2C->INTR_STAT & I2C_INT_RD_REQ)
    {
        if (g_tHalI2cIntTxCallback != 0)
            g_tHalI2cIntTxCallback();
        
        // clear IRQ
        ulIrqClr = I2C->CLR_RD_REQ;
    }
    
    // slave read
    if (I2C->INTR_STAT & I2C_INT_RX_FULL)
    {
        Hal_I2c_SlaveReceive(&u8Data, 1);
        if (g_tHalI2cIntRxCallback != 0)
            g_tHalI2cIntRxCallback(u8Data);
        
        // automatically clear IRQ
    }
    
    // tx abort
	if (I2C->INTR_STAT & I2C_INT_TX_ABRT)
    {
        // clear IRQ
        ulIrqClr = I2C->CLR_TX_ABRT;
    }
    
    (void)ulIrqClr;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_MasterInit
* 
* DESCRIPTION:
*   1. Initial I2c as master mode
* 
* CALLS
* 
* PARAMETERS
*   1. eAddrMode: 7/10bit Address mode. refer to E_I2cAddrMode_t
*   2. eSpeed   : Standard(100K)/Fast(400K)/High speed(3.4M) mode. refer to E_I2cSpeed_t
* 
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_I2c_MasterInit_patch(E_I2cAddrMode_t eAddrMode, E_I2cSpeed_t eSpeed)
{
    // Enable clock of module
    Hal_Sys_ApsClkEn(1, APS_CLK_I2C);
    // Reset module
    Hal_Sys_ApsModuleRst(ASP_RST_I2C);
    
    // ---> Disable first, to set registers 
    _Hal_I2c_Eanble( 0 );
    
    // Master Mode, and restart enable
    I2C->CON = I2C_CON_MASTER_MODE | I2C_CON_SLAVE_DISABLE | I2C_CON_RESTART_EN;
    
    // Address Bits
    if(eAddrMode == I2C_10BIT)
    {
        // 10-Bit Master
        I2C->CON = I2C_CON_MASTER_10BIT;
    }else{
        // 7-Bit Master
        I2C->CON = I2C_CON_MASTER_07BIT;
    }
    
    // Set FIFO
    I2C->RX_TL = 0;
    I2C->TX_TL = 0;
    
    // Speed
    Hal_I2c_SpeedSet( eSpeed );

    // Mask all interrupt, handle Tx abort
    I2C->INTR_MASK = I2C_INT_TX_ABRT;
    
    // <--- Enable modules
    _Hal_I2c_Eanble( 1 );
    
    // VIC 1) Clear interrupt
    Hal_Vic_IntClear(I2C_IRQn);
    // VIC 2) un-Mask VIC
    Hal_Vic_IntMask(I2C_IRQn, 0);
    // VIC 3) Enable VIC
    Hal_Vic_IntEn(I2C_IRQn, 1);

    // NVIC 1) Clean NVIC
    NVIC_ClearPendingIRQ(I2C_IRQn);
    // NVIC 2) Set prority
    NVIC_SetPriority(I2C_IRQn, IRQ_PRIORITY_I2C);
    // NVIC 3) Enable NVIC
    NVIC_EnableIRQ(I2C_IRQn);
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_SlaveInit
* 
* DESCRIPTION:
*   1. Initial I2c as salve mode
* 
* CALLS
* 
* PARAMETERS
*   1. eAddrMode      : 7/10bit Address mode. refer to E_I2cAddrMode_t
*   2. u16SlaveAddr   : Slave address
* 
* RETURNS
*   0: setting complete
*   1: error 
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_I2c_SlaveInit_patch(E_I2cAddrMode_t eAddrMode, uint16_t u16SlaveAddr)
{
    // Enable clock of module
    Hal_Sys_ApsClkEn(1, APS_CLK_I2C);
    // Reset module
    Hal_Sys_ApsModuleRst(ASP_RST_I2C);
    
    // ---> Disable first, to set registers 
    _Hal_I2c_Eanble( 0 );
    
    // Slave Mode, and restart enable
    I2C->CON = I2C_CON_RESTART_EN;
    
    // Address Bits
    if(eAddrMode == I2C_10BIT)
    {
        // 10-Bit Master
        I2C->CON |= I2C_CON_SLAVE_10BIT;
    }
    
    // Set FIFO
    I2C->RX_TL = 0;
    I2C->TX_TL = 0;
    
    // Speed, recommend set to max speed available 
    Hal_I2c_SpeedSet( I2C_SPEED_FAST );

    // Mask all interrupt, handle Tx abort
    I2C->INTR_MASK = I2C_INT_TX_ABRT;
    
    // Slave address
    I2C->SAR = u16SlaveAddr & I2C_SAR_SLAVE_ADDR_MASK;
    
    // <--- Enable modules
    _Hal_I2c_Eanble( 1 );
    
    // VIC 1) Clear interrupt
    Hal_Vic_IntClear(I2C_IRQn);
    // VIC 2) un-Mask VIC
    Hal_Vic_IntMask(I2C_IRQn, 0);
    // VIC 3) Enable VIC
    Hal_Vic_IntEn(I2C_IRQn, 1);

    // NVIC 1) Clean NVIC
    NVIC_ClearPendingIRQ(I2C_IRQn);
    // NVIC 2) Set prority
    NVIC_SetPriority(I2C_IRQn, IRQ_PRIORITY_I2C);
    // NVIC 3) Enable NVIC
    NVIC_EnableIRQ(I2C_IRQn);
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_SlaveIntTxEn
* 
* DESCRIPTION:
*   1. enable / disable the slave Tx interrupt
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
uint32_t Hal_I2c_SlaveIntTxEn(uint8_t u8Enable)
{
    if(u8Enable)
    {
        I2C->INTR_MASK |= I2C_INT_RD_REQ;
    }
    else
    {
        I2C->INTR_MASK &= ~I2C_INT_RD_REQ;
    }
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_SlaveIntRxEn
* 
* DESCRIPTION:
*   1. enable / disable the slave Rx interrupt
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
uint32_t Hal_I2c_SlaveIntRxEn(uint8_t u8Enable)
{
    if(u8Enable)
    {
        I2C->INTR_MASK |= I2C_INT_RX_FULL;
    }
    else
    {
        I2C->INTR_MASK &= ~I2C_INT_RX_FULL;
    }
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_SlaveIntTxCallBackFuncSet
* 
* DESCRIPTION:
*   1. set the slave Tx callback fucntion
* 
* CALLS
* 
* PARAMETERS
*   1. tFunc: the callback function for slave Tx
* 
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_I2c_SlaveIntTxCallBackFuncSet(T_Hal_I2c_SlaveIntTxCallBack tFunc)
{
    g_tHalI2cIntTxCallback = tFunc;
}

/*************************************************************************
* FUNCTION:
*  Hal_I2c_SlaveIntRxCallBackFuncSet
* 
* DESCRIPTION:
*   1. set the slave Rx callback fucntion
* 
* CALLS
* 
* PARAMETERS
*   1. tFunc: the callback function for slave Rx
* 
* RETURNS
* 
* GLOBALS AFFECTED
* 
*************************************************************************/
void Hal_I2c_SlaveIntRxCallBackFuncSet(T_Hal_I2c_SlaveIntRxCallBack tFunc)
{
    g_tHalI2cIntRxCallback = tFunc;
}
