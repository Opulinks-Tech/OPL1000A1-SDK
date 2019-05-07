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
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "cmsis_os.h"

#include "at_cmd.h"
#include "at_cmd_task.h"
#include "at_cmd_common.h"
#include "at_cmd_app.h"
#include "at_cmd_tcpip.h"
#include "at_cmd_nvm.h"
#include "sys_os_config.h"

#include "at_cmd_task_patch.h"
#include "at_cmd_app_patch.h"
#include "sys_os_config_patch.h"

#define CONFIG_MAX_SOCKETS_NUM      5

#if defined(__AT_CMD_SUPPORT__)
RET_DATA at_nvm_trans_config_t gTransCfg;
extern volatile bool at_ip_mode; // 0: normal transmission mode. 1:transparent transmission
#endif

/*
 * @brief Global variable xAtQueue retention attribute segment
 *
 */
extern RET_DATA osMessageQId xAtQueue;

/*
 * @brief Global variable AtTaskHandle retention attribute segment
 *
 */
extern RET_DATA osThreadId AtTaskHandle;

/*
 * @brief Global variable AtMemPoolId retention attribute segment
 *
 */
extern RET_DATA osPoolId AtMemPoolId;

/*
 * @brief Global variable at_semaId retention attribute segment
 *
 */
extern RET_DATA osSemaphoreId at_semaId;

/** @brief Semaphore for switch Dbguart and AT UART at IO0/IO1 */
osSemaphoreId g_tSwitchuartSem;


extern const osPoolDef_t os_pool_def_atMemPool;
extern const osSemaphoreDef_t os_semaphore_def_at_sema;

/*
 * @brief An external global variable g_uart1_mode prototype declaration
 *
 */
extern unsigned int g_uart1_mode;

extern osThreadId at_app_task_id;
extern uint8_t g_wifi_init_mode;

volatile uint8_t g_u8AtCrLfTerm = 0;
char *g_sAtPendBuf = NULL;

#if defined(__AT_CMD_SUPPORT__)
void at_blewifi_auto_trans_init(void)
{
    uint8_t at_blewifi_mode = 0;
    
    at_cmd_nvm_trans_config_get(&gTransCfg);
    at_cmd_nvm_cw_ble_wifi_mode_get(&at_blewifi_mode);
    
    if (at_blewifi_mode == true) {
        g_wifi_init_mode = 4;
        
        // 1. AT BleWifi init function
        if (at_blewifi_init()) {
            tracer_log(LOG_HIGH_LEVEL, "at_blewifi_init not yet\n");
            return;
        }
        
        // 2. Create a task for transparent if link was saved.
        if (gTransCfg.enable == true) {
            at_ip_mode = true;
            at_trans_save_link_task_create();
        }
    } else {
        /* Initial AT transparent if link was saved. */
        if (gTransCfg.enable == true) {
            at_ip_mode = true;
            at_wifi_net_task_init();
        }
    }
}
#endif

void at_module_init_patch(uint32_t netconn_max, const char *custom_version)
{
    osMessageQDef_t at_queue_def;
    osThreadDef_t at_task_def;
    osSemaphoreDef_t tSemDef = {0};
    
    
    /** create task */
    at_task_def.name = OS_TASK_NAME_AT;

#if defined(__AT_CMD_SUPPORT__)
    at_task_def.stacksize = OS_TASK_STACK_SIZE_AT_CMD_SUPPORT_PATCH;
#else
    at_task_def.stacksize = OS_TASK_STACK_SIZE_AT_PATCH;
#endif
    
    at_task_def.tpriority = OS_TASK_PRIORITY_DIAG;
    at_task_def.pthread = at_task;
    AtTaskHandle = osThreadCreate(&at_task_def, (void *)AtTaskHandle);

    if(AtTaskHandle == NULL)
    {
        tracer_log(LOG_HIGH_LEVEL, "create thread fail \r\n");
        msg_print_uart1("create thread fail \r\n");
    }

    /** create memory pool */
    AtMemPoolId = osPoolCreate (osPool(atMemPool)); /** create Mem Pool */
    if (!AtMemPoolId)
    {
        tracer_log(LOG_HIGH_LEVEL, "AT Task Mem Pool create Fail \r\n"); /** MemPool object not created, handle failure */
        msg_print_uart1("AT Task Mem Pool create Fail \r\n");
    }

    /** create message queue */
    at_queue_def.item_sz = sizeof( xATMessage );
    at_queue_def.queue_sz = AT_QUEUE_SIZE;
    xAtQueue = osMessageCreate(&at_queue_def, AtTaskHandle);
    if(xAtQueue == NULL)
    {
        tracer_log(LOG_HIGH_LEVEL, "create queue fail \r\n");
        msg_print_uart1("create queue fail \r\n");
    }
    
    /* Create semaphore for switch UART */    
    g_tSwitchuartSem = osSemaphoreCreate(&tSemDef, 1);
    if (g_tSwitchuartSem == NULL)
    {
        tracer_log(LOG_HIGH_LEVEL, "create semaphore fail \r\n");
        msg_print_uart1("create swUART sema fail \r\n");
    }
    
    //move from sys_init
    uart1_mode_set_default();

    uart1_mode_set_at();
    
    //at tcpip structure initial
    at_link_init(AT_LINK_MAX_NUM);
}

int at_wifi_net_task_init(void)
{
    if (at_app_task_id == NULL) {
        at_cmd_wifi_hook();
        return 0;
    }
    else {
        msg_print_uart1("AT Wifi and TCPIP initialize already \r\n");
        return -1;
    }
}

/*
 * @brief Send message to AT CMD task
 *
 * @param [in] txMsg
 *
 * @return  0 osOK non-0 other status codes
 *
 */
osStatus at_task_send_patch(xATMessage txMsg)
{
    osStatus ret = osErrorOS;
    xATMessage *pMsg = NULL;

    /** Mem pool allocate */
    pMsg = (xATMessage *)osPoolCAlloc (AtMemPoolId); /** get Mem Block */

    if(pMsg == NULL)
    {
        goto done;
    }

    pMsg->event = txMsg.event;
    pMsg->length = txMsg.length;
    pMsg->pcMessage = NULL;

    if((txMsg.pcMessage) && (txMsg.length))
    {
        /** malloc buffer */
        pMsg->pcMessage = (void *)malloc(txMsg.length);

        if(pMsg->pcMessage == NULL)
        {
            tracer_log(LOG_HIGH_LEVEL, "at task message allocate fail \r\n");
            msg_print_uart1("at task message allocate fail \r\n");
            goto done;
        }

        memcpy((void *)pMsg->pcMessage, (void *)txMsg.pcMessage, txMsg.length);
    }

    if(osMessagePut (xAtQueue, (uint32_t)pMsg, osWaitForever) != osOK) /** Send Message */
    {
        goto done;
    }

    ret = osOK;

done:
    if(ret != osOK)
    {
        if(pMsg)
        {
            if(pMsg->pcMessage)
            {
                free(pMsg->pcMessage);
            }

            osPoolFree(AtMemPoolId, pMsg);
        }
    }

    return ret;
}

void at_cmd_crlf_term_set(uint8_t u8Enable)
{
    if(u8Enable != g_u8AtCrLfTerm)
    {
        g_u8AtCrLfTerm = u8Enable;
    }
    
    return;
}

uint8_t at_cmd_crlf_term_get(void)
{
    return g_u8AtCrLfTerm;
}

void at_term_char_remove(char *sBuf)
{
    int iLen = strlen(sBuf);
    int i = 0;

    if(!iLen)
    {
        goto done;
    }

    for(i = iLen - 1; i >= 0; i--)
    {
        if((sBuf[i] == 0x0D) || (sBuf[i] == 0x0A))
        {
            sBuf[i] = 0x00;
        }
        else
        {
            break;
        }
    }

done:
    return;
}

/*
 * @brief AT CMD task body
 *
 * @param [in] pvParameters Task parameters
 *
 */
void at_task_patch(void *pvParameters)
{
    osEvent rxEvent;
    xATMessage *rxMsg;
    at_uart_buffer_t *pData;

    tracer_log(LOG_HIGH_LEVEL, "AT task is created successfully! \r\n");
    msg_print_uart1("\r\n>");

#if defined(__AT_CMD_SUPPORT__)
    at_blewifi_auto_trans_init();
#endif
    
    for(;;)
    {
        /** wait event */
        rxEvent = osMessageGet(xAtQueue, osWaitForever);
        if(rxEvent.status != osEventMessage) continue;

        rxMsg = (xATMessage *) rxEvent.value.p;

#if 1
        if(rxMsg->event == AT_UART1_EVENT)
        {
            pData = (at_uart_buffer_t *)rxMsg->pcMessage;

            if(g_sAtPendBuf)
            {
                if(pData->buf[0] == 0x0A)
                {
                    at_term_char_remove(g_sAtPendBuf);
                    at_task_cmd_process(g_sAtPendBuf, strlen(g_sAtPendBuf));
                }
                else if(pData->buf[pData->in - 1] == 0x0D)
                {
                    msg_print_uart1("\r\nLF not found. Discard pending command.\r\n");
                    msg_print_uart1("ERROR\r\n");

                    if(strlen(g_sAtPendBuf) < pData->in)
                    {
                        free(g_sAtPendBuf);

                        g_sAtPendBuf = (char *)malloc(pData->in + 1);

                        if(!g_sAtPendBuf)
                        {
                            goto done;
                        }
                    }
                        
                    memcpy(g_sAtPendBuf, pData->buf, pData->in + 1);
                    goto done;
                }
                else
                {
                    msg_print_uart1("\r\nUnexpected begining[%02X]. Discard pending and current commands.\r\n", pData->buf[0]);
                    msg_print_uart1("ERROR\r\n");
                }

                free(g_sAtPendBuf);
                g_sAtPendBuf = NULL;
            }
            else
            {
                if(at_cmd_crlf_term_get())
                {
                    if(pData->buf[pData->in - 1] == 0x0D)
                    {
                        g_sAtPendBuf = (char *)malloc(pData->in + 1);
                        
                        if(g_sAtPendBuf)
                        {
                            memcpy(g_sAtPendBuf, pData->buf, pData->in + 1);
                        }
                    }

                    goto done;
                }
                
                at_term_char_remove(pData->buf);
                at_task_cmd_process(pData->buf, strlen(pData->buf));
            }
        }

    done:
#else
        if(rxMsg->event == AT_UART1_EVENT)
        {
            pData = (at_uart_buffer_t *)rxMsg->pcMessage;
			at_task_cmd_process(pData->buf, strlen(pData->buf));
            //at_clear_uart_buffer();
        }
#endif
        if(rxMsg->pcMessage != NULL) free(rxMsg->pcMessage);
        osPoolFree (AtMemPoolId, rxMsg);
    }
}

/*
 * @brief An external Function at_task_cmd_process prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_cmd_process_fp_t at_task_cmd_process;

/*
 * @brief An external Function at_task_create_semaphore prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_create_semaphore_fp_t at_task_create_semaphore;

/*
 * @brief An external Function at_semaphore_release prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_semaphore_release_fp_t at_semaphore_release;

/*
 * @brief An external Function at_task_create prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_create_fp_t at_task_create;

/*
 * @brief An external Function at_task_send prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_send_fp_t at_task_send;

/*
 * @brief An external Function at_task prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_task_fp_t at_task;


/*
 * @brief An external Function at_module_init prototype declaration retention attribute segment
 *
 */
extern RET_DATA at_module_init_fp_t at_module_init;



/*
 * @brief AT Command Interface Initialization for AT Command Task
 *
 */
void at_task_func_init_patch(void)
{
#if defined(__AT_CMD_SUPPORT__)
    memset(&gTransCfg, 0, sizeof(at_nvm_trans_config_t));
#endif   
    at_task_send = at_task_send_patch;
    at_task = at_task_patch;
    at_module_init = at_module_init_patch;
}

