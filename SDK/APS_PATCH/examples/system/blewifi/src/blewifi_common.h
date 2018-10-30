/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
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


//#define BLEWIFI_SHOW_INFO
//#define BLEWIFI_SHOW_DUMP

#ifdef BLEWIFI_SHOW_INFO
#define BLEWIFI_INFO(fmt, ...)      tracer_log(LOG_LOW_LEVEL, fmt, ##__VA_ARGS__)
#define BLEWIFI_WARN(fmt, ...)      tracer_log(LOG_MED_LEVEL, fmt, ##__VA_ARGS__)
#define BLEWIFI_ERROR(fmt, ...)     tracer_log(LOG_HIGH_LEVEL, fmt, ##__VA_ARGS__)
#else
#define BLEWIFI_INFO(fmt, ...)
#define BLEWIFI_WARN(fmt, ...)
#define BLEWIFI_ERROR(fmt, ...)
#endif  // end of BLEWIFI_SHOW_INFO

#ifdef BLEWIFI_SHOW_DUMP
#define BLEWIFI_DUMP(a, b, c)    BleWifi_HexDump(a, b, c)
#else
#define BLEWIFI_DUMP(a, b, c)
#endif  // end of BLEWIFI_SHOW_DUMP

#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

void BleWifi_HexDump(const char *title, const uint8_t *buf, size_t len);

#endif  // end of __BLEWIFI_COMMON_H__
