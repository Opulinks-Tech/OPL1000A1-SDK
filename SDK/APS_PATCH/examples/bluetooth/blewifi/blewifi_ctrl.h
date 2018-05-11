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
 * @file blewifi_ctrl.h
 * @author Vincent Chen, Michael Liao
 * @date 20 Feb 2018
 * @brief File includes the function declaration of blewifi ctrl task.
 *
 */

#ifndef __BLEWIFI_CTRL_H__
#define __BLEWIFI_CTRL_H__

#include <stdint.h>
#include <stdbool.h>

#define BLEWIFI_CTRL_MSG_MAX_LEN        (128)
#define BLEWIFI_CTRL_QUEUE_SIZE         (20)

typedef enum blewifi_ctrl_msg_type
{
    /* BLE Trigger */
    BLEWIFI_CTRL_MSG_BLE_INIT_COMPLETE = 0,    //BLE report status
    BLEWIFI_CTRL_MSG_BLE_ADVERTISING_CMF,      //BLE report status
    BLEWIFI_CTRL_MSG_BLE_CONNECTION_COMPLETE,  //BLE report status
    BLEWIFI_CTRL_MSG_BLE_DISCONNECT,           //BLE report status
    BLEWIFI_CTRL_MSG_BLE_DATA_IND,             //BLE send data to blewifi task
    BLEWIFI_CTRL_MSG_BLE_RSV1,                 //BLE reserve 1
    BLEWIFI_CTRL_MSG_BLE_RSV2,                 //BLE reserve 2

    /* Wi-Fi Trigger */
    BLEWIFI_CTRL_MSG_WIFI_SCAN_DONE_IND,       //Wi-Fi report status
    BLEWIFI_CTRL_MSG_WIFI_CONNECTION_IND,      //Wi-Fi report status
    BLEWIFI_CTRL_MSG_WIFI_DISCONNECTION_IND,   //Wi-Fi report status
    BLEWIFI_CTRL_MSG_WIFI_SCAN_RESULTS_SEND,   //Wi-Fi send scan results to BLE
    BLEWIFI_CTRL_MSG_WIFI_RSV1,                //Wi-Fi reserve 1
    BLEWIFI_CTRL_MSG_WIFI_RSV2,                //Wi-Fi reserve 2

    BLEWIFI_CTRL_MSG_NUM
} blewifi_ctrl_msg_type_e;

typedef struct
{
    uint32_t event;
	uint32_t length;
	uint8_t *pcMessage;
} xBleWifiCtrlMessage_t;


/**
  * @brief This function send data to blewifi task
  *
  * @param[in] message type
  * @param[in] payload data
  * @param[in] payload data length
  * @return
  *    - 0 : succeed
  *    - -1: faild
  */
int blewifi_ctrl_msg_send(int msg_type, uint8_t *data, int data_len);

/**
  * @brief This function send data to BLE Stack
  *
  * @param[in] message type
  * @param[in] payload data
  * @param[in] payload data length
  */
void blewifi_ble_send_data(int msg_type, uint8_t *data, int data_len);


void blewifi_ctrl_task_create(void);

#endif /* __BLEWIFI_CTRL_H__ */

