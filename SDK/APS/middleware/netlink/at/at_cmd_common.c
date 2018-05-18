/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlnik Communication Corp. (C) 2017
******************************************************************************/
/**
 * @file at_cmd_common.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File contains the AT Command common api.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include "nl1000.h"
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_wifi.h"
#include "at_cmd_ble.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_rf.h"
#include "at_cmd_pip.h"
#include "at_cmd_others.h"
#include "le_ctrl.h"
#include "hal_uart.h"
#include "at_cmd_common.h"
#include "hal_dbg_uart.h"
#include "hal_uart.h"
#if defined(__AT_CMD_TASK__)
#include "at_cmd_task.h"
#endif

/*
 * @brief Global variable msg_print_uart1 retention attribute segment
 *
 */
RET_DATA msg_print_uart1_fp_t msg_print_uart1;

/*
 * @brief Global variable uart1_mode_get retention attribute segment
 *
 */
RET_DATA uart1_mode_get_fp_t uart1_mode_get;

/*
 * @brief Global variable uart1_mode_set retention attribute segment
 *
 */
RET_DATA uart1_mode_set_fp_t uart1_mode_set;

/*
 * @brief Global variable uart1_mode_set_at retention attribute segment
 *
 */
RET_DATA uart1_mode_set_at_fp_t uart1_mode_set_at;

/*
 * @brief Global variable uart1_mode_set_ble_hci retention attribute segment
 *
 */
RET_DATA uart1_mode_set_ble_hci_fp_t uart1_mode_set_ble_hci;

/*
 * @brief Global variable uart1_mode_set_ble_host retention attribute segment
 *
 */
RET_DATA uart1_mode_set_ble_host_fp_t uart1_mode_set_ble_host;

/*
 * @brief Global variable uart1_mode_set_others retention attribute segment
 *
 */
RET_DATA uart1_mode_set_others_fp_t uart1_mode_set_others;

/*
 * @brief Global variable uart1_mode_set_default retention attribute segment
 *
 */
RET_DATA uart1_mode_set_default_fp_t uart1_mode_set_default;

/*
 * @brief Global variable uart1_rx_int_do_at retention attribute segment
 *
 */
RET_DATA uart1_rx_int_do_at_fp_t uart1_rx_int_do_at;

/*
 * @brief Global variable uart1_rx_int_do_ble_hci retention attribute segment
 *
 */
RET_DATA uart1_rx_int_do_ble_hci_fp_t uart1_rx_int_do_ble_hci;

/*
 * @brief Global variable uart1_rx_int_do_ble_host retention attribute segment
 *
 */
RET_DATA uart1_rx_int_do_ble_host_fp_t uart1_rx_int_do_ble_host;

/*
 * @brief Global variable uart1_rx_int_handler_default retention attribute segment
 *
 */
RET_DATA uart1_rx_int_handler_default_fp_t uart1_rx_int_handler_default;

/*
 * @brief Global variable uart1_rx_int_do_others retention attribute segment
 *
 */
RET_DATA uart1_rx_int_do_others_fp_t uart1_rx_int_do_others;

/*
 * @brief Global variable at_clear_uart_buffer retention attribute segment
 *
 */
RET_DATA at_clear_uart_buffer_fp_t at_clear_uart_buffer;

/*
 * @brief Global variable g_uart1_mode retention attribute segment
 *
 */
RET_DATA unsigned int g_uart1_mode;

/*
 * @brief Global variable at_rx_buf retention attribute segment
 *
 */
RET_DATA at_uart_buffer_t at_rx_buf;

/*
 * @brief Print Message to UART1
 *
 * @param [in] fmt message
 */
void msg_print_uart1_impl(char *fmt,...)
{
	va_list ap;
    char string[256];
    char *pt;

	{
		va_start(ap,fmt);
    	vsprintf(string,fmt,ap);

	    pt = &string[0];
        while(*pt != '\0')
        {
            if(*pt == '\n') {
                Hal_Uart_DataSend(UART_IDX_1, '\r');
            }
            Hal_Uart_DataSend(UART_IDX_1, *pt++);
        }

    	va_end(ap);
	}
}

/*
 * @brief Get UART1 mode
 *
 * @return int Current UART1 mode
 */
int uart1_mode_get_impl(void)
{
    return g_uart1_mode;
}

/*
 * @brief UART1 Mode Setting's Handler
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int uart1_mode_set_impl(int argc, char *argv[])
{
    int mode = UART1_MODE_DEFAULT;

    if(argc > 1) {
        mode = atoi(argv[1]);
        if ((mode >= UART1_MODE_NUM) || (mode < UART1_MODE_AT)) return false;
    }
    g_uart1_mode = mode;

    switch (mode) {
    	case UART1_MODE_AT:
            /** Do something when set AT mode */
            uart1_mode_set_at();
    		break;
        case UART1_MODE_BLE_HCI:
            /** Do something when set BLE HCI mode */
            uart1_mode_set_ble_hci();
    		break;
        case UART1_MODE_OTHERS:
            /** Do something when set Others mode */
            uart1_mode_set_others();
            break;
    	default:
    		break;
	}

    return true;
}

/*
 * @brief UART1 AT Mode's Configuration
 *
 */
void uart1_mode_set_at_impl(void)
{
    /** Nothing to do */
}

/*
 * @brief UART1 BLE HCI Mode's Configuration
 *
 */
void uart1_mode_set_ble_hci_impl(void)
{
    le_ctrl_data.is_uart_hci_mode = (g_uart1_mode == UART1_MODE_BLE_HCI);
}

/*
 * @brief UART1 BLE HOST Mode's Configuration ()
 *
 */
void uart1_mode_set_ble_host_impl(void)
{
    /** Reserved */
}

/*
 * @brief UART1 BLE HCI Mode's Configuration
 *
 */
void uart1_mode_set_others_impl(void)
{
    /** Reserved */
}

/*
 * @brief Configure the UART1 to default UART AT Mode
 *
 */
void uart1_mode_set_default_impl(void)
{
    Hal_Uart_RxCallBackFuncSet(UART_IDX_1, uart1_rx_int_handler_default);
    Hal_Uart_RxIntEn(UART_IDX_1, 1);
}

/*
 * @brief AT mode handler
 *
 * @param [in] u32Data 4bytes data from UART1 interrupt
 */
void uart1_rx_int_do_at_impl(uint32_t u32Data)
{
    at_uart_buffer_t *p;
    xATMessage txMsg;

    p = &at_rx_buf;
    if ((p->in & ~(AT_RBUF_SIZE-1)) == 0)
    {
        if((u32Data & 0xFF) == 0x0D)
        {
            p->buf [p->in & (AT_RBUF_SIZE-1)] = 0x00;

            /** send message */
            txMsg.pcMessage = (char *)p;
            txMsg.length = sizeof(at_uart_buffer_t);
            txMsg.event = AT_UART1_EVENT;

            /** Post the byte. */
            at_task_send( txMsg );
        }
        else if((u32Data & 0xFF) == 0x08)
        {
            if (p->in > 0)
            {
                p->in--;
                p->buf[p->in & (AT_RBUF_SIZE-1)] = 0x00;
                Hal_Uart_DataSend(UART_IDX_1, 0x08);
                Hal_Uart_DataSend(UART_IDX_1, 0x20);
                Hal_Uart_DataSend(UART_IDX_1, 0x08);
            }
        }
        else
        {
            p->buf [p->in & (AT_RBUF_SIZE-1)] = (u32Data & 0xFF);
            Hal_Uart_DataSend(UART_IDX_1, p->buf [p->in]);
            p->in++;
        }
    }
    else
    {
        p->in = 0;
        p->buf [p->in & (AT_RBUF_SIZE-1)] = (u32Data & 0xFF);
        Hal_Uart_DataSend(UART_IDX_1, p->buf [p->in]);
    }

}

/*
 * @brief BLE HCI mode handler
 *
 * @param [in] u32Data 4bytes data from UART1 interrupt
 */
void uart1_rx_int_do_ble_hci_impl(uint32_t u32Data)
{
    le_ctrl_hci_uart_input(u32Data);
}

/*
 * @brief BLE HOST mode handler (Reserved)
 *
 * @param [in] u32Data 4bytes data from UART1 interrupt
 */
void uart1_rx_int_do_ble_host_impl(uint32_t u32Data)
{
    /** Reserved */
}

/*
 * @brief Others mode handler (Reserved)
 *
 * @param [in] u32Data 4bytes data from UART1 interrupt
 */
void uart1_rx_int_do_others_impl(uint32_t u32Data)
{
    /** Reserved */
}

/*
 * @brief UART1 RX Interrupt default handler
 *
 * @param [in] u32Data 4bytes data from UART1 interrupt
 */
void uart1_rx_int_handler_default_impl(uint32_t u32Data)
{
    int mode;

    mode = uart1_mode_get();
    switch (mode) {
    	case UART1_MODE_AT:
            /** Do AT CMD */
            uart1_rx_int_do_at(u32Data);
    		break;
        case UART1_MODE_BLE_HCI:
            /** Do BLE HCI */
            uart1_rx_int_do_ble_hci(u32Data);
    		break;
        case UART1_MODE_BLE_HOST:
            /** Do BLE HOST (Reserved) */
            //uart1_rx_int_do_ble_host(u32Data);
    		break;
        case UART1_MODE_OTHERS:
            /** Do Others (Reserved) */
            uart1_rx_int_do_others(u32Data);
            break;
    	default:
            /** Do AT CMD */
            uart1_rx_int_do_at(u32Data);
    		break;
	}
}

/*
 * @brief buffer flag clear
 *
 */
void at_clear_uart_buffer_impl(void)
{
    at_rx_buf.in = 0;
}

/*
 * @brief AT Common Interface Initialization (AT Common)
 *
 */
void at_cmd_common_func_init(void)
{
    /** Functions: AT Common */
    msg_print_uart1 = msg_print_uart1_impl;
    uart1_mode_get = uart1_mode_get_impl;
    uart1_mode_set = uart1_mode_set_impl;
    uart1_mode_set_at = uart1_mode_set_at_impl;
    uart1_mode_set_ble_hci = uart1_mode_set_ble_hci_impl;
    uart1_mode_set_ble_host = uart1_mode_set_ble_host_impl;
    uart1_mode_set_others = uart1_mode_set_others_impl;
    uart1_mode_set_default = uart1_mode_set_default_impl;
    uart1_rx_int_do_at = uart1_rx_int_do_at_impl;
    uart1_rx_int_do_ble_hci = uart1_rx_int_do_ble_hci_impl;
    uart1_rx_int_do_ble_host = uart1_rx_int_do_ble_host_impl;
    uart1_rx_int_do_others = uart1_rx_int_do_others_impl;
    uart1_rx_int_handler_default = uart1_rx_int_handler_default_impl;
    at_clear_uart_buffer = at_clear_uart_buffer_impl;
}

