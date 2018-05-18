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
*  sys_init_patch.c
*
*  Project:
*  --------
*  NL1000 Project - the system initial implement file
*
*  Description:
*  ------------
*  This implement file is include the system initial function and api.
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
#include "hal_auxadc.h"
#include "diag_task.h"
#include "le_ctrl.h"
#include "at_cmd.h"
#include "at_cmd_common.h"

// Sec 1: Include File
#include "sys_init_patch.h"
#include "hal_patch.h"
#include "hal_vic_patch.h"
#include "hal_system_patch.h"
#include "hal_uart_patch.h"
#include "svn_rev.h"
#include "wifi_mac_patch.h"
#include "controller_wifi_patch_init.h"
#include "scrt_patch.h"
#include "peri_patch_init.h"
#include "nl1000_it_patch.h"
#include "ps_patch.h"
#include "ipc_patch.h"
#include "ble_host_patch_init.h"
#include "diag_task_patch.h"
#include "lwip_jmptbl_patch.h"
#include "wpas_patch_init.h"
#include "at_cmd_patch.h"
#include "at_cmd_task_patch.h"
#include "mw_fim.h"
#include "cmsis_os_patch.h"
#include "cmsis_os_patch_init.h"
#include "mw_fim_default_group01.h"
#include "hal_dbg_uart_patch.h"
#include "msg_patch.h"
#include "le_ctrl_patch.h"
#include "controller_wifi_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define BOOT_MODE_ICE       0x2
#define BOOT_MODE_JTAG      0x3


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
typedef void (*T_Main_DriverInit_fp)(void);
typedef void (*T_Main_PowerSetup_fp)(void);
typedef void (*T_Main_ClockSetup_fp)(void);
typedef void (*T_Main_MiscModulesInit_fp)(void);
typedef void (*T_Main_MiscDriverConfigSetup_fp)(void);
typedef void (*T_Main_ServiceInit_fp)(void);
typedef void (*T_Main_AppInit_fp)(void);
typedef void (*T_Main_PostInit_fp)(void);
typedef void (*T_Main_IdleHook_fp)(void);


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern unsigned int Image$$RW_IRAM1$$ZI$$Length;
extern char Image$$RW_IRAM1$$ZI$$Base[];

extern T_Main_DriverInit_fp Main_DriverInit;
extern T_Main_PowerSetup_fp Main_PowerSetup;
extern T_Main_ClockSetup_fp Main_ClockSetup;
extern T_Main_MiscModulesInit_fp Main_MiscModulesInit;
extern T_Main_MiscDriverConfigSetup_fp Main_MiscDriverConfigSetup;
extern T_Main_ServiceInit_fp Main_ServiceInit;
extern T_Main_AppInit_fp Main_AppInit;
extern T_Main_PostInit_fp Main_PostInit;
extern T_Main_IdleHook_fp Main_IdleHook;


// Sec 5: declaration of global function prototype
extern void Main_ServiceInit_impl(void);


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype
static void __Test_ForSwPatch(void);
static void SysInit_LibVersion(void);
static void SysInit_UartInit(void);

static void Boot_PreInitWarm_patch(void)
{
    // According to sleep strategy, drivers' global paramters re-init here
    Hal_DbgUart_ParamInit();
}

void Main_PowerSetup_patch(void)
{
    Hal_Sys_PwrDefaultSettings();
}

void Main_ClockSetup_patch(void)
{
	if (!Boot_CheckWarmBoot())
	{
		// Work around for A0: 32K xtal
		Hal_Sys_PmuSfVolSet(PMU_SF_VAL_0P95V);
		Hal_Sys_32kXtalGainSet(0xF);
		Hal_Sys_32kXtalCapSet(0x0);

		// Switch to Xtal
		Hal_Sys_ApsClkTreeSetup(ASP_CLKTREE_SRC_XTAL, 0, 0);

#if defined(__NL1000_A0__)
		// RC calibration
		Hal_Sys_RcCal();
#endif
	}
}


void Main_WaitforMsqReady()
{
	const uint32_t m0_ready_msk = 1 << 4;
	uint32_t reg_spare_0_val;

	do {
		Hal_Sys_SpareRegRead(SPARE_0, &reg_spare_0_val);
	} while (!(reg_spare_0_val & m0_ready_msk));

	Hal_Sys_SpareRegWrite(SPARE_0, reg_spare_0_val & ~m0_ready_msk);
}

void Main_DriverInit_patch(void)
{
    // os init
    osKernelInitialize_patch();

    // Set power
    Main_PowerSetup();

    // Set system clock
    Main_ClockSetup();

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
    SysInit_UartInit();

    // Init PWM
    Hal_Pwm_Init();

    // Init AUXADC
    Hal_Aux_Init();

    // Other modules' init
    Main_MiscModulesInit();
    Main_WaitforMsqReady();

    //-------------------------------------------------------------------------------------
    // Other driver config need by Task-level (sleep strategy)

    // Diag task
    Hal_DbgUart_RxCallBackFuncSet(uartdbg_rx_int_patch_handler);
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
    //uart1_mode_set_default();

    // Other tasks' driver config
    Main_MiscDriverConfigSetup();

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

void Main_ServiceInit_patch(void)
{
    // Lib version
    SysInit_LibVersion();

    // scrt init, not enabled in ROM code
    nl_scrt_Init();

    // the original service init
    Main_ServiceInit_impl();

    //Auto connect
    auto_connect_init();    
}

void Main_IdleHook_patch(void)
{
	ps_sleep();
}

/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   SysInit_EntryPoint
*
* DESCRIPTION:
*   put the sw patch here
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void SysInit_EntryPoint(void)
{
    // for cold boot only
    if (0 != Boot_CheckWarmBoot())
        return;

    // init bss section
    memset(Image$$RW_IRAM1$$ZI$$Base, 0, (unsigned int)&Image$$RW_IRAM1$$ZI$$Length);

    // apply the sw patch from here
    Test_ForSwPatch = __Test_ForSwPatch;

	// Boot Init
	Boot_PreInitWarm = Boot_PreInitWarm_patch;

    // Main Init
    Main_DriverInit = Main_DriverInit_patch;
    Main_PowerSetup = Main_PowerSetup_patch;
    Main_ClockSetup = Main_ClockSetup_patch;
    Main_ServiceInit = Main_ServiceInit_patch;

    // Idel task
	// TODO: disable ps mechanism temporarily until MCUs initialization status checking plan done
	Main_IdleHook = Main_IdleHook_patch;

    //wifi mac patch
    wifi_mac_crypto_func_init_patch();
    wifi_mac_task_func_init_patch();
    wifi_mac_rx_data_func_init_patch();

    //wifi control
    wifi_ctrl_patch_init();
    /** SCRT Driver Patch Initialization */
    scrt_drv_func_init_patch();

    /** WPA_Supplicant Patch Initialization */
    wpas_patch_init();

    //AT command
    at_task_func_init_patch();
    at_cmd_init_patch();

    // Peripheral
    peripheral_patch_init();

    // ISRs
    isr_patch_init();

	// IPC
	ipc_patch_init();

	// Power Saving
	ps_patch_init();

    // le_ctrl
    le_ctrl_pre_patch_init();

    // BLE HOST
    LeHostPatchAssign();

    // diag task
    diag_task_patch_func_init();

    // LwIP
    lwip_module_interface_init_patch();

    // os
    os_patch_init();

    // Tracer
    Tracer_PatchInit();
}

/*************************************************************************
* FUNCTION:
*   __Test_ForSwPatch
*
* DESCRIPTION:
*   it is a test function for SW patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void __Test_ForSwPatch(void)
{
    printf("Sw patch is changed successfully.\n");
}

/*************************************************************************
* FUNCTION:
*   SysInit_LibVersion
*
* DESCRIPTION:
*   the version of lib
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void SysInit_LibVersion(void)
{
    printf("\n");
    printf("[Lib] SDK version info: %d\n", SVN_REVISION);
//    printf("[Lib] SVN_LOCAL_MODIFICATIONS:%d \n", SVN_LOCAL_MODIFICATIONS);
//    printf("[Lib] SVN_DATE:%s \n", SVN_DATE);
    printf("[Lib] Compile time: %s\n", SVN_TIME_NOW);
//    printf("[Lib] SVN_URL:%s \n", SVN_URL);
}

/*************************************************************************
* FUNCTION:
*   SysInit_UartInit
*
* DESCRIPTION:
*   the init of UART0 / UART1
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void SysInit_UartInit(void)
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
