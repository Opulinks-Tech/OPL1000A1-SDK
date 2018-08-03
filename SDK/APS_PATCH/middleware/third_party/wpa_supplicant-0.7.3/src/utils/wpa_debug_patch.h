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

#ifndef __WPA_DEBUG_PATCH_H__
#define __WPA_DEBUG_PATCH_H__

#include "common.h"
#include "wpabuf.h"
#include "msg.h"

extern int g_DbgMode;

#define wpa_printf_dbg(level, _message, ...) { if (g_DbgMode) msg_print(LOG_HIGH_LEVEL, (_message), ##__VA_ARGS__);}

void wpa_debug_func_init_patch(void);

#endif

