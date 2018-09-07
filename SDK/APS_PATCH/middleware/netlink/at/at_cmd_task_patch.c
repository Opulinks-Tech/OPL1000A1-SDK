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

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os.h"

#include "at_cmd.h"
#include "at_cmd_task.h"
#include "at_cmd_common.h"
#include "at_cmd_app.h"
#include "at_cmd_tcpip.h"
#include "sys_os_config.h"

#include "at_cmd_task_patch.h"
#include "sys_os_config_patch.h"

#define CONFIG_MAX_SOCKETS_NUM      5

/*
 * @brief Global variable xAtQueue retention attribute segment
 *
 */
extern RET_DATA osMessageQId xAtQueue;

/*
 * @brief Global variable AtTaskHandle retention attribute segment
 *
 */
extern RET_DATA osThreadId AtTaskHandle;

/*
 * @brief Global variable AtMemPoolId retention attribute segment
 *
 */
extern RET_DATA osPoolId AtMemPoolId;

/*
 * @brief Global variable at_semaId retention attribute segment
 *
 */
extern RET_DATA osSemaphoreId at_semaId;

extern const osPoolDef_t os_pool_def_atMemPool;
extern const osSemaphoreDef_t os_semaphore_def_at_sema;

/*
 * @brief An external global variable g_uart1_mode prototype declaration
 *
 */
extern unsigned int g_uart1_mode;

extern osThreadId at_app_task_id;

void at_module_init_patch(uint32_t netconn_max, const char *custom_version)
{
    osMessageQDef_t at_queue_def;
    osThreadDef_t at_task_def;

    /** create task */
    at_task_def.name = OS_TASK_NAME_AT;
    at_task_def.stacksize = OS_TASK_STACK_SIZE_AT_PATCH;
    at_task_def.tpriority = OS_TASK_PRIORITY_DIAG;
    at_task_def.pthread = at_task;
    AtTaskHandle = osThreadCreate(&at_task_def, (void *)AtTaskHandle);

    if(AtTaskHandle == NULL)
    {
        tracer_log(LOG_HIGH_LEVEL, "create thread fail \r\n");
        msg_print_uart1("create thread fail \r\n");
    }

    /** create memory pool */
    AtMemPoolId = osPoolCreate (osPool(atMemPool)); /** create Mem Pool */
    if (!AtMemPoolId)
    {
        tracer_log(LOG_HIGH_LEVEL, "AT Task Mem Pool create Fail \r\n"); /** MemPool object not created, handle failure */
        msg_print_uart1("AT Task Mem Pool create Fail \r\n");
    }

    /** create message queue */
    at_queue_def.item_sz = sizeof( xATMessage );
    at_queue_def.queue_sz = AT_QUEUE_SIZE;
    xAtQueue = osMessageCreate(&at_queue_def, AtTaskHandle);
    if(xAtQueue == NULL)
    {
        tracer_log(LOG_HIGH_LEVEL, "create queue fail \r\n");
        msg_print_uart1("create queue fail \r\n");
    }

    //move from sys_init
    uart1_mode_set_default();

    uart1_mode_set_at();
}

int at_wifi_net_task_init(void)
{
    if (at_app_task_id == NULL) {
        at_link_init(AT_LINK_MAX_NUM);
        at_create_tcpip_data_task();
        at_cmd_wifi_hook();
        return 0;
    }
    else {
        msg_print_uart1("AT Wifi and TCPIP initialize already \r\n");
        return -1;
    }
}

/*
 * @brief Send message to AT CMD task
 *
 * @param [in] txMsg
 *
 * @return  0 osOK non-0 other status codes
 *
 */
osStatus at_task_send_patch(xATMessage txMsg)
{
    osStatus ret = osErrorOS;
    xATMessage *pMsg = NULL;

    /** Mem pool allocate */
    pMsg = (xATMessage *)osPoolCAlloc (AtMemPoolId); /** get Mem Block */

    if(pMsg == NULL)
    {
        goto done;
    }

    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    pMsg->pcMessage = NULL;

    if((txMsg.pcMessage) && (txMsg.length))
    {
        /** malloc buffer */
        pMsg->pcMessage = (void *)malloc(txMsg.length);

        if(pMsg->pcMessage == NULL)
        {
            tracer_log(LOG_HIGH_LEVEL, "at task message allocate fail \r\n");
            msg_print_uart1("at task message allocate fail \r\n");
            goto done;
        }

        memcpy((void *)pMsg->pcMessage, (void *)txMsg.pcMessage, txMsg.length);
    }

    if(osMessagePut (xAtQueue, (uint32_t)pMsg, osWaitForever) != osOK) /** Send Message */
    {
        goto done;
    }

    ret = osOK;

done:
    if(ret != osOK)
    {
        if(pMsg)
        {
            if(pMsg->pcMessage)
            {
                free(pMsg->pcMessage);
            }

            osPoolFree(AtMemPoolId, pMsg);
        }
    }

    return ret;
}

/*
 * @brief AT CMD task body
 *
 * @param [in] pvParameters Task parameters
 *
 */
void at_task_patch(void *pvParameters)
{
    osEvent rxEvent;
    xATMessage *rxMsg;
    at_uart_buffer_t *pData;

    tracer_log(LOG_HIGH_LEVEL, "AT task is created successfully! \r\n");
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
            //at_clear_uart_buffer();
        }

        if(rxMsg->pcMessage != NULL) free(rxMsg->pcMessage);
        osPoolFree (AtMemPoolId, rxMsg);
    }
}

/*
 * @brief An external Function at_task_cmd_process prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_cmd_process_fp_t at_task_cmd_process;

/*
 * @brief An external Function at_task_create_semaphore prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_create_semaphore_fp_t at_task_create_semaphore;

/*
 * @brief An external Function at_semaphore_release prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_semaphore_release_fp_t at_semaphore_release;

/*
 * @brief An external Function at_task_create prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_create_fp_t at_task_create;

/*
 * @brief An external Function at_task_send prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_send_fp_t at_task_send;

/*
 * @brief An external Function at_task prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_fp_t at_task;


/*
 * @brief An external Function at_module_init prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_module_init_fp_t at_module_init;



/*
 * @brief AT Command Interface Initialization for AT Command Task
 *
 */
void at_task_func_init_patch(void)
{
    at_task_send = at_task_send_patch;
    at_task = at_task_patch;
    at_module_init = at_module_init_patch;
}

