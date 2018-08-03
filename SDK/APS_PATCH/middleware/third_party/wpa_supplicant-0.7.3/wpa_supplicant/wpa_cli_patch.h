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

#ifndef _WPA_CLI_PATCH_H_
#define _WPA_CLI_PATCH_H_

void wpa_cli_func_init_patch(void);
int check_mac_addr_len(const char *txt);
void debug_cli_mac_addr_src(void);
int wpa_cli_scan_by_cfg(void *cfg);

#endif /* _WPA_CLI_PATCH_H_ */
