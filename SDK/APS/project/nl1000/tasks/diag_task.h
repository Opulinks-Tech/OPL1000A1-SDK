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

#ifndef __DIAG_TASK_H__
#define __DIAG_TASK_H__

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "task.h"
#include "queue.h"

#define DIAG_COMMAND_EVENT          0x01        //USART1
#define DIAG_COMMAND1_EVENT         0x02        //USART2

#define DIAG_UART_EVENT             (0x0001)
#define DIAG_TEST_EVENT             (0x0002)

#define DIAG_QUEUE_SIZE                     (5)
#define RBUF_SIZE                           64

typedef struct
{
    unsigned char in;                                // Next In Index
    unsigned char out;                               // Next Out Index
    char buf[RBUF_SIZE];                             // Buffer
} uart_buffer_t;

typedef struct
{
	unsigned int event;
	unsigned int length;
	char *pcMessage;
} xDiagMessage;

/* ----------------------------------------------------------------------- */

#define	MAX_CMD_BUFFER_SIZE		24
#define MAX_CMD_NAME_SIZE		4
#define MAX_CMD_ARGV_SIZE		16

typedef struct
{
	uint8_t	cmd;
} mtsk_cmd, *pmtsk_cmd;

typedef struct
{
	char	CmdName[MAX_CMD_NAME_SIZE];
	uint8_t	Argc;
	uint8_t	Argv[MAX_CMD_ARGV_SIZE];
	uint8_t	IsCmdFlag;
} cmd_package_t;

typedef void Cmd_t(uint8_t argc, uint8_t *argv);

typedef struct
{
	char	CmdName[MAX_CMD_NAME_SIZE];
	Cmd_t	*fun;
} cmd_table_t;

typedef void (*cli_cmd_handle)(void);
typedef struct nl_cli_cmd
{
    const char      *cmd;
    cli_cmd_handle  cmd_handle;
    //const char      *cmd_usage;
} nl_cli_cmd_t;

extern osMessageQId xDiagQueue;
extern osThreadId DiagTaskHandle;

//extern void diag_task( void *pvParameters );
//void diag_task_create(void);
//osStatus diag_task_send(xDiagMessage txMsg);
void uartdbg_rx_int_handler(uint32_t u32Data);
void ParseWPAParam(char* pszData);
void wpa_scan(void);
void wpa_scan_results(void);
void wpa_connect(void);
void wpa_disconnect(void);
void wpa_reconnect(void);
uint8_t is_M0_ready(void);

typedef void (*diag_task_create_fp_t)(void);
typedef osStatus (*diag_task_send_fp_t)(xDiagMessage txMsg);
typedef void (*diag_task_fp_t)( void *pvParameters );
typedef void (*diag_task_create_semaphore_fp_t)(void);
typedef void (*diag_semaphore_release_fp_t)(void);
typedef void (*CmdProcess_fp_t)(char *pbuf, int len);
typedef void (*ParseWPACommand_fp_t)(char* pszData);
typedef void (*ParseLWIPCommand_fp_t)(char* pszData);
typedef void (*ParsePINGCommand_fp_t)(char* pszData);
typedef void (*ParseWIFICommand_fp_t)(char* pszData);
#ifdef __PMP_ENABLE__
typedef void (*ParsePmpCommand_fp_t)(char* pszData);
#endif // #__PMP_ENABLE__
//#ifdef ENHANCE_IPC
typedef void (*ParseIpcCommand_fp_t)(char *sCmd);
//#endif // #ENHANCE_IPC
typedef void (*ParseTracerCommand_fp_t)(char *sCmd);
typedef void (*ParseSYSCommand_fp_t)(char* pszData);
typedef void (*ParseIperfCommand_fp_t)(char *sCmd);
typedef void (*ParseUnknownCommand_fp_t)(char *sCmd);

extern diag_task_create_fp_t diag_task_create;
extern diag_task_send_fp_t diag_task_send;
extern diag_task_fp_t diag_task;
extern diag_task_create_semaphore_fp_t diag_task_create_semaphore;
extern diag_semaphore_release_fp_t diag_semaphore_release;
extern CmdProcess_fp_t CmdProcess;
extern ParseWPACommand_fp_t ParseWPACommand;
extern ParseLWIPCommand_fp_t ParseLWIPCommand;
extern ParsePINGCommand_fp_t ParsePINGCommand;
extern ParseWIFICommand_fp_t ParseWIFICommand;
#ifdef __PMP_ENABLE__
extern ParsePmpCommand_fp_t ParsePmpCommand;
#endif // #__PMP_ENABLE__
//#ifdef ENHANCE_IPC
extern ParseIpcCommand_fp_t ParseIpcCommand;
//#endif // #ENHANCE_IPC
extern ParseTracerCommand_fp_t ParseTracerCommand;
extern ParseSYSCommand_fp_t ParseSYSCommand;
extern ParseIperfCommand_fp_t ParseIperfCommand;
extern ParseUnknownCommand_fp_t ParseUnknownCommand;


/*
   Interface Initialization: DIAG TASK
 */
void diag_task_func_init(void);


#endif
