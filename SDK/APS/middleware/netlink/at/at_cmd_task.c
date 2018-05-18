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
 * @file at_cmd_task.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief This file creates the AT command tasks.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os.h"
#include "hal_dbg_uart.h"
#include "msg.h"
#include "controller_wifi.h"
#include "le_ctrl.h"
#include "cli.h"
#include "ipc.h"
#include "le_misc_patch.h"
#include "at_cmd.h"
#include "at_cmd_task.h"
#include "at_cmd_common.h"
#include "sys_os_config.h"
#include "hal_uart.h"
#include "le_ctrl.h"

/*
 * @brief Global variable xAtQueue retention attribute segment
 *
 */
RET_DATA osMessageQId xAtQueue;

/*
 * @brief Global variable AtTaskHandle retention attribute segment
 *
 */
RET_DATA osThreadId AtTaskHandle;

/*
 * @brief Global variable AtMemPoolId retention attribute segment
 *
 */
RET_DATA osPoolId AtMemPoolId;

/*
 * @brief Global variable at_semaId retention attribute segment
 *
 */
RET_DATA osSemaphoreId at_semaId;

osPoolDef (atMemPool, AT_QUEUE_SIZE, xATMessage); /** memory pool object */
osSemaphoreDef(at_sema);

/*
 * @brief An external global variable g_uart1_mode prototype declaration
 *
 */
extern unsigned int g_uart1_mode;

/*
 * @brief find if the str2 position is just at the beginning of str1
 *
 * @param [in] str1 Source String
 *
 * @param [in] str2 Sub String
 *
 * @return 1 yes,str2 position is just at the beginning of str1
 *
 * @return 0 no,str2 position is not at the beginning of str1
 */
static int find_str(const char * str1, const char * str2)
{
    char *loc = strstr(str1, str2);
    if(loc == NULL) return false;
    if((loc - str1) == 0) return true;
    return false;
}

/*
 * @brief Parse AT CMD string
 *
 * @param [in] pbuf AT command string
 *
 * @param [in] len length of AT command string
 *
 */
void at_task_cmd_process_impl(char *pbuf, int len)
{
    if(find_str(pbuf, "at"))
    {
        at_cmd_parse(pbuf);
    }
    else
    {
        //msg_printII(false, LOG_HIGH_LEVEL, "%s", pbuf);
        msg_print_uart1("\n>");
    }
}

/*
 * @brief Create semaphore for AT CMD task (reserved, unused now)
 *
 */
void at_task_create_semaphore_impl(void)
{
	at_semaId = osSemaphoreCreate(osSemaphore(at_sema), 10);
}

/*
 * @brief Release semaphore (reserved, unused now)
 *
 */
void at_semaphore_release_impl(void)
{
	osSemaphoreRelease(at_semaId);
}

/*
 * @brief Create AT CMD task
 *
 */
void at_task_create_impl(void)
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

    uart1_mode_set_at();
}

/*
 * @brief Send message to AT CMD task
 *
 * @param [in] txMsg
 *
 * @return  0 osOK non-0 other status codes
 *
 */
osStatus at_task_send_impl(xATMessage txMsg)
{
    osStatus ret = osOK;
    xATMessage *pMsg;

    /** Mem pool allocate */
    pMsg = (xATMessage *)osPoolCAlloc (AtMemPoolId); /** get Mem Block */
    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    if(txMsg.length != 0)
    {
        /** malloc buffer */
        pMsg->pcMessage = (void *)pvPortMalloc(txMsg.length);
        if(txMsg.pcMessage != NULL)
        {
            memcpy((void *)pMsg->pcMessage, (void *)txMsg.pcMessage, txMsg.length);
        }
        else
        {
            ret = osErrorOS;
            tracer_cli(LOG_HIGH_LEVEL, "at task message allocate fail \r\n");
            msg_print_uart1("at task message allocate fail \r\n");
        }
    }
    if(ret == osOK)
        osMessagePut (xAtQueue, (uint32_t)pMsg, osWaitForever); /** Send Message */

    return ret;
}

/*
 * @brief AT CMD task body
 *
 * @param [in] pvParameters Task parameters
 *
 */
void at_task_impl(void *pvParameters)
{
    osEvent rxEvent;
    xATMessage *rxMsg;
    at_uart_buffer_t *pData;

    tracer_cli(LOG_HIGH_LEVEL, "AT task is created successfully! \r\n");
    msg_print_uart1("\r\n>");

    for(;;)
    {
        /** wait event */
        rxEvent = osMessageGet(xAtQueue, osWaitForever);
        if(rxEvent.status != osEventMessage) continue;

        rxMsg = (xATMessage *) rxEvent.value.p;
        if(rxMsg->event == AT_UART1_EVENT)
        {
            pData = (at_uart_buffer_t *)rxMsg->pcMessage;
			at_task_cmd_process(pData->buf, strlen(pData->buf));
            at_clear_uart_buffer();
        }

        if(rxMsg->pcMessage != NULL) vPortFree(rxMsg->pcMessage);
        osPoolFree (AtMemPoolId, rxMsg);
    }
}

/*
 * @brief An external Function at_task_cmd_process prototype declaration retention attribute segment
 *
 */
RET_DATA at_task_cmd_process_fp_t at_task_cmd_process;

/*
 * @brief An external Function at_task_create_semaphore prototype declaration retention attribute segment
 *
 */
RET_DATA at_task_create_semaphore_fp_t at_task_create_semaphore;

/*
 * @brief An external Function at_semaphore_release prototype declaration retention attribute segment
 *
 */
RET_DATA at_semaphore_release_fp_t at_semaphore_release;

/*
 * @brief An external Function at_task_create prototype declaration retention attribute segment
 *
 */
RET_DATA at_task_create_fp_t at_task_create;

/*
 * @brief An external Function at_task_send prototype declaration retention attribute segment
 *
 */
RET_DATA at_task_send_fp_t at_task_send;

/*
 * @brief An external Function at_task prototype declaration retention attribute segment
 *
 */
RET_DATA at_task_fp_t at_task;

/*
 * @brief AT Command Interface Initialization for AT Command Task
 *
 */
void at_task_func_init(void)
{
    at_task_cmd_process = at_task_cmd_process_impl;
    at_task_create_semaphore = at_task_create_semaphore_impl;
    at_semaphore_release = at_semaphore_release_impl;
    at_task_create = at_task_create_impl;
    at_task_send = at_task_send_impl;
    at_task = at_task_impl;
}

