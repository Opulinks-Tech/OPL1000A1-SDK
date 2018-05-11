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
 * @file blewifi_ctrl.h
 * @author Vincent Chen, Michael Liao
 * @date 20 Feb 2018
 * @brief File includes the function declaration of blewifi ctrl task.
 *
 */

#ifndef __BLEWIFI_USER_APP_H__
#define __BLEWIFI_USER_APP_H__

#include <stdint.h>
#include <stdbool.h>

#define BLEWIFI_USER_APP_DATA_BUF_LEN   (128)
#define BLEWIFI_USER_APP_QUEUE_SIZE     (5)

typedef enum blewifi_user_app_msg_type
{
    BLEWIFI_USER_APP_MSG_RSV1,
    BLEWIFI_USER_APP_MSG_RSV2,

    BLEWIFI_USER_APP_MSG_NUM
} blewifi_user_app_msg_type_e;

typedef struct
{
    uint32_t event;
	uint32_t length;
	uint8_t *pcMessage;
} xBleWifiUserAppMessage_t;

void blewifi_user_app_task_create(void);

#endif

