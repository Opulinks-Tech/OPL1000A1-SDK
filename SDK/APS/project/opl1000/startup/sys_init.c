/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
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
*  sys_init.c
*
*  Project:
*  --------
*  OPL1000 Project - the system initialize implement file
*
*  Description:
*  ------------
*  This implement file is include the system initialize function and api.
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "sys_init.h"
#include "hal_system.h"
#include "hal_vic.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#include "hal_spi.h"
#include "hal_flash.h"
#include "hal_pwm.h"
#include "hal_auxadc.h"
#include "mw_fim\mw_fim.h"
#include "at_cmd_common.h"
#include "ipc.h"
#include "msg.h"
#include "ps.h"
#include "boot_sequence.h"
#include "mw_fim\mw_fim_default_group01.h"
#include "controller_wifi_com.h"

#define __SVN_REVISION__
#define __DIAG_TASK__
//#define __AT_CMD_TASK__
//#define __WIFI_MAC_TASK__
//#define __LWIP_TASK__
#define __HW_CRYPTO_ENGINE__
//#define __WPA_SUPPLICANT__
#define __CONTROLLER_TASK__
//#define __BLE__

#if defined(__SVN_REVISION__)
#include "svn_rev.h"
#endif

#if defined(__DIAG_TASK__)
#include "diag_task.h"
#endif

#if defined(__AT_CMD_TASK__)
#include "at_cmd_task.h"
#endif

#if defined(__WIFI_MAC_TASK__)
#include "wifi_mac_task.h"
#endif

#if defined(__LWIP_TASK__)
extern void lwip_task_create(void);
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

#if defined(__BLE__)
#include "le_rtos_task.h"
#endif

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define BOOT_MODE_ICE       0x2
#define BOOT_MODE_JTAG      0x3


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
RET_DATA T_Sys_PowerSetup_fp Sys_PowerSetup;
RET_DATA T_Sys_ClockSetup_fp Sys_ClockSetup;
RET_DATA T_Sys_UartInit_fp Sys_UartInit;
RET_DATA T_Sys_MiscModulesInit_fp Sys_MiscModulesInit;
RET_DATA T_Sys_MiscDriverConfigSetup_fp Sys_MiscDriverConfigSetup;
RET_DATA T_Sys_DriverInit_fp Sys_DriverInit;
RET_DATA T_Sys_RomVersion_fp Sys_RomVersion;
RET_DATA T_Sys_ServiceInit_fp Sys_ServiceInit;
RET_DATA T_Sys_AppInit_fp Sys_AppInit;
RET_DATA T_Sys_PostInit_fp Sys_PostInit;
RET_DATA T_Sys_IdleHook_fp Sys_IdleHook;


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   Sys_PowerSetup
*
* DESCRIPTION:
*   the initial for power driver
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_PowerSetup_impl(void)
{
    Hal_Sys_PowerDefaultSettings();
}

/*************************************************************************
* FUNCTION:
*   Sys_ClockSetup
*
* DESCRIPTION:
*   the initial for clock driver
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_ClockSetup_impl(void)
{
    // Work around for A0: 32K xtal
    Hal_Sys_PmuSfVolSet(PMU_SF_VAL_0P95V);
    Hal_Sys_32kXtalGainSet(0xF);
    Hal_Sys_32kXtalCapSet(0x0);
    
    // Switch to Xtal
    Hal_Sys_ApsClkTreeSetup(ASP_CLKTREE_SRC_XTAL, 0, 0);
    
#if defined(__NL1000_SOC_RF__)
    // RC calibration
    Hal_Sys_RcCal();
#endif
}

/*************************************************************************
* FUNCTION:
*   Sys_UartInit
*
* DESCRIPTION:
*   1. get the config of UART from FIM
*   2. init the UART modules
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_UartInit_impl(void)
{
    T_HalUartConfig tUartConfig;
    
    // Init UART0
    // cold boot
    if (0 == Boot_CheckWarmBoot())
    {
        if (MW_FIM_OK != MwFim_FileRead(MW_FIM_IDX_GP01_UART_CFG, 0, MW_FIM_UART_CFG_SIZE, (uint8_t*)&tUartConfig))
        {
            // if fail, get the default value
            memcpy(&tUartConfig, &g_tMwFimDefaultUartConfig, MW_FIM_UART_CFG_SIZE);
        }
    }
    // warm boot
    else
    {
        if (0 != Hal_Uart_ConfigGet(UART_IDX_0, &tUartConfig))
        {
            // if fail, get the default value
            memcpy(&tUartConfig, &g_tMwFimDefaultUartConfig, MW_FIM_UART_CFG_SIZE);
        }
    }
    Hal_Uart_Init(UART_IDX_0,
                  tUartConfig.ulBuadrate,
                  (E_UartDataBit_t)(tUartConfig.ubDataBit),
                  (E_UartParity_t)(tUartConfig.ubParity),
                  (E_UartStopBit_t)(tUartConfig.ubStopBit),
                  tUartConfig.ubFlowCtrl);
    
    // Init UART1
    // cold boot
    if (0 == Boot_CheckWarmBoot())
    {
        if (MW_FIM_OK != MwFim_FileRead(MW_FIM_IDX_GP01_UART_CFG, 1, MW_FIM_UART_CFG_SIZE, (uint8_t*)&tUartConfig))
        {
            // if fail, get the default value
            memcpy(&tUartConfig, &g_tMwFimDefaultUartConfig, MW_FIM_UART_CFG_SIZE);
        }
    }
    // warm boot
    else
    {
        if (0 != Hal_Uart_ConfigGet(UART_IDX_1, &tUartConfig))
        {
            // if fail, get the default value
            memcpy(&tUartConfig, &g_tMwFimDefaultUartConfig, MW_FIM_UART_CFG_SIZE);
        }
    }
    Hal_Uart_Init(UART_IDX_1,
                  tUartConfig.ulBuadrate,
                  (E_UartDataBit_t)(tUartConfig.ubDataBit),
                  (E_UartParity_t)(tUartConfig.ubParity),
                  (E_UartStopBit_t)(tUartConfig.ubStopBit),
                  tUartConfig.ubFlowCtrl);
}

/*************************************************************************
* FUNCTION:
*   Sys_MiscModulesInit
*
* DESCRIPTION:
*   the initial for misc driver
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_MiscModulesInit_impl(void)
{
    __DSB();
    __ISB();
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*   Sys_MiscDriverConfigSetup
*
* DESCRIPTION:
*   set the configuration for misc driver
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_MiscDriverConfigSetup_impl(void)
{
    __DSB();
    __ISB();
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*   Sys_DriverInit
*
* DESCRIPTION:
*   the initial for driver
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_DriverInit_impl(void)
{
    // Set power
    Sys_PowerSetup();

    // Set system clock
    Sys_ClockSetup();

    // Set pin-mux
    Hal_SysPinMuxAppInit();

    // Init VIC
    Hal_Vic_Init();
    
    // Init GPIO
    Hal_Vic_GpioInit();

	// Init IPC
    Hal_Vic_IpcIntEn(IPC_IDX_0, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_1, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_2, 1);
    Hal_Vic_IpcIntEn(IPC_IDX_3, 1);

    // Init DBG_UART
    Hal_DbgUart_Init(115200);
    printf("\n");

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

    // FIM
    MwFim_Init();

    // Init UART0 / UART1
    Sys_UartInit();
    
    // Init PWM
    Hal_Pwm_Init();
    
    // Init AUXADC
    Hal_Aux_Init();

    // Other modules' init
    Sys_MiscModulesInit();

    //-------------------------------------------------------------------------------------
    // Other driver config need by Task-level (sleep strategy)

    // Diag task
    Hal_DbgUart_RxCallBackFuncSet(uartdbg_rx_int_handler);
    // cold boot
    if (0 == Boot_CheckWarmBoot())
    {
        // ICE or JTag
        if ((BOOT_MODE_ICE == Hal_Sys_StrapModeRead()) || (BOOT_MODE_JTAG == Hal_Sys_StrapModeRead()))
        {
            // the default is on
            Hal_DbgUart_RxIntEn(1);
        }
        // others
        else
        {
            // the default is off
            Hal_DbgUart_RxIntEn(0);
        }
    }
    // warm boot
    else
    {
        Hal_DbgUart_RxIntEn(Hal_DbgUart_RxIntEnStatusGet());
    }

    // HCI and AT command
    uart1_mode_set_default();

    // Other tasks' driver config
    Sys_MiscDriverConfigSetup();

	// power-saving module init
	ps_init();

	if (Boot_CheckWarmBoot())
	{
		ps_wait_xtal_ready();
		Hal_Sys_ApsClkTreeSetup(ASP_CLKTREE_SRC_XTAL, 0, 0);

		// TODO: Revision will be provided by Ophelia after peripheral restore mechanism completed
		uart1_mode_set_default();
		uart1_mode_set_at();
	}
}

/*************************************************************************
* FUNCTION:
*   Sys_RomVersion
*
* DESCRIPTION:
*   output the ROM version
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_RomVersion_impl(void)
{
#if defined(__SVN_REVISION__)
    printf("\n");
    printf("[SVN REV] SVN_REVISION:%d \n", SVN_REVISION);
//    printf("[SVN REV] SVN_LOCAL_MODIFICATIONS:%d \n", SVN_LOCAL_MODIFICATIONS);
//    printf("[SVN REV] SVN_DATE:%s \n", SVN_DATE);
//    printf("[SVN REV] SVN_TIME_NOW:%s \n", SVN_TIME_NOW);
//    printf("[SVN REV] SVN_URL:%s \n", SVN_URL);
#endif
}

/*************************************************************************
* FUNCTION:
*   Sys_ServiceInit
*
* DESCRIPTION:
*   the initial for service
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_ServiceInit_impl(void)
{
    Sys_RomVersion();

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
#endif

#if defined(__BLE__)
    LeRtosTaskCreat();
#endif

#if defined(__WIFI_AUTO_CONNECT__)
    auto_connect_init();
#endif

    wifi_sta_info_init();
}

/*************************************************************************
* FUNCTION:
*   Sys_AppInit
*
* DESCRIPTION:
*   the initial for application
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_AppInit_impl(void)
{
    __DSB();
    __ISB();
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*   Sys_PostInit
*
* DESCRIPTION:
*   the post initial for sys init
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_PostInit_impl(void)
{
    /* Patch Function */
    tracer_init();
}

/*************************************************************************
* FUNCTION:
*   Sys_IdleHook
*
* DESCRIPTION:
*   the hook function of idle task
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_IdleHook_impl(void)
{
	ps_sleep();
}

/*************************************************************************
* FUNCTION:
*   Sys_PreInit
*
* DESCRIPTION:
*   the previous initial for sys init
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_PreInit(void)
{
    Sys_PowerSetup = Sys_PowerSetup_impl;
    Sys_ClockSetup = Sys_ClockSetup_impl;
    Sys_UartInit = Sys_UartInit_impl;
    Sys_MiscModulesInit = Sys_MiscModulesInit_impl;
    Sys_MiscDriverConfigSetup = Sys_MiscDriverConfigSetup_impl;
    Sys_DriverInit = Sys_DriverInit_impl;
    Sys_RomVersion = Sys_RomVersion_impl;
    Sys_ServiceInit = Sys_ServiceInit_impl;
    Sys_AppInit = Sys_AppInit_impl;
    Sys_PostInit = Sys_PostInit_impl;
    Sys_IdleHook = Sys_IdleHook_impl;
}
