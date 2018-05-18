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
#include "includes.h"
#include "common.h"
#include "msg.h"
#include "wpa_debug_patch.h"

int g_DbgMode;
extern int wpa_debug_level;

int wpa_get_debug_mode(void)
{
    return g_DbgMode;
}

void wpa_set_debug_mode(int mode)
{
    g_DbgMode = mode;
}

void _wpa_hexdump_patch(int level, const char *title, const u8 *buf,
			 size_t len, int show)
{
	size_t i;

	//if (level < wpa_debug_level)
	//	return;
	//wpa_debug_print_timestamp();
    //wpa_printf_dbg(MSG_DEBUG, "\r\n %s - hexdump(len=%lu):", title, (unsigned long) len);

	if (buf == NULL) {
		wpa_printf_dbg(MSG_DEBUG," [NULL]");
	} else if (show) {
		for (i = 0; i < len; i++)
            wpa_printf_dbg(MSG_DEBUG, " %02x", buf[i]);
	} else {
		wpa_printf_dbg(MSG_DEBUG, " [REMOVED]");
	}

	wpa_printf_dbg(MSG_DEBUG, "\r\n");
}

void wpa_hexdump_patch(int level, const char *title, const u8 *buf, size_t len)
{
	_wpa_hexdump_patch(level, title, buf, len, 1);
}

/*
   Interface Initialization: WPA Debug
 */
void wpa_debug_func_init_patch(void)
{
    wpa_set_debug_mode(false);
    wpa_hexdump = wpa_hexdump_patch;
    _wpa_hexdump = _wpa_hexdump_patch;
   return;
}

