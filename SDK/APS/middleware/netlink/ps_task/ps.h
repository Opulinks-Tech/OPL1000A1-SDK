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

#ifndef __PS_H__
#define __PS_H__

#include <stdint.h>

#include "hal_vic.h"

/******************** Constant Value Definition ********************/

#define TIMER_1M_MAX_VAL								2147483647
#if defined(__NL1000_A0__)
  #define TICK_32K										30.517578125
#else
  #define TICK_32K										31.31818181526318
#endif
#define TICK_RTOS										1000
#define PS_SLEEP_MIN_LIMIT_US							5000
#define PS_SLEEP_COST_GAIN_US							500
#define PS_SLEEP_COST_ESTIMATE_US						(5000 + PS_SLEEP_COST_GAIN_US)

#define PS_RET_SUCCESS									0
#define PS_RET_EXIST_MODULE_BUSY						0x01
#define PS_RET_SLEEP_MIN_LIMIT_NOT_SATISFIED			0x02
#define PS_RET_APS_NOT_FIRE_SLEEP						0x11
#define PS_RET_APS_NOT_IDLE								0x12
#define PS_RET_APS_IPC_NOT_COMPLETED					0x14
#define PS_RET_APS_NOT_TAKE_QUILT						0x18
#define PS_RET_MSQ_IPC_NOT_COMPLETED					0x21
#define PS_RET_MSQ_NOT_IDLE								0x22

#define XTAL_READY_TIME									0x15	// unit in 32K cycle

/******************** Data Struct Declaration ********************/

typedef enum
{
	proc_type_aps_req,
	proc_type_msq_rsp,
	proc_type_msq_req,
	proc_type_aps_rsp

} e_ps_proc_type;

typedef struct
{
	uint8_t aps_system_busy;
	uint8_t aps_driver_busy;
	uint8_t aps_ble_app_busy;
	uint8_t aps_ble_host_busy;
	uint8_t aps_lwip_busy;
	uint8_t aps_wpa_supplicant_busy;
	uint8_t aps_wifi_busy;
	uint8_t aps_ble_controller_layer_busy;
	uint8_t aps_wifi_controller_layer_busy;
	uint8_t aps_at_busy;
	uint8_t aps_iperf_busy;

	uint8_t msq_system_busy;
	uint8_t msq_driver_busy;
	uint8_t msq_mw_cli_busy;
	uint8_t msq_mw_ipc_busy;
	uint8_t msq_mw_le_ctrl_busy;
	uint8_t msq_mw_ps_busy;
	uint8_t msq_mw_token_switch_busy;
	uint8_t msq_mw_wifi_busy;

	uint8_t reserved[4];

} t_ps_busy_flag;

typedef struct
{
	uint8_t aps_req_exist;
	uint8_t msq_req_exist;
	uint32_t aps_req_time;
	uint32_t msq_rsp_time;
	uint32_t msq_req_time;
	uint32_t aps_rsp_time;

	uint32_t aps_sleep_time;
	uint32_t aps_sleep_duration;
	uint32_t msq_sleep_time;
	uint32_t msq_sleep_duration;
	uint32_t ps_sleep_duration;
	uint8_t msq_ps_init;
	uint8_t aps_can_take_quilt;
	uint8_t aps_is_dreaming;

} t_ps_proc_ctrl;

typedef struct
{
	uint64_t msq_32k_timer;
	uint64_t aps_32k_timer;
	uint32_t msq_1m_timer;
	uint32_t aps_rtos_timer;

} t_ps_snap_time;

typedef struct
{
	uint32_t last_status;

	t_ps_proc_ctrl proc_ctrl;
	t_ps_busy_flag busy_flag;
	t_ps_snap_time snap_time;

	uint64_t wakeup_time;
	uint32_t sleep_min_limit;
	uint32_t wakeup_cost_gain;
	uint32_t wakeup_cost;
	uint8_t wakeup_cost_precise;

	uint8_t app_apply_sleep;
	uint8_t app_active_sleep;

} t_ps_conf;

/******************** Patch Declaration ********************/

typedef void (*t_ps_init_fp)(void);
typedef void (*t_ps_wait_xtal_ready_fp)(void);
typedef void (*t_ps_update_processing_time_fp)(e_ps_proc_type type);
typedef int  (*t_ps_sleep_fp)(void);
typedef void (*t_ps_default_wakeup_callback_fp)(void);
typedef void (*t_ps_sleep_requested_by_app_fp)(uint32_t sleep_duration_us, void (*callback)(void));
typedef void (*t_ps_enable_fp)(uint8_t is_enable);
typedef void (*t_ps_parse_command_fp)(char *pbuf, int len);
typedef void (*t_ps_snapshot_timers_fp)(void);
typedef uint32_t (*t_ps_synchronize_timers_fp)(void);
typedef void (*t_ps_notify_msq_dreaming_fp)(void);
typedef uint32_t (*t_ps_get_1m_timer_fp)(void);
typedef void (*t_ps_early_wakeup_callback_fp)(E_GpioIdx_t eIdx);
typedef void (*t_ps_early_wakeup_enable_fp)(void);

extern t_ps_init_fp                        ps_init;
extern t_ps_wait_xtal_ready_fp             ps_wait_xtal_ready;
extern t_ps_update_processing_time_fp      ps_update_processing_time;
extern t_ps_sleep_fp                       ps_sleep;
extern t_ps_default_wakeup_callback_fp     ps_default_wakeup_callback;
extern t_ps_sleep_requested_by_app_fp      ps_sleep_requested_by_app;
extern t_ps_enable_fp                      ps_enable;
extern t_ps_parse_command_fp               ps_parse_command;
extern t_ps_snapshot_timers_fp             ps_snapshot_timers;
extern t_ps_synchronize_timers_fp          ps_synchronize_timers;
extern t_ps_notify_msq_dreaming_fp         ps_notify_msq_dreaming;
extern t_ps_get_1m_timer_fp                ps_get_1m_timer;
extern t_ps_early_wakeup_callback_fp       ps_early_wakeup_callback;
extern t_ps_early_wakeup_enable_fp         ps_early_wakeup_enable;

/******************** Function Prototype ********************/

void ps_pre_init(void);

// public function
void ps_init_impl(void);
void ps_wait_xtal_ready_impl(void);
void ps_update_processing_time_impl(e_ps_proc_type type);
int  ps_sleep_impl(void);
void ps_default_wakeup_callback_impl(void);
void ps_sleep_requested_by_app_impl(uint32_t sleep_duration_us, void (*callback)(void));
void ps_enable_impl(uint8_t is_enable);
void ps_parse_command_impl(char *pbuf, int len);

// private function
void ps_snapshot_timers_impl(void);
uint32_t ps_synchronize_timers_impl(void);
void ps_notify_msq_dreaming_impl(void);
uint32_t ps_get_1m_timer_impl(void);
void ps_early_wakeup_callback_impl(E_GpioIdx_t eIdx);
void ps_early_wakeup_enable_impl(void);

#endif
