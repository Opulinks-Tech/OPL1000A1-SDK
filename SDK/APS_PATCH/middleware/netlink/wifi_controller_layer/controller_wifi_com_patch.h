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

#ifndef _CONTROLLER_WIFI_COM_PATCH_H_
#define _CONTROLLER_WIFI_COM_PATCH_H_

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "controller_wifi_com.h"

#define WIFI_MAX_SKIP_DTIM_PERIODS_PATCH      255
#define WIFI_MLME_SCAN_ALL_CHANNELS             0
#define WIFI_MLME_SCAN_MAX_NUM_CHANNELS        14

#define SCAN_ACTIVE_MIN_DUR_TIME_DEF  100
#define SCAN_PASSIVE_MIN_DUR_TIME_DEF 150
#define SCAN_MAX_NUM_OF_DUR_TIME      1500

int send_port_security_done_event(void);
int _wifi_get_sta_cfg_from_share_memory_patch(u8 cfg_idx, u8 *ptr);

typedef enum mlme_evt_type_patch
{
    //For set wifi mac configuration
    MLME_EVT_SET_PARAM_CNF_PATCH  = MLME_EVT_DEAUTH_NACK+1, // 16
    MLME_EVT_GET_PARAM_CNF_PATCH,
    MLME_EVT_UPDATE_DTIM_PATCH,
    MLME_EVT_PORT_SCRT_DONE,      // 19
} mlme_evt_type_patch_e;

#define AUTO_CONNECT_REASON_CODE_FAILED      200
#define CONNECT_AP_REASON_CODE_NOT_FOUND     201
#define CONNECT_AP_REASON_CODE_TIMEOUT       202

//Encrypt type recognition
#define WIFIS_EID_RSN                48
#define WIFIS_EID_VENDOR_SPECIFIC   221
#define WPAS_IE_VENDOR_TYPE  0x0050f201

//Hidden AP bitwise operation
#define SET_BIT(x,n) ((x)|=(1<<(n)))
#define CHK_BIT(x,n) (((x)&(1<<(n)))!=0)

#define WIFI_SCAN_BY_CFG_TOUT_TIME     22000   //20 seconds

typedef enum
{
    CONNECT_OPEN_DONE,
    CONNECT_PORT_SCRT_DONE,
} connect_done_st_e;


typedef enum {
    WIFI_MLME_SCAN_TYPE_ACTIVE = 0,      /**< Actively scan a network by sending 802.11 probe(s)         */
    WIFI_MLME_SCAN_TYPE_PASSIVE,         /**< Passively scan a network by listening for beacons from APs */
    WIFI_MLME_SCAN_TYPE_MIX,             /**< Active + Passive */
} E_WIFI_MLME_SCAN_TYPE;



typedef struct
{
    u8                hap_en;
    u8                hap_index;
    u8                hap_final_index;
    u16               hap_bitvector;
    char              hap_ssid[IEEE80211_MAX_SSID_LEN+1];
    auto_conn_info_t *hap_ap_info;    
}hap_control_t;


/** The parameter of MLME_CMD_SCAN */
typedef struct {
    scan_report_t *ptScanReport;                    /**< The scan report which filled by MSQ, report to APS */
    uint32_t u32ActiveScanDur;                      /**< Scan duration per scan counter in channel. units: millisecond */
    uint32_t u32PassiveScanDur;                     /**< Scan duration per channel. units: millisecond */
    E_WIFI_MLME_SCAN_TYPE tScanType;                /**< scan type. active, passive, or mix mode */
    uint8_t u8Channel;                              /**< Only specific channel or scan all channels */
    uint8_t u8aSsid[IEEE80211_MAX_SSID_LEN + 1];    /**< Not supported yet. SSID of AP */
    uint8_t u8aBssid[MAC_ADDR_LEN];                 /**< Not supported yet. MAC address of AP */
    uint8_t u8MaxScanApNum;                         /**< Max scan AP number. When scanned AP number over this value, 
                                                         MSQ will drop the AP with smallest RSSI value */
    uint8_t u8ResendCnt;                            /**< Send probe req counter per channel when active scan.
                                                         After send probe req, it will wait active scan time, 
                                                         and then send next probe req. 
                                                         The total time will be increased by a factor of this value */
} S_WIFI_MLME_SCAN_CFG;


/* Hidden AP connection*/
hap_control_t *get_hap_control_struct(void);
void wifi_sta_join_for_hiddenap(void);
void hiddenap_complete(void);
u8 get_repeat_conn(void);
int set_repeat_conn(u8 mode);
u8 get_dtim_period_from_bss_info(void);
void wifi_scan_cfg_init(S_WIFI_MLME_SCAN_CFG *psScanCfg);
int wifi_scan_req_by_cfg(void *cfg);
int wpas_get_assoc_freq_patch(void);
#endif /* _CONTROLLER_WIFI_COM_PATCH_H_ */
