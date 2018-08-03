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

#ifndef __PS_PATCH_H__
#define __PS_PATCH_H__

#include "ps.h"
#include "ps_task.h"
#include "ps_public.h"

#include "cmsis_os_patch.h"
#include "hal_system.h"
#include "hal_vic.h"

/******************** Constant Value Definition ********************/

#define PS_OS_TASK_TRIGGER_PRECISE           1

#define SOC_TICK_32K                         30.517578125

/******************** Data Struct Declaration ********************/

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
	
	// patch for public
	uint8_t xtal_ready_cost;
	uint8_t app_apply_deep;

} t_ps_conf_patch;

/******************** Function Prototype ********************/

void ps_patch_init(void);

// ps
void ps_init_patch(void);
void ps_wait_xtal_ready_patch(void);
int ps_sleep_patch(void);
void ps_dbg_cmd_conf_io(char *io_p);
void ps_parse_command_patch(char *pbuf, int len);
uint32_t ps_synchronize_timers_patch(void);
void ps_enable_wakeup_ext_io(void);
void ps_inner_io_wakeup_callback(E_GpioIdx_t eIdx);
void ps_default_wakeup_callback_patch(PS_WAKEUP_TYPE wakeup_type);
void ps_config_wakeup_ext_io(E_GpioIdx_t ext_io_num, E_ItrType_t ext_io_type);
void ps_config_wakeup_callback(PS_WAKEUP_CALLBACK callback);
void ps_enable_deep_sleep(void);
void ps_enable_timer_sleep(uint32_t sleep_duration_us);
void ps_enable_smart_sleep(int is_enable);

// ps_task
void ps_task_create_patch(void);
void ps_task_patch(void *argument);

#endif
