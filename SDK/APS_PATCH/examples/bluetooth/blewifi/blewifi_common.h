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

#ifndef __BLEWIFI_COMMON_H__
#define __BLEWIFI_COMMON_H__

#include "msg.h"

#define BLEWIFI_INFO(fmt, ...)    msg_print(LOG_HIGH_LEVEL, fmt, ##__VA_ARGS__)
#define BLEWIFI_ERROR(fmt, ...)   msg_print(LOG_HIGH_LEVEL, fmt, ##__VA_ARGS__)

#define BLEWIFI_DUMP blewifi_hexdump

enum
{
	BLEWIFI_MSG_EXCESSIVE,
	BLEWIFI_MSG_MSGDUMP,
	BLEWIFI_MSG_DEBUG,
	BLEWIFI_MSG_INFO,
	BLEWIFI_MSG_WARNING,
	BLEWIFI_MSG_ERROR
};

void blewifi_hexdump(int level, const char *title, const uint8_t *buf, size_t len, int show);

#endif

