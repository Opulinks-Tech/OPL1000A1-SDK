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

/******************** Constant Value Definition ********************/

#define TIMER_1M_MAX_VAL								2147483647
#define TICK_32K										31.31818181526318	// TODO: change to 30.517578125 (32768)
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
	t_ps_proc_ctrl proc_ctrl;
	t_ps_busy_flag busy_flag;
	t_ps_snap_time snap_time;

	uint64_t wakeup_time;
	uint32_t sleep_min_limit;
	uint32_t wakeup_cost_gain;
	uint32_t wakeup_cost;
	uint8_t wakeup_cost_precise;

	uint8_t app_apply_sleep;

} t_ps_conf;

/******************** Function Prototype ********************/

// public function
void ps_init(void);
void ps_wait_xtal_ready(void);
void ps_update_processing_time(e_ps_proc_type type);
int  ps_sleep(void);
void ps_enable(uint8_t is_enable);
void ps_parse_command(char *pbuf, int len);

// private function
void ps_snapshot_timers(void);
uint32_t ps_synchronize_timers(void);
void ps_notify_msq_dreaming(void);
uint32_t ps_get_1m_timer(void);

#endif
