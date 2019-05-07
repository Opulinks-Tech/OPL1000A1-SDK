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

#ifndef _AT_CMD_NVM_H_
#define _AT_CMD_NVM_H_

typedef struct  {
    uint8_t  enable;             /* whether enter transmit transparently mode */
    uint8_t  link_id;
    uint8_t  link_type;
    uint8_t  change_mode;
    uint8_t  remote_ip[64];
    int32_t  remote_port;
    int32_t  local_port;
    uint16_t keep_alive;
    uint8_t  ssl_enable;
    uint8_t  reserved;
} at_nvm_trans_config_t;


int at_cmd_nvm_trans_config_get(at_nvm_trans_config_t *cfg);
int at_cmd_nvm_trans_config_set(at_nvm_trans_config_t *cfg);
int at_cmd_nvm_cw_ble_wifi_mode_get(uint8_t *enable);
int at_cmd_nvm_cw_ble_wifi_mode_set(uint8_t *enable);

#endif /* _AT_CMD_NVM_H_ */
