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

#ifndef _AT_CMD_MSG_EXT_PATCH_H_
#define _AT_CMD_MSG_EXT_PATCH_H_

#if defined(__AT_CMD_SUPPORT__)
void at_msg_ext_init_patch(void);
void at_msg_ext_wifi_dispatch_connect_reason(bool connected, int reason);
#endif


/**
 * @brief AT Extending Error Code for CWJAP
 *
 */
typedef enum {
    ERR_WIFI_CWJAP_DONE,
    //ERR_WIFI_CWJAP_TO,
    //ERR_WIFI_CWJAP_PWD_INVALID,
    //ERR_WIFI_CWJAP_NO_AP,
    //ERR_WIFI_CWJAP_FAIL,
    //ERR_WIFI_CWJAP_FAIL_OTHERS,
    ERR_WIFI_CWJAP_DISCONNECT = 10,
} at_wifi_error_code_cwjap_ext;

#endif /* _AT_CMD_MSG_EXT_PATCH_H_ */
