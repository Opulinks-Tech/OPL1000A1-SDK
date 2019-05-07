/******************************************************************************
*  Copyright 2017, Netlink Communication Corp.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/

/******************************************************************************
*  Filename:
*  ---------
*  hal_system_patch.c
*
*  Project:
*  --------
*  NL1000_A1 series
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
#include <string.h>
#include "hal_system.h"
#include "hal_system_patch.h"
#include "hal_wdt.h"
#include "hal_spi.h"
#include "hal_i2c.h"
#include "hal_dbg_uart.h"
#include "hal_pin.h"
#include "hal_pin_def.h"
#include "hal_pin_config_patch.h"

// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define AOS             ((S_Aos_Reg_t *) AOS_BASE)
#define SYS_REG         ((S_Sys_Reg_t *) SYS_BASE)
#define OTP             ((S_Otp_Reg_t *) OTP_BASE)

#define MHZ                          1000000
#define CLK_RC                       XTAL
#define CLK_XTAL                     XTAL
#define CLK_RF                       (176*MHZ)

// SYS 0x00C
#define SYS_APS_RAM_WAIT_STATE_BYPASS    (0x1 << 2)
#define SYS_SHM_RAM_WAIT_STATE_BYPASS    (0x1 << 3)

// AOS 0x004
#define AOS_SLP_MODE_EN              (1<<0)

// AOS 0x020
#define AOS_RET_SF_VOL_POS           0
#define AOS_RET_SF_VOL_MSK           (0xF << AOS_RET_SF_VOL_POS)
#define AOS_RET_SF_VOL_0P55          (0x0 << AOS_RET_SF_VOL_POS)
#define AOS_RET_SF_VOL_0P67          (0x3 << AOS_RET_SF_VOL_POS)
#define AOS_RET_SF_VOL_0P86          (0x8 << AOS_RET_SF_VOL_POS)
#define AOS_RET_SF_VOL_1P20          (0xF << AOS_RET_SF_VOL_POS)


// AOS 0x134
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
#define AOS_APS_CLK_EN_I2C_PCLK      (1<<5)
#define AOS_APS_CLK_EN_TMR_0_PCLK    (1<<6)
#define AOS_APS_CLK_EN_TMR_1_PCLK    (1<<7)
#define AOS_APS_CLK_EN_WDT_PCLK      (1<<8)
#define AOS_APS_CLK_EN_SPI_0_PCLK    (1<<10)
#define AOS_APS_CLK_EN_SPI_1_PCLK    (1<<11)
#define AOS_APS_CLK_EN_SPI_2_PCLK    (1<<12)
#define AOS_APS_CLK_EN_UART_0_PCLK   (1<<13)
#define AOS_APS_CLK_EN_UART_1_PCLK   (1<<14)
#define AOS_APS_CLK_EN_DBG_UART_PCLK (1<<15)
#define AOS_APS_CLK_EN_OTP_PCLK      (1<<16)
#define AOS_APS_CLK_EN_DMA_HCLK      (1<<18)
#define AOS_APS_CLK_EN_SCRT_HCLK     (1<<24)
#define AOS_APS_CLK_EN_PWM_CLK       (1<<26)
#define AOS_APS_CLK_EN_JTAG_HCLK     (1<<28)
#define AOS_APS_CLK_EN_WDT_INTERNAL  (1<<30)
#define AOS_APS_CLK_DIV2             (1<<2)
#define AOS_APS_PCLK_DIV2            (1<<3)

// AOS 0x138
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


#define WDT_TIMEOUT_SECS            10



#define STRAP_NORMAL_MODE       0xA

// For patch only (internal use)
typedef void (*T_Hal_Sys_SystemCoreClockUpdate)(uint32_t u32CoreClk);

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
    volatile uint32_t DATA[128]; // Offset: 0x000 ~ 0x1FC (R/ ) OTP data                                                                 */
} S_Otp_Reg_t;

/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global  variable
/* For store current clock */
E_ApsClkTreeSrc_t g_eClkTreeSrc_Curr = ASP_CLKTREE_SRC_RC_BB;
uint8_t g_u8ClkDivEn_Curr = 0;
uint8_t g_u8PclkDivEn_Curr = 0;

/* For store the clock be resumed */
E_ApsClkTreeSrc_t g_eClkTreeSrc_Resume = ASP_CLKTREE_SRC_RC_BB;
uint8_t g_u8ClkDivEn_Resume = 0;
uint8_t g_u8PclkDivEn_Resume = 0;

T_Hal_Sys_DisableClock Hal_Sys_DisableClock;

// Sec 5: declaration of global function prototype
/* Power relative */

/* Sleep Mode relative */

/* Pin-Mux relative*/
RET_DATA T_Hal_SysPinMuxM3UartSwitch Hal_SysPinMuxM3UartSwitch;

/* Ret RAM relative*/

/* Xtal fast starup relative */

/* SW reset relative */

/* Clock relative */
extern uint32_t Hal_Sys_MsqClkTreeSetup_impl(E_MsqClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn );

/* Remap relative */

/* Miscellaneous */
T_Hal_Sys_OtpRead  Hal_Sys_OtpRead;

/* For patch only (internal use)*/
extern T_Hal_Sys_SystemCoreClockUpdate  _Hal_Sys_ApsSystemCoreClockUpdate;

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
*  Hal_Sys_PowerDefaultSettings
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
uint32_t Hal_Sys_PowerDefaultSettings_patch(void)
{
    // Enable PowerManual
    Hal_Sys_PowerManualEn(1);

    // Remove CBC, RC and XTAL setting. It must exist.

    return 0;
}

void Hal_Sys_SleepInit_patch(void)
{
    // Set RetRAM voltage
    AOS->RET_SF_VAL_CTL  = AOS_RET_SF_VOL_0P86;

    // Need make rising pulse. So clean bit first
    AOS->MODE_CTL &= ~AOS_SLP_MODE_EN;

    // HW provide setting to make pulse
    AOS->PS_TIMER_PRESET = 0x001A; // PS 0b' 11 1111 1111
    AOS->ON1_TIME        = 0x9401; // ON_1 0b' 0 | 1010 01 | 00 0010 0000
    AOS->ON2_TIME        = 0x9C02; // ON_2 0b' 0 | 1010 11 | 00 0100 0000
    AOS->ON3_TIME        = 0xDC03; // ON_3 0b' 0 | 1111 11 | 00 0110 0000
    AOS->ON4_TIME        = 0xDC03; // ON_4 0b' 0 | 1111 11 | 00 1000 0000
    AOS->ON5_TIME        = 0xDC03; // ON_5 0b' 0 | 1111 11 | 00 1010 0000
    AOS->ON6_TIME        = 0xDC03; // ON_6 0b' 0 | 1101 11 | 00 1100 0000
    AOS->ON7_TIME        = 0x6C09; // ON_7 0b' 0 | 0010 11 | 11 1111 1110
    AOS->CPOR_N_ON_TIME  = 0x041A; // CPOR 0b' 0100 1111 1111 1110

    AOS->SPS_TIMER_PRESET = 0x0006; // SPS 0b' 00 0000 1011 0000
    AOS->SON1_TIME        = 0xDC01; // SON_1 0b' 1 | 1101 11 | 00 0010 0000
    AOS->SON2_TIME        = 0xDC02; // SON_2 0b' 1 | 1111 11 | 00 0100 0000
    AOS->SON3_TIME        = 0x9C03; // SON_3 0b' 0 | 1111 11 | 00 0100 0101
    AOS->SON4_TIME        = 0x9404; // SON_4 0b' 0 | 1111 11 | 00 0101 0000
    AOS->SON5_TIME        = 0x9005; // SON_5 0b' 0 | 1011 11 | 00 0110 0000
    AOS->SON6_TIME        = 0x9005; // SON_6 0b' 0 | 1011 01 | 00 1000 0000
    AOS->SON7_TIME        = 0x9005; // SON_7 0b' 0 | 1011 00 | 00 1010 0000
    AOS->SCPOR_N_ON_TIME  = 0x0006; //SCPOR 0b' 0000 0000 0100 0000
}

uint32_t Hal_Sys_ApsClkTreeSetup_patch(E_ApsClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn, uint8_t u8PclkDivEn )
{
/* Two level switch: 176M-switch and src-switch. 176M-switch output to src-switch
 *  @176M-switch: [31][29]
 *      0b'00:CLK_XTAL_X4, 0b'01:CLK_DECI, 0b'10:CLK_1P2G, 0b'11: CLK_EXT
 *  @src-switch: [1:0]
 *        0b'00:CLK_RC, 0b'01:CLK_XTAL, 0b'10:CLK_XTAL_X2, 0b'11: CLK_FROM_176M_SWTICH
 */
    uint32_t u32Temp = 0;
    uint32_t u32CoreClk = 0;
    uint32_t u32Div_Idx = 0;

    // Assigned the default 1P2G clock    
    if(eClkTreeSrc == ASP_CLKTREE_SRC_1P2G_DIV)
            eClkTreeSrc = (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_152MHZ;
        
    // Due to RC source need
    if(eClkTreeSrc == ASP_CLKTREE_SRC_RC_BB)
    {
        // make sure RC clock enable, due to RF turn off RC
        *(volatile uint32_t *)0x40009090 |= (0x1 << 13);
        *(volatile uint32_t *)0x40009048 |= (0x1 << 11) | (0x1 << 14);
    }

    // Due to VCO relative clocks
    if( ( eClkTreeSrc == ASP_CLKTREE_SRC_DECI ) || 
        ( ( eClkTreeSrc >= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MIN ) &&  ( eClkTreeSrc <= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MAX ) )
    )
    {
        // make sure VCO enable
        *(volatile uint32_t *)0x40009090 |= (0x1 << 5) | (0x1 << 9) | (0x1 << 30);
    }

    // Due to DECI gating
    if( eClkTreeSrc == ASP_CLKTREE_SRC_DECI )
    {
        // make sure DECI ungated
        *(volatile uint32_t *)0x40009048 |= (0x1 << 5);
    }
    
    // Due to 1P2G gating
    if( ( eClkTreeSrc >= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MIN ) &&  ( eClkTreeSrc <= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MAX ) )
    {
        // make sure 1P2G ungated
        *(volatile uint32_t *)0x40009008 |= (0x1 << 30);
    }

    // Setup clock
    u32Temp = AOS->R_M3CLK_SEL;
    switch(eClkTreeSrc)
    {
        // From RC/ XTAL
        // Note: don't change AOS_APS_CLK_176M_SRC_MASK part in the same time
        case ASP_CLKTREE_SRC_RC_BB:
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_RC;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = CLK_RC;
            break;
        case ASP_CLKTREE_SRC_XTAL:
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_XTAL;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = CLK_XTAL;
            break;
        case ASP_CLKTREE_SRC_XTAL_X2:
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_XTAL_X2;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = CLK_XTAL*2;
            break;
        //-------------------------------------------------------------
        // From VCO
        case ASP_CLKTREE_SRC_DECI:
            // Switch source to XTAL
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_XTAL;
            AOS->R_M3CLK_SEL = u32Temp;
            // set 176M-switch to DECI
            u32Temp &= ~AOS_APS_CLK_176M_SRC_MASK;
            u32Temp |= AOS_APS_CLK_176M_SRC_DECI;
            AOS->R_M3CLK_SEL = u32Temp;
            // switch source to 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = CLK_RF;
            break;
        case ASP_CLKTREE_SRC_1P2G_078MHZ:
        case ASP_CLKTREE_SRC_1P2G_081MHZ:
        case ASP_CLKTREE_SRC_1P2G_084MHZ:
        case ASP_CLKTREE_SRC_1P2G_087MHZ:
        case ASP_CLKTREE_SRC_1P2G_090MHZ:
        case ASP_CLKTREE_SRC_1P2G_093MHZ:
        case ASP_CLKTREE_SRC_1P2G_097MHZ:
        case ASP_CLKTREE_SRC_1P2G_101MHZ:
        case ASP_CLKTREE_SRC_1P2G_106MHZ:
        case ASP_CLKTREE_SRC_1P2G_110MHZ:
        case ASP_CLKTREE_SRC_1P2G_116MHZ:
        case ASP_CLKTREE_SRC_1P2G_122MHZ:
        case ASP_CLKTREE_SRC_1P2G_128MHZ:
        case ASP_CLKTREE_SRC_1P2G_135MHZ:
        case ASP_CLKTREE_SRC_1P2G_143MHZ:
        case ASP_CLKTREE_SRC_1P2G_152MHZ:
            // Switch source to XTAL
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_XTAL;
            AOS->R_M3CLK_SEL = u32Temp;
            // set 176M-switch to 1P2G
            u32Temp &= ~AOS_APS_CLK_176M_SRC_MASK;
            u32Temp |= AOS_APS_CLK_176M_SRC_1P2G;
            AOS->R_M3CLK_SEL = u32Temp;
            // Set DIV
            u32Div_Idx = 15 - ( eClkTreeSrc - ASP_CLKTREE_SRC_1P2G_MIN);
            *(volatile uint32_t *)0x40009008 &= ~ 0xF000;
            *(volatile uint32_t *)0x40009008 |= u32Div_Idx << 12;
            // switch source to 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = 2440/(u32Div_Idx + 16)*MHZ;
            break;
        case ASP_CLKTREE_SRC_EXTERNAL:
            // Switch source to XTAL
            u32Temp &= ~AOS_APS_CLK_SRC_MASK;
            u32Temp |= AOS_APS_CLK_SRC_XTAL;
            AOS->R_M3CLK_SEL = u32Temp;
            // set 176M-switch to EXT_SRC
            u32Temp &= ~AOS_APS_CLK_176M_SRC_MASK;
            u32Temp |= AOS_APS_CLK_176M_SRC_EXT;
            AOS->R_M3CLK_SEL = u32Temp;
            // switch source to 176M-switch
            u32Temp |= AOS_APS_CLK_SRC_176M_SWITCH;
            AOS->R_M3CLK_SEL = u32Temp;
            u32CoreClk = CLK_RF;
            break;
        //-------------------------------------------------------------
        case ASP_CLKTREE_SRC_XTAL_X4:
            // Not surpported
        default:
            return 1;
    }

    // Due to RC source need
    if(eClkTreeSrc == ASP_CLKTREE_SRC_RC_BB){}
    else
    {
         // Check the other CPU cloock
        if( (AOS->R_M0CLK_SEL & AOS_MSQ_CLK_SRC_MASK) != AOS_MSQ_SRC_RC)
        {
            // *(volatile uint32_t *)0x40009048 &= ~( (0x1 << 11) | (0x1 << 14) );
            // *(volatile uint32_t *)0x40009090 &= ~(0x1 << 13);
        }
    }
    if( ( eClkTreeSrc == ASP_CLKTREE_SRC_DECI ) || 
        ( ( eClkTreeSrc >= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MIN ) &&  ( eClkTreeSrc <= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MAX ) )
    ){}
    else
    {
        *(volatile uint32_t *)0x40009090 &= ~( (0x1 << 5) | (0x1 << 9) | (0x1 << 30) );
    }
    if( eClkTreeSrc == ASP_CLKTREE_SRC_DECI ){}
    else
    {
        *(volatile uint32_t *)0x40009048 &= ~(0x1 << 5);
    }
    if( ( eClkTreeSrc >= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MIN ) &&  ( eClkTreeSrc <= (E_ApsClkTreeSrc_t)ASP_CLKTREE_SRC_1P2G_MAX ) ){}
    else
    {
        *(volatile uint32_t *)0x40009008 &= ~(0x1 << 30);
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

    // Backup clock setting for sleep-wakeup
    g_eClkTreeSrc_Curr = eClkTreeSrc;
    g_u8ClkDivEn_Curr  = u8ClkDivEn;
    g_u8PclkDivEn_Curr = u8PclkDivEn;

    return 0;
}

void Hal_Sys_ApsClkStore( void )
{
    g_eClkTreeSrc_Resume = g_eClkTreeSrc_Curr;
    g_u8ClkDivEn_Resume  = g_u8ClkDivEn_Curr;
    g_u8PclkDivEn_Resume = g_u8PclkDivEn_Curr;
}

void Hal_Sys_ApsClkResume( void )
{
    Hal_Sys_ApsClkTreeSetup(g_eClkTreeSrc_Resume, g_u8ClkDivEn_Resume, g_u8PclkDivEn_Resume);
}

uint32_t Hal_Sys_MsqClkTreeSetup_patch(E_MsqClkTreeSrc_t eClkTreeSrc, uint8_t u8ClkDivEn )
{
    if(eClkTreeSrc == MSQ_CLKTREE_SRC_RC)
    {
        // make sure RC clock enable, due to RF turn off RC
        *(volatile uint32_t *)0x40009090 |= (0x1 << 13);
        *(volatile uint32_t *)0x40009048 |= (0x1 << 11) | (0x1 << 14);
    }

    // Orignal code
    return Hal_Sys_MsqClkTreeSetup_impl( eClkTreeSrc, u8ClkDivEn );
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
    Hal_Pin_ConfigSet(0, HAL_PIN_TYPE_PATCH_IO_0, HAL_PIN_DRIVING_PATCH_IO_0);
    Hal_Pin_ConfigSet(1, HAL_PIN_TYPE_PATCH_IO_1, HAL_PIN_DRIVING_PATCH_IO_1);
    Hal_Pin_ConfigSet(2, HAL_PIN_TYPE_PATCH_IO_2, HAL_PIN_DRIVING_PATCH_IO_2);
    Hal_Pin_ConfigSet(3, HAL_PIN_TYPE_PATCH_IO_3, HAL_PIN_DRIVING_PATCH_IO_3);
    Hal_Pin_ConfigSet(4, HAL_PIN_TYPE_PATCH_IO_4, HAL_PIN_DRIVING_PATCH_IO_4);
    Hal_Pin_ConfigSet(5, HAL_PIN_TYPE_PATCH_IO_5, HAL_PIN_DRIVING_PATCH_IO_5);
    Hal_Pin_ConfigSet(6, HAL_PIN_TYPE_PATCH_IO_6, HAL_PIN_DRIVING_PATCH_IO_6);
    Hal_Pin_ConfigSet(7, HAL_PIN_TYPE_PATCH_IO_7, HAL_PIN_DRIVING_PATCH_IO_7);
    Hal_Pin_ConfigSet(8, HAL_PIN_TYPE_PATCH_IO_8, HAL_PIN_DRIVING_PATCH_IO_8);
    Hal_Pin_ConfigSet(9, HAL_PIN_TYPE_PATCH_IO_9, HAL_PIN_DRIVING_PATCH_IO_9);
    Hal_Pin_ConfigSet(10, HAL_PIN_TYPE_PATCH_IO_10, HAL_PIN_DRIVING_PATCH_IO_10);
    Hal_Pin_ConfigSet(11, HAL_PIN_TYPE_PATCH_IO_11, HAL_PIN_DRIVING_PATCH_IO_11);
    Hal_Pin_ConfigSet(12, HAL_PIN_TYPE_PATCH_IO_12, HAL_PIN_DRIVING_PATCH_IO_12);
    Hal_Pin_ConfigSet(13, HAL_PIN_TYPE_PATCH_IO_13, HAL_PIN_DRIVING_PATCH_IO_13);
    Hal_Pin_ConfigSet(14, HAL_PIN_TYPE_PATCH_IO_14, HAL_PIN_DRIVING_PATCH_IO_14);
    Hal_Pin_ConfigSet(15, HAL_PIN_TYPE_PATCH_IO_15, HAL_PIN_DRIVING_PATCH_IO_15);
    Hal_Pin_ConfigSet(16, HAL_PIN_TYPE_PATCH_IO_16, HAL_PIN_DRIVING_PATCH_IO_16);
    Hal_Pin_ConfigSet(17, HAL_PIN_TYPE_PATCH_IO_17, HAL_PIN_DRIVING_PATCH_IO_17);
    Hal_Pin_ConfigSet(18, HAL_PIN_TYPE_PATCH_IO_18, HAL_PIN_DRIVING_PATCH_IO_18);
    Hal_Pin_ConfigSet(19, HAL_PIN_TYPE_PATCH_IO_19, HAL_PIN_DRIVING_PATCH_IO_19);
    Hal_Pin_ConfigSet(20, HAL_PIN_TYPE_PATCH_IO_20, HAL_PIN_DRIVING_PATCH_IO_20);
    Hal_Pin_ConfigSet(21, HAL_PIN_TYPE_PATCH_IO_21, HAL_PIN_DRIVING_PATCH_IO_21);
    Hal_Pin_ConfigSet(22, HAL_PIN_TYPE_PATCH_IO_22, HAL_PIN_DRIVING_PATCH_IO_22);
    Hal_Pin_ConfigSet(23, HAL_PIN_TYPE_PATCH_IO_23, HAL_PIN_DRIVING_PATCH_IO_23);
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
void Hal_SysPinMuxDownloadInit_patch(void)
{
    Hal_SysPinMuxM3UartSwitch();
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
void Hal_SysPinMuxSpiFlashInit_patch(void)
{
// SPI0 standard mode
    // IO12(CS), IO13(CLK), IO14(MOSI), IO15(MISO)
    Hal_Pin_ConfigSet(12, PIN_TYPE_SPI0_CS, PIN_DRIVING_FLOAT);
    Hal_Pin_ConfigSet(13, PIN_TYPE_SPI0_CLK, PIN_DRIVING_FLOAT);
    Hal_Pin_ConfigSet(14, PIN_TYPE_SPI0_IO_0, PIN_DRIVING_FLOAT);
    Hal_Pin_ConfigSet(15, PIN_TYPE_SPI0_IO_1, PIN_DRIVING_FLOAT);
}

/*************************************************************************
* FUNCTION:
*  Hal_SysPinMuxM3UartSwitch
*
* DESCRIPTION:
*   1. Pin-Mux for download stage
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
void Hal_SysPinMuxM3UartSwitch_impl(void)
{
// APS_dbg_uart
    // IO0(TX), IO1(RX)
    Hal_Pin_ConfigSet(0, PIN_TYPE_UART_APS_TX, PIN_DRIVING_FLOAT);
    Hal_Pin_ConfigSet(1, PIN_TYPE_UART_APS_RX, PIN_DRIVING_FLOAT);
    
// UART1
    // IO8(TX), IO9(RX)
    Hal_Pin_ConfigSet(8, PIN_TYPE_UART1_TX, PIN_DRIVING_FLOAT);
    Hal_Pin_ConfigSet(9, PIN_TYPE_UART1_RX, PIN_DRIVING_HIGH);
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
void Hal_Sys_ApsClkChangeApply_patch(void)
{
    // FreeRTOS, update system tick.
    // FIXME: Need used some define...
    SysTick->LOAD =( SystemCoreClockGet()/1000 ) - 1;

    // Modules (ex. I2c, SPI, PWM...)
    // debug UART
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_DBG_UART_PCLK)
        Hal_DbgUart_BaudRateSet( Hal_DbgUart_BaudRateGet() );

    // SPI
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_SPI_0_PCLK)
        Hal_Spi_BaudRateSet(SPI_IDX_0, Hal_Spi_BaudRateGet( SPI_IDX_0 ) );
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_SPI_1_PCLK)
        Hal_Spi_BaudRateSet(SPI_IDX_1, Hal_Spi_BaudRateGet( SPI_IDX_1 ) );
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_SPI_2_PCLK)
        Hal_Spi_BaudRateSet(SPI_IDX_2, Hal_Spi_BaudRateGet( SPI_IDX_2 ) );
    
    // I2C
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_I2C_PCLK)
        Hal_I2c_SpeedSet( Hal_I2c_SpeedGet() );
    
    // WDT
    if (AOS->R_M3CLK_SEL & AOS_APS_CLK_EN_WDT_PCLK)
        Hal_Wdt_Feed(WDT_TIMEOUT_SECS * SystemCoreClockGet());
    
}


/**
 * @brief To gate perpherials clock.
 *        When enable perpherial, API needs to turn on the clock.
 *
 * Peripherals:
 *    - I2C
 *    - TMR 0/1
 *    - WDT
 *    - SPI 0/1/2
 *    - UART 0/1
 *    - DbgUart
 *    - Pwm
 *    - Jtag
 *    - OTP
 *    - DMA
 *    - SCRT
 */
void Hal_Sys_DisableClock_impl(void)
{
    uint32_t u32DisClk;
    
    u32DisClk = AOS_APS_CLK_EN_I2C_PCLK |
                AOS_APS_CLK_EN_TMR_0_PCLK |
                AOS_APS_CLK_EN_TMR_1_PCLK |
                AOS_APS_CLK_EN_WDT_PCLK |
                AOS_APS_CLK_EN_SPI_0_PCLK |
                AOS_APS_CLK_EN_SPI_1_PCLK |
                AOS_APS_CLK_EN_SPI_2_PCLK |
                AOS_APS_CLK_EN_UART_0_PCLK |
                AOS_APS_CLK_EN_UART_1_PCLK |
                AOS_APS_CLK_EN_DBG_UART_PCLK |
                AOS_APS_CLK_EN_PWM_CLK |
                AOS_APS_CLK_EN_WDT_INTERNAL |
                AOS_APS_CLK_EN_OTP_PCLK |
                AOS_APS_CLK_EN_DMA_HCLK | 
                AOS_APS_CLK_EN_SCRT_HCLK;
    
    if (Hal_Sys_StrapModeRead() == STRAP_NORMAL_MODE)
        u32DisClk |= AOS_APS_CLK_EN_JTAG_HCLK;
    
    AOS->R_M3CLK_SEL = AOS->R_M3CLK_SEL & ~u32DisClk;    
    
}

/*************************************************************************
* FUNCTION:
*  Hal_Sys_OtpRead
*
* DESCRIPTION:
*   1. Get OTP data address and copy data to buffer
*
* PARAMETERS
*   u16Offset : offset of OTP data
*   u8aBuf    : buffer to read data
*   u16BufSize: size of buffer
* RETURNS
*   Non-NULL: setting complete
*   NULL: errror
* GLOBALS AFFECTED
*
*************************************************************************/
uint8_t *Hal_Sys_OtpRead_impl(uint16_t u16Offset, uint8_t *u8aBuf, uint16_t u16BufSize)
{
    uint8_t *pu8Data = NULL;
    uint16_t u16Total = sizeof(OTP->DATA);

    if(u16Offset >= u16Total)
    {
        goto done;
    }

    Hal_Sys_ApsClkEn(1, APS_CLK_OTP);

    pu8Data = (uint8_t *)(OTP->DATA);
    pu8Data += u16Offset;

    if(u8aBuf && u16BufSize)
    {
        uint16_t u16DataSize = u16BufSize;

        if(u16Offset + u16BufSize > u16Total)
        {
            u16DataSize = u16Total - u16Offset;
        }

        memcpy(u8aBuf, pu8Data, u16DataSize);
    }

done:
    return pu8Data;
}

