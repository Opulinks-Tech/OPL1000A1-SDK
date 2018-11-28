/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
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
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "sys_os_config_patch.h"

#include "blewifi_common.h"
#include "blewifi_configuration.h"
#include "blewifi_ctrl.h"
#include "blewifi_wifi_api.h"
#include "blewifi_ble_api.h"
#include "blewifi_data.h"
#include "blewifi_app.h"
#include "mw_ota_def.h"
#include "mw_ota.h"
#include "hal_system.h"

#define BLEWIFI_CTRL_RESET_DELAY    (3000)  // ms

osThreadId   g_tAppCtrlTaskId;
osPoolId     g_tAppCtrlMemPoolId;
osMessageQId g_tAppCtrlQueueId;
osTimerId    g_tAppCtrlAutoConnectTriggerTimer;

uint8_t g_ubAppCtrlBleStatus;     //true:BLE is connected false:BLE is idle
uint8_t g_ubAppCtrlWifiStatus;    //true:Wifi is connected false:Wifi is idle
uint8_t g_ubAppCtrlOtaStatus;

uint8_t g_ubAppCtrlRequestRetryTimes;
uint32_t g_ulAppCtrlAutoConnectInterval;

void BleWifi_Ctrl_BleStatusSet(uint8_t status)
{
    g_ubAppCtrlBleStatus = status;
}

uint8_t BleWifi_Ctrl_BleStatusGet(void)
{
    return g_ubAppCtrlBleStatus;
}

void BleWifi_Ctrl_WifiStatusSet(uint8_t status)
{
    g_ubAppCtrlWifiStatus = status;
}

uint8_t BleWifi_Ctrl_WifiStatusGet(void)
{
    return g_ubAppCtrlWifiStatus;
}

void BleWifi_Ctrl_OtaStatusSet(uint8_t status)
{
    g_ubAppCtrlOtaStatus = status;
}

uint8_t BleWifi_Ctrl_OtaStatusGet(void)
{
    return g_ubAppCtrlOtaStatus;
}

void BleWifi_Ctrl_DoAutoConnect(void)
{
    uint8_t data[2];

    // if the count of auto-connection list is empty, don't do the auto-connect
    if (0 == BleWifi_Wifi_AutoConnectListNum())
        return;

    // if request connect by Peer side, don't do the auto-connection
    if (g_ubAppCtrlRequestRetryTimes <= BLEWIFI_WIFI_REQ_CONNECT_RETRY_TIMES)
        return;

    // BLE is disconnect and Wifi is disconnect, too.
    if ((false == BleWifi_Ctrl_BleStatusGet()) && (false == BleWifi_Ctrl_WifiStatusGet()))
    {
        // start to scan
        // after scan, do the auto-connect
        if (g_ubAppCtrlRequestRetryTimes == BLEWIFI_CTRL_AUTO_CONN_STATE_IDLE)
        {
            data[0] = 1;    // Enable to scan AP whose SSID is hidden
            data[1] = 2;    // mixed mode
            BleWifi_Wifi_DoScan(data, 2);

            g_ubAppCtrlRequestRetryTimes = BLEWIFI_CTRL_AUTO_CONN_STATE_SCAN;
        }
    }
}

void BleWifi_Ctrl_AutoConnectTrigger(void const *argu)
{
    BleWifi_Ctrl_MsgSend(BLEWIFI_CTRL_MSG_WIFI_AUTO_CONNECT_IND, NULL, 0);
}

void BleWifi_Ctrl_TaskEvtHandler(uint32_t evt_type, void *data, int len)
{
	switch (evt_type)
    {
        // BLE
	    case BLEWIFI_CTRL_MSG_BLE_INIT_COMPLETE:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_INIT_COMPLETE \r\n");

            /* BLE Init Step 2: Do BLE Advertising*/
            BleWifi_Ble_StartAdvertising();
			break;

        case BLEWIFI_CTRL_MSG_BLE_ADVERTISING_CFM:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_ADVERTISING_CFM \r\n");

            /* BLE Init Step 3: BLE is ready for peer BLE device's connection trigger */
            break;

        case BLEWIFI_CTRL_MSG_BLE_ADVERTISING_EXIT_CFM:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_ADVERTISING_EXIT_CFM \r\n");
            break;

        case BLEWIFI_CTRL_MSG_BLE_CONNECTION_COMPLETE:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_CONNECTION_COMPLETE \r\n");
            BleWifi_Ctrl_BleStatusSet(true);

            /* BLE Init Step 4: BLE said it's connected with a peer BLE device */
            break;

        case BLEWIFI_CTRL_MSG_BLE_CONNECTION_FAIL:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_CONNECTION_FAIL \r\n");
            BleWifi_Ble_StartAdvertising();
            break;

        case BLEWIFI_CTRL_MSG_BLE_DISCONNECT:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_DISCONNECT \r\n");
            BleWifi_Ctrl_BleStatusSet(false);
            BleWifi_Ble_StartAdvertising();
            
            /* start to do auto-connection. */
            g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_INIT;
            BleWifi_Ctrl_DoAutoConnect();

            /* stop the OTA behavior */
            if (gTheOta)
            {
                MwOta_DataGiveUp();
                free(gTheOta);
                gTheOta = 0;

                BleWifi_Ctrl_MsgSend(BLEWIFI_CTRL_MSG_OTHER_OTA_OFF_FAIL, NULL, 0);
            }
            break;

        case BLEWIFI_CTRL_MSG_BLE_DATA_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_BLE_DATA_IND \r\n");
            BleWifi_Ble_DataRecvHandler(data, len);
            break;

        //Wi-Fi
        case BLEWIFI_CTRL_MSG_WIFI_INIT_COMPLETE:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_INIT_COMPLETE \r\n");

            /* When device power on, start to do auto-connection. */
            g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_INIT;
            BleWifi_Ctrl_DoAutoConnect();
            break;
            
        case BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND \r\n");
            // scan by auto-connect
            if (g_ubAppCtrlRequestRetryTimes == BLEWIFI_CTRL_AUTO_CONN_STATE_SCAN)
            {
                BleWifi_Wifi_UpdateScanInfoToAutoConnList();
                BleWifi_Wifi_DoAutoConnect();
                g_ulAppCtrlAutoConnectInterval = g_ulAppCtrlAutoConnectInterval + BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_DIFF;
                if (g_ulAppCtrlAutoConnectInterval > BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_MAX)
                    g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_MAX;

                g_ubAppCtrlRequestRetryTimes = BLEWIFI_CTRL_AUTO_CONN_STATE_IDLE;
            }
            // scan by user
            else
            {
                BleWifi_Wifi_SendScanReport();
                BleWifi_Ble_SendResponse(BLEWIFI_RSP_SCAN_END, 0);
            }
            break;

        case BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND \r\n");
            BleWifi_Ctrl_WifiStatusSet(true);
            
            // return to the idle of the connection retry
            g_ubAppCtrlRequestRetryTimes = BLEWIFI_CTRL_AUTO_CONN_STATE_IDLE;
            g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_INIT;
            BleWifi_Ble_SendResponse(BLEWIFI_RSP_CONNECT, BLEWIFI_WIFI_CONNECTED_DONE);
            break;

        case BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND \r\n");
            BleWifi_Ctrl_WifiStatusSet(false);
            BleWifi_Wifi_SetDTIM(0);

            // continue the connection retry
            if (g_ubAppCtrlRequestRetryTimes < BLEWIFI_WIFI_REQ_CONNECT_RETRY_TIMES)
            {
                BleWifi_Wifi_ReqConnectRetry();
                g_ubAppCtrlRequestRetryTimes++;
            }
            // stop the connection retry
            else if (g_ubAppCtrlRequestRetryTimes == BLEWIFI_WIFI_REQ_CONNECT_RETRY_TIMES)
            {
                // return to the idle of the connection retry
                g_ubAppCtrlRequestRetryTimes = BLEWIFI_CTRL_AUTO_CONN_STATE_IDLE;
                g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_INIT;
                BleWifi_Ble_SendResponse(BLEWIFI_RSP_CONNECT, BLEWIFI_WIFI_CONNECTED_FAIL);

                /* do auto-connection. */
                if (false == BleWifi_Ctrl_BleStatusGet())
                {
                    osTimerStop(g_tAppCtrlAutoConnectTriggerTimer);
                    osTimerStart(g_tAppCtrlAutoConnectTriggerTimer, g_ulAppCtrlAutoConnectInterval);
                }
            }
            else
            {
                BleWifi_Ble_SendResponse(BLEWIFI_RSP_DISCONNECT, BLEWIFI_WIFI_DISCONNECTED_DONE);
            
                /* do auto-connection. */
                if (false == BleWifi_Ctrl_BleStatusGet())
                {
                    osTimerStop(g_tAppCtrlAutoConnectTriggerTimer);
                    osTimerStart(g_tAppCtrlAutoConnectTriggerTimer, g_ulAppCtrlAutoConnectInterval);
                }
            }
            break;

        case BLEWIFI_CTRL_MSG_WIFI_GOT_IP_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_GOT_IP_IND \r\n");
            BleWifi_Wifi_UpdateBeaconInfo();
            BleWifi_Wifi_SetDTIM(BLEWIFI_WIFI_DTIM_INTERVAL);
            BleWifi_Wifi_SendStatusInfo(BLEWIFI_IND_IP_STATUS_NOTIFY);
            break;

        case BLEWIFI_CTRL_MSG_WIFI_AUTO_CONNECT_IND:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_WIFI_AUTO_CONNECT_IND \r\n");
            BleWifi_Ctrl_DoAutoConnect();
            break;

        // Others
        case BLEWIFI_CTRL_MSG_OTHER_OTA_ON:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_OTHER_OTA_ON \r\n");
            BleWifi_Ctrl_OtaStatusSet(true);
            break;

        case BLEWIFI_CTRL_MSG_OTHER_OTA_OFF:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_OTHER_OTA_OFF \r\n");
            BleWifi_Ctrl_OtaStatusSet(false);

            // restart the system
            osDelay(BLEWIFI_CTRL_RESET_DELAY);
            Hal_Sys_SwResetAll();
            break;

        case BLEWIFI_CTRL_MSG_OTHER_OTA_OFF_FAIL:
            BLEWIFI_INFO("BLEWIFI: MSG BLEWIFI_CTRL_MSG_OTHER_OTA_OFF_FAIL \r\n");
            BleWifi_Ctrl_OtaStatusSet(false);
            break;

		default:
			break;
	}
}

void BleWifi_Ctrl_Task(void *args)
{
    osEvent rxEvent;
    xBleWifiCtrlMessage_t *rxMsg;

    for(;;)
    {
        /* Wait event */
        rxEvent = osMessageGet(g_tAppCtrlQueueId, osWaitForever);
        if(rxEvent.status != osEventMessage)
            continue;

        rxMsg = (xBleWifiCtrlMessage_t *)rxEvent.value.p;
        BleWifi_Ctrl_TaskEvtHandler(rxMsg->event, rxMsg->pcMessage, rxMsg->length);

        /* Release buffer */
        if(rxMsg->pcMessage != NULL)
            free(rxMsg->pcMessage);
            
        osPoolFree(g_tAppCtrlMemPoolId, rxMsg);
    }
}

int BleWifi_Ctrl_MsgSend(int msg_type, uint8_t *data, int data_len)
{
    xBleWifiCtrlMessage_t *pMsg = 0;

	if ((NULL == g_tAppCtrlMemPoolId) || (NULL == g_tAppCtrlQueueId))
	{
        BLEWIFI_ERROR("BLEWIFI: No pool or queue \r\n");
        return -1;
	}
    
    /* Mem pool allocate */
    pMsg = (xBleWifiCtrlMessage_t *)osPoolCAlloc(g_tAppCtrlMemPoolId);

    if (!pMsg) 
	{
        BLEWIFI_ERROR("BLEWIFI: ctrl task pmsg allocate fail \r\n");
	    goto error;
    }
    
    pMsg->event = msg_type;
    pMsg->length = data_len;
    pMsg->pcMessage = 0;
    
    if(data_len != 0)
    {
        /* Malloc buffer */
        pMsg->pcMessage = (void *)malloc(data_len);

		if (!pMsg->pcMessage) 
		{
            BLEWIFI_ERROR("BLEWIFI: ctrl task message allocate fail \r\n");
	        goto error;
        }

        memcpy(pMsg->pcMessage, data, data_len);
    }

    if (osMessagePut(g_tAppCtrlQueueId, (uint32_t)pMsg, osWaitForever) != osOK)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task message send fail \r\n");
        goto error;
    }

    return 0;

error:

	if (pMsg)
	{
		if (pMsg->pcMessage)
		    free(pMsg->pcMessage);

		osPoolFree(g_tAppCtrlMemPoolId, pMsg);
    }
	
	return -1;
}

void BleWifi_Ctrl_Init(void)
{
    osThreadDef_t task_def;
    osPoolDef_t tMemPoolDef;
    osMessageQDef_t blewifi_queue_def;
    osTimerDef_t timer_auto_connect_def;

    /* Create ble-wifi task */
    task_def.name = "blewifi ctrl";
    task_def.stacksize = OS_TASK_STACK_SIZE_BLEWIFI_CTRL;
    task_def.tpriority = OS_TASK_PRIORITY_APP;
    task_def.pthread = BleWifi_Ctrl_Task;
    g_tAppCtrlTaskId = osThreadCreate(&task_def, (void*)NULL);
    if(g_tAppCtrlTaskId == NULL)
    {
        BLEWIFI_INFO("BLEWIFI: ctrl task create fail \r\n");
    }
    else
    {
        BLEWIFI_INFO("BLEWIFI: ctrl task create successful \r\n");
    }

    /* create memory pool */
    tMemPoolDef.pool_sz = BLEWIFI_CTRL_QUEUE_SIZE;          // number of items (elements) in the pool
    tMemPoolDef.item_sz = sizeof(xBleWifiCtrlMessage_t);    // size of an item
    tMemPoolDef.pool = NULL;                                // reserved, it is no used
    g_tAppCtrlMemPoolId = osPoolCreate(&tMemPoolDef);
    if (g_tAppCtrlMemPoolId == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task mem pool create fail \r\n");
    }

    /* Create message queue*/
    blewifi_queue_def.item_sz = sizeof(xBleWifiCtrlMessage_t);
    blewifi_queue_def.queue_sz = BLEWIFI_CTRL_QUEUE_SIZE;
    g_tAppCtrlQueueId = osMessageCreate(&blewifi_queue_def, NULL);
    if(g_tAppCtrlQueueId == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task create queue fail \r\n");
    }

    /* creat timer to trig auto connect */
    timer_auto_connect_def.ptimer = BleWifi_Ctrl_AutoConnectTrigger;
    g_tAppCtrlAutoConnectTriggerTimer = osTimerCreate(&timer_auto_connect_def, osTimerOnce, NULL);
    if(g_tAppCtrlAutoConnectTriggerTimer == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: ctrl task create auto-connection timer fail \r\n");
    }

    /* the init state of BLE is idle */
    g_ubAppCtrlBleStatus = false;
    /* the init state of Wifi is idle */
    g_ubAppCtrlWifiStatus = false;
    /* the init state of OTA is idle */
    g_ubAppCtrlOtaStatus = false;

    // the idle of the connection retry
    g_ubAppCtrlRequestRetryTimes = BLEWIFI_CTRL_AUTO_CONN_STATE_IDLE;
    g_ulAppCtrlAutoConnectInterval = BLEWIFI_WIFI_AUTO_CONNECT_INTERVAL_INIT;
}
