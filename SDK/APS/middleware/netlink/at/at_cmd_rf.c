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
 * @file at_cmd_rf.c
 * @author Michael Liao
 * @date 14 Dec 2017
 * @brief File supports the RF module AT Commands.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "os.h"
#include "at_cmd.h"
#include "at_cmd_rf.h"
#include "ipc.h"
#include "data_flow.h"
#include "controller_wifi.h"
#include "at_cmd_common.h"

/*
 * @brief Command at+mode
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_mode(int argc, char *argv[])
{
    /** at+mode=3 | 4 --> WIFI | BLE */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_MODE;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;

    uint32_t ui32Data = strtol(argv[1], NULL, 10);
    switch(ui32Data)
    {
        case WIFI_STA_MODE:
            st_rf_cmd.u32cmd_type = WIFI_STA_MODE;
            IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
            printf("Mode is STA\r\n");
            msg_print_uart1("Mode is STA\r\n");
            break;
        case WIFI_AP_MODE:
            st_rf_cmd.u32cmd_type = WIFI_AP_MODE;
            IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
            printf("Mode is AP\r\n");
            msg_print_uart1("Mode is AP\r\n");
            break;
        case WIFI_AP_STA_MODE:
            st_rf_cmd.u32cmd_type = WIFI_AP_STA_MODE;
            IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
            printf("Mode is STA+AP\r\n");
            msg_print_uart1("Mode is STA+AP\r\n");
            break;
        case WIFI_RF_MODE:
            st_rf_cmd.u32cmd_type = WIFI_RF_MODE;
            st_rf_cmd.u8Trigger_wifi_tx = 1;
            ipc_enable(0);
            IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
            printf("Mode is RF\r\n");
            msg_print_uart1("Mode is RF\r\n");
            break;
        case BLE_DTM_MODE:
#if 0
            st_rf_cmd.u32cmd_type = BLE_DTM_MODE;
            st_rf_cmd.u8media_access = 3; /** BLE_DTM_ACCESS */
            st_rf_cmd.u8Trigger_wifi_tx = 0;
            IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
            printf("Mode is BLE DTM\r\n");
#endif //#if 0
            break;
        case CMD_MODE:
            break;
        default: break;
    }

    return true;
}

/*
 * @brief Command at+go
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_go(int argc, char *argv[])
{
    /** at+go=1,30,40,1 */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_GO;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;
    st_rf_cmd.u32Pramble = strtol(argv[1], NULL, 10);
    st_rf_cmd.u32DataLen = strtol(argv[2], NULL, 10);
    st_rf_cmd.u32Interval = strtol(argv[3], NULL, 10);
    st_rf_cmd.fDataRate = strtof(argv[4], NULL);

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+channel
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_channel(int argc, char *argv[])
{
    /** at+channel=2412 */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_CHANNEL;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;
    st_rf_cmd.u32Channel = strtol(argv[1], NULL, 10);

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+at_cmd_rf_resetcnts
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_resetcnts(int argc, char *argv[])
{
    /** at+reset_cnts */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_RX_RESET_CNTS;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+counters?
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_counters(int argc, char* argv[])
{
    /** at+counters */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_RX_COUNTERS;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+tx
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_tx(int argc, char *argv[])
{
    /** at+tx=1 | 0 */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_WIFI_TX;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;
    st_rf_cmd.u8TXEnable = strtol(argv[1], NULL, 10);

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+rx
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_rx(int argc, char *argv[])
{
    /** at+rx=1 | 0 */
    rf_cmd_t st_rf_cmd = {0};
    st_rf_cmd.u8TestMode = TM_WIFI_RX;
    st_rf_cmd.u8media_access = 1; /** WIFI_ACCESS */
    st_rf_cmd.u8Trigger_wifi_tx = 0;
    st_rf_cmd.u8RXEnable = strtol(argv[1], NULL, 10);

    IPC_CMD_SEND(RF_CMD_EVT, (void*)&st_rf_cmd, sizeof(rf_cmd_t));
    printf("ok\r\n");
    msg_print_uart1("ok\r\n");

    return true;
}

/*
 * @brief Command at+rfstart
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_start(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+rfend
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_end(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command at+rx
 *
 * @param [in] argc count of parameters
 *
 * @param [in] argv parameters array
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_rsv(int argc, char *argv[])
{
    return true;
}

/*
 * @brief Command Sample to do RF test
 *
 * @return 0 fail 1 success
 *
 */
int at_cmd_rf_sample(void)
{
    /** call RF at cmd "at+channel" to configure the channel */

    return true;
}

/**
  * @brief AT Command Table for RF Module
  *
  */
at_command_t gAtCmdTbl_Rf[] =
{
    { "at+mode",                at_cmd_rf_mode,           "Switch standard mode, debug mode" },
    { "at+go",                  at_cmd_rf_go,             "Setup Wi-Fi preamble, data length, interval and data rate" },
    { "at+channel",             at_cmd_rf_channel,        "Setup Wi-Fi channel number" },
    { "at+reset_cnts",          at_cmd_rf_resetcnts,      "Reset Wi-Fi rx receive pkt counter" },
    { "at+counters?",           at_cmd_rf_counters,       "read pkt continues" },
    { "at+tx",                  at_cmd_rf_tx,             "Enable/Disable continue tx" },
    { "at+rx",                  at_cmd_rf_rx,             "Enable/Disable continue rx" },
    { "at+rfstart",             at_cmd_rf_start,          "RF Start" },    //Back Door
    { "at+rfend",               at_cmd_rf_end,            "RF End" },      //Back Door
    { "at+rx",                  at_cmd_rf_rsv,            "RF Reserved" }, //Back Door
    { NULL,                     NULL,                     NULL},
};

/*
 * @brief Global variable g_AtCmdTbl_Rf_Ptr retention attribute segment
 *
 */
RET_DATA at_command_t *g_AtCmdTbl_Rf_Ptr;

/*
 * @brief AT Command Interface Initialization for RF modules
 *
 */
void at_cmd_rf_func_init(void)
{
    /** Command Table (RF) */
    g_AtCmdTbl_Rf_Ptr = gAtCmdTbl_Rf;
}

