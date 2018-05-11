/**
 * @file lwip_cli.h
 *
 *  lwip_cli command
 *
 */

#ifndef __LWIP_CLI_H__
#define __LWIP_CLI_H__

/**
 * @ingroup LwIP
 * @addtogroup CLI
 * @{
 */
#include <stdint.h>
#include "cli.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * LwIP CLI declaration.
 *
 * Provides commands to debug
 */
extern cli_command_t lwip_cli[];


/**
 * IP configuration command handler.
 *
 * Provides commands to config IP address mode (DHCP/STATIC) and IP address,
 * netmask, and gateway.
 */
int lwip_ip_cli(int len, char *param[]);
int lwip_cli_handler(int token_cnt, char *param[]);

#ifdef __cplusplus
}
#endif


/** }@ */


#endif /* __LWIP_CLI_H__ */


