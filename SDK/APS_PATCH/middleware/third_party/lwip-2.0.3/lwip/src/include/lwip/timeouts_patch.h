/**
 * @file
 * Timer implementations
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
 *         Simon Goldschmidt
 *
 */
#ifndef LWIP_HDR_TIMEOUTS_PATCH_H
#define LWIP_HDR_TIMEOUTS_PATCH_H

#include "lwip/timeouts.h"

#include "lwip/opt.h"
#include "lwip/err.h"
#if !NO_SYS
#include "lwip/sys.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LWIP_DEBUG_TIMERNAMES
#ifdef LWIP_DEBUG
#define LWIP_DEBUG_TIMERNAMES SYS_DEBUG
#else /* LWIP_DEBUG */
#define LWIP_DEBUG_TIMERNAMES 0
#endif /* LWIP_DEBUG*/
#endif

/** This array contains all stack-internal cyclic timers. To get the number of
 * timers, use LWIP_ARRAYSIZE() */
extern const struct lwip_cyclic_timer lwip_cyclic_timers[];

#if LWIP_TIMERS

void sys_timeouts_suspend(void *arg);
void sys_timeouts_resume(void *arg);
void sys_timeouts_check(void *arg);


#endif /* LWIP_TIMERS */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_TIMEOUTS_PATCH_H */
