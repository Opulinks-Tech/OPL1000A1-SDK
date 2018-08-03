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

#include <stdio.h>
#include <stdbool.h>


#include "msg.h"


#define TRACER_ISR_NAME_PREFIX_PATCH    "opl_isr_"
#define TRACER_QUEUE_SIZE_PATCH         80
#define TRACER_QUEUE_NUM_PATCH          128
#define TRACER_TASK_NAME_LEN_PATCH      16 // include '\0'
#define TRACER_TASK_STACK_SIZE_MIN      128 // number of uint32_t


typedef struct
{
    char baName[TRACER_TASK_NAME_LEN_PATCH];
    uint8_t bLevel;
    uint8_t bStatus;
    uint8_t baPadding[2];
} T_TracerTaskInfoExt;

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


typedef T_TracerTaskInfoExt *(*T_TracerTaskInfoGetExtFp)(char *baName, T_TracerTaskInfoExt *taTaskInfo, uint8_t bTaskNum);
typedef int (*T_TracerTaskCfgSaveFp)(void);
typedef int (*T_TracerTaskInfoSaveFp)(uint8_t bIdx);


extern T_TracerTaskInfoGetExtFp tracer_task_info_get_ext;
extern T_TracerCommonFp tracer_load;

void Tracer_PatchInit(void);


#endif //#ifndef __MSG_PATCH_H__

