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
#include "blewifi_configuration.h"
#include "blewifi_ctrl.h"
#include "blewifi_data.h"

#include "agent.h"
#include "mw_fim.h"
#include "at_cmd_data_process_patch.h"

//#define AT_LOG                      msg_print_uart1
#define AT_LOG(...)

#define AT_FLASH_READ_START         0x00000000
#define AT_FLASH_READ_END           0x00100000
#define AT_FLASH_WRITE_START        0x00000000
#define AT_FLASH_WRITE_END          0x00100000
#define AT_FLASH_WRITE_ARGS_MAX     ((AT_RBUF_SIZE - 18 - 1) / 2) // (AT_RBUF_SIZE - length of "at+writeflash=x,yy") / 2
#define AT_FLASH_BUF_SIZE           32

/* For at_cmd_sys_write_fim */
#define AT_FIM_DATA_LENGTH 2 /* EX: 2 = FF */
#define AT_FIM_DATA_LENGTH_WITH_COMMA (AT_FIM_DATA_LENGTH + 1) /* EX: 3 = FF, */

typedef struct
{
    uint32_t u32Id;
    uint16_t u16Index;
    uint16_t u16DataTotalLen;
    
    uint32_t u32DataRecv;       // Calcuate the receive data
    uint32_t TotalSize;         // user need to input total bytes
    
    char     u8aReadBuf[8];
    uint8_t  *ResultBuf;
    uint32_t u32StringIndex;       // Indicate the location of reading string
    uint16_t u16Resultindex;       // Indicate the location of result string
    uint8_t  fIgnoreRestString;    // Set a flag for last one comma
    uint8_t  u8aTemp[1];
} T_AtFimParam;

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

int app_at_cmd_sys_read_fim(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};
    uint32_t i = 0;
    uint8_t *readBuf = NULL;
    uint32_t u32Id  = 0;
    uint16_t u16Index  = 0;
    uint16_t u16Size  = 0;
    
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    
    if(argc != 4)
    {
        AT_LOG("invalid param number\r\n");
        goto done;
    }
    
    u32Id  = (uint32_t)strtoul(argv[1], NULL, 16);
    u16Index  = (uint16_t)strtoul(argv[2], NULL, 0);
    u16Size  = (uint16_t)strtoul(argv[3], NULL, 0);
    
    if((u16Size == 0) )
    {
        AT_LOG("invalid size[%d]\r\n", u16Size);
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_SET:
        {
            readBuf = (uint8_t *)malloc(u16Size);
            
            if(MW_FIM_OK == MwFim_FileRead(u32Id, u16Index, u16Size, readBuf))
            {
                msg_print_uart1("%02X",readBuf[0]);
                for(i = 1 ; i < u16Size ; i++)
                {
                    msg_print_uart1(",%02X",readBuf[i]);
                }
            }
            else
            {
                goto done;
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

    if(readBuf != NULL)
        free(readBuf);
    return iRet;
}

int write_fim_handle(uint32_t u32Type, uint8_t *u8aData, uint32_t u32DataLen, void *pParam)
{
    T_AtFimParam *ptParam = (T_AtFimParam *)pParam;
    
    uint8_t  iRet = 0;
    uint8_t  u8acmp[] = ",\0";
    uint32_t i = 0;

    ptParam->u32DataRecv += u32DataLen;

    /* If previous segment is error then ignore the rest of segment */
    if(ptParam->fIgnoreRestString)
    {
        goto done;
    }
    
    for(i = 0 ; i < u32DataLen ; i++)
    {
        if(u8aData[i] != u8acmp[0])
        {
            if(ptParam->u32StringIndex >= AT_FIM_DATA_LENGTH)
            {
                ptParam->fIgnoreRestString = 1;
                goto done;
            }

            /* compare string. If not comma then store into array. */
            ptParam->u8aReadBuf[ptParam->u32StringIndex] = u8aData[i];
            ptParam->u32StringIndex++;
        }
        else
        {
            /* Convert string into Hex and store into array */
            ptParam->ResultBuf[ptParam->u16Resultindex] = (uint8_t)strtoul(ptParam->u8aReadBuf, NULL, 16);
            
            /* Result index add one */
            ptParam->u16Resultindex++;

            /* re-count when encounter comma */
            ptParam->u32StringIndex=0;
        }
    }
    
    /* If encounter the last one comma
       1. AT_FIM_DATA_LENGTH:
       Max character will pick up to compare.
       
       2. (ptParam->u16DataTotalLen - 1):
       If total length minus 1 is equal (ptParam->u16Resultindex) mean there is no comma at the rest of string.
    */	
    if((ptParam->u16Resultindex == (ptParam->u16DataTotalLen - 1)) && (ptParam->u32StringIndex >= AT_FIM_DATA_LENGTH))
    {
        ptParam->ResultBuf[ptParam->u16Resultindex] = (uint8_t)strtoul(ptParam->u8aReadBuf, NULL, 16);
        
        /* Result index add one */
        ptParam->u16Resultindex++;
    }
    
    /* Collect array data is equal to total lengh then write data to fim. */
    if(ptParam->u16Resultindex == ptParam->u16DataTotalLen)
    {
       	if(MW_FIM_OK == MwFim_FileWrite(ptParam->u32Id, ptParam->u16Index, ptParam->u16DataTotalLen, ptParam->ResultBuf))
        {
            msg_print_uart1("OK\r\n");
        }
        else
        {
            ptParam->fIgnoreRestString = 1;
        }
    }
    else
    {
        goto done;
    }
    
done:
    if(ptParam->TotalSize >= ptParam->u32DataRecv) 
    {
        if(ptParam->fIgnoreRestString)
        {
            msg_print_uart1("ERROR\r\n");
        }

        if(ptParam != NULL)
        {
            if (ptParam->ResultBuf != NULL)
            {
                free(ptParam->ResultBuf);
            }
            free(ptParam);
            ptParam = NULL;
        }
    }

    return iRet;
}

int app_at_cmd_sys_write_fim(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};

    /* Initialization the value */
    T_AtFimParam *tAtFimParam = (T_AtFimParam*)malloc(sizeof(T_AtFimParam));
    if(tAtFimParam == NULL)
    {
        goto done;
    }
    memset(tAtFimParam, 0, sizeof(T_AtFimParam));
		
    if(!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }
    
    if(argc != 4)
    {
        msg_print_uart1("invalid param number\r\n");
        goto done;
    }

    /* save parameters to process uart1 input */
    tAtFimParam->u32Id = (uint32_t)strtoul(argv[1], NULL, 16);
    tAtFimParam->u16Index = (uint16_t)strtoul(argv[2], NULL, 0);
    tAtFimParam->u16DataTotalLen = (uint16_t)strtoul(argv[3], NULL, 0);

    /* If user input data length is 0 then go to error.*/
    if(tAtFimParam->u16DataTotalLen == 0)
    {
        goto done;
    }
    
    switch(mode)
    {
        case AT_CMD_MODE_SET:
        {
            tAtFimParam->TotalSize = ((tAtFimParam->u16DataTotalLen * AT_FIM_DATA_LENGTH_WITH_COMMA) - 1);
            
            /* Memory allocate a memory block for pointer */
            tAtFimParam->ResultBuf = (uint8_t *)malloc(tAtFimParam->u16DataTotalLen);
            if(tAtFimParam->ResultBuf == NULL)
                goto done;
            
            // register callback to process uart1 input
            agent_data_handle_reg(write_fim_handle, tAtFimParam);
            
            // redirect uart1 input to callback
            data_process_lock_patch(LOCK_OTHERS, (tAtFimParam->TotalSize));

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
        if (tAtFimParam != NULL)
        {
            if (tAtFimParam->ResultBuf != NULL)
            {
		        free(tAtFimParam->ResultBuf);
            }
            free(tAtFimParam);
            tAtFimParam = NULL;
        }
    }
    
    return iRet;
}

int app_at_cmd_sys_dtim_time(char *buf, int len, int mode)
{
    int iRet = 0;
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};

    uint32_t ulDtimTime;
    
    if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
    {
        goto done;
    }

    switch (mode)
    {
        case AT_CMD_MODE_READ:
        {
            msg_print_uart1("DTIM Time: %d\r\n", BleWifi_Ctrl_DtimTimeGet());
            break;
        }
    
        case AT_CMD_MODE_SET:
        {
            // at+dtim=<value>
            if(argc != 2)
            {
                AT_LOG("invalid param number\r\n");
                goto done;
            }

            ulDtimTime = strtoul(argv[1], NULL, 0);
            BleWifi_Ctrl_DtimTimeSet(ulDtimTime);
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

#if (WIFI_OTA_FUNCTION_EN == 1)
int app_at_cmd_sys_do_wifi_ota(char *buf, int len, int mode)
{
    int argc = 0;
    char *argv[AT_MAX_CMD_ARGS] = {0};

    if (AT_CMD_MODE_EXECUTION == mode)
    {
        BleWifi_Wifi_OtaTrigReq(WIFI_OTA_HTTP_URL);
        //msg_print_uart1("OK\r\n");
    }
    else if (AT_CMD_MODE_SET == mode)
    {
        if (!_at_cmd_buf_to_argc_argv(buf, &argc, argv, AT_MAX_CMD_ARGS))
        {
            return false;
        }

        BleWifi_Wifi_OtaTrigReq((uint8_t*)(argv[1]));
        //msg_print_uart1("OK\r\n");
    }

    return true;
}
#endif

_at_command_t g_taAppAtCmd[] =
{
    { "at+readflash",   app_at_cmd_sys_read_flash,  "Read flash" },
    { "at+writeflash",  app_at_cmd_sys_write_flash, "Write flash" },
    { "at+eraseflash",  app_at_cmd_sys_erase_flash, "Erase flash" },
    { "at+readfim",     app_at_cmd_sys_read_fim,    "Read FIM data" },
    { "at+writefim",    app_at_cmd_sys_write_fim,   "Write FIM data" },
    { "at+dtim",        app_at_cmd_sys_dtim_time,   "Wifi DTIM" },
#if (WIFI_OTA_FUNCTION_EN == 1)
    { "at+ota",         app_at_cmd_sys_do_wifi_ota, "Do Wifi OTA" },
#endif
    { NULL,             NULL,                       NULL},
};

void app_at_cmd_add(void)
{
    at_cmd_process_func_register(g_taAppAtCmd);
    return;
}
