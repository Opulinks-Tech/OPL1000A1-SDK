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

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__


typedef struct
{
    uint8_t u8HighPwrStatus;
} T_RfCfg;

typedef struct
{
    uint8_t u8WifiOnly;
} T_WifiCfg;

// internal
typedef int (*T_SysCfgCommFp)(void);
typedef int (*T_SysCfgM0SetFp)(void *pCfg);

// external
typedef int (*T_SysCfgInitFp)(uint8_t u8SetM0);
typedef int (*T_SysCfgSetFp)(void *pCfg, uint8_t u8SetM0);
typedef int (*T_SysCfgGetFp)(void *pCfg);


extern T_SysCfgCommFp sys_cfg_init;
extern T_SysCfgSetFp sys_cfg_rf_set;
extern T_SysCfgGetFp sys_cfg_rf_get;
extern T_SysCfgSetFp sys_cfg_wifi_set;
extern T_SysCfgGetFp sys_cfg_wifi_get;

void sys_cfg_pre_init_patch(void);


#endif //#ifndef __SYS_CFG_H__

