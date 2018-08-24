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
*  OPL1000 Project - the system initial implement file
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
#include "hal_wdt.h"
#include "mw_fim\mw_fim.h"
#include "at_cmd_common.h"
#include "ipc.h"
#include "msg.h"
#include "ps_patch.h"
#include "boot_sequence.h"
#include "mw_fim\mw_fim_default_group01.h"
#include "controller_wifi_com.h"
#include "mw_fim_default_patch.h"
#include "ipc_patch.h"
#include "msg_patch.h"
#include "agent.h"
#include "le_ctrl_patch.h"
#include "ble_host_patch_init.h"
#include "at_cmd_sys_patch.h"
#include "diag_task_patch.h"
#include "controller_wifi_patch.h"
#include "wpas_init_patch.h"
#include "at_cmd_patch.h"
#include "wifi_mac_tx_data_patch.h"
#include "at_cmd_func_patch.h"
#include "wifi_nvm_patch.h"
#include "wifi_service_func_init_patch.h"
#include "lwip_jmptbl_patch.h"
#include "cmsis_os_patch.h"
#include "opl1000_it_patch.h"

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
//#include "le_rtos_task.h"
#endif

// the include file for patch from here
#include "sys_init_patch.h"
#include "hal_patch.h"
#include "hal_system_patch.h"
#include "peri_patch_init.h"
#include "mw_ota.h"
#include "scrt_patch.h"
#include "controller_task_patch.h"
#include "rf_cfg.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define BOOT_MODE_ICE       0x2
#define BOOT_MODE_JTAG      0x3
#define BOOT_MODE_NORMAL    0xA

#define WDT_TIMEOUT_SECS    10

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern unsigned int Image$$RW_IRAM1$$ZI$$Length;
extern char Image$$RW_IRAM1$$ZI$$Base[];


// Sec 5: declaration of global function prototype
extern void LeRtosTaskCreat(void);
void SysInit_EntryPoint(void);
void Sys_ClockSetup_patch(void);


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype
static void __Test_ForSwPatch(void);
static void Sys_DriverInit_patch(void);
static void Sys_ServiceInit_patch(void);
static void SysInit_LibVersion(void);
static void Sys_IdleHook_patch(void);

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
    
    // system
    Sys_ClockSetup = Sys_ClockSetup_patch;
    Sys_DriverInit = Sys_DriverInit_patch;
    Sys_ServiceInit = Sys_ServiceInit_patch;
    Sys_IdleHook = Sys_IdleHook_patch;

    // FIM Default
    mw_fim_default_patch_init();

    // IPC
    ipc_patch_init();

    /** WPA_Supplicant Patch Initialization */
    wpas_init_patch();
    
    //wifi mac patch
    wifi_mac_txdata_func_init_patch();
    
    //Wifi controller
    wifi_ctrl_init_patch();
    wifi_nvm_func_patch();
    wifi_service_func_init_patch();
    
    // le_ctrl
    le_ctrl_pre_patch_init();
    
    // Agent
    agent_patch_init();

    // Tracer
    Tracer_PatchInit();

    // BLE HOST
    LeHostPatchAssign();
    
    // AT
    _at_cmd_sys_func_patch_init();
    at_func_init_patch();
    
    // diag task
    diag_task_func_patch_init();
    
    // LwIP
    lwip_module_interface_init_patch();
    
    // Peripheral
    peripheral_patch_init();
    
    // SCRT
    scrt_drv_func_init_patch();
    
    // power saving
    ps_patch_init();

    // controller task
    controller_task_func_init_patch();

    // RF config
    rf_cfg_pre_init_patch();
	
	// CMSIS-RTOS
	freertos_patch_init();
	ISR_Pre_Init_patch();
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
void Sys_ClockSetup_patch(void)
{
	if (!Boot_CheckWarmBoot())
    {
        // Switch to Xtal
        Hal_Sys_ApsClkTreeSetup(ASP_CLKTREE_SRC_XTAL, 0, 0);
        Hal_Sys_DisableClock();
    }
}

/*************************************************************************
* FUNCTION:
*   Main_WaitforMsqReady
*
* DESCRIPTION:
*   wait for M0 initialization to be completed
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Main_WaitforMsqReady()
{
	const uint32_t m0_ready_msk = 1 << 4;
	uint32_t reg_spare_0_val;

	do {
		Hal_Sys_SpareRegRead(SPARE_0, &reg_spare_0_val);
	} while (!(reg_spare_0_val & m0_ready_msk));

	Hal_Sys_SpareRegWrite(SPARE_0, reg_spare_0_val & ~m0_ready_msk);
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
static void Sys_DriverInit_patch(void)
{
    // Set power
    Sys_PowerSetup();

    // Set system clock
    Sys_ClockSetup();

    // Set pin-mux
    Hal_SysPinMuxAppInit();

	  // Added for mapping IO8/9 to mini-USB UART 
	  // Hal_SysPinMuxDownloadInit();
	
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
    //Hal_Spi_Init(SPI_IDX_1, SystemCoreClockGet()/2,
    //    SPI_CLK_PLOAR_HIGH_ACT, SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);
    //Hal_Spi_Init(SPI_IDX_2, SystemCoreClockGet()/2,
    //    SPI_CLK_PLOAR_HIGH_ACT, SPI_CLK_PHASE_START, SPI_FMT_MOTOROLA, SPI_DFS_08_bit, 1);

    // Init flash on SPI 0/1/2
    Hal_Flash_Init(SPI_IDX_0);
    //Hal_Flash_Init(SPI_IDX_1);
    //Hal_Flash_Init(SPI_IDX_2);

    // FIM
    MwFim_Init();

    // Init UART0 / UART1
    Sys_UartInit();
    
    // Init PWM
    //Hal_Pwm_Init();
    
    // Init AUXADC
    Hal_Aux_Init();

    // Other modules' init
    Sys_MiscModulesInit();

    // Wait for M0 initialization to be completed
    Main_WaitforMsqReady();

    //-------------------------------------------------------------------------------------
    // Other driver config need by Task-level (sleep strategy)

    // Diag task
    Hal_DbgUart_RxCallBackFuncSet(uartdbg_rx_int_handler);
    // cold boot
    if (0 == Boot_CheckWarmBoot())
    {
			  // the default is on
        Hal_DbgUart_RxIntEn(1);
			
			  /*
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
			  */ 
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
    if (Hal_Sys_StrapModeRead() == BOOT_MODE_NORMAL)
    {
        Hal_Vic_IntTypeSel(WDT_IRQn, INT_TYPE_FALLING_EDGE);
        Hal_Vic_IntInv(WDT_IRQn, 1);
        Hal_Wdt_Init(WDT_TIMEOUT_SECS * SystemCoreClockGet());
    }
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
static void Sys_ServiceInit_patch(void)
{
    T_MwOtaLayoutInfo tLayout;
    
    Sys_RomVersion();
    
    SysInit_LibVersion();

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
    
    // Agent
    agent_init();

    // Load param from FIM for Tracer
    tracer_load();
    
    // OTA
    MwOta_PreInitCold();
    tLayout.ulaHeaderAddr[0] = MW_OTA_HEADER_ADDR_1;
    tLayout.ulaHeaderAddr[1] = MW_OTA_HEADER_ADDR_2;
    tLayout.ulaImageAddr[0] = MW_OTA_IMAGE_ADDR_1;
    tLayout.ulaImageAddr[1] = MW_OTA_IMAGE_ADDR_2;
    tLayout.ulImageSize = MW_OTA_IMAGE_SIZE;
    MwOta_Init(&tLayout, 0);
}
void Sys_IdleHook_patch(void)
{
    if (Hal_Sys_StrapModeRead() == BOOT_MODE_NORMAL)
    {
        Hal_Wdt_Clear();
    }
	ps_sleep();
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
    printf("[Lib] Compile time: %s\n", SVN_TIME_NOW);
}

/*************************************************************************
* FUNCTION:
*   Sys_StackOverflowHook
*
* DESCRIPTION:
*   the hook function of stack overflow
*
* PARAMETERS
*   1. xTask      : [In] the pointer of task handle
*   2. pcTaskName : [In] the task name
*
* RETURNS
*   none
*
*************************************************************************/
void Sys_StackOverflowHook_patch(TaskHandle_t xTask, char *pcTaskName) __attribute__((section(".ARM.__at_0x00424000")));
void Sys_StackOverflowHook_patch(TaskHandle_t xTask, char *pcTaskName) __attribute__((used));
void Sys_StackOverflowHook_patch(TaskHandle_t xTask, char *pcTaskName)
{
	tracer_drct_printf("stack overflow: %x %s\r\n", (unsigned int)xTask, (portCHAR *)pcTaskName);
    
    while(1) {}
}
