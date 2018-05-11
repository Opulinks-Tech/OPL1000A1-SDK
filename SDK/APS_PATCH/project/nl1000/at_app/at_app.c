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
#include <string.h>
#include <stdio.h>
#include "opl_at.h"

#define CONFIG_MAX_SOCKETS_NUM      5

void at_task_init(void)
{
    opl_at_module_init(CONFIG_MAX_SOCKETS_NUM, NULL);
}

