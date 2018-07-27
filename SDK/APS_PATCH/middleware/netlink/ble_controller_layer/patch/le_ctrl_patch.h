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
#define CommandOpcode_LE_Encrypt_Command                        (CommandOGF_LE_Controller_Commands << 10 | 0x0017)
#define CommandOpcode_LE_Rand_Command                           (CommandOGF_LE_Controller_Commands << 10 | 0x0018)
#define CommandOpcode_LE_Read_Local_P_256_Public_Key_Command    (CommandOGF_LE_Controller_Commands << 10 | 0x0025)
#define CommandOpcode_LE_Generate_DHKey_Command                 (CommandOGF_LE_Controller_Commands << 10 | 0x0026)
#define CommandOpcode_Vendor_Specific_Set_LE_Cfg_Command        (CommandOGF_Vendor_Commands << 10 | 0x16)

#define SubEventCode_LE_Read_Local_P_256_Public_Key_Complete_Event  0x08
#define SubEventCode_LE_DHKey_Generation_Complete_Event             0x09 
#define ENC_P256_PUBLIC_KEY_LEN     64
#define ENC_DHKEY_LEN               32

/******************************
Declaration of data structure
******************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct
{
    uint16_t hci_revision;
    uint16_t manufacturer_name;
    uint16_t lmp_pal_subversion;
    uint8_t  hci_version;
    uint8_t  lmp_pal_version;
    uint8_t bd_addr[6];
} le_cfg_t;

//	0xFF.16 Vendor-Specific Set BD_ADDR Command
typedef void (*t_send_vendor_specific_set_le_cfg_command_fp)(le_cfg_t *cfg);

typedef void (*t_le_encrypt_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_rand_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_read_local_p_256_public_key_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_generate_dhkey_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_send_le_read_local_p_256_public_key_complete_event_fp)(uint8_t status, uint8_t *key);
typedef void (*t_send_le_dhkey_generation_complete_event_fp)(uint8_t status, uint8_t *key);
typedef struct {
	le_cfg_t cfg;
} Vendor_Specific_Set_LE_Cfg_Command_Parameters;
typedef struct {
	uint8_t key[16];
	uint8_t plaintext_data[16];
} le_encrypt_command_parameters_t;
typedef struct {
	uint8_t status;
	uint8_t encrypted_data[16];
} le_encrypt_command_return_parameters_t;

typedef struct {
    uint8_t status;
    uint8_t random_number[8];
} le_rand_command_return_parameters_t;

typedef struct {
	uint8_t remote_p_256_public_key[64];
} le_generate_dhkey_command_parameters_t;
typedef struct {
    uint8_t X[32];
    uint8_t Y[32];
} le_p_256_key_t;
typedef struct {
	uint8_t Subevent_Code;
	uint8_t Status;
	uint8_t Local_P_256_Public_Key[64];
} LE_Read_Local_P_256_Public_Key_Complete_Event_Parameters;
typedef struct {
	uint8_t Subevent_Code;
	uint8_t Status;
	uint8_t DHKey[32];
} LE_DHKey_Generation_Complete_Event_Parameters;
/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern t_vendor_specific_exit_hci_mode_command_handler_fp vendor_specific_exit_hci_mode_command_handler;
extern t_vendor_specific_send_control_pdu_event_handler_fp vendor_specific_send_control_pdu_event_handler;
extern t_send_vendor_specific_set_le_cfg_command_fp send_vendor_specific_set_le_cfg_command;
extern t_le_encrypt_command_handler_fp le_encrypt_command_handler;
extern t_le_rand_command_handler_fp le_rand_command_handler;
extern t_le_read_local_p_256_public_key_command_handler_fp le_read_local_p_256_public_key_command_handler;
extern t_le_generate_dhkey_command_handler_fp le_generate_dhkey_command_handler;
extern t_send_le_read_local_p_256_public_key_complete_event_fp send_le_read_local_p_256_public_key_complete_event;
extern t_send_le_dhkey_generation_complete_event_fp send_le_dhkey_generation_complete_event;


// Sec 5: declaration of global function prototype


/******************** BLE Controller Function Implement ********************/
void le_ctrl_pre_patch_init(void);
void le_ctrl_init_patch(void);
void le_ctrl_hci_proc_tx_cmd_patch(hci_command_packet_01 *cmd_packet);

void le_enhanced_connection_complete_event_handler_patch(hci_event_packet_04 *event_packet);
//	0x3F.01 Vendor-Specific Exit HCI Mode Command
void vendor_specific_exit_hci_mode_command_handler_patch(hci_command_packet_01 *cmd_packet);
//	0x3F.16 Vendor-Specific Set BD_ADDR Command
void send_vendor_specific_set_le_cfg_command_impl(le_cfg_t *cfg);

//	vendor event handler
void vendor_specific_send_control_pdu_event_handler_patch(hci_event_packet_04 *event_packet);

void le_encrypt_command_handler_impl(hci_command_packet_01 *cmd_packet);
void le_rand_command_handler_impl(hci_command_packet_01 *cmd_packet);
void le_read_local_p_256_public_key_command_handler_impl(hci_command_packet_01 *md_packet);
void le_generate_dhkey_command_handler_impl(hci_command_packet_01 *cmd_packet);
void send_le_read_local_p_256_public_key_complete_event_impl(uint8_t status, uint8_t *key);
void send_le_dhkey_generation_complete_event_impl(uint8_t status, uint8_t *key);
/* MwFim related functions */
void le_read_cfg_from_flash(le_cfg_t *cfg);
void le_write_cfg_to_flash(le_cfg_t *cfg);
void le_write_bd_addr_to_flash(uint8_t *addr);
void ParseLeCfgCommand(char *pbuf);

void le_ctrl_util_encrypt(uint8_t *key, uint8_t *pData, uint8_t *eData);
uint8_t le_ctrl_util_gen_key_pair(uint8_t *pubKey, uint8_t *privKey);
uint8_t le_ctrl_util_gen_dhkey(uint8_t *pub_key_x, uint8_t *pub_key_y, uint8_t *priv_key, uint8_t *dhkey);
/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _LE_CTRL_PATCH_H_
