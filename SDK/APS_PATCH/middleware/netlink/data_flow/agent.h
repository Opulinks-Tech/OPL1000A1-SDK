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

#ifndef __AGENT_H__
#define __AGENT_H__


#define AGENT_TASK_NAME     "opl_agent"


typedef struct
{
    uint32_t u32Type;
    void *pParam;

    // Todo
    
} T_AgentMsg;


// internal
typedef void (*T_AgentTaskMainFp)(void *pParam);
typedef void (*T_AgentMsgFreeFp)(T_AgentMsg *ptMsg);

// external
typedef void (*T_AgengCommFp)(void);
typedef int (*T_AgengMsgSendFp)(T_AgentMsg *ptMsg);


extern T_AgengCommFp agent_init;
extern T_AgengMsgSendFp agent_msg_send;

void agent_patch_init(void);


#endif //#ifndef __AGENT_H__

