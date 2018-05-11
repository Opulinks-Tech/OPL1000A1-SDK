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

#ifndef __MSG_H__
#define __MSG_H__

#include <stdio.h>
#include <stdbool.h>

#define TRACER

#ifdef TRACER

//#define TRACER_SUT

#include "nl1000.h"

#define TRACER_MSG_MAX_SIZE     256 // include '\0'
#define TRACER_QUEUE_SIZE       128 // must <= TRACER_MSG_MAX_SIZE
#define TRACER_QUEUE_NUM        128

#define TRACER_TASK_NUM_MAX     32
#define TRACER_TASK_IDX_MAX     255

#define LOG_HIGH_LEVEL          0x04
#define LOG_MED_LEVEL           0x02
#define LOG_LOW_LEVEL           0x01
#define LOG_NONE_LEVEL          0x00
#define LOG_ALL_LEVEL           (LOG_HIGH_LEVEL | LOG_MED_LEVEL | LOG_LOW_LEVEL)

#define TRACER_DEF_LEVEL        LOG_ALL_LEVEL
#define TRACER_DEF_MODE         TRACER_MODE_DRCT

typedef enum
{
    TRACER_TYPE_LOG = 0,
    TRACER_TYPE_CLI,

    TRACER_TYPE_OPT_ADD,

    TRACER_TYPE_MAX
} T_TracerType;

typedef enum
{
    TRACER_MODE_DISABLE = 0,
    TRACER_MODE_NORMAL,
    TRACER_MODE_DRCT,

    TRACER_MODE_MAX
} T_TracerMode;

typedef struct
{
    uint32_t dwHandle;
    uint8_t bLevel;
    uint8_t bUsed;
} T_TracerOpt;

typedef struct
{
    uint8_t bType;      // log/cli
    uint8_t bLevel;

    uint32_t dwHandle;  // task handle or ISR index
} T_TracerInfo;

typedef struct
{
    T_TracerInfo tInfo;
    char baBuf[1];
} T_TracerCb;

typedef struct
{
    T_TracerCb *ptCb;
} T_TracerMsg;

typedef void (*T_TracerCommonFp)(void);
typedef int (*T_TracerLogLevelSetFp)(uint8_t bIdx, uint8_t bLevel);
typedef void (*T_TracerOptSetFp)(uint8_t bMode);
typedef uint8_t (*T_TracerOptGetFp)(void);
typedef void (*T_TracerPrioritySetFp)(int iPriority);
typedef void (*T_TracerNameDisplayFp)(uint8_t bDisplay);
typedef int (*T_TracerPrintfFp)(const char *sFmt, ...);
typedef int (*T_TracerMsgFp)(uint8_t bType, uint8_t bLevel, char *sFmt, ...);

typedef void (*T_TracerProcFp)(char *sString);
typedef void (*T_TracerMsgFreeFp)(T_TracerMsg *ptMsg);
typedef void (*T_TracerOptEntryAddFp)(uint32_t dwHandle, uint8_t bLevel);
typedef void (*T_TracerNameGetFp)(uint32_t dwHandle, char *baName, uint32_t dwSize);
typedef void (*T_TracerTaskMainFp)(void *pParam);
typedef uint32_t (*T_TracerHandleGetFp)(uint8_t *pbIsr);
typedef int (*T_TracerLevelGetFp)(uint32_t dwHandle, uint8_t *pbLevel);
typedef void (*T_TracerStringSendFp)(char *sString);


#define tracer_log(level, args...)          tracer_msg(TRACER_TYPE_LOG, level, args)
//#define tracer_cli(level, args...)          tracer_msg(TRACER_TYPE_CLI, level, args)
#define tracer_cli(level, args...)          tracer_drct_printf(args)

#define printf(args...)                     tracer_msg(TRACER_TYPE_LOG, LOG_HIGH_LEVEL, args)
#define msg_print2(args...)                 tracer_log(LOG_HIGH_LEVEL, args)
#define msg_print                           tracer_log


extern T_TracerCommonFp tracer_init;
extern T_TracerLogLevelSetFp tracer_log_level_set;
extern T_TracerOptSetFp tracer_log_mode_set;
extern T_TracerOptGetFp tracer_log_mode_get;
extern T_TracerOptSetFp tracer_log_def_level_set;
extern T_TracerOptGetFp tracer_log_def_level_get;
extern T_TracerPrioritySetFp tracer_priority_set;
extern T_TracerCommonFp tracer_dump;
extern T_TracerNameDisplayFp tracer_name_display;
extern T_TracerPrintfFp tracer_drct_printf;
extern T_TracerMsgFp tracer_msg;

void Tracer_PreInit(void);

#ifdef TRACER_SUT
int tracer_sut_task_create(uint8_t bEnableCliAt);
#endif //#ifdef TRACER_SUT

#else //#ifdef TRACER

/*This is global log level*/
extern int log_level_now;

#define tracer_cli      msg_print

/*define Global setting data*/
#define LOG_HIGH_LEVEL				2
#define LOG_MED_LEVEL				1
#define LOG_LOW_LEVEL				0
#define LOG_THRESHOLD				3

typedef void (*T_MsgInitFp)(void);
typedef void (*T_MsgPrintFp)(int level, char *fmt,...);
typedef void (*T_MsgPrint2Fp)(char *fmt,...);
typedef void (*T_MsgLevelSetFp)(int level);
typedef void (*T_MsgStringSendFp)(char *sString);


extern T_MsgInitFp msg_init;
extern T_MsgPrintFp msg_print;
extern T_MsgPrint2Fp msg_print2;
extern T_MsgLevelSetFp msg_set_level;

#endif //#ifdef TRACER

typedef void (*T_MsgPringIIFp)(bool bFlag, int iLevel, char* pszData, ...);

extern T_MsgPringIIFp msg_printII;

#endif /*__MSG_H__*/
