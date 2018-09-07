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
#include "wpabuf.h"
#include "common.h"
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
//#include "tcpecho.h"
//#include "le_host_cmd.h"
//#endif
//#include "wpa_demo.h"
#include "supplicant_task_patch.h"
#include "at_cmd_msg_ext.h"
#include "at_cmd_msg_ext.h"
#include "controller_wifi_com_patch.h"
#include "sys_os_config_patch.h"

extern RET_DATA osMessageQId    xSupplicantQueue;
extern RET_DATA osThreadId      SupplicantTaskHandle;
extern RET_DATA osPoolId        supplicantMemPoolId;

extern struct wpa_supplicant *wpa_s;
extern auto_connect_cfg_t g_AutoConnect; //Fast Connect Report

//osPoolDef (supplicantMemPoolId, SUPP_QUEUE_SIZE, xSupplicantMessage_t); // memory pool object
extern const osPoolDef_t os_pool_def_supplicantMemPoolId;

void supplicant_task_evt_handle_patch(uint32_t evt_type)
{
    hap_control_t *hap_temp;
    hap_temp=get_hap_control_struct();
	switch (evt_type)
    {
	    case MLME_EVT_ASSOC:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_ASSOC \r\n\r\n");
            msg_print(LOG_HIGH_LEVEL, "connected\r\n\r\n");
            wpa_clr_key_info();
            wpa_supplicant_event_assoc(wpa_s, NULL);
            wpa_supplicant_set_state(wpa_s, WPA_ASSOCIATED);
            if(hap_temp->hap_en && (hap_temp->hap_ap_info->rsn_ie[0]==0) && (hap_temp->hap_ap_info->wpa_ie[0]==0) )
            {
               hiddenap_complete();
            }
			break;

	    case MLME_EVT_DISASSOC:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_DISASSOC \r\n");
            wpa_supplicant_event_disassoc(wpa_s, WLAN_REASON_DEAUTH_LEAVING);
            wpa_supplicant_set_state(wpa_s, WPA_DISCONNECTED);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL ||
                get_auto_connect_mode() == AUTO_CONNECT_DIRECT) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }
			break;

		case MLME_EVT_SCAN_RESULTS:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-EVENT_SCAN_RESULTS \r\n");
            msg_print(LOG_HIGH_LEVEL, "WPA: scan done \r\n");
            if (!(wpa_s->wpa_state == WPA_COMPLETED || wpa_s->wpa_state == WPA_ASSOCIATED)) {
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
            /* Set successfully connect info to Auto Connect list */
            if ((get_auto_connect_mode() == AUTO_CONNECT_MANUAL ||
                 get_auto_connect_mode() == AUTO_CONNECT_DIRECT) && 
                !get_repeat_conn()) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }
			break;

        case MLME_EVT_AUTH_TIMED_OUT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_AUTH_TIMED_OUT \r\n");
            msg_print(LOG_HIGH_LEVEL, "\r\n\r\nconnect time out\r\n\r\n");
            wpa_supplicant_set_state(wpa_s, WPA_INACTIVE);
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL ||
                get_auto_connect_mode() == AUTO_CONNECT_DIRECT) {
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
            /* Set successfully connect info to Auto Connect list */
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL ||
                get_auto_connect_mode() == AUTO_CONNECT_DIRECT) {
                set_auto_connect_mode(AUTO_CONNECT_ENABLE);
            }
            break;

        case MLME_EVT_ASSOC_REJECT:
            msg_print(LOG_HIGH_LEVEL, "[EVT]WPA: Event-MLME_EVT_ASSOC_REJECT \r\n");
            /* Set successfully connect info to Auto Connect list */
            if (hap_temp->hap_en){
                wifi_sta_join_for_hiddenap();
                break;
            }
            if (get_auto_connect_mode() == AUTO_CONNECT_MANUAL ||
                get_auto_connect_mode() == AUTO_CONNECT_DIRECT) {
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

void supplicant_task_create_patch(void)
{
    osThreadDef_t   task_def;
    osMessageQDef_t queue_def;

    //create task
    task_def.name = OS_TASK_NAME_SUPPLICANT;
    task_def.stacksize = OS_TASK_STACK_SIZE_SUPPLICANT_PATCH;
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

/*
   Interface Initialization: Supplicant Task
 */
void wpa_supplicant_task_func_init_patch(void)
{
    supplicant_task_evt_handle = supplicant_task_evt_handle_patch;
    supplicant_task_create = supplicant_task_create_patch;
}
