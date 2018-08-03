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
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "msg.h"
#include "supplicant_task.h"
#include "driver.h"
#include "controller_wifi_com.h"
#include "wpa_common.h"
#include "config.h"
#include "wpa_cli.h"
#include "wpa_supplicant_i.h"
#include "common.h"
#include "wpa.h"
#include "events_netlink.h"
#include "sys_os_config.h"
#include "at_cmd_common.h"
#include "tcpecho.h"
#include "wpa_demo.h"
#include "at_cmd_msg_ext.h"

RET_DATA osMessageQId    xSupplicantQueue;
RET_DATA osThreadId      SupplicantTaskHandle;
RET_DATA osPoolId        supplicantMemPoolId;

extern struct wpa_supplicant *wpa_s;
extern auto_connect_cfg_t g_AutoConnect; //Fast Connect Report

osPoolDef (supplicantMemPoolId, SUPP_QUEUE_SIZE, xSupplicantMessage_t); // memory pool object

void supplicant_task_create_impl(void)
{
    osThreadDef_t   task_def;
    osMessageQDef_t queue_def;

    //create task
    task_def.name = OS_TASK_NAME_SUPPLICANT;
    task_def.stacksize = OS_TASK_STACK_SIZE_SUPPLICANT;
    task_def.tpriority = OS_TASK_PRIORITY_SUPPLICANT;
    task_def.pthread = supplicant_task;
    SupplicantTaskHandle = osThreadCreate(&task_def, (void *)SupplicantTaskHandle);
    if(SupplicantTaskHandle == NULL)
    {
        msg_print(LOG_HIGH_LEVEL, "Supplicant task is created failed! \r\n");
    }
    else
    {
        msg_print(LOG_HIGH_LEVEL, "Supplicant task is created successfully! \r\n");
    }

    //create memory pool
    supplicantMemPoolId = osPoolCreate (osPool(supplicantMemPoolId)); // create Mem Pool
    if (!supplicantMemPoolId)
    {
        msg_print(LOG_HIGH_LEVEL, "Supplicant Task Mem Pool create Fail \r\n"); // MemPool object not created, handle failure
    }

    //create queue
    queue_def.item_sz = sizeof( xSupplicantMessage_t );
    queue_def.queue_sz = SUPP_QUEUE_SIZE;
    xSupplicantQueue = osMessageCreate(&queue_def, SupplicantTaskHandle);
    if(xSupplicantQueue == NULL)
    {
        msg_print(LOG_HIGH_LEVEL, "Supplicant Task Queue fail \r\n");
    }
}

void supplicant_task_init_impl(void)
{
    msg_print(LOG_HIGH_LEVEL, "supplicant_task_init entry \r\n");
}

osStatus supplicant_task_send_impl(xSupplicantMessage_t txMsg)
{
    osStatus ret = osErrorOS;
    xSupplicantMessage_t    *pMsg = NULL;

    //Mem pool allocate
    pMsg = (xSupplicantMessage_t *)osPoolCAlloc (supplicantMemPoolId);         // get Mem Block

    if(pMsg == NULL)
    {
        goto done;
    }

    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    pMsg->pcMessage = NULL;

    if((txMsg.pcMessage) && (txMsg.length))
    {
        //malloc buffer
        pMsg->pcMessage = (void *)malloc(txMsg.length);

        if(pMsg->pcMessage == NULL)
        {
            msg_print(LOG_HIGH_LEVEL, "Supplicant task message allocate fail \r\n");
            goto done;
        }

        memcpy(pMsg->pcMessage, txMsg.pcMessage, txMsg.length);
    }

    if(osMessagePut (xSupplicantQueue, (uint32_t)pMsg, osWaitForever) != osOK) // Send Message
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

            osPoolFree(supplicantMemPoolId, pMsg);
        }
    }

    return ret;
}

/*
 supervisor task
*/
void supplicant_task_impl( void *pvParameters )
{
    osEvent         rxEvent;
    xSupplicantMessage_t    *rxMsg;
    //xDiagMessage    txMsg;

    //task init
    supplicant_task_init();

    //For Demo
    wpa_demo_send_msg_to_wpas();

    for(;;)
    {
        //wait event
        rxEvent = osMessageGet(xSupplicantQueue, osWaitForever);
        if(rxEvent.status != osEventMessage)
            continue;

		rxMsg = (xSupplicantMessage_t *) rxEvent.value.p;
		supplicant_task_evt_handle(rxMsg->event);

        /*release buffer*/
        if(rxMsg->pcMessage != NULL)
            free(rxMsg->pcMessage);
        osPoolFree (supplicantMemPoolId, rxMsg);
    }
}

void supplicant_task_evt_handle_impl(uint32_t evt_type)
{
	switch (evt_type)
    {
	    case MLME_EVT_ASSOC:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_ASSOC \r\n\r\n");
            msg_print(LOG_HIGH_LEVEL, "connected\r\n\r\n");
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, MSG_WIFI_CONNECTED_OPEN);
            wpa_clr_key_info();
            wpa_supplicant_event_assoc(wpa_s, NULL);
            wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATED);
			break;

	    case MLME_EVT_DISASSOC:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_DISASSOC \r\n");
            wpa_supplicant_event_disassoc(wpa_s, WLAN_REASON_DEAUTH_LEAVING);
            wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }            
			break;

		case MLME_EVT_SCAN_RESULTS:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_SCAN_RESULTS \r\n");
            msg_print(LOG_HIGH_LEVEL, "WPA: scan done \r\n");
            if (wpa_s->wpa_state != WPA_COMPLETED) {
                wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
            }
            wpa_cli_showscanresults_handler(NULL, NULL);
			break;

		case MLME_EVT_AUTH:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_AUTH \r\n");
			break;

		case MLME_EVT_DEAUTH:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_DEAUTH \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\ndisconnected \r\n\r\n");
            wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
            wpa_clr_key_info();
            _at_msg_ext_wifi_connect(AT_MSG_EXT_ESPRESSIF, MSG_WIFI_DISCONNECTED);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }            
			break;

        case MLME_EVT_AUTH_TIMED_OUT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_AUTH_TIMED_OUT \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\nconnect time out\r\n\r\n");
            wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
            at_msg_ext_wifi_err(AT_MSG_EXT_ESPRESSIF, "+CWJAP", ERR_WIFI_CWJAP_TO);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }            
            break;

		case MLME_EVT_EAPOL_RX:
			//msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_EAPOL_RX \r\n");
            //msg_print(LOG_HIGH_LEVEL, "WPA: wpa_state:%d \r\n", wpa_s->wpa_state);
            if (wpa_s->wpa_state == WPA_ASSOCIATED ||
                wpa_s->wpa_state == WPA_4WAY_HANDSHAKE ||
                wpa_s->wpa_state == WPA_GROUP_HANDSHAKE ||
                wpa_s->wpa_state == WPA_COMPLETED)
            {
                wpa_supplicant_event_eapol_rx(wpa_s);
            }
            break;

        case MLME_EVT_ASSOC_TIMED_OUT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_ASSOC_TIMED_OUT \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\nconnect time out\r\n\r\n");
            at_msg_ext_wifi_err(AT_MSG_EXT_ESPRESSIF, "+CWJAP", ERR_WIFI_CWJAP_TO);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }            
            break;

        case MLME_EVT_ASSOC_REJECT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_ASSOC_REJECT \r\n");
            at_msg_ext_wifi_err(AT_MSG_EXT_ESPRESSIF, "+CWJAP", ERR_WIFI_CWJAP_FAIL);\
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }            
            break;
        case MLME_EVT_AUTO_CONNECT:
            g_AutoConnect.retryCount++;
            control_auto_connect();
            break;            
        case MLME_EVT_AUTO_CONNECT_START:
            msg_print(LOG_HIGH_LEVEL, "Supplicant Receive EVT_AUTO_CONNECT_START \r\n");
            control_auto_connect();
            break;

		default:
			break;
	}
}

RET_DATA supplicant_task_create_fp_t supplicant_task_create;
RET_DATA supplicant_task_init_fp_t supplicant_task_init;
RET_DATA supplicant_task_fp_t supplicant_task;
RET_DATA supplicant_task_send_fp_t supplicant_task_send;
RET_DATA supplicant_task_evt_handle_fp_t supplicant_task_evt_handle;

/*
   Interface Initialization: Supplicant Task
 */
void wpa_supplicant_task_func_init(void)
{
    supplicant_task_create = supplicant_task_create_impl;
    supplicant_task_init = supplicant_task_init_impl;
    supplicant_task = supplicant_task_impl;
    supplicant_task_send = supplicant_task_send_impl;
    supplicant_task_evt_handle = supplicant_task_evt_handle_impl;
}

