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

#include "Hal_pinmux_gpio.h"


ERR_CODE Hal_Pinmux_Gpio_Init(T_OPL1000_Gpio *gpio)
{
    if(gpio->pin < GPIO_IDX_MAX)
    {
        if(gpio->type == IO_OUTPUT)                                     //IO_OUTPUT
        {
            Hal_Vic_GpioDirection((E_GpioIdx_t)gpio->pin, GPIO_OUTPUT);
            if(gpio->pull == PULL_DOWN)                                 //PULL_DOWN
            {
                Hal_Vic_GpioOutput((E_GpioIdx_t)gpio->pin, LEV_LOW);    //0: low 
            }
            else if(gpio->pull == PULL_UP)                              //PULL_UP
            {
                Hal_Vic_GpioOutput((E_GpioIdx_t)gpio->pin, LEV_HIGH);   //1: high
            }
        }
        else if(gpio->type == IO_INPUT)                                 //IO_INPUT
        {
            Hal_Vic_GpioDirection((E_GpioIdx_t)gpio->pin, GPIO_INPUT);  //the GPIO direction
        }
        else
        {
            return RET_FAIL;
        }
    }

    return RET_SUCCESS;
}








