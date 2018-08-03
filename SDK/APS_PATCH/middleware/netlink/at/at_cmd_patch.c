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

#include "at_cmd_patch.h"
#include "at_cmd_tcpip_patch.h"
#include "at_cmd_func_patch.h"
#include "at_cmd_wifi_patch.h"
#include "at_cmd_data_process_patch.h"
#include "at_cmd_ble_patch.h"


int _at_cmd_handler_patch(char *buf, int len, int mode)
{
    data_process_handler_impl(buf, len);
    return true;
}

/*
 * @brief AT Command Interface Initialization for AT Command module
 *
 */
void _at_cmd_func_init_patch(void)
{
    /** Command Tables */
    _at_cmd_ble_func_init_patch();
    _at_cmd_wifi_func_init_patch();
    _at_cmd_tcpip_func_init_patch();

    /** Functions */
    _at_cmd_handler = _at_cmd_handler_patch;
}
