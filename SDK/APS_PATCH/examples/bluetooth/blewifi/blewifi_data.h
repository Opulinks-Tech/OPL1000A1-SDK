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
 * @file blewifi_data.h
 * @author Michael Liao
 * @date 20 Feb 2018
 * @brief File includes the function declaration of blewifi task.
 *
 */

#ifndef __BLEWIFI_DATA_H__
#define __BLEWIFI_DATA_H__

#include <stdint.h>
#include <stdbool.h>

#include "wifi_types.h"

//#define BLEWIFI_DATA_QUEUE_SIZE     (50)

/* default GATT MTU size over LE link
*/
#define GATT_DEF_BLE_MTU_SIZE               23


/* BLEWIFI HEADER + L2CAP RESERVED */
#define BLEWIFI_MTU_RESERVED_SIZE     (sizeof(blewifi_hdr_t) + 3)
#define BLEWIFI_FRAG_DATA_DEFAULT_LEN (GATT_DEF_BLE_MTU_SIZE - BLEWIFI_MTU_RESERVED_SIZE)

typedef enum blewifi_data_msg_type
{
    BLEWIFI_DATA_MSG_BLE_TO_WIFI = 0,   //Data is from BLE, send it to Wi-Fi
    BLEWIFI_DATA_MSG_WIFI_TO_BLE,       //Data is from Wi-Fi, send it to BLE
    BLEWIFI_DATA_MSG_RSV1,              //Reserve 1
    BLEWIFI_DATA_MSG_RSV2,              //Reserve 2

    BLEWIFI_DATA_MSG_NUM
} blewifi_data_msg_type_e;


/** @brief This enumeration defines the wireless authentication mode to indicate the Wi-Fi device authentication attribute.
*/
typedef enum {
    BLEWIFI_AUTH_OPEN = 0,             /**< authenticate mode : open */
    BLEWIFI_AUTH_WEP,                  /**< authenticate mode : WEP */
    BLEWIFI_AUTH_WPA_PSK,              /**< authenticate mode : WPA_PSK */
    BLEWIFI_AUTH_WPA2_PSK,             /**< authenticate mode : WPA2_PSK */
    BLEWIFI_AUTH_WPA_WPA2_PSK,         /**< authenticate mode : WPA_WPA2_PSK */
} blewifi_auth_mode_t;

/** @brief This enumeration defines wireless security cipher suits.
*/
typedef enum {
    BLEWIFI_CIPHER_TYPE_NONE = 0,      /**< 0, the cipher type is none */
    BLEWIFI_CIPHER_TYPE_WEP40,         /**< 1, the cipher type is WEP40 */
    BLEWIFI_CIPHER_TYPE_WEP104,        /**< 2, the cipher type is WEP104 */
    BLEWIFI_CIPHER_TYPE_TKIP,          /**< 3, the cipher type is TKIP */
    BLEWIFI_CIPHER_TYPE_CCMP,          /**< 4, the cipher type is CCMP */
    BLEWIFI_CIPHER_TYPE_TKIP_CCMP,     /**< 5, the cipher type is TKIP and CCMP */
    BLEWIFI_CIPHER_TYPE_UNKNOWN,       /**< 6, the cipher type is unknown */
} blewifi_cipher_type_t;

/** @brief This enumeration defines the wireless STA scan type
*/
typedef enum {
    BLEWIFI_SCAN_TYPE_ACTIVE = 0,      /**< Actively scan a network by sending 802.11 probe(s)         */
    BLEWIFI_SCAN_TYPE_PASSIVE,         /**< Passively scan a network by listening for beacons from APs */
} blewifi_scan_type_t;

typedef struct {
	uint8_t show_hidden;                 /**< enable/disable to scan AP whose SSID is hidden */
	uint8_t scan_type;                   /**< scan type, active or passive */
} blewifi_cmd_scan_t;

typedef struct {
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< The MAC address of the target AP. */
	uint8_t password_length;                  /**< The length of the password. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the target AP. */
} blewifi_cmd_connect_t;

/** @brief This structure defines the information of scanned APs
*/
typedef struct {
	uint8_t  ssid_length;                      /**< Length of the SSID. */
    uint8_t  ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< Stores the predefined SSID. */
    uint8_t  bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< AP's MAC address. */
    uint8_t  auth_mode;                        /**< Please refer to the definition of #wifi_auth_mode_t. */
    int8_t   rssi;                             /**< Records the RSSI value when probe response is received. */
}__attribute__((packed)) blewifi_scan_info_t;

typedef enum {
    BLEWIFI_REQ_SCAN = 0,
	BLEWIFI_REQ_CONNECT,
	BLEWIFI_REQ_DISCONNECT,
	BLEWIFI_REQ_RECONNECT,

    BLEWIFI_RSP_SCAN_REPORT = 0x1000,
    BLEWIFI_RSP_SCAN_END,
	BLEWIFI_RSP_CONNECT,
	BLEWIFI_RSP_DISCONNECT,
	BLEWIFI_RSP_RECONNECT,
	BLEWIFI_RSP_END = 0xFFFF,
}blewifi_type_id_e;

/* BLEWIF protocol */
typedef struct blewifi_hdr_tag
{
    uint16_t type;
    uint16_t data_len;
    uint8_t  data[]; //variable size
}blewifi_hdr_t;

void blewifi_data_event_handler(uint32_t evt_type, void *data, int data_len);
void blewifi_data_recv_handler(uint8_t *data, int len);
void blewifi_data_send_encap(uint16_t type_id, uint8_t *data, int total_data_len);
void blewifi_send_response(uint16_t type, uint8_t status);
void blewifi_send_scan_report(uint16_t apCount, blewifi_scan_info_t *ap_list);

#endif /* __BLEWIFI_DATA_H__ */

