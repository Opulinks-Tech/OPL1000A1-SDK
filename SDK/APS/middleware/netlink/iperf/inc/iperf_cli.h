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

#ifndef __IPERF_CLI_H__
#define __IPERF_CLI_H__

#include "cli.h"


#ifdef __cplusplus
extern "C" {
#endif

#if 1
typedef void (*T_IperfCmdProc)(int len, char *param[]);

extern T_IperfCmdProc iperf_cmd_proc;

void Iperf_PreInit(void);
#else

#if 1//defined(CLI_ENABLE) && defined(IPERF_ENABLE)
extern cli_command_t iperf_cli[];
#define IPERF_CLI_ENTRY { "iperf", "iperf", NULL, iperf_cli },
#else
#define IPERF_CLI_ENTRY
#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* __IPERF_CLI_H__ */

