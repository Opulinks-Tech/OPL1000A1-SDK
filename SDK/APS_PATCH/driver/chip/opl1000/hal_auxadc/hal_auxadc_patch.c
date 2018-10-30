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
*  hal_auxadc_patch.c
*
*  Project:
*  --------
*  OPL1000 Project - the AUXADC implement file
*
*  Description:
*  ------------
*  This implement file is include the AUXADC function and api.
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
#include <string.h>
#include "hal_auxadc.h"
#include "hal_auxadc_internal.h"
#include "hal_auxadc_patch.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define AOS                     ((S_Aos_Reg_t *) AOS_BASE)
#define RF                      ((S_Rf_Reg_t *) RF_BASE)

#define RF_RG_EOCB              (1 << 16)   // the bit[16] of RG_AUX_ADC_ECL_OUT: End of conversion sign

#define HAL_AUX_ADC_ERROR_COUNT (3)


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct
{
    volatile uint32_t RET_MUX;            // 0x000
    volatile uint32_t MODE_CTL;           // 0x004
    volatile uint32_t OSC_SEL;            // 0x008
    volatile uint32_t SLP_TIMER_CURR_L;   // 0x00C
    volatile uint32_t SLP_TIMER_CURR_H;   // 0x010
    volatile uint32_t SLP_TIMER_PRESET_L; // 0x014
    volatile uint32_t SLP_TIMER_PRESET_H; // 0x018
    volatile uint32_t PS_TIMER_PRESET;    // 0x01C
    volatile uint32_t RET_SF_VAL_CTL;     // 0x020
    volatile uint32_t PMU_SF_VAL_CTL;     // 0x024
    volatile uint32_t HPBG_CTL;           // 0x028
    volatile uint32_t LPBG_CTL;           // 0x02C
    volatile uint32_t BUCK_CTL;           // 0x030
    volatile uint32_t ON1_TIME;           // 0x034
    volatile uint32_t ON2_TIME;           // 0x038
    volatile uint32_t ON3_TIME;           // 0x03C
    volatile uint32_t ON4_TIME;           // 0x040
    volatile uint32_t ON5_TIME;           // 0x044
    volatile uint32_t ON6_TIME;           // 0x048
    volatile uint32_t ON7_TIME;           // 0x04C
    volatile uint32_t CPOR_N_ON_TIME;     // 0x050
    volatile uint32_t reserve_054;        // 0x054, reserved
    volatile uint32_t SPS_TIMER_PRESET;   // 0x058
    volatile uint32_t SON1_TIME;          // 0x05C
    volatile uint32_t SON2_TIME;          // 0x060
    volatile uint32_t SON3_TIME;          // 0x064
    volatile uint32_t SON4_TIME;          // 0x068
    volatile uint32_t SON5_TIME;          // 0x06C
    volatile uint32_t SON6_TIME;          // 0x070
    volatile uint32_t SON7_TIME;          // 0x074
    volatile uint32_t SCPOR_N_ON_TIME;    // 0x078
    volatile uint32_t PU_CTL;             // 0x07C
    volatile uint32_t OSC_CTL;            // 0x080
    volatile uint32_t PMS_SPARE;          // 0x084, HW reservd for debug
    volatile uint32_t ADC_CTL;            // 0x088
    volatile uint32_t LDO_CTL;            // 0x08C
    volatile uint32_t RG_PD_IE;           // 0x090
    volatile uint32_t RG_PD_PE;           // 0x094
    volatile uint32_t RG_PD_O_INV;        // 0x098
    volatile uint32_t RG_PD_DS;           // 0x09C
    volatile uint32_t RG_GPO;             // 0x0A0
    volatile uint32_t RG_PD_I_INV;        // 0x0A4
    volatile uint32_t RG_PDOV_MODE;       // 0x0A8
    volatile uint32_t RG_PD_DIR;          // 0x0AC
    volatile uint32_t RG_PD_OENP_INV;     // 0x0B0
    volatile uint32_t RG_PDOC_MODE;       // 0x0B4
    volatile uint32_t RG_GPI;             // 0x0B8
    volatile uint32_t reserve_0bc;        // 0x0BC, reserved
    volatile uint32_t RG_PDI_SRC_IO_A;    // 0x0C0
    volatile uint32_t RG_PDI_SRC_IO_B;    // 0x0C4
    volatile uint32_t RG_PDI_SRC_IO_C;    // 0x0C8
    volatile uint32_t RG_PDI_SRC_IO_D;    // 0x0CC
    volatile uint32_t RG_PTS_INMUX_A;     // 0x0D0
    volatile uint32_t RG_PTS_INMUX_B;     // 0x0D4
    volatile uint32_t RG_PTS_INMUX_C;     // 0x0D8
    volatile uint32_t RG_PTS_INMUX_D;     // 0x0DC
    volatile uint32_t RG_SRAM_IOS_EN;     // 0x0E0
    volatile uint32_t RG_SRAM_RET_OFF;    // 0x0E4
    volatile uint32_t RG_PHY_WR_SRAM;     // 0x0E8
    volatile uint32_t RG_PHY_RD_SRAM;     // 0x0EC
    volatile uint32_t CAL_CEN;            // 0x0F0
    volatile uint32_t CAL_STR;            // 0x0F4
    volatile uint32_t SDM_PT_SEL;         // 0x0F8
    volatile uint32_t SDM_CTL;            // 0x0FC
    volatile uint32_t R_STRAP_MODE_CTL;   // 0x100
    volatile uint32_t R_APS_SWRST;        // 0x104
    volatile uint32_t R_MSQ_SWRST;        // 0x108
    volatile uint32_t RG_SPARE;           // 0x10C
    volatile uint32_t RG_PTS_INMUX_E;     // 0x110
    volatile uint32_t RG_PTS_INMUX_F;     // 0x114
    volatile uint32_t RG_SRAM_RET_ACK;    // 0x118
    volatile uint32_t RG_MSQ_ROM_MAP;     // 0x11C
    volatile uint32_t RG_AOS_ID;          // 0x120
    volatile uint32_t RG_SPARE_1;         // 0x124
    volatile uint32_t RG_RSTS;            // 0x128
    volatile uint32_t RG_SPARE_2;         // 0x12C
    volatile uint32_t RG_SPARE_3;         // 0x130
    volatile uint32_t R_M3CLK_SEL;        // 0x134
    volatile uint32_t R_M0CLK_SEL;        // 0x138
    volatile uint32_t R_RFPHY_SEL;        // 0x13C
    volatile uint32_t R_SCRT_EN;          // 0x140
    volatile uint32_t reserve_144[21];    // 0x144 ~ 0x194, move to sys_reg
    volatile uint32_t R_CLK_MMFACTOR_CM3; // 0x198
} S_Aos_Reg_t;

typedef struct
{
    volatile uint32_t reserve_000[2];     // 0x000 ~ 0x004, reserved
    volatile uint32_t RG_SPARE;           // 0x008
    volatile uint32_t reserve_00C[15];    // 0x00C ~ 0x044, reserved
    volatile uint32_t RG_CK_GATE_CTRL;    // 0x048
    volatile uint32_t reserve_04C[4];     // 0x04C ~ 0x058, reserved
    volatile uint32_t AUXADC_CTRL0;       // 0x05C
    volatile uint32_t reserve_060[1];     // 0x060 ~ 0x060, reserved
    volatile uint32_t RG_AUX_IN_SEL;      // 0x064
    volatile uint32_t reserve_068[10];    // 0x068 ~ 0x08C, reserved
    volatile uint32_t PU_VAL;             // 0x090
    volatile uint32_t reserve_094[14];    // 0x094 ~ 0x0C8, reserved
    volatile uint32_t AUX_ADC_CK_GEN_CTL; // 0x0CC
    volatile uint32_t RG_AUX_ADC_ECL_OUT; // 0x0D0
} S_Rf_Reg_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable
extern E_HalAux_Src_t g_tHalAux_CurrentType;
RET_DATA uint32_t g_ulHalAux_AverageCount;


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
*   Hal_Aux_AdcValueGet
*
* DESCRIPTION:
*   get the ADC value from AUXADC
*
* PARAMETERS
*   1. pulValue : [Out] the ADC value
*
* RETURNS
*   1. HAL_AUX_OK   : success
*   2. HAL_AUX_FAIL : fail
*
*************************************************************************/
uint8_t Hal_Aux_AdcValueGet_patch(uint32_t *pulValue)
{
    volatile uint32_t tmp;
    volatile uint32_t i;
    uint8_t ubRet = HAL_AUX_FAIL;
    
    uint32_t ulAdcValue;
    uint32_t ulAdcCurrent;
    uint32_t ulRepeatCount;
    uint32_t j;
    
    if (g_ulHalAux_AverageCount == 0)
        g_ulHalAux_AverageCount = 1;
    
    ulAdcValue = 0;
    ulRepeatCount = g_ulHalAux_AverageCount;
    for (j=0; j<ulRepeatCount; j++)
    {
        // Enable AUXADC
        tmp = AOS->ADC_CTL;
        tmp &= ~(0x1 << 0);
        tmp |= (0x1 << 0);
        AOS->ADC_CTL = tmp;

        // Enable the internal temperature sensor
        if (g_tHalAux_CurrentType == HAL_AUX_SRC_TEMP_SEN)
        {
            tmp = AOS->HPBG_CTL;
            tmp &= ~(0x1 << 18);
            tmp |= (0x1 << 18);
            AOS->HPBG_CTL = tmp;
        }
        
        // Turn on PU of AUXADC
        tmp = RF->PU_VAL;
        tmp &= ~(0x1 << 26);
        tmp |= (0x1 << 26);
        RF->PU_VAL = tmp;

        // Turn on clock to AUXADC
        tmp = RF->RG_CK_GATE_CTRL;
        tmp &= ~(0x1 << 6);
        tmp |= (0x1 << 6);
        RF->RG_CK_GATE_CTRL = tmp;

        // Trigger
        tmp = RF->AUX_ADC_CK_GEN_CTL;
        tmp &= ~(0x1 << 0);
        tmp |= (0x1 << 0);
        RF->AUX_ADC_CK_GEN_CTL = tmp;

        // get the ADC value
        i = 0;
        while (RF->RG_AUX_ADC_ECL_OUT & RF_RG_EOCB)
        {
            if (i >= 0xFF)
                goto done;
            i++;
        }
        ulAdcCurrent = RF->RG_AUX_ADC_ECL_OUT & 0x03FF;
        ulAdcValue = ulAdcValue + ulAdcCurrent;

        // !!! ADC value should be not zero, the DC offset is not zero
        if (ulAdcCurrent == 0)
        {
            // error handle if always zero
            if (ulRepeatCount < (g_ulHalAux_AverageCount * HAL_AUX_ADC_ERROR_COUNT))
                ulRepeatCount++;
        }

        // Disable AUXADC
        tmp = AOS->ADC_CTL;
        tmp &= ~(0x1 << 0);
        tmp |= (0x0 << 0);
        AOS->ADC_CTL = tmp;
        
        // Disable the internal temperature sensor
        if (g_tHalAux_CurrentType == HAL_AUX_SRC_TEMP_SEN)
        {
            tmp = AOS->HPBG_CTL;
            tmp &= ~(0x1 << 18);
            tmp |= (0x0 << 18);
            AOS->HPBG_CTL = tmp;
        }

        // Turn off PU of AUXADC
        tmp = RF->PU_VAL;
        tmp &= ~(0x1 << 26);
        tmp |= (0x0 << 26);
        RF->PU_VAL = tmp;

        // Turn off clock to AUXADC
        tmp = RF->RG_CK_GATE_CTRL;
        tmp &= ~(0x1 << 6);
        tmp |= (0x0 << 6);
        RF->RG_CK_GATE_CTRL = tmp;
        
        // Idle (non-trigger)
        tmp = RF->AUX_ADC_CK_GEN_CTL;
        tmp &= ~(0x1 << 0);
        tmp |= (0x0 << 0);
        RF->AUX_ADC_CK_GEN_CTL = tmp;
    }

    *pulValue = (ulAdcValue + (g_ulHalAux_AverageCount / 2)) / g_ulHalAux_AverageCount;
    ubRet = HAL_AUX_OK;

done:
    return ubRet;
}
