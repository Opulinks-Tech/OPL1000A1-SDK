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

/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nl1000.h"        /* For Irq number */
#include "msg.h"
#include "ipc.h"
#include "boot_sequence.h"
#include "hal_vic.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "hal_system.h"
#include "hal_patch.h"
#include "hal_spi.h"
#include "hal_flash.h"
#include "hal_pwm.h"
#include "diag_task.h"
#include "le_ctrl.h"
#include "at_cmd.h"
#include "at_cmd_common.h"

#if defined(__AT_CMD_TASK__)
#include "at_cmd_task.h"
#endif



#define __DIAG_TASK__
//#define __WIFI_MAC_TASK__
//#define __HW_CRYPTO_ENGINE__
//#define __WPA_SUPPLICANT__
#define __CONTROLLER_TASK__
//#define __LWIP_TASK__
//#define __BLE__
#define __VIC_IPC__

//#undef __WIFI_MAC_TASK__
//#undef __WPA_SUPPLICANT__
//#undef __HW_CRYPTO_ENGINE__
//#undef __LWIP_TASK__
//#undef __BLE__

#if defined(__WIFI_MAC_TASK__)
#include "wifi_mac_task.h"
#endif

#if defined(__HW_CRYPTO_ENGINE__)
#include "scrt.h"
#endif

#if defined(__WPA_SUPPLICANT__)
#include "wpa_supplicant_i.h"
#include "supplicant_task.h"
#endif

#if defined(__CONTROLLER_TASK__)
#include "controller_task.h"
#endif

#define __SVN_REVISION__
#if defined(__SVN_REVISION__)
#include "svn_rev.h"
#endif

#if defined(__BLE__)
#include "le_rtos_task.h"

extern void LeRtosTask(void *pvParameters);
#endif

void Main_DriverInit_impl(void);
void Main_PowerSetup_impl(void);
void Main_ClockSetup_impl(void);
void Main_MiscModulesInit_impl(void);
void Main_MiscDriverConfigSetup_impl(void);
void Main_ServiceInit_impl(void);
void Main_AppInit_impl(void);
void Main_PostInit_impl(void);
void Main_IdleHook_impl(void);

typedef void (*T_Main_DriverInit_fp)(void);
typedef void (*T_Main_PowerSetup_fp)(void);
typedef void (*T_Main_ClockSetup_fp)(void);
typedef void (*T_Main_MiscModulesInit_fp)(void);
typedef void (*T_Main_MiscDriverConfigSetup_fp)(void);
typedef void (*T_Main_ServiceInit_fp)(void);
typedef void (*T_Main_AppInit_fp)(void);
typedef void (*T_Main_PostInit_fp)(void);
typedef void (*T_Main_IdleHook_fp)(void);

RET_DATA T_Main_DriverInit_fp Main_DriverInit;
RET_DATA T_Main_PowerSetup_fp Main_PowerSetup;
RET_DATA T_Main_ClockSetup_fp Main_ClockSetup;
RET_DATA T_Main_MiscModulesInit_fp Main_MiscModulesInit;
RET_DATA T_Main_MiscDriverConfigSetup_fp Main_MiscDriverConfigSetup;
RET_DATA T_Main_ServiceInit_fp Main_ServiceInit;
RET_DATA T_Main_AppInit_fp Main_AppInit;
RET_DATA T_Main_PostInit_fp Main_PostInit;
RET_DATA T_Main_IdleHook_fp Main_IdleHook;

void Main_Fun_Init(void)
{
    Main_DriverInit = Main_DriverInit_impl;
    Main_PowerSetup = Main_PowerSetup_impl;
    Main_ClockSetup = Main_ClockSetup_impl;
    Main_MiscModulesInit = Main_MiscModulesInit_impl;
    Main_MiscDriverConfigSetup = Main_MiscDriverConfigSetup_impl;
    Main_ServiceInit = Main_ServiceInit_impl;
    Main_AppInit = Main_AppInit_impl;
    Main_PostInit = Main_PostInit_impl;
    Main_IdleHook = Main_IdleHook_impl;
}

extern void lwip_task_create(void);
//extern void diag_task_create(void);

void Main_PowerSetup_impl(void)
{
    __DSB();
    __ISB();
}

void Main_ClockSetup_impl(void)
{
    __DSB();
    __ISB();
}

void Main_MiscModulesInit_impl(void)
{
    __DSB();
    __ISB();
}

void Main_MiscDriverConfigSetup_impl(void)
{
    __DSB();
    __ISB();
}

void Main_DriverInit_impl(void)
{
    // Set power
    Main_PowerSetup();

    // Set system clock
    Main_ClockSetup();

    // Set pin-mux
    Hal_SysPinMuxAppInit();

    // Init VIC
    Hal_Vic_Init();

	// Init IPC
    Hal_Vic_IpcIntEn(IPC_IDX_0, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_1, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_2, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_3, 1);

    // Init DBG_UART
    Hal_DbgUart_Init(115200);

    // Init UART0
    Hal_Uart_Init(UART_IDX_0, 115200, DATA_BIT_8, PARITY_NONE, STOP_BIT_1, 0);

    Hal_Uart_Init(UART_IDX_1, 115200, DATA_BIT_8, PARITY_NONE, STOP_BIT_1, 0);

    // Init SPI 0/1/2
    Hal_Spi_Init(SPI_IDX_0, SystemCoreClockGet()/2,
        SPI_CLK_PLOAR_HIGH_ACT, SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);
    Hal_Spi_Init(SPI_IDX_1, SystemCoreClockGet()/2,
        SPI_CLK_PLOAR_HIGH_ACT, SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);
    Hal_Spi_Init(SPI_IDX_2, SystemCoreClockGet()/2,
        SPI_CLK_PLOAR_HIGH_ACT, SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);

    // Init flash on SPI 0/1/2
    Hal_Flash_Init(SPI_IDX_0);
    Hal_Flash_Init(SPI_IDX_1);
    Hal_Flash_Init(SPI_IDX_2);

    // Init PWM
    Hal_Pwm_Init();

    // Other modules' init
    Main_MiscModulesInit();

    //-------------------------------------------------------------------------------------
    // Other driver config need by Task-level (sleep strategy)

    // Diag task
    Hal_DbgUart_RxCallBackFuncSet(uartdbg_rx_int_handler);
    Hal_DbgUart_RxIntEn(1);

    // HCI and AT command
    uart1_mode_set_default();

	// Init SEQ module
	Hal_Sys_SleepInit();

    // Other tasks' driver config
    Main_MiscDriverConfigSetup();
}

void Main_ServiceInit_impl(void)
{
#if defined(__SVN_REVISION__)
    printf("\n");
    printf("[SVN REV] SVN_REVISION:%d \n", SVN_REVISION);
//    printf("[SVN REV] SVN_LOCAL_MODIFICATIONS:%d \n", SVN_LOCAL_MODIFICATIONS);
//    printf("[SVN REV] SVN_DATE:%s \n", SVN_DATE);
//    printf("[SVN REV] SVN_TIME_NOW:%s \n", SVN_TIME_NOW);
//    printf("[SVN REV] SVN_URL:%s \n", SVN_URL);
#endif

#if defined(__DIAG_TASK__)
    diag_task_create();
#endif

#if defined(__AT_CMD_TASK__)
    at_task_create();
#endif

#if defined(__WIFI_MAC_TASK__)
    wifi_mac_task_create();
#endif

#if defined(__LWIP_TASK__)
    lwip_task_create();
#endif

#if defined(__HW_CRYPTO_ENGINE__)
    nl_scrt_Init();
#endif

#if defined(__WPA_SUPPLICANT__)
    do_supplicant_init();
#endif

#if defined(__CONTROLLER_TASK__)
	controller_task_create();
#endif

#ifdef ENHANCE_IPC
    ipc_init();
#else
    //TODO: we should negotiate the share memory address in the future.
	//trigger APS2MSQ IPC0
	printf("APS2MSQ\r\n");
	reg_write(REG_VIC_IPC_INT,0x1);
#endif

#if defined(__BLE__)
    LeRtosTaskCreat();
#endif
}

void Main_AppInit_impl(void)
{
    __DSB();
    __ISB();
}

void Main_PostInit_impl(void)
{
    /* Patch Function */
#ifdef TRACER
    tracer_init();
#else
    msg_init();
#endif
}

void Main_IdleHook_impl(void)
{
    __DSB();
    __ISB();
}

/*
 * main: initialize and start the system
 */
int main (void)
{
    Boot_Sequence();

    Main_DriverInit();

    osKernelInitialize();

    Main_ServiceInit();

    // for Sw patch test, need to remove
    Test_ForSwPatch();

    Main_AppInit();

    Main_PostInit();
#if defined  (__RTL_SIMULATION__)
    reg_write(DEBUG_ADDR, 40);
#endif    
    osKernelStart();

    while(1);
}

//ToDo: move to another file: ex: panic.c
#if( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    msg_print(LOG_HIGH_LEVEL, "stack overflow: %x %s\r\n",(unsigned int)xTask,(portCHAR *)pcTaskName);
    //configASSERT(0);
    for (;; ) {}
}
#endif

#if( configUSE_IDLE_HOOK > 0)
void vApplicationIdleHook( void )
{
    // Idle task hook...
    Main_IdleHook();
}
#endif

