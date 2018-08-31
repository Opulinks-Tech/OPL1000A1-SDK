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

/* standard library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cli.h"
#include "lwip_cli.h"
#include "sys_common_ctrl.h"

extern cli_command_t *g_lwip_cmdtbl_ptr;
extern lwip_cli_handler_fp_t lwip_cli_handler;
extern int cli_do_multilevel_cmd(cli_command_t *cmd_tble, int argc, char *argv[]);

cli_command_t *g_lwip_cfg_cmdtbl_ptr;

int lwip_cli_cfg_dhcp(int len, char *param[])
{
    u8 mode;
    mode = atoi(param[0]);
    set_dhcp_arp_check(mode);
    printf("\nCurrent Setting = %d \n", get_dhcp_arp_check());
    printf("DHCP ARP check 0:Disable, 1:Enable. \n");
    return 0;
}

cli_command_t lwip_cfg_cli[] = {
    { "cfg_dhcp",       "Enable/Disable DHCP ARP check",      lwip_cli_cfg_dhcp,          NULL },
    { NULL,          NULL,                  NULL,                           NULL }
};

/****************************************************************************
 *
 * API functions.
 *
 ****************************************************************************/
int lwip_cli_handler_patch(int token_cnt, char *param[])
{
    if (token_cnt < 1) return 0;

    cli_do_multilevel_cmd(g_lwip_cmdtbl_ptr, token_cnt, param);
    cli_do_multilevel_cmd(g_lwip_cfg_cmdtbl_ptr, token_cnt, param);
    
    return 0;
}

void lwip_load_interface_cli_patch(void)
{
    g_lwip_cfg_cmdtbl_ptr  =     lwip_cfg_cli;
    lwip_cli_handler       =     lwip_cli_handler_patch;
}
