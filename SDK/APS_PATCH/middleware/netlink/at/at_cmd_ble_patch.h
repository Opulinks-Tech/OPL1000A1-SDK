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

#ifndef __AT_CMD_BLE_PATCH_H__
#define __AT_CMD_BLE_PATCH_H__

int _at_cmd_letest(char *buf, int len, int mode);
void _at_cmd_ble_func_init_patch(void);

#endif //__AT_CMD_BLE_PATCH_H__
