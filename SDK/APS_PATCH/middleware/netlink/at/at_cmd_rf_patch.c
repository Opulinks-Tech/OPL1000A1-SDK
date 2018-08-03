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
 * @file at_cmd_rf.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the RF module AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_rf.h"
#include "ipc.h"
#include "data_flow.h"
#include "controller_wifi.h"
#include "at_cmd_common.h"
#include "at_cmd_rf.h"
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "controller_wifi.h"

#include "at_cmd_rf_patch.h"


extern volatile uint8_t g_u8RfCmdRun;
extern T_RfCmd g_tRfCmd;


/**
  * @brief AT Command Table for RF Module
  *
  */
extern _at_command_t _gAtCmdTbl_Rf[];

/*
 * @brief Global variable g_AtCmdTbl_Rf_Ptr retention attribute segment
 *
 */
extern RET_DATA _at_command_t *_g_AtCmdTbl_Rf_Ptr;

/*
 * @brief AT Command Interface Initialization for RF modules
 *
 */
void _at_cmd_rf_func_init_patch(void)
{
    //_g_AtCmdTbl_Rf_Ptr = _gAtCmdTbl_Rf;
    _g_AtCmdTbl_Rf_Ptr[4].cmd = "at+counters";
    return;
}

