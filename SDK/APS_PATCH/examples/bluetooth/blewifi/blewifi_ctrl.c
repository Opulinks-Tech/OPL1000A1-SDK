
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
#include "blewifi_common.h"
#include "blewifi_ctrl.h"
#include "blewifi_api.h"
#include "blewifi_data.h"
#include "wifi_api.h"

osThreadId   BleWifiCtrlTaskId;
osMessageQId BleWifiCtrlQueueId;
osPoolId     BleWifiCtrlMemPoolId;

unsigned char g_BleWifiDataBuf[BLEWIFI_CTRL_MSG_MAX_LEN];

osPoolDef (BleWifiCtrlMemPoolId, BLEWIFI_CTRL_QUEUE_SIZE, xBleWifiCtrlMessage_t); // memory pool object

int wifi_scan_send_report(void);
int wifi_scan_report_complete(void);
int wifi_connect_complete(uint8_t *data, int len);
int wifi_disconnect_complete(uint8_t *data, int len);



void blewifi_ctrl_task_evt_handler(uint32_t evt_type, void *data, int len)
{
	switch (evt_type)
    {
	    case BLEWIFI_CTRL_MSG_BLE_INIT_COMPLETE: //BLE
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_INIT_COMPLETE \r\n");

            /* BLE Init Step1: Do BLE Advertising*/
            blewifi_ble_start_advertising();
			break;
        case BLEWIFI_CTRL_MSG_BLE_ADVERTISING_CMF: //BLE
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_ADVERTISING_CMF \r\n");

            /* BLE Init Step 3: BLE is ready for peer BLE device's connection trigger */
            break;
        case BLEWIFI_CTRL_MSG_BLE_CONNECTION_COMPLETE: //BLE
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_CONNECTION_COMPLETE \r\n");

            /* BLE Init Step 4: BLE said it's connected with a peer BLE device */
            break;
        case BLEWIFI_CTRL_MSG_BLE_DISCONNECT: //BLE
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_DISCONNECT \r\n");

            blewifi_ble_start_advertising();
            break;

        case BLEWIFI_CTRL_MSG_BLE_DATA_IND:   //BLE GATTS Write EVENT
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_DATA_IND \r\n");
            blewifi_data_recv_handler(data, len);
            break;

        case BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND: //Wi-Fi
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND \r\n");
            wifi_scan_send_report();
            wifi_scan_report_complete();
            break;

        case BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND: //Wi-Fi
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND \r\n");
            wifi_connect_complete(data, len);
            break;

        case BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND: //Wi-Fi
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND \r\n");
            wifi_disconnect_complete(data, len);
            break;

        case BLEWIFI_CTRL_MSG_WIFI_SCAN_RESULTS_SEND: //Wi-Fi
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_SCAN_RESULTS_SEND \r\n");
            break;

		default:
			break;
	}
}

void blewifi_ctrl_task(void *args)
{
    osEvent rxEvent;
    xBleWifiCtrlMessage_t *rxMsg;

    for(;;)
    {
        /* Wait event */
        rxEvent = osMessageGet(BleWifiCtrlQueueId, osWaitForever);
        if(rxEvent.status != osEventMessage)
            continue;

        rxMsg = (xBleWifiCtrlMessage_t *)rxEvent.value.p;
        blewifi_ctrl_task_evt_handler(rxMsg->event, rxMsg->pcMessage, rxMsg->length);

        /* Release buffer */
        if(rxMsg->pcMessage != NULL)
            free(rxMsg->pcMessage);
        osPoolFree (BleWifiCtrlMemPoolId, rxMsg);
    }
}

void blewifi_ctrl_task_create(void)
{
    osThreadDef_t task_def;
    osMessageQDef_t blewifi_queue_def;

    /* Create ble-wifi task */
    task_def.name = "blewifi ctrl";
    task_def.stacksize = OS_TASK_STACK_SIZE_APP;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = blewifi_ctrl_task;
    BleWifiCtrlTaskId = osThreadCreate(&task_def, (void*)NULL);
    if(BleWifiCtrlTaskId == NULL)
    {
        BLEWIFI_INFO("BLEWIFI: ctrl task create fail \r\n");
    }
    else
    {
        BLEWIFI_INFO("BLEWIFI: ctrl task create successful \r\n");
    }

    /* create memory pool */
    BleWifiCtrlMemPoolId = osPoolCreate(osPool(BleWifiCtrlMemPoolId));
    if (!BleWifiCtrlMemPoolId)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task mem pool create fail \r\n");
    }

    /* Create message queue*/
    blewifi_queue_def.item_sz = sizeof(xBleWifiCtrlMessage_t);
    blewifi_queue_def.queue_sz = BLEWIFI_CTRL_QUEUE_SIZE;
    BleWifiCtrlQueueId = osMessageCreate(&blewifi_queue_def, NULL);
    if(BleWifiCtrlQueueId == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task create queue fail \r\n");
    }
}

int blewifi_ctrl_task_send(xBleWifiCtrlMessage_t *txMsg)
{
    int iRet = -1;
    xBleWifiCtrlMessage_t *pMsg = NULL;

    if (txMsg == NULL)
        goto done;

    /* Mem pool allocate */
    pMsg = (xBleWifiCtrlMessage_t *)osPoolCAlloc(BleWifiCtrlMemPoolId);

    if(pMsg == NULL)
    {
        goto done;
    }

    pMsg->event = txMsg->event;
    pMsg->length = txMsg->length;
    pMsg->pcMessage = NULL;

    if((txMsg->pcMessage) && (txMsg->length))
    {
        /* Malloc buffer */
        pMsg->pcMessage = (void *)malloc(txMsg->length);

        if(pMsg->pcMessage != NULL)
        {
            memcpy(pMsg->pcMessage, txMsg->pcMessage, txMsg->length);
        }
        else
        {
            BLEWIFI_ERROR("BLEWIFI: ctrl task message allocate fail \r\n");
            goto done;
        }
    }

    if (osMessagePut(BleWifiCtrlQueueId, (uint32_t)pMsg, osWaitForever) != osOK)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task message send fail \r\n");
        goto done;
    }

    iRet = 0;

done:
    if(iRet)
    {
        if(pMsg)
        {
            if(pMsg->pcMessage)
            {
                free(pMsg->pcMessage);
            }

            osPoolFree(BleWifiCtrlMemPoolId, pMsg);
        }
    }

    return iRet;
}

int blewifi_ctrl_msg_send(int msg_type, uint8_t *data, int data_len)
{
    xBleWifiCtrlMessage_t txMsg = {0};

    txMsg.event = msg_type;
    txMsg.length = data_len;
    txMsg.pcMessage = data;

    return (blewifi_ctrl_task_send(&txMsg));
}

int wifi_connection_repeat_connect(wifi_config_t *config,int repeat_times)
{
	  int ret = 1;
	  /*
	  int i; 
	  for (i=0;i<repeat_times;i++)
	  {
	      ret = wifi_connection_connect(config);
			  if (ret == 0)
					break;  // connect is successful
		}
	  */
    ret = wifi_connection_connect(config);		
		return ret;
}


