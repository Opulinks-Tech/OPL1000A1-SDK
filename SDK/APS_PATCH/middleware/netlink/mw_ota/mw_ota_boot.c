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
*  mw_ota_boot.c
*
*  Project:
*  --------
*  OPL1000 Project - the Over The Air (OTA) implement file
*
*  Description:
*  ------------
*  This implement file is include the Over The Air (OTA) function and api.
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
#include <stdlib.h>
#include <string.h>
#include "mw_ota.h"
#include "mw_ota_boot.h"
#include "boot_sequence.h"
#include "hal_dbg_uart.h"


// Sec 2: Constant Definitions, Imported Symbols, miscellaneous
#define PATCH_HEAD_LEN      8
#define PATCH_DATA_LEN      256

#define CHECK               "<CHECK>"
#define ACK                 "<ACK>"
#define NACK                "<NACK>"
#define START               "<START>"
#define ESCAPE              "<ESCAPE>"
#define AGENT               "<AGENT>"

#define BOOT_CHECK_RETRY    100     // 100ms

#define PATTERN_ENTRY       0xfe
#define PATTERN_BOOT        'u'
#define PATTERN_PATCH       'p'
#define PATTERN_ESCAPE      'e'
#define PATTERN_WRITE       '1'
#define PATTERN_ERASE       '2'
#define PATTERN_HEADER      '3'     // OTA image header


/********************************************
Declaration of data structure
********************************************/
// Sec 3: structure, uniou, enum, linked list
typedef struct UART_DATA_HEADER {
    uint8_t head[PATCH_HEAD_LEN];
    uint32_t number;
} UART_DATA_HEADER_t;


/********************************************
Declaration of Global Variables & Functions
********************************************/
// Sec 4: declaration of global variable


// Sec 5: declaration of global function prototype
extern bool Boot_CheckPattern(uint8_t checkPattern, uint32_t retry);
extern void Boot_SpiLoadPatch(uint32_t ulStartAddr, uint32_t ulMaxSize, uint8_t ubUseQuadMode);
extern uint32_t Boot_RecvMultiData(uint8_t *data, uint32_t u32Length);
extern uint32_t Boot_SwapEndian(uint32_t num);
extern uint32_t Boot_GetCheckSum(uint8_t * data, uint32_t lenth);

RET_DATA T_MwOta_Boot_Init_Fp MwOta_Boot_Init;
RET_DATA T_MwOta_Boot_CheckUartBehavior_Fp MwOta_Boot_CheckUartBehavior;
RET_DATA T_MwOta_Boot_LoadPatchImage_Fp MwOta_Boot_LoadPatchImage;

// internal part
RET_DATA T_MwOta_Boot_HeaderPaser_Fp MwOta_Boot_HeaderPaser;
RET_DATA T_MwOta_Boot_WritePatchImage_Fp MwOta_Boot_WritePatchImage;


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
*   MwOta_Boot_Init
*
* DESCRIPTION:
*   the init of OTA boot
*
* PARAMETERS
*   none
*
* RETURNS
*   MW_OTA_OK   : successful
*   MW_OTA_FAIL : fail
*
*************************************************************************/
uint8_t MwOta_Boot_Init_impl(void)
{
    T_MwOtaLayoutInfo tLayout;
    
    // give the layout information
    tLayout.ulaHeaderAddr[0] = MW_OTA_HEADER_ADDR_1;
    tLayout.ulaHeaderAddr[1] = MW_OTA_HEADER_ADDR_2;
    tLayout.ulaImageAddr[0] = MW_OTA_IMAGE_ADDR_1;
    tLayout.ulaImageAddr[1] = MW_OTA_IMAGE_ADDR_2;
    tLayout.ulImageSize = MW_OTA_IMAGE_SIZE;
    if (MW_OTA_OK != MwOta_Init(&tLayout, 1))
        return MW_OTA_FAIL;
    
    // give the fake current index
    MwOta_CurrentIdxFake();
    
    return MW_OTA_OK;
}

/*************************************************************************
* FUNCTION:
*   MwOta_Boot_CheckUartBehavior
*
* DESCRIPTION:
*   do the 2nd boot loader
*
* PARAMETERS
*   none
*
* RETURNS
*   0 : go to load the image from flash
*   1 : don't load the image from flash
*
*************************************************************************/
uint8_t MwOta_Boot_CheckUartBehavior_impl(void)
{
    uint32_t ulData = 0;

StartCheck:
    Boot_SendMultiData(AGENT);

    if (Boot_CheckPattern(PATTERN_ENTRY, BOOT_CHECK_RETRY))
    {
        Hal_DbgUart_DataRecvTimeOut(&ulData, 1);
        switch(ulData)
        {
            case PATTERN_WRITE:
                if (MW_OTA_OK == MwOta_Boot_WritePatchImage())
                {
                    MwOta_DataFinish();
                }
                goto StartCheck;

            case PATTERN_HEADER:
                MwOta_Boot_HeaderPaser();
                goto StartCheck;

            case PATTERN_ESCAPE:
            default:
                Boot_SendMultiData(ESCAPE);
                break;
        }
    }
    
    return 0;
}

/*************************************************************************
* FUNCTION:
*   MwOta_Boot_LoadPatchImage
*
* DESCRIPTION:
*   load the patch image
*
* PARAMETERS
*   none
*
* RETURNS
*   MW_OTA_OK   : successful
*   MW_OTA_FAIL : fail
*
*************************************************************************/
uint8_t MwOta_Boot_LoadPatchImage_impl(void)
{
    uint32_t ulImageAddr;
    
    // get the start address of patch image
    if (MW_OTA_OK != MwOta_BootAddrGet(&ulImageAddr))
        return MW_OTA_FAIL;
    
    // load the patch image
    Boot_SpiLoadPatch(ulImageAddr, MW_OTA_IMAGE_SIZE, 0);
    
    return MW_OTA_OK;
}

/*************************************************************************
* FUNCTION:
*   MwOta_Boot_HeaderPaser
*
* DESCRIPTION:
*   get the header information and prepare the information for OTA write
*
* PARAMETERS
*   none
*
* RETURNS
*   MW_OTA_OK   : successful
*   MW_OTA_FAIL : fail
*
*************************************************************************/
uint8_t MwOta_Boot_HeaderPaser_impl(void)
{
    T_MwOtaImageHeader tHeader;
    
    Boot_SendMultiData(START);
    
    // receive the data
    if (0 == Boot_RecvMultiData((uint8_t *)&tHeader, sizeof(T_MwOtaImageHeader)))
    {
        // change from big-endian to little-endian
        tHeader.uwProjectId = ((tHeader.uwProjectId & 0x00FF) << 8) + ((tHeader.uwProjectId & 0xFF00) >> 8);
        tHeader.uwChipId = ((tHeader.uwChipId & 0x00FF) << 8) + ((tHeader.uwChipId & 0xFF00) >> 8);
        tHeader.uwFirmwareId = ((tHeader.uwFirmwareId & 0x00FF) << 8) + ((tHeader.uwFirmwareId & 0xFF00) >> 8);
        tHeader.uwCheckSum = ((tHeader.uwCheckSum & 0x00FF) << 8) + ((tHeader.uwCheckSum & 0xFF00) >> 8);
        tHeader.ulImageSize = ((tHeader.ulImageSize & 0x000000FF) << 24)
                            + ((tHeader.ulImageSize & 0x0000FF00) << 8)
                            + ((tHeader.ulImageSize & 0x00FF0000) >> 8)
                            + ((tHeader.ulImageSize & 0xFF000000) >> 24);
        
        // prepare the information
        if (MW_OTA_OK != MwOta_Prepare(tHeader.uwProjectId, tHeader.uwChipId, tHeader.uwFirmwareId, tHeader.ulImageSize, tHeader.uwCheckSum))
        {
            Boot_SendMultiData(NACK);
            return MW_OTA_FAIL;
        }
    }
    
    Boot_SendMultiData(ACK);
    return MW_OTA_OK;
}

/*************************************************************************
* FUNCTION:
*   MwOta_Boot_WritePatchImage
*
* DESCRIPTION:
*   write the image to flash
*
* PARAMETERS
*   none
*
* RETURNS
*   MW_OTA_OK   : successful
*   MW_OTA_FAIL : fail
*
*************************************************************************/
uint8_t MwOta_Boot_WritePatchImage_impl(void)
{
    uint32_t i = 0;
    UART_DATA_HEADER_t dataHeader;
    uint32_t dataAddr = 0;
    uint32_t dataLenth = 0;
    uint8_t data[PATCH_DATA_LEN] = {0};
    uint32_t dataChecksum = 0;
    uint32_t checksum = 0;
    uint8_t *pHead = dataHeader.head;

    Boot_SendMultiData(START);

    if(Boot_RecvMultiData((uint8_t *)&dataHeader, sizeof(UART_DATA_HEADER_t)) == 0) {

        if(strncmp((char *)pHead, "netlinkc", PATCH_HEAD_LEN) == 0) {

            dataHeader.number = Boot_SwapEndian(dataHeader.number);

            Boot_SendMultiData(ACK);

            for(i = 0; i < dataHeader.number; i++) {
                Boot_RecvMultiData((uint8_t *)&dataAddr, 4);
                Boot_RecvMultiData((uint8_t *)&dataLenth, 4);

                dataAddr  =  Boot_SwapEndian(dataAddr);
                dataLenth =  Boot_SwapEndian(dataLenth);

                if(dataLenth > 0 && dataLenth <= PATCH_DATA_LEN) {
                    Boot_RecvMultiData(data, dataLenth);
                    Boot_RecvMultiData((uint8_t *)&dataChecksum, 4);

                    dataChecksum =  Boot_SwapEndian(dataChecksum);

                    checksum = Boot_GetCheckSum(data, dataLenth);

                    if(checksum == dataChecksum) {
                        if (MW_OTA_OK == MwOta_DataIn(data, dataLenth)) {
                            Boot_SendMultiData(ACK);
                        } else {
                            Boot_SendMultiData(NACK);
                            return MW_OTA_FAIL;
                        }
                    } else {
                        printf("Invalid checksum %X %X size %d\r\n", dataChecksum, checksum, dataLenth);
                        Boot_SendMultiData(NACK);
                        return MW_OTA_FAIL;
                    }
                } else {
                    printf("Invalid data lenth %X\r\n", dataLenth);
                    Boot_SendMultiData(NACK);
                    return MW_OTA_FAIL;
                }
            }
        } else {
            printf("Invalid data head %s\r\n", pHead);
            Boot_SendMultiData(NACK);
            return MW_OTA_FAIL;
        }
    }
    
    return MW_OTA_OK;
}

/*************************************************************************
* FUNCTION:
*   MwOta_Boot_PreInitCold
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
void MwOta_Boot_PreInitCold(void)
{
    MwOta_Boot_Init = MwOta_Boot_Init_impl;
    MwOta_Boot_CheckUartBehavior = MwOta_Boot_CheckUartBehavior_impl;
    MwOta_Boot_LoadPatchImage = MwOta_Boot_LoadPatchImage_impl;
    
    MwOta_Boot_HeaderPaser = MwOta_Boot_HeaderPaser_impl;
    MwOta_Boot_WritePatchImage = MwOta_Boot_WritePatchImage_impl;
}
