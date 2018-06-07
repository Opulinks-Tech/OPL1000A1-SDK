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
 * @file at_cmd_patch.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the patch code's implementation & modification.
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "os.h"
#include "at_cmd_patch.h"
#include "at_cmd_common.h"


/** Sample to do patch */
#if 0
int at_cmd_parse_impl_v1(char *pbuf)
{
    printf("\r\n at_cmd_parse_impl_v1 \r\n");
	return 1;
}

int at_cmd_wifi_cwmode_v1(int argc, char *argv[])
{
    wpa_cli_scan_handler(argc, argv);
    printf("\r\nOK\r\n");
    return 1;
}

int at_cmd_wifi_cwscan_v1(int argc, char *argv[])
{
    printf("\r\n at_cmd_wifi_cwscan_v1 \r\n");
    wpa_cli_scan_handler(argc, argv);
    printf("\r\nOK\r\n");
    return 1;
}

at_command_t gAtCmdTbl_v1[] = //sample to re-define new cmd table
{
    { "at+cwscan",           at_cmd_wifi_cwscan,       "Wi-Fi Scan" },     //sample to call to rom's original api
    { "at+cwscanv1",         at_cmd_wifi_cwscan_v1,     "Wi-Fi Scan v1" }, //sample to call to new api
    { NULL,                  NULL,                     NULL},
};
#endif

/*
 * @brief Interface Initialization for patch functions
 *
 */
void at_cmd_patch_init(void)
{
    /** Sample to do patch */

    //Sample1: Replace the parser, it's working
    //at_cmd_parse = at_cmd_parse_impl_v1;

    //Sample2: Replace a function, it's working
    //gAtCmdTbl[14].cmd_handle = at_cmd_wifi_cwmode_v1;

    //Sample3: Replace the command table, it's working
    //g_AtCmdTbl_Ptr = gAtCmdTbl_v1;
}

/*
 *
  How to use it?
  In main_patch.c, in __Patch_EntryPoint(), call at_cmd_patch_init() to reassign the golbal pointer for AT CMD
*/

