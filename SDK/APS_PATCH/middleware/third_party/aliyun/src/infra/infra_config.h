#ifndef _INFRA_CONFIG_H_
#define _INFRA_CONFIG_H_

#define PLATFORM_HAS_STDINT
#define PLATFORM_HAS_DYNMEM
#define INFRA_COMPAT
#define INFRA_NET
#define INFRA_REPORT
#define INFRA_SHA256
#define INFRA_STRING
#define INFRA_TIMER
#define MQTT_COMM_ENABLED

// modified for ali_lib by Jeff, 20190525
// redefine the HAL_Printf function
#if 1   // the modified
#define HAL_Printf  printf
#else   // the original
#endif

#endif

