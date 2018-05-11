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

#ifndef __AT_CMD_COMMOM_H__
#define __AT_CMD_COMMOM_H__
/**
 * @file at_cmd_common.h
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File containing declaration of at_cmd_common api & definition of structure for reference.
 *
 */

/**
 * @brief UART1 Mode Types
 *
 */
enum {
	UART1_MODE_AT = 0,       /**< AT Mode (default). */
	UART1_MODE_BLE_HCI = 1,  /**< BLE HCI Mode. */
	UART1_MODE_BLE_HOST = 2, /**< BLE HOST Mode (Need to confirm with BLE Protocol Stack owner that if it's necessary). */
	UART1_MODE_OTHERS = 3,   /**< Reserved */
	UART1_MODE_NUM
};

/**
 * @brief The Default is AT mode
 *
 */
#define UART1_MODE_DEFAULT UART1_MODE_AT

/**
 * @brief Function Pointer Type for API msg_print_uart1
 *
 */
typedef void (*msg_print_uart1_fp_t)(char *fmt,...);

/**
 * @brief Function Pointer Type for API uart1_mode_get
 *
 */
typedef int (*uart1_mode_get_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_mode_set
 *
 */
typedef int (*uart1_mode_set_fp_t)(int argc, char *argv[]);

/**
 * @brief Function Pointer Type for API uart1_mode_set_at
 *
 */
typedef void (*uart1_mode_set_at_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_mode_set_ble_hci
 *
 */
typedef void (*uart1_mode_set_ble_hci_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_mode_set_ble_host
 *
 */
typedef void (*uart1_mode_set_ble_host_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_mode_set_others
 *
 */
typedef void (*uart1_mode_set_others_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_mode_set_default
 *
 */
typedef void (*uart1_mode_set_default_fp_t)(void);

/**
 * @brief Function Pointer Type for API uart1_rx_int_do_at
 *
 */
typedef void (*uart1_rx_int_do_at_fp_t)(uint32_t u32Data);

/**
 * @brief Function Pointer Type for API uart1_rx_int_do_ble_hci
 *
 */
typedef void (*uart1_rx_int_do_ble_hci_fp_t)(uint32_t u32Data);

/**
 * @brief Function Pointer Type for API uart1_rx_int_do_ble_host
 *
 */
typedef void (*uart1_rx_int_do_ble_host_fp_t)(uint32_t u32Data);

/**
 * @brief Function Pointer Type for API uart1_rx_int_do_others
 *
 */
typedef void (*uart1_rx_int_do_others_fp_t)(uint32_t u32Data);

/**
 * @brief Function Pointer Type for API uart1_rx_int_handler_default
 *
 */
typedef void (*uart1_rx_int_handler_default_fp_t)(uint32_t u32Data);

/**
 * @brief Function Pointer Type for API at_clear_uart_buffer
 *
 */
typedef void (*at_clear_uart_buffer_fp_t)(void);

/**
 * @brief Extern Function msg_print_uart1
 *
 */
extern msg_print_uart1_fp_t msg_print_uart1;

/**
 * @brief Extern Function uart1_mode_get
 *
 */
extern uart1_mode_get_fp_t uart1_mode_get;

/**
 * @brief Extern Function uart1_mode_set
 *
 */
extern uart1_mode_set_fp_t uart1_mode_set;

/**
 * @brief Extern Function uart1_mode_set_at
 *
 */
extern uart1_mode_set_at_fp_t uart1_mode_set_at;

/**
 * @brief Extern Function uart1_mode_set_ble_hci
 *
 */
extern uart1_mode_set_ble_hci_fp_t uart1_mode_set_ble_hci;

/**
 * @brief Extern Function uart1_mode_set_ble_host
 *
 */
extern uart1_mode_set_ble_host_fp_t uart1_mode_set_ble_host;

/**
 * @brief Extern Function uart1_mode_set_others
 *
 */
extern uart1_mode_set_others_fp_t uart1_mode_set_others;

/**
 * @brief Extern Function uart1_mode_set_default
 *
 */
extern uart1_mode_set_default_fp_t uart1_mode_set_default;

/**
 * @brief Extern Function uart1_rx_int_do_at
 *
 */
extern uart1_rx_int_do_at_fp_t uart1_rx_int_do_at;

/**
 * @brief Extern Function uart1_rx_int_do_ble_hci
 *
 */
extern uart1_rx_int_do_ble_hci_fp_t uart1_rx_int_do_ble_hci;

/**
 * @brief Extern Function uart1_rx_int_do_ble_host
 *
 */
extern uart1_rx_int_do_ble_host_fp_t uart1_rx_int_do_ble_host;

/**
 * @brief Extern Function uart1_rx_int_do_others
 *
 */
extern uart1_rx_int_do_others_fp_t uart1_rx_int_do_others;

/**
 * @brief Extern Function uart1_rx_int_handler_default
 *
 */
extern uart1_rx_int_handler_default_fp_t uart1_rx_int_handler_default;

/**
 * @brief Extern Function at_clear_uart_buffer
 *
 */
extern at_clear_uart_buffer_fp_t at_clear_uart_buffer;

/*
 * @brief at_cmd_common_func_init - AT Common Interface Initialization (AT Common)
 *
 */
void at_cmd_common_func_init(void);

#endif /* __AT_CMD_COMMOM_H__ */

