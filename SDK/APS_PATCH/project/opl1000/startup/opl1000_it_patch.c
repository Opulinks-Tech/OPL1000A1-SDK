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

#include "opl1000.h"
#include "cmsis_os.h"
#include "opl1000_it.h"

#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "hal_vic.h"
#include "hal_system.h"
#include "hal_tmr.h"
#include "hal_i2c.h"
#include "hal_wdt.h"

#include "ipc.h"
#include "diag_task.h"

#ifdef ENHANCE_IPC
#else
extern uint8_t g_GET_DATA;
extern uint8_t g_INIT_DATA_FLOW;
#endif

typedef void (*T_InterruptHandler)(void);

extern T_InterruptHandler IPC0_IRQHandler_Entry;
extern T_InterruptHandler IPC1_IRQHandler_Entry;
extern T_InterruptHandler IPC2_IRQHandler_Entry;
extern T_InterruptHandler IPC3_IRQHandler_Entry;
extern T_InterruptHandler GPIO_IRQHandler_Entry;
extern T_InterruptHandler SCRT_IRQHandler_Entry;
extern T_InterruptHandler MSQ_IRQHandler_Entry;
extern T_InterruptHandler UARTDBG_IRQHandler_Entry;
extern T_InterruptHandler UART0_IRQHandler_Entry;
extern T_InterruptHandler UART1_IRQHandler_Entry;
extern T_InterruptHandler I2C_IRQHandler_Entry;
extern T_InterruptHandler SPI0_IRQHandler_Entry;
extern T_InterruptHandler SPI1_IRQHandler_Entry;
extern T_InterruptHandler SPI2_IRQHandler_Entry;
extern T_InterruptHandler TIM0_IRQHandler_Entry;
extern T_InterruptHandler TIM1_IRQHandler_Entry;
extern T_InterruptHandler WDT_IRQHandler_Entry;
extern T_InterruptHandler JTAG_IRQHandler_Entry;
extern T_InterruptHandler DMA_IRQHandler_Entry;

void WDT_IRQHandler_Entry_patch(void)
{
    printf("Watchdog expired!!\r\n");
    // VIC 1) Clear interrupt
    Hal_Vic_IntClear(WDT_IRQn);
}
void ISR_Pre_Init_patch(void)
{
    WDT_IRQHandler_Entry     = WDT_IRQHandler_Entry_patch;
}

