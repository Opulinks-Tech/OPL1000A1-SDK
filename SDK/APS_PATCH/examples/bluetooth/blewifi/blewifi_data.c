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
 * @file blewifi_data.c
 * @author Michael Liao
 * @date 20 Feb 2018
 * @brief File creates the wifible_data task architecture.
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"
#include "sys_os_config.h"
#include "msg.h"
#include "wifi_api.h"
#include "blewifi_common.h"
#include "blewifi_data.h"
#include "blewifi_ctrl.h"
#include "blewifi_server_app.h"

typedef struct {
    uint16_t total_len;
    uint16_t remain;
    uint16_t offset;
    uint8_t *aggr_buf;
} blewifi_rx_packet_t;

blewifi_rx_packet_t g_rx_packet;

#if 1
//for command test
static void blewifi_data_encap_input(uint16_t type_id, uint8_t *data, int data_len)
{
    blewifi_hdr_t *hdr = NULL;

    /* 1.Pack blewifi header */
    hdr = malloc(sizeof(blewifi_hdr_t) + data_len);
    if (hdr == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: memory alloc fail\r\n");
        return;
    }

    hdr->type = type_id;
    hdr->data_len = data_len;
    memcpy(hdr->data, data, hdr->data_len);

    BLEWIFI_DUMP(BLEWIFI_MSG_DEBUG, "[BLEWIFI]:in packet", (uint8_t*)hdr, (hdr->data_len + sizeof(blewifi_hdr_t)), 1);

    /* 2.send BLE data to blewifi task */
    blewifi_ctrl_msg_send(BLEWIFI_CTRL_MSG_BLE_DATA_IND, (uint8_t *)hdr, (hdr->data_len + sizeof(blewifi_hdr_t)));

    free(hdr);
}

void blewifi_build_scan_req(void)
{
    blewifi_cmd_scan_t cmd = {0};
    uint8_t *data, *pos;
    int data_len;

    pos = data = malloc(sizeof(blewifi_cmd_scan_t));
    if (data == NULL) {
        printf("malloc error\r\n");
        return;
    }

    cmd.show_hidden = true;
    cmd.scan_type = BLEWIFI_SCAN_TYPE_ACTIVE;
    memcpy(pos, &cmd, sizeof(blewifi_cmd_scan_t));
    pos += 2;
    data_len = (pos - data);

    BLEWIFI_DUMP(BLEWIFI_MSG_DEBUG, "scan cmd payload", data, data_len, 1);

    /* create connection cmd data packet */
    blewifi_data_encap_input(BLEWIFI_REQ_SCAN, data, data_len);

    free(data);
}

void blewifi_build_connect_req(void)
{
    uint8_t bssid[6] = {0x70, 0x4f, 0x57, 0x74, 0xf9, 0x00}; //TP-Link_F900
    char password[32] = "12345678";
    uint8_t password_len = 0;
    uint8_t *data, *pos;
    int data_len;

    pos = data = malloc(sizeof(blewifi_cmd_connect_t));
    if (data == NULL) {
        printf("malloc error\r\n");
        return;
    }

    password_len = strlen(password);
    memcpy(data, bssid, 6);
    pos += 6;
    *pos++ = password_len;
    memcpy(pos, password, password_len);
    pos += password_len;

    data_len = (pos - data);

    BLEWIFI_DUMP(BLEWIFI_MSG_DEBUG, "conn cmd payload", data, data_len, 1);

    /* create connection cmd data packet */
    blewifi_data_encap_input(BLEWIFI_REQ_CONNECT, data, data_len);

    free(data);
}

void blewifi_build_disconnect_req(void)
{
    blewifi_data_encap_input(BLEWIFI_REQ_DISCONNECT, NULL, 0);
}

#endif

void blewifi_send_scan_report(uint16_t apCount, blewifi_scan_info_t *ap_list)
{
    uint8_t *data;
    int data_len;
    uint8_t *pos;

    int malloc_size =sizeof(blewifi_scan_info_t) *apCount;

    pos = data = malloc(malloc_size);
    if (data == NULL) {
        printf("malloc error\r\n");
        return;
    }

    for (int i = 0; i < apCount; ++i)
    {
        uint8_t len = ap_list[i].ssid_length;
        data_len = (pos - data);

        *pos++ = len;
        memcpy(pos, ap_list[i].ssid, len);
        pos += len;
        memcpy(pos, ap_list[i].bssid,6);
        pos += 6;
        *pos++ = ap_list[i].auth_mode;
        *pos++ = ap_list[i].rssi;
    }

    data_len = (pos - data);

    BLEWIFI_DUMP(BLEWIFI_MSG_MSGDUMP, "scan report data", data, data_len, 1);
    /* create scan report data packet */
    blewifi_data_send_encap(BLEWIFI_RSP_SCAN_REPORT, data, data_len);

    free(data);
}


void blewifi_send_response(uint16_t type, uint8_t status)
{
    blewifi_data_send_encap(type, &status, 1);
}

void blewifi_ble_send_data(int msgType,  uint8_t* dataBuf, int len)
{
    /* Send data to BLE Stack */
    BleWifiSendAppMsgToBle(msgType, len, dataBuf);
}

void blewifi_wifi_send_data(int msgType, int len, unsigned char * dataBuf)
{
    /* Send data by socket client or http client, TBD */
}

void blewifi_data_event_handler(uint32_t evt_type, void *data, int data_len)
{
	switch (evt_type)
    {
	    case BLEWIFI_DATA_MSG_BLE_TO_WIFI:
            /* Send BLE data to Wi-Fi */
			break;

        case BLEWIFI_DATA_MSG_WIFI_TO_BLE:
            /* Send Wi-Fi data to BLE */
            break;

        case BLEWIFI_DATA_MSG_RSV1:
        case BLEWIFI_DATA_MSG_RSV2:
            break;

		default:
			break;
	}
}

void blewifi_protocol_handler(uint16_t type, uint8_t *data, int len)
{
    /* parsing header type and do action, ex: scan, connection, disconnect */

    switch(type)
    {
        case BLEWIFI_REQ_SCAN:
        {
            wifi_scan_config_t scan_config = {0};
            scan_config.show_hidden = data[0];
            scan_config.scan_type = (wifi_scan_type_t)data[1];

            BLEWIFI_INFO("BLEWIFI: Recv Scan Request\r\n");
            wifi_scan_start(&scan_config, NULL);
        }
            break;
        case BLEWIFI_REQ_CONNECT:
        {
            wifi_config_t wifi_config = {0};

            memcpy(wifi_config.sta_config.bssid, &data[0], WIFI_MAC_ADDRESS_LENGTH);

            if (len > 6)
            {
                wifi_config.sta_config.password_length = data[6];
                memcpy((char *)wifi_config.sta_config.password, &data[7], wifi_config.sta_config.password_length);
            }

            BLEWIFI_INFO("BLEWIFI: Recv Connect Request\r\n");
            wifi_set_config(WIFI_MODE_STA, &wifi_config);
            wifi_connection_connect(&wifi_config);
        }
            break;
        case BLEWIFI_REQ_DISCONNECT:
            BLEWIFI_INFO("BLEWIFI: Recv Disconnect Request\r\n");
            wifi_connection_disconnect_ap();
            break;
        case BLEWIFI_REQ_RECONNECT:
            break;
        default:
            break;
    }

}

void blewifi_data_send_encap(uint16_t type_id, uint8_t *data, int total_data_len)
{
    blewifi_hdr_t *hdr = NULL;
    int remain_len = total_data_len;

    /* 1.fragment data packet to fit MTU size */

    /* 2.Pack blewifi header */
    hdr = malloc(sizeof(blewifi_hdr_t) + remain_len);
    if (hdr == NULL)
    {
        BLEWIFI_ERROR("BLEWIFI: memory alloc fail\r\n");
        return;
    }

    hdr->type = type_id;
    hdr->data_len = remain_len;
    memcpy(hdr->data, data, hdr->data_len);

    //BLEWIFI_INFO("hdr size = %d\n", sizeof(blewifi_hdr_t));
    //BLEWIFI_INFO("hdr->data_len = %d\n", hdr->data_len);

    BLEWIFI_DUMP(BLEWIFI_MSG_DEBUG, "[BLEWIFI]:out packet", (uint8_t*)hdr, (hdr->data_len + sizeof(blewifi_hdr_t)), 1);

    /* 3.send app data to BLE stack */
    blewifi_ble_send_data(BLEWIFI_APP_MSG_SEND_DATA, (uint8_t *)hdr, (hdr->data_len + sizeof(blewifi_hdr_t)));

    free(hdr);
}

void blewifi_data_recv_handler(uint8_t *data, int data_len)
{
    blewifi_hdr_t *hdr = NULL;
    int hdr_len = sizeof(blewifi_hdr_t);

    /* 1.aggregate fragment data packet, only first frag packet has header */
    /* 2.handle blewifi data packet, if data frame is aggregated completely */
    if (g_rx_packet.offset == 0)
    {
        hdr = (blewifi_hdr_t*)data;
        g_rx_packet.total_len = hdr->data_len + hdr_len;
        g_rx_packet.remain = g_rx_packet.total_len;
        g_rx_packet.aggr_buf = malloc(g_rx_packet.total_len);

        if (g_rx_packet.aggr_buf == NULL) {
           BLEWIFI_ERROR("%s no mem, len %d\n", __func__, g_rx_packet.total_len);
           return;
        }
    }

    memcpy(g_rx_packet.aggr_buf + g_rx_packet.offset, data, data_len);
    g_rx_packet.offset += data_len;
    g_rx_packet.remain -= data_len;

    /* no frag or last frag packet */
    if (g_rx_packet.remain == 0)
    {
        hdr = (blewifi_hdr_t*)g_rx_packet.aggr_buf;
        blewifi_protocol_handler(hdr->type, g_rx_packet.aggr_buf + hdr_len,  (g_rx_packet.total_len - hdr_len));
        g_rx_packet.offset = 0;
        g_rx_packet.remain = 0;
        free(g_rx_packet.aggr_buf);
        g_rx_packet.aggr_buf = NULL;
    }
}

