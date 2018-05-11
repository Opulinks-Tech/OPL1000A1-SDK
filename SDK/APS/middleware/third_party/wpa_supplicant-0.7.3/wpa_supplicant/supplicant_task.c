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
//#ifdef DEMO_TC
#include "tcpecho.h"
#include "le_host_cmd.h"
//#endif
#include "wpa_demo.h"

RET_DATA osMessageQId    xSupplicantQueue;
RET_DATA osThreadId      SupplicantTaskHandle;
RET_DATA osPoolId        supplicantMemPoolId;

extern struct wpa_supplicant *wpa_s;

//#ifdef DEMO_TC
RET_DATA int gDemoScanTimes;
//#endif

//void wpa_supplicant_event_assoc(struct wpa_supplicant *wpa_s, union wpa_event_data *data);
//void wpa_supplicant_event_disassoc(struct wpa_supplicant *wpa_s, u16 reason_code);
//void wpa_supplicant_event_eapol_rx(struct wpa_supplicant *wpa_s);


osPoolDef (supplicantMemPoolId, SUPP_QUEUE_SIZE, xSupplicantMessage_t); // memory pool object

void supplicant_task_create_impl(void)
{
    osThreadDef_t   task_def;
    osMessageQDef_t queue_def;

    //create task
    task_def.name = "supplicant";
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
    osStatus ret = osOK;
    xSupplicantMessage_t    *pMsg;

    //Mem pool allocate
    pMsg = (xSupplicantMessage_t *)osPoolCAlloc (supplicantMemPoolId);         // get Mem Block
    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    if(txMsg.length != 0)
    {
        //malloc buffer
        pMsg->pcMessage = (void *)malloc(txMsg.length);
        if(txMsg.pcMessage != NULL)
        {
            memcpy(pMsg->pcMessage, txMsg.pcMessage, txMsg.length);
        }
        else
        {
            ret = osErrorOS;
            msg_print(LOG_HIGH_LEVEL, "Supplicant task message allocate fail \r\n");
        }
    }
    if(ret == osOK)
        osMessagePut (xSupplicantQueue, (uint32_t)pMsg, osWaitForever); // Send Message

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

#if 0
#ifdef DEMO_TC
    xSupplicantMessage_t txMsg = {0};
    gDemoScanTimes = 0;
#endif
#endif

    //task init
    supplicant_task_init();

#if 0
#ifdef DEMO_TC
    txMsg.event = MLME_EVT_DEMO_SCAN_START;
    txMsg.length = 0;
    supplicant_task_send(txMsg);
#endif
#endif

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

void supplicant_task_evt_handle_impl(uint32_t evt_type){
#if 0
#ifdef DEMO_TC
    scan_info_t *pInfo = NULL;
    scan_info_t *pInfo2 = NULL;
    u8 expected_bssid[MAC_ADDR_LEN] = {0x80, 0x26, 0x89, 0x58, 0x64, 0xd3};
    int i;
    //char my_ssid[IEEE80211_MAX_SSID_LEN + 1] = "D-Link_DIR-600M-Michael";
#endif
#endif

	switch (evt_type)
    {
	    case MLME_EVT_ASSOC:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_ASSOC \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\nconnected\r\n\r\n");

            wpa_clr_key_info();

#ifdef WIFI_KEY_EXCHANGE
            wpa_supplicant_event_assoc(wpa_s, NULL);
#endif
            wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATED);

#if 0
#ifdef DEMO_TC
            msg_print(LOG_HIGH_LEVEL, "[DEMO]WPA: Start Socket Server \r\n");
            socket_server_demo();
#endif
#endif
            //For Demo
            wpa_demo_hdl_evt_assoc();

			break;
	    case MLME_EVT_DISASSOC:
            //sme_event_disassoc(wpa_s, data);
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_DISASSOC \r\n");
            wpa_supplicant_event_disassoc(wpa_s, WLAN_REASON_DEAUTH_LEAVING);
            wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
			break;
		case MLME_EVT_SCAN_RESULTS:
            //wpa_supplicant_event_scan_results(wpa_s, data);
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_SCAN_RESULTS \r\n");
            msg_print(LOG_HIGH_LEVEL, "WPA: scan done \r\n");
            wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
            wpa_cli_showscanresults_handler(NULL, NULL);

#if 0
#ifdef DEMO_TC
            //Check if bssid==80:26:89:58:64:d3 exist, if no, scan again
            pInfo = wifi_get_scan_record(expected_bssid);
            if(pInfo == NULL){ //scan again
                msg_print(LOG_HIGH_LEVEL, "[DEMO]WPA: No Scan Results \r\n");
                msg_print(LOG_HIGH_LEVEL, "[DEMO]WPA: Scan again \r\n");
                wpa_cli_scan_for_demo(0);
            } else {
                msg_print(LOG_HIGH_LEVEL, "[DEMO]WPA: Find Expected AP \r\n");
                msg_print(LOG_HIGH_LEVEL, "[DEMO]WPA: call ParseLeHostCommand(leh+act, 7) \r\n");
                ParseLeHostCommand("leh+act", 7);
            }
#endif
#endif
            //For Demo
            wpa_demo_hdl_evt_scan_results();

			break;
		case MLME_EVT_AUTH:
            //sme_event_auth(wpa_s, data);
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_AUTH \r\n");
            //Do nothing
			break;
		case MLME_EVT_DEAUTH:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_DEAUTH \r\n");
            //wpa_supplicant_event_disassoc(wpa_s, reason_code);
            //wpa_supplicant_event_disassoc(wpa_s, WLAN_REASON_DEAUTH_LEAVING);
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\ndisconnected \r\n\r\n");
            wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
            wpa_clr_key_info();

            //For Demo
            wpa_demo_hdl_evt_deauth();
			break;
        case MLME_EVT_AUTH_TIMED_OUT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_AUTH_TIMED_OUT \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\nconnect time out\r\n\r\n");
            wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
            break;
		case MLME_EVT_EAPOL_RX:
			msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_EAPOL_RX \r\n");
            //wpa_supplicant_set_state(wpa_s, WPA_4WAY_HANDSHAKE);
            msg_print(LOG_HIGH_LEVEL, "WPA: wpa_state:%d \r\n", wpa_s->wpa_state);
#ifdef WIFI_KEY_EXCHANGE
            if (wpa_s->wpa_state == WPA_ASSOCIATED ||
                wpa_s->wpa_state == WPA_4WAY_HANDSHAKE ||
                wpa_s->wpa_state == WPA_GROUP_HANDSHAKE ||
                wpa_s->wpa_state == WPA_COMPLETED)
            {
                wpa_supplicant_event_eapol_rx(wpa_s);
            }
#endif
            break;

//#ifdef DEMO_TC
        case MLME_EVT_DEMO_SCAN_START:
            msg_print(LOG_HIGH_LEVEL, "[DEMO][EVT]WPA: Event-MLME_EVT_DEMO_SCAN_START, times:%d \r\n", gDemoScanTimes);
            //wpa_cli_scan_for_demo(0);

            //For Demo
            wpa_demo_hdl_evt_demo_scan_start();
			break;
//#endif

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

