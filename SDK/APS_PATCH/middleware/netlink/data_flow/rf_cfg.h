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

#ifndef __RF_CFG_H__
#define __RF_CFG_H__


typedef struct
{
    uint8_t u8HighPwrStatus;
} T_RfCfg;

// internal
typedef int (*T_RfCfgCommFp)(void);
typedef int (*T_RfCfgM0SetFp)(T_RfCfg *ptCfg);

// external
typedef int (*T_RfCfgInitFp)(uint8_t u8SetM0);
typedef int (*T_RfCfgSetFp)(T_RfCfg *ptCfg, uint8_t u8SetM0);
typedef int (*T_RfCfgGetFp)(T_RfCfg *ptCfg);


extern T_RfCfgInitFp rf_cfg_init;
extern T_RfCfgSetFp rf_cfg_set;
extern T_RfCfgGetFp rf_cfg_get;

void rf_cfg_pre_init_patch(void);


#endif //#ifndef __RF_CFG_H__

