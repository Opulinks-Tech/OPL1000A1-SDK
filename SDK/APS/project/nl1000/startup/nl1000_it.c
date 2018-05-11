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

#include "nl1000.h"
#include "cmsis_os.h"
#include "nl1000_it.h"

#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "hal_vic.h"
#include "hal_system.h"

#include "ipc.h"
#include "diag_task.h"

#ifdef ENHANCE_IPC
#else
extern uint8_t g_GET_DATA;
extern uint8_t g_INIT_DATA_FLOW;
#endif

uint8_t IS_M0_READY = 1;

typedef void (*T_InterruptHandler)(void);

RET_DATA T_InterruptHandler IPC0_IRQHandler_Patch;
RET_DATA T_InterruptHandler IPC1_IRQHandler_Patch;
RET_DATA T_InterruptHandler IPC2_IRQHandler_Patch;
RET_DATA T_InterruptHandler IPC3_IRQHandler_Patch;
RET_DATA T_InterruptHandler GPIO_IRQHandler_Patch;
RET_DATA T_InterruptHandler SCRT_IRQHandler_Patch;
RET_DATA T_InterruptHandler MSQ_IRQHandler_Patch;
RET_DATA T_InterruptHandler UARTDBG_IRQHandler_Patch;
RET_DATA T_InterruptHandler UART0_IRQHandler_Patch;
RET_DATA T_InterruptHandler UART1_IRQHandler_Patch;
RET_DATA T_InterruptHandler I2C_IRQHandler_Patch;
RET_DATA T_InterruptHandler SPI0_IRQHandler_Patch;
RET_DATA T_InterruptHandler SPI1_IRQHandler_Patch;
RET_DATA T_InterruptHandler SPI2_IRQHandler_Patch;
RET_DATA T_InterruptHandler TIM0_IRQHandler_Patch;
RET_DATA T_InterruptHandler TIM1_IRQHandler_Patch;
RET_DATA T_InterruptHandler WDT_IRQHandler_Patch;
RET_DATA T_InterruptHandler JTAG_IRQHandler_Patch;
RET_DATA T_InterruptHandler DMA_IRQHandler_Patch;

uint8_t is_M0_ready(void){
	return IS_M0_READY;
}

// --------------------------------- ISR Patch---------------------------------
void IPC0_IRQHandler_Patch_impl(void)
{
    // Clear interrupt
    Hal_Vic_IpcIntClear(IPC_IDX_0);
	
    //IPC0 bit0
    #ifdef ENHANCE_IPC
        #ifdef IPC_SUT
            ipc_peer_ready(1, 1);
        #endif
    #else
        g_INIT_DATA_FLOW = 1;
        data_flow_task();
    #endif
}

void IPC1_IRQHandler_Patch_impl(void)
{
    // Clear interrupt
    Hal_Vic_IpcIntClear(IPC_IDX_1);
	
    //IPC1 bit1
    #ifdef ENHANCE_IPC
        #ifdef IPC_SUT
            ipc_peer_ready(1, 0);
        #endif
    #else
        g_GET_DATA = 1;
        data_flow_task();	
    #endif
}

void IPC2_IRQHandler_Patch_impl(void)
{
    // Clear interrupt
    Hal_Vic_IpcIntClear(IPC_IDX_2);

    //IPC1 bit2
    #ifdef ENHANCE_IPC
        ipc_proc();
    #endif
}

void IPC3_IRQHandler_Patch_impl(void)
{
    // Clear interrupt
    Hal_Vic_IpcIntClear(IPC_IDX_3);
	
    //IPC1 bit3
    #ifdef IPC_SUT
        ipc_sut_proc();
    #endif
}

void GPIO_IRQHandler_Patch_impl(void)
{
    E_GpioIdx_t eGpioIdx = GPIO_IDX_00;
    uint32_t u32Status = 0;

    // Get status
    u32Status = Hal_Vic_GpioIntStatRead();

    for(eGpioIdx = GPIO_IDX_00; eGpioIdx<GPIO_IDX_MAX; eGpioIdx++)
    {
        if( u32Status & (1<<eGpioIdx) )
        {
            // Clear module interrupt
            Hal_Vic_GpioIntClear(eGpioIdx);

            // Resume Mmfactor
            if( Hal_Vic_MmFactorResumeByGpioIntStatRead() | (1<<eGpioIdx) )
            {
                // Need this function to apply system clock change
                Hal_Sys_ApsClkMmFactorSet(16);
            }

            // Hook here...
        }
    }

    // Clear VIC interrupt
    Hal_Vic_IntClear(GPIO_IRQn);
}

void SCRT_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void MSQ_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void UARTDBG_IRQHandler_Patch_impl(void)
{
    // Note: Level-sensitive interrupt
    uint32_t u32Temp = 0;
    uint32_t u32Status = 0; 

    // Get status
    u32Status = Hal_DbgUart_IntStatusRead();

    // Clear all module interrupts
    Hal_DbgUart_IntClear(u32Status);

    // Clear VIC interrupt
    Hal_Vic_IntClear(UARTDBG_IRQn);

    // Check status and handle interrupt
    if(u32Status & DBG_UART_INT_RX)
    {
        // ---- Rx interrupt case ----
        // Get data
        if(Hal_DbgUart_DataRecvTimeOut(&u32Temp,0) == 0)
        {
            // if get data, execute hook function
            if( DbgUart_RxCallBack != 0 )
                DbgUart_RxCallBack(u32Temp);
        }
    }
}

void UART0_IRQHandler_Patch_impl(void)
{
    // Note: Level-sensitive interrupt
    uint32_t u32Temp = 0;
    uint32_t u32InterruptId = 0;

    // Get status
    u32InterruptId = Hal_Uart_IntIdRead(UART_IDX_0);

    // Clear module interrupt 
    Hal_Uart_IntClear(UART_IDX_0, (E_UartIntId_t)u32InterruptId, &u32Temp);

    // Clear VIC interrupt
    Hal_Vic_IntClear(UART0_IRQn);

    if(u32InterruptId == UART_INT_RX_AVALIBLE)
    {
        // ---- Rx interrupt case ----
        // Hook here ...
        if( Uart_0_RxCallBack != 0 )
            Uart_0_RxCallBack(u32Temp);
    }
}

void UART1_IRQHandler_Patch_impl(void)
{
    // Note: Level-sensitive interrupt
    uint32_t u32Temp = 0;
    uint32_t u32InterruptId = 0;

    // Get status
    u32InterruptId = Hal_Uart_IntIdRead(UART_IDX_1);

    // Clear module interrupt 
    Hal_Uart_IntClear(UART_IDX_1, (E_UartIntId_t)u32InterruptId, &u32Temp);

    // Clear VIC interrupt
    Hal_Vic_IntClear(UART1_IRQn);

    if(u32InterruptId == UART_INT_RX_AVALIBLE)
    {
        // ---- Rx interrupt case ----
        // Hook here ...
        if( Uart_1_RxCallBack != 0 )
            Uart_1_RxCallBack(u32Temp);
    }
}

void I2C_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void SPI0_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void SPI1_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void SPI2_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void TIM0_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void TIM1_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void WDT_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void JTAG_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

void DMA_IRQHandler_Patch_impl(void)
{
    __DSB();
    __ISB();
}

// --------------------------------- ISR ---------------------------------
void IPC0_IRQHandler(void)
{
    IPC0_IRQHandler_Patch();
}

void IPC1_IRQHandler(void)
{
    IPC1_IRQHandler_Patch();
}

void IPC2_IRQHandler(void)
{
    IPC2_IRQHandler_Patch();
}

void IPC3_IRQHandler(void)
{
    IPC3_IRQHandler_Patch();
}

void GPIO_IRQHandler(void)
{
    GPIO_IRQHandler_Patch();
}  

void SCRT_IRQHandler(void)
{
    SCRT_IRQHandler_Patch();
}  

void MSQ_IRQHandler(void)
{
    MSQ_IRQHandler_Patch();
}

void UARTDBG_IRQHandler(void)
{
    UARTDBG_IRQHandler_Patch();
}

void UART0_IRQHandler(void)
{
    UART0_IRQHandler_Patch();
}

void UART1_IRQHandler(void)
{
    UART1_IRQHandler_Patch();
}

void I2C_IRQHandler(void)
{
    I2C_IRQHandler_Patch();
}

void SPI0_IRQHandler(void)
{
    SPI0_IRQHandler_Patch();
}

void SPI1_IRQHandler(void)
{
    SPI1_IRQHandler_Patch();
}

void SPI2_IRQHandler(void)
{
    SPI2_IRQHandler_Patch();
}

void TIM0_IRQHandler(void)
{
    TIM0_IRQHandler_Patch();
}

void TIM1_IRQHandler(void)
{
    TIM1_IRQHandler_Patch();
}

void WDT_IRQHandler(void)
{
    WDT_IRQHandler_Patch();
}

void JTAG_IRQHandler(void)
{
    JTAG_IRQHandler_Patch();
}

void DMA_IRQHandler(void)
{
    DMA_IRQHandler_Patch();
}

void ISR_Pre_Init(void)
{
    IPC0_IRQHandler_Patch    = IPC0_IRQHandler_Patch_impl;
    IPC1_IRQHandler_Patch    = IPC1_IRQHandler_Patch_impl;
    IPC2_IRQHandler_Patch    = IPC2_IRQHandler_Patch_impl;
    IPC3_IRQHandler_Patch    = IPC3_IRQHandler_Patch_impl;
    GPIO_IRQHandler_Patch    = GPIO_IRQHandler_Patch_impl;
    SCRT_IRQHandler_Patch    = SCRT_IRQHandler_Patch_impl;
    MSQ_IRQHandler_Patch     = MSQ_IRQHandler_Patch_impl;
    UARTDBG_IRQHandler_Patch = UARTDBG_IRQHandler_Patch_impl;
    UART0_IRQHandler_Patch   = UART0_IRQHandler_Patch_impl;
    UART1_IRQHandler_Patch   = UART1_IRQHandler_Patch_impl;
    I2C_IRQHandler_Patch     = I2C_IRQHandler_Patch_impl;
    SPI0_IRQHandler_Patch    = SPI0_IRQHandler_Patch_impl;
    SPI1_IRQHandler_Patch    = SPI1_IRQHandler_Patch_impl;
    SPI2_IRQHandler_Patch    = SPI2_IRQHandler_Patch_impl;
    TIM0_IRQHandler_Patch    = TIM0_IRQHandler_Patch_impl;
    TIM1_IRQHandler_Patch    = TIM1_IRQHandler_Patch_impl;
    WDT_IRQHandler_Patch     = WDT_IRQHandler_Patch_impl;
    JTAG_IRQHandler_Patch    = JTAG_IRQHandler_Patch_impl;
    DMA_IRQHandler_Patch     = DMA_IRQHandler_Patch_impl;
}

