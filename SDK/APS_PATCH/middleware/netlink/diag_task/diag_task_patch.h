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

#ifndef __DIAG_TASK_PATCH_H__
#define __DIAG_TASK_PATCH_H__

/* Scheduler includes. */
#include "diag_task.h"

typedef void (*ParseAtVersionCommand_fp_t)(char *sCmd);
typedef void (*ParseSimpleAtCommand_fp_t)(char *sCmd);

extern ParseAtVersionCommand_fp_t ParseAtVersionCommand;
extern ParseSimpleAtCommand_fp_t ParseSimpleAtCommand;

/*
   Interface Initialization: DIAG TASK
 */
void diag_task_func_patch_init(void);

#endif
