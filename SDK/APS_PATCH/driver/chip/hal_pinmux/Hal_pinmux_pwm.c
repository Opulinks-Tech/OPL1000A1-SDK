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


#include "Hal_pinmux_pwm.h"


uint8_t Get_Pwm_Index(uint8_t pin)
{
    uint8_t pwm_index =0;

    switch(pin)
    {
        case OPL1000_IO23_PIN:
            pwm_index = HAL_PWM_IDX_0;
            break;
        case OPL1000_IO22_PIN:
            pwm_index = HAL_PWM_IDX_1;
            break;
        case OPL1000_IO21_PIN:
            pwm_index = HAL_PWM_IDX_2;
            break;
        case OPL1000_IO20_PIN:
            pwm_index = HAL_PWM_IDX_3;
            break;
        case OPL1000_IO19_PIN:
            pwm_index = HAL_PWM_IDX_4;
            break;
        case OPL1000_IO18_PIN:
            pwm_index = HAL_PWM_IDX_5;
            break;
        default:
            break;
    }
    return pwm_index;
}

ERR_CODE Hal_Pinmux_Pwm_Init(T_OPL1000_Pwm *pwm)
{
    uint8_t pwm_index = 0;
    S_Hal_Pwm_Config_t sConfig;

    if(pwm->pin > OPL1000_IO18_PIN && pwm->pin < OPL1000_IO23_PIN)
    {
        Hal_Pwm_Init();

        pwm_index = Get_Pwm_Index(pwm->pin);
        printf("pwm_index = %d \r\n",pwm_index);
        Hal_Pwm_ClockSourceSet(pwm->clkSrc);
        if(pwm->cfgType == CFG_SIMPLE)
        {
            printf("Hal_Pwm_SimpleConfigSet \r\n");
            Hal_Pwm_SimpleConfigSet(pwm_index,pwm->duty, pwm->clkHz);
        }
        else if(pwm->cfgType == CFG_COMPLEX)
        {
            sConfig.ulPeriod     = pwm->period;
            sConfig.ulDutyBright = pwm->dutyBright;
            sConfig.ulDutyDull   = pwm->dutyDull;
            sConfig.ulRampUp     = pwm->rampUp;
            sConfig.ulRampDown   = pwm->rampDown;
            sConfig.ulHoldBright = pwm->holdBright;
            sConfig.ulHoldDull   = pwm->holdDull;
            printf("Hal_Pwm_ComplexConfigSet \r\n");
            Hal_Pwm_ComplexConfigSet(pwm_index, sConfig);
        }

        Hal_PinMux_OutPutConfig(Hal_Get_Point(pwm->pin,PWM_E));

    }
    else
    {
        printf("pwm init failed\r\n");
    }

    Hal_Pwm_Enable(pwm_index);
    return RET_SUCCESS;
}




