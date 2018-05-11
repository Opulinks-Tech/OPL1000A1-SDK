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

#include "scrt_patch.h"

#if 0
bool nl_hmac_sha_1_v1(uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac)
{
    unsigned int data                   = 0;
    unsigned int c_output_token_id      = 0x9d37;
    unsigned int r_output_token_data    = 0;
    bool status = true;
    unsigned int word_6;
    int i;

    //Mailbox 1
    *(volatile unsigned int *)0x60003f00 = 0x4;

    //Polling -1
    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
        delay(1);
    }

    //Write Token to mailbox
    *(volatile unsigned int *)0x60000000 = 0x03009d37;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = in_data_len; //The length of the input data (in bytes).
    *(volatile unsigned int *)0x6000000c = (unsigned int) in_data;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = in_data_len;

    //*(volatile unsigned int *)0x60000018 = 0x00200001;
    word_6 = ((sk_len & 0xff) << 16) | ((0x1) << 0);
    *(volatile unsigned int *)0x60000018 = word_6;      //key length: 0x08  ;  [3:0] Algorithm  HMAC-SHA-1, 160-bit MAC, block size is 64 Bytes

    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = 0x00000000;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;
    *(volatile unsigned int *)0x6000002c = 0x00000000;
    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;
    *(volatile unsigned int *)0x60000044 = 0x00000000;
    *(volatile unsigned int *)0x60000048 = 0x00000000;
    *(volatile unsigned int *)0x6000004c = 0x00000000;
    *(volatile unsigned int *)0x60000050 = 0x00000000;
    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;

    //word 24
    *(volatile unsigned int *)0x60000060 = in_data_len;

    *(volatile unsigned int *)0x60000064 = 0x00000000;
    *(volatile unsigned int *)0x60000068 = 0x00000000;
    *(volatile unsigned int *)0x6000006c = 0x00000000;

    //msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, in_data_len:%d word_6:%08x \r\n", in_data_len, word_6);

    //Key
     for (i=0; i<sk_len; i++) {
         *(volatile unsigned int *)(0x60000070 + i) = *(sk + i);
     }

    //Write a word
    *(volatile unsigned int *)0x60003f00 = 0x1;

    //Polling - 2
    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
        delay(1);
    }

    //Check output token status
    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = false;
    }

    //Copy the output MAC data
    os_memcpy((void *)mac, (void *)0x60000008, 20);

    //Write
    *(volatile unsigned int *)0x60003f00 = 0x2;

    return status;
}
#endif

void scrt_patch_init(void)
{
    //Sample1: Replace the nl_hmac_sha_1 api, it's working
    //nl_hmac_sha_1 = nl_hmac_sha_1_v1;
}


