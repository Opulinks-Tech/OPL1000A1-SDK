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

#ifndef _LE_CTRL_PATCH_H_
#define _LE_CTRL_PATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "le_ctrl.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define CommandOpcode_LE_Encrypt_Command                        (CommandOGF_LE_Controller_Commands << 10 | 0x0017)
#define CommandOpcode_LE_Rand_Command                           (CommandOGF_LE_Controller_Commands << 10 | 0x0018)
#define CommandOpcode_LE_Read_Local_P_256_Public_Key_Command    (CommandOGF_LE_Controller_Commands << 10 | 0x0025)
#define CommandOpcode_LE_Generate_DHKey_Command                 (CommandOGF_LE_Controller_Commands << 10 | 0x0026)
#define CommandOpcode_Vendor_Specific_Set_LE_Cfg_Command        (CommandOGF_Vendor_Commands << 10 | 0x16)
#define CommandOpcode_Vendor_Specific_Throughput_Test_Command   (CommandOGF_Vendor_Commands << 10 | 0x17)    

#define SubEventCode_LE_Read_Local_P_256_Public_Key_Complete_Event  0x08
#define SubEventCode_LE_DHKey_Generation_Complete_Event             0x09 
    
#define ENC_P256_PUBLIC_KEY_LEN     64
#define ENC_DHKEY_LEN               32
#define LE_TEST_MAX_LENGTH          251

/******************************
Declaration of data structure
******************************/
// Sec 3: structure, union, enum, linked list
typedef struct
{
    uint16_t hci_revision;
    uint16_t manufacturer_name;
    uint16_t lmp_pal_subversion;
    uint8_t  hci_version;
    uint8_t  lmp_pal_version;
    uint8_t  bd_addr[6];
} le_cfg_patch_t;

typedef void (*t_le_encrypt_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_rand_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_read_local_p_256_public_key_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_generate_dhkey_command_handler_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_send_le_read_local_p_256_public_key_complete_event_fp)(uint8_t status, uint8_t *key);
typedef void (*t_send_le_dhkey_generation_complete_event_fp)(uint8_t status, uint8_t *key);
typedef void (*t_send_vendor_specific_set_le_cfg_command_fp)(le_cfg_patch_t *cfg);
typedef void (*t_vendor_specific_throughput_test_command_fp)(hci_command_packet_01 *cmd_packet);
typedef void (*t_le_read_cfg_from_flash_patch_fp)(le_cfg_patch_t *cfg);
typedef void (*t_le_write_cfg_to_flash_patch_fp)(le_cfg_patch_t *cfg);

// 7.8.22 LE Encrypt Command
typedef struct {
	uint8_t key[16];
	uint8_t plaintext_data[16];
} le_encrypt_command_parameters_t;

typedef struct {
	uint8_t status;
	uint8_t encrypted_data[16];
} le_encrypt_command_return_parameters_t;

// 7.8.23 LE Rand Command 
typedef struct {
    uint8_t status;
    uint8_t random_number[8];
} le_rand_command_return_parameters_t;

// 7.8.37 LE Generate DHKey Command
typedef struct {
	uint8_t remote_p_256_public_key[64];
} le_generate_dhkey_command_parameters_t;

typedef struct {
    uint8_t X[32];
    uint8_t Y[32];
} le_p_256_key_t;

// 7.7.65.8 LE Read Local P-256 Public Key Complete event
typedef struct {
	uint8_t Subevent_Code;
	uint8_t Status;
	uint8_t Local_P_256_Public_Key[64];
} LE_Read_Local_P_256_Public_Key_Complete_Event_Parameters;

// 7.7.65.9 LE DHKey Generation Complete event
typedef struct {
	uint8_t Subevent_Code;
	uint8_t Status;
	uint8_t DHKey[32];
} LE_DHKey_Generation_Complete_Event_Parameters;

typedef struct {
	le_cfg_patch_t cfg;
} Vendor_Specific_Set_LE_Cfg_Command_Parameters_patch;

typedef struct {
    uint8_t length;
} Vendor_Specific_Throughput_Test_Command_Parameters;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable

extern t_le_encrypt_command_handler_fp le_encrypt_command_handler;
extern t_le_rand_command_handler_fp le_rand_command_handler;
extern t_le_read_local_p_256_public_key_command_handler_fp le_read_local_p_256_public_key_command_handler;
extern t_le_generate_dhkey_command_handler_fp le_generate_dhkey_command_handler;
extern t_send_le_read_local_p_256_public_key_complete_event_fp send_le_read_local_p_256_public_key_complete_event;
extern t_send_le_dhkey_generation_complete_event_fp send_le_dhkey_generation_complete_event;
extern t_send_vendor_specific_set_le_cfg_command_fp send_vendor_specific_set_le_cfg_command;
extern t_vendor_specific_throughput_test_command_fp vendor_specific_throughput_test_command_handler;
extern t_le_read_cfg_from_flash_patch_fp _le_read_cfg_from_flash;
extern t_le_write_cfg_to_flash_patch_fp _le_write_cfg_to_flash;

// Sec 5: declaration of global function prototype
/******************** BLE Controller Function Implement ********************/
void le_ctrl_pre_patch_init(void);
void le_ctrl_init_patch(void);
void le_ctrl_hci_proc_tx_cmd_patch(hci_command_packet_01 *cmd_packet);

void le_enhanced_connection_complete_event_handler_patch(hci_event_packet_04 *event_packet);

void le_encrypt_command_handler_impl(hci_command_packet_01 *cmd_packet);
void le_rand_command_handler_impl(hci_command_packet_01 *cmd_packet);
void le_read_local_p_256_public_key_command_handler_impl(hci_command_packet_01 *md_packet);
void le_generate_dhkey_command_handler_impl(hci_command_packet_01 *cmd_packet);
void send_vendor_specific_set_le_cfg_command_patch(le_cfg_patch_t *cfg);
void vendor_specific_throughput_test_command_impl(hci_command_packet_01 *cmd_packet);

void send_le_read_local_p_256_public_key_complete_event_impl(uint8_t status, uint8_t *key);
void send_le_dhkey_generation_complete_event_impl(uint8_t status, uint8_t *key);
void send_number_of_completed_packets_event_patch(uint16_t handle, uint16_t num);

// le_ctrl_data
void le_ctrl_data_hci_buf_fragment_patch(le_ctrl_conn_t *conn, hci_data_buf_t *buf);

// le_ctrl_enc 
void le_ctrl_util_encrypt(uint8_t *key, uint8_t *pData, uint8_t *eData);
uint8_t le_ctrl_util_gen_key_pair(uint8_t *pubKey, uint8_t *privKey);
uint8_t le_ctrl_util_gen_dhkey(uint8_t *pub_key_x, uint8_t *pub_key_y, uint8_t *priv_key, uint8_t *dhkey);

// le_ctrl_fim 
void le_read_cfg_from_flash_patch(le_cfg_patch_t *cfg);
void le_write_cfg_to_flash_patch(le_cfg_patch_t *cfg);
void le_write_bd_addr_to_flash_impl(uint8_t *addr);
void ParseLeCfgCommand_patch(char *pszData);

// le_ctrl_cmd
void ParseLeTestCommand(char *pbuf);
void le_ctrl_throughput_test(uint8_t tx_octets, uint8_t time);

/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


#ifdef __cplusplus
}
#endif

#endif // _LE_CTRL_PATCH_H_
