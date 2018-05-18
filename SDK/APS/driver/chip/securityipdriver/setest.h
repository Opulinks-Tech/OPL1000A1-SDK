/*
 * Security IP low level test functions
 * Copyright (c) 2016-2017, Netlink
 *
 * This program is used to do low level test for security IP functions.
 *
 * See README and COPYING for more details.
 */
#include "basic_defs.h"

 //Run the test on ROM
void test_onrom_level1_scrt0_reg_access(void);
void test_onrom_level1_scrt1_reg_access(void);
void test_onrom_level1_scrt0_scrt1_cross_reg_access(void);
void test_onrom_level1_sram_access(void);
void test_onrom_level2_scrt0_all_reg_access(void);
void test_onrom_level2_sram_block_write_read_compare(void);
void test_onrom_level3_scrt0_aes_64bytes_with_delay(void);
void test_onrom_level4_scrt0_aes_64bytes_no_delay(void);
void test_onrom_level5_scrt0_aes_64bytes_with_new_sram_address_400000(void);
void test_onrom_level5_scrt0_aes_2k(void);
void test_onrom_level5_scrt0_aes_64b_1mailbox_3tokens(void);


//Run the test on SRAM
void test_onsram_scrt_sram_burst(void);
void test_onsram_scrt_sram_burst_oneword(void);
void test_onsram_scrt_mailbox_link(void);
void test_onsram_scrt_do_aes_sixty_four_bytes(void);
void test_onsram_scrt_do_aes_2k_bytes(void);
void test_onsram_scrt_do_aes_2k_bytes_3times(void);
void test_onsram_scrt_do_aes_2k_bytes_src_dst_addr_the_same(void);
void test_onsram_scrt_do_aes_64b_1mailbox_3tokens(void);
void test_onsram_scrt_core_reg_test(void);


//NOP, it means "Memory Copy by Security IP's DMA"
void test_onsram_scrt_do_nop_in_4_mailboxs(void);


//CMAC
void test_onsram_scrt_aes_cmac(void);


//HMAC SHA-1
void test_onsram_scrt_hmac_sha_1(void);


//HASH
void test_onsram_scrt_hash(void);


//TRNG
void test_onsram_scrt_trng(void);
void test_onsram_scrt_trng2(void);


//FPGA test on ROM
void fpga_test_onrom_level1_scrt0_reg_access(void);
void fpga_test_onrom_level1_scrt1_reg_access(void);
void fpga_test_onrom_level1_sram_access(void);
void fpga_test_onrom_level5_scrt_sram_burst(void);
void fpga_test_onrom_level4_scrt0_aes(void);
void fpga_test_onrom_level2_scrt0_all_reg_access(void);
void fpga_test_onrom_level4_scrt0_aes_64bytes(void);
void fpga_test_onrom_level4_scrt0_aes_64bytes_debug(void);
void fpga_test_aes_ccm(bool bEncrypt);
void fpga_test_aes_ccm_2(bool bEncrypt);
void fpga_test_aes_ccm_3(bool bEncrypt);
bool fpga_aes_ccm_4 (bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);
bool fpga_aes_ccm_5 (bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);
void fpga_aes_ccm_sample_wifi(void);
void fpga_aes_ccm_sample_wifi_2(void);
bool fpga_aes_ccm_6(bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);
bool fpga_aes_ccm_7(bool bEncrypt, uint8_t *sk, int sk_len, uint8_t *nonce, int nonce_len, unsigned int adata, int adata_len, unsigned int plain_text, unsigned int encrypted_text, int text_len, uint8_t *tag, int tag_len);
void fpga_aes_ccm_sample_enc_dec(void);


//ITP test
bool itp_scrt_scrt0_reg_access(void);
bool itp_scrt_scrt1_reg_access(void);
bool itp_scrt_sram_access(void);
bool itp_scrt_sram_burst(void);
bool itp_scrt_scrt0_all_reg_access(void);
bool itp_scrt_aes_64bytes(void);
bool itp_scrt_aes_cmac(void);
bool itp_scrt_hash(void);


