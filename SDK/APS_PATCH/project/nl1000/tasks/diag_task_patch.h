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

#ifndef __DIAG_TASK_PATCH_H__
#define __DIAG_TASK_PATCH_H__

#define RBUF_SIZE_PATCH           128

typedef struct
{
    unsigned char in;                                // Next In Index
    unsigned char out;                               // Next Out Index
    char buf[RBUF_SIZE_PATCH];                       // Buffer
} uart_buffer_patch_t;

void diag_task_patch_func_init(void);
void uartdbg_rx_int_patch_handler(uint32_t u32Data);

#endif
