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

#ifndef _LE_APP_GATT_H_
#define _LE_APP_GATT_H_

#include "le_app.h"
#include "le_gatt_if.h"



enum
{
    GAP_IDX_SVC,

    GAP_IDX_SPPS_IN_CHAR,
    GAP_IDX_SPPS_IN_VAL,

    GAP_IDX_SPPS_OUT_CHAR,
    GAP_IDX_SPPS_OUT_VAL,
    GAP_IDX_SPPS_OUT_CFG,

	GAP_IDX_DEVICE_NAME_CHAR,
	GAP_IDX_DEVICE_NAME_VAL,

	GAP_IDX_TX_POWER_LEVEL_CHAR,
	GAP_IDX_TX_POWER_LEVEL_VAL,

	GAP_IDX_PRIVACY_FLAG_CHAR,
	GAP_IDX_PRIVACY_FLAG_VAL,
	GAP_IDX_RECONN_ADDR_CHAR,
	GAP_IDX_RECONN_ADDR_VAL,

	GAP_IDX_APPEARANCE_CHAR,
	GAP_IDX_APPEARANCE_VAL,

	GAP_IDX_CONN_PARAM_CHAR,
	GAP_IDX_CONN_PARAM_VAL,




    GAP_IDX_TOP
};

enum
{
    BPS_IDX_SVC,

    BPS_IDX_BP_MEAS_CHAR,
    BPS_IDX_BP_MEAS_VAL,
    BPS_IDX_BP_MEAS_IND_CFG,

    BPS_IDX_BP_FEATURE_CHAR,
    BPS_IDX_BP_FEATURE_VAL,

    BPS_IDX_INTM_CUFF_PRESS_CHAR,
    BPS_IDX_INTM_CUFF_PRESS_VAL,
    BPS_IDX_INTM_CUFF_PRESS_NTF_CFG,

    BPS_IDX_TOP
};

enum
{
    HOGPD_IDX_SVC,

    // Included Service
    HOGPD_IDX_INCL_SVC,

    // HID Information
    HOGPD_IDX_HID_INFO_CHAR,
    HOGPD_IDX_HID_INFO_VAL,

    // HID Control Point
    HOGPD_IDX_HID_CTNL_PT_CHAR,
    HOGPD_IDX_HID_CTNL_PT_VAL,

    // Report Map
    HOGPD_IDX_REPORT_MAP_CHAR,
    HOGPD_IDX_REPORT_MAP_VAL,
    HOGPD_IDX_REPORT_MAP_EXT_REP_REF,

    // Protocol Mode
    HOGPD_IDX_PROTO_MODE_CHAR,
    HOGPD_IDX_PROTO_MODE_VAL,

    // Boot Keyboard Input Report
    HOGPD_IDX_BOOT_KB_IN_REPORT_CHAR,
    HOGPD_IDX_BOOT_KB_IN_REPORT_VAL,
    HOGPD_IDX_BOOT_KB_IN_REPORT_NTF_CFG,

    // Boot Keyboard Output Report
    HOGPD_IDX_BOOT_KB_OUT_REPORT_CHAR,
    HOGPD_IDX_BOOT_KB_OUT_REPORT_VAL,

    // Boot Mouse Input Report
    HOGPD_IDX_BOOT_MOUSE_IN_REPORT_CHAR,
    HOGPD_IDX_BOOT_MOUSE_IN_REPORT_VAL,
    HOGPD_IDX_BOOT_MOUSE_IN_REPORT_NTF_CFG,

    // Report
    HOGPD_IDX_REPORT_CHAR,
    HOGPD_IDX_REPORT_VAL,
    HOGPD_IDX_REPORT_REP_REF,
    HOGPD_IDX_REPORT_NTF_CFG,

    HOGPD_IDX_NB,
};



void LeTestAppGattTaskHandler(TASK task, MESSAGEID id, MESSAGE message);

UINT16 LeTestAppGetServiceHandle(UINT16 svcAttId, UINT16 attId);

LE_GATT_SERVICE_T *LeTestAppGetSvcPtr(UINT16 sid);




































#endif
