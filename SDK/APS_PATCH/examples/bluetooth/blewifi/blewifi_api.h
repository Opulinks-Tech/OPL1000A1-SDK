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

#ifndef __BLEWIFI_API_H__
#define __BLEWIFI_API_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************
 *                    Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/


/******************************************************
 *                   Enumerations
 ******************************************************/


/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
void blewifi_ble_start_initialization(void);
void blewifi_ble_start_advertising(void);
void blewifi_ble_stop_advertising(void);
void blewifi_ble_init(void);



#ifdef __cplusplus
}
#endif

#endif /* __BLEWIFI_API_H__ */

