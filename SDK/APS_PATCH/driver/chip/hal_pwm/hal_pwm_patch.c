/******************************************************************************
*  Copyright 2017 - 2018, Opulinks Technology Ltd.
*  ----------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Opulinks Technology Ltd. (C) 2018
******************************************************************************/

/******************************************************************************
*  Filename:
*  ---------
*  hal_pwm_patch.c
*
*  Project:
*  --------
*  NL1000 Project - the PWM implement file
*
*  Description:
*  ------------
*  This implement file is include the PWM function and api.
*
*  Author:
*  -------
*  Jeff Kuo
*
******************************************************************************/
/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file


// Sec 1: Include File
#include "nl1000.h"
#include "hal_system.h"
#include "hal_pwm.h"
#include "hal_pwm_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define PWM                         ((S_Pwm_Reg_t *) PWM_BASE)

/* PWM Register Definitions */
#define PWM_MODE_POS                0U
#define PWM_MODE_MASK               (0x01UL << PWM_MODE_POS)
#define PWM_MODE_INDIVIDUAL         (0x00UL << PWM_MODE_POS)
#define PWM_MODE_SYNC               (0x01UL << PWM_MODE_POS)

#define PWM_SYNC_POS                1U
#define PWM_SYNC_MASK               (0x01UL << PWM_SYNC_POS)
#define PWM_SYNC_DISABLE            (0x00UL << PWM_SYNC_POS)
#define PWM_SYNC_ENABLE             (0x01UL << PWM_SYNC_POS)

#define PWM_IDX_ENABLE_POS          8U
#define PWM_IDX_ENABLE_MASK         (HAL_PWM_IDX_ALL << PWM_IDX_ENABLE_POS)

#define PWM_RAMP_UP_POS             0U
#define PWM_RAMP_UP_MASK            (0xFFUL << PWM_RAMP_UP_POS)

#define PWM_RAMP_DOWN_POS           8U
#define PWM_RAMP_DOWN_MASK          (0xFFUL << PWM_RAMP_DOWN_POS)

#define PWM_PERIOD_POS              16U
#define PWM_PERIOD_MASK             (0xFFUL << PWM_PERIOD_POS)

#define PWM_HOLD_BRIGHT_POS         0U
#define PWM_HOLD_BRIGHT_MASK        (0x01FFUL << PWM_HOLD_BRIGHT_POS)

#define PWM_HOLD_DULL_POS           16U
#define PWM_HOLD_DULL_MASK          (0x01FFUL << PWM_HOLD_DULL_POS)

#define PWM_DUTY_BRIGHT_POS         0U
#define PWM_DUTY_BRIGHT_MASK        (0xFFUL << PWM_DUTY_BRIGHT_POS)

#define PWM_DUTY_DULL_POS           16U
#define PWM_DUTY_DULL_MASK          (0xFFUL << PWM_DUTY_DULL_POS)

// the default value of 32KHz and 22MHz
#define PWM_32KHZ                   32768UL
#define PWM_22MHZ                   22000000UL


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct
{
    volatile uint32_t Ctrl;                 /* Offset: 0x000      (R/W) PWM Control Register */
    volatile uint32_t Period[6];            /* Offset: 0x004~0x18 (R/W) PWM Ramp and Period Register */
    volatile uint32_t Hold[6];              /* Offset: 0x01C~0x30 (R/W) PWM Hold Time Register */
    volatile uint32_t Duty[6];              /* Offset: 0x034~0x48 (R/W) PWM Duty Register */
} S_Pwm_Reg_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable


// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   Hal_Pwm_SyncEnable
*
* DESCRIPTION:
*   enable the index of PWM synchronously
*
* PARAMETERS
*   1. ubIdxMask       : [In] the bit mask of index
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_Pwm_SyncEnable(uint8_t ubIdxMask)
{
    // error handler
    ubIdxMask = ubIdxMask & HAL_PWM_IDX_ALL;

    // step 1. mode
    PWM->Ctrl = PWM_MODE_SYNC;
    // step 2. index
    PWM->Ctrl = PWM_MODE_SYNC | (ubIdxMask << PWM_IDX_ENABLE_POS);
    // step 3. sync start
    PWM->Ctrl = PWM_MODE_SYNC | PWM_SYNC_ENABLE | (ubIdxMask << PWM_IDX_ENABLE_POS);
}
