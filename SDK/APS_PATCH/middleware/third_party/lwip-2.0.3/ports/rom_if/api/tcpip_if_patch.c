/**
 * @file
 * Sequential API Main thread module
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#include "lwip/opt.h"

#if !NO_SYS /* don't build if not configured for use in lwipopts.h */

#include "lwip/priv/tcpip_priv.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/pbuf.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"

#include "sys_os_config_patch.h"
#include "api/tcpip_if.h"


extern RET_DATA tcpip_thread_fp_t                      tcpip_thread_adpt;
extern RET_DATA pbuf_free_int_fp_t                     pbuf_free_int_adpt;

extern RET_DATA tcpip_send_msg_wait_sem_fp_t           tcpip_send_msg_wait_sem_adpt;
extern RET_DATA tcpip_api_call_fp_t                    tcpip_api_call_adpt;
extern RET_DATA tcpip_init_fp_t                        tcpip_init_adpt;
extern RET_DATA tcpip_inpkt_fp_t                       tcpip_inpkt_adpt;
extern RET_DATA tcpip_input_fp_t                       tcpip_input_adpt;
extern RET_DATA tcpip_callback_with_block_fp_t         tcpip_callback_with_block_adpt;
extern RET_DATA tcpip_callbackmsg_new_fp_t             tcpip_callbackmsg_new_adpt;
extern RET_DATA tcpip_callbackmsg_delete_fp_t          tcpip_callbackmsg_delete_adpt;
extern RET_DATA tcpip_trycallback_fp_t                 tcpip_trycallback_adpt;
extern RET_DATA pbuf_free_callback_fp_t                pbuf_free_callback_adpt;
extern RET_DATA mem_free_callback_fp_t                 mem_free_callback_adpt;

#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
extern RET_DATA tcpip_timeout_fp_t                     tcpip_timeout_adpt;
extern RET_DATA tcpip_untimeout_fp_t                   tcpip_untimeout_adpt;
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */

extern LWIP_RETDATA u16_t tcpip_thread_stacksize;
extern LWIP_RETDATA u16_t tcpip_thread_prio;

void lwip_load_interface_tcpip_if_patch(void)
{
    tcpip_thread_stacksize = OS_TASK_STACK_SIZE_TCPIP_PATCH;
    tcpip_thread_prio = osPriorityAboveNormal - osPriorityIdle;
    return;
}

#endif /* !NO_SYS */
