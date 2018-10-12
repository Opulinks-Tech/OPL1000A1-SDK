/**
 * @file
 * Stack-internal timers implementation.
 * This file includes timer callbacks for stack-internal timers as well as
 * functions to set up or stop timers and check for expired timers.
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
 *         Simon Goldschmidt
 *
 */

#include "lwip/opt.h"

#include "lwip/timeouts.h"
#include "lwip/priv/tcp_priv.h"

#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/priv/tcpip_priv.h"

#include "lwip/ip4_frag.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/nd6.h"
#include "lwip/ip6_frag.h"
#include "lwip/mld6.h"
#include "lwip/sys.h"
#include "lwip/pbuf.h"

#if defined(LWIP_ROMBUILD)
#include "timeouts_if.h"
#endif

#include "lwip/timeouts_patch.h"
#include "ps_patch.h"

#include "cmsis_os.h"
int timer_suspend_state = 0;

#if LWIP_DEBUG_TIMERNAMES
#define HANDLER(x) x, #x
#else /* LWIP_DEBUG_TIMERNAMES */
#define HANDLER(x) x
#endif /* LWIP_DEBUG_TIMERNAMES */

#define LWIP_MAX_TIMEOUT  0x7fffffff
#define LWIP_UINT32_MAX   0xffffffff

/** Returned by sys_timeouts_sleeptime() to indicate there is no timer, so we
 * can sleep forever.
 */
#define SYS_TIMEOUTS_SLEEPTIME_INFINITE 0xFFFFFFFF


/* Check if timer's expiry time is greater than time and care about u32_t wraparounds */
#define TIME_LESS_THAN(t, compare_to) ( (((u32_t)((t)-(compare_to))) > LWIP_MAX_TIMEOUT) ? 1 : 0 )

/** This array contains all stack-internal cyclic timers. To get the number of
 * timers, use LWIP_ARRAYSIZE() */
extern const struct lwip_cyclic_timer lwip_cyclic_timers[];

#if LWIP_TIMERS && !LWIP_TIMERS_CUSTOM

/** The one and only timeout list */
extern struct sys_timeo *next_timeout;

static u32_t current_timeout_due_time = 0;

#define OPL_SMART_SLEEP         1
#define USE_LWIP_NEW_TIMER      1
#define LWIP_DEBUG_SEELPTIME    0

#ifdef USE_LWIP_NEW_TIMER
#include "cmsis_os.h"
static void sys_timeout_abs(u32_t abs_time, sys_timeout_handler handler, void *arg, const char *handler_name)
{
  struct sys_timeo *timeout, *t;

  timeout = (struct sys_timeo *)memp_malloc(MEMP_SYS_TIMEOUT);
  if (timeout == NULL) {
    LWIP_ASSERT("sys_timeout: timeout != NULL, pool MEMP_SYS_TIMEOUT is empty", timeout != NULL);
    return;
  }

  timeout->next = NULL;
  timeout->h = handler;
  timeout->arg = arg;
  timeout->time = abs_time;

#if LWIP_DEBUG_TIMERNAMES
  timeout->handler_name = handler_name;
  LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: %p abs_time=%"U32_F" handler=%s arg=%p\n",
                             (void *)timeout, abs_time, handler_name, (void *)arg));
#endif /* LWIP_DEBUG_TIMERNAMES */

  if (next_timeout == NULL) {
    next_timeout = timeout;
    return;
  }
  if (TIME_LESS_THAN(timeout->time, next_timeout->time)) {
    timeout->next = next_timeout;
    next_timeout = timeout;
  } else {
    for (t = next_timeout; t != NULL; t = t->next) {
      if ((t->next == NULL) || TIME_LESS_THAN(timeout->time, t->next->time)) {
        timeout->next = t->next;
        t->next = timeout;
        break;
      }
    }
  }
}


/**
 * Timer callback function that calls cyclic->handler() and reschedules itself.
 *
 * @param arg unused argument
 */
static void cyclic_timer_patch(void *arg)
{
  u32_t now;
  u32_t next_timeout_time;
  const struct lwip_cyclic_timer *cyclic = (const struct lwip_cyclic_timer *)arg;

#if LWIP_DEBUG_TIMERNAMES
  //LWIP_DEBUGF(TIMERS_DEBUG, ("tcpip: %s()\n", cyclic->handler_name));
#endif
  cyclic->handler();

  now = sys_now();
  next_timeout_time = (u32_t)(current_timeout_due_time + cyclic->interval_ms);  /* overflow handled by TIME_LESS_THAN macro */

  if (TIME_LESS_THAN(next_timeout_time, now)) {
    /* timer would immediately expire again -> "overload" -> restart without any correction */
#if LWIP_DEBUG_TIMERNAMES
    LWIP_DEBUGF(TIMERS_DEBUG , ("restart tmr "));
    sys_timeout_abs((u32_t)(now + cyclic->interval_ms), cyclic_timer_patch, arg, cyclic->handler_name);
#else
    sys_timeout_abs((u32_t)(now + cyclic->interval_ms), cyclic_timer_patch, arg);
#endif

  } else {
    /* correct cyclic interval with handler execution delay and sys_check_timeouts jitter */
#if LWIP_DEBUG_TIMERNAMES
     LWIP_DEBUGF(TIMERS_DEBUG ,("correct tmr "));

    sys_timeout_abs(next_timeout_time, cyclic_timer_patch, arg, cyclic->handler_name);
#else
    sys_timeout_abs(next_timeout_time, cyclic_timer_patch, arg);
#endif
  }
}

/** Initialize this module */
void sys_timeouts_init_patch(void)
{
  size_t i;
  /* tcp_tmr() at index 0 is started on demand */
  for (i = (LWIP_TCP ? 1 : 0); i < 11; i++) {
    /* we have to cast via size_t to get rid of const warning
      (this is OK as cyclic_timer() casts back to const* */
    sys_timeout(lwip_cyclic_timers[i].interval_ms, cyclic_timer_patch, LWIP_CONST_CAST(void *, &lwip_cyclic_timers[i]));
  }
}


/**
 * Create a one-shot timer (aka timeout). Timeouts are processed in the
 * following cases:
 * - while waiting for a message using sys_timeouts_mbox_fetch()
 * - by calling sys_check_timeouts() (NO_SYS==1 only)
 *
 * @param msecs time in milliseconds after that the timer should expire
 * @param handler callback function to call when msecs have elapsed
 * @param arg argument to pass to the callback function
 */
void sys_timeout_debug_patch(u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name)
{
  u32_t next_timeout_time;

  //LWIP_ASSERT_CORE_LOCKED();

  LWIP_ASSERT("Timeout time too long, max is LWIP_UINT32_MAX/4 msecs", msecs <= (LWIP_UINT32_MAX / 4));

  next_timeout_time = (u32_t)(sys_now() + msecs); /* overflow handled by TIME_LESS_THAN macro */

#if LWIP_DEBUG_TIMERNAMES
  sys_timeout_abs(next_timeout_time, handler, arg, handler_name);
#else
  sys_timeout_abs(next_timeout_time, handler, arg);
#endif
}

/**
 * Go through timeout list (for this task only) and remove the first matching
 * entry (subsequent entries remain untouched), even though the timeout has not
 * triggered yet.
 *
 * @param handler callback function that would be called by the timeout
 * @param arg callback argument that would be passed to handler
*/
void
sys_untimeout_patch(sys_timeout_handler handler, void *arg)
{
  struct sys_timeo *prev_t, *t;

  //LWIP_ASSERT_CORE_LOCKED();

  if (next_timeout == NULL) {
    return;
  }

  for (t = next_timeout, prev_t = NULL; t != NULL; prev_t = t, t = t->next) {
    if ((t->h == handler) && (t->arg == arg)) {
      /* We have a match */
      /* Unlink from previous in list */
      if (prev_t == NULL) {
        next_timeout = t->next;
      } else {
        prev_t->next = t->next;
      }
      memp_free(MEMP_SYS_TIMEOUT, t);
      return;
    }
  }
  return;
}

/**
 * @ingroup lwip_nosys
 * Handle timeouts for NO_SYS==1 (i.e. without using
 * tcpip_thread/sys_timeouts_mbox_fetch(). Uses sys_now() to call timeout
 * handler functions when timeouts expire.
 *
 * Must be called periodically from your main loop.
 */
void sys_check_timeouts_patch(void)
{
  u32_t now;

  //LWIP_ASSERT_CORE_LOCKED();

  /* Process only timers expired at the start of the function. */
  now = sys_now();

  do {
    struct sys_timeo *tmptimeout;
    sys_timeout_handler handler;
    void *arg;

    PBUF_CHECK_FREE_OOSEQ();

    tmptimeout = next_timeout;
    if (tmptimeout == NULL) {
      return;
    }

    if (TIME_LESS_THAN(now, tmptimeout->time)) {
      return;
    }

    /* Timeout has expired */
    next_timeout = tmptimeout->next;
    handler = tmptimeout->h;
    arg = tmptimeout->arg;
    current_timeout_due_time = tmptimeout->time;
#if LWIP_DEBUG_TIMERNAMES
    if (handler != NULL) {
      //LWIP_DEBUGF(TIMERS_DEBUG, ("sct calling h=%s t=%"U32_F" arg=%p\n",
      //                           tmptimeout->handler_name, sys_now() - tmptimeout->time, arg));
    }
#endif /* LWIP_DEBUG_TIMERNAMES */
    memp_free(MEMP_SYS_TIMEOUT, tmptimeout);
    if (handler != NULL) {
      handler(arg);
    }
    LWIP_TCPIP_THREAD_ALIVE();

    /* Repeat until all expired timers have been called */
  } while (1);
}

/** Rebase the timeout times to the current time.
 * This is necessary if sys_check_timeouts() hasn't been called for a long
 * time (e.g. while saving energy) to prevent all timer functions of that
 * period being called.
 */
void sys_restart_timeouts_patch(void)
{
  u32_t now;
  u32_t base;
  struct sys_timeo *t;

  if (next_timeout == NULL) {
    return;
  }

  now = sys_now();
  base = next_timeout->time;

  for (t = next_timeout; t != NULL; t = t->next) {
    t->time = (t->time - base) + now;
  }
}

/** Return the time left before the next timeout is due. If no timeouts are
 * enqueued, returns 0xffffffff
 */
u32_t sys_timeouts_sleeptime_patch(void)
{
  u32_t now;

 // LWIP_ASSERT_CORE_LOCKED();

  if (next_timeout == NULL) {
    return SYS_TIMEOUTS_SLEEPTIME_INFINITE;
  }
  now = sys_now();
  if (TIME_LESS_THAN(next_timeout->time, now)) {
    return 0;
  } else {
    u32_t ret = (u32_t)(next_timeout->time - now);
    LWIP_ASSERT("invalid sleeptime", ret <= LWIP_MAX_TIMEOUT);
    return ret;
  }
}

/**
 * Wait (forever) for a message to arrive in an mbox.
 * While waiting, timeouts are processed.
 *
 * @param mbox the mbox to fetch the message from
 * @param msg the place to store the message
 */
static void sys_timeouts_mbox_fetch_patch(sys_mbox_t *mbox, void **msg)
{
  u32_t sleeptime, res;
#if LWIP_DEBUG_SEELPTIME
  int s_time, e_time;
#endif
again:
  //LWIP_ASSERT_CORE_LOCKED();

#if OPL_SMART_SLEEP
  if (ps_is_smart_sleep_enabled())
  {
      sys_arch_mbox_fetch(mbox, msg, 0);
  }
  else
#endif
  {
    sleeptime = sys_timeouts_sleeptime();

    LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: sleeptime=%"U32_F"\n", sleeptime));

    if (sleeptime == SYS_TIMEOUTS_SLEEPTIME_INFINITE) {
      //UNLOCK_TCPIP_CORE();
      sys_arch_mbox_fetch(mbox, msg, 0);
      //LOCK_TCPIP_CORE();
      return;
    } else if (sleeptime == 0) {
      sys_check_timeouts();
      /* We try again to fetch a message from the mbox. */
      goto again;
    }

    //UNLOCK_TCPIP_CORE();
#if LWIP_DEBUG_SEELPTIME
    s_time= osKernelSysTick();
#endif
    res = sys_arch_mbox_fetch(mbox, msg, sleeptime);
#if LWIP_DEBUG_SEELPTIME
    e_time= osKernelSysTick();
#endif

#if LWIP_DEBUG_TIMERNAMES && LWIP_DEBUG_SEELPTIME
    printf("real sleeptime = %d\r\n",e_time - s_time);
#endif
    //LOCK_TCPIP_CORE();
    if (res == SYS_ARCH_TIMEOUT) {
      /* If a SYS_ARCH_TIMEOUT value is returned, a timeout occurred
         before a message could be fetched. */
      sys_check_timeouts();
      /* We try again to fetch a message from the mbox. */
      goto again;
    }
  }
}

void sys_timeouts_suspend(void *arg)
{
    /* deactivate ip, arp, dhcp, dns and igmp timer */
    /* they will be simply removed from the timers list */
    /* this function must only be used when the system goes powerdown */
    /* and the networking stack activity msut be stopped for the time the system is sleeping */
    size_t i;

    if (timer_suspend_state) return;

    for (i = (LWIP_TCP ? 1 : 0); i < 11; i++) {
        /* we have to cast via size_t to get rid of const warning
        (this is OK as cyclic_timer() casts back to const* */
        sys_untimeout(cyclic_timer_patch, LWIP_CONST_CAST(void*, &lwip_cyclic_timers[i]));
    }

    timer_suspend_state = 1;

    return;
}

void sys_timeouts_resume(void *arg)
{
    size_t i;

    if (timer_suspend_state == 0) return;

    for (i = (LWIP_TCP ? 1 : 0); i < 11; i++) {
        /* we have to cast via size_t to get rid of const warning
        (this is OK as cyclic_timer() casts back to const* */
        sys_timeout(lwip_cyclic_timers[i].interval_ms, cyclic_timer_patch, LWIP_CONST_CAST(void*, &lwip_cyclic_timers[i]));
    }

    timer_suspend_state = 0;

    return;
}

void sys_timeouts_check(void *arg)
{
    sys_check_timeouts();
    return;
}
#else  /* USE_NEW_TIMER */

void sys_timeouts_mbox_fetch_patch(sys_mbox_t *mbox, void **msg)
{
  u32_t sleeptime;
  int s_time, e_time;
  u32_t diff;
again:

#if OPL_SMART_SLEEP
  if (ps_is_smart_sleep_enabled())
  {
    sys_arch_mbox_fetch(mbox, msg, 0);
  }
  else
#endif
  {
    if (!next_timeout) {
      sys_arch_mbox_fetch(mbox, msg, 0);
      return;
    }

    sleeptime = sys_timeouts_sleeptime();
    LWIP_DEBUGF(TIMERS_DEBUG, ("sys_timeout: sleeptime=%"U32_F"\n", sleeptime));

    s_time= osKernelSysTick();
    if (sleeptime == 0 || sys_arch_mbox_fetch(mbox, msg, sleeptime) == SYS_ARCH_TIMEOUT) {
      /* If a SYS_ARCH_TIMEOUT value is returned, a timeout occurred
         before a message could be fetched. */
      e_time= osKernelSysTick();
      printf("real sleeptime = %d\r\n",e_time - s_time);

      sys_check_timeouts();
      /* We try again to fetch a message from the mbox. */
      goto again;
    }
  }
}


/* OPULINKS_CHANGES */
void sys_timeouts_suspend(void *arg)
{
    /* deactivate ip, arp, dhcp, dns and igmp timer */
    /* they will be simply removed from the timers list */
    /* this function must only be used when the system goes powerdown */
    /* and the networking stack activity msut be stopped for the time the system is sleeping */
    size_t i;

    if (timer_suspend_state) return;

    for (i = (LWIP_TCP ? 1 : 0); i < 11; i++) {
        /* we have to cast via size_t to get rid of const warning
        (this is OK as cyclic_timer() casts back to const* */
        sys_untimeout(cyclic_timer, LWIP_CONST_CAST(void*, &lwip_cyclic_timers[i]));
    }

    timer_suspend_state = 1;

    return;
}

void sys_timeouts_resume(void *arg)
{
    size_t i;

    if (timer_suspend_state == 0) return;

    for (i = (LWIP_TCP ? 1 : 0); i < 11; i++) {
        /* we have to cast via size_t to get rid of const warning
        (this is OK as cyclic_timer() casts back to const* */
        sys_timeout(lwip_cyclic_timers[i].interval_ms, cyclic_timer, LWIP_CONST_CAST(void*, &lwip_cyclic_timers[i]));
    }

    timer_suspend_state = 0;

    return;
}

void sys_timeouts_check(void *arg)
{
    sys_check_timeouts();
    return;
}

/* OPULINKS_CHANGES */

#endif /* USE_LWIP_NEW_TIMER */


void lwip_load_interface_timeouts_patch(void)
{
#ifdef USE_LWIP_NEW_TIMER

    sys_timeouts_init_adpt                  = sys_timeouts_init_patch;

#if LWIP_DEBUG_TIMERNAMES
    sys_timeout_debug_adpt                  = sys_timeout_debug_patch;
#else /* LWIP_DEBUG_TIMERNAMES */
    sys_timeout_adpt                        = sys_timeout_patch;
#endif /* LWIP_DEBUG_TIMERNAMES */


    cyclic_timer_adpt                       = cyclic_timer_patch;
    sys_check_timeouts_adpt                 = sys_check_timeouts_patch;
    sys_timeouts_sleeptime_adpt             = sys_timeouts_sleeptime_patch;

    sys_untimeout_adpt                      = sys_untimeout_patch;
    sys_restart_timeouts_adpt               = sys_restart_timeouts_patch;
#if NO_SYS
#else /* NO_SYS */
    sys_timeouts_mbox_fetch_adpt            = sys_timeouts_mbox_fetch_patch;
#endif

#else /* USE_LWIP_NEW_TIMER*/

    sys_timeouts_mbox_fetch_adpt            = sys_timeouts_mbox_fetch_patch;
#endif /*USE_LWIP_NEW_TIMER */
}


#endif /* LWIP_TIMERS && !LWIP_TIMERS_CUSTOM */

