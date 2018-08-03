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

#ifndef _SYS_COMMON_API_H_
#define _SYS_COMMON_API_H_

#include <stdint.h>
#include <stdbool.h>

#include "sys_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
  * @brief     Get the configuration of Mac address source
  *
  * @attention 1. API returns false if try to get Configuration which something error
  *
  * @param[in]   iface: The interface of mac address
  *              - MAC_IFACE_WIFI_STA
  *              - MAC_IFACE_WIFI_SOFTAP (Not support yet)
  *              - MAC_IFACE_BLE
  *
  * @param[out]  type: Mac address source type
  *              - MAC_SOURCE_FROM_OTP
  *              - MAC_SOURCE_FROM_FLASH
  *
  * @return    0  : success
  * @return    other : failed
 */
int mac_addr_get_config_source(mac_iface_t iface, mac_source_type_t *type);

/**
  * @brief     Set the configuration of Mac address source
  *
  * @attention 1. API returns false if try to set Configuration which something error
  *
  * @param[in]   iface: The interface of mac address
  *              - MAC_IFACE_WIFI_STA
  *              - MAC_IFACE_WIFI_SOFTAP (Not support yet)
  *              - MAC_IFACE_BLE
  *
  * @param[in]  type: Mac address source type
  *              - MAC_SOURCE_FROM_OTP
  *              - MAC_SOURCE_FROM_FLASH
  *
  * @return    0  : success
  * @return    other : failed
 */
int mac_addr_set_config_source(mac_iface_t iface, mac_source_type_t type);

/**
  * @brief     Get the configuration of RF power level
  *
  * @attention 1. API returns false if try to get Configuration which something error
  *
  * @param[out]   level: The level of RF power
  *              - SYS_RF_LOW_POWER
  *              - SYS_RF_HIGH_POWER
  *
  * @return    0  : success
  * @return    other : failed
 */
int sys_get_config_rf_power_level(sys_rf_power_level_t *level);

/**
  * @brief     Set the configuration of RF power level
  *
  * @attention 1. API returns false if try to set Configuration which something error
  *
  * @param[in]   level: The level of RF power
  *              - SYS_RF_LOW_POWER
  *              - SYS_RF_HIGH_POWER
  *
  * @return    0  : success
  * @return    other : failed
 */
int sys_set_config_rf_power_level(sys_rf_power_level_t level);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_COMMON_API_H_ */
