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

#include "ps.h"
#include "ps_task.h"

#include "cmsis_os.h"

void ps_patch_init(void);
void ps_task_create_patch(void);
TickType_t ps_adjust_tick_patch(TickType_t tick);
