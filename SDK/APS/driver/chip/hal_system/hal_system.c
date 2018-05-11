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
*  hal_system.c
*
*  Project:
*  --------
*  NL1000_A0 series
*
*  Description:
*  ------------
*  This include file defines the proto-types of system functions
*  Include sys-reg and AOS domains.
*  Ref. documents are:
*    SYS: input.xml (Last modified: 2017/10/26)
*    AOS: aos_reg_A0_171025.xml
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
#include "hal_system.h"
#include "hal_spi.h"
#include "hal_i2c.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define AOS             ((S_Aos_Reg_t *) AOS_BASE)
#define SYS_REG         ((S_Sys_Reg_t *) SYS_BASE)

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

#define AOS_PWM_CLK_MASK             0x1

#define CLK_RC                       22000000
#define CLK_XTAL                     22000000
#define CLK_RF                       176000000

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

// For patch only (internal use)
typedef void (*T_Hal_Sys_SystemCoreClockUpdate)(uint32_t u32CoreClk);

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
/* Power relative */
RET_DATA T_Hal_Sys_PowerManualEn Hal_Sys_PowerManualEn;

/* Sleep Mode relative */
RET_DATA T_Hal_Sys_SleepInit     Hal_Sys_SleepInit;
RET_DATA T_Hal_Sys_SleepTimerGet Hal_Sys_SleepTimerGet;
RET_DATA T_Hal_Sys_SleepTimerSet Hal_Sys_SleepTimerSet;
RET_DATA T_Hal_Sys_SleepTrig     Hal_Sys_SleepTrig;

/* Pin-Mux relative*/
RET_DATA T_Hal_SysPinMuxBootROMInit Hal_SysPinMuxBootROMInit;
RET_DATA T_Hal_SysPinMuxAppInit     Hal_SysPinMuxAppInit;

/* Ret RAM relative*/
RET_DATA T_Hal_Sys_RetRamStatusRead Hal_Sys_RetRamStatusRead;
RET_DATA T_Hal_Sys_RetRamTurnOff    Hal_Sys_RetRamTurnOff;
RET_DATA T_Hal_Sys_RetRamTurnOn     Hal_Sys_RetRamTurnOn;

/* Xtal fast starup relative */

/* SW reset relative */
RET_DATA T_Hal_Sys_SwResetAll   Hal_Sys_SwResetAll;
RET_DATA T_Hal_Sys_ApsModuleRst Hal_Sys_ApsModuleRst;
RET_DATA T_Hal_Sys_MsqModuleRst Hal_Sys_MsqModuleRst;

/* Clock relative */
RET_DATA T_Hal_Sys_ApsClkTreeSetup Hal_Sys_ApsClkTreeSetup;
RET_DATA T_Hal_Sys_MsqClkTreeSetup Hal_Sys_MsqClkTreeSetup;
RET_DATA T_Hal_Sys_PhyClkTreeSetup Hal_Sys_PhyClkTreeSetup;
RET_DATA T_Hal_Sys_PwmSrcSelect    Hal_Sys_PwmSrcSelect;
RET_DATA T_Hal_Sys_ApsClkEn        Hal_Sys_ApsClkEn;
RET_DATA T_Hal_Sys_MsqClkEn        Hal_Sys_MsqClkEn;
RET_DATA T_Hal_Sys_PhyClkEn        Hal_Sys_PhyClkEn;

/* Remap relative */
RET_DATA T_Hal_Sys_MsqRomReampAll Hal_Sys_MsqRomReampAll;

/* Miscellaneous */
RET_DATA T_Hal_Sys_SramDffBypass      Hal_Sys_SramDffBypass;
RET_DATA T_Hal_Sys_SwDebugEn          Hal_Sys_SwDebugEn;
RET_DATA T_Hal_Sys_StrapModeRead      Hal_Sys_StrapModeRead;
RET_DATA T_Hal_Sys_StrapModeOverWrite Hal_Sys_StrapModeOverWrite;
RET_DATA T_Hal_Sys_AosIdRead          Hal_Sys_AosIdRead;
RET_DATA T_Hal_Sys_ChipIdRead         Hal_Sys_ChipIdRead;
RET_DATA T_Hal_Sys_SpareRegRead       Hal_Sys_SpareRegRead;
RET_DATA T_Hal_Sys_SpareRegWrite      Hal_Sys_SpareRegWrite;
RET_DATA T_Hal_Sys_ApsClkMmFactorSet  Hal_Sys_ApsClkMmFactorSet;
RET_DATA T_Hal_Sys_ClkChangeApply     Hal_Sys_ApsClkChangeApply;
RET_DATA T_Hal_Sys_ClkChangeApply     Hal_Sys_MsqClkChangeApply;
RET_DATA T_Hal_Sys_ClkChangeApply     Hal_Sys_PhyClkChangeApply;

/* For patch only (internal use)*/
RET_DATA T_Hal_Sys_SystemCoreClockUpdate  _Hal_Sys_ApsSystemCoreClockUpdate;
RET_DATA T_Hal_Sys_SystemCoreClockUpdate  _Hal_Sys_MsqSystemCoreClockUpdate;
RET_DATA T_Hal_Sys_SystemCoreClockUpdate  _Hal_Sys_PhySystemCoreClockUpdate;


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
*  Hal_Sys_SramDffBypass
*
* DESCRIPTION:
*   1. Enable SRAM FIFO bypass (Turn-off when use 150Mhz clock)
*   2. Related reg.: sys_reg 0x00C
* CALLS
*
* PARAMETERS
*   1. u8Enable: 1 for enable/ 0 for disable
*
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SramDffBypass_impl(uint8_t u8Enable)
{
    if(u8Enable)
    {
        SYS_REG->R_SRAM_BYPASS |= (SYS_SRAM_BYPASS_APS|SYS_SRAM_BYPASS_SHARE);
    }else{
        SYS_REG->R_SRAM_BYPASS &= ~(SYS_SRAM_BYPASS_APS|SYS_SRAM_BYPASS_SHARE);
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SwResetAll
*
* DESCRIPTION:
*   1. Reset whole system
*   2. Related reg.: sys_reg 0x010
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SwResetAll_impl(void)
{
    SYS_REG->R_SW_RESET_EN |= SYS_SW_RESET_ALL;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SwDebugEn
*
* DESCRIPTION:
*   1. Enable SWD debug function
*   2. Related reg.: sys_reg 0x014
* CALLS
*
* PARAMETERS
*   1. u8Enable: 1 for enable/ 0 for disable
*
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SwDebugEn_impl(uint8_t u8Enable)
{
    if(u8Enable)
        SYS_REG->R_SW_DBG_EN |= SYS_SW_DBG_EN;
    else
        SYS_REG->R_SW_DBG_EN &= ~SYS_SW_DBG_EN;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_StrapModeRead
*
* DESCRIPTION:
*   1. Get boot 4-bit strap mode
*   2. Related reg.: sys_reg 0x018
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   LBS 4 bits are strap mode
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_StrapModeRead_impl(void)
{
    return (SYS_REG->R_BOOT_STATUS & AOS_STRAP_MODE_MASK);
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SwDebugEn
*
* DESCRIPTION:
*   1. Over-wrtie strap mode by reg.
*   2. Related reg.: AOS 0x100
* CALLS
*
* PARAMETERS
*   1. u8Enable    : 1 for enable/ 0 for disable
*   2. u8StrapMode : strap mode
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_StrapModeOverWrite_impl(uint8_t u8Enable, uint8_t u8StrapMode)
{
    if(u8Enable)
    {
        if(u8StrapMode > AOS_STRAP_MODE_MAX)
            return 1;
        // Clear all field
        AOS->R_STRAP_MODE_CTL &= ~AOS_STRAP_MODE_MASK;
        // Set new value
        AOS->R_STRAP_MODE_CTL |= (AOS_STRAP_MODE_OVERWRITE_EN|u8StrapMode);
    }else{
        AOS->R_STRAP_MODE_CTL &= ~AOS_STRAP_MODE_OVERWRITE_EN;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_AosIdRead
*
* DESCRIPTION:
*   1. Get AOS ID
*   2. Related reg.: AOS 0x120
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   uint32_t for AOS ID
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_AosIdRead_impl(void)
{
    return AOS->RG_AOS_ID;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ChipIdRead
*
* DESCRIPTION:
*   1. [7:0]: HW ver. (ECO)
*   2. Related reg.: sys_reg 0x504[27:20]
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   uint32_t for HW ver.
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ChipIdRead_impl(void)
{
    return (SYS_REG->R_CM0_RMP_MASK[2] & SYS_ECO_VER_MASK)>>SYS_ECO_VER_SHIFT;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PowerManualEn
*
* DESCRIPTION:
*   1. Enable/disable power relative setting manual control
*   2. Related reg.: AOS 0x000
* CALLS
*
* PARAMETERS
*   1. u8Enable : 1 for enable/ 0 for disable
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PowerManualEn_impl(uint8_t u8Enable)
{
    if(u8Enable)
        AOS->RET_MUX = AOS_RET_FW_SETTING;
    else
        AOS->RET_MUX = AOS_RET_HW_SETTING;
	return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SleepInit
*
* DESCRIPTION:
*   1. Sleep mode initail
*   2. Related reg.: AOS 0x01C, 0x034~0x050, and 0x058 ~0x078
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_Sys_SleepInit_impl(void)
{
    // Need make rising pulse. So clean bit first
    AOS->MODE_CTL &= ~AOS_SLP_MODE_EN;

	/* SEQ Configuration from Joe
    // HW provide setting to make pulse
    AOS->PS_TIMER_PRESET = 0x3FF; // PS 0b' 11 1111 1111
    AOS->ON1_TIME        = 0xB420; // ON_1 0b' 0 | 1010 01 | 00 0010 0000
    AOS->ON2_TIME        = 0xBC40; // ON_2 0b' 0 | 1010 11 | 00 0100 0000
    AOS->ON3_TIME        = 0xFC60; // ON_3 0b' 0 | 1111 11 | 00 0110 0000
    AOS->ON4_TIME        = 0xFC80; // ON_4 0b' 0 | 1111 11 | 00 1000 0000
    AOS->ON5_TIME        = 0xFCA0; // ON_5 0b' 0 | 1111 11 | 00 1010 0000
    AOS->ON6_TIME        = 0xDCC0; // ON_6 0b' 0 | 1101 11 | 00 1100 0000
    AOS->ON7_TIME        = 0x4FFE; // ON_7 0b' 0 | 0010 11 | 11 1111 1110
    AOS->CPOR_N_ON_TIME  = 0x4FFE; // CPOR 0b' 0100 1111 1111 1110

    AOS->SPS_TIMER_PRESET = 0x0B0; // SPS 0b' 00 0000 1011 0000
    AOS->SON1_TIME        = 0x1DC20; // SON_1 0b' 1 | 1101 11 | 00 0010 0000
    AOS->SON2_TIME        = 0x1FC40; // SON_2 0b' 1 | 1111 11 | 00 0100 0000
    AOS->SON3_TIME        = 0x0FC45; // SON_3 0b' 0 | 1111 11 | 00 0100 0101
    AOS->SON4_TIME        = 0x0FC50; // SON_4 0b' 0 | 1111 11 | 00 0101 0000
    AOS->SON5_TIME        = 0x0BC60; // SON_5 0b' 0 | 1011 11 | 00 0110 0000
    AOS->SON6_TIME        = 0x0B480; // SON_6 0b' 0 | 1011 01 | 00 1000 0000
    AOS->SON7_TIME        = 0x0B0A0; // SON_7 0b' 0 | 1011 00 | 00 1010 0000
    AOS->SCPOR_N_ON_TIME  = 0x0040; //SCPOR 0b' 0000 0000 0100 0000
	*/

	// SEQ Configuration from Joe with Luke
    // HW provide setting to make pulse
    AOS->PS_TIMER_PRESET = 0x0005; // PS 0b' 11 1111 1111
    AOS->ON1_TIME        = 0xB401; // ON_1 0b' 0 | 1010 01 | 00 0010 0000
    AOS->ON2_TIME        = 0xBC02; // ON_2 0b' 0 | 1010 11 | 00 0100 0000
    AOS->ON3_TIME        = 0xFC03; // ON_3 0b' 0 | 1111 11 | 00 0110 0000
    AOS->ON4_TIME        = 0xFC04; // ON_4 0b' 0 | 1111 11 | 00 1000 0000
    AOS->ON5_TIME        = 0xFC04; // ON_5 0b' 0 | 1111 11 | 00 1010 0000
    AOS->ON6_TIME        = 0xFC04; // ON_6 0b' 0 | 1101 11 | 00 1100 0000
    AOS->ON7_TIME        = 0x4C05; // ON_7 0b' 0 | 0010 11 | 11 1111 1110
    AOS->CPOR_N_ON_TIME  = 0x0404; // CPOR 0b' 0100 1111 1111 1110

    AOS->SPS_TIMER_PRESET = 0x0005; // SPS 0b' 00 0000 1011 0000
    AOS->SON1_TIME        = 0x1DC01; // SON_1 0b' 1 | 1101 11 | 00 0010 0000
    AOS->SON2_TIME        = 0x1FC02; // SON_2 0b' 1 | 1111 11 | 00 0100 0000
    AOS->SON3_TIME        = 0xFC03; // SON_3 0b' 0 | 1111 11 | 00 0100 0101
    AOS->SON4_TIME        = 0xFC04; // SON_4 0b' 0 | 1111 11 | 00 0101 0000
    AOS->SON5_TIME        = 0xBC04; // SON_5 0b' 0 | 1011 11 | 00 0110 0000
    AOS->SON6_TIME        = 0xB404; // SON_6 0b' 0 | 1011 01 | 00 1000 0000
    AOS->SON7_TIME        = 0xB005; // SON_7 0b' 0 | 1011 00 | 00 1010 0000
    AOS->SCPOR_N_ON_TIME  = 0x0004; //SCPOR 0b' 0000 0000 0100 0000
}

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
uint64_t Hal_Sys_SleepTimerGet_impl(void)
{
    return ((uint64_t)(AOS->SLP_TIMER_CURR_H))<<32 | AOS->SLP_TIMER_CURR_L;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SleepTimerSet
*
* DESCRIPTION:
*   1. Setup a sleep event
*   2. Related reg.: AOS 0x004, 0x014 and 0x018
* CALLS
*
* PARAMETERS
*   1. eSleepMode    : Sleep mode select. refert to  E_SleepModeIdx_t
*   2. u64WakeupTime : 37-bits of timer, Max: 0x1F FFFFFFFF
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SleepTimerSet_impl(E_SleepModeIdx_t eSleepMode, uint64_t u64WakeupTime)
{
    switch(eSleepMode)
    {
        case SLEEP_TIMER:
            AOS->MODE_CTL |= AOS_SLP_MODE_TIMER;
            break;
        case SLEEP_DEEP:
            AOS->MODE_CTL |= AOS_SLP_MODE_DEEP;
            break;
        default:
            return 1;
    }
    //Always need early wake (wake up by interrupt)
    AOS->MODE_CTL |= AOS_SLP_MODE_EARLY_WAKE_UP;

    if(u64WakeupTime > AOS_SLP_TIMER_MASK_MAX)
        return 1;
    AOS->SLP_TIMER_PRESET_L = u64WakeupTime & AOS_SLP_TIMER_MASK_L;
    AOS->SLP_TIMER_PRESET_H = (u64WakeupTime>>32) & AOS_SLP_TIMER_MASK_H;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SleepTrig
*
* DESCRIPTION:
*   1. Sleep mode trigger
*   2. Related reg.: AOS 0x004
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_Sys_SleepTrig_impl(void)
{
    AOS->MODE_CTL |= AOS_SLP_MODE_EN;
}

/*************************************************************************
* FUNCTION:
*  Hal_SysPinMuxBootROMInit
*
* DESCRIPTION:
*   1. Pin-Mux initial for BootROM
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
void Hal_SysPinMuxBootROMInit_impl(void)
{
/*
    AOS->RG_PD_IE;           // 0x090
    AOS->RG_PD_PE;           // 0x094
    AOS->RG_PD_O_INV;        // 0x098
    AOS->RG_PD_DS;           // 0x09C
    AOS->RG_GPO;             // 0x0A0
    AOS->RG_PD_I_INV;        // 0x0A4
    AOS->RG_PDOV_MODE;       // 0x0A8
    AOS->RG_PD_DIR;          // 0x0AC
    AOS->RG_PD_OENP_INV;     // 0x0B0
    AOS->RG_PDOC_MODE;       // 0x0B4
    AOS->RG_GPI;             // 0x0B8
    AOS->reserve_0bc;        // 0x0BC, reserved
    AOS->RG_PDI_SRC_IO_A;    // 0x0C0
    AOS->RG_PDI_SRC_IO_B;    // 0x0C4
    AOS->RG_PDI_SRC_IO_C;    // 0x0C8
    AOS->RG_PDI_SRC_IO_D;    // 0x0CC
    AOS->RG_PTS_INMUX_A;     // 0x0D0
    AOS->RG_PTS_INMUX_B;     // 0x0D4
    AOS->RG_PTS_INMUX_C;     // 0x0D8
    AOS->RG_PTS_INMUX_D;     // 0x0DC
*/
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
    tmp &= ~((0x1 << 1) | (0x1 << 0));
    AOS->RG_PD_PE = tmp;

    // function pin
    tmp = AOS->RG_PDOC_MODE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOC_MODE = tmp;

    tmp = AOS->RG_PDOV_MODE;
    tmp |= ((0x1 << 1) | (0x1 << 0));
    AOS->RG_PDOV_MODE = tmp;

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
void Hal_SysPinMuxAppInit_impl(void)
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
    tmp &= ~((0x1 << 1) | (0x1 << 0));
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
    tmp &= ~((0x1 << 9) | (0x1 << 8) | (0x1 << 7) | (0x1 << 6));
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
    tmp &= ~((0x1 << 9) | (0x1 << 8) | (0x1 << 3) | (0x1 << 2));
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
    tmp &= ~((0x1 << 7) | (0x1 << 6) | (0x1 << 5) | (0x1 << 4));
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
    tmp &= ~((0x1 << 17) | (0x1 << 16));
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
uint32_t Hal_Sys_ApsModuleRst_impl(E_ApsRstModule_t eModule)
{
    switch(eModule)
    {
        case ASP_RST_RFM:
			AOS->R_APS_SWRST |= AOS_APS_RST_RFM;
			break;
        case ASP_RST_I2C:
			AOS->R_APS_SWRST |= AOS_APS_RST_I2C;
			break;
        case ASP_RST_PWM:
			AOS->R_APS_SWRST |= AOS_APS_RST_PWM;
			break;
        case ASP_RST_TIMER_0:
			AOS->R_APS_SWRST |= AOS_APS_RST_TMR_0;
			break;
        case ASP_RST_TIMER_1:
			AOS->R_APS_SWRST |= AOS_APS_RST_TMR_1;
			break;
        case ASP_RST_WDT:
			AOS->R_APS_SWRST |= AOS_APS_RST_WDT;
			break;
        case ASP_RST_VIC:
			AOS->R_APS_SWRST |= AOS_APS_RST_VIC;
			break;
        case ASP_RST_SYS_BUS:
			AOS->R_APS_SWRST |= AOS_APS_RST_SYS_BUS;
			break;
        case ASP_RST_SPI_0:
			AOS->R_APS_SWRST |= AOS_APS_RST_SPI_0;
			break;
        case ASP_RST_SPI_1:
			AOS->R_APS_SWRST |= AOS_APS_RST_SPI_1;
			break;
        case ASP_RST_SPI_2:
			AOS->R_APS_SWRST |= AOS_APS_RST_SPI_2;
			break;
        case ASP_RST_UART_0:
			AOS->R_APS_SWRST |= AOS_APS_RST_UART_0;
			break;
        case ASP_RST_UART_1:
			AOS->R_APS_SWRST |= AOS_APS_RST_UART_1;
			break;
        case ASP_RST_DBG_UART:
			AOS->R_APS_SWRST |= AOS_APS_RST_DBG_UART;
			break;
        case ASP_RST_OTP:
			AOS->R_APS_SWRST |= AOS_APS_RST_OTP;
			break;
        case ASP_RST_DMA:
			AOS->R_APS_SWRST |= AOS_APS_RST_DMA;
			break;
        case ASP_RST_ROM:
			AOS->R_APS_SWRST |= AOS_APS_RST_ROM;
			break;
        case ASP_RST_RAM:
			AOS->R_APS_SWRST |= AOS_APS_RST_RAM;
			break;
        case ASP_RST_JTAG:
			AOS->R_APS_SWRST |= AOS_APS_RST_JTAG;
			break;
        case ASP_RST_AHB_TO_APB_BUS:
			AOS->R_APS_SWRST |= AOS_APS_RST_AHB_TO_APB;
			break;
        case ASP_RST_BRIDGE_APS_2_MSQ:
			AOS->R_APS_SWRST |= AOS_APS_RST_BRIDGE_APS;
			break;
        case ASP_RST_BRIDGE_MSQ_2_APS:
			AOS->R_APS_SWRST |= AOS_APS_RST_BRIDGE_MSQ;
			break;
        case ASP_RST_CORE_M3:
			AOS->R_APS_SWRST |= AOS_APS_RST_CM3;
			break;
        case ASP_RST_SCRT:
			AOS->R_APS_SWRST |= AOS_APS_RST_SCRT;
			break;
        case ASP_RST_TAP:
			AOS->R_APS_SWRST |= AOS_APS_RST_TAP;
			break;
		default:
			return 1;
    }
	return 0;
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
uint32_t Hal_Sys_MsqModuleRst_impl(E_MsqRstModule_t eModule)
{
    switch(eModule)
    {
        case MSQ_RST_ROM:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_ROM;
			break;
        case MSQ_RST_SDM_XTAL:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_SDM_XTAL;
			break;
        case MSQ_RST_BRIDGE_MSQ_TO_APS:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_BRIDGE_APS;
			break;
        case MSQ_RST_BRIDGE_APS_TO_MSQ:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_BRIDGE_MSQ;
			break;
        case MSQ_RST_MAC:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_MAC;
			break;
        case MSQ_RST_PHY:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_PHY;
			break;
        case MSQ_RST_WDT:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_WDT;
			break;
        case MSQ_RST_VIC:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_VIC;
			break;
        case MSQ_RST_PHY_REG:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_PHY_REG;
			break;
        case MSQ_RST_AOS:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_AOS;
			break;
        case MSQ_RST_CORE_M0:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_CM0;
			break;
        case MSQ_RST_RFPHY:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_RFPHY;
			break;
        case MSQ_RST_SPI:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_SPI;
			break;
        case MSQ_RST_DBG_UART:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_DBG_UART;
			break;
        case MSQ_RST_PROG_PU:
			AOS->R_MSQ_SWRST |= AOS_MSQ_RST_PROG_PU;
			break;
		default:
			return 1;
    }
	return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_RetRamStatusRead
*
* DESCRIPTION:
*   1. Get the status of each RetRam block. 1 for power-on, and 0 for power-off.
*      Ref. to E_RetRamIdx_t
*   2. Related reg.: AOS 0x118
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   SRAM status, each bit for the single SRAM block.
*   Ref. to E_RetRamIdx_t
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_RetRamStatusRead_impl(void)
{
    return AOS->RG_SRAM_RET_ACK;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_RetRamTurnOff
*
* DESCRIPTION:
*   1. Turn off the given RetRam blocks. Ref. to E_RetRamIdx_t
*   2. Related reg.: AOS 0x0E0 ,0x0E4 and 0x118
* CALLS
*
* PARAMETERS
*   1. u32RetRamIdxs : Multi index combined. index refer to E_RetRamIdx_t
* RETURNS
*   0: setting complete
*   1: error 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Sys_RetRamTurnOff_impl(uint32_t u32RetRamIdxs )
{
    // Check Indexes are all in legal region
    if( u32RetRamIdxs & (~AOS_RET_RAM_MASK) )
        return 1;

    // Isolated
    AOS->RG_SRAM_IOS_EN |= u32RetRamIdxs;

    // Chech status
    while( Hal_Sys_RetRamStatusRead() & u32RetRamIdxs != 0);

    // Power off
    AOS->RG_SRAM_RET_OFF |= u32RetRamIdxs;

    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_RetRamTurnOn
*
* DESCRIPTION:
*   1. Turn on the given RetRam blocks. Ref. to E_RetRamIdx_t
*   2. Related reg.: AOS 0x0E0 ,0x0E4 and 0x118
* CALLS
*
* PARAMETERS
*   1. u32RetRamIdxs : Multi index combined. index refer to E_RetRamIdx_t
* RETURNS
*   0: setting complete
*   1: error 
* GLOBALS AFFECTED
* 
*************************************************************************/
uint32_t Hal_Sys_RetRamTurnOn_impl(uint32_t u32RetRamIdxs)
{
    // Check Indexes are all in legal region
    if( u32RetRamIdxs & (~AOS_RET_RAM_MASK) )
        return 1;

    // Power on
    AOS->RG_SRAM_RET_OFF &= ~u32RetRamIdxs;

    // un-Isolated
    AOS->RG_SRAM_IOS_EN &= ~u32RetRamIdxs;

    // Chech status
    while( Hal_Sys_RetRamStatusRead() & u32RetRamIdxs != u32RetRamIdxs);

    return 0;
}

// TODO: XTAL CAL

/*************************************************************************
* FUNCTION:
*  Hal_Sys_MsqRomReampAll
*
* DESCRIPTION:
*   1. Enable/disable remap all MSQ ROM
*   2. Related reg.: AOS 0x11C
* CALLS
*
* PARAMETERS
*   1. u8Enable : 1 for enable/ 0 for disable
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_MsqRomReampAll_impl(uint8_t u8Enable)
{
    if(u8Enable)
        AOS->RG_MSQ_ROM_MAP |= AOS_MSQ_ROM_REMAP_ALL_EN;
    else
        AOS->RG_MSQ_ROM_MAP &= ~AOS_MSQ_ROM_REMAP_ALL_EN;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ResetStatusRead
*
* DESCRIPTION:
*   1. Read the source of previous reset event. Refer to E_RstSrc_t
*   2. Related reg.: AOS 0x128
* CALLS
*
* PARAMETERS
*   None
* RETURNS
*   Reset sources, each bit for the reset sources.
*   Ref. to E_RstSrc_t
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ResetStatusRead(void)
{
    return AOS->RG_RSTS;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ResetStatusClear
*
* DESCRIPTION:
*   1. Clear the source of previous reset event. Refer to E_RstSrc_t
*   2. Related reg.: AOS 0x128
* CALLS
*
* PARAMETERS
*   1. u32Sources : each bit for the reset sources. Refer to E_RstSrc_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ResetStatusClear(uint32_t u32Sources)
{
    AOS->RG_RSTS = u32Sources;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  _Hal_Sys_ApsSystemCoreClockUpdate
*
* DESCRIPTION:
*   1. Updated corresponding clock golbal values. Internal function
* CALLS
*
* PARAMETERS
*   1. u32CoreClk : update clock speed
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void _Hal_Sys_ApsSystemCoreClockUpdate_impl(uint32_t u32CoreClk)
{
    SystemCoreClockSet(u32CoreClk);
}

/*************************************************************************
* FUNCTION:
*  _Hal_Sys_MsqSystemCoreClockUpdate
*
* DESCRIPTION:
*   1. Updated corresponding clock golbal values. Internal function
* CALLS
*
* PARAMETERS
*   1. u32CoreClk : update clock speed
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void _Hal_Sys_MsqSystemCoreClockUpdate_impl(uint32_t u32CoreClk)
{
    // Do nothing
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*  _Hal_Sys_PhySystemCoreClockUpdate
*
* DESCRIPTION:
*   1. Updated corresponding clock golbal values. Internal function
* CALLS
*
* PARAMETERS
*   1. u32CoreClk : update clock speed
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void _Hal_Sys_PhySystemCoreClockUpdate_impl(uint32_t u32CoreClk)
{
    // Do nothing
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ApsClkTreeSetup
*
* DESCRIPTION:
*   1. APS(M3) clock-tree setup. It contain a source select and two divider in clock-tree
*       [SRC]---CLK DIV2---+---PCLK DIV2--->PCLK
*                          +--->Other CLKs
*   2. Related reg.: AOS 0x134 and 0x138
* CALLS
*
* PARAMETERS
*   1. eClkTreeSrc : Select the source of clock tree. Refer to E_ApsClkTreeSrc_t
*   2. u8ClkDivEn  : 1 for Enable/0 for Disable the CLK divider
*   3. u8PclkDivEn : 1 for Enable/0 for Disable the PCLK divider
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ApsClkTreeSetup_impl(E_ApsClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn, uint8_t u8PclkDivEn )
{
/* Two level switch: 176M-switch and src-switch. 176M-switch output to src-switch
 *  @176M-switch: [31][29]
 *      0b'00:CLK_XTAL_X4, 0b'01:CLK_DECI, 0b'10:CLK_1P2G, 0b'11: CLK_EXT
 *  @src-switch: [1:0]
 *        0b'00:CLK_RC, 0b'01:CLK_XTAL, 0b'10:CLK_XTAL_X2, 0b'11: CLK_FROM_176M_SWTICH
 */
    uint32_t u32Temp = 0;
    uint32_t u32CoreClk = 0;
    u32Temp = AOS->R_M3CLK_SEL;
    u32Temp &= ~(AOS_APS_CLK_SRC_MASK | AOS_APS_CLK_176M_SRC_MASK);
    switch(eClkTreeSrc)
    {
        case ASP_CLKTREE_SRC_RC_BB:
            u32Temp |= AOS_APS_CLK_SRC_RC;
            AOS->R_M3CLK_SEL = u32Temp;

            u32CoreClk = CLK_RC;
            break;
        case ASP_CLKTREE_SRC_XTAL:
            u32Temp |= AOS_APS_CLK_SRC_XTAL;
            AOS->R_M3CLK_SEL = u32Temp;

            u32CoreClk = CLK_XTAL;
            break;
        case ASP_CLKTREE_SRC_XTAL_X2:
            u32Temp |= AOS_APS_CLK_SRC_XTAL_X2;
            AOS->R_M3CLK_SEL = u32Temp;

            u32CoreClk = CLK_XTAL*2;
            break;
        case ASP_CLKTREE_SRC_XTAL_X4:
            AOS->R_M3CLK_SEL = u32Temp; // Switch source to RC
            u32Temp |= AOS_APS_CLK_176M_SRC_XTAL_X4;
            AOS->R_M3CLK_SEL = u32Temp; // Select 176M-switch to XTAL_X4
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp; // switch source to 176M-switch

            u32CoreClk = CLK_XTAL*4;
            break;
        case ASP_CLKTREE_SRC_DECI:
            AOS->R_M3CLK_SEL = u32Temp; // Switch source to RC
            u32Temp |= AOS_APS_CLK_176M_SRC_DECI;
            AOS->R_M3CLK_SEL = u32Temp; // Select 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp; // switch source to 176M-switch

            u32CoreClk = CLK_RF;
            break;
        case ASP_CLKTREE_SRC_1P2G_DIV:
            AOS->R_M3CLK_SEL = u32Temp; // Switch source to RC
            u32Temp |= AOS_APS_CLK_176M_SRC_1P2G;
            AOS->R_M3CLK_SEL = u32Temp; // Select 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp; // switch source to 176M-switch

            u32CoreClk = CLK_RF;
            break;
        case ASP_CLKTREE_SRC_EXTERNAL:
            AOS->R_M3CLK_SEL = u32Temp; // Switch source to RC
            u32Temp |= AOS_APS_CLK_176M_SRC_EXT;
            AOS->R_M3CLK_SEL = u32Temp; // Select 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp; // switch source to 176M-switch

            u32CoreClk = CLK_RF;
            break;
        default:
            return 1;
    }

    if(u8ClkDivEn)
    {
        AOS->R_M0CLK_SEL |= AOS_APS_CLK_DIV2_UNGATED;
        AOS->R_M3CLK_SEL |= AOS_APS_CLK_DIV2;

        u32CoreClk = u32CoreClk/2;
    }else{
        AOS->R_M3CLK_SEL &= ~AOS_APS_CLK_DIV2;
        AOS->R_M0CLK_SEL &= ~AOS_APS_CLK_DIV2_UNGATED;
    }

    // This only effect pclk
    if(u8PclkDivEn)
    {
        AOS->R_M0CLK_SEL |= AOS_APS_PCLK_DIV2_UNGATED;
        AOS->R_M3CLK_SEL |= AOS_APS_PCLK_DIV2;
    }else{
        AOS->R_M3CLK_SEL &= ~AOS_APS_PCLK_DIV2;
        AOS->R_M0CLK_SEL &= ~AOS_APS_PCLK_DIV2_UNGATED;
    }

    // Update system clock.
    _Hal_Sys_ApsSystemCoreClockUpdate(u32CoreClk);

    // Apply to all relative modules
    Hal_Sys_ApsClkChangeApply();

    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_MsqClkTreeSetup
*
* DESCRIPTION:
*   1. MSQ(M0) clock-tree setup. It contain a source select and one divider in clock-tree
*       [SRC]---CLK DIV2---> all CLKs
*   2. Related reg.:  0x138
* CALLS
*
* PARAMETERS
*   1. eClkTreeSrc : Select the source of clock tree. Refer to E_MsqClkTreeSrc_t
*   2. u8ClkDivEn  : 1 for Enable/0 for Disable the CLK divider
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_MsqClkTreeSetup_impl(E_MsqClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn )
{
    uint32_t u32Temp = 0;
    uint32_t u32CoreClk = 0;
    u32Temp = AOS->R_M0CLK_SEL;

    u32Temp &= ~AOS_MSQ_CLK_SRC_MASK;
    switch(eClkTreeSrc)
    {
        case MSQ_CLKTREE_SRC_RC:
            u32Temp |= AOS_MSQ_SRC_RC;
            u32CoreClk = CLK_RC;
            break;
        case MSQ_CLKTREE_SRC_XTAL:
            u32Temp |= AOS_MSQ_SRC_XTAL;
            u32CoreClk = CLK_XTAL;
            break;
        case MSQ_CLKTREE_SRC_XTAL_X2:
            u32Temp |= AOS_MSQ_SRC_XTAL_X2;
            u32CoreClk = CLK_XTAL*2;
            break;
        case MSQ_CLKTREE_SRC_EXTERNAL:
            u32Temp |= AOS_MSQ_SRC_EXT;
            u32CoreClk = CLK_RF;
            break;
        default:
            return 1;
    }

    if(u8ClkDivEn)
    {
        u32Temp |= AOS_MSQ_CLK_DIV2_UNGATED;
        u32Temp |= AOS_MSQ_CLK_DIV2;
    }else{
        u32Temp &= ~AOS_MSQ_CLK_DIV2;
        u32Temp &= ~AOS_MSQ_CLK_DIV2_UNGATED;
    }

    AOS->R_M0CLK_SEL = u32Temp;

    // Update system clock.
    _Hal_Sys_MsqSystemCoreClockUpdate(u32CoreClk);

    // Apply to all relative modules
    Hal_Sys_MsqClkChangeApply();

    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PhyClkTreeSetup
*
* DESCRIPTION:
*   1. PHY clock-tree setup. It contain a source select and one divider in clock-tree
*       Mainly 3 domains:  S-domain (from XTAL_X2), Q-domain(from DECI) and RFMMD
*   2. Related reg.:  0x13C
* CALLS
*
* PARAMETERS
*   1. eRfTestingSrc: Select the source of RF testing. Refer to E_PhyRfTestingSrc_t
*   2. eRfCapSrc    : Select the source of RF Cap data2FIFO. Refer to E_PhyRfCapSrc_t
*   3. u8RfClkSDivEn: RF clock s-domain div by two. 1 for Enable/0 for Disable.
*   4. u8RfClkSInv  : S-domain clock invert. 1 for Enable/0 for Disable.
*   5. u8RfClkQInv  : Q-domain clock invert. 1 for Enable/0 for Disable.
*   6. u8RfClkQInv  : RFMMD clock invert. 1 for Enable/0 for Disable.
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PhyClkTreeSetup_impl(E_PhyRfTestingSrc_t eRfTestingSrc, E_PhyRfCapSrc_t eRfCapSrc, uint8_t u8RfClkSDivEn, uint8_t u8RfClkSInv, uint8_t u8RfClkQInv, uint8_t u8RfmmdClkInv)
{
    uint32_t u32Temp = 0;
    uint32_t u32CoreClk = 0;
    u32Temp = AOS->R_RFPHY_SEL;

    u32Temp &= ~AOS_PHY_RF_TEST_SRC_MASK;
    switch(eRfTestingSrc)
    {
        case PHY_TESTING_SRC_S_022M:
            u32Temp |= AOS_PHY_RF_TEST_SRC_S_22M;
            break;
        case PHY_TESTING_SRC_S_044M:
            u32Temp |= AOS_PHY_RF_TEST_SRC_S_44M;
            break;
        case PHY_TESTING_SRC_Q_044M:
            u32Temp |= AOS_PHY_RF_TEST_SRC_Q_44M;
            break;
        case PHY_TESTING_SRC_RFMMD:
            u32Temp |= AOS_PHY_RF_TEST_SRC_RFMMD;
            break;
        default:
            return 1;
    }
    u32Temp &= ~AOS_PHY_RF_CAP_SRC_MASK;
	switch(eRfCapSrc)
    {
        case PHY_CAP_SRC_S_044M:
            u32Temp |= AOS_PHY_RF_CAP_SRC_S_044M;
            u32CoreClk = 440000;
            break;
        case PHY_CAP_SRC_Q_176M:
            u32Temp |= AOS_PHY_RF_CAP_SRC_Q_176M;
            u32CoreClk = 1760000;
            break;
        default:
            return 1;
    }

    if(u8RfClkSDivEn)
    {
        u32Temp |= AOS_PHY_CLK_S_DIV2_UNGATED;
        u32Temp |= AOS_PHY_CLK_S_DIV2;
    }else{
        u32Temp &= ~AOS_PHY_CLK_S_DIV2;
        u32Temp &= ~AOS_PHY_CLK_S_DIV2_UNGATED;
    }

	if(u8RfClkSInv)
    {
        u32Temp |= AOS_PHY_CLK_S_INV;
    }else{
        u32Temp &= ~AOS_PHY_CLK_S_INV;
    }

	if(u8RfClkQInv)
    {
        u32Temp |= AOS_PHY_CLK_Q_INV;
    }else{
        u32Temp &= ~AOS_PHY_CLK_Q_INV;
    }

	if(u8RfmmdClkInv)
    {
        u32Temp |= AOS_PHY_CLK_RFMMD_INV;
    }else{
        u32Temp &= ~AOS_PHY_CLK_RFMMD_INV;
    }

    AOS->R_RFPHY_SEL = u32Temp;

    // Update system clock.
    _Hal_Sys_PhySystemCoreClockUpdate(u32CoreClk);

    // Apply to all relative modules
    Hal_Sys_PhyClkChangeApply();

    return 0;
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
uint32_t Hal_Sys_PwmSrcSelect_impl(E_PwmClkSrc_t eSrc)
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
*  Hal_Sys_ApsClkEn
*
* DESCRIPTION:
*   1. APS(M3) module clock enable/disable.
*       [CLK Tree] -->GLOBAL Enable--->Other modules enable
*   2. Related reg.: AOS 0x134
* CALLS
*
* PARAMETERS
*   1. u8Enable : 1 for Enable/0 for Disable the asigned module
*   2. eModule  : The single module refer to E_ApsClkEnModule_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ApsClkEn_impl(uint8_t u8Enable, E_ApsClkEnModule_t eModule)
{
    uint32_t u32Mask = 0;
    switch(eModule)
    {
        /* root */
        case APS_CLK_GLOBAL:
            u32Mask = AOS_APS_CLK_EN_GLOBAL;
            break;

        /* pclk domain */
        case APS_CLK_I2C:
            u32Mask = AOS_APS_CLK_EN_I2C_PCLK;
            break;
        case APS_CLK_TIMER_0:
            u32Mask = AOS_APS_CLK_EN_TMR_0_PCLK;
            break;
        case APS_CLK_TIMER_1:
            u32Mask = AOS_APS_CLK_EN_TMR_1_PCLK;
            break;
        case APS_CLK_WDT:
            u32Mask = AOS_APS_CLK_EN_WDT_PCLK |    // WDT to bus
                      AOS_APS_CLK_EN_WDT_INTERNAL; // WDT internal
            break;
        case APS_CLK_VIC:
            u32Mask = AOS_APS_CLK_EN_VIC_PCLK;
            break;
        case APS_CLK_SPI_0:
            u32Mask = AOS_APS_CLK_EN_SPI_0_PCLK;
            break;
        case APS_CLK_SPI_1:
            u32Mask = AOS_APS_CLK_EN_SPI_1_PCLK;
            break;
        case APS_CLK_SPI_2:
            u32Mask = AOS_APS_CLK_EN_SPI_2_PCLK;
            break;
        case APS_CLK_UART_0:
            u32Mask = AOS_APS_CLK_EN_UART_0_PCLK;
            break;
        case APS_CLK_UART_1:
            u32Mask = AOS_APS_CLK_EN_UART_1_PCLK;
            break;
        case APS_CLK_DBG_UART:
            u32Mask = AOS_APS_CLK_EN_DBG_UART_PCLK;
            break;
        case APS_CLK_OTP:
            u32Mask = AOS_APS_CLK_EN_OTP_PCLK;
            break;

        /* hclk domain */
        case APS_CLK_DMA:
            u32Mask = AOS_APS_CLK_EN_DMA_HCLK;
            break;
        case APS_CLK_ROM:
            u32Mask = AOS_APS_CLK_EN_ROM_HCLK;
            break;
        case APS_CLK_RAM:
            u32Mask = AOS_APS_CLK_EN_RAM_HCLK;
            break;
        case APS_CLK_AHB_TO_APB_BUS:
            u32Mask = AOS_APS_CLK_EN_APB_HCLK;
            break;
        case APS_CLK_APS2MSQ_BRIDGE:
            u32Mask = AOS_APS_CLK_EN_APS_HCLK |
                      AOS_APS_CLK_EN_MSQ_HCLK;
            break;
        case APS_CLK_SCRT:
            u32Mask = AOS_APS_CLK_EN_SCRT_HCLK;
            break;
        case APS_CLK_JTAG:
            u32Mask = AOS_APS_CLK_EN_JTAG_HCLK;
            break;

        /* fclk + hclk domain*/
        case APS_CLK_CM3_CORE:
            u32Mask = AOS_APS_CLK_EN_FCLK |    // CPU
                      AOS_APS_CLK_EN_CM3_HCLK; // CM3 internal bus
            break;

        /* stand-alone clock  */
        case APS_CLK_PWM:
            u32Mask = AOS_APS_CLK_EN_PWM_CLK;
            break;

        /* all */
        case APS_CLK_ALL:
            u32Mask = AOS_APS_CLK_EN_GLOBAL |
                      AOS_APS_CLK_EN_I2C_PCLK |
                      AOS_APS_CLK_EN_TMR_0_PCLK |
                      AOS_APS_CLK_EN_TMR_1_PCLK |
                      AOS_APS_CLK_EN_WDT_PCLK |
                      AOS_APS_CLK_EN_VIC_PCLK |
                      AOS_APS_CLK_EN_SPI_0_PCLK |
                      AOS_APS_CLK_EN_SPI_1_PCLK |
                      AOS_APS_CLK_EN_SPI_2_PCLK |
                      AOS_APS_CLK_EN_UART_0_PCLK |
                      AOS_APS_CLK_EN_UART_1_PCLK |
                      AOS_APS_CLK_EN_DBG_UART_PCLK |
                      AOS_APS_CLK_EN_OTP_PCLK |
                      AOS_APS_CLK_EN_CM3_HCLK |
                      AOS_APS_CLK_EN_DMA_HCLK |
                      AOS_APS_CLK_EN_ROM_HCLK |
                      AOS_APS_CLK_EN_RAM_HCLK |
                      AOS_APS_CLK_EN_APB_HCLK |
                      AOS_APS_CLK_EN_APS_HCLK |
                      AOS_APS_CLK_EN_MSQ_HCLK |
                      AOS_APS_CLK_EN_SCRT_HCLK |
                      AOS_APS_CLK_EN_FCLK |
                      AOS_APS_CLK_EN_PWM_CLK |
                      AOS_APS_CLK_EN_JTAG_HCLK |
                      AOS_APS_CLK_EN_WDT_INTERNAL;
        default:
            return 1;
    }
    if(u8Enable)
        AOS->R_M3CLK_SEL |= u32Mask;
    else
        AOS->R_M3CLK_SEL &= ~u32Mask;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_MsqClkEn
*
* DESCRIPTION:
*   1. MSQ(M0) module clock enable/disable.
*       [CLK Tree] -->GLOBAL Enable--->Other modules enable
*   2. Related reg.: AOS 0x138
* CALLS
*
* PARAMETERS
*   1. u8Enable : 1 for Enable/0 for Disable the asigned module
*   2. eModule  : The single module refer to E_MsqClkEnModule_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_MsqClkEn_impl(uint8_t u8Enable, E_MsqClkEnModule_t eModule)
{
    uint32_t u32Mask = 0;
    switch(eModule)
    {
        /* root */
        case MSQ_CLK_GLOBAL:
            u32Mask |= AOS_MSQ_CLK_EN_GLOBAL;
            break;

        /* pclk domain */
        case MSQ_CLK_SPI:
            u32Mask |= AOS_MSQ_CLK_EN_SPI;
            break;
        case MSQ_CLK_DBG_UART:
            u32Mask |= AOS_MSQ_CLK_EN_UART;
            break;

        /* hclk domain */
        case MSQ_CLK_ROM:
            u32Mask |= AOS_MSQ_CLK_EN_ROM_HCLK;
            break;
        case MSQ_CLK_RAM:
            u32Mask |= AOS_MSQ_CLK_EN_RAM_HCLK;
            break;
        case MSQ_CLK_MSQ2APS_BRIDGE:
            u32Mask |= AOS_MSQ_CLK_EN_MSQ_HCLK |
                       AOS_MSQ_CLK_EN_APS_HCLK;
            break;
        case MSQ_CLK_MAC:
            u32Mask |= AOS_MSQ_CLK_EN_MAC_HCLK;
            break;
        case MSQ_CLK_PHY:
            u32Mask |= AOS_MSQ_CLK_EN_PHY_HCLK |
                       AOS_MSQ_CLK_EN_PHY_REG_HCLK;
            break;
        case MSQ_CLK_AOS:
            u32Mask |= AOS_MSQ_CLK_EN_AOS_HCLK;
            break;
        case MSQ_CLK_WDT:
            u32Mask |= AOS_MSQ_CLK_EN_WDT_HCLK |
                       AOS_MSQ_CLK_EN_WDT;
            break;
        case MSQ_CLK_VIC:
            u32Mask |= AOS_MSQ_CLK_EN_VIC_HCLK;
            break;
        case MSQ_CLK_PU:
            u32Mask |= AOS_MSQ_CLK_EN_PU_HCLK;
            break;

        /* fclk + dclk + sclk + hclk domain*/
        case MSQ_CLK_CM0_CORE:
            u32Mask |= AOS_MSQ_CLK_EN_FCLK |
                       AOS_MSQ_CLK_EN_DCLK |
                       AOS_MSQ_CLK_EN_SCLK |
                       AOS_MSQ_CLK_EN_CM0_HCLK;

        /* all */
        case MSQ_CLK_ALL:
            u32Mask |= AOS_MSQ_CLK_EN_GLOBAL |
                       AOS_MSQ_CLK_EN_ROM_HCLK |
                       AOS_MSQ_CLK_EN_RAM_HCLK |
                       AOS_MSQ_CLK_EN_MSQ_HCLK |
                       AOS_MSQ_CLK_EN_APS_HCLK |
                       AOS_MSQ_CLK_EN_MAC_HCLK |
                       AOS_MSQ_CLK_EN_PHY_HCLK |
                       AOS_MSQ_CLK_EN_PHY_REG_HCLK |
                       AOS_MSQ_CLK_EN_AOS_HCLK |
                       AOS_MSQ_CLK_EN_WDT_HCLK |
                       AOS_MSQ_CLK_EN_VIC_HCLK |
                       AOS_MSQ_CLK_EN_FCLK |
                       AOS_MSQ_CLK_EN_DCLK |
                       AOS_MSQ_CLK_EN_SCLK |
                       AOS_MSQ_CLK_EN_PU_HCLK |
                       AOS_MSQ_CLK_EN_CM0_HCLK |
                       AOS_MSQ_CLK_EN_WDT |
                       AOS_MSQ_CLK_EN_SPI |
                       AOS_MSQ_CLK_EN_UART;
        default:
            return 1;
    }
    if(u8Enable)
        AOS->R_M0CLK_SEL |= u32Mask;
    else
        AOS->R_M0CLK_SEL &= ~u32Mask;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PhyClkEn
*
* DESCRIPTION:
*   1. PHY module clock enable/disable.
*       [XTAL_X2] -->CLK_S Enable--->S_22M and S_44M...
*       [DECI]    -->CLK_Q Enable--->Q_176M, Q_088M and Q_044M ...
*       [RFMMD Tree] -->RFMMD Enable---> RF_MMD...
*   2. Related reg.: AOS 0x138
* CALLS
*
* PARAMETERS
*   1. u8Enable : 1 for Enable/0 for Disable the asigned module
*   2. eModule  : The single module refer to E_PhyClkEnModule_t
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_PhyClkEn_impl(uint8_t u8Enable, E_PhyClkEnModule_t eModule)
{
    uint32_t u32Mask = 0;
    switch(eModule)
    {
        /* roots */
        case PHY_CLK_S:
            u32Mask |= AOS_PHY_CLK_S_EN;
            break;
        case PHY_CLK_Q:
            u32Mask |= AOS_PHY_CLK_Q_EN;
            break;
        case PHY_CLK_RFMMD:
            u32Mask |= AOS_PHY_CLK_RFMMD_EN;
            break;

        /* modules */
        case PHY_CLK_S_22M_TX:
            u32Mask |= AOS_PHY_CLK_S_22M_TX_EN;
            break;
        case PHY_CLK_S_22M_RX:
            u32Mask |= AOS_PHY_CLK_S_22M_RX_EN;
            break;
        case PHY_CLK_S_44M_TX:
            u32Mask |= AOS_PHY_CLK_S_44M_TX_EN;
            break;
        case PHY_CLK_S_44M_RX:
            u32Mask |= AOS_PHY_CLK_S_44M_RX_EN;
            break;
        case PHY_CLK_Q_176M_TX:
            u32Mask |= AOS_PHY_CLK_Q_176M_TX_EN;
            break;
        case PHY_CLK_Q_176M_RX:
            u32Mask |= AOS_PHY_CLK_Q_176M_RX_EN;
            break;
        case PHY_CLK_Q_088M_TX:
            u32Mask |= AOS_PHY_CLK_Q_088M_RX_EN;
            break;
        case PHY_CLK_Q_088M_RX:
            u32Mask |= AOS_PHY_CLK_Q_088M_TX_EN;
            break;
        case PHY_CLK_Q_044M_TX:
            u32Mask |= AOS_PHY_CLK_Q_044M_RX_EN;
            break;
        case PHY_CLK_Q_044M_RX:
            u32Mask |= AOS_PHY_CLK_Q_044M_TX_EN;
            break;
        case PHY_CLK_RF_CAP:
            u32Mask |= AOS_PHY_CLK_CAP_FIFO2SRAM_EN |
                       AOS_PHY_CLK_CAP_DATA2FIFO_EN;
            break;

        /* all */
        case PHY_CLK_ALL:
            u32Mask |= AOS_PHY_CLK_S_EN|
                       AOS_PHY_CLK_Q_EN |
                       AOS_PHY_CLK_RFMMD_EN |
                       AOS_PHY_CLK_S_22M_TX_EN |
                       AOS_PHY_CLK_S_22M_RX_EN |
                       AOS_PHY_CLK_S_44M_TX_EN |
                       AOS_PHY_CLK_S_44M_RX_EN |
                       AOS_PHY_CLK_Q_176M_TX_EN |
                       AOS_PHY_CLK_Q_176M_RX_EN |
                       AOS_PHY_CLK_Q_088M_TX_EN |
                       AOS_PHY_CLK_Q_088M_RX_EN |
                       AOS_PHY_CLK_Q_044M_TX_EN |
                       AOS_PHY_CLK_Q_044M_RX_EN |
                       AOS_PHY_CLK_CAP_FIFO2SRAM_EN |
                       AOS_PHY_CLK_CAP_DATA2FIFO_EN;
        default:
            return 1;
    }
    if(u8Enable)
        AOS->R_RFPHY_SEL |= u32Mask;
    else
        AOS->R_RFPHY_SEL &= ~u32Mask;
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SpareRegRead
*
* DESCRIPTION:
*   1. Read spare reg. refer to E_SpareRegIdx_t
*   2. Related reg.: AOS 0x124, 0x12C and 0x130
* CALLS
*
* PARAMETERS
*   1. eSpareIdx : each bit for the reset sources. Refer to E_SpareRegIdx_t
*   2. pu32Data  : Addr store read data
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SpareRegRead_impl(E_SpareRegIdx_t eSpareIdx, uint32_t *pu32Data)
{
    switch(eSpareIdx)
    {
        case SPARE_0:
            *pu32Data = AOS->RG_SPARE;
            break;
        case SPARE_1:
            *pu32Data = AOS->RG_SPARE_1;
            break;
        case SPARE_2:
            *pu32Data = AOS->RG_SPARE_2;
            break;
        case SPARE_3:
            *pu32Data = AOS->RG_SPARE_3;
            break;
        default:
            return 1;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_SpareRegWrite
*
* DESCRIPTION:
*   1. Wrtie spare reg. refer to E_SpareRegIdx_t
*   2. Related reg.: AOS 0x124, 0x12C and 0x130
* CALLS
*
* PARAMETERS
*   1. eSpareIdx : each bit for the reset sources. Refer to E_SpareRegIdx_t
*   2. u32Data   : Data to wrtie
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_SpareRegWrite_impl(E_SpareRegIdx_t eSpareIdx, uint32_t u32Data)
{
    switch(eSpareIdx)
    {
        case SPARE_0:
            AOS->RG_SPARE = u32Data;
            break;
        case SPARE_1:
            AOS->RG_SPARE_1 = u32Data;
            break;
        case SPARE_2:
            AOS->RG_SPARE_2 = u32Data;
            break;
        case SPARE_3:
            AOS->RG_SPARE_3 = u32Data;
            break;
        default:
            return 1;
    }
    return 0;
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_ApsClkMmFactorSet
*
* DESCRIPTION:
*   1. Set APS(M3) all clocks slow-down. Slow down Clk = Orig Clk * u32Factor/16
*   2. Related reg.: AOS 0x198
* CALLS
*
* PARAMETERS
*   1. u32Factor : Factor of slow-down function. Max:0x10
* RETURNS
*   0: setting complete
*   1: error
* GLOBALS AFFECTED
*
*************************************************************************/
uint32_t Hal_Sys_ApsClkMmFactorSet_impl(uint32_t u32Factor)
{
    if(u32Factor>AOS_M3_MMFACTOR_MAX)
    {
        // Out of range
        return 1;
    }else{
        // 0 ~ MAX, Slow-down M3 clock

        // Set factor
        AOS->R_CLK_MMFACTOR_CM3 &= ~AOS_M3_MMFACTOR_MASK;
        AOS->R_CLK_MMFACTOR_CM3 |= u32Factor;

        // Make a rising edge to trigger
        AOS->R_CLK_MMFACTOR_CM3 &= ~AOS_M3_MMFACTOR_EN;
        AOS->R_CLK_MMFACTOR_CM3 |= AOS_M3_MMFACTOR_EN;
    }

    // Update system clock divider
    SystemCoreClockDivFactorSet(u32Factor);

    // Apply to all relative modules
    Hal_Sys_ApsClkChangeApply();
    
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
void Hal_Sys_ApsClkChangeApply_impl(void)
{
    // FreeRTOS, update system tick.
    // FIXME: Need used some define...
    SysTick->LOAD =( SystemCoreClockGet()/1000 ) - 1;

    // Modules (ex. I2c, SPI, PWM...)
    // SPI
    Hal_Spi_BaudRateSet(SPI_IDX_0, Hal_Spi_BaudRateGet( SPI_IDX_0 ) );
    Hal_Spi_BaudRateSet(SPI_IDX_1, Hal_Spi_BaudRateGet( SPI_IDX_1 ) );
    Hal_Spi_BaudRateSet(SPI_IDX_2, Hal_Spi_BaudRateGet( SPI_IDX_2 ) );
    
    // I2C
    Hal_I2c_SpeedSet( Hal_I2c_SpeedGet() );
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_MsqClkChangeApply
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
void Hal_Sys_MsqClkChangeApply_impl(void)
{
    // Do nothing
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_PhyClkChangeApply
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
void Hal_Sys_PhyClkChangeApply_impl(void)
{
    // Do nothing
    __DSB();
    __ISB();
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_Pre_Init
*
* DESCRIPTION:
*   1. Inital of functuin pointers
*
* CALLS
*
* PARAMETERS
*   None
*
* RETURNS
*   None
* GLOBALS AFFECTED
*
*************************************************************************/
void Hal_Sys_Pre_Init(void)
{
    /* Power relative */
    Hal_Sys_PowerManualEn = Hal_Sys_PowerManualEn_impl;

    /* Sleep Mode relative */
    Hal_Sys_SleepInit     = Hal_Sys_SleepInit_impl;
    Hal_Sys_SleepTimerGet = Hal_Sys_SleepTimerGet_impl;
    Hal_Sys_SleepTimerSet = Hal_Sys_SleepTimerSet_impl;
    Hal_Sys_SleepTrig     = Hal_Sys_SleepTrig_impl;

    /* Pin-Mux relative*/
    Hal_SysPinMuxBootROMInit = Hal_SysPinMuxBootROMInit_impl;
    Hal_SysPinMuxAppInit     = Hal_SysPinMuxAppInit_impl;

    /* Ret RAM relative*/
    Hal_Sys_RetRamStatusRead = Hal_Sys_RetRamStatusRead_impl;
    Hal_Sys_RetRamTurnOff    = Hal_Sys_RetRamTurnOff_impl;
    Hal_Sys_RetRamTurnOn     = Hal_Sys_RetRamTurnOn_impl;

    /* Xtal fast starup relative */

    /* SW reset relative */
    Hal_Sys_SwResetAll   = Hal_Sys_SwResetAll_impl;
    Hal_Sys_ApsModuleRst = Hal_Sys_ApsModuleRst_impl;
    Hal_Sys_MsqModuleRst = Hal_Sys_MsqModuleRst_impl;

    /* Clock relative */
    Hal_Sys_ApsClkTreeSetup = Hal_Sys_ApsClkTreeSetup_impl;
    Hal_Sys_MsqClkTreeSetup = Hal_Sys_MsqClkTreeSetup_impl;
    Hal_Sys_PhyClkTreeSetup = Hal_Sys_PhyClkTreeSetup_impl;
    Hal_Sys_PwmSrcSelect    = Hal_Sys_PwmSrcSelect_impl;
    Hal_Sys_ApsClkEn        = Hal_Sys_ApsClkEn_impl;
    Hal_Sys_MsqClkEn        = Hal_Sys_MsqClkEn_impl;
    Hal_Sys_PhyClkEn        = Hal_Sys_PhyClkEn_impl;

    /* Remap relative */
    Hal_Sys_MsqRomReampAll = Hal_Sys_MsqRomReampAll_impl;

    /* Miscellaneous */
    Hal_Sys_SramDffBypass      = Hal_Sys_SramDffBypass_impl;
    Hal_Sys_SwDebugEn          = Hal_Sys_SwDebugEn_impl;
    Hal_Sys_StrapModeRead      = Hal_Sys_StrapModeRead_impl;
    Hal_Sys_StrapModeOverWrite = Hal_Sys_StrapModeOverWrite_impl;
    Hal_Sys_AosIdRead          = Hal_Sys_AosIdRead_impl;
    Hal_Sys_ChipIdRead         = Hal_Sys_ChipIdRead_impl;
    Hal_Sys_SpareRegRead       = Hal_Sys_SpareRegRead_impl;
    Hal_Sys_SpareRegWrite      = Hal_Sys_SpareRegWrite_impl;
    Hal_Sys_ApsClkMmFactorSet  = Hal_Sys_ApsClkMmFactorSet_impl;
    Hal_Sys_ApsClkChangeApply  = Hal_Sys_ApsClkChangeApply_impl;
    Hal_Sys_MsqClkChangeApply  = Hal_Sys_MsqClkChangeApply_impl;
    Hal_Sys_PhyClkChangeApply  = Hal_Sys_PhyClkChangeApply_impl;

    /* For patch only (internal use)*/
    _Hal_Sys_ApsSystemCoreClockUpdate = _Hal_Sys_ApsSystemCoreClockUpdate_impl;
    _Hal_Sys_MsqSystemCoreClockUpdate = _Hal_Sys_MsqSystemCoreClockUpdate_impl;
    _Hal_Sys_PhySystemCoreClockUpdate = _Hal_Sys_PhySystemCoreClockUpdate_impl;
}

