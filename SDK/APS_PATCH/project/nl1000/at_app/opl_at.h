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

#ifndef __OPL_AT_H__
#define __OPL_AT_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

    
/**
 * @brief This function should be called only once, before any other AT functions are called.
 *
 * @param netconn_max the maximum number of the link in the at module
 * @param custom_version version information by custom
 */
void opl_at_module_init(uint32_t netconn_max, const char *custom_version);


#ifdef __cplusplus
}
#endif

#endif /* __OPL_AT_H__ */
