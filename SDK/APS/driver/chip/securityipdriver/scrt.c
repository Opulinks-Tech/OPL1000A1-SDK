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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "nl1000.h"
#include "basic_defs.h"
#include "msg.h"
#include "scrt.h"


/*
 * delay - Perform a delay.
 *
 * @param [in] times  The times to do __NOP
 *
 */
void delay(int times){
    int i;
    for(i=0; i<times; i++){
        __NOP();
    }
}


/*
 * nl_scrt_Init_impl - Perform the SCRT initialization.
 *
 * @return N/A
 *
 */
int nl_scrt_Init_impl(void)
{
    int rc;

	msg_print(LOG_HIGH_LEVEL, "[security] nl_scrt_Init \r\n");
    rc = vex_Init();
    return rc;
}


/*
 * nl_aes_ccm  - AES CCM Operation. [deprecated]
 *
 * @param [in] bEncrypt
 *    true:encrypt false:decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in] nonce
 *    Nonce
 *
 * @param [in] nonce_len
 *    Size of Nonce
 *
 * @param [in] adata
 *    Additional Associated Data
 *
 * @param [in] adata_len
 *    Size of Additional Associated Data
 *
 * @param [in/out] plain_text
 *    Plain Text Data
 *
 * @param [in/out] encrypted_text
 *    Encrypted Data
 *
 * @param [in] text_len
 *    The length of the Data Transfer
 *
 * @param [out] tag
 *    Output Tag Data
 *
 * @param [in] tag_len
 *    Size of the Output Tag Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 * @deprecated It will be removed in the future
 *
 */
bool nl_aes_ccm_impl (bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len)
{
    unsigned int token_id_output;
    unsigned int token_id_current = 0xd82c;
    unsigned int data = 0;
    unsigned int word_11;
    bool status = true;
    int i;
    int quo, rem;
    int buf_len;

    quo = (text_len / 16);
    rem = (text_len % 16);
    if(rem == 0) {
        buf_len = (quo * 16);
    } else {
        buf_len = ((quo + 1) * 16);
    }

    //msg_print(LOG_HIGH_LEVEL, "\r\n[scrt]quo:%d rem:%d buf_len:%d \r\n", quo, rem, buf_len);

    /*
         SCRT Control- Link Mailbox
       */
    *(volatile unsigned int *)0x60003f00 = 0x4;
    //*(volatile unsigned int *)0x60003f00 = 0x40;
    //*(volatile unsigned int *)0x60003f00 = 0x400;
    //*(volatile unsigned int *)0x60003f00 = 0x4000;

    if (bEncrypt)
    {
        /*
                SCRT Control- Check Mailbox Status
             */
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

        /* Burst write 37 words */
        *(volatile unsigned int *)0x60000000 = 0x0104d82c;
        *(volatile unsigned int *)0x60000004 = 0x00000000;
        *(volatile unsigned int *)0x60000008 = text_len;                            //The length of the data transfer   //0x10
        *(volatile unsigned int *)0x6000000c = (unsigned int) plain_text;           //addr_input_data;//plain_text;//(unsigned int) plain_text;           //Input Data                                 //0x00401000
        *(volatile unsigned int *)0x60000010 = 0x00000000;                          //Input Data
        *(volatile unsigned int *)0x60000014 = buf_len;                            //Input Data Length                     //0x10
        *(volatile unsigned int *)0x60000018 = (unsigned int) encrypted_text;       //Output Data                              //0x00401500
        *(volatile unsigned int *)0x6000001c = 0x00000000;                          //Output Data
        *(volatile unsigned int *)0x60000020 = (buf_len + 4);                      //Output Data Length                  //0x14
        *(volatile unsigned int *)0x60000024 = (unsigned int) adata;                //Associated Data                        //0x00402000
        *(volatile unsigned int *)0x60000028 = 0x00000000;                          //Associated Data
        //*(volatile unsigned int *)0x6000002c = 0x10d18050;                                                //0x10d18050;                           //Nonce length              => word 11[23:20]       => 13  => 0xd
                                                                                    //Key length             => word 11[19:16]    => 1 (1 means 128 bits key)
                                                                                    //Tag length             => word 11[28:24]    5 bits => 0x10  => 16
                                                                                    //Encrypt or Decrypt  => word11[15]         => 1 (1 means encrypt)

        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x80) << 8) | ((0x50) << 0);
        //msg_print(LOG_HIGH_LEVEL, "[scrt]text_len:%d word_11:%08x \r\n", text_len, word_11);
        *(volatile unsigned int *)0x6000002c = word_11;
        *(volatile unsigned int *)0x60000030 = 0x00000000;
        *(volatile unsigned int *)0x60000034 = 0x00000000;
        *(volatile unsigned int *)0x60000038 = 0x00000000;
        *(volatile unsigned int *)0x6000003c = 0x00000000;
        *(volatile unsigned int *)0x60000040 = 0x00000000;

        /* Key */
        for (i=0; i<sk_len; i++) {
            *(volatile unsigned int *)(0x60000044 + i) = *(sk + i);
        }

        *(volatile unsigned int *)0x60000054 = 0x00000000;          //key
        *(volatile unsigned int *)0x60000058 = 0x00000000;          //key
        *(volatile unsigned int *)0x6000005c = 0x00000000;          //key
        *(volatile unsigned int *)0x60000060 = 0x00000000;          //key
        *(volatile unsigned int *)0x60000064 = adata_len;           //Associated Data Length
        *(volatile unsigned int *)0x60000068 = 0x00000000;
        *(volatile unsigned int *)0x6000006c = 0x00000000;
        *(volatile unsigned int *)0x60000070 = 0x00000000;

        /* Nonce */
        for (i=0; i<nonce_len; i++) {
            *(volatile unsigned int *)(0x60000074 + i) = *(nonce + i);
        }

        *(volatile unsigned int *)0x60000084 = 0x00000000;
        *(volatile unsigned int *)0x60000088 = 0x00000000;
        *(volatile unsigned int *)0x6000008c = 0x00000000;
        *(volatile unsigned int *)0x60000090 = 0x00000000;

        /*
                SCRT Control- Write Token
             */
        *(volatile unsigned int *)0x60003f00 = 0x1;

        /*
                SCRT Control- Check Operation Status
             */
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

        /*
                SCRT Control- Check output token status
             */
        token_id_output = *((volatile unsigned int *)0x60000000);
        if (token_id_output != token_id_current) {
            status = false;
            //msg_print(LOG_HIGH_LEVEL, "[scrt]output token status fail \r\n");
        }

        /*
                SCRT Control- Check Token ID in output data's last word
             */
        token_id_output = *((volatile unsigned int *)(encrypted_text + buf_len));
        if (token_id_output != token_id_current) {
            status = false;
            //msg_print(LOG_HIGH_LEVEL, "[scrt]output data's last word's token id fail \r\n");
        }

        /*
                Copy the output tag data to the buffer "tag"
             */
        os_memcpy((void *)tag, (void *)0x60000018, tag_len);

        /*
                SCRT Control- Clear Status
             */
        *(volatile unsigned int *)0x60003f00 = 0x2;

        //msg_print(LOG_HIGH_LEVEL, "[scrt]encryption status:%s \r\n", (status==true)?"success":"fail");
    }
    else
    {
        /*
                SCRT Control- Check Mailbox Status
             */
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

        //Burst write 37 words
        *(volatile unsigned int *)0x60000000 = 0x0104d82c;
        *(volatile unsigned int *)0x60000004 = 0x00000000;
        *(volatile unsigned int *)0x60000008 = text_len;                            //The length of the data transfer           //0x10
        *(volatile unsigned int *)0x6000000c = (unsigned int) encrypted_text;       //addr_input_data;//plain_text;          //(unsigned int) plain_text;           //Input Data      //0x00401000
        *(volatile unsigned int *)0x60000010 = 0x00000000;                          //Input Data
        *(volatile unsigned int *)0x60000014 = buf_len;                            //Input Data Length                              //0x10
        *(volatile unsigned int *)0x60000018 = (unsigned int) plain_text;           //Output Data                                        //0x00401500
        *(volatile unsigned int *)0x6000001c = 0x00000000;                          //Output Data
        *(volatile unsigned int *)0x60000020 = (buf_len + 4);                      //Output Data Length                             //0x14
        *(volatile unsigned int *)0x60000024 = (unsigned int) adata;                //Associated Data                                   //0x00402000
        *(volatile unsigned int *)0x60000028 = 0x00000000;                          //Associated Data
        //*(volatile unsigned int *)0x6000002c = 0x10d10050;                                                //0x10d18050;                               //Nonce length              => word 11[23:20]       => 13  => 0xd
                                                                                    //Key length             => word 11[19:16]    => 1 (1 means 128 bits key)
                                                                                    //Tag length             => word 11[28:24]    5 bits => 0x10  => 16
                                                                                    //Encrypt or Decrypt  => word11[15]         => 0 (0 means decrypt)

        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x0) << 8) | ((0x50) << 0);
        //msg_print(LOG_HIGH_LEVEL, "[scrt]text_len:%d word_11:%08x \r\n", text_len, word_11);
        *(volatile unsigned int *)0x6000002c = word_11;

        *(volatile unsigned int *)0x60000030 = 0x00000000;
        *(volatile unsigned int *)0x60000034 = 0x00000000;
        *(volatile unsigned int *)0x60000038 = 0x00000000;
        *(volatile unsigned int *)0x6000003c = 0x00000000;
        *(volatile unsigned int *)0x60000040 = 0x00000000;

        //Key
        for (i=0; i<sk_len; i++) {
            *(volatile unsigned int *)(0x60000044 + i) = *(sk + i);
        }

        *(volatile unsigned int *)0x60000054 = 0x00000000;          //key
        *(volatile unsigned int *)0x60000058 = 0x00000000;          //key
        *(volatile unsigned int *)0x6000005c = 0x00000000;          //key
        *(volatile unsigned int *)0x60000060 = 0x00000000;          //key
        *(volatile unsigned int *)0x60000064 = adata_len;           //Associated Data Length
        *(volatile unsigned int *)0x60000068 = 0x00000000;
        *(volatile unsigned int *)0x6000006c = 0x00000000;
        *(volatile unsigned int *)0x60000070 = 0x00000000;

        //Nonce
        for (i=0; i<nonce_len; i++) {
            *(volatile unsigned int *)(0x60000074 + i) = *(nonce + i);
        }

        //Tag to verify
        for (i=0; i<tag_len; i++) {
            *(volatile unsigned int *)(0x60000084 + i) = *(tag + i);
        }

        /*
                SCRT Control- Write Token
            */
        *(volatile unsigned int *)0x60003f00 = 0x1;

        /*
                SCRT Control- Check Operation Status
            */
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
        token_id_output = *((volatile unsigned int *)0x60000000);
        if (token_id_output != token_id_current) {
            status = false;
            //msg_print(LOG_HIGH_LEVEL, "[scrt]output token status fail \r\n");
        }

        //Check Token ID in output data's last word
        token_id_output = *((volatile unsigned int *)(plain_text + buf_len)); //0x00401500 + 0x10
        if (token_id_output != token_id_current) {
            status = false;
            //msg_print(LOG_HIGH_LEVEL, "[scrt]output data's last word's token id fail \r\n");
        }

        /*
                SCRT Control- Clear Status
            */
        *(volatile unsigned int *)0x60003f00 = 0x2;

        //msg_print(LOG_HIGH_LEVEL, "[scrt]decryption status:%s \r\n", (status==true)?"success":"fail");
    }


func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}


/*
 * nl_scrt_aes_ccm - Perform the AES CCM Operation.
 *
 * @param [in] bEncrypt
 *    1:encrypt 0:decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in] nonce
 *    Nonce
 *
 * @param [in] nonce_len
 *    Size of Nonce
 *
 * @param [in] adata
 *    Additional Associated Data
 *
 * @param [in] adata_len
 *    Size of Additional Associated Data
 *
 * @param [in/out] plain_text
 *    Plain Text Data
 *
 * @param [in/out] encrypted_text
 *    Encrypted Data
 *
 * @param [in] text_len
 *    The length of the Data Transfer
 *
 * @param [out] tag
 *    Output Tag Data
 *
 * @param [in] tag_len
 *    Size of the Output Tag Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_aes_ccm_impl (int bEncrypt, unsigned char *sk, int sk_len, unsigned char *nonce, int nonce_len, unsigned char *adata, int adata_len, unsigned char *plain_text, unsigned char *encrypted_text, int text_len, unsigned char *tag, int tag_len)
{
    unsigned int token_id_output;
    unsigned int token_id_current = 0xd82c;
    unsigned int data = 0;
    unsigned int word_11;
    int status = 1;
    int i;
    int quo, rem;
    int buf_len;
    unsigned int adata_m;
    unsigned int plain_m;
    unsigned int cipher_m;
    int cnt = 0;

    quo = (text_len / 16);
    rem = (text_len % 16);
    if(rem == 0) {
        buf_len = (quo * 16);
    } else {
        buf_len = ((quo + 1) * 16);
    }

#if 0 //Remapping for m0 usage
    adata_m = (unsigned int)adata | 0x20000000;
    plain_m = (unsigned int)plain_text | 0x20000000;
    cipher_m = (unsigned int)encrypted_text | 0x20000000;
#else //For M3 usage
    adata_m = (unsigned int)adata;
    plain_m = (unsigned int)plain_text;
    cipher_m = (unsigned int)encrypted_text;
#endif

    //printf("\r\n sk_len:%d nonce_len:%d adata_len:%d text_len:%d tag_len:%d \r\n", sk_len, nonce_len, adata_len, text_len, tag_len);
    //printf("\r\n quo:%d rem:%d buf_len:%d \r\n", quo, rem, buf_len);
    //printf("\r\n adata_m:%lx plain_m:%lx cipher_m:%lx \r\n", adata_m, plain_m, cipher_m);

    /*
        SCRT Control- Link Mailbox
      */
    *(volatile unsigned int *)0x60003f00 = 0x4;

    /*
        SCRT Control- Check Mailbox Status
      */
    while(1){
        delay(1);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    /*Burst write 37 words*/
    *(volatile unsigned int *)0x60000000 = 0x0104d82c;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = text_len;                            //The length of the data transfer

    if (bEncrypt == 1) {
        *(volatile unsigned int *)0x6000000c = plain_m;                         //Input Data
    } else {
        *(volatile unsigned int *)0x6000000c = cipher_m;                        //Input Data
    }

    *(volatile unsigned int *)0x60000010 = 0x00000000;                          //Input Data
    *(volatile unsigned int *)0x60000014 = buf_len;                             //Input Data Length

    if (bEncrypt == 1) {
        *(volatile unsigned int *)0x60000018 = cipher_m;                       //Output Data
    } else {
        *(volatile unsigned int *)0x60000018 = plain_m;                        //Output Data
    }

    *(volatile unsigned int *)0x6000001c = 0x00000000;                          //Output Data
    *(volatile unsigned int *)0x60000020 = (buf_len + 4);                       //Output Data Length
    *(volatile unsigned int *)0x60000024 = adata_m;                            //Associated Data
    *(volatile unsigned int *)0x60000028 = 0x00000000;                          //Associated Data
    //*(volatile unsigned int *)0x6000002c = 0x10d18050;                                                //0x10d18050;           //Nonce length               => word 11[23:20]  => 13  => 0xd
                                                                                //Key length             => word 11[19:16]           => 1 (1 means 128 bits key)
                                                                                //Tag length             => word 11[28:24] 5 bits  => 0x10  => 16
                                                                                //Encrypt or Decrypt  => word11[15]                => 1:encrypt 0:decrypt

    if (bEncrypt == 1) {
        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x80) << 8) | ((0x50) << 0);
    } else {
        word_11 = ((tag_len & 0xff) << 24) | ((((nonce_len & 0xf) << 4) | (0x1)) << 16) | ((0x0) << 8) | ((0x50) << 0);
    }

    //printf("\r\n text_len:%d word_11:%08x \r\n", text_len, word_11);

    *(volatile unsigned int *)0x6000002c = word_11;
    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;

    memcpy((void *)0x60000044, sk, sk_len);

    *(volatile unsigned int *)0x60000054 = 0x00000000;          //key
    *(volatile unsigned int *)0x60000058 = 0x00000000;          //key
    *(volatile unsigned int *)0x6000005c = 0x00000000;          //key
    *(volatile unsigned int *)0x60000060 = 0x00000000;          //key
    *(volatile unsigned int *)0x60000064 = adata_len;           //Associated Data Length
    *(volatile unsigned int *)0x60000068 = 0x00000000;
    *(volatile unsigned int *)0x6000006c = 0x00000000;
    *(volatile unsigned int *)0x60000070 = 0x00000000;

    memcpy((void *)0x60000074, nonce, nonce_len);

    if (bEncrypt == 1) {
        *(volatile unsigned int *)0x60000084 = 0x00000000;
        *(volatile unsigned int *)0x60000088 = 0x00000000;
        *(volatile unsigned int *)0x6000008c = 0x00000000;
        *(volatile unsigned int *)0x60000090 = 0x00000000;
    } else {
        memcpy((void *)0x60000084, tag, tag_len);
    }

    /*
            SCRT Control- Write Token
        */
    *(volatile unsigned int *)0x60003f00 = 0x1;

    /*
            SCRT Control- Check Operation Status
        */
    while(1){
        cnt++;
        delay(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
    }

    /*
        SCRT Control- Check output token status
       */
    token_id_output = *((volatile unsigned int *)0x60000000);
    if (token_id_output != token_id_current) {
        status = 0;
        //printf("\r\n output token status fail \r\n");
    }

    /*
        SCRT Control- Check Token ID in output data's last word
       */
    if (bEncrypt == 1) {
        token_id_output = *((volatile unsigned int *)(encrypted_text + buf_len));
    } else {
        token_id_output = *((volatile unsigned int *)(plain_text + buf_len));
    }

    if (token_id_output != token_id_current) {
        status = 0;
        //printf("\r\n output data's last word's token id fail \r\n");
    }

    if (bEncrypt == 1) {
        /* Copy the output tag data to the buffer "tag" */
        memcpy((void *)tag, (void *)0x60000018, tag_len);
    }

    /*
            SCRT Control- Clear Status
       */
    *(volatile unsigned int *)0x60003f00 = 0x2;

    //printf("[scrt]encryption status:%s cnt:%d \r\n", (status==1)?"success":"fail", cnt);
    return status;

func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}


/*
 * aes_ccm_sample - Sample code for AES CCM output 4 bytes tag
 *
 */
void aes_ccm_sample(void)
{
    unsigned char sk[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    unsigned char nonce[13] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d};
    unsigned char a_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    unsigned char plain_text[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    unsigned char cipher_text[16] = {0};
    unsigned char tag[4] = {0};
    unsigned char plain_output[16] = {0};

    int i;

    printf("\r\n do aes ccm encryption \r\n");

    memset(cipher_text, 0, 16);
    memset(tag, 0, 8);
    memset(plain_output, 0, 16);

    //AES CCM Encryption
    nl_scrt_aes_ccm(1, sk, sizeof(sk), nonce, sizeof(nonce), a_data, sizeof(a_data), plain_text, cipher_text, sizeof(plain_text), tag, sizeof(tag));

    //Show output tag data
    for(i=0; i < sizeof(tag) ;i++) {
        printf("output tag[%d]:%02x \r\n", i, tag[i]);
    }

    //Show output cipher data (size of cipher data is the same as size of plain text)
    for(i=0; i < sizeof(cipher_text); i++) {
        printf("output cipher[%d]:%02x \r\n", i, *((volatile char *)(cipher_text + i)));
    }

    printf("\r\n do aes ccm decryption \r\n");

    //AES CCM Decryption
    nl_scrt_aes_ccm(0, sk, sizeof(sk), nonce, sizeof(nonce), a_data, sizeof(a_data), plain_output, cipher_text, sizeof(cipher_text), tag, sizeof(tag));

    //Show output plain text data
    for(i=0; i<sizeof(plain_text); i++) {
       printf("output plain text[%d]:%02x \r\n", i, plain_output[i]);
    }
}


/*
 * nl_hmac_sha_1_impl - Perform the HMAC SHA1 Operation
 *
 * @param [in] sk
 *      The Key
 *
 * @param [in] sk_len
 *      The Key Length
 *
 * @param [in] in_data
 *      The Input Data which to be handled
 *
 * @param [in] in_data_len
 *      The length of Input Data
 *
 * @param [out] mac
 *      Output MAC Data
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
bool nl_hmac_sha_1_impl(uint8_t *sk, int sk_len, uint8_t *in_data, int in_data_len, uint8_t *mac)
{
    unsigned int data                   = 0;
    unsigned int c_output_token_id      = 0x9d37;
    unsigned int r_output_token_data    = 0;
    bool status = true;
    unsigned int word_6;
    int i;
    int cnt = 0;

    /*
        SCRT Control- Link Mailbox
      */
    *(volatile unsigned int *)0x60003f00 = 0x4;

    /*
        SCRT Control- Check Mailbox Status
      */
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

    /* Write Token to mailbox */
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

    /* Key */
     for (i=0; i<sk_len; i++) {
         *(volatile unsigned int *)(0x60000070 + i) = *(sk + i);
     }

    /* Write a word */
    *(volatile unsigned int *)0x60003f00 = 0x1;

    /*
            SCRT Control- Check Operation Status
        */
    while(1){
        cnt++;
        delay(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
    }

    /*
        SCRT Control- Check output token status
       */
    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = false;
        msg_print(LOG_HIGH_LEVEL, "[scrt] nl_hmac_sha_1, output token status fail \r\n");
    }

    /* Copy the output MAC data */
    os_memcpy((void *)mac, (void *)0x60000008, 20);

    /*
            SCRT Control- Clear Status
       */
    *(volatile unsigned int *)0x60003f00 = 0x2;

    return status;
func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}


/*
 * hmac_sha_1_sample - Sample code to do HMAC SHA1
 *
 */
void hmac_sha_1_sample(void)
{
    uint8_t passphrase[8] = {0x32, 0x32, 0x32, 0x32,
                             0x32, 0x32, 0x32, 0x32};
    uint8_t data[20] = {0xad, 0x82, 0x16, 0xc9,
                        0x63, 0x59, 0x33, 0xfd,
                        0x82, 0xa7, 0x6b, 0xad,
                        0x69, 0xe4, 0x51, 0x81,
                        0xa1, 0x18, 0x37, 0xb7
                        };
    uint8_t mac[20] = {0};
    int i;

    msg_print(LOG_HIGH_LEVEL, "\r\nhmac_sha_1_sample\r\n");
    os_memset(mac, 0, sizeof(mac));

    nl_hmac_sha_1(passphrase, sizeof(passphrase), data, sizeof(data), mac);

    /* Show output MAC */
    for(i=0; i < sizeof(mac) ;i++) {
        msg_print(LOG_HIGH_LEVEL, "hmac_sha_1_sample, output mac[%d]:%02x \r\n", i, mac[i]);
    }
}


/*
 * nl_scrt_aes_ecb - Perform the AES ECB Operation.
 *
 * @param [in] bEncrypt
 *    1:Encrypt
 *    0:Decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in/out] data_in
 *    Input Data
 *
 * @param [out] data_out
 *    Output Data
 *
 * @param [in] data_len
 *    The length of the Data Transfer
 *
 * @return 1 success
 *
 * @return 0 fail
 *
 */
int nl_scrt_aes_ecb_impl(int bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len)
{
    unsigned int data;
    unsigned int r_output_token_data;
    unsigned int c_output_token_id = 0xd82c;
    int status = 1;
    int i;
    unsigned int word_11;
    int quo, rem;
    int buf_len;
    int key_len = 1;
    int fieldEncrypt;
    unsigned int sk_m;
    unsigned int data_in_m;
    unsigned int data_out_m;
    unsigned char w_key[4] = {0};
    int cnt = 0;

    //printf("\r\n nl_scrt_aes_ecb, sk:%lx data_in:%lx data_out:%lx \r\n", sk, data_in, data_out); //sk:602b28 data_in:602b38 data_out:602b18

#if 0
    //Remapping
    sk_m = (unsigned int)sk | 0x20000000;
    data_in_m = (unsigned int)data_in | 0x20000000;
    data_out_m = (unsigned int)data_out | 0x20000000;
#else
    sk_m = (unsigned int)sk;
    data_in_m = (unsigned int)data_in;
    data_out_m = (unsigned int)data_out;
#endif

    if(sk == 0||
       data_in == 0 ||
       data_out == 0){
        return 0;
    }

    //printf("\r\n nl_scrt_aes_ecb, sk_m:%lx \r\n", sk_m);
    //printf("\r\n nl_scrt_aes_ecb, data_in_m:%lx \r\n", data_in_m);
    //printf("\r\n nl_scrt_aes_ecb, data_out_m:%lx \r\n", data_out_m);

    if(sk_len != 16 &&
       sk_len != 24 &&
       sk_len != 32) {
        return 0;
    }

    if(data_len <= 0) {
        return 0;
    }

    if(bEncrypt == 1) {
        fieldEncrypt = 1;
    } else {
        fieldEncrypt = 0;
    }

    switch (sk_len)
    {
        case 16:
            key_len = 1;
            break;
        case 24:
            key_len = 2;
            break;
        case 32:
            key_len = 3;
            break;
    }

    //It should be a multiple of block size (16 bytes)
    quo = (data_len / 16);
    rem = (data_len % 16);
    if(rem == 0) {
        buf_len = (quo * 16);
    } else {
        buf_len = ((quo + 1) * 16);
    }

    *(volatile unsigned int *)0x60003f00 = 0x4;

    //printf("\r\n nl_scrt_aes_ecb, 444, fieldEncrypt:%d key_len:%d buf_len:%d \r\n", fieldEncrypt, key_len, buf_len);

    while(1){
        delay(3);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    //printf("\r\n nl_scrt_aes_ecb, 555 \r\n");

    *(volatile unsigned int *)0x60000000 = 0x0104d82c;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = buf_len;
    *(volatile unsigned int *)0x6000000c = data_in_m;//(unsigned int) data_in;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = buf_len;
    *(volatile unsigned int *)0x60000018 = data_out_m;//(unsigned int) data_out;
    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = buf_len + 4;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;

    word_11 = ((key_len << 16) | (fieldEncrypt << 15) | (0x0));
    *(volatile unsigned int *)0x6000002c = word_11;
    //printf("\r\n word_11:0x%lx \r\n", word_11);

    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;

    memcpy((void *)0x60000044, (void *)sk_m, 16);

    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;
    *(volatile unsigned int *)0x60000060 = 0x00000000;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    while(1){
        cnt++;
        delay(INTERVAL_EACH_CHECKING);
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
        if (cnt > MAX_TIMES_CHECKING)
        {
            status = 0;
            goto func_return;
        }
    }

    //printf("\r\n nl_scrt_aes_ecb, 777 \r\n");

    r_output_token_data = *((volatile unsigned int *)0x60000000);
    if (r_output_token_data != c_output_token_id) {
        status = 0;
        //printf("\r\n nl_scrt_aes_ecb, 888 \r\n");
    }

    r_output_token_data = *((volatile unsigned int *) (data_out + buf_len));
    if ( r_output_token_data != c_output_token_id )
    {
       status = 0;
       //printf("\r\n nl_scrt_aes_ecb, 999 \r\n");
    }

    *(volatile unsigned int *)0x60003f00 = 0x2;
    //printf("\r\n nl_scrt_aes_ecb, ret:%d \r\n", ret);

    return status;

func_return:
    /* Unlink MailBox */
    *(volatile unsigned int *)0x60003f00 = 0x8;

    return status;
}


/*
 * aes_ecb_sample - Sample code for AES ECB
 *
 */
void aes_ecb_sample(void)
{
    int i;
    unsigned char sk[16] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10};
    //unsigned char sk[24] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    //unsigned char sk[36] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    unsigned char data_in[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x60, 0x50};
    unsigned char data_out[16] = {0};
    unsigned char data_out2[16] = {0};

    printf("\r\n aes_ecb_sample \r\n");

    /* AEC ECB Encryption */
    memset(data_out, 0, 16);
    nl_scrt_aes_ecb(1, sk, sizeof(sk), data_in, data_out, sizeof(data_in));
    for(i=0; i < sizeof(data_in); i++) {
        printf( "output data_out[%d]:%02x \r\n", i, data_out[i]);
    }

    /* AES ECB Decryption */
    memset(data_out2, 0, 16);
    nl_scrt_aes_ecb(0, sk, sizeof(sk), data_out, data_out2, sizeof(data_out));
    for(i=0; i < sizeof(data_out); i++) {
        printf( "output data_out2[%d]:%02x \r\n", i, data_out2[i]);
    }
}


RET_DATA nl_scrt_Init_fp_t nl_scrt_Init;
RET_DATA nl_aes_ccm_fp_t nl_aes_ccm;
RET_DATA nl_scrt_aes_ccm_fp_t nl_scrt_aes_ccm;
RET_DATA nl_hmac_sha_1_fp_t nl_hmac_sha_1;
RET_DATA nl_scrt_aes_ecb_fp_t nl_scrt_aes_ecb;


/*
 * scrt_drv_func_init - Interface Initialization: SCRT
 *
 */
void scrt_drv_func_init(void)
{
    nl_scrt_Init = nl_scrt_Init_impl;
    nl_aes_ccm = nl_aes_ccm_impl;
    nl_scrt_aes_ccm = nl_scrt_aes_ccm_impl;
    nl_hmac_sha_1 = nl_hmac_sha_1_impl;
    nl_scrt_aes_ecb = nl_scrt_aes_ecb_impl;
}


#if 0
#include "valtest_internal.h"
#include "testvectors_aes_basic.h"
#include "testvectors_des.h"
#include "testvectors_xts_aes.h"
#include "testvectors_aes_f8.h"
#include "sfzutf.h"
#include "valtest_sym_crypto.h"
#include "valtest_random.h"
#include "valtest_hash.h"
#include "valtest_cipher_mac.h"
#include "api_val_buffers.h"
#include "api_val_random.h"
#endif

#if 0
/*----------------------------------------------------------------------------
 * do_SymCrypt
 *
 * Perform a test with the 'val_SymCipher' API using the data and
 * options from the given "SymCryptTestCtx_t" test context.
 */
int
do_SymCrypt(
        SymCryptCtx_t * const TestCntxt_p,
        bool fDecrypt)
{
    static uint8_t result_text[SYM_CRYPT_MAX_BUFLEN];
    static uint8_t input_text[SYM_CRYPT_MAX_BUFLEN];

    ValSize_t result_len = sizeof(result_text);
    ValStatus_t Status;
    uint8_t * TxtIn_p;
    uint8_t * TxtOut_p;
    uint8_t * Result_p = result_text;
    uint32_t padding = 0;

    if (fDecrypt)
    {//Decrypt
        TxtIn_p = TestCntxt_p->ctx_p;
        TxtOut_p = TestCntxt_p->ptx_p;
    }
    else
    {//Encrypt
        Status = val_SymCipherInitEncrypt(TestCntxt_p->SymContext_p);
        //fail_if(Status != VAL_SUCCESS, "val_SymCipherInitEncrypt()=", Status);

        TxtIn_p = TestCntxt_p->ptx_p;
        TxtOut_p = TestCntxt_p->ctx_p;
    }

    if (TestCntxt_p->options & SYM_CRYPT_IN_PLACE)
    {
        memset(Result_p, 0xDC, result_len);
        memcpy(Result_p, TxtIn_p, TestCntxt_p->txt_len);
        TxtIn_p = Result_p;
    }

    if (TestCntxt_p->options & SYM_CRYPT_UNALIGNED)
    {
        int offset = 1 + (TestCntxt_p->misalign % 3);

        memset(input_text, 0xDC, sizeof(input_text));
        memcpy(input_text + offset, TxtIn_p, TestCntxt_p->txt_len);
        TxtIn_p = input_text + offset;
        Result_p += 1 + ((offset + 1) % 3);
        result_len -= 1 + ((offset + 1) % 3);
        TestCntxt_p->misalign += 1;
    }

    if (TestCntxt_p->txt_len & (TestCntxt_p->BlockSize - 1))
    {
        //fail_if(TestCntxt_p->fPadding == false, "Bad length for ECB/CBC ", TestCntxt_p->txt_len);
        msg_print(LOG_HIGH_LEVEL, "[security] do_SymCrypt, Bad length for ECB/CBC \r\n");

        padding = (0 - TestCntxt_p->txt_len) & (TestCntxt_p->BlockSize - 1);
        if (!(TestCntxt_p->options & SYM_CRYPT_UNALIGNED))
        {
            memset(input_text, TestCntxt_p->PaddingByte, (TestCntxt_p->txt_len + padding));
            memcpy(input_text, TxtIn_p, TestCntxt_p->txt_len);
            TxtIn_p = input_text;
        }
    }

    if (TestCntxt_p->options & SYM_CRYPT_MULTIPART)
    {
        ValSize_t src_len = TestCntxt_p->txt_len + padding;
        ValSize_t dst_space = result_len;
        uint8_t * srcpart_p = TxtIn_p;
        uint8_t * dstpart_p = Result_p;

        do
        {
            ValSize_t part_len = MIN(TestCntxt_p->BlockSize, src_len);
            ValSize_t dst_len = dst_space;

            if (src_len == part_len)
            {
                uint8_t * InCopy_p;
                uint8_t * OutCopy_p;

                InCopy_p = (uint8_t *)SFZUTF_MALLOC(part_len);
                //fail_if(InCopy_p == NULL, "Allocation ", (int)part_len);
                if (InCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCrypt, InCopy_p == NULL \r\n");
                }

                OutCopy_p = (uint8_t *)SFZUTF_MALLOC(dst_len);
                //fail_if(OutCopy_p == NULL, "Allocation ", (int)dst_len);
                if (OutCopy_p == NULL) {
                    msg_print(LOG_HIGH_LEVEL, "[security] do_SymCrypt, OutCopy_p == NULL \r\n");
                }

                memcpy(InCopy_p, srcpart_p, part_len);
                Status = val_SymCipherFinal(TestCntxt_p->SymContext_p,
                                            InCopy_p, part_len,
                                            OutCopy_p, &dst_len);
                if (Status == VAL_SUCCESS)
                {
                    memcpy(dstpart_p, OutCopy_p, dst_len);
                }

                SFZUTF_FREE(OutCopy_p);
                SFZUTF_FREE(InCopy_p);

                //fail_if(Status != VAL_SUCCESS, "val_SymCipherFinal()=", Status);
            }
            else
            {
                uint8_t * InCopy_p;
                uint8_t * OutCopy_p;

                InCopy_p = (uint8_t *)SFZUTF_MALLOC(part_len);
                //fail_if(InCopy_p == NULL, "Allocation ", (int)part_len);
                OutCopy_p = (uint8_t *)SFZUTF_MALLOC(dst_len);
                //fail_if(OutCopy_p == NULL, "Allocation ", (int)dst_len);

                memcpy(InCopy_p, srcpart_p, part_len);
                Status = val_SymCipherUpdate(TestCntxt_p->SymContext_p,
                                             InCopy_p, part_len,
                                             OutCopy_p, &dst_len);
                if (Status == VAL_SUCCESS)
                {
                    memcpy(dstpart_p, OutCopy_p, dst_len);
                }

                SFZUTF_FREE(OutCopy_p);
                SFZUTF_FREE(InCopy_p);

                //fail_if(Status != VAL_SUCCESS, "val_SymCipherUpdate()=", Status);
            }

            //fail_if(dst_len != TestCntxt_p->BlockSize,"Unexpected partial result length ",(int)dst_len);

            src_len -= part_len;
            dst_space -= dst_len;
            srcpart_p += part_len;
            dstpart_p += part_len;
        } while (src_len > 0);

        /* Let result_len = accumulated result length. */
        result_len -= dst_space;
    }
    else
    {
		{
	        uint8_t * InCopy_p;
	        uint8_t * OutCopy_p;

	        InCopy_p = (uint8_t *)SFZUTF_MALLOC(TestCntxt_p->txt_len + padding);
	        OutCopy_p = (uint8_t *)SFZUTF_MALLOC(result_len);

	        memcpy(InCopy_p, TxtIn_p, (TestCntxt_p->txt_len + padding));
	        Status = val_SymCipherFinal(TestCntxt_p->SymContext_p,
	                                    InCopy_p, (TestCntxt_p->txt_len + padding),
	                                    OutCopy_p, &result_len);
	        if (Status == VAL_SUCCESS)
	        {
	            memcpy(Result_p, OutCopy_p, result_len);
	        }

	        SFZUTF_FREE(OutCopy_p);
	        SFZUTF_FREE(InCopy_p);

		}
    }

    //fail_if(result_len != (ValSize_t)(TestCntxt_p->txt_len + padding),"Unexpected result length: ", (int)result_len);
    //fail_if(memcmp(Result_p, TxtOut_p, TestCntxt_p->txt_len) != 0,"Unexpected output result ", -1);


	/*
    if (memcmp(Result_p, TxtOut_p, TestCntxt_p->txt_len) != 0) {

	} else {

	}*/

    return END_TEST_SUCCES;
}
#endif

#if 0
/*
 * nl_se_SetupContextAndEnvironment
 *
 * @param [in] PubKey_p
 *     Pointer to the asymmetric crypto key structure of the public key for ECDH.
 *
 * @param [in] PrivKey_p
 *     Pointer to the asymmetric crypto key structure of the private key for ECDH.
 *
 */
int
nl_se_SetupContextAndEnvironment(
        SymCryptCtx_t * TestCntxt_p,
        ValSymAlgo_t Algorithm,
        TestVectors_Mode_t TestMode,
        const uint8_t * const Key_p,
        uint32_t KeySize,
        const uint8_t * const Iv_p,
        uint8_t * Ptx_p,
        uint8_t * Ctx_p,
        const uint32_t TxtLen)
{
    ValSymMode_t Mode = VAL_SYM_MODE_NONE;
    ValStatus_t Status;
    uint32_t IvSize = 0;

    memset(TestCntxt_p, 0, sizeof(SymCryptCtx_t));

    switch (Algorithm)
    {
    case VAL_SYM_ALGO_CIPHER_AES:
        switch (TestMode)
        {
	        case TESTVECTORS_MODE_ECB:
	            Mode = VAL_SYM_MODE_CIPHER_ECB;
	            break;
	        case TESTVECTORS_MODE_CBC:
	            Mode = VAL_SYM_MODE_CIPHER_CBC;
	            IvSize = VAL_SYM_ALGO_AES_IV_SIZE;
	            break;
	        default:
	            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_AES_BLOCK_SIZE;
        break;
#if 0
    case VAL_SYM_ALGO_CIPHER_DES:
        // Note: No key Asset is allowed
        switch (TestMode)
        {
        case TESTVECTORS_MODE_ECB:
            Mode = VAL_SYM_MODE_CIPHER_ECB;
            break;
        case TESTVECTORS_MODE_CBC:
            Mode = VAL_SYM_MODE_CIPHER_CBC;
            IvSize = VAL_SYM_ALGO_DES_IV_SIZE;
            break;
        default:
            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_DES_BLOCK_SIZE;
        break;
    case VAL_SYM_ALGO_CIPHER_TRIPLE_DES:
        switch (TestMode)
        {
        case TESTVECTORS_MODE_ECB:
            Mode = VAL_SYM_MODE_CIPHER_ECB;
            break;
        case TESTVECTORS_MODE_CBC:
            Mode = VAL_SYM_MODE_CIPHER_CBC;
            IvSize = VAL_SYM_ALGO_DES_IV_SIZE;
            break;
        default:
            break;
        }
        TestCntxt_p->BlockSize = VAL_SYM_ALGO_DES_BLOCK_SIZE;
        break;
#endif

    default:
        break;
    }

    Status = val_SymAlloc(Algorithm, Mode, &TestCntxt_p->SymContext_p);
	if(Status == VAL_INVALID_ALGORITHM) {

	}

    Status = val_SymInitKey(TestCntxt_p->SymContext_p,
                            TestCntxt_p->KeyAssetId,
                            Key_p,
                            KeySize);

    if (IvSize)
    {
        Status = val_SymInitIV(TestCntxt_p->SymContext_p,
                               Iv_p, IvSize);
    }

    //TestCntxt_p->Index = Index;
    //TestCntxt_p->ptx_p = sfzutf_discard_const(Ptx_p);
    //TestCntxt_p->ctx_p = sfzutf_discard_const(Ctx_p);
    TestCntxt_p->ptx_p = Ptx_p;
    TestCntxt_p->ctx_p = Ctx_p;
    TestCntxt_p->txt_len = TxtLen;

    return END_TEST_SUCCES;
}
#endif

#if 0
/*
 * nl_scrt_aes_cbc
 *
 * AES CBC Encryption/Decryption.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] fDecrypt
 *    Encrypt or Decrypt.
 *
 * @param [in] key
 *    Encryption key.
 *
 * @param [in] keySize.
 *    Key size.
 *
 * @param [in] iv
 *    Encryption IV for CBC mode.
 *
 * @param [in] Ptx_p
 *   Point to the Plain Text Data.
 *
 * @param [in] TxtLen
 *    The length of the Plain Text Data.
 *
 * @param [in] Ctx_p
 *    Point to the Output Encrypted data buffer.
 *
 */
int
nl_scrt_aes_cbc(Nl_Scrt_Usr_t user, bool fDecrypt, const uint8_t *key, uint32_t keySize, const uint8_t *iv, uint8_t *Ptx_p, size_t TxtLen, uint8_t *Ctx_p)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    //Link Mailbox here

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CBC,
                                       	key,
                                       	(keySize/8),
                                       	iv,
                                       	Ptx_p,
                                       	Ctx_p,
                                       	(uint32_t)TxtLen) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, fDecrypt) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
/*
 * nl_scrt_aes_ecb
 *
 * AES ECB Encryption/Decryption.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] fDecrypt
 *    Encrypt or Decrypt.
 *
 * @param [in] key
 *    Encryption key.
 *
 * @param [in] keySize
 *    Key size.
 *
 * @param [in] iv
 *    Encryption IV for ECB mode
 *
 * @param [in] Ptx_p
 *    Pointer to the Plain Text Data.
 *
 * @param [in] TxtLen
 *    The length of the Plain Text Data.
 *
 * @param [in] Ctx_p
 *    Pointer to the Output Encrypted data buffer.
 *
 */
int
nl_scrt_aes_ecb(Nl_Scrt_Usr_t user, bool fDecrypt, const uint8_t *key, uint32_t keySize, const uint8_t *iv, uint8_t *Ptx_p, size_t TxtLen, uint8_t *Ctx_p)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    //Link Mailbox here

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_ECB,
                                       	key,
                                       	(keySize/8),
                                       	NULL,
                                       	Ptx_p,
                                       	Ctx_p,
                                       	(uint32_t)TxtLen) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, fDecrypt) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
/*
 * nl_scrt_aes_ctr
 *
 * AES CBC Encryption/Decryption.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] fDecrypt
 *    Encrypt or Decrypt.
 *
 * @param [in] key
 *    Encryption key.
 *
 * @param [in] keySize.
 *    Key size.
 *
 * @param [in] iv
 *    Encryption IV for CBC mode.
 *
 * @param [in] Ptx_p
 *   Point to the Plain Text Data.
 *
 * @param [in] TxtLen
 *    The length of the Plain Text Data.
 *
 * @param [in] Ctx_p
 *    Point to the Output Encrypted data buffer.
 *
 */
int
nl_scrt_aes_ctr(Nl_Scrt_Usr_t user, bool fDecrypt, const uint8_t *key, uint32_t keySize, const uint8_t *iv, uint8_t *Ptx_p, size_t TxtLen, uint8_t *Ctx_p)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    //Link Mailbox here

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CTR,
                                       	key,
                                       	(keySize/8),
                                       	iv,
                                       	Ptx_p,
                                       	Ctx_p,
                                       	(uint32_t)TxtLen) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, fDecrypt) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
/*
 * nl_scrt_aes_cmac
 *
 * AES-CMAC Message Authentication Code operation.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] Key_p
 *    Pointer to the key structure of the key.
 *
 * @param [in] KeySize
 *    Size of the key.
 *
 * @param [in] Msg_p
 *    Pointer to the message data.
 *
 * @param [in] MsgLen
 *    Size of the message data.
 *
 * @param [out] Mac_p
 *    Pointer to the output MAC data.
 *
 * @param [in] MacLen
 *    Size of the MAC data.
 *
 */
int
nl_scrt_aes_cmac(Nl_Scrt_Usr_t user, uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen)
{
    //Link Mailbox here

	return do_CMAC_Gen(Key_p, KeySize, Msg_p, MsgLen, Mac_p, MacLen);
}
#endif

#if 0
/*
 * nl_scrt_aes_cbcmac
 *
 * AES-CBC-MAC Message Authentication Code operation.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] Key_p
 *    Pointer to the key structure of the key.
 *
 * @param [in] KeySize
 *    Size of the key.
 *
 * @param [in] Msg_p
 *    Pointer to the message data.
 *
 * @param [in] MsgLen
 *    Size of the message data.
 *
 * @param [out] Mac_p
 *    Pointer to the output MAC data.
 *
 * @param [in] MacLen
 *    Size of the MAC data.
 *
 * @param [in] Iv_p
 *    Encryption IV for CBC mode.
 *
 */
int
nl_scrt_aes_cbcmac(Nl_Scrt_Usr_t user, uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen, uint8_t * Iv_p)
{
    //Link Mailbox here

	return do_CBCMAC_Gen(Key_p, KeySize, Msg_p, MsgLen, Mac_p, MacLen, Iv_p);
}
#endif

#if 0
/*
 * nl_scrt_trng_gen
 *
 * True Random Number Generation.
 *
 * @param [in] user
 *    User Type.
 *
 * @param [in] DataSize
 *    Size of the Random Number.
 *
 * @param [out] Data_p
 *    Pointer to the output Random Number buffer.
 *
 */
ValStatus_t
nl_scrt_trng_gen(Nl_Scrt_Usr_t user, const ValSize_t DataSize, ValOctetsOut_t * const  Data_p)
{
    //Link Mailbox here

	return val_RandomData(DataSize, Data_p);
}
#endif

#if 0
void nl_se_sample_test(){
	//Crypto
	test_sym_crypto_common();
	test_sym_crypto_aes();

	//MAC
	test_aes_cmac();
	test_aes_cbcmac();

    //Random Number
	//Fail, status is not change, #TBC
	//test_rand();
}
#endif

#if 0
/*
 * nl_scrt_aes_ecb
 *
 * AES ECB Operation.
 *
 * @param [in] bEncrypt
 *    true:Encrypt
 *    false:Decrypt
 *
 * @param [in] sk
 *    Key
 *
 * @param [in] sk_len
 *    Size of Key
 *
 * @param [in/out] data_in
 *    Input Data
 *
 * @param [out] data_out
 *    Output Data
 *
 * @param [in] data_len
 *    The length of the Data Transfer
 *
 */
bool nl_scrt_aes_ecb(bool bEncrypt, unsigned char *sk, int sk_len, unsigned char *data_in, unsigned char *data_out, unsigned data_len)
{
    unsigned int data;
    unsigned int r_output_token_data;
    unsigned int c_output_token_id = 0xd82c;
    bool ret = true;
    int i;
    unsigned int word_11;
    int quo, rem;
    int buf_len;
    int key_len = 1;
    int fieldEncrypt;

    if(sk == NULL ||
       data_in == NULL ||
       data_out == NULL){
        return false;
    }

    if(sk_len != 16 &&
       sk_len != 24 &&
       sk_len != 32) {
        return false;
    }

    if(data_len <= 0) {
        return false;
    }

    if(bEncrypt) {
        fieldEncrypt = 1;
    } else {
        fieldEncrypt = 0;
    }

    switch (sk_len)
    {
        case 16:
            key_len = 1;
            break;
        case 24:
            key_len = 2;
            break;
        case 32:
            key_len = 3;
            break;
    }

    //It should be a multiple of block size (16 bytes)
    quo = (data_len / 16);
    rem = (data_len % 16);
    if(rem == 0) {
        buf_len = (quo * 16);
    } else {
        buf_len = ((quo + 1) * 16);
    }

    *(volatile unsigned int *)0x60003f00 = 0x4;

    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x1 ) == 0x0) {
            //success
            break;
        } else {
            continue;
        }
    }

    *(volatile unsigned int *)0x60000000 = 0x0104d82c;
    *(volatile unsigned int *)0x60000004 = 0x00000000;
    *(volatile unsigned int *)0x60000008 = buf_len;
    *(volatile unsigned int *)0x6000000c = (unsigned int) data_in;
    *(volatile unsigned int *)0x60000010 = 0x00000000;
    *(volatile unsigned int *)0x60000014 = buf_len;
    *(volatile unsigned int *)0x60000018 = (unsigned int) data_out;
    *(volatile unsigned int *)0x6000001c = 0x00000000;
    *(volatile unsigned int *)0x60000020 = buf_len + 4;
    *(volatile unsigned int *)0x60000024 = 0x00000000;
    *(volatile unsigned int *)0x60000028 = 0x00000000;

    word_11 = ((key_len << 16) | (fieldEncrypt << 15) | (0x0));
    *(volatile unsigned int *)0x6000002c = word_11;
    //printf("\r\n [nl_aes_ecb] word_11:%lx \r\n", word_11);

    *(volatile unsigned int *)0x60000030 = 0x00000000;
    *(volatile unsigned int *)0x60000034 = 0x00000000;
    *(volatile unsigned int *)0x60000038 = 0x00000000;
    *(volatile unsigned int *)0x6000003c = 0x00000000;
    *(volatile unsigned int *)0x60000040 = 0x00000000;

    for (i=0; i<sk_len; i++) {
        *(volatile unsigned int *)(0x60000044 + i) = *(sk + i);
    }

    *(volatile unsigned int *)0x60000054 = 0x00000000;
    *(volatile unsigned int *)0x60000058 = 0x00000000;
    *(volatile unsigned int *)0x6000005c = 0x00000000;
    *(volatile unsigned int *)0x60000060 = 0x00000000;

    *(volatile unsigned int *)0x60003f00 = 0x1;

    while(1){
        data = *((volatile unsigned int *)0x60003f00);
        if (( data & 0x2 ) == 0x2) {
            //success
            break;
        } else {
            continue;
        }
    }

    r_output_token_data = *((volatile unsigned int *)0x60000c00);
    if (r_output_token_data != c_output_token_id) {
        ret = false;
    }

    r_output_token_data = *((volatile unsigned int *) (data_out + buf_len));
    if ( r_output_token_data != c_output_token_id )
    {
       ret = false;
    }

    *(volatile unsigned int *)0x60003f00 = 0x2;

    return ret;
}


void aes_ecb_sample(void)
{
    int i;
    unsigned char sk[16] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10};
    //unsigned char sk[24] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    //unsigned char sk[36] = {0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    unsigned char data_in[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x60, 0x50};
    unsigned char data_out[16] = {0}; //The length of data_out should be the same as data_in
    unsigned char data_out2[16] = {0};//The length of data_out2 should be the same as data_out

    printf("\r\n aes_ecb_sample \r\n");

    //AEC ECB Encryption
    memset(data_out, 0, 16);
    nl_scrt_aes_ecb(true, sk, sizeof(sk), data_in, data_out, sizeof(data_in));
    for(i=0; i < sizeof(data_in); i++) {
        printf( "output data_out[%d]:%02x \r\n", i, data_out[i]);
    }

    //AES ECB Decryption
    memset(data_out2, 0, 16);
    nl_scrt_aes_ecb(false, sk, sizeof(sk), data_out, data_out2, sizeof(data_out));
    for(i=0; i < sizeof(data_out); i++) {
        printf( "output data_out2[%d]:%02x \r\n", i, data_out2[i]);
    }
}
#endif


#if 0
/**
 * nl_se_aes_cbc_128_encrypt - AES-128 CBC encryption
 * @key: Encryption key
 * @iv: Encryption IV for CBC mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes (must be divisible by 16)
 * Returns: 0 on success, -1 on failure
 */
int nl_se_aes_cbc_128_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CBC,
                                       	key,
                                       	(128/8),
                                       	iv,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, false) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
/**
 * nl_se_aes_cbc_128_decrypt - AES-128 CBC decryption
 * @key: Decryption key
 * @iv: Decryption IV for CBC mode (16 bytes)
 * @data: Data to decrypt in-place
 * @data_len: Length of data in bytes (must be divisible by 16)
 * Returns: 0 on success, -1 on failure
 */
int nl_se_aes_cbc_128_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CBC,
                                       	key,
                                       	(128/8),
                                       	iv,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, true) == END_TEST_SUCCES)
        {
        	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;

}
#endif

#if 0
/**
 * nl_se_aes_cbc_256_encrypt - AES-256 CBC encryption
 * @key: Encryption key
 * @iv: Encryption IV for CBC mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes (must be divisible by ?)
 * Returns: 0 on success, -1 on failure
 */
int nl_se_aes_cbc_256_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CBC,
                                       	key,
                                       	(256/8),
                                       	iv,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, false) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
/**
 * nl_se_aes_cbc_256_decrypt - AES-256 CBC decryption
 * @key: Decryption key
 * @iv: Decryption IV for CBC mode (16 bytes)
 * @data: Data to decrypt in-place
 * @data_len: Length of data in bytes (must be divisible by ?)
 * Returns: 0 on success, -1 on failure
 */
int nl_se_aes_cbc_256_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_CBC,
                                       	key,
                                       	(256/8),
                                       	iv,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, true) == END_TEST_SUCCES)
        {
        	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;

}
#endif

#if 0
int nl_se_aes_ecb_128_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_ECB,
                                       	key,
                                       	(128/8),
                                       	NULL,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, false) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
int nl_se_aes_ecb_128_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_ECB,
                                       	key,
                                       	(128/8),
                                       	NULL,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, true) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
int nl_se_aes_ecb_256_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_ECB,
                                       	key,
                                       	(256/8),
                                       	NULL,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, false) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif

#if 0
int nl_se_aes_ecb_256_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len, uint8_t *out_data)
{
    SymCryptCtx_t CryptoCntxt;
    TestVector_AES_BASIC_t tv_p;

    if (nl_se_SetupContextAndEnvironment(&CryptoCntxt,
                                       	VAL_SYM_ALGO_CIPHER_AES,
                                       	TESTVECTORS_MODE_ECB,
                                       	key,
                                       	(128/8),
                                       	NULL,
                                       	data,
                                       	out_data,
                                       	(uint32_t)data_len) == END_TEST_SUCCES)
    {

        CryptoCntxt.options |= SYM_CRYPT_IN_PLACE;

        if (do_SymCrypt(&CryptoCntxt, true) == END_TEST_SUCCES)
        {
          	return END_TEST_SUCCES;
        }
    }

	return END_TEST_FAIL;
}
#endif


