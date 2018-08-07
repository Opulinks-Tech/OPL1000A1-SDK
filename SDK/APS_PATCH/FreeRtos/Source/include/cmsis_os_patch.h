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

#ifndef __CMSIS_OS_H__
#define __CMSIS_OS_H__

#include "cmsis_os.h"

// freertos_cmsis
void freertos_patch_init(void);
osStatus osKernelRestart_patch(void);
TickType_t osKernelNextTaskUnblockTime(void);
osStatus osMemoryPoolCreate_patch(void);

// task
void vTaskRestartScheduler_patch(void);

#endif
