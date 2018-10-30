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

#include <stdlib.h>
#include <stdio.h>
#include "wpa_cli_patch.h"
#include "wpa_patch.h"
#include "supplicant_task_patch.h"
#include "driver_netlink_patch.h"
#include "events_netlink_patch.h"

/*
 * wpas_patch_init - WPAS Patch Initialization
 *
 */
void wpas_init_patch(void)
{
    wpa_driver_func_init_patch();
        
    //wpa_cli_patch
    wpa_cli_func_init_patch();

    //events_netlink_patch
    wpa_events_func_init_patch();
    
    //wpa
    wpa_func_init_patch();
    
    //supplicant_task
    wpa_supplicant_task_func_init_patch();
    
    return;
}

