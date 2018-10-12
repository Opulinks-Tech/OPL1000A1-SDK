/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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

#include "sys_os_config_patch.h"



/* lwIP includes. */
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"

#include <stdio.h>
#include "cmsis_os.h"

//sys_mutex_t critical_section;

#if !NO_SYS
/* ------------------------ System architecture includes ----------------------------- */
#include "arch/sys_arch.h"

#if defined(LWIP_ROMBUILD)
#include "port/sys_arch_if.h"
#endif
/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/opt.h"
#include "lwip/stats.h"


/*---------------------------------------------------------------------------*
 * Routine:  sys_thread_new
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts a new thread with priority "prio" that will begin its
 *      execution in the function "thread()". The "arg" argument will be
 *      passed as an argument to the thread() function. The id of the new
 *      thread is returned. Both the id and the priority are system
 *      dependent.
 * Inputs:
 *      char *name              -- Name of thread
 *      void (* thread)(void *arg) -- Pointer to function to run.
 *      void *arg               -- Argument passed into function
 *      int stacksize           -- Required stack amount in bytes
 *      int prio                -- Thread priority
 * Outputs:
 *      sys_thread_t            -- Pointer to per-thread timeouts.
 *---------------------------------------------------------------------------*/
#ifdef OS_TASK_INFO_DUMP
sys_thread_t sys_thread_new_patch(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
	TaskHandle_t xCreatedTask;
	BaseType_t xResult;
	sys_thread_t xReturn;

	xResult = xTaskCreate(thread, name, stacksize, arg, prio, &xCreatedTask);

	if( xResult == pdPASS )
	{
		xReturn = xCreatedTask;
		
        {
            extern void task_info_add(void *pHandle, uint32_t u32StackSize);
    
            task_info_add(xCreatedTask, stacksize);
        }
	}
	else
	{
		xReturn = NULL;
	}

	return xReturn;
}
#endif //#ifdef OS_TASK_INFO_DUMP

void assert_loop_patch(void)
{
    sys_msleep(1000);
	while (1) {}
    
}

void lwip_load_interface_sys_arch_freertos_patch(void)
{
    #ifdef OS_TASK_INFO_DUMP
    sys_thread_new_adpt = sys_thread_new_patch;
    #endif //#ifdef OS_TASK_INFO_DUMP
    assert_loop_adpt                = assert_loop_patch;
    return;
}

#endif //#if !NO_SYS



