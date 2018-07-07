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

#ifndef __WIFI_API_H__
#define __WIFI_API_H__

#include <stdint.h>
#include <stdbool.h>

#include "wifi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup WIFI_APIs WIFI APIs
  * @brief WIFI APIs 
  */

/** @addtogroup WIFI_APIs
  * @{
  */

/** \defgroup WIFI_STA_APIs WIFI STA APIs
 */

/** @addtogroup WIFI_STA_APIs
 * @{
 */


/******************************************************
 *                    Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef int32_t wifi_result_t;


/**
 * @brief This defines the Wi-Fi event handler. Call #wifi_connection_register_event_handler() to register a handler, 
 * then the Wi-Fi driver generates an event and sends it to the handler.
 * @param[in] event  is an optional event to register. For more details, please refer to #wifi_event_t.
 *
 * @param[in] payload  is the payload for the event.
 *                     When the event is WIFI_EVENT_IOT_CONNECTED in AP mode, payload is the connected STA's MAC address.
 *                     When the event is WIFI_EVENT_IOT_CONNECTED in STA mode, payload is the connected AP's BSSID.
 * @param[in] length is the length of a packet.
 *
 * @return The return value is reserved and it is ignored.
 */
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);


/**
 * @brief Initialization of complete callback function.
 *
 * Invoked when Wi-Fi initialization is complete.
 *
 * @param ctx is context pointer that provided to wifi_init(). It will be
 *            passed back to the callback.
 */
typedef void (*wifi_init_complete_cb_t)(void* ctx);


/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/**
  * @brief  Init Wi-Fi
  *  Initializes the wifi according to the specified parameters in the config.
  *
  * @attention 1. This API must be called before other Wi-Fi APIs are invoked
  *
  * @param[in]  config: pointer to Wi-Fi init configuration structure; can point to a temporary variable.
  * @param[in]  init_cb: pointer to Wi-Fi init complete configuration structure; can point to a temporary variable.
  *
  * @return    0  : success
  * @return    other : failed
  *
  */
int wifi_init(const wifi_init_config_t *config, wifi_init_complete_cb_t init_cb);

/**
  * @brief  De-init Wi-Fi Initialization and Configuration functions 
  *
  * @attention 1. This API should be called if want to remove Wi-Fi driver from the system
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_deinit(void);

/**
  * @brief     Set configuration of OPL1000 STA 
  *
  * @attention 1. This API is called only when specified interface is enabled, otherwise API calling will be failed
  * @attention 2. For station configuration, bssid_set shall be set to 0; set to 1 menas user want to check MAC address of certain AP.
  * @attention 3. OPL1000 is limited to working on one channel.
  * @param[in]   interface: Configure wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  *
  * @param[in]   conf: structure of configuration paremeters 
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_set_config(wifi_mode_t interface, wifi_config_t *conf);

/**
  * @brief     Get configuration of specified interface
  *
  * @param[in]  interface: Configure wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[out] conf: return wifi's current operating parameters
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_get_config(wifi_mode_t interface, wifi_config_t *conf);

/**
  * @brief  Start Wi-Fi working 
  *         - If mode is WIFI_MODE_STA, it creates station control block and starts station
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_start(void);

/**
  * @brief  Stop wifi working
  *         - If mode is WIFI_MODE_STA, it stops station and releases station control block
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_stop(void);

/**
  * @brief     Scan all available APs.
  *            After invoke the #wifi_set_config() and #wifi_start(), then call #wifi_scan_start() to scan APs
  *
  * @param[in] config: Configure parameters for scan operation 
  * @param[in] block: if block is true, this API blocks the caller until scan operation is done, otherwise it returns immediately
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_scan_start(const wifi_scan_config_t *config, bool block);

/**
  * @brief     Stop scanning process
  *
  * @attention This API shall be called after #wifi_scan_start()
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_scan_scan_stop(void);

/**
  * @brief     Get list of APs that found in last scan operation
  *
  * @attention This API only be called when scan is completed, otherwise it may get wrong value.
  *
  * @param[out] scan_list: store APs' informaton that found in last scan operation
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_scan_get_ap_list(wifi_scan_list_t *scan_list);

/**
  * @brief     Get the number of scanned APs
  *
  * @param[out] number: store number of APs found in last scan operation 
  *
  * @attention This API only be called when scan is completed, otherwise it may get wrong value.
  *
  * @return the scan result of AP number
  */
int wifi_scan_get_ap_num(uint16_t *number);

/**
  * @brief     Get AP list found in last scan operation
  *
  * @param[out]  number: As input param, it stores max AP number that ap_records can hold. 
  *              As output param, it receives the actual AP number that this API returns.
  * @param[out]  ap_records: wifi_scan_info_t array stores the found APs
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_scan_get_ap_records(uint16_t *number, wifi_scan_info_t *ap_records);

/**
  * @brief     Get information of AP which OPL1000 station is associated with
  *
  * @param[out]     ap_info: get AP information from list
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_sta_get_ap_info(wifi_scan_info_t *ap_info);

/**
  * @brief Disconnect the link between OPL1000 and connected AP.
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_disconnect_ap(void);

/**
  * @brief Disconnect the link between the current device and the station.
  *
  * @param[in]  address: station address
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_disconnect_sta(uint8_t *address);

/**
  * @brief     Connect OPL1000 Wi-Fi station to certain AP.
  *
  * @attention 1. This API only impact WIFI_MODE_STA or WIFI_MODE_AP mode
  * @attention 2. If OPL1000 is connected to an AP, call wifi_disconnect to disconnect.
  * @param[in]  config: Establish connection parameters
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_connect(wifi_config_t *config);

/**
  * @brief     Scan start
  *
  * @param     ssid: ssid string
  * @param     ssid_length: ssid string length
  * @param     bssid: bssid
  * @param     scan_mode: refer to #wifi_scan_mode_ext in wpa_common_patch.h
  * @param     scan_option: if scan_option is true, this API will block the caller until the scan is done,
  *                         otherwise it will return immediately
  * 
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_scan_start(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);

/**
  * @brief get signal strength of AP
  *
  * @attention 1. If the scan is successful, this API returns signal strength value, otherwise it will get wrong result
  *
  * @param[out] rssi: rssi value
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_get_rssi(int8_t *rssi);

/**
  * @brief register wifi call back handler
  *
  * @param[in] event: The type of the registered event. Options are 
  *            - WIFI_EVENT_INIT_COMPLETE
  *            - WIFI_EVENT_SCAN_COMPLETE
  *            - WIFI_EVENT_STA_START
  *            - WIFI_EVENT_STA_STOP
  *            - WIFI_EVENT_STA_CONNECTED
  *            - WIFI_EVENT_STA_DISCONNECTED
  *            - WIFI_EVENT_STA_CONNECTION_FAILED
  *            - WIFI_EVENT_STA_GOT_IP
  *
  * @param[in] handler: registered event handler 
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
  * @brief unregister wifi call back handler
  *
  * @param[in] event: The type of the unregistered event. Options please refer to #wifi_connection_register_event_handler()
  * @param[in] handler: unregistered event handler 
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
  * @brief     Set wifi operation mode
  *
  * @param     mode: refer to #wifi_mode_t
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_set_opmode(uint8_t mode);

/**
  * @brief     Set wifi operation mode
  *
  * @param     mode: refer to #wifi_mode_t
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_get_opmode(uint8_t *mode);

/**
  * @brief     Get mac of specified interface
  *
  * @param[in]   interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[out]  address: Get the MAC address of the device through this interface, 
  *              The address is similar to this structure: xx:xx:xx:xx:xx:xx
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_get_mac_address(wifi_mode_t interface, uint8_t *address);

/**
  * @brief     Set MAC address of OPL1000 Wi-Fi station or the soft-AP interface.
  *
  * @attention 1. This API can only be called when the interface is disabled
  * @attention 2. OPL1000 soft-AP and station have different MAC addresses, do not set them to be the same.
  *
  * @param[in]  interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[in]  address: set MAC address
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_set_mac_address(wifi_mode_t interface, uint8_t *address);

/**
  * @brief Get ssid value of AP
  *
  * @param[out]  ssid: Get ssid by pointer
  * @param[out]  ssid_length: Get the length of the ssid character
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_get_ssid(uint8_t *ssid, uint8_t *ssid_length);

/**
  * @brief Set the ssid value of the current device
  *
  * @param[in] interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[in] ssid: Set the value of ssid
  * @param[in] ssid_length: The length of ssid parameter
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_set_ssid(wifi_mode_t interface, uint8_t *ssid, uint8_t ssid_length);

/**
  * @brief get bssid after scan 
  *
  * @param[out] bssid: the string of bssid
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_get_bssid(uint8_t *bssid);

/**
  * @brief  config OPL1000 Wi-Fi bssid.
  *
  * @param[in] bssid: the string of bssid
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_set_bssid(uint8_t *bssid);

/**
  * @brief     Get the primary/secondary channel of OPL1000
  *
  * @attention 1. API returns false if try to get an interface which is not enabled
  *
  * @param[in]   interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[out]  channel: Get Current module wifi work channel number
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_get_channel(wifi_mode_t interface, uint8_t *channel);

/**
  * @brief     Set primary/secondary channel of OPL1000
  *
  * @attention 1. This is a special API for sniffer
  * @attention 2. This API should be called after #wifi_start()
  *
  * @param[in]  interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[in]  channel: Set current Wi-Fi work channel number
  *
  * @return    0  : success
  * @return    other : failed
  */
int wifi_config_set_channel(wifi_mode_t interface, uint8_t channel);

/**
  * @brief     Set the bandwidth of OPL1000 specified interface
  *
  * @param[in]  interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[in]  bandwidth: Set the working bandwidth of wifi
  *
  * @return    0  : success
  * @return    other : failed
  *
 */
int wifi_config_set_bandwidth(wifi_mode_t interface, wifi_bandwidth_t bandwidth);

/**
  * @brief     Get the bandwidth of OPL1000 specified interface
  *
  * @attention 1. API returns false if try to get an interface which is not enable
  *
  * @param[in]   interface: Configure the current wifi working mode,The options are 
  *              - WIFI_MODE_STA 
  *              - WIFI_MODE_AP (currently not support)
  * @param[out]  bandwidth: Get the bandwidth value of the current wifi module working through the pointer
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_get_bandwidth(wifi_mode_t interface, wifi_bandwidth_t *bandwidth);

/**
  * @brief     Get the interval of DTIM
  *
  * @param[in]   interval: the interval of DTIM
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_get_dtim_interval(uint8_t *interval);

/**
  * @brief     Set the interval of DTIM
  *
  * @param[in]   interval: the interval of DTIM
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_set_dtim_interval(uint8_t interval);

/**
  * @brief     Get the interval of listen
  *
  * @param[in]   interval: the interval of listen
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_get_listen_interval(uint8_t *interval);

/**
  * @brief     Set the interval of listen
  *
  * @param[in]   interval: the interval of listen
  *
  * @return    0  : success
  * @return    other : failed
 */
int wifi_config_set_listen_interval(uint8_t interval);

/**
  * @brief     Get the Skip DTIM value in current wifi setting of OPL1000
  *
  * @param[out]  value: Get the Skip DTIM value in current wifi setting
  * 
  * @return    0  : success
  * @return    other : failed
  *
 */
int wifi_config_get_skip_dtim(uint8_t *value);

/**
  * @brief     Set the Skip DTIM value of OPL1000
  *
  * @param[in]  value: Set the Skip DTIM value
  * 
  * @attention 1. This API will set the skip DTIM value to share memory and
  *                     stored in flash, please use wifi_config_get_skip_dtim() to 
  *                     check it.
  *
  * @attention 2. The setting will be effect after next connect. We recommend  
  *                     re-connect AP after setting to make sure the value is correct.
  *
  * @return    0  : success
  * @return    other : failed
  *
 */
int wifi_config_set_skip_dtim(uint8_t value);

/**
  * @brief Set the connection type 
  * 
  * @param[in] Connection Type
  *            - WIFI_MODE_STA 
  *            - WIFI_MODE_AP (currently not support)
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_set_mode(u8 mode);


/**
  * @brief Get the status of the current automatic connection mode
  * 
  * @return    0  : off 
  * @return    1  : on
  * 
*/
u8 wifi_auto_connect_get_mode(void);

/**
  * @brief Get the number of automatically connected aps that have been saved in the flash
  * 
  * @return    0-3 ap number
  * 
*/
u8 wifi_auto_connect_get_ap_num(void);

/**
  * @brief Save the number of automatically connected ap to flash
  * 
  * @param[in] Connection Type

  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_set_ap_num(u8 num);

/**
  * @brief Get ap detailed information saved in flash
  * 
  * @param[in] index : Index of ap information,The range is 0 to 3
  * @param[in] info  : wifi_auto_connect_info_f array to hold the found APs
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_get_ap_info(u8 index, wifi_auto_connect_info_t *info);

/**
  * @brief Delete automatically connected AP information stored in flash
  * 
  * @param[in] index : Index of ap information,The range is 0 to 3
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_del_ap_info(u8 index);

/**
  * @brief Initialize wifi automatic connection
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_init(void);

/**
  * @brief Start wifi automatic connection process
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_start(void);

/**
  * @brief Get the status of AP fast connection
  * 
  * @param[in] ap_index : Index of ap information,The range is 0 to 3
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
u8 wifi_fast_connect_get_mode(u8 ap_index);

/**
  * @brief Set the fast connection type 
  * 
  * @param[in] mode : Configure the fast connect mode ,0 means disable fast connection,
  *                   and 1 enable the fast connection mode
  * 
  * @param[in] ap_index : Index of ap information,The range is 0 to 3
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_fast_connect_set_mode(u8 mode, u8 ap_index);
int wifi_fast_connect_start(void);

/**
  * @brief Start the fast connection process
  * 
  * @return    0  : success
  * @return    other : failed
  * 
*/
int wifi_auto_connect_reset(void);



/**
  * @}
  */

 /**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __WIFI_API_H__ */

