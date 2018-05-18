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

/******************************************************************************
*  Filename:
*  ---------
*  hal_system_patch.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the patch proto-types of system functions
*
*  Author:
*  -------
*  Chung-Chun Wang
******************************************************************************/

/***********************
Head Block of The File
***********************/
// Sec 0: Comment block of the file

// Sec 1: Include File
#include "nl1000.h"
#include "hal_system_patch.h"
#include "hal_dbg_uart_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define RF_BASE         (MSQ_PERIPH_BASE + 0x9000)

#define AOS             ((S_Aos_Reg_t *) AOS_BASE)
#define SYS_REG         ((S_Sys_Reg_t *) SYS_BASE)
#define RF              ((S_Rf_Reg_t *) RF_BASE)

#define RF_BASE             (MSQ_PERIPH_BASE + 0x9000)
#define P_RC_OSC_CAL_CTL    (volatile uint32_t *)(RF_BASE + 0xC0)
#define P_RC_OSC_CAL_OUTPUT (volatile uint32_t *)(RF_BASE + 0xC4)

#define SYS_SRAM_BYPASS_APS          (1<<2)
#define SYS_SRAM_BYPASS_SHARE        (1<<3)

#define SYS_SW_RESET_ALL             (1<<0)

#define SYS_SW_DBG_EN                (1<<2)

#define SYS_ECO_VER_SHIFT            20
#define SYS_ECO_VER_MASK             (0xFF<<SYS_ECO_VER_SHIFT)

#define AOS_STRAP_MODE_MAX           0xF
#define AOS_STRAP_MODE_MASK          0xF

#define AOS_RET_HW_SETTING           1
#define AOS_RET_FW_SETTING           0

#define AOS_SLP_MODE_EN              (1<<0)
#define AOS_SLP_MODE_TIMER           (0<<1)
#define AOS_SLP_MODE_DEEP            (1<<1)
#define AOS_SLP_MODE_MASK            (0x1<<1)
#define AOS_SLP_MODE_EARLY_WAKE_UP   (0x1<<2)

#define AOS_SLP_TIMER_MASK_L         0xFFFFFFFF
#define AOS_SLP_TIMER_MASK_H         0x1F
#define AOS_SLP_TIMER_MASK_MAX       ((uint64_t)(AOS_SLP_TIMER_MASK_H)<<32 | AOS_SLP_TIMER_MASK_L)


#define AOS_STRAP_MODE_OVERWRITE_EN  (1<<4)

#define AOS_RST_DELAY                30

#define AOS_APS_RST_RFM              (1<<0)
#define AOS_APS_RST_I2C              (1<<2)
#define AOS_APS_RST_PWM              (1<<3)
#define AOS_APS_RST_TMR_0            (1<<4)
#define AOS_APS_RST_TMR_1            (1<<5)
#define AOS_APS_RST_WDT              (1<<6)
#define AOS_APS_RST_VIC              (1<<7)
#define AOS_APS_RST_SYS_BUS          (1<<8)
#define AOS_APS_RST_SPI_0            (1<<9)
#define AOS_APS_RST_SPI_1            (1<<10)
#define AOS_APS_RST_SPI_2            (1<<11)
#define AOS_APS_RST_UART_0           (1<<12)
#define AOS_APS_RST_UART_1           (1<<13)
#define AOS_APS_RST_DBG_UART         (1<<14)
#define AOS_APS_RST_OTP              (1<<15)
#define AOS_APS_RST_DMA              (1<<16)
#define AOS_APS_RST_ROM              (1<<17)
#define AOS_APS_RST_RAM              (1<<18)
#define AOS_APS_RST_JTAG             (1<<19)
#define AOS_APS_RST_AHB_TO_APB       (1<<20)
#define AOS_APS_RST_BRIDGE_APS       (1<<21)
#define AOS_APS_RST_BRIDGE_MSQ       (1<<22)
#define AOS_APS_RST_CM3              (1<<23)
#define AOS_APS_RST_SCRT             (1<<24)
#define AOS_APS_RST_TAP              (1<<25)

#define AOS_MSQ_RST_ROM              (1<<0)
#define AOS_MSQ_RST_SDM_XTAL         (1<<1)
#define AOS_MSQ_RST_BRIDGE_MSQ       (1<<3)
#define AOS_MSQ_RST_BRIDGE_APS       (1<<4)
#define AOS_MSQ_RST_MAC              (1<<5)
#define AOS_MSQ_RST_PHY              (1<<6)
#define AOS_MSQ_RST_WDT              (1<<7)
#define AOS_MSQ_RST_VIC              (1<<8)
#define AOS_MSQ_RST_PHY_REG          (1<<9)
#define AOS_MSQ_RST_AOS              (1<<10)
#define AOS_MSQ_RST_CM0              (1<<11)
#define AOS_MSQ_RST_RFPHY            (1<<12)
#define AOS_MSQ_RST_SPI              (1<<13)
#define AOS_MSQ_RST_DBG_UART         (1<<14)
#define AOS_MSQ_RST_PROG_PU          (1<<15)

#define AOS_RET_RAM_MASK             0xFFFFF /* 20 bits*/
#define AOS_RET_RAM_TIMEOUT          0x500

#define AOS_MSQ_ROM_REMAP_ALL_EN     1

#define AOS_PWM_CLK_MASK             (0x1<<27)

#define CLK_RC                       22000000
#define CLK_XTAL                     22000000
#define CLK_RF                       176000000

#define RC_CAL_PATTERN_LEN           12
#define RC_CAL_PATTERN_MSK           ( (1 << RC_CAL_PATTERN_LEN) - 1)

#define AOS_APS_CLK_SRC_RC           0
#define AOS_APS_CLK_SRC_XTAL         1
#define AOS_APS_CLK_SRC_XTAL_X2      2
#define AOS_APS_CLK_SRC_176M_SWITCH  3
#define AOS_APS_CLK_SRC_MASK         0x3
#define AOS_APS_CLK_176M_SRC_XTAL_X4 ( ((uint32_t)0<<31) | (0<<29) )
#define AOS_APS_CLK_176M_SRC_DECI    ( ((uint32_t)0<<31) | (1<<29) )
#define AOS_APS_CLK_176M_SRC_1P2G    ( ((uint32_t)1<<31) | (0<<29) )
#define AOS_APS_CLK_176M_SRC_EXT     ( ((uint32_t)1<<31) | (1<<29) )
#define AOS_APS_CLK_176M_SRC_MASK    ( ((uint32_t)0x1<<31) | (0x1<<29) )
#define AOS_APS_CLK_EN_GLOBAL        (1<<4)
#define AOS_APS_CLK_EN_I2C_PCLK      (1<<5)
#define AOS_APS_CLK_EN_TMR_0_PCLK    (1<<6)
#define AOS_APS_CLK_EN_TMR_1_PCLK    (1<<7)
#define AOS_APS_CLK_EN_WDT_PCLK      (1<<8)
#define AOS_APS_CLK_EN_VIC_PCLK      (1<<9)
#define AOS_APS_CLK_EN_SPI_0_PCLK    (1<<10)
#define AOS_APS_CLK_EN_SPI_1_PCLK    (1<<11)
#define AOS_APS_CLK_EN_SPI_2_PCLK    (1<<12)
#define AOS_APS_CLK_EN_UART_0_PCLK   (1<<13)
#define AOS_APS_CLK_EN_UART_1_PCLK   (1<<14)
#define AOS_APS_CLK_EN_DBG_UART_PCLK (1<<15)
#define AOS_APS_CLK_EN_OTP_PCLK      (1<<16)
#define AOS_APS_CLK_EN_CM3_HCLK      (1<<17)
#define AOS_APS_CLK_EN_DMA_HCLK      (1<<18)
#define AOS_APS_CLK_EN_ROM_HCLK      (1<<19)
#define AOS_APS_CLK_EN_RAM_HCLK      (1<<20)
#define AOS_APS_CLK_EN_APB_HCLK      (1<<21)
#define AOS_APS_CLK_EN_APS_HCLK      (1<<22)
#define AOS_APS_CLK_EN_MSQ_HCLK      (1<<23)
#define AOS_APS_CLK_EN_SCRT_HCLK     (1<<24)
#define AOS_APS_CLK_EN_FCLK          (1<<25)
#define AOS_APS_CLK_EN_PWM_CLK       (1<<26)
#define AOS_APS_CLK_EN_JTAG_HCLK     (1<<28)
#define AOS_APS_CLK_EN_WDT_INTERNAL  (1<<30)
#define AOS_APS_CLK_DIV2             (1<<2)
#define AOS_APS_PCLK_DIV2            (1<<3)

#define AOS_MSQ_SRC_RC               0
#define AOS_MSQ_SRC_XTAL             1
#define AOS_MSQ_SRC_XTAL_X2          2
#define AOS_MSQ_SRC_EXT              3
#define AOS_MSQ_CLK_SRC_MASK         0x3
#define AOS_MSQ_CLK_EN_GLOBAL        (1<<2)
#define AOS_MSQ_CLK_EN_ROM_HCLK      (1<<4)
#define AOS_MSQ_CLK_EN_RAM_HCLK      (1<<5)
#define AOS_MSQ_CLK_EN_MSQ_HCLK      (1<<7)
#define AOS_MSQ_CLK_EN_APS_HCLK      (1<<8)
#define AOS_MSQ_CLK_EN_MAC_HCLK      (1<<9)
#define AOS_MSQ_CLK_EN_PHY_HCLK      (1<<10)
#define AOS_MSQ_CLK_EN_PHY_REG_HCLK  (1<<11)
#define AOS_MSQ_CLK_EN_AOS_HCLK      (1<<12)
#define AOS_MSQ_CLK_EN_WDT_HCLK      (1<<13)
#define AOS_MSQ_CLK_EN_VIC_HCLK      (1<<14)
#define AOS_MSQ_CLK_EN_FCLK          (1<<15)
#define AOS_MSQ_CLK_EN_DCLK          (1<<16)
#define AOS_MSQ_CLK_EN_SCLK          (1<<17)
#define AOS_MSQ_CLK_EN_PU_HCLK       (1<<18)
#define AOS_MSQ_CLK_EN_CM0_HCLK      (1<<19)
#define AOS_MSQ_CLK_EN_WDT           (1<<20)
#define AOS_MSQ_CLK_EN_SPI           (1<<21)
#define AOS_MSQ_CLK_EN_UART          (1<<22)
#define AOS_MSQ_CLK_DIV2             (1<<3)

#define AOS_APS_CLK_DIV2_UNGATED     (1<<23)
#define AOS_APS_PCLK_DIV2_UNGATED    (1<<24)
#define AOS_MSQ_CLK_DIV2_UNGATED     (1<<25)

#define AOS_PHY_RF_TEST_SRC_S_22M    (0<<4)
#define AOS_PHY_RF_TEST_SRC_S_44M    (1<<4)
#define AOS_PHY_RF_TEST_SRC_Q_44M    (2<<4)
#define AOS_PHY_RF_TEST_SRC_RFMMD    (3<<4)
#define AOS_PHY_RF_TEST_SRC_MASK     (0x3<<4)
#define AOS_PHY_RF_CAP_SRC_S_044M    (0<<6)
#define AOS_PHY_RF_CAP_SRC_Q_176M    (1<<6)
#define AOS_PHY_RF_CAP_SRC_MASK      (0x3<<6)
#define AOS_PHY_CLK_S_EN             (1<<0)
#define AOS_PHY_CLK_Q_EN             (1<<1)
#define AOS_PHY_CLK_RFMMD_EN         (1<<2)
#define AOS_PHY_CLK_S_22M_TX_EN      (1<<8)
#define AOS_PHY_CLK_S_22M_RX_EN      (1<<9)
#define AOS_PHY_CLK_S_44M_TX_EN      (1<<10)
#define AOS_PHY_CLK_S_44M_RX_EN      (1<<11)
#define AOS_PHY_CLK_Q_176M_TX_EN     (1<<12)
#define AOS_PHY_CLK_Q_176M_RX_EN     (1<<13)
#define AOS_PHY_CLK_Q_088M_TX_EN     (1<<15)
#define AOS_PHY_CLK_Q_088M_RX_EN     (1<<16)
#define AOS_PHY_CLK_Q_044M_TX_EN     (1<<17)
#define AOS_PHY_CLK_Q_044M_RX_EN     (1<<18)
#define AOS_PHY_CLK_CAP_FIFO2SRAM_EN (1<<14)
#define AOS_PHY_CLK_CAP_DATA2FIFO_EN (1<<19)
#define AOS_PHY_CLK_RFMMD_INV        (1<<20)
#define AOS_PHY_CLK_Q_INV            (1<<21)
#define AOS_PHY_CLK_S_INV            (1<<22)
#define AOS_PHY_CLK_S_DIV2           (1<<3)

#define AOS_PHY_CLK_S_DIV2_UNGATED   (1<<23)

#define AOS_M3_MMFACTOR_MAX          0x10
#define AOS_M3_MMFACTOR_MASK         0x1F
#define AOS_M3_MMFACTOR_EN           (1<<5)

#define SYS_PINMUX_OPTION_1     1   // UART1
#define SYS_PINMUX_OPTION_2     2   // UART0 + UART1
#define SYS_PINMUX_OPTION_3     3   // Reserved
#define SYS_PINMUX_TYPE         SYS_PINMUX_OPTION_1

// AOS 0x28
#define AOS_PTAT_EN (1 << 17)

// RC calibration, in RF bank
// 0xC0
#define RC_CAL_RST (1 << 0)
#define RC_CAL_EN  (1 << 1)

// 0xC4
#define RC_CAL_XTAL_CNT_POS 0
#define RC_CAL_XTAL_CNT_MSK (0x3FFF << RC_CAL_XTAL_CNT_POS)
#define RC_CAL_RC_CNT_POS   16
#define RC_CAL_RC_CNT_MSK   (0x3FFF << RC_CAL_RC_CNT_POS)

/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list...
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
    volatile uint32_t reserve_r0[3];         // 0x000 ~ 0x008, reserved
    volatile uint32_t R_SRAM_BYPASS;         // 0x00C
    volatile uint32_t R_SW_RESET_EN;         // 0x010
    volatile uint32_t R_SW_DBG_EN;           // 0x014
    volatile uint32_t R_BOOT_STATUS;         // 0x018
    volatile uint32_t R_CHIP_ID;             // 0x01C
    volatile uint32_t reserve_r1[12];        // 0x020 ~ 0x04C, reserved
    volatile uint32_t R_CM3_I_PATCH[128];    // 0x050 ~ 0x24C
    volatile uint32_t R_CM3_I_PATCH_ST;      // 0x250
    volatile uint32_t R_CM3_I_PATCH_EN[4];   // 0x254 ~ 0x260
    volatile uint32_t R_CM3_D_P_ADDR[4];     // 0x268 ~ 0x270
    volatile uint32_t R_CM3_D_PATCH_EN;      // 0x274
    volatile uint32_t R_CM3_D_PATCH_DATA[4]; // 0x278 ~ 0x284
    volatile uint32_t reserve_r2[6];         // 0x288 ~ 0x29C, reserved
    volatile uint32_t R_CM0_I_PATCH[128];    // 0x2A0 ~ 0x49C
    volatile uint32_t R_CM0_I_PATCH_ST;      // 0x4A0
    volatile uint32_t R_CM0_I_PATCH_EN[4];   // 0x4A4 ~ 0x4B0
    volatile uint32_t R_CM3_ORIG_ADD[4];     // 0x4B4 ~ 0x4C0
    volatile uint32_t R_CM3_TAG_ADD[4];      // 0x4C4 ~ 0x4D0
    volatile uint32_t R_CM3_RMP_MASK[4];     // 0x4D4 ~ 0x4E0
    volatile uint32_t R_CM0_ORIG_ADD[3];     // 0x4E4 ~ 0x4EC
    volatile uint32_t R_CM0_TAG_ADD[3];      // 0x4F0 ~ 0x4F8
    volatile uint32_t R_CM0_RMP_MASK[3];     // 0x4FC ~ 0x504
} S_Sys_Reg_t;

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

// For patch only (internal use)
typedef void (*T_Hal_Sys_SystemCoreClockUpdate)(uint32_t u32CoreClk);

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
/* Power relative */

/* Sleep Mode relative */

/* Pin-Mux relative*/

/* Ret RAM relative*/

/* Xtal fast starup relative */

/* SW reset relative */

/* Clock relative */

/* Remap relative */

/* Miscellaneous */


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables &  Functions
***************************************************/
// Sec 6: declaration of static global  variable

// Sec 7: declaration of static function prototype


/***********
C Functions
***********/
// Sec 8: C Functions
/*************************************************************************
* FUNCTION:
*  Hal_Sys_SleepTimerGet
*
* DESCRIPTION:
*   1. Setup a sleep event
*   2. Related reg.: AOS 0x014 and 0x018
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   uint64_t 37-bits of timer, Max: 0x1F FFFFFFFF
* GLOBALS AFFECTED
*
*************************************************************************/
extern uint64_t Hal_Sys_SleepTimerGet_impl(void);
uint64_t Hal_Sys_SleepTimerGet_patch(void)
{
    uint64_t u64Time_1 = 0;
    uint64_t u64Time_2 = 0;

    do{
        u64Time_1 = Hal_Sys_SleepTimerGet_impl();
        u64Time_2 = Hal_Sys_SleepTimerGet_impl();
    }while(u64Time_1 != u64Time_2);
    
    return u64Time_1;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ApsModuleRst
*
* DESCRIPTION:
*   1. Reset a module in APS domain
*   2. Related reg.: AOS 0x104
* CALLS
*
* PARAMETERS
*   1. eModule : The module of APS. Refer to E_ApsRstModule_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
extern uint32_t Hal_Sys_ApsModuleRst_impl(E_ApsRstModule_t eModule);
uint32_t Hal_Sys_ApsModuleRst_patch(E_ApsRstModule_t eModule)
{
    volatile uint32_t u32Count = 0;

    if(Hal_Sys_ApsModuleRst_impl(eModule) == 1 )
    {
        // Added delay for setup
        while(u32Count < AOS_RST_DELAY)
            u32Count++;
        
        return 0;
    }else{
        return 1;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_MsqModuleRst
*
* DESCRIPTION:
*   1. Reset a module in MSQ domain
*   2. Related reg.: AOS 0x108
* CALLS
*
* PARAMETERS
*   1. eModule : The module of MSQ. Refer to E_MsqRstModule_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
extern uint32_t Hal_Sys_MsqModuleRst_impl(E_MsqRstModule_t eModule);
uint32_t Hal_Sys_MsqModuleRst_patch(E_MsqRstModule_t eModule)
{
    volatile uint32_t u32Count = 0;

    if(Hal_Sys_MsqModuleRst_impl(eModule) == 1 )
    {
        // Added delay for setup
        while(u32Count < AOS_RST_DELAY)
            u32Count++;
        
        return 0;
    }else{
        return 1;
    }
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PwmSrcSelect
*
* DESCRIPTION:
*   1. PWM clock source select
*   2. Related reg.: AOS 0x134
* CALLS
*
* PARAMETERS
*   1. eSrc : PWM clock select. Refer to E_PwmClkSrc_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PwmSrcSelect_patch(E_PwmClkSrc_t eSrc)
{
    switch(eSrc)
    {
        case PWM_CLK_SRC_32KHZ:
            AOS->R_M3CLK_SEL &= ~AOS_PWM_CLK_MASK;
            break;
        case PWM_CLK_SRC_22MHZ:
            AOS->R_M3CLK_SEL |= AOS_PWM_CLK_MASK;
            break;
        default:
            return 1;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PmuSfVolset
*
* DESCRIPTION:
*   1. PMU_SF voltage select
*   2. Related reg.: AOS 0x024
* CALLS
*
* PARAMETERS
*   1. eVol : PMU_SF voltage select. Refer to E_PmuSfVal_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PmuSfVolSet(E_PmuSfVal_t eVol)
{
    switch(eVol)
    {
        case PMU_SF_VAL_0P65V:
        case PMU_SF_VAL_0P73V:
        case PMU_SF_VAL_0P95V: 
        case PMU_SF_VAL_1P10V:
            AOS->PMU_SF_VAL_CTL = eVol;
            break;
        default:
            return 1;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PwrDefaultSettings
*
* DESCRIPTION:
*   the default settings of power part
* CALLS
*
* PARAMETERS
*
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PwrDefaultSettings(void)
{
    volatile uint32_t tmp;
    
    // Enable PowerManual
    Hal_Sys_PowerManualEn(1);
    
    // Enable CBC               // for all bias (can't be turn off)
    tmp = AOS->HPBG_CTL;
    tmp &= ~(0x1 << 2);
    tmp |= (0x1 << 2);
    AOS->HPBG_CTL = tmp;
    
    // Turn on PU of CBC
    tmp = RF->PU_VAL;
    tmp &= ~(0x1 << 6);
    tmp |= (0x1 << 6);          // bit[6] : for all (can't be turn off)
    RF->PU_VAL = tmp;
    
    // Turn on clock
    tmp = RF->RG_CK_GATE_CTRL;
    tmp &= ~((0x1 << 14) | (0x1 << 13));
    tmp |= ((0x1 << 14) | (0x1 << 13));     // bit[13] : for XTAL
                                            // bit[14] : for RC
    RF->RG_CK_GATE_CTRL = tmp;
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_32kXtalGainSet
*
* DESCRIPTION:
*   1. 32k_Xtal Gain 
*   2. Related reg.: AOS 0x080
* CALLS
*
* PARAMETERS
*   1. u8Gain : Gain of 32K xtal. Max value refer to XTAL_32K_GM_MAX
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_32kXtalGainSet(uint8_t u8Gain)
{
    uint32_t u32Temp = 0;
    
    if(u8Gain > XTAL_32K_GM_MAX)
        return 1;
    
    u32Temp = AOS->OSC_CTL &~ XTAL_32K_GM_MASK;
    AOS->OSC_CTL = u32Temp | (u8Gain << XTAL_32K_GM_POS);
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_32kXtalCapSet
*
* DESCRIPTION:
*   1. 32k_Xtal Gain 
*   2. Related reg.: AOS 0x080
* CALLS
*
* PARAMETERS
*   1. u8Cap : Capacitance of 32K xtal. Max value refer to XTAL_32K_Cap_MAX
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_32kXtalCapSet(uint8_t u8Cap)
{
    uint32_t u32Temp = 0;
    
    if(u8Cap > XTAL_32K_CAP_MAX)
        return 1;
    
    u32Temp = AOS->OSC_CTL &~ XTAL_32K_CAP_MASK;
    AOS->OSC_CTL = u32Temp | (u8Cap << XTAL_32K_CAP_POS);
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ApsClkChangeApply
*
* DESCRIPTION:
*   1. Update all system clock relative
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
extern void Hal_Sys_ApsClkChangeApply_impl(void);
void Hal_Sys_ApsClkChangeApply_patch(void)
{
    Hal_Sys_ApsClkChangeApply_impl();
    
    // dbg UART
    Hal_DbgUart_BaudRateSet_patch(Hal_DbgUart_BaudRateGet());
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_RcCal
*
* DESCRIPTION:
*   1. Calibration RC to 22MHz Xtal
*   2. Related reg.: AOS 0x0F0 amd 0xF4, RF 0xC0 and 0xC4
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_RcCal(void)
{
    int32_t i32Idx;
    uint32_t u32ApsClk = 0;
    uint32_t u32Xtal = 0;
    uint32_t u32Rc = 0;
    
    // Check the clock. make sure not in RC
    u32ApsClk = AOS->R_M3CLK_SEL;
    if( (u32ApsClk & AOS_APS_CLK_SRC_MASK) == AOS_APS_CLK_SRC_RC)
        AOS->R_M3CLK_SEL |= AOS_APS_CLK_SRC_XTAL;
    else
        u32ApsClk = 0;
   
    // Set RC pattern in the middle
    AOS->CAL_CEN = 0;

    // Calibration
    for(i32Idx = RC_CAL_PATTERN_LEN-1 ; i32Idx >= 0; i32Idx--)
    {
        // Disable
        *P_RC_OSC_CAL_CTL &= ~RC_CAL_EN;
        
        // Reset, 0:reset 
        *P_RC_OSC_CAL_CTL &= ~RC_CAL_RST;
        *P_RC_OSC_CAL_CTL |= RC_CAL_RST;
        
        // Set pattern, binary search
        AOS->CAL_CEN |= (1 << i32Idx);
        
        // Enable
        *P_RC_OSC_CAL_CTL |= RC_CAL_EN;
        
        // Check Xtal ticks
        while( 1 )
        {
            u32Xtal = (*P_RC_OSC_CAL_OUTPUT) & RC_CAL_XTAL_CNT_MSK;
            if( u32Xtal >= 10000)
            {
                // Disable
                *P_RC_OSC_CAL_CTL &= ~RC_CAL_EN;
                break;
            }
        }
        
        // Get RC ticks
        u32Rc = ( (*P_RC_OSC_CAL_OUTPUT) & RC_CAL_RC_CNT_MSK ) >> RC_CAL_RC_CNT_POS;
        
        // Binary search
        if(u32Xtal == u32Rc)
        {
            //Find the value
            break;
        }else if(u32Xtal < u32Rc)
        {
            // RC too fast, decreased the pattern value
            AOS->CAL_CEN &= ~(1 << i32Idx);
        }
        // else, RC is slower than Xtal, increased the pattern value.
    }
    
    // Setup start point
    if(AOS->CAL_CEN <= 0x80)
        AOS->CAL_STR = 0;
    else
        AOS->CAL_STR = AOS->CAL_CEN - 0x80;
    
    // Resume clock if had changed
    if(u32ApsClk != 0)
        AOS->R_M3CLK_SEL = u32ApsClk;
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_SysPinMuxAppInit
*
* DESCRIPTION:
*   1. Pin-Mux initial for application stage
*   2. Related reg.: AOS 0x090 ~ 0x0DC
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_SysPinMuxAppInit_patch(void)
{
    volatile uint32_t tmp;

// APS_dbg_uart
    // IO0(TX), IO1(RX)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0xD << 4) | (0xA << 0));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~(0xF << 16);
    tmp |= (0x0 << 16);
    AOS->RG_PTS_INMUX_A = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~(0x1 << 0);
    tmp |= (0x1 << 1);
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOV_MODE = tmp;

#if (SYS_PINMUX_TYPE == SYS_PINMUX_OPTION_1)
// UART1
    // IO6(RTS), IO7(CTS), IO8(TX), IO9(RX)
    // output source
    // IO6, IO7
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~(((uint32_t)0xF << 28) | (0xF << 24));
    tmp |= (((uint32_t)0xD << 28) | (0x3 << 24));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // IO8, IO9
    tmp = AOS->RG_PDI_SRC_IO_B;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0xD << 4) | (0x4 << 0));
    AOS->RG_PDI_SRC_IO_B = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0x2 << 12) | (0x0 << 8));
    AOS->RG_PTS_INMUX_A = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 7) | (0x1 << 6));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 8) | (0x1 << 6));
    tmp |= ((0x1 << 9) | (0x1 << 7));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 7) | (0x1 << 6));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 7) | (0x1 << 6));
    AOS->RG_PDOV_MODE = tmp;

#elif (SYS_PINMUX_TYPE == SYS_PINMUX_OPTION_2)
// UART0
    // IO2(TX), IO3(RX), IO8(CTS), IO9(RTS)
    // output source
    // IO2, IO3
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0xD << 12) | (0x3 << 8));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // IO8, IO9
    tmp = AOS->RG_PDI_SRC_IO_B;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0x3 << 4) | (0xD << 0));
    AOS->RG_PDI_SRC_IO_B = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0x0 << 4) | (0x1 << 0));
    AOS->RG_PTS_INMUX_A = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 3) | (0x1 << 2));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 9) | (0x1 << 2));
    tmp |= ((0x1 << 8) | (0x1 << 3));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 3) | (0x1 << 2));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8) | (0x1 << 3) | (0x1 << 2));
    AOS->RG_PDOV_MODE = tmp;

// UART1
    // IO4(TX), IO5(RX), IO6(RTS), IO7(CTS)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~(((uint32_t)0xF << 28) | (0xF << 24) | (0xF << 20) | (0xF << 16));
    tmp |= (((uint32_t)0xD << 28) | (0x3 << 24) | (0xD << 20) | (0x3 << 16));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0x1 << 12) | (0x0 << 8));
    AOS->RG_PTS_INMUX_A = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 7) | (0x1 << 6) | (0x1 << 5) | (0x1 << 4));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 6) | (0x1 << 4));
    tmp |= ((0x1 << 7) | (0x1 << 5));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 7) | (0x1 << 6) | (0x1 << 5) | (0x1 << 4));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 7) | (0x1 << 6) | (0x1 << 5) | (0x1 << 4));
    AOS->RG_PDOV_MODE = tmp;
#endif

// SPI0 standard mode
    // IO12(CS), IO13(CLK), IO14(MOSI), IO15(MISO)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_B;
    tmp &= ~(((uint32_t)0xF << 28) | (0xF << 24) | (0xF << 20) | (0xF << 16));
    tmp |= ((0x0 << 28) | (0x0 << 24) | (0x0 << 20) | (0x0 << 16));
    AOS->RG_PDI_SRC_IO_B = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_C;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0x0 << 4) | (0x0 << 0));
    AOS->RG_PTS_INMUX_C = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PDOV_MODE = tmp;

// MSQ_dbg_uart
    // IO16(RX), IO17(TX)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_C;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0xB << 4) | (0xD << 0));
    AOS->RG_PDI_SRC_IO_C = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~(0xF << 20);
    tmp |= (0x8 << 20);
    AOS->RG_PTS_INMUX_A = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 17) | (0x1 << 16));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~(0x1 << 17);
    tmp |= (0x1 << 16);
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 17) | (0x1 << 16));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 17) | (0x1 << 16));
    AOS->RG_PDOV_MODE = tmp;

// M0 SWD
    // IO18(CLK), IO19(DAT)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_C;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0x9 << 12) | (0xD << 8));
    AOS->RG_PDI_SRC_IO_C = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_B;
    tmp &= ~((0xF << 24) | (0xF << 20));
    tmp |= ((0x9 << 24) | (0x9 << 20));
    AOS->RG_PTS_INMUX_B = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 19) | (0x1 << 18));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 19) | (0x1 << 18));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 19) | (0x1 << 18));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 19) | (0x1 << 18));
    AOS->RG_PDOV_MODE = tmp;

// M3 SWD
    // IO20(DAT), IO21(CLK)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_C;
    tmp &= ~((0xF << 20) | (0xF << 16));
    tmp |= ((0xD << 20) | (0x8 << 16));
    AOS->RG_PDI_SRC_IO_C = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_B;
    tmp &= ~((0xF << 16) | (0xF << 12));
    tmp |= ((0xA << 16) | (0xA << 12));
    AOS->RG_PTS_INMUX_B = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 21) | (0x1 << 20));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 21) | (0x1 << 20));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 21) | (0x1 << 20));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 21) | (0x1 << 20));
    AOS->RG_PDOV_MODE = tmp;

    // M3 SWD enable
    Hal_Sys_SwDebugEn(0x1);
}

/*************************************************************************
* FUNCTION:
*  Hal_SysPinMuxDownloadInit
*
* DESCRIPTION:
*   1. Pin-Mux initial for download stage
*   2. Related reg.: AOS 0x090 ~ 0x0DC
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_SysPinMuxDownloadInit(void)
{
    volatile uint32_t tmp;

// GPIO
    // IO0(Input), IO1(Input)
    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PD_PE = tmp;

    // input
    tmp = AOS->RG_PD_DIR;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PD_DIR = tmp;
    
    // GPIO pin
    tmp = AOS->RG_PDOC_MODE;
    tmp &= ~((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp &= ~((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOV_MODE = tmp;

    // from APS_dbg_uart to idle
    // output source
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0xD << 4) | (0xD << 0));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // input IO
    //tmp = AOS->RG_PTS_INMUX_A;
    //tmp &= ~(0xF << 16);
    //tmp |= (0x0 << 16);
    //AOS->RG_PTS_INMUX_A = tmp;

#if (SYS_PINMUX_TYPE == SYS_PINMUX_OPTION_1)
// APS_dbg_uart
    // IO8(TX), IO9(RX)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_B;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0xD << 4) | (0xA << 0));
    AOS->RG_PDI_SRC_IO_B = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~(0xF << 16);
    tmp |= (0x4 << 16);
    AOS->RG_PTS_INMUX_A = tmp;
    
    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 9) | (0x1 << 8));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~(0x1 << 8);
    tmp |= (0x1 << 9);
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 9) | (0x1 << 8));
    AOS->RG_PDOV_MODE = tmp;
    
    // from UART1 to idle
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0x0 << 12) | (0x0 << 8));
    AOS->RG_PTS_INMUX_A = tmp;

#elif (SYS_PINMUX_TYPE == SYS_PINMUX_OPTION_2)
// APS_dbg_uart
    // IO4(TX), IO5(RX)
    // output source
    tmp = AOS->RG_PDI_SRC_IO_A;
    tmp &= ~((0xF << 20) | (0xF << 16));
    tmp |= ((0xD << 20) | (0xA << 16));
    AOS->RG_PDI_SRC_IO_A = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~(0xF << 16);
    tmp |= (0x2 << 16);
    AOS->RG_PTS_INMUX_A = tmp;
    
    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 5) | (0x1 << 4));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~(0x1 << 4);
    tmp |= (0x1 << 5);
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 5) | (0x1 << 4));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 5) | (0x1 << 4));
    AOS->RG_PDOV_MODE = tmp;
    
    // from UART1 to idle
    // input IO
    tmp = AOS->RG_PTS_INMUX_A;
    tmp &= ~((0xF << 12) | (0xF << 8));
    tmp |= ((0x0 << 12) | (0x0 << 8));
    AOS->RG_PTS_INMUX_A = tmp;

#endif
    Hal_SysPinMuxSpiFlashInit();
}

/*************************************************************************
* FUNCTION:
*  Hal_SysPinMuxSpiFlashInit
*
* DESCRIPTION:
*   1. Pin-Mux initial for SPI flash
*   2. Related reg.: AOS 0x090 ~ 0x0DC
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_SysPinMuxSpiFlashInit(void)
{
    volatile uint32_t tmp;
    
// SPI0 quad mode
    // IO12(CS), IO13(CLK), IO14(DAT_0), IO15(DAT_1), IO16(DAT_2), IO17(DAT_3)
    // output source
    // IO12, IO13, IO14, IO15
    tmp = AOS->RG_PDI_SRC_IO_B;
    tmp &= ~(((uint32_t)0xF << 28) | (0xF << 24) | (0xF << 20) | (0xF << 16));
    tmp |= ((0x0 << 28) | (0x0 << 24) | (0x0 << 20) | (0x0 << 16));
    AOS->RG_PDI_SRC_IO_B = tmp;
    // IO16, IO17
    tmp = AOS->RG_PDI_SRC_IO_C;
    tmp &= ~((0xF << 4) | (0xF << 0));
    tmp |= ((0x0 << 4) | (0x0 << 0));
    AOS->RG_PDI_SRC_IO_C = tmp;
    // input IO
    tmp = AOS->RG_PTS_INMUX_C;
    tmp &= ~((0xF << 12) | (0xF << 8) | (0xF << 4) | (0xF << 0));
    tmp |= ((0x0 << 12) | (0x0 << 8) | (0x0 << 4) | (0x0 << 0));
    AOS->RG_PTS_INMUX_C = tmp;

    // input Enable
    tmp = AOS->RG_PD_IE;
    tmp |= ((0x1 << 17) | (0x1 << 16) | (0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PD_IE = tmp;

    // pull-up / pull-down
    tmp = AOS->RG_PD_PE;
    tmp &= ~((0x1 << 17) | (0x1 << 16) | (0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 17) | (0x1 << 16) | (0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 17) | (0x1 << 16) | (0x1 << 15) | (0x1 << 14) | (0x1 << 13) | (0x1 << 12));
    AOS->RG_PDOV_MODE = tmp;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_FastStartUpEn
*
* DESCRIPTION:
*   1. Enable/Disable fast start-up (recommand set after Hal_Sys_RcCal)
*   2. Related reg.: AOS 0x0FC
*
* CALLS
*
* PARAMETERS
*   1. u8Enable : 0 for disable, 1 for enable
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_FastStartUpEn(uint8_t u8Enable)
{
    if(u8Enable)
        AOS->SDM_CTL = 0x3;
    else
        AOS->SDM_CTL = 0x0;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PtatEn
*
* DESCRIPTION:
*   1. Enable/Disable PTAT
*   2. Related reg.: AOS 0x028
*
* CALLS
*
* PARAMETERS
*   1. u8Enable : 0 for disable, 1 for enable
*
* RETURNS
*   None
*
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PtatEn(uint8_t u8Enable)
{
    if(u8Enable)
        AOS->HPBG_CTL |= AOS_PTAT_EN;
    else
        AOS->HPBG_CTL &= ~AOS_PTAT_EN;
    return 0;
}
