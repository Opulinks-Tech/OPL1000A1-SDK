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

#ifndef _LE_HOST_CMD_H_
#define _LE_HOST_CMD_H_

#include "le.h"


#define LE_HOST_INIT_COMPLETE				"LE<HIC>"
#define LE_HOST_ENTER_IDLE					"LE<IDL>"
#define LE_HOST_ENTER_ADVERTISING			"LE<ADV>"
#define LE_HOST_EXIT_ADVERTISING			"LE<EAD>"
#define LE_HOST_ENTER_SCANNING				"LE<SCN>"
#define LE_HOST_EXIT_SCANNING				"LE<ESN>"
#define LE_HOST_SCAN_REPORT					"LE<SRP>"
#define LE_HOST_ENTER_CONNECTING			"LE<CNG>"
#define LE_HOST_ENTER_CONNECTION			"LE<CON>"
#define LE_HOST_ENTER_DISCONNECT			"LE<DSC>"
#define LE_HOST_RECEIVED_PEER_DATA			"LE<RPD>"
#define LE_HOST_DISPLAY_PASSKEY				"LE<DPK>"
#define LE_HOST_PASSKEY_INPUT_REQ			"LE<PKI>"
#define LE_HOST_USER_CONFIRM_DISPLAY		"LE<UCP>"
#define LE_HOST_CONN_PRAR_REQ				"LE<CPR>"
#define LE_HOST_SIGNAL_PRAR_REQ				"LE<SPR>"
#define LE_HOST_CONN_PRAR_UPDATA_CFM		"LE<CPU>"
#define LE_HOST_CONN_ENTER_CONNECTING		"LE<CNG>"
#define LE_HOST_CONN_EXIT_CONNECTING		"LE<ECN>"
#define LE_HOST_LOST_BOND_INFO				"LE<LBI>"


enum
{
	LE_HOST_CMD_ACK_SUCCESS,
	LE_HOST_CMD_LE_TASK_NOT_ACTIVE,
	LE_HOST_CMD_MEM_CAPACITY_EXCEED,
	LE_HOST_CMD_ACK_NO_SUCH_CMD,
	LE_HOST_CMD_ACK_INVALID_PARAMETER,
	LE_HOST_CMD_ACK_INVALID_STATE,
	LE_HOST_CMD_ACK_IN_PROGRESS,
	LE_HOST_CMD_ACK_FAIL,
};


void ParseLeHostCommand(char* pszData, int dataLen);

void LeTestAppSendPasskeyDisplay(UINT32 passkey);

void LeTestAppSendUserConfirmDisplay(UINT32 passkey);

void LeTestAppNumToStr(void *data, UINT16 len, UINT8 **p);

#ifdef LE_HOST_USE_CMD
void LeTestAppProcessCliCmd(void);
void LeTestAppSendCmdRsp(const UINT8 *rsp);

#define LeHostSendCmdRsp(x)		LeTestAppSendCmdRsp(x)
#define NumToStr(x, y, z)		LeTestAppNumToStr(x, y, z)

#else
#define LeTestAppProcessCliCmd();
#define LeHostSendCmdRsp(x);
#define NumToStr(x, y, z);

#endif

#endif
