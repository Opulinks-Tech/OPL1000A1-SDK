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

#ifndef _LE_APP_H_
#define _LE_APP_H_

#include "le.h"
#include "le_host.h"
#include "le_gap_if.h"
#include "le_smp.h"


#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


enum
{
	LE_APP_USR_MSG_STOP_ADVERTISING = LE_APP_USR_MSG_BASE,
	LE_APP_USR_MSG_START_ADVERTISING,
	LE_APP_USR_MSG_STOP_SCANNING,
	LE_APP_USR_MSG_START_SCANNING,




    
	LE_APP_USR_MSG_DISCONNECTION,
	
	LE_APP_USR_MSG_SHOW_ONLY,
	LE_APP_USR_MSG_TEST_NOTIFY,
	LE_APP_USR_MSG_TEST_INDICATION,

	LE_APP_USR_MSG_TEST_HOST_CMD,
	LE_APP_USR_MSG_DUMP_MM,


	LE_APP_USR_MSG_TEST_TIMER,

	LE_APP_USR_MSG_RPA_TIMEOUT,



    
	LE_APP_USR_MSG_TOP
};

enum
{
	LE_APP_STATE_IDLE = 0,
	LE_APP_STATE_ADVERTISING = 1,
	LE_APP_STATE_SCANNING = 2,
	LE_APP_STATE_CONNECTING = 4,
	LE_APP_STATE_CONNECTED = 8,
};

enum
{
	LE_APP_ACTION_START_ADVERTISING 	= (1 << 0),
	LE_APP_ACTION_END_ADVERTISING 		= (1 << 1),
	LE_APP_ACTION_START_CONNECTING 		= (1 << 2),
	LE_APP_ACTION_CANCEL_CONNECTING 	= (1 << 3),
	LE_APP_ACTION_START_SCANNING 		= (1 << 4),
	LE_APP_ACTION_END_SCANNING          = (1 << 5),




};

typedef struct
{
	UINT8 enable;
	UINT8 duplicate;
} LE_APP_USR_MSG_SCANNING_T;








typedef struct
{
	TASKPACK		task;
	UINT16			state;
    UINT16			action;
    UINT16			flag;
	UINT8			wl_size;
	UINT16			rpa_timeout;
    
	UINT16			curr_mtu;
    UINT16			ucp_id;
    
	UINT16			len;
	UINT8			buf[256];

	UINT16			conn_hdl;
    LE_BT_ADDR_T	btaddr;
    UINT8			oob_data[16];
	BOOL			oob_present;
    
    LE_SMP_SC_OOB_DATA_T	peer_oob;
    LE_SMP_SC_OOB_DATA_T	our_oob;
} LE_APP_DATA_T;


void LeTestAppInit(void);

void LeTestAppSetAdvtisingPara(UINT8 type, UINT8 own_addr_type, LE_BT_ADDR_T *peer_addr, UINT8 filter);

LE_APP_DATA_T *LeTestAppGetInst(void);

#endif
