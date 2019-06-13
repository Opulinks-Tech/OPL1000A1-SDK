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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "at_cmd.h"
#include "at_cmd_sys.h"
#include "at_cmd_common.h"
#include "at_cmd_data_process.h"
#include "at_cmd_table_ext.h"
#include "common.h"
#include "sys_common_api.h"
#include "hal_flash.h"
#include "at_cmd_task.h"
#include "at_cmd_common_patch.h"
#include "hal_uart.h"
#include "hal_dbg_uart.h"
#include "wifi_types.h"
#include "wifi_api.h"
#include "data_flow_patch.h"
#include "hal_system.h"
#include "hal_tick.h"
#include "mw_fim_default_group03.h"
#include "mw_fim_default_group03_patch.h"
#include "sys_cfg.h"
#include "at_cmd_task_patch.h"

//#define AT_FLASH_CHECK_BEFORE_WRITE
//#define AT_DEBUG
//#define AT_LOG                      msg_print_uart1
#define AT_LOG(...)

#if defined(__AT_CMD_SUPPORT__)

#define AT_FLASH_READ_START         0x00000000
#define AT_FLASH_READ_END           0x00100000
#define AT_FLASH_WRITE_START        0x00000000
#define AT_FLASH_WRITE_END          0x00100000
#define AT_FLASH_WRITE_ARGS_MAX     ((AT_RBUF_SIZE - 18 - 1) / 2) // (AT_RBUF_SIZE - length of "at+writeflash=x,yy") / 2
#define AT_FLASH_BUF_SIZE           32

uint32_t g_u32FlashReadStart = AT_FLASH_READ_START;
uint32_t g_u32FlashReadEnd = AT_FLASH_READ_END;
uint32_t g_u32FlashWriteStart = AT_FLASH_WRITE_START;
uint32_t g_u32FlashWriteEnd = AT_FLASH_WRITE_END;

#endif /* __AT_CMD_SUPPORT__ */

extern volatile uint8_t g_u8RfCmdRun;
extern T_RfCmd g_tRfCmd;
extern int rf_cmd_param_alloc(int iArgc, char *saArgv[]);
extern void rf_cmd_param_free(void);

#define CMD_TOKEN_SIZE  16
extern void Cmd_TokenParse(char* pszData, uint8_t* pubCount, uint8_t* pubaToken[]);


#if defined(__AT_CMD_SUPPORT__)

int at_cmd_sys_mac_addr_def(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int ret;
    u8 ret_st = true;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode) {
        case AT_CMD_MODE_READ:
        {
            u8 sta_type, ble_type;
            
            ret = mac_addr_get_config_source(MAC_IFACE_WIFI_STA, (mac_source_type_t *)&sta_type);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
            
            ret = mac_addr_get_config_source(MAC_IFACE_BLE, (mac_source_type_t *)&ble_type);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
            
            msg_print_uart1("\r\n+MACADDRDEF:%d,%d,%d,%d\r\n", MAC_IFACE_WIFI_STA, sta_type,
                             MAC_IFACE_BLE - 1, ble_type);
        }
            break;
        case AT_CMD_MODE_SET:
        {
            u8 iface, type;
            if (argc != 3) {
                ret_st = false;
                goto done;
            }
            
            iface = atoi(argv[1]);
            type = atoi(argv[2]);
            
            //Skip SoftAP mode since we have not support it.
            if (iface > MAC_IFACE_BLE - 1) {
                ret_st = false;
                goto done;
            }
            
            if (type > MAC_SOURCE_FROM_FLASH) {
                ret_st = false;
                goto done;
            }

            //AT command : 0 = STA, 1 = BLE
            if (iface == 0) {
                ret = mac_addr_set_config_source(MAC_IFACE_WIFI_STA, (mac_source_type_t)type);
            }
            else if (iface == 1) {
                ret = mac_addr_set_config_source(MAC_IFACE_BLE, (mac_source_type_t)type);
            }
            
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
        }
            break;
        default:
            ret_st = false;
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

int at_cmd_tcp_dhcp_arp_check(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    u8 ret_st = 0;
    u8 arp_mode;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            tcp_get_config_dhcp_arp_check(&arp_mode);
            msg_print_uart1("\r\n+DHCPARPCHK:%d\r\n", arp_mode);
            
            ret_st = 1;
            break;
        case AT_CMD_MODE_SET:
            if (argc != 2) {
                goto done;
            }
            
            arp_mode = atoi(argv[1]);
            if (arp_mode > 1) {
                goto done;
            }
            
            if (tcp_set_config_dhcp_arp_check(arp_mode) != 0) {
                goto done;
            }
            
            ret_st = 1;
            
            break;
        default:
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

int at_cmd_mac_data_rate(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    u8 ret_st = 0;
    u8 data_rate;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);

    switch(mode)
    {
        case AT_CMD_MODE_READ:
            wifi_config_get_mac_tx_data_rate((wifi_mac_data_rate_t *)&data_rate);
            msg_print_uart1("\r\n+MACDATARATE:%d\r\n", data_rate);
            ret_st = 1;
            break;
        case AT_CMD_MODE_SET:
            if (argc != 2) {
                goto done;
            }

            data_rate = atoi(argv[1]);
            if (data_rate > WIFI_MAC_DATA_RATE_11M) {
                goto done;
            }

            if (wifi_config_set_mac_tx_data_rate((wifi_mac_data_rate_t)data_rate) != 0) {
                goto done;
            }
            
            ret_st = 1;
            break;
        default:
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

int at_cmd_sys_phy_dump(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 2)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_PHY_DUMP;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_reload_ow(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 1)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_RELOAD_OW;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_store_ow(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 1)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_STORE_OW;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_show_ow(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 2)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_SHOW_OW;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_add_ow(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 4)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_ADD_OW;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_del_ow(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 3)
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_DEL_OW;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

int at_cmd_sys_wifi_only(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_READ:
        {
            T_WifiCfg tCfg = {0};
            
            if(sys_cfg_wifi_get(&tCfg))
            {
                goto done;
            }

            msg_print_uart1("WiFi_Only[%u]\r\n", tCfg.u8WifiOnly);
            break;
        }

        case AT_CMD_MODE_SET:
        {
            T_WifiCfg tCfg = {0};

            if(argc < 2)
            {
                AT_LOG("invalid param number\r\n");
                goto done;
            }
            
            if(sys_cfg_wifi_get(&tCfg))
            {
                goto done;
            }

            tCfg.u8WifiOnly = strtoul(argv[1], NULL, 0);

            if(sys_cfg_wifi_set(&tCfg, 1))
            {
                goto done;
            }

            break;
        }

        default:
            goto done;
    }

    iRet = 1;

done:
    if(iRet)
    {
        msg_print_uart1("OK\r\n");
    }
    else
    {
        msg_print_uart1("ERROR\r\n");
    }
    
    return iRet;
}
#endif /* __AT_CMD_SUPPORT__ */

int at_cmd_sys_rf_hp(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int ret;
    u8 ret_st = true;
    
    _at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS);
    
    switch(mode) {
        case AT_CMD_MODE_READ:
        {
            u8 level = 0;
            sys_get_config_rf_power_level(&level);
            msg_print_uart1("\r\n+RFHP:%d\r\n", level);
        }
            break;
        case AT_CMD_MODE_SET:
        {
            u8 level = 0;
            
            if (argc != 2) {
                ret_st = false;
                goto done;
            }
            
            level = atoi(argv[1]);
            
            ret = sys_set_config_rf_power_level(level);
            if (ret != 0) {
                ret_st = false;
                goto done;
            }
        }
            break;
        default:
            ret_st = false;
            break;
    }

done:
    if (ret_st)
        msg_print_uart1("\r\nOK\r\n");
    else 
        msg_print_uart1("\r\nError\r\n");
    
    return ret_st;
}

int at_cmd_sys_rf_test_mode(char *buf, int len, int mode)
{
    int iRet = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    int argc = 0;

    if(g_u8RfCmdRun)
    {
        at_output("RF cmd is running\r\nERROR\r\n");
        goto ignore;
    }

    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if((argc < 2) || (argc > RF_CMD_PARAM_NUM))
    {
        at_output("invalid param number\r\n");
        goto done;
    }

    if(rf_cmd_param_alloc(argc, argv))
    {
        goto done;
    }

    g_tRfCmd.u32Type = RF_EVT_TEST_MODE;
    g_u8RfCmdRun = 1;
    IPC_CMD_SEND(RF_CMD_EVT, (void*)&g_tRfCmd, sizeof(g_tRfCmd));

    iRet = 1;

done:
    if(!iRet)
    {
        rf_cmd_param_free();

        at_output("ERROR\r\n");
    }

ignore:
    return iRet;
}

#if defined(__AT_CMD_SUPPORT__)
int at_cmd_sys_read_flash(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 3)
    {
        AT_LOG("invalid param number\r\n");
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_SET:
        {
            E_SpiIdx_t u32SpiIdx = SPI_IDX_0;
            uint32_t u32Addr = (uint32_t)strtoul(argv[1], NULL, 16);
            uint32_t u32Size = (uint32_t)strtoul(argv[2], NULL, 0);
            uint32_t u32End = 0;
            uint32_t i = 0;
            uint8_t u8aReadBuf[AT_FLASH_BUF_SIZE] = {0};
            uint32_t u32BufSize = AT_FLASH_BUF_SIZE;
            uint32_t u32Offset = 0;

            if((u32Addr < g_u32FlashReadStart) || (u32Addr > g_u32FlashReadEnd))
            {
                AT_LOG("invalid address[%#x]\r\n", u32Addr);
                goto done;
            }

            u32End = u32Addr + u32Size - 1;

            if((u32Size == 0) || (u32End >= g_u32FlashReadEnd))
            {
                AT_LOG("invalid size[%d]\r\n", u32Size);
                goto done;
            }

            while(u32Offset < u32Size)
            {
                uint32_t u32ProcSize = 0;

                if(u32Offset + u32BufSize < u32Size)
                {
                    u32ProcSize = u32BufSize;
                }
                else
                {
                    u32ProcSize = u32Size - u32Offset;
                }

                AT_LOG("\r\nread_addr[%08X] offset[%08X] size[%u]\r\n", u32Addr, u32Offset, u32ProcSize);

                if(Hal_Flash_AddrRead(u32SpiIdx, u32Addr + u32Offset, 0, u32ProcSize, u8aReadBuf))
                {
                    AT_LOG("Hal_Flash_AddrRead fail\r\n");
                    goto done;
                }

                for(i = 0; i < u32ProcSize; i++)
                {
                    if((u32Offset) || (i))
                    {
                        msg_print_uart1(",");
                    }
    
                    msg_print_uart1("%02X", u8aReadBuf[i]);
                }

                u32Offset += u32ProcSize;
            }

            msg_print_uart1("\r\n");
            break;
        }

        default:
            goto done;
    }

    iRet = 1;

done:
    if(iRet)
    {
        msg_print_uart1("OK\r\n");
    }
    else
    {
        msg_print_uart1("ERROR\r\n");
    }
    
    return iRet;
}

int at_cmd_sys_write_flash(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char **argv = NULL;
    uint32_t u32ArgBufSize = AT_FLASH_WRITE_ARGS_MAX * sizeof(char *);
    uint8_t *baArgBuf = NULL;

    baArgBuf = (uint8_t *)malloc(u32ArgBufSize);

    if(!baArgBuf)
    {
        AT_LOG("malloc fail\r\n");
        goto done;
    }

    argv = (char **)baArgBuf;
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_FLASH_WRITE_ARGS_MAX))
    {
        goto done;
    }

    if(argc < 4)
    {
        AT_LOG("invalid param number\r\n");
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_SET:
        {
            E_SpiIdx_t u32SpiIdx = SPI_IDX_0;
            uint32_t u32Addr = (uint32_t)strtoul(argv[1], NULL, 16);
            uint32_t u32Size = (uint32_t)strtoul(argv[2], NULL, 0);
            uint32_t u32End = 0;
            uint32_t i = 0;
            uint8_t u8aWriteBuf[AT_FLASH_BUF_SIZE] = {0};
            uint8_t u8aReadBuf[AT_FLASH_BUF_SIZE] = {0};
            uint32_t u32BufSize = AT_FLASH_BUF_SIZE;
            uint32_t u32Offset = 0;
            uint8_t u8ValueArgOffset = 3; // skip at+writeflash, address and size

        #ifdef AT_FLASH_CHECK_BEFORE_WRITE
            uint8_t u8NeedErase = 0;
            uint32_t u32EraseStart = 0;
            uint32_t u32EraseEnd = 0;
        #endif

            #ifdef AT_DEBUG
            uint8_t u8Dump = 0;
            #endif

            if((u32Addr < g_u32FlashWriteStart) || (u32Addr >= g_u32FlashWriteEnd))
            {
                AT_LOG("invalid address[%#x]\r\n", u32Addr);
                goto done;
            }

            u32End = u32Addr + u32Size - 1;

            if((u32Size == 0) || (u32End >= g_u32FlashWriteEnd))
            {
                AT_LOG("invalid size[%d]\r\n", u32Size);
                goto done;
            }

        #ifdef AT_FLASH_CHECK_BEFORE_WRITE
            // read and check
            while(u32Offset < u32Size)
            {
                uint32_t u32ProcSize = 0;

                #ifdef AT_DEBUG
                u8Dump = 0;
                #endif

                if(u32Offset + u32BufSize < u32Size)
                {
                    u32ProcSize = u32BufSize;
                }
                else
                {
                    u32ProcSize = u32Size - u32Offset;
                }

                #ifdef AT_DEBUG
                if((!u32Offset) || ((u32Offset + u32ProcSize) >= u32Size))
                {
                    u8Dump = 1;
                    AT_LOG("\r\nread_addr[%08X] offset[%08X] size[%u]\r\n", u32Addr, u32Offset, u32ProcSize);
                }
                #endif

                if(Hal_Flash_AddrRead(u32SpiIdx, u32Addr + u32Offset, 0, u32ProcSize, u8aReadBuf))
                {
                    AT_LOG("Hal_Flash_AddrRead fail\r\n");
                    goto done;
                }

                #ifdef AT_DEBUG
                if(u8Dump)
                {
                    AT_LOG("check: ");
                }
                #endif

                for(i = 0; i < u32ProcSize; i++)
                {
                    #ifdef AT_DEBUG
                    if(u8Dump)
                    {
                        AT_LOG(", %02X", u8aReadBuf[i]);
                    }
                    #endif

                    if(u8aReadBuf[i] != 0xFF)
                    {
                        if(u8NeedErase)
                        {
                            u32EraseEnd = u32Addr + u32Offset + i;
                        }
                        else
                        {
                            u32EraseStart = u32Addr + u32Offset + i;
                            u32EraseEnd = u32EraseStart;
                            u8NeedErase = 1;
                        }
                    }
                }

                u32Offset += u32ProcSize;
            }

            #ifdef AT_DEBUG
            if(u8Dump)
            {
                AT_LOG("\r\n");
            }
            #endif

            // erase
            if(u8NeedErase)
            {
                uint32_t u32EraseUnit = 0x1000; // 4K

                u32EraseStart &= ~(u32EraseUnit - 1);
                u32EraseEnd &= ~(u32EraseUnit - 1);

                while(u32EraseStart <= u32EraseEnd)
                {
                    AT_LOG("\r\nerase_addr[%08X]\r\n", u32EraseStart);

                    if(Hal_Flash_4KSectorAddrErase(u32SpiIdx, u32EraseStart))
                    {
                        AT_LOG("Hal_Flash_4KSectorAddrErase fail\r\n");
                        goto done;
                    }

                    u32EraseStart += u32EraseUnit;
                }
            }

            u32Offset = 0;
        #endif //#ifdef AT_FLASH_CHECK_BEFORE_WRITE

            // write
            while(u32Offset < u32Size)
            {
                uint32_t u32ProcSize = 0;

                #ifdef AT_DEBUG
                u8Dump = 0;
                #endif

                if(u32Offset + u32BufSize < u32Size)
                {
                    u32ProcSize = u32BufSize;
                }
                else
                {
                    u32ProcSize = u32Size - u32Offset;
                }

                for(i = 0; i < u32ProcSize; i++)
                {
                    uint32_t u32ArgIdx = u8ValueArgOffset + u32Offset + i;

                    if(u32ArgIdx >= argc)
                    {
                        u32ArgIdx = argc - 1;
                    }

                    u8aWriteBuf[i] = (uint8_t)strtoul(argv[u32ArgIdx], NULL, 16);
                }

                #ifdef AT_DEBUG
                if((!u32Offset) || ((u32Offset + u32ProcSize) >= u32Size))
                {
                    u8Dump = 1;
                    AT_LOG("\r\nwrite_addr[%08X] offset[%08X] size[%u]\r\n", u32Addr, u32Offset, u32ProcSize);
                }
                #endif

                if(Hal_Flash_AddrProgram(u32SpiIdx, u32Addr + u32Offset, 0, u32ProcSize, u8aWriteBuf))
                {
                    AT_LOG("Hal_Flash_AddrProgram fail\r\n");
                    goto done;
                }

                if(Hal_Flash_AddrRead(u32SpiIdx, u32Addr + u32Offset, 0, u32ProcSize, u8aReadBuf))
                {
                    AT_LOG("Hal_Flash_AddrRead fail\r\n");
                    goto done;
                }

                if(memcmp(u8aReadBuf, u8aWriteBuf, u32ProcSize))
                {
                    AT_LOG("data not matched\r\n");
                    goto done;
                }

                u32Offset += u32ProcSize;
            }

            #ifdef AT_DEBUG
            if(u8Dump)
            {
                AT_LOG("\r\n");
            }
            #endif
            
            break;
        }

        default:
            goto done;
    }

    iRet = 1;

done:
    if(baArgBuf)
    {
        free(baArgBuf);
    }

    if(iRet)
    {
        msg_print_uart1("OK\r\n");
    }
    else
    {
        msg_print_uart1("ERROR\r\n");
    }
    
    return iRet;
}

int at_cmd_sys_erase_flash(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    if(argc < 3)
    {
        AT_LOG("invalid param number\r\n");
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_SET:
        {
            E_SpiIdx_t u32SpiIdx = SPI_IDX_0;
            uint32_t u32Addr = (uint32_t)strtoul(argv[1], NULL, 16);
            uint32_t u32SectorNum = (uint32_t)strtoul(argv[2], NULL, 0);
            uint32_t u32EraseUnit = 0x1000; // 4K
            uint32_t u32EraseStart = 0;
            uint32_t u32EraseEnd = 0;

            if((u32Addr < g_u32FlashWriteStart) || (u32Addr >= g_u32FlashWriteEnd))
            {
                AT_LOG("invalid address[%#x]\r\n", u32Addr);
                goto done;
            }

            if(u32Addr & (u32EraseUnit - 1))
            {
                AT_LOG("invalid sector start address[%#x]\r\n", u32Addr);
                goto done;
            }

            u32EraseStart = u32Addr;
            u32EraseEnd = u32EraseStart + u32EraseUnit * u32SectorNum - 1;

            if((u32SectorNum == 0) || (u32EraseEnd >= g_u32FlashWriteEnd))
            {
                AT_LOG("invalid sector number[%d]\r\n", u32SectorNum);
                goto done;
            }

            while(u32EraseStart <= u32EraseEnd)
            {
                AT_LOG("\r\nerase_addr[%08X]\r\n", u32EraseStart);

                if(Hal_Flash_4KSectorAddrErase(u32SpiIdx, u32EraseStart))
                {
                    AT_LOG("Hal_Flash_4KSectorAddrErase fail\r\n");
                    goto done;
                }

                u32EraseStart += u32EraseUnit;
            }

            break;
        }

        default:
            goto done;
    }

    iRet = 1;

done:
    if(iRet)
    {
        msg_print_uart1("OK\r\n");
    }
    else
    {
        msg_print_uart1("ERROR\r\n");
    }
    
    return iRet;
}
#endif /* __AT_CMD_SUPPORT__ */

int at_cmd_at_switch_to_dbg(char *buf, int len, int mode)
{
    msg_print_uart1("\r\n");
    tracer_drct_printf("\r\n");
    
    at_cmd_switch_uart1_dbguart();
    
    /* Make uart host buffer clean */
    Hal_Uart_DataSend(UART_IDX_1, 0);
    Hal_DbgUart_DataSend(0);
    
    msg_print_uart1("\r\nSwitch: AT UART\r\n>");
    tracer_drct_printf("\r\nSwitch: Dbg UART\r\n>");
        
    return true;
}

int at_cmd_sys_mp_rst(char *buf, int len, int mode)
{
    extern int _at_cmd_sys_rst(char *buf, int len, int mode);
    return _at_cmd_sys_rst(buf, len, mode);
}

int at_cmd_at_slp_tmr(char *buf, int len, int mode)
{
    uint32_t u32TickStart = 0;
    uint32_t u32TickDiff = 0;
    uint64_t u64SlpTmrStart = 0;
    uint64_t u64SlpTmrEnd = 0;

    Hal_Tick_Init();
    
    u64SlpTmrStart = Hal_Sys_SleepTimerGet();
    u32TickStart = Hal_Tick_Diff( 0 );
    while( u32TickDiff < ( 1000* Hal_Tick_PerMilliSec() ) )
    {
        // busy wait here
        u32TickDiff = Hal_Tick_Diff( u32TickStart );
    }
    u64SlpTmrEnd = Hal_Sys_SleepTimerGet();
    
    msg_print_uart1("\r\n32K XTAL Freq: %ld\n\r", (uint32_t) (u64SlpTmrEnd - u64SlpTmrStart));
    tracer_drct_printf("\r\n32K XTAL Freq: %ld\n\r", (uint32_t) (u64SlpTmrEnd - u64SlpTmrStart));
    return true;
}

int at_cmd_sys_mode(char *buf, int len, int mode)
{
    T_MwFim_SysMode tSysMode;

    uint8_t ubCount;
    uint8_t* pubaToken[CMD_TOKEN_SIZE];

    if (AT_CMD_MODE_READ == mode)
    {
        msg_print_uart1("\r\n");

        // get the settings of system mode
        if (MW_FIM_OK != MwFim_FileRead(MW_FIM_IDX_GP03_PATCH_SYS_MODE, 0, MW_FIM_SYS_MODE_SIZE, (uint8_t*)&tSysMode))
        {
            // if fail, get the default value
            memcpy(&tSysMode, &g_tMwFimDefaultSysMode, MW_FIM_SYS_MODE_SIZE);
        }

        msg_print_uart1("System Mode: %d \r\n", tSysMode.ubSysMode);

        msg_print_uart1("\r\nOK\r\n");
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        msg_print_uart1("\r\n");

        // pre-parser the input string
        Cmd_TokenParse(buf, &ubCount, pubaToken);

        // set the settings of system mode
        tSysMode.ubSysMode = strtoul((const char*)pubaToken[1], NULL, 0);
        if (tSysMode.ubSysMode < MW_FIM_SYS_MODE_MAX)
        {
            if (MW_FIM_OK == MwFim_FileWrite(MW_FIM_IDX_GP03_PATCH_SYS_MODE, 0, MW_FIM_SYS_MODE_SIZE, (uint8_t*)&tSysMode))
            {
                msg_print_uart1("\r\nOK\r\n");
            }
        }
    }

    return true;
}

int at_cmd_sys_crlf_term(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_READ:
            msg_print_uart1("AT_CRLF_TERM: %u\r\n", at_cmd_crlf_term_get());
            break;

        case AT_CMD_MODE_SET:
        {
            uint8_t u8Value = 0;

            if(argc < 2)
            {
                AT_LOG("invalid param number\r\n");
                goto done;
            }

            u8Value = (uint8_t)strtoul(argv[1], NULL, 10);
            at_cmd_crlf_term_set((u8Value)?(1):(0));
            break;
        }

        default:
            goto done;
    }

    iRet = 1;

done:
    if(iRet)
    {
        msg_print_uart1("OK\r\n");
    }
    else
    {
        msg_print_uart1("ERROR\r\n");
    }
    
    return iRet;
}

/**
  * @brief extern AT Command Table for All Module
  *
  */

_at_command_t gAtCmdTbl_ext[] =
{
#if defined(__AT_CMD_SUPPORT__)
    { "at+macaddrdef",          at_cmd_sys_mac_addr_def,    "Default mac address from OTP or others storage" },
    { "at+dhcparpchk",          at_cmd_tcp_dhcp_arp_check,  "Enable/Disable DHCP ARP check mechanism"},
    { "at+macdatarate",         at_cmd_mac_data_rate,       "Control Wifi Mac Tx data rate"},
    { "at+phydump",             at_cmd_sys_phy_dump,      "PHY dump mode (for RD debug)"},
    { "at+reloadow",            at_cmd_sys_reload_ow,     "Read setting from flash"},
    { "at+storeow",             at_cmd_sys_store_ow,      "Store setting to flash"},
    { "at+showow",              at_cmd_sys_show_ow,       "Display overwrite table"},
    { "at+addow",               at_cmd_sys_add_ow,        "Add entry to overwrite table"},
    { "at+delow",               at_cmd_sys_del_ow,        "Delete entry from overwrite table"},
    { "at+wifionly",            at_cmd_sys_wifi_only,     "Disable BLE during WiFi connection"},
    { "at+readflash",           at_cmd_sys_read_flash,    "Read flash" },
    { "at+writeflash",          at_cmd_sys_write_flash,   "Write flash" },
    { "at+eraseflash",          at_cmd_sys_erase_flash,   "Erase flash" },
#endif /* __AT_CMD_SUPPORT__ */

    { "at+rfhp",                at_cmd_sys_rf_hp,         "Set RF power"},
    { "at+rftm",                at_cmd_sys_rf_test_mode,  "Set RF test mode"},
    { "at+switchdbg",           at_cmd_at_switch_to_dbg,  "AT switch to Debug UART"},
    { "at+mprst",               at_cmd_sys_mp_rst,        "Restart module (MP usage)"},
    { "at+slptmr",              at_cmd_at_slp_tmr,        "Got measured 32K XTAL freq"},
    { "at+sysmode",             at_cmd_sys_mode,          "Set the system mode"},
    { "at+crlfterm",            at_cmd_sys_crlf_term,     "Enable/disable CRLF termination"},
    { NULL,                     NULL,                     NULL},
};
