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

#ifndef _WIFI_MAC_STA_H_
#define _WIFI_MAC_STA_H_

//#include "wifi_mac_common.h"
//#include "wifi_mac_types.h"
//#include "wifi_mac_eth.h"

#define ETH_LLC_ENCAP_MODE_AP           0
#define ETH_LLC_ENCAP_MODE_STA          1
#define ETH_LLC_ENCAP_MODE_IBSS         2

//#include "wifi_mac_queue.h"
//#include "wifi_mac_dcf.h"


typedef struct {
    u8      au8Dot11MACAddress[MAC_ADDR_LEN];
    u8      u8Dot11ShortRetryLimit;
    u8      u8Dot11LongRetryLimit;
    u8      u8CwExpMin;
    u8      u8CwExpMax;
    // 11th
    u8      u8SilenceThr;
    u8      u8KeepConnectThr;
    u16     u16TimeUnit;
    u8      u8Sifs;
    u8      u8Difs;
    u16     u16Eifs;
    u8      u8Slot;
    u8      u8MaEnLen;
    // 21st
    u8      u8TrxEnLen;
    u8      u8PhyMode;
    u8      u8ConsSuccThr;      // The threshold to increase data rate when u8ConsSuccCnt of g_DcfInfo reaches
    u8      u8ConsFailThr;      // The threshold to decrease data rate when u8ConsFailCnt of g_DcfInfo reaches
    u16     u16RtsThr;          // The threshold to exchange RTS/CTS when packet size is larger
    u8      u8RetryThr1;        // The threshold to exchange RTS/CTS when u8RetryCnt of g_DcfInfo reaches
    u8      u8RetryThr2;        // The threshold to decrease current rate when u8RetryCnt of g_DcfInfo is the power of
    // N bytes

    u8      reserved[228];   // 256 - N bytes
} WifiSta_StaInfo_s;

typedef struct {
	wifi_device_mode_t device_mode;
	u8 bssid[MAC_ADDR_LEN];
#ifdef CHIP_DEMO_WIFI
	u8 current_ap[MAC_ADDR_LEN];
#endif
	s8 ssid[IEEE80211_MAX_SSID_LEN + 1];
	u8 phy_mode;
	u8 supported_rates[SUPPORTED_RATES_MAX];
	mac_sub_state_t mac_sub_state;
	mac_state_t mac_state;
	u16 rts_threshold;
	u16 aid;
	u32 tx_mgmt_count;
	u32 tx_data_count;
	u32 rx_mgmt_count;
	u32 rx_data_count;
	u16 tx_sequence;
	u16 rx_sequence;
	u32 auth_algorithm;
	u16 capabilities;
	u16 listen_interval;
	u16 beacon_interval;
	u8  current_channel;    //Current RF Channel. Table 17-9
	wifi_rate_mode_t wifi_rate_mode;
	u8  sta_pm_mode;    //Power Mode : 0 -> STA_IS_AWAKE, 1 -> STA_IS_DOZE
	u8  u8DtimCount;
	u8  u8DtimPeriod;
	u8  silence_count;  // Unit: beacon interval
	s8  rssi;
	u8  u8ConsSuccCnt;
	u8  u8ConsFailCnt;

    u8 wpa_assoc_ie[100];
    wpa_ssid_t own_wpa_info;
    rx_eapol_report eapol_msg;
    u8  rsna_en;
	//dl_list_t tx_queue;
	//dl_list_t rx_queue;
	//linked_list_t *tx_queue;
	//linked_list_t *rx_queue;
    u32 volatile spin_lock;   //Henry test
} bss_info_t;


extern volatile sl_list_t g_wifi_mac_msq_rx_queue;
extern volatile sl_list_t g_wifi_mac_msq_tx_queue;
extern volatile sl_list_t g_wifi_mac_aps_rx_queue;
extern volatile sl_list_t g_wifi_mac_aps_tx_queue;

extern wifi_mac_msq_rx_entry_t *g_wifi_mac_msq_rx_entry_current;
extern wifi_mac_aps_rx_entry_t *g_wifi_mac_aps_rx_entry_current;

extern bss_info_t bss_info;
extern WifiSta_StaInfo_s s_StaInfo;

void wifi_mac_init(void);

u32 send_probe_req(void);
u32 send_auth_req(void);
u32 send_assoc_req(void);
u32 send_reassoc_req(void);
u32 send_disassoc_req(u16 reason_code);
u32 send_deauth_req(u16 reason_code);
u32 send_rts(u16 frame_length);
void wifi_queue_reset(void);
#ifdef WIFI_DEBUG_MODE
u32 send_debug_frame(void);
u32 send_ack(void);
u32 dbg_send_frame(void);
#endif
u32 send_beacon(void);
u8 management_frame_parser(rx_frame_info_t *rx_frame_info);

#endif /* _WIFI_MAC_STA_H_ */

