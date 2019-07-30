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

#ifndef __IPC_PATCH_H__
#define __IPC_PATCH_H__


#include "ipc.h"
#include "ps_patch.h"



// For PS module ps_conf
#define IPC_PS_CONF_PATCH_START    		IPC_PS_CONF_START
#define IPC_PS_CONF_PATCH_LEN         	sizeof(t_ps_conf_patch)
#define IPC_PS_CONF_PATCH_END          	(IPC_PS_CONF_PATCH_START + IPC_PS_CONF_PATCH_LEN)

// For message from M0 to M3
#define IPC_M0_MSG_SIZE                 256
#define IPC_M0_MSG_QUEUE_WRITE          IPC_ADDR_ALIGN(IPC_PS_CONF_PATCH_END, 4)
#define IPC_M0_MSG_QUEUE_READ           IPC_ADDR_ALIGN(IPC_M0_MSG_QUEUE_WRITE + sizeof(uint32_t), 4)
#define IPC_M0_MSG_QUEUE_START          IPC_ADDR_ALIGN(IPC_M0_MSG_QUEUE_READ + sizeof(uint32_t), 4)
#define IPC_M0_MSG_BUF_SIZE             IPC_VALUE_ALIGN(IPC_M0_MSG_SIZE + 1, 4)
#define IPC_M0_MSG_BUF_NUM              4
#define IPC_M0_MSG_QUEUE_END            (IPC_M0_MSG_QUEUE_START + (IPC_M0_MSG_BUF_SIZE * IPC_M0_MSG_BUF_NUM))

// For message from M3 to M0
#define IPC_M3_MSG_SIZE                 256
#define IPC_M3_MSG_QUEUE_WRITE          IPC_ADDR_ALIGN(IPC_M0_MSG_QUEUE_END, 4)
#define IPC_M3_MSG_QUEUE_READ           IPC_ADDR_ALIGN(IPC_M3_MSG_QUEUE_WRITE + sizeof(uint32_t), 4)
#define IPC_M3_MSG_QUEUE_START          IPC_ADDR_ALIGN(IPC_M3_MSG_QUEUE_READ + sizeof(uint32_t), 4)
#define IPC_M3_MSG_BUF_SIZE             IPC_VALUE_ALIGN(IPC_M3_MSG_SIZE + 1, 4)
#define IPC_M3_MSG_BUF_NUM              4
#define IPC_M3_MSG_QUEUE_END            (IPC_M3_MSG_QUEUE_START + (IPC_M3_MSG_BUF_SIZE * IPC_M3_MSG_BUF_NUM))

// For GPIO sleep IO state
#define IPC_SLEEP_IO_START    		    IPC_ADDR_ALIGN(IPC_M3_MSG_QUEUE_END, 4)
#define IPC_SLEEP_IO_LEN         	    sizeof(S_SLEEP_IO_CFG)
#define IPC_SLEEP_IO_END          	    (IPC_SLEEP_IO_START + IPC_SLEEP_IO_LEN)

#define IPC_SHM_AVAIL_ADDR              IPC_SLEEP_IO_END

typedef struct
{
    uint32_t dwNum;
    uint32_t dwMask;
    uint32_t dwBufSize;
    uint32_t *pdwWrite;
    uint32_t *pdwRead;
    uint32_t *pdwaBuf[IPC_M0_MSG_BUF_NUM];
} T_IpcM0MsgRb;

typedef struct
{
    uint32_t dwNum;
    uint32_t dwMask;
    uint32_t dwBufSize;
    uint32_t *pdwWrite;
    uint32_t *pdwRead;
    uint32_t *pdwaBuf[IPC_M3_MSG_BUF_NUM];
} T_IpcM3MsgRb;

typedef enum
{
    IPC_TYPE_M0_MSG = 0,
    IPC_TYPE_M3_MSG,

    IPC_EXT_TYPE_MAX
} T_IpcExtType;


#ifdef IPC_MSQ
extern T_IpcReadWriteBufGetFp ipc_m0_msg_write;
extern T_IpcCommonFp ipc_m0_msg_write_done;
extern T_IpcReadWriteBufGetFp ipc_m3_msg_read;
extern T_IpcCommonFp ipc_m3_msg_read_done;
#else
extern T_IpcReadWriteBufGetFp ipc_m3_msg_write;
extern T_IpcCommonFp ipc_m3_msg_write_done;
extern T_IpcReadWriteBufGetFp ipc_m0_msg_read;
extern T_IpcCommonFp ipc_m0_msg_read_done;
#endif

void ipc_patch_init(void);


#endif //#ifndef __IPC_PATCH_H__

