/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef __PLATFORM_DEBUG_H__
#define __PLATFORM_DEBUG_H__

// modified for ali_lib by Jeff, 20190627
// redefine the HAL_Printf function
#if 1   // the modified
#include "infra_config.h"
#else   // the original
#include "iotx_log.h"
#endif

#define hal_emerg(...)      HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")
#define hal_crit(...)       HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")
#define hal_err(...)        HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")
#define hal_warning(...)    HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")
#define hal_info(...)       HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")
#define hal_debug(...)      HAL_Printf("[prt] "), HAL_Printf(__VA_ARGS__), HAL_Printf("\r\n")

#endif  /* __PLATFORM_DEBUG_H__ */


