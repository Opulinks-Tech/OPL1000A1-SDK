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
 * @file at_cmd_msg_ext_patch.h
 * @author Michael Liao
 * @date 20 Mar 2018
 * @brief File containing declaration of at_cmd_msg_ext api & definition of structure for reference.
 *
 */

#ifndef __AT_CMD_MSG_EXT_PATCH_H__
#define __AT_CMD_MSG_EXT_PATCH_H__
#include <stddef.h>
#include <ctype.h>
#include "controller_wifi_com.h"
#include "wifi_types.h"

/**
 * @brief AT Extending Error Code for CWJAP
 *
 */
typedef enum {
    ERR_WIFI_CWJAP_TO = 1,
    ERR_WIFI_CWJAP_PWD_INVALID,
    ERR_WIFI_CWJAP_NO_AP,
    ERR_WIFI_CWJAP_FAIL,
    ERR_WIFI_CWJAP_FAIL_OTHERS
} at_wifi_error_code_cwjap;

/**
 * @brief AT Extending Error Code for CWAUTOCONN
 *
 */
typedef enum {
    ERR_WIFI_CWAUTOCONN_INVALID = 1,
} at_wifi_error_code_cwautoconn;

/**
 * @brief AT Extending Error Code for CWFASTCONN
 *
 */
typedef enum {
    ERR_WIFI_CWFASTCONN_INVALID = 1,
    ERR_WIFI_CWFASTCONN_AP_NULL,
    ERR_WIFI_CWFASTCONN_PARAMETER_TOO_FEW,
} at_wifi_error_code_cwfastconn;


/**
 * @brief AT Extending Message for WIFI
 *
 */
typedef enum {
    MSG_WIFI_CONNECTED_OPEN,
    MSG_WIFI_CONNECTED_SECURITY,
    MSG_WIFI_DISCONNECTED,
    MSG_WIFI_NUM
} at_wifi_msg_code_connected;

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_err
 *
 */
typedef void (*at_msg_ext_wifi_err_fp_t)(int cusType, char *cmd_str, int error_code);

/**
 * @brief Function Pointer Type for API at_msg_ext_wifi_connect
 *
 */
typedef void (*_at_msg_ext_wifi_connect_fp_t)(int cusType, int msg_code);

/**
 * @brief Function Pointer Type for API scan_report_sorting
 *
 */
typedef void (*at_msg_ext_wifi_sorting_fp_t)(wifi_scan_list_t *data);

/**
 * @brief Extern Function _at_msg_ext_wifi_connect
 *
 */
extern _at_msg_ext_wifi_connect_fp_t _at_msg_ext_wifi_connect;

/**
 * @brief Extern Function at_msg_ext_wifi_err
 *
 */
extern at_msg_ext_wifi_err_fp_t at_msg_ext_wifi_err;

/**
 * @brief Extern Function scan_report_sorting
 *
 */
extern at_msg_ext_wifi_sorting_fp_t at_msg_ext_wifi_sorting;

void _at_msg_ext_wifi_show_one_ap_patch(int argc, char *argv[]);

void _at_msg_ext_init(void);

#endif

