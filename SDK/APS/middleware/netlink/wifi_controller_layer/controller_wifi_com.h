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

#ifndef __CONTROLLER_WIFI_COM_H__
#define __CONTROLLER_WIFI_COM_H__
#include <stdint.h>
#include "wifi_mac_types.h"


//#include "wifi_wpa_rsne.h"
typedef void (*os_ptimer) (void const *argument);


#ifndef IEEE80211_MAX_SSID_LEN
#define IEEE80211_MAX_SSID_LEN          32
#endif
#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN            6
#endif
#ifndef SUPPORTED_RATES_MAX
#define SUPPORTED_RATES_MAX 8
#endif

typedef enum mlme_cmd_type
{
    MLME_CMD_CONNECT = 0,
    MLME_CMD_DISCONNECT,
    MLME_CMD_SCAN,
    MLME_CMD_SCAN_ABORT,
    MLME_CMD_SCAN_RESULTS,
    MLME_CMD_GET_BSSID,
    MLME_CMD_GET_SSID,
    MLME_CMD_TX_STATUS,
    MLME_CMD_NUM,

    MLME_INIT,  //Private command
    MLME_CONNECT_DONE,
    MLME_CMD_TIMEOUT,
} mlme_cmd_type_e;

typedef enum mlme_evt_type
{
    MLME_EVT_ASSOC = 0,       //Association completed
    MLME_EVT_DISASSOC,        //Association lost
    MLME_EVT_SCAN_RESULTS,    //Scan results available
    MLME_EVT_ASSOCINFO,       //Report optional extra information for association
    MLME_EVT_AUTH,            //Authentication result
    MLME_EVT_DEAUTH,          //Authentication lost
    MLME_EVT_ASSOC_REJECT,    //Association rejected
    MLME_EVT_AUTH_TIMED_OUT,  //Authentication timed out
    MLME_EVT_ASSOC_TIMED_OUT, //Association timed out
    MLME_EVT_TX_STATUS,       //Report TX status
    MLME_EVT_RX_MGMT,         //Report RX of a management frame
    MLME_EVT_SIGNAL_CHANGE,   //Indicate change in signal strength
    MLME_EVT_EAPOL_RX,        //Report received EAPOL frame
//#ifdef DEMO_TC
    MLME_EVT_DEMO_SCAN_START,
//#endif
    MLME_EVT_NUM,

    MLME_EVT_DEAUTH_NACK,   //Private event
} mlme_evt_type_e;

typedef enum {
	MAC_STA_IDLE,
	MAC_STA_0,
	MAC_STA_1, //spec
	MAC_STA_2, //spec
	MAC_STA_3, //spec
	MAC_STA_4  //spec
} mac_sta_t;

//from wifi_wpa_rsn.h
typedef struct _wpa_ie_data
{
    int proto;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int capabilities;
    size_t num_pmkid;
    const u8 *pmkid;
    int mgmt_group_cipher;
}wpa_ie_data_t;

//typedef struct wpa_ie_data wpa_ie_data_t;


typedef struct
{
    unsigned char       free_ocpy;                  //scan info buffer is free or occupied, 0:free, 1:occupied
    uint8_t             bssid[MAC_ADDR_LEN];               /* BSS ID - 48 bit HW address */
    uint8_t             ap_channel;                         /* Which Channel */
    uint64_t            latest_beacon_rx_time;	           /* Timestamp - Last interaction with BSS */
    char                ssid[IEEE80211_MAX_SSID_LEN + 1];            /* SSID of the BSS - 33 bytes */
    uint8_t             supported_rates[SUPPORTED_RATES_MAX];
    int8_t              rssi;            /* Last observed Rx Power (dBm) */
    //u16             padding0;
    uint16_t            beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    uint16_t            capabilities;                      /* Supported capabilities */
    uint8_t             dtim_prod;              //DTIM Period

    wpa_ie_data_t wpa_data;
    u8 rsn_ie[100];
    u8 wpa_ie[100];
    //u8 priv_assoc_ie[100];
    //u8 priv_wpa_ie[100];
    //u32     flags;
    //dl_list members;
} scan_info_t;

typedef struct scan_report_t
{
    u32     uScanApNum;
    scan_info_t    *pScanInfo;
}scan_report_t;

typedef struct
{
    //u8 * frame_buffer;
    u8 frame_buffer[384];
    unsigned int frame_length;
}rx_eapol_data;

typedef struct
{
    u8 psk[32];
    int psk_set;
    char *passphrase;
    int pairwise_cipher;
    int group_cipher;
    int key_mgmt;
    int mgmt_group_cipher;  //for 802.11w?
    int proto;

#define EAPOL_FLAG_REQUIRE_KEY_UNICAST BIT(0)
#define EAPOL_FLAG_REQUIRE_KEY_BROADCAST BIT(1)

#if 1//def IEEE8021X_EAPOL
    int eapol_flags;
    int leap;
    int non_leap;
    unsigned int eap_workaround;
#endif /* IEEE8021X_EAPOL */
}asso_data;

extern scan_report_t gScanReport;


void wifi_cmd_tout_handle_impl(mlme_cmd_type_e ctrl_cmd);
void wifi_create_cmd_timer_impl(os_ptimer ptimer_fxn);
int wifi_get_bssid_impl(u8 *bssid_holder);
int wifi_get_mac_addr_impl(u8 *mac_addr);
void wifi_set_mac_addr_impl(u8 *mac_addr);
int wifi_scan_req_impl(u32 scan_mode);
int wifi_sta_join_impl(u8 *pBssid);
int wifi_sta_join_complete_impl(unsigned char is_success);
void wifi_sta_leave_impl(void);
scan_report_t *wifi_get_scan_result_impl(void);
scan_info_t *wifi_get_scan_record_impl(u8* bssid);
scan_info_t *wifi_get_scan_record_by_ssid_impl(char* ssid);
rx_eapol_data * wifi_get_eapol_data_impl(void);
asso_data * wifi_get_asso_data_impl(void);
u8 * wifi_get_asso_ie_impl(void);
u8 wifi_get_mac_state_impl(void);
u8 *wifi_get_ssid_impl(void);
int wifi_get_rssi_impl(void);
int wifi_cli_handler_impl(int argc, char *argv[]);

/* Interface initial function */
void wifi_ctrl_func_init(void);

/* Rom patch interface */
typedef void (*wifi_cmd_tout_handle_fp_t)(mlme_cmd_type_e ctrl_cmd);
typedef void (*wifi_create_cmd_timer_fp_t)(os_ptimer ptimer_fxn);
typedef int (*wifi_get_bssid_fp_t)(u8 *bssid_holder);
typedef int (*wifi_get_mac_addr_fp_t)(u8 *mac_addr);
typedef void (*wifi_set_mac_addr_fp_t)(u8 *mac_addr);
typedef int (*wifi_scan_req_fp_t)(u32 scan_mode);
typedef s32 (*wifi_sta_get_scan_idx_fp_t)(u8 *pBssid);
typedef int (*wifi_sta_join_fp_t)(u8 *pBssid);
typedef int (*wifi_sta_join_complete_fp_t)(unsigned char is_success);
typedef void (*wifi_sta_leave_fp_t)(void);
typedef scan_report_t * (*wifi_get_scan_result_fp_t)(void);
typedef scan_info_t * (*wifi_get_scan_record_fp_t)(u8* bssid);
typedef scan_info_t * (*wifi_get_scan_record_by_ssid_fp_t)(char* ssid);
typedef rx_eapol_data * (*wifi_get_eapol_data_fp_t)(void);
typedef asso_data * (*wifi_get_asso_data_fp_t)(void);
typedef u8 * (*wifi_get_asso_ie_fp_t)(void);
typedef u8 * (*wifi_get_ssid_fp_t)(void);
typedef u8 (*wifi_get_mac_state_fp_t)(void);
typedef int (*wifi_get_rssi_fp_t)(void);
typedef int (*wifi_cli_handler_fp_t)(int argc, char *argv[]);

/* Export interface funtion pointer */
extern wifi_cmd_tout_handle_fp_t wifi_cmd_tout_handle;
extern wifi_create_cmd_timer_fp_t wifi_create_cmd_timer;
extern wifi_get_bssid_fp_t wifi_get_bssid;
extern wifi_get_mac_addr_fp_t wifi_get_mac_addr;
extern wifi_set_mac_addr_fp_t wifi_set_mac_addr;
extern wifi_scan_req_fp_t wifi_scan_req;
extern wifi_sta_get_scan_idx_fp_t wifi_sta_get_scan_idx;
extern wifi_sta_join_fp_t wifi_sta_join;
extern wifi_sta_join_complete_fp_t wifi_sta_join_complete;
extern wifi_sta_leave_fp_t wifi_sta_leave;
extern wifi_get_scan_result_fp_t wifi_get_scan_result;
extern wifi_get_scan_record_fp_t wifi_get_scan_record;
extern wifi_get_scan_record_by_ssid_fp_t wifi_get_scan_record_by_ssid;
extern wifi_get_eapol_data_fp_t wifi_get_eapol_data;
extern wifi_get_asso_data_fp_t wifi_get_asso_data;
extern wifi_get_asso_ie_fp_t wifi_get_asso_ie;
extern wifi_get_ssid_fp_t wifi_get_ssid;
extern wifi_get_mac_state_fp_t wifi_get_mac_state;
extern wifi_get_rssi_fp_t wifi_get_rssi;
extern wifi_cli_handler_fp_t wifi_cli_handler;

#endif  //__CONTROLLER_WIFI_COM_H__
