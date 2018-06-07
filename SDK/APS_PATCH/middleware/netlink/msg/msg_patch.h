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

#ifndef __MSG_PATCH_H__
#define __MSG_PATCH_H__


#include "msg.h"
#define TRACER_TASK_NAME            "tracer"

#define TRACER_ISR_NAME_PREFIX      "ISR_"
#define TRACER_ISR_HANDLE_MASK      0xFF

#define TRACER_INT_TASK_NUM_MAX     32
#define TRACER_EXT_TASK_NUM_MAX     32

#define TRACER_QUEUE_SIZE_PATCH     80
#define TRACER_QUEUE_NUM_PATCH      128

#define TRACER_TASK_NAME_LEN        16 // include '\0'
#define TRACER_TASK_STACK_SIZE_PATCH    128 // number of uint32_t
#define TRACER_DBG(...)



typedef enum
{
    TRACER_TASK_TYPE_INTERNAL = 0,
    TRACER_TASK_TYPE_APP,

    TRACER_TASK_TYPE_MAX
} T_TracerTaskType;

typedef struct
{
    char baName[TRACER_TASK_NAME_LEN];
    uint8_t bLevel;
    uint8_t bStatus;
    uint8_t baPadding[2];
} T_TracerTaskInfo;

typedef struct
{
    uint8_t bMode;
    uint8_t bExtTaskDefLevel;
    uint8_t bNameDisplay;
    uint8_t bPadding;
    int iPriority;
    uint32_t dwStackSize;
    uint32_t dwQueueNum;
    uint32_t dwQueueSize;
} T_TracerCfg;
typedef T_TracerTaskInfo *(*T_TracerTaskInfoGetFp)(char *baName, T_TracerTaskInfo *taTaskInfo, uint8_t bTaskNum);
typedef int (*T_TracerTaskCfgSaveFp)(void);
typedef int (*T_TracerTaskInfoSaveFp)(uint8_t bIdx);
typedef int (*T_TracerDefLevelFp)(uint8_t bType, uint8_t bLevel);
typedef void (*T_TracerCmdFp)(char *sCmd);


extern T_TracerCommonFp tracer_load;
extern T_TracerDefLevelFp tracer_def_level_set;
extern T_TracerCmdFp tracer_cmd;


void Tracer_PatchInit(void);


#endif //#ifndef __MSG_PATCH_H__

