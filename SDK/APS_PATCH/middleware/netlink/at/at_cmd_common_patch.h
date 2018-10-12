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

#ifndef __AT_CMD_COMMOM_PATCH_H__
#define __AT_CMD_COMMOM_PATCH_H__


#include "at_cmd_common.h"
#include "hal_pin_def.h"


/**
 * @file at_cmd_common_patch.h
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File containing declaration of at_cmd_common api & definition of structure for reference.
 *
 */

 /**
 * @brief AT_VER_PATCH
 *
 */
#define AT_VER_PATCH "1.0"


void at_cmd_common_func_init_patch(void);
void at_cmd_switch_uart1_dbguart(void);
void at_io01_uart_mode_set(E_IO01_UART_MODE eMode);


#endif /* __AT_CMD_COMMOM_PATCH_H__ */

