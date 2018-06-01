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


#define TRACER_TASK_NAME            "tracer"

#define TRACER_ISR_NAME_PREFIX      "ISR_"
#define TRACER_ISR_HANDLE_MASK      0xFF

#define TRACER_INT_TASK_NUM_MAX     32
#define TRACER_EXT_TASK_NUM_MAX     32

#define TRACER_QUEUE_SIZE_PATCH     80
#define TRACER_QUEUE_NUM_PATCH      128

#define TRACER_DBG(...)

#define TRACER_TASK_NAME_LEN        17 // include '\0'


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
} T_TracerTaskInfo;

typedef T_TracerTaskInfo *(*T_TracerTaskInfoGetFp)(char *baName, T_TracerTaskInfo *taTaskInfo, uint8_t bTaskNum);
typedef int (*T_TracerDefLevelFp)(uint8_t bType, uint8_t bLevel);
typedef void (*T_TracerCmdFp)(char *sCmd);


extern T_TracerTaskInfoGetFp tracer_task_info_get;
extern T_TracerDefLevelFp tracer_def_level_set;
extern T_TracerCmdFp tracer_cmd;


void Tracer_PatchInit(void);


#endif //#ifndef __MSG_PATCH_H__

