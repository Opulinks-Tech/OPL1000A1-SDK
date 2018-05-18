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

/***********************
Head Block of The File
***********************/
#ifndef _LE_CTRL_PATCH_H_
#define _LE_CTRL_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

// Sec 0: Comment block of the file


// Sec 1: Include File
#include "le_ctrl.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define CommandOpcode_Vendor_Specific_Set_Bd_Addr_Command    (CommandOGF_Vendor_Commands << 10 | 0x16)


/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct
{
    uint8_t bd_addr[6];
} le_cfg_t;

//	0xFF.16 Vendor-Specific Set BD_ADDR Command
typedef void (*t_send_vendor_specific_set_bd_addr_command_fp)(le_cfg_t *cfg);

typedef struct {
	uint8_t bd_addr[6];
} Vendor_Specific_Set_Bd_Addr_Command_Parameters;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern t_vendor_specific_send_control_pdu_event_handler_fp vendor_specific_send_control_pdu_event_handler;
extern t_send_vendor_specific_set_bd_addr_command_fp send_vendor_specific_set_bd_addr_command;


// Sec 5: declaration of global function prototype


/******************** BLE Controller Function Implement ********************/
void le_ctrl_pre_patch_init(void);
void le_ctrl_init_patch(void);

//	0x3F.16 Vendor-Specific Set BD_ADDR Command
void send_vendor_specific_set_bd_addr_command_impl(le_cfg_t *cfg);

//	vendor event handler
void vendor_specific_send_control_pdu_event_handler_patch(hci_event_packet_04 *event_packet);

/* MwFim related functions */
void le_read_cfg_from_flash(le_cfg_t *cfg);
void le_write_cfg_to_flash(le_cfg_t *cfg);
void ParseLeCfgCommand(char *pbuf);

/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _LE_CTRL_PATCH_H_
