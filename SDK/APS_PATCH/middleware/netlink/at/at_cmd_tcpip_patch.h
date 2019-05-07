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

#ifndef _AT_CMD_TCPIP_PATCH_H_
#define _AT_CMD_TCPIP_PATCH_H_

#include "at_cmd_nvm.h"

#define AT_TRANS_TIMER_EVENT     0x0003

void _at_cmd_tcpip_func_init_patch(void);
int at_trans_client_init(at_nvm_trans_config_t *trans_cfg);
int at_cmd_trans_lock(void);

#endif /* _AT_CMD_TCPIP_PATCH_H_ */
