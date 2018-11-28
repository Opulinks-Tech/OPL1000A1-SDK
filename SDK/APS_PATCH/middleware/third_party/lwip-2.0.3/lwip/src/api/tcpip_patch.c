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

#include "lwip/tcpip_patch.h"
#include "lwip/timeouts_patch.h"

#include "cmsis_os.h"

#if defined(LWIP_ROMBUILD)
#include "api/tcpip_if.h"
#endif

#define TCPIP_MSG_VAR_REF(name)     API_VAR_REF(name)
#define TCPIP_MSG_VAR_DECLARE(name) API_VAR_DECLARE(struct tcpip_msg, name)
#define TCPIP_MSG_VAR_ALLOC(name)   API_VAR_ALLOC(struct tcpip_msg, MEMP_TCPIP_MSG_API, name, ERR_MEM)
#define TCPIP_MSG_VAR_FREE(name)    API_VAR_FREE(MEMP_TCPIP_MSG_API, name)

/* global variables */
extern tcpip_init_done_fn tcpip_init_done;
extern void *tcpip_init_done_arg;
extern sys_mbox_t mbox;
extern sys_thread_t tcpip_thread_handle;
extern SemaphoreHandle_t tcpip_timer_suspend;

#if defined(LWIP_ROMBUILD)
extern u16_t tcpip_thread_stacksize;
extern u16_t tcpip_thread_prio;
#else
extern u16_t tcpip_thread_stacksize = 512;
extern u16_t tcpip_thread_prio = (tskIDLE_PRIORITY + 2); //osPriorityBelowNormal
#endif

#if LWIP_TCPIP_CORE_LOCKING
/** The global semaphore to lock the stack. */
extern sys_mutex_t lock_tcpip_core;
#endif /* LWIP_TCPIP_CORE_LOCKING */

#if LWIP_TIMERS
/* wait for a message, timeouts are processed while waiting */
#define TCPIP_MBOX_FETCH(mbox, msg) sys_timeouts_mbox_fetch(mbox, msg)
#else /* LWIP_TIMERS */
/* wait for a message with timers disabled (e.g. pass a timer-check trigger into tcpip_thread) */
#define TCPIP_MBOX_FETCH(mbox, msg) sys_mbox_fetch(mbox, msg)
#endif /* LWIP_TIMERS */

/**
 * The main lwIP thread. This thread has exclusive access to lwIP core functions
 * (unless access to them is not locked). Other threads communicate with this
 * thread using message boxes.
 *
 * It also starts all the timers to make sure they are running in the right
 * thread context.
 *
 * @param arg unused argument
 */
static void tcpip_thread_patch(void *arg)
{
  struct tcpip_msg *msg;
  LWIP_UNUSED_ARG(arg);

  if (tcpip_init_done != NULL) {
    tcpip_init_done(tcpip_init_done_arg);
  }

  LOCK_TCPIP_CORE();
  while (1) {                          /* MAIN Loop */
    //UNLOCK_TCPIP_CORE();
    LWIP_TCPIP_THREAD_ALIVE();

    /* wait for a message, timeouts are processed while waiting */
    TCPIP_MBOX_FETCH(&mbox, (void **)&msg);
    //LOCK_TCPIP_CORE();
    if (msg == NULL) {
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: NULL\n"));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);
      continue;
    }
    switch (msg->type) {
#if !LWIP_TCPIP_CORE_LOCKING
    case TCPIP_MSG_API:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API message %p\n", (void *)msg));
      msg->msg.api_msg.function(msg->msg.api_msg.msg);
      break;
    case TCPIP_MSG_API_CALL:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: API CALL message %p\n", (void *)msg));
      msg->msg.api_call.arg->err = msg->msg.api_call.function(msg->msg.api_call.arg);
      sys_sem_signal(msg->msg.api_call.sem);
      break;
#endif /* !LWIP_TCPIP_CORE_LOCKING */

#if !LWIP_TCPIP_CORE_LOCKING_INPUT
    case TCPIP_MSG_INPKT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: PACKET %p\n", (void *)msg));
      msg->msg.inp.input_fn(msg->msg.inp.p, msg->msg.inp.netif);
      memp_free(MEMP_TCPIP_MSG_INPKT, msg);
      break;
#endif /* !LWIP_TCPIP_CORE_LOCKING_INPUT */

#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
    case TCPIP_MSG_TIMEOUT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: TIMEOUT %p\n", (void *)msg));
      sys_timeout(msg->msg.tmo.msecs, msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
    case TCPIP_MSG_UNTIMEOUT:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: UNTIMEOUT %p\n", (void *)msg));
      sys_untimeout(msg->msg.tmo.h, msg->msg.tmo.arg);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
    /* OPULINKS_CHANGES */
    case TCPIP_MSG_SUSPEND_ALL_TCPIP_TIMEOUTS:
      /* deactivate all timers which are controlling the stack, we are going to deep sleep so we dont need them now */
      sys_timeouts_suspend(NULL);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
    case TCPIP_MSG_RESUME_ALL_TCPIP_TIMEOUTS:
      /* activate all timers  */
      sys_timeouts_resume(NULL);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;
     /* OPULINKS_CHANGES */
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */

    case TCPIP_MSG_CALLBACK:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      memp_free(MEMP_TCPIP_MSG_API, msg);
      break;

    case TCPIP_MSG_CALLBACK_STATIC:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: CALLBACK_STATIC %p\n", (void *)msg));
      msg->msg.cb.function(msg->msg.cb.ctx);
      break;

    default:
      LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_thread: invalid message: %d\n", msg->type));
      LWIP_ASSERT("tcpip_thread: invalid message", 0);
      break;
    }
  }
}

/**
 * @ingroup lwip_os
 * Initialize this module:
 * - initialize all sub modules
 * - start the tcpip_thread
 *
 * @param initfunc a function to call when tcpip_thread is running and finished initializing
 * @param arg argument to pass to initfunc
 */
void tcpip_init_patch(tcpip_init_done_fn initfunc, void *arg)
{
  lwip_init();

  tcpip_init_done = initfunc;
  tcpip_init_done_arg = arg;
  if (sys_mbox_new(&mbox, TCPIP_MBOX_SIZE) != ERR_OK) {
    LWIP_ASSERT("failed to create tcpip_thread mbox", 0);
  }
#if LWIP_TCPIP_CORE_LOCKING
  if (sys_mutex_new(&lock_tcpip_core) != ERR_OK) {
    LWIP_ASSERT("failed to create lock_tcpip_core", 0);
  }
#endif /* LWIP_TCPIP_CORE_LOCKING */

  tcpip_thread_handle = sys_thread_new(TCPIP_THREAD_NAME, tcpip_thread_patch, NULL, TCPIP_THREAD_STACKSIZE, TCPIP_THREAD_PRIO);

  LWIP_DEBUGF(TCPIP_DEBUG, ("tcpip_task_handle : %x, prio:%d,stack:%d\n",
		 (u32_t)tcpip_thread_handle, TCPIP_THREAD_PRIO, TCPIP_THREAD_STACKSIZE));
}

/* OPULINKS_CHANGES */
/**
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t tcpip_deactivate_tcpip_timeouts(void)
{
    return tcpip_callback_with_block(sys_timeouts_suspend, NULL, 1);
}

/**
 * @return ERR_MEM on memory error, ERR_OK otherwise
 */
err_t tcpip_activate_tcpip_timeouts(void)
{
    return tcpip_callback_with_block(sys_timeouts_resume, NULL, 1);
}

err_t tcpip_check_timeouts(void)
{
    return tcpip_callback_with_block(sys_timeouts_check, NULL, 1);
}
/* OPULINKS_CHANGES */


void lwip_load_interface_tcpip_patch(void)
{
    tcpip_thread_adpt                = tcpip_thread_patch;
    tcpip_init_adpt                  = tcpip_init_patch;
}


#endif /* !NO_SYS */
