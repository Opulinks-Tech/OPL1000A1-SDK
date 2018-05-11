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
 * @file blewifi_ctrl.c
 * @author Vincent Chen, Michael Liao
 * @date 20 Feb 2018
 * @brief File creates the blewifi ctrl task architecture.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "lwip_helper.h"
#include "wifi_types.h"
#include "blewifi_user_app.h"

osThreadId   BleWifiUserAppTaskId;
osMessageQId BleWifiUserAppQueueId;
osPoolId     BleWifiUserAppMemPoolId;

osPoolDef (BleWifiUserAppMemPoolId, BLEWIFI_USER_APP_QUEUE_SIZE, xBleWifiUserAppMessage_t); // memory pool object

void blewifi_user_app_task_evt_handle(uint32_t evt_type)
{
	switch (evt_type)
    {
	    case BLEWIFI_USER_APP_MSG_RSV1:
        case BLEWIFI_USER_APP_MSG_RSV2:
			break;

		default:
			break;
	}
}

void blewifi_user_app_task(void *args)
{
    osEvent rxEvent;
    xBleWifiUserAppMessage_t *rxMsg;

    /* Tcpip stack and net interface initialization,  dhcp client process initialization. */
    lwip_network_init(WIFI_MODE_STA);
    lwip_net_start(WIFI_MODE_STA);

    /* Waiting for connection & got IP from DHCP server */
    lwip_net_ready();

    for(;;)
    {
        /* Wait event */
        rxEvent = osMessageGet(BleWifiUserAppQueueId, osWaitForever);
        if(rxEvent.status != osEventMessage)
            continue;

        rxMsg = (xBleWifiUserAppMessage_t *) rxEvent.value.p;
        blewifi_user_app_task_evt_handle(rxMsg->event);

        /* Release buffer */
        if(rxMsg->pcMessage != NULL)
            free(rxMsg->pcMessage);
        osPoolFree (BleWifiUserAppMemPoolId, rxMsg);
    }
}

void blewifi_user_app_task_create(void)
{
    osThreadDef_t task_def;
    osMessageQDef_t user_app_queue_def;

    /* Create user app task */
    task_def.name = "blewifi user app";
    task_def.stacksize = OS_TASK_STACK_SIZE_APP;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = blewifi_user_app_task;
    BleWifiUserAppTaskId = osThreadCreate(&task_def, (void*)NULL);
    if(BleWifiUserAppTaskId == NULL)
    {
        printf("BLEWIFI: user app task create fail \r\n");
    }
    else
    {
        printf("BLEWIFI: user app task create successful \r\n");
    }

    /* create memory pool for user app task */
    BleWifiUserAppMemPoolId = osPoolCreate (osPool(BleWifiUserAppMemPoolId));
    if (!BleWifiUserAppMemPoolId)
    {
        msg_print(LOG_HIGH_LEVEL, "BLEWIFI: user app task mem pool create fail \r\n");
    }

    /* Create message queue for user app task */
    user_app_queue_def.item_sz = sizeof(xBleWifiUserAppMessage_t);
    user_app_queue_def.queue_sz = BLEWIFI_USER_APP_QUEUE_SIZE;
    BleWifiUserAppQueueId = osMessageCreate(&user_app_queue_def, NULL);
    if(BleWifiUserAppQueueId == NULL)
    {
        printf("BLEWIFI: user app task create queue fail \r\n");
    }
}

osStatus blewifi_user_app_task_send(xBleWifiUserAppMessage_t txMsg)
{
    osStatus ret = osOK;
    xBleWifiUserAppMessage_t *pMsg;

    /* Mem pool allocate */
    pMsg = (xBleWifiUserAppMessage_t *)osPoolCAlloc(BleWifiUserAppMemPoolId);
    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    if(txMsg.length != 0)
    {
        /* Malloc buffer */
        pMsg->pcMessage = (void *)malloc(txMsg.length);

        if(txMsg.pcMessage != NULL)
        {
            memcpy(pMsg->pcMessage, txMsg.pcMessage, txMsg.length);
        }
        else
        {
            ret = osErrorOS;
            msg_print(LOG_HIGH_LEVEL, "BLEWIFI: user app task message allocate fail \r\n");
        }
    }
    if(ret == osOK)
        osMessagePut(BleWifiUserAppQueueId, (uint32_t)pMsg, osWaitForever);

    return ret;
}

