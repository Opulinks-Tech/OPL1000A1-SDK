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

#ifndef __AT_CMD_TASK_PATCH_H__
#define __AT_CMD_TASK_PATCH_H__
#include "cmsis_os.h"

extern osSemaphoreId g_tSwitchuartSem;

void at_task_func_init_patch(void);
int at_wifi_net_task_init(void);

#endif //__AT_CMD_TASK_PATCH_H__

