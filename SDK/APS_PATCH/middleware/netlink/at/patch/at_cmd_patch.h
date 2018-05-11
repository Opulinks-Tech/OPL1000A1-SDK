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
 * @file at_cmd_patch.h
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File containing declaration of at_cmd_patch api & definition of structure for reference.
 *
 */
#ifndef __AT_CMD_PATCH_H__
#define __AT_CMD_PATCH_H__
#include "at_cmd.h"
#include "common.h"
#include "wpabuf.h"
#include "wpa_cli.h"

/** Sample to do external reference */
//External Reference: Global Variable
//extern at_command_t gAtCmdTbl[];
//extern at_command_t *g_AtCmdTbl_Ptr;
//External Reference: Function
//extern int at_cmd_wifi_cwscan(int argc, char *argv[]);

void _at_cmd_func_init(void);

typedef int (*_at_cmd_parse_fp_t)(char *pbuf);
typedef int (*_at_cmd_handler_fp_t)(char *buf, int len, int mode);
typedef int (*_at_cmd_extend_fp_t)(char *buf, int len, int mode);

extern _at_cmd_parse_fp_t _at_cmd_parse;
extern _at_cmd_handler_fp_t _at_cmd_handler;
extern _at_cmd_extend_fp_t _at_cmd_extend;

/**
 * @brief AT Command Structure
 *
 */
typedef struct _at_command {
	const char *cmd; /**< Command String. */
	_at_cmd_handler_fp_t cmd_handle; /**< Command Handler. */
    const char *cmd_usage; /**< Command Description. */
}_at_command_t;


/** Function Delaration */
/*
 * @brief Interface Initialization for patch functions
 *
 */
void at_cmd_init_patch(void);

#endif //__AT_CMD_PATCH_H__

