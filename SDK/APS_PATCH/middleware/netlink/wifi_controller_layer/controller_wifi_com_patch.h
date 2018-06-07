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

#ifndef __CONTROLLER_WIFI_COM_PATCH_H__
#define __CONTROLLER_WIFI_COM_PATCH_H__

#include "common.h"
#include "controller_wifi_com.h"
#include "mw_fim_default_group02.h"

#define FLAG_OF_CBS_READ_WRITE_INFO

/**
 * @brief Attribute write data type from the client
 */
typedef enum
{
//    MLME_CMD_CONNECT = 0,
//    MLME_CMD_DISCONNECT,
//    MLME_CMD_SCAN,
//    MLME_CMD_SCAN_ABORT,
//    MLME_CMD_SCAN_RESULTS,
//    MLME_CMD_GET_BSSID,
//    MLME_CMD_GET_SSID,
//    MLME_CMD_TX_STATUS,
//    MLME_CMD_NUM,
//
//    MLME_INIT,  //Private command
//    MLME_CONNECT_DONE,
//    MLME_CMD_TIMEOUT,
    MLME_CMD_PS_FORCE_STA = MLME_CMD_TIMEOUT+1, /*!< arg1: force power-saving mode with WifiSta_PSForceMode_t */
    MLME_CMD_BSS_LISTEN_INV,                    /*!< arg1: listen interval. MIN: 1, MAX: 255                  */
    MLME_CMD_FAST_CONNECT,
    MLME_CMD_SET_PARAM,                         /* arg1: parameter id. prvData: Value or pointer to be set */
    MLME_CMD_GET_PARAM,                         /* arg1: parameter id. */
} mlme_cmd_type_ext_e;

/* Used for arg1 of MLME command of MLME_CMD_PS_FORCE_STA */
/**
 * @brief Attribute write data type from the client
 */
typedef enum {
    STA_PS_NONE,         
    STA_PS_AWAKE_MODE,   
    STA_PS_DOZE_MODE,   

    STA_PS_MODE_NUM
} WifiSta_PSForceMode_t;

typedef enum
{
//    MLME_EVT_ASSOC = 0,       //Association completed
//    MLME_EVT_DISASSOC,        //Association lost
//    MLME_EVT_SCAN_RESULTS,    //Scan results available
//    MLME_EVT_ASSOCINFO,       //Report optional extra information for association
//    MLME_EVT_AUTH,            //Authentication result
//    MLME_EVT_DEAUTH,          //Authentication lost
//    MLME_EVT_ASSOC_REJECT,    //Association rejected
//    MLME_EVT_AUTH_TIMED_OUT,  //Authentication timed out
//    MLME_EVT_ASSOC_TIMED_OUT, //Association timed out
//    MLME_EVT_TX_STATUS,       //Report TX status
//    MLME_EVT_RX_MGMT,         //Report RX of a management frame
//    MLME_EVT_SIGNAL_CHANGE,   //Indicate change in signal strength
//    MLME_EVT_EAPOL_RX,        //Report received EAPOL frame
//#ifdef DEMO_TC
//    MLME_EVT_DEMO_SCAN_START,
//#endif
//    MLME_EVT_NUM,

//    MLME_EVT_DEAUTH_NACK,   //Private event
    MLME_EVT_SET_PARAM_CNF  = MLME_EVT_DEAUTH_NACK+1, /* [0000560] */
    MLME_EVT_GET_PARAM_CNF,
    MLME_EVT_UPDATE_DTIM,
    MLME_EVT_PORT_SCRT_DONE,
    
    //For Auto connect use
    MLME_EVT_AUTO_CONNECT_START = 100,       
    MEML_EVT_AUTO_CONNECT_FAILED_IND,
    MLME_EVT_AUTO_CONNECT,
    MLME_EVT_FAST_CONNECT_START,
} mlme_evt_type_ext_e;

typedef enum
{
    CONNECT_OPEN_DONE,
    CONNECT_PORT_SCRT_DONE,
} connect_done_st_e;

/* For CBS use */
#define STA_INFO_MAX_MANUF_NAME_SIZE   32

/* WIFI STA configuration */
#define WIFI_MAX_SKIP_DTIM_PERIODS      10

typedef enum {   
    E_WIFI_PARAM_MAC_ADDRESS=0,    
    E_WIFI_PARAM_SKIP_DTIM_PERIODS,
    
    /* Read only parameters */
    E_WIFI_PARAM_BSS_DTIM_PERIOD=200,
}E_WIFI_PARAM_ID;

/* Auto Connect Report and Info*/
#define MAX_NUM_OF_AUTO_CONNECT 3
#define MAX_NUM_OF_AUTO_CONNECT_RETRY  1
#define MAX_LEN_OF_PASSPHRASE 64 //please refer to #define MAX_LEN_OF_PASSWD

#define AUTO_CONNECT_DISABLE 0
#define AUTO_CONNECT_ENABLE  1
#define AUTO_CONNECT_MANUAL  2  //Internal use, for compatible Auto and Manual mode

#define AUTO_CONNECT_REASON_CODE_FAILED   200

typedef struct
{
    bool            free_ocpy;                         //scan info buffer is free or occupied, 0:free, 1:occupied
    u8              bssid[MAC_ADDR_LEN];               /* BSS ID - 48 bit HW address */
    u8              ap_channel;                        /* Which Channel */
    u64             latest_beacon_rx_time;             /* Timestamp - Last interaction with BSS */
    s8              ssid[IEEE80211_MAX_SSID_LEN + 1];  /* SSID of the BSS - 33 bytes */
    u8              supported_rates[SUPPORTED_RATES_MAX];
    s8              rssi;                              /* Last observed Rx Power (dBm) */
    u16             beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    u16             capabilities;                      /* Supported capabilities */
    u8              dtim_prod;                         //DTIM Period

    wpa_ie_data_t wpa_data;
    u8            rsn_ie[100];
    u8            wpa_ie[100];
    s8            passphrase[MAX_LEN_OF_PASSPHRASE];         /* maximum number of passphrase is 64 bytes */
    s8            hid_ssid[IEEE80211_MAX_SSID_LEN + 1];   /* [APS write/MSQ read] Hidden SSID of the BSS. When ssid is null, using this field. */
    u8            psk[32];
    u8            fast_connect;
} auto_conn_info_t;

typedef struct
{
    u32                 uFCApNum;
    u8                  targetIdx;
    u8                  retryCount;
    s8                  front;
    s8                  rear;
    bool                flag;
    u8                  max_save_num;
    auto_conn_info_t    *pFCInfo;
}auto_connect_cfg_t;

typedef struct
{
#if 1
    bool            free_ocpy;                         //scan info buffer is free or occupied, 0:free, 1:occupied
    u8              bssid[MAC_ADDR_LEN];               /* BSS ID - 48 bit HW address */
    u8              ap_channel;                        /* Which Channel */
    u64             latest_beacon_rx_time;             /* Timestamp - Last interaction with BSS */
    s8              ssid[IEEE80211_MAX_SSID_LEN + 1];  /* SSID of the BSS - 33 bytes */
    u8              supported_rates[SUPPORTED_RATES_MAX];
    s8              rssi;                              /* Last observed Rx Power (dBm) */
    u16             beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    u16             capabilities;                      /* Supported capabilities */
    u8              dtim_prod;                         //DTIM Period

    wpa_ie_data_t wpa_data;
    u8            rsn_ie[100];
    u8            wpa_ie[100];
    s8            passphrase[64];         /* maximum number of passphrase is 64 bytes */
    s8            hid_ssid[IEEE80211_MAX_SSID_LEN + 1];   /* [APS write/MSQ read] Hidden SSID of the BSS. When ssid is null, using this field. */
    u8            psk[32];
    u8            fast_connect;
#else
    u8 bssid[6];
    u8 ap_channel;
    u8 fast_connect;
    s8 ssid[IEEE80211_MAX_SSID_LEN + 1];
    u8 psk[32];
    wpa_ie_data_t wpa_data;
    u16 capabilities;
    u8 rsn[100];
#endif
} mw_wifi_auto_connect_ap_info_t;

typedef struct
{
    s8     front;
    s8     rear;
    bool   flag;
    u8     targetIdx;
    u8     max_save_num;
} MwFimAutoConnectCFG_t;

typedef int (*wifi_sta_join_fast_fp_t)(u8 ap_index);
typedef auto_connect_cfg_t * (*wifi_get_ac_result_fp_t)(void);
typedef int (*wifi_set_sta_cfg_req_fp_t)(u8 idx, u8 *value);
typedef int (*wifi_get_sta_cfg_from_share_memory_fp_t)(u8 cfg_idx, u8 *ptr); //[0000526]

/* Export interface funtion pointer */
extern wifi_sta_join_fast_fp_t wifi_sta_join_fast;
extern wifi_get_ac_result_fp_t wifi_get_ac_result;
extern wifi_set_sta_cfg_req_fp_t wifi_set_sta_cfg_req;
extern wifi_get_sta_cfg_from_share_memory_fp_t wifi_get_sta_cfg_from_share_memory;

int write_auto_conn_mode_to_flash(u8 mode);
int auto_connect_init(void);
u8 read_auto_conn_mode_from_flash(u8 *mode);
void read_auto_conn_ap_info_from_flash(u8 index, mw_wifi_auto_connect_ap_info_t *info);
int read_auto_conn_ap_num_from_flash(u8 *num);
u8 get_auto_connect_mode(void);
u8 get_auto_connect_save_ap_num(void);
u8 set_auto_connect_mode(u8 mode);
u8 get_auto_connect_ap_num(void);
u8 set_auto_connect_ap_num(u8 num);
u8 set_auto_connect_save_ap_num(u8 num);
int wifi_auto_connect_req(void);
int wifi_fast_connect_req(void);
void control_auto_connect(void);
auto_conn_info_t *wifi_get_ac_record(u8* bssid);
void delete_auto_connect_list_from_flash(u8 index);
void delete_auto_conn_ap_num_to_flash(void);
void delete_auto_connect_cfg_from_flash(void);
void reset_auto_connect_list(void);
void add_auto_connect_list(void);
void write_auto_connect_ap_num_to_flash(u8 num);
void write_auto_connect_ap_info_to_flash(u8 index, mw_wifi_auto_connect_ap_info_t *info);
void get_auto_connect_ap_cfg(MwFimAutoConnectCFG_t *cfg);
void set_auto_connect_ap_cfg(MwFimAutoConnectCFG_t *cfg);
void write_auto_connect_ap_cfg_to_flash(MwFimAutoConnectCFG_t *cfg);
u8 get_fast_connect_mode(u8 ap_idx);
u8 set_fast_connect_mode(u8 ap_idx, u8 mode);
u8 get_auto_connect_info(u8 idx, auto_conn_info_t *info);
u8 set_auto_connect_info(u8 idx, auto_conn_info_t *info);
void wifi_sta_info_init(void);
int send_port_security_done_event(void);

#endif  //__CONTROLLER_WIFI_COM_PATCH_H__

