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
#ifndef IEEE80211_MAX_SUPPORTED_RATES
#define IEEE80211_MAX_SUPPORTED_RATES 8
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
    MLME_CMD_PS_FORCE_STA,      // arg1: force power-saving mode with WifiSta_PSForceMode_t
    MLME_CMD_BSS_LISTEN_INV,    // arg1: listen interval. MIN: 1, MAX: 255
    MLME_CMD_FAST_CONNECT,
    MLME_CMD_SET_PARAM,                         /* arg1: parameter id. prvData: Value or pointer to be set */
    MLME_CMD_GET_PARAM,                         /* arg1: parameter id. */
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
//    MLME_EVT_DEMO_SCAN_START,
//#endif
    MLME_EVT_NUM,

    MLME_EVT_DEAUTH_NACK,   //Private event
    //For set wifi mac configuration
    MLME_EVT_SET_PARAM_CNF  = MLME_EVT_DEAUTH_NACK,
    MLME_EVT_GET_PARAM_CNF,
    MLME_EVT_UPDATE_DTIM,
    //For Auto connect use
    MLME_EVT_AUTO_CONNECT_START = 100,       
    MEML_EVT_AUTO_CONNECT_FAILED_IND,
    MLME_EVT_AUTO_CONNECT,
    MLME_EVT_FAST_CONNECT_START,
} mlme_evt_type_e;

typedef enum {
	MAC_STA_IDLE,
	MAC_STA_0,
	MAC_STA_1, //spec
	MAC_STA_2, //spec
	MAC_STA_3, //spec
	MAC_STA_4  //spec
} mac_sta_t;

/* Used for arg1 of MLME command of MLME_CMD_PS_FORCE_STA */
typedef enum {
    STA_PS_NONE,
    STA_PS_AWAKE_MODE,
    STA_PS_DOZE_MODE,

    STA_PS_MODE_NUM
} WifiSta_PSForceMode_t;

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
    uint8_t             ssid_len;                              /* Length of SSID */
    uint8_t             supported_rates[IEEE80211_MAX_SUPPORTED_RATES];
    int8_t              rssi;            /* Last observed Rx Power (dBm) */
    //u16             padding0;
    uint16_t            beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    uint16_t            capabilities;                      /* Supported capabilities */
    uint8_t             dtim_prod;              //DTIM Period

    wpa_ie_data_t wpa_data;
    u8 rsn_ie[256];     // ELEMENT_HEADER_LEN + IEEE80211_MAX_RSN_LEN
    u8 wpa_ie[257];     // ELEMENT_HEADER_LEN + IEEE80211_MAX_WPA_LEN
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

/* STA information */
#define WIFI_MAX_SKIP_DTIM_PERIODS      10

typedef struct {
    uint8_t     au8Dot11MACAddress[MAC_ADDR_LEN];
    uint8_t     u8SkipDtimPeriods;
} mw_wifi_sta_info_t;

#define STA_INFO_MAX_MANUF_NAME_SIZE   32

typedef struct {
    uint8_t     manufacture_name[STA_INFO_MAX_MANUF_NAME_SIZE];
} mw_blewifi_cbs_store_t;

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

typedef struct
{
    bool            free_ocpy;                         //scan info buffer is free or occupied, 0:free, 1:occupied
    u8              bssid[MAC_ADDR_LEN];               /* BSS ID - 48 bit HW address */
    u8              ap_channel;                        /* Which Channel */
    u64             latest_beacon_rx_time;             /* Timestamp - Last interaction with BSS */
    s8              ssid[IEEE80211_MAX_SSID_LEN + 1];  /* SSID of the BSS - 33 bytes */
    u8              ssid_len;                          /* Length of SSID */
    u8              supported_rates[IEEE80211_MAX_SUPPORTED_RATES];
    s8              rssi;                              /* Last observed Rx Power (dBm) */
    u16             beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    u16             capabilities;                      /* Supported capabilities */
    u8              dtim_prod;                         //DTIM Period

    wpa_ie_data_t wpa_data;
    u8            rsn_ie[256];
    u8            wpa_ie[257];
    s8            passphrase[MAX_LEN_OF_PASSPHRASE];         /* maximum number of passphrase is 64 bytes */
    s8            hid_ssid[IEEE80211_MAX_SSID_LEN + 1];   /* [APS write/MSQ read] Hidden SSID of the BSS. When ssid is null, using this field. */
    u8            hid_ssid_len;                           /* Length of SSID */
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
    u8              ssid_len;                          /* Length of SSID */
    u8              supported_rates[IEEE80211_MAX_SUPPORTED_RATES];
    s8              rssi;                              /* Last observed Rx Power (dBm) */
    u16             beacon_interval;                   /* Beacon interval - In time units of 1024 us */
    u16             capabilities;                      /* Supported capabilities */
    u8              dtim_prod;                         //DTIM Period

    wpa_ie_data_t wpa_data;
    u8            rsn_ie[256];
    u8            wpa_ie[257];
    s8            passphrase[64];         /* maximum number of passphrase is 64 bytes */
    s8            hid_ssid[IEEE80211_MAX_SSID_LEN + 1];   /* [APS write/MSQ read] Hidden SSID of the BSS. When ssid is null, using this field. */
    u8            hid_ssid_len;
    u8            psk[32];
    u8            fast_connect;
#else
    u8 bssid[6];
    u8 ap_channel;
    u8 fast_connect;
    s8 ssid[IEEE80211_MAX_SSID_LEN + 1];
    u8 ssid_len;                              /* Length of SSID */
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

/* Auto connection */
int auto_connect_init_impl(void);
u8 get_auto_connect_mode_impl(void);
u8 set_auto_connect_mode_impl(u8 mode);
u8 get_auto_connect_ap_num_impl(void);
u8 set_auto_connect_ap_num_impl(u8 num);
void get_auto_connect_ap_cfg_impl(MwFimAutoConnectCFG_t *cfg);
void set_auto_connect_ap_cfg_impl(MwFimAutoConnectCFG_t *cfg);
void add_auto_connect_list_impl (void);
s32 wifi_sta_get_ac_idx_impl(u8 *pBssid);
auto_conn_info_t *wifi_get_ac_record_impl(u8* bssid);
int auto_connect_is_full_impl(auto_connect_cfg_t *cfg);
int auto_connect_add_queue_impl(auto_connect_cfg_t *cfg, scan_info_t *info);
int auto_connect_del_queue_impl(auto_connect_cfg_t *cfg);
void control_auto_connect_impl(void);
int wifi_sta_join_fast_impl(u8 ap_index);
int wifi_auto_connect_req_impl(void);
u8 get_auto_connect_info_impl(u8 idx, auto_conn_info_t *info);
u8 set_auto_connect_info_impl(u8 idx, auto_conn_info_t *info);
void reset_auto_connect_list_impl(void);
u8 get_fast_connect_mode_impl(u8 ap_idx);
u8 set_fast_connect_mode_impl(u8 ap_idx, u8 mode);
u8 get_auto_connect_save_ap_num_impl(void);
u8 set_auto_connect_save_ap_num_impl(u8 num);

int wifi_get_sta_cfg_from_share_memory_impl(u8 cfg_idx, void *ptr);
int wifi_set_sta_cfg_req_impl(u8 idx, u8 *value);
void wifi_sta_info_init_impl(void);

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

/* Auto connection */
typedef int (*auto_connect_init_fp_t) (void);
typedef u8 (*get_auto_connect_mode_fp_t) (void);
typedef u8 (*set_auto_connect_mode_fp_t) (u8 mode);
typedef u8 (*get_auto_connect_ap_num_fp_t) (void);
typedef u8 (*set_auto_connect_ap_num_fp_t) (u8 num);
typedef void (*get_auto_connect_ap_cfg_fp_t) (MwFimAutoConnectCFG_t *cfg);
typedef void (*set_auto_connect_ap_cfg_fp_t) (MwFimAutoConnectCFG_t *cfg);
typedef void (*add_auto_connect_list_fp_t) (void);
typedef s32 (*wifi_sta_get_ac_idx_fp_t) (u8 *pBssid);
typedef auto_conn_info_t * (*wifi_get_ac_record_fp_t) (u8* bssid);
typedef int (*auto_connect_is_full_fp_t) (auto_connect_cfg_t *cfg);
typedef int (*auto_connect_add_queue_fp_t) (auto_connect_cfg_t *cfg, scan_info_t *info);
typedef int (*auto_connect_del_queue_fp_t) (auto_connect_cfg_t *cfg);
typedef void (*control_auto_connect_fp_t) (void);
typedef int (*wifi_sta_join_fast_fp_t) (u8 ap_index);
typedef int (*wifi_auto_connect_req_fp_t) (void);
typedef u8 (*get_auto_connect_info_fp_t) (u8 idx, auto_conn_info_t *info);
typedef u8 (*set_auto_connect_info_fp_t) (u8 idx, auto_conn_info_t *info);
typedef void (*reset_auto_connect_list_fp_t) (void);
typedef u8 (*get_fast_connect_mode_fp_t) (u8 ap_idx);
typedef u8 (*set_fast_connect_mode_fp_t) (u8 ap_idx, u8 mode);
typedef u8 (*get_auto_connect_save_ap_num_fp_t) (void);
typedef u8 (*set_auto_connect_save_ap_num_fp_t) (u8 num);

typedef int (*wifi_get_sta_cfg_from_share_memory_fp_t) (u8 cfg_idx, void *ptr);
typedef int (*wifi_set_sta_cfg_req_fp_t) (u8 idx, u8 *value);
typedef void (*wifi_sta_info_init_fp_t) (void);

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

/* Auto connection */
extern auto_connect_init_fp_t auto_connect_init;
extern get_auto_connect_mode_fp_t get_auto_connect_mode;
extern set_auto_connect_mode_fp_t set_auto_connect_mode;
extern get_auto_connect_ap_num_fp_t get_auto_connect_ap_num;
extern set_auto_connect_ap_num_fp_t set_auto_connect_ap_num;
extern get_auto_connect_ap_cfg_fp_t get_auto_connect_ap_cfg;
extern set_auto_connect_ap_cfg_fp_t set_auto_connect_ap_cfg;
extern add_auto_connect_list_fp_t add_auto_connect_list;
extern wifi_sta_get_ac_idx_fp_t wifi_sta_get_ac_idx;
extern wifi_get_ac_record_fp_t wifi_get_ac_record;
extern auto_connect_is_full_fp_t auto_connect_is_full;
extern auto_connect_add_queue_fp_t auto_connect_add_queue;
extern auto_connect_del_queue_fp_t auto_connect_del_queue;
extern control_auto_connect_fp_t control_auto_connect;
extern wifi_sta_join_fast_fp_t wifi_sta_join_fast;
extern wifi_auto_connect_req_fp_t wifi_auto_connect_req;
extern get_auto_connect_info_fp_t get_auto_connect_info;
extern set_auto_connect_info_fp_t set_auto_connect_info;
extern reset_auto_connect_list_fp_t reset_auto_connect_list;
extern get_fast_connect_mode_fp_t get_fast_connect_mode;
extern set_fast_connect_mode_fp_t set_fast_connect_mode;
extern get_auto_connect_save_ap_num_fp_t get_auto_connect_save_ap_num;
extern set_auto_connect_save_ap_num_fp_t set_auto_connect_save_ap_num;

extern wifi_get_sta_cfg_from_share_memory_fp_t wifi_get_sta_cfg_from_share_memory;
extern wifi_set_sta_cfg_req_fp_t wifi_set_sta_cfg_req;
extern wifi_sta_info_init_fp_t wifi_sta_info_init;

#endif  //__CONTROLLER_WIFI_COM_H__
