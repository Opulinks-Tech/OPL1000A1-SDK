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

#include <stdlib.h>
#include <string.h>
#include "at_cmd.h"
#include "at_cmd_common.h"
#include "at_cmd_data_process.h"
#include "hal_flash.h"
#include "at_cmd_task.h"
#include "at_cmd_func_patch.h"


//#define AT_LOG                      msg_print_uart1
#define AT_LOG(...)

#define AT_FLASH_READ_START         0x00000000
#define AT_FLASH_READ_END           0x00100000
#define AT_FLASH_WRITE_START        0x00000000
#define AT_FLASH_WRITE_END          0x00100000
#define AT_FLASH_WRITE_ARGS_MAX     ((AT_RBUF_SIZE - 18 - 1) / 2) // (AT_RBUF_SIZE - length of "at+writeflash=x,yy") / 2
#define AT_FLASH_BUF_SIZE           32


int app_at_cmd_sys_read_flash(char *buf, int len, int mode)
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
            uint32_t u32FlashReadStart = AT_FLASH_READ_START;
            uint32_t u32FlashReadEnd = AT_FLASH_READ_END;

            if((u32Addr < u32FlashReadStart) || (u32Addr > u32FlashReadEnd))
            {
                AT_LOG("invalid address[%#x]\r\n", u32Addr);
                goto done;
            }

            u32End = u32Addr + u32Size - 1;

            if((u32Size == 0) || (u32End >= u32FlashReadEnd))
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

int app_at_cmd_sys_write_flash(char *buf, int len, int mode)
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
            uint32_t u32FlashWriteStart = AT_FLASH_WRITE_START;
            uint32_t u32FlashWriteEnd = AT_FLASH_WRITE_END;

            if((u32Addr < u32FlashWriteStart) || (u32Addr >= u32FlashWriteEnd))
            {
                AT_LOG("invalid address[%#x]\r\n", u32Addr);
                goto done;
            }

            u32End = u32Addr + u32Size - 1;

            if((u32Size == 0) || (u32End >= u32FlashWriteEnd))
            {
                AT_LOG("invalid size[%d]\r\n", u32Size);
                goto done;
            }

            // write
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

                for(i = 0; i < u32ProcSize; i++)
                {
                    uint32_t u32ArgIdx = u8ValueArgOffset + u32Offset + i;

                    if(u32ArgIdx >= argc)
                    {
                        u32ArgIdx = argc - 1;
                    }

                    u8aWriteBuf[i] = (uint8_t)strtoul(argv[u32ArgIdx], NULL, 16);
                }

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

int app_at_cmd_sys_erase_flash(char *buf, int len, int mode)
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
            uint32_t u32FlashWriteStart = AT_FLASH_WRITE_START;
            uint32_t u32FlashWriteEnd = AT_FLASH_WRITE_END;

            if((u32Addr < u32FlashWriteStart) || (u32Addr >= u32FlashWriteEnd))
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

            if((u32SectorNum == 0) || (u32EraseEnd >= u32FlashWriteEnd))
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

_at_command_t g_taAppAtCmd[] =
{
    { "at+readflash",   app_at_cmd_sys_read_flash,  "Read flash" },
    { "at+writeflash",  app_at_cmd_sys_write_flash, "Write flash" },
    { "at+eraseflash",  app_at_cmd_sys_erase_flash, "Erase flash" },
    { NULL,             NULL,                       NULL},
};

void app_at_cmd_add(void)
{
    at_cmd_process_func_register(g_taAppAtCmd);
    return;
}

