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
 * @file at_cmd_msg_ext.h
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File containing declaration of at_cmd_msg_ext api & definition of structure for reference.
 *
 */

#ifndef __AT_CMD_MSG_EXT_H__
#define __AT_CMD_MSG_EXT_H__
#include <stddef.h>
#include <ctype.h>
#include "controller_wifi_com.h"

/**
 * @brief AT Extending Customer's Message Type
 *
 */
enum at_msg_ext_type{
	AT_MSG_EXT_AMPEC = 0,     /**< Message for APMEC AT Command. */
	AT_MSG_EXT_ESPRESSIF = 1, /**< Message for Espressif AT Command. */
	AT_MSG_EXT_OTHERS = 2,    /**< Message for others AT Command. */
	AT_MSG_EXT_NUM
};

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_scan
 *
 */
typedef void (*at_msg_ext_wifi_scan_fp_t)(int cusType, scan_report_t *result);

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_connect
 *
 */
typedef void (*at_msg_ext_wifi_connect_fp_t)(int cusType);

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_disconnect
 *
 */
typedef void (*at_msg_ext_wifi_disconnect_fp_t)(int cusType);

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_get_rssi
 *
 */
typedef void (*at_msg_ext_wifi_get_rssi_fp_t)(int cusType, int rssi);

/**
 * @brief Extern Function at_msg_ext_wifi_scan
 *
 */
extern at_msg_ext_wifi_scan_fp_t at_msg_ext_wifi_scan;

/**
 * @brief Extern Function at_msg_ext_wifi_connect
 *
 */
extern at_msg_ext_wifi_connect_fp_t at_msg_ext_wifi_connect;

/**
 * @brief Extern Function at_msg_ext_wifi_disconnect
 *
 */
extern at_msg_ext_wifi_disconnect_fp_t at_msg_ext_wifi_disconnect;

/**
 * @brief Extern Function at_msg_ext_wifi_get_rssi
 *
 */
extern at_msg_ext_wifi_get_rssi_fp_t at_msg_ext_wifi_get_rssi;

/*
 * @brief AT Command extending message interface Initialization
 *
 */
void at_msg_ext_init(void);

#endif //__AT_CMD_MSG_EXT_H__

