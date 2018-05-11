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

/**
 * @file at_cmd_task_patch.c
 * @author Vincent Tai
 * @date 8 Mar 2018
 * @brief This file creates the AT command tasks.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_task.h"
#include "at_cmd_task_patch.h"
#include "at_cmd_patch.h"
#include "at_cmd_app.h"
#include "at_cmd_tcpip_patch.h"

#include "sys_os_config.h"

extern int find_str(const char * str1, const char * str2);
extern osMessageQId xAtQueue;
extern  osThreadId AtTaskHandle;
extern osPoolId AtMemPoolId;
extern osPoolDef_t os_pool_def_atMemPool;

void opl_at_module_init(uint32_t netconn_max, const char *custom_version)
{
    osMessageQDef_t at_queue_def;
    osThreadDef_t at_task_def;

    /** create task */
    at_task_def.name = "AT";
    at_task_def.stacksize = OS_TASK_STACK_SIZE_DIAG;
    at_task_def.tpriority = OS_TASK_PRIORITY_DIAG;
    at_task_def.pthread = at_task;
    AtTaskHandle = osThreadCreate(&at_task_def, (void *)AtTaskHandle);

    if(AtTaskHandle == NULL)
    {
        tracer_cli(LOG_HIGH_LEVEL, "create thread fail \r\n");
        msg_print_uart1("create thread fail \r\n");
    }

    /** create memory pool */
    AtMemPoolId = osPoolCreate (osPool(atMemPool)); /** create Mem Pool */
    if (!AtMemPoolId)
    {
        tracer_cli(LOG_HIGH_LEVEL, "AT Task Mem Pool create Fail \r\n"); /** MemPool object not created, handle failure */
        msg_print_uart1("AT Task Mem Pool create Fail \r\n");
    }

    /** create message queue */
    at_queue_def.item_sz = sizeof( xATMessage );
    at_queue_def.queue_sz = AT_QUEUE_SIZE;
    xAtQueue = osMessageCreate(&at_queue_def, AtTaskHandle);
    if(xAtQueue == NULL)
    {
        tracer_cli(LOG_HIGH_LEVEL, "create queue fail \r\n");
        msg_print_uart1("create queue fail \r\n");
    }

    uart1_mode_set_default();

    uart1_mode_set_at();

    at_link_init(AT_LINK_MAX_NUM);
    at_create_tcpip_data_task();
    at_cmd_wifi_hook();
}


/*
 * @brief Parse AT CMD string
 *
 * @param [in] pbuf AT command string
 *
 * @param [in] len length of AT command string
 *
 */
void at_task_cmd_process_patch(char *pbuf, int len)
{
    if(find_str(pbuf, "at") | find_str(pbuf, "AT") |
       find_str(pbuf, "aT") | find_str(pbuf, "At"))
    {
        _at_cmd_parse(pbuf);
    }
    else
    {
        //msg_printII(false, LOG_HIGH_LEVEL, "%s", pbuf);
        msg_print_uart1("\n>");
    }
}

void at_task_create_patch(void)
{
    //stub function
}


/*
 * @brief AT Command Interface Initialization for AT Command Task
 *
 */
void at_task_func_init_patch(void)
{
    at_task_create = at_task_create_patch;
    at_task_cmd_process = at_task_cmd_process_patch;
}

