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

typedef enum 
{
    CMD_FINISHED=0,
    CMD_CONTINUE=1, 
}E_CLI_CMD_PROC;

/**
 * @brief User CLI command in DbgUart.
 *        OPL1000 DbgUart command parser is after this function
 * @param pbuf [in] The input buffer pointer.
 * @param len [in] The length of input buffer.
 * @return Command searching status.
 * @retval CMD_FINISHED - Search command success. CLI command finished here.
 * @retval CMD_CONTINUE - Search command fail. After this function, it will run OPL1000 CLI commands.
 */
typedef E_CLI_CMD_PROC(*T_Cli_UserCmdProcess_fp)(char *pbuf, int len);

typedef void (*ParseAtVersionCommand_fp_t)(char *sCmd);
typedef void (*ParseSimpleAtCommand_fp_t)(char *sCmd);

extern ParseAtVersionCommand_fp_t ParseAtVersionCommand;
extern ParseSimpleAtCommand_fp_t ParseSimpleAtCommand;

/*
   Interface Initialization: DIAG TASK
 */
void diag_task_func_patch_init(void);
void Diag_UserCmdSet(T_Cli_UserCmdProcess_fp ptUserCmdFunc);
#endif
