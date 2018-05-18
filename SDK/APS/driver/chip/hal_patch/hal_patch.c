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
*  hal_patch.c
*
*  Project:
*  --------
*  NL1000 Project - the patch implement file
*
*  Description:
*  ------------
*  This implement file is include the patch function and api.
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
#include "hal_patch.h"
#include "string.h"
#include "boot_sequence.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define PATCH_REG       ((S_Patch_Reg_t *) (SYS_BASE+0x050))
#define REMAP_REG       ((S_Remap_Reg_t *) (SYS_BASE+0x4B4))

// the max count of Hw patch
#define HAL_PATCH_HW_MAX_M3             128
#define HAL_PATCH_HW_MAX_M0             128

// the max count of remap
#define HAL_PATCH_REMAP_MAX_M3          4
#define HAL_PATCH_REMAP_MAX_M0          3

// remap enable
#define HAL_PATCH_REMAP_ENABLE          (1U << 31)


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct
{
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
} S_Patch_Reg_t;

typedef struct
{
    volatile uint32_t R_CM3_ORIG_ADD[4];  // 0x4B4 ~ 0x4C0
    volatile uint32_t R_CM3_TAG_ADD[4];   // 0x4C4 ~ 0x4D0
    volatile uint32_t R_CM3_RMP_MASK[4];  // 0x4D4 ~ 0x4E0
    volatile uint32_t R_CM0_ORIG_ADD[3];  // 0x4E4 ~ 0x4EC
    volatile uint32_t R_CM0_TAG_ADD[3];   // 0x4F0 ~ 0x4F8
    volatile uint32_t R_CM0_RMP_MASK[3];  // 0x4FC ~ 0x504
} S_Remap_Reg_t;

typedef struct
{
    uint32_t ulOrigAddr;        // original address
    uint32_t ulTagAddr;         // target address
    uint32_t ulMask;            // mask
} S_Remap_Data_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype


/***************************************************
Declaration of static Global Variables & Functions
***************************************************/
// Sec 6: declaration of static global variable
// rest data
static uint8_t g_ubaHalPatch_RestBuffer[HAL_PATCH_SIZE_HEADER];
static uint32_t g_ulHalPatch_RestSize;

// current information
static uint8_t g_ubHalPatch_CurrState;
static uint32_t g_ulHalPatch_CurrPatchType;
static uint32_t g_ulHalPatch_CurrSize;
static uint32_t g_ulHalPatch_CurrCodeAddr;
static uint32_t g_ulHalPatch_CurrCheckSum;

// hw patch and remap
// M3
// 0x00400000 ~ 0x00400200
static uint32_t g_ulaHalPatch_HwPatchMachineCodeM3[HAL_PATCH_HW_MAX_M3] __attribute__((at(0x00400000), aligned(512), used, zero_init));
// 0x00400200 ~ 0x00400400
static uint32_t g_ulaHalPatch_HwPatchAddrM3[HAL_PATCH_HW_MAX_M3] __attribute__((at(0x00400200), used, zero_init));
// 0x00400400 ~ 0x00400430
static S_Remap_Data_t g_taHalPatch_RemapDataM3[HAL_PATCH_REMAP_MAX_M3] __attribute__((at(0x00400400), used, zero_init));
// 0x00400430 ~ 0x00400434
static uint32_t g_ulHalPatch_HwPatchCountM3 __attribute__((at(0x00400430), used, zero_init));
// 0x00400434 ~ 0x00400438
static uint32_t g_ulHalPatch_RemapCountM3 __attribute__((at(0x00400434), used, zero_init));
// M0
// 0x20600000 ~ 0x20600200
static uint32_t g_ulaHalPatch_HwPatchMachineCodeM0[HAL_PATCH_HW_MAX_M0] __attribute__((at(0x20600000), aligned(512), used, zero_init));
// 0x20600200 ~ 0x20600400
static uint32_t g_ulaHalPatch_HwPatchAddrM0[HAL_PATCH_HW_MAX_M0] __attribute__((at(0x20600200), used, zero_init));
// 0x20600400 ~ 0x20600424
static S_Remap_Data_t g_taHalPatch_RemapDataM0[HAL_PATCH_REMAP_MAX_M0] __attribute__((at(0x20600400), used, zero_init));
// 0x20600424 ~ 0x20600428
static uint32_t g_ulHalPatch_HwPatchCountM0 __attribute__((at(0x20600424), used, zero_init));
// 0x20600428 ~ 0x2060042C
static uint32_t g_ulHalPatch_RemapCountM0 __attribute__((at(0x20600428), used, zero_init));


// Sec 7: declaration of static function prototype
static uint8_t Hal_Patch_RestDataStore(uint8_t ubaBuffer[], uint32_t ulIdx, uint32_t ulTotal);
static void Hal_Patch_VariableReset(void);

// state function
static uint8_t Hal_Patch_StateHeader(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateHw(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateSw(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateCodeAddr(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateCodeData(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateRemap(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateDataCheckSum(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static uint8_t Hal_Patch_StateSkip(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);


// state table
typedef uint8_t (*T_Hal_Patch_StateFp)(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest);
static T_Hal_Patch_StateFp g_taHalPatch_StateFpTable[HAL_PATCH_STATE_MAX] = 
{
    Hal_Patch_StateHeader,
    Hal_Patch_StateHw,
    Hal_Patch_StateSw,
    Hal_Patch_StateCodeAddr,
    Hal_Patch_StateCodeData,
    Hal_Patch_StateRemap,
    Hal_Patch_StateDataCheckSum,
    Hal_Patch_StateSkip
};

static void Test_ForSwPatch_impl(void);


/***********
C Functions
***********/
// Sec 8: C Functions

/*************************************************************************
* FUNCTION:
*   Hal_Patch_PreInitCold
*
* DESCRIPTION:
*   the previous initial for cold boot
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_Patch_PreInitCold(void)
{
    Test_ForSwPatch = Test_ForSwPatch_impl;

    // hw patch
    g_ulHalPatch_HwPatchCountM3 = 0;
    g_ulHalPatch_HwPatchCountM0 = 0;
//    memset(g_ulaHalPatch_HwPatchMachineCodeM3, 0, sizeof(g_ulaHalPatch_HwPatchMachineCodeM3));
//    memset(g_ulaHalPatch_HwPatchMachineCodeM0, 0, sizeof(g_ulaHalPatch_HwPatchMachineCodeM0));
//    memset(g_ulaHalPatch_HwPatchAddrM3, 0, sizeof(g_ulaHalPatch_HwPatchAddrM3));
//    memset(g_ulaHalPatch_HwPatchAddrM0, 0, sizeof(g_ulaHalPatch_HwPatchAddrM0));
    
    // remap
    g_ulHalPatch_RemapCountM3 = 0;
    g_ulHalPatch_RemapCountM0 = 0;
//    memset(g_taHalPatch_RemapDataM3, 0, sizeof(g_taHalPatch_RemapDataM3));
//    memset(g_taHalPatch_RemapDataM0, 0, sizeof(g_taHalPatch_RemapDataM0));
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_Init
*
* DESCRIPTION:
*   initial
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_Patch_Init(void)
{
    // rest data
    memset(g_ubaHalPatch_RestBuffer, 0, HAL_PATCH_SIZE_HEADER);
    g_ulHalPatch_RestSize = 0;
    
    // current information
    g_ubHalPatch_CurrState = HAL_PATCH_STATE_HEADER;
    g_ulHalPatch_CurrPatchType = 0;
    g_ulHalPatch_CurrSize = 0;
    g_ulHalPatch_CurrCodeAddr = 0;
    g_ulHalPatch_CurrCheckSum = 0;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_DataIn
*
* DESCRIPTION:
*   input data to parse and handle
*
* PARAMETERS
*   1. pulAddr         : [In] the address of memory buffer
*   2. ulSize          : [In] the size of memory buffer
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*
*************************************************************************/
uint8_t Hal_Patch_DataIn(uint32_t *pulAddr, uint32_t ulSize)
{
    uint8_t ubaBuffer[HAL_PATCH_SIZE_HEADER + HAL_PATCH_IN_SIZE_MAX];
    uint32_t ulTotal = 0;
    uint32_t ulRest = 0;
    uint32_t ulIdx = 0;

    uint8_t ubRet = HAL_PATCH_RET_FINISH;

    // 0. check the max size or zero
    if ((ulSize > HAL_PATCH_IN_SIZE_MAX) || (ulSize == 0))
    {
        // flush (init) global variables
        ubRet = HAL_PATCH_RET_FAIL; 
        goto done;
    }

    // 1. prepare the buffer data
    // 1-1. the rest data
    if (g_ulHalPatch_RestSize > 0)
    {
        memcpy(ubaBuffer, g_ubaHalPatch_RestBuffer, g_ulHalPatch_RestSize);
    }
    // 1-2. the input data
    memcpy(&(ubaBuffer[g_ulHalPatch_RestSize]), pulAddr, ulSize);
    // 1-3. compute the total size, init the index and reset the rest size
    ulTotal = g_ulHalPatch_RestSize + ulSize;
    ulIdx = 0;
    g_ulHalPatch_RestSize = 0;

    // 2. parse and handle
    while (ulIdx < ulTotal)
    {
        // the rest size of buffer
        ulRest = ulTotal - ulIdx;

        // handle the state
        if (g_ubHalPatch_CurrState < HAL_PATCH_STATE_MAX)
            ubRet = g_taHalPatch_StateFpTable[g_ubHalPatch_CurrState](ubaBuffer, &ulIdx, ulRest);
        else
        {
            // flush (init) global variables
            ubRet = HAL_PATCH_RET_FAIL; 
            goto done;
        }

        // check the return value
        if (ubRet != HAL_PATCH_RET_LOOP)
            break;
        // if loop and the rest size = 0
        else if (ulIdx == ulTotal)
        {
            ubRet = HAL_PATCH_RET_NEXT;
        }
    }

done:
    // fail: flush (init) global variables
    if (ubRet == HAL_PATCH_RET_FAIL)
    {
        Hal_Patch_VariableReset();
    }
    // next: store the rest data
    else if (ubRet == HAL_PATCH_RET_NEXT)
    {
        ubRet = Hal_Patch_RestDataStore(ubaBuffer, ulIdx, ulTotal);
    }
    // finish: do nothing
    // loop: it should not be here

    return ubRet;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_ApplyHwPatchM3
*
* DESCRIPTION:
*   apply m3 hw patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_Patch_ApplyHwPatchM3(void)
{
    uint32_t i;
    
    // Hw patch
    PATCH_REG->R_CM3_I_PATCH_ST = (uint32_t)g_ulaHalPatch_HwPatchMachineCodeM3;
    if (g_ulHalPatch_HwPatchCountM3 > 0)
    {
        // reset
        PATCH_REG->R_CM3_I_PATCH_EN[0] = 0;
        PATCH_REG->R_CM3_I_PATCH_EN[1] = 0;
        PATCH_REG->R_CM3_I_PATCH_EN[2] = 0;
        PATCH_REG->R_CM3_I_PATCH_EN[3] = 0;
    
        for (i=0; i<g_ulHalPatch_HwPatchCountM3; i++)
        {
            PATCH_REG->R_CM3_I_PATCH[i] = g_ulaHalPatch_HwPatchAddrM3[i];
            PATCH_REG->R_CM3_I_PATCH_EN[i/32] = PATCH_REG->R_CM3_I_PATCH_EN[i/32] | (1<<(i % 32));
        }
    }

    // Remap
    if (g_ulHalPatch_RemapCountM3 > 0)
    {
        // reset
        REMAP_REG->R_CM3_RMP_MASK[0] = 0;
        REMAP_REG->R_CM3_RMP_MASK[1] = 0;
        REMAP_REG->R_CM3_RMP_MASK[2] = 0;
        REMAP_REG->R_CM3_RMP_MASK[3] = 0;
    
        for (i=0; i<g_ulHalPatch_RemapCountM3; i++)
        {
            REMAP_REG->R_CM3_ORIG_ADD[i] = g_taHalPatch_RemapDataM3[i].ulOrigAddr;
            REMAP_REG->R_CM3_TAG_ADD[i] = g_taHalPatch_RemapDataM3[i].ulTagAddr;
            REMAP_REG->R_CM3_RMP_MASK[i] = HAL_PATCH_REMAP_ENABLE | g_taHalPatch_RemapDataM3[i].ulMask;
        }
    }
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_ApplyHwPatchM0
*
* DESCRIPTION:
*   apply m0 hw patch
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
void Hal_Patch_ApplyHwPatchM0(void)
{
    uint32_t i;

    // Hw patch
    PATCH_REG->R_CM0_I_PATCH_ST = (uint32_t)g_ulaHalPatch_HwPatchMachineCodeM0;
    if (g_ulHalPatch_HwPatchCountM0 > 0)
    {
        // reset
        PATCH_REG->R_CM0_I_PATCH_EN[0] = 0;
        PATCH_REG->R_CM0_I_PATCH_EN[1] = 0;
        PATCH_REG->R_CM0_I_PATCH_EN[2] = 0;
        PATCH_REG->R_CM0_I_PATCH_EN[3] = 0;
    
        for (i=0; i<g_ulHalPatch_HwPatchCountM0; i++)
        {
            PATCH_REG->R_CM0_I_PATCH[i] = g_ulaHalPatch_HwPatchAddrM0[i];
            PATCH_REG->R_CM0_I_PATCH_EN[i/32] = PATCH_REG->R_CM0_I_PATCH_EN[i/32] | (1<<(i % 32));
        }
    }

    // Remap
    if (g_ulHalPatch_RemapCountM0 > 0)
    {
        // reset
        REMAP_REG->R_CM0_RMP_MASK[0] = 0;
        REMAP_REG->R_CM0_RMP_MASK[1] = 0;
        REMAP_REG->R_CM0_RMP_MASK[2] = 0;
    
        for (i=0; i<g_ulHalPatch_RemapCountM0; i++)
        {
            REMAP_REG->R_CM0_ORIG_ADD[i] = g_taHalPatch_RemapDataM0[i].ulOrigAddr;
            REMAP_REG->R_CM0_TAG_ADD[i] = g_taHalPatch_RemapDataM0[i].ulTagAddr;
            REMAP_REG->R_CM0_RMP_MASK[i] = HAL_PATCH_REMAP_ENABLE | g_taHalPatch_RemapDataM0[i].ulMask;
        }
    }
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_RestDataStore
*
* DESCRIPTION:
*   store the rest data
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. ulIdx           : [In] the current index of data buffer
*   3. ulTotal         : [In] the total size of data buffer
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*
*************************************************************************/
static uint8_t Hal_Patch_RestDataStore(uint8_t ubaBuffer[], uint32_t ulIdx, uint32_t ulTotal)
{
    uint8_t ubRet = HAL_PATCH_RET_NEXT;

    // the rest size of buffer
    g_ulHalPatch_RestSize = ulTotal - ulIdx;

    // fail: the size is overflow
    if (g_ulHalPatch_RestSize > HAL_PATCH_SIZE_HEADER)
        ubRet = HAL_PATCH_RET_FAIL;
    else if (g_ulHalPatch_RestSize > 0)
        memcpy(g_ubaHalPatch_RestBuffer, &(ubaBuffer[ulIdx]), g_ulHalPatch_RestSize);

    return ubRet;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_VariableReset
*
* DESCRIPTION:
*   reset the global variables
*
* PARAMETERS
*   none
*
* RETURNS
*   none
*
*************************************************************************/
static void Hal_Patch_VariableReset(void)
{
    Hal_Patch_Init();

    // hw patch
    g_ulHalPatch_HwPatchCountM3 = 0;
    g_ulHalPatch_HwPatchCountM0 = 0;
    
    // remap
    g_ulHalPatch_RemapCountM3 = 0;
    g_ulHalPatch_RemapCountM0 = 0;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateHeader
*
* DESCRIPTION:
*   handle the header state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateHeader(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;

    uint32_t ulCurrMagicNum = 0;
    uint32_t ulCurrCheckSum = 0;
    uint32_t ulPatchType = 0;
    uint32_t ulCheckSum = 0;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_HEADER)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    ulCheckSum = 0;
    for (i=0; i<HAL_PATCH_SIZE_HEADER_WITHOUT_CHECKSUM; i++)     // without check sum (itself)
    {
        ulCheckSum = ulCheckSum + pubBufferStart[i];
    }
        
    // magic number
    ulCurrMagicNum = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    // patch type
    g_ulHalPatch_CurrPatchType = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    // size
    g_ulHalPatch_CurrSize = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    // check sum
    ulCurrCheckSum = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    
    // compare the magic number
    // if not match, it is "finish"
    if (ulCurrMagicNum != HAL_PATCH_MARK)
        return HAL_PATCH_RET_FINISH;
    
    // compare the check sum
    if (ulCurrCheckSum != ulCheckSum)
    {
        // flush (init) global variables
        return HAL_PATCH_RET_FAIL;
    }
    
    // change the state by patch type
    // check the valid size (not yet)
    ulPatchType = g_ulHalPatch_CurrPatchType & HAL_PATCH_FUNC_MASK;
    // Hw
    if (ulPatchType == HAL_PATCH_FUNC_HW)
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_HW_PATCH;
    // Sw
    else if (ulPatchType == HAL_PATCH_FUNC_SW)
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_SW_PATCH;
    // code (code addr)
    else if (ulPatchType == HAL_PATCH_FUNC_CODE)
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_CODE_ADDR;
    // remap
    else if (ulPatchType == HAL_PATCH_FUNC_REMAP)
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_REMAP;
    else
    {
        // flush (init) global variables
        return HAL_PATCH_RET_FAIL;
    }

    // check the boot status to skip or not
    ulPatchType = g_ulHalPatch_CurrPatchType & HAL_PATCH_APPLY_MASK;
    // cold boot
    if (ulPatchType == HAL_PATCH_APPLY_COLD)
    {
        if (1 == Boot_CheckWarmBoot())
            g_ubHalPatch_CurrState = HAL_PATCH_STATE_SKIP;
    }
    // warm boot
    else if (ulPatchType == HAL_PATCH_APPLY_WARM)
    {
        if (0 == Boot_CheckWarmBoot())
            g_ubHalPatch_CurrState = HAL_PATCH_STATE_SKIP;
    }
    
    // reset the check sum of data
    g_ulHalPatch_CurrCheckSum = 0;

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateHw
*
* DESCRIPTION:
*   handle the hw patch state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateHw(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;
    uint32_t ulUsed = 0;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_HW)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the used size
    if (ulRest < g_ulHalPatch_CurrSize)
    {
        ulUsed = ulRest;
    }
    else
    {
        ulUsed = g_ulHalPatch_CurrSize;
    }
    ulUsed = (ulUsed / HAL_PATCH_SIZE_HW) * HAL_PATCH_SIZE_HW;
    
    // check the max size and CPU type
    // M3
    if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M3)
    {
        // check the max size
        if ((g_ulHalPatch_HwPatchCountM3 + (ulUsed / HAL_PATCH_SIZE_HW)) > HAL_PATCH_HW_MAX_M3)
        {
            // flush (init) global variables
            return HAL_PATCH_RET_FAIL;
        }
    }
    // M0
    else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M0)
    {
        // check the max size
        if ((g_ulHalPatch_HwPatchCountM0 + (ulUsed / HAL_PATCH_SIZE_HW)) > HAL_PATCH_HW_MAX_M0)
        {
            // flush (init) global variables
            return HAL_PATCH_RET_FAIL;
        }
    }
    else
    {
        // flush (init) global variables
        return HAL_PATCH_RET_FAIL;
    }            
    
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<ulUsed; i++)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // store the Hw patch information
    for (i=0; i<ulUsed; i=i+HAL_PATCH_SIZE_HW)
    {
        // M3
        if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M3)
        {
            g_ulaHalPatch_HwPatchAddrM3[g_ulHalPatch_HwPatchCountM3] = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_ulaHalPatch_HwPatchMachineCodeM3[g_ulHalPatch_HwPatchCountM3] = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
        
            g_ulHalPatch_HwPatchCountM3++;
        }
        // M0
        else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M0)
        {
            g_ulaHalPatch_HwPatchAddrM0[g_ulHalPatch_HwPatchCountM0] = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_ulaHalPatch_HwPatchMachineCodeM0[g_ulHalPatch_HwPatchCountM0] = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
        
            g_ulHalPatch_HwPatchCountM0++;
        }
    }
    
    // update the current address and size
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - ulUsed;
    
    // change the state to check sum of data
    if (g_ulHalPatch_CurrSize == 0)
    {
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_DATA_CHECK_SUM;
    }

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateSw
*
* DESCRIPTION:
*   handle the sw patch state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateSw(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;
    uint32_t ulUsed = 0;

    uint32_t *pulTargetAddr;
    uint16_t *puwTargetAddr;
    uint8_t *pubTargetAddr;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_SW)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the used size
    if (ulRest < g_ulHalPatch_CurrSize)
    {
        ulUsed = ulRest;
    }
    else
    {
        ulUsed = g_ulHalPatch_CurrSize;
    }
    ulUsed = (ulUsed / HAL_PATCH_SIZE_SW) * HAL_PATCH_SIZE_SW;

    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<ulUsed; i++)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // fill the Sw patch information
    for (i=0; i<ulUsed; i=i+HAL_PATCH_SIZE_SW)
    {
        // 1 byte
        if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_LENGTH_MASK) == HAL_PATCH_LENGTH_1)
        {
            pubTargetAddr = (uint8_t *)((ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0));
            *pulIdx = *pulIdx + 4;
            *pubTargetAddr = (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
        }
        // 2 bytes
        else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_LENGTH_MASK) == HAL_PATCH_LENGTH_2)
        {
            puwTargetAddr = (uint16_t *)((ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0));
            *pulIdx = *pulIdx + 4;
            *puwTargetAddr = (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
        }
        // 4 bytes
        else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_LENGTH_MASK) == HAL_PATCH_LENGTH_4)
        {
            pulTargetAddr = (uint32_t *)((ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0));
            *pulIdx = *pulIdx + 4;
            *pulTargetAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
        }
        else
        {
            // flush (init) global variables
            return HAL_PATCH_RET_FAIL;
        }
    }
    
    // update the current address and size
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - ulUsed;
    
    // change the state to check sum of data
    if (g_ulHalPatch_CurrSize == 0)
    {
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_DATA_CHECK_SUM;
    }

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateCodeAddr
*
* DESCRIPTION:
*   handle the code address state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateCodeAddr(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_CODE_ADDR)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<HAL_PATCH_SIZE_CODE_ADDR; i++)     // without check sum (itself)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // get the code address
    g_ulHalPatch_CurrCodeAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    
    // update the current size
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - HAL_PATCH_SIZE_CODE_ADDR;
    
    // change the state to code data
    g_ubHalPatch_CurrState = HAL_PATCH_STATE_CODE_DATA;

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateCodeData
*
* DESCRIPTION:
*   handle the code data state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateCodeData(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;
    uint32_t ulUsed = 0;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_CODE_DATA)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the used size
    if (ulRest < g_ulHalPatch_CurrSize)
    {
        ulUsed = ulRest;
    }
    else
    {
        ulUsed = g_ulHalPatch_CurrSize;
    }
    
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<ulUsed; i++)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // copy the code data
    memcpy((void*)g_ulHalPatch_CurrCodeAddr, pubBufferStart, ulUsed);
    *pulIdx = *pulIdx + ulUsed;
    
    // update the current address and size
    g_ulHalPatch_CurrCodeAddr = g_ulHalPatch_CurrCodeAddr + ulUsed;
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - ulUsed;
    
    // change the state to check sum of data
    if (g_ulHalPatch_CurrSize == 0)
    {
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_DATA_CHECK_SUM;
    }

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateRemap
*
* DESCRIPTION:
*   handle the remap state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateRemap(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;
    uint32_t ulUsed = 0;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_REMAP)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the used size
    if (ulRest < g_ulHalPatch_CurrSize)
    {
        ulUsed = ulRest;
    }
    else
    {
        ulUsed = g_ulHalPatch_CurrSize;
    }
    ulUsed = (ulUsed / HAL_PATCH_SIZE_REMAP) * HAL_PATCH_SIZE_REMAP;
    
    // check the max size and CPU type
    // M3
    if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M3)
    {
        // check the max size
        if ((g_ulHalPatch_RemapCountM3 + (ulUsed / HAL_PATCH_SIZE_REMAP)) > HAL_PATCH_REMAP_MAX_M3)
        {
            // flush (init) global variables
            return HAL_PATCH_RET_FAIL;
        }
    }
    // M0
    else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M0)
    {
        // check the max size
        if ((g_ulHalPatch_RemapCountM0 + (ulUsed / HAL_PATCH_SIZE_REMAP)) > HAL_PATCH_REMAP_MAX_M0)
        {
            // flush (init) global variables
            return HAL_PATCH_RET_FAIL;
        }
    }
    else
    {
        // flush (init) global variables
        return HAL_PATCH_RET_FAIL;
    }            
    
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<ulUsed; i++)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // store the remap information
    for (i=0; i<ulUsed; i=i+HAL_PATCH_SIZE_REMAP)
    {
        // M3
        if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M3)
        {
            g_taHalPatch_RemapDataM3[g_ulHalPatch_RemapCountM3].ulOrigAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_taHalPatch_RemapDataM3[g_ulHalPatch_RemapCountM3].ulTagAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_taHalPatch_RemapDataM3[g_ulHalPatch_RemapCountM3].ulMask = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
    
            g_ulHalPatch_RemapCountM3++;
        }
        // M0
        else if ((g_ulHalPatch_CurrPatchType & HAL_PATCH_CPU_MASK) == HAL_PATCH_CPU_M0)
        {
            g_taHalPatch_RemapDataM0[g_ulHalPatch_RemapCountM0].ulOrigAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_taHalPatch_RemapDataM0[g_ulHalPatch_RemapCountM0].ulTagAddr = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
            g_taHalPatch_RemapDataM0[g_ulHalPatch_RemapCountM0].ulMask = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
            *pulIdx = *pulIdx + 4;
    
            g_ulHalPatch_RemapCountM0++;
        }
    }
    
    // update the current address and size
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - ulUsed;
    
    // change the state to check sum of data
    if (g_ulHalPatch_CurrSize == 0)
    {
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_DATA_CHECK_SUM;
    }

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateDataCheckSum
*
* DESCRIPTION:
*   handle the data check sum state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateDataCheckSum(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint32_t ulCurrCheckSum = 0;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_CHECK_SUM)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // check sum
    ulCurrCheckSum = (ubaBuffer[*pulIdx] << 24) + (ubaBuffer[*pulIdx+1] << 16) + (ubaBuffer[*pulIdx+2] << 8) + (ubaBuffer[*pulIdx+3] << 0);
    *pulIdx = *pulIdx + 4;
    
    // compare the check sum
    if (ulCurrCheckSum != g_ulHalPatch_CurrCheckSum)
    {
        // flush (init) global variables
        return HAL_PATCH_RET_FAIL;
    }
    
    // change the state to header
    g_ubHalPatch_CurrState = HAL_PATCH_STATE_HEADER;

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

/*************************************************************************
* FUNCTION:
*   Hal_Patch_StateSkip
*
* DESCRIPTION:
*   handle the skip state
*
* PARAMETERS
*   1. ubaBuffer[]     : [In] the array of data buffer
*   2. *pulIdx         : [In/Out] the current index of data buffer
*   3. ulRest          : [In] the rest size of data buffer (ulTotal - ulIdx)
*
* RETURNS
*   HAL_PATCH_RET_FAIL   : fail
*   HAL_PATCH_RET_NEXT   : wait the next data
*   HAL_PATCH_RET_FINISH : finish
*   HAL_PATCH_RET_LOOP   : continue to loop
*
*************************************************************************/
static uint8_t Hal_Patch_StateSkip(uint8_t ubaBuffer[], uint32_t *pulIdx, uint32_t ulRest)
{
    uint8_t *pubBufferStart;
    uint32_t ulUsed = 0;

    uint32_t i;

    // the size is not enough
    if (ulRest < HAL_PATCH_SIZE_SKIP)
    {
        // store the rest data
        return HAL_PATCH_RET_NEXT;
    }
    
    // the size is enough
    // compute the used size
    if (ulRest < g_ulHalPatch_CurrSize)
    {
        ulUsed = ulRest;
    }
    else
    {
        ulUsed = g_ulHalPatch_CurrSize;
    }
    
    // compute the check sum
    pubBufferStart = &(ubaBuffer[*pulIdx]);
    for (i=0; i<ulUsed; i++)
    {
        g_ulHalPatch_CurrCheckSum = g_ulHalPatch_CurrCheckSum + pubBufferStart[i];
    }
    
    // skip the data
    *pulIdx = *pulIdx + ulUsed;
    
    // update the current address and size
    g_ulHalPatch_CurrSize = g_ulHalPatch_CurrSize - ulUsed;
    
    // change the state to check sum of data
    if (g_ulHalPatch_CurrSize == 0)
    {
        g_ubHalPatch_CurrState = HAL_PATCH_STATE_DATA_CHECK_SUM;
    }

    // loop for the next state
    return HAL_PATCH_RET_LOOP;
}

// for Sw patch test, need to remove
static void Test_ForSwPatch_impl(void)
{
    printf("Test_ForSwPatch_impl.\n");
}
RET_DATA T_Test_ForSwPatchFp Test_ForSwPatch;
