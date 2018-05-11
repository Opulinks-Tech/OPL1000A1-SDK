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

#ifndef _PINMUX_PWM__H_
#define _PINMUX_PWM__H_

#include "Hal_pinmux_common.h"
#include "hal_pwm.h"
/**
 * @brief  Pinmux pwm init    
 *
 * @param[in] the struct of pwm
 *                
 * @return - 0: successful
 *         - other: failed
 */
ERR_CODE Hal_Pinmux_Pwm_Init(T_OPL1000_Pwm *pwm);

#endif

