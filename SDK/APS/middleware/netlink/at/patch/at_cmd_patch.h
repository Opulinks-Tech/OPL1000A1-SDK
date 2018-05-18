/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlnik Communication Corp. (C) 2017
******************************************************************************/
/**
 * @file at_cmd_patch.h
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File containing declaration of at_cmd_patch api & definition of structure for reference.
 *
 */
#ifndef __AT_CMD_PATCH_H__
#define __AT_CMD_PATCH_H__
#include "at_cmd.h"
#include "wpa_cli.h"

/** Sample to do external reference */
//External Reference: Global Variable
//extern at_command_t gAtCmdTbl[];
//extern at_command_t *g_AtCmdTbl_Ptr;
//External Reference: Function
//extern int at_cmd_wifi_cwscan(int argc, char *argv[]);

/** Function Delaration */
/*
 * @brief Interface Initialization for patch functions
 *
 */
void at_cmd_patch_init(void);

#endif //__AT_CMD_PATCH_H__

