
void test_aes_cmac(void);
void test_aes_cbcmac(void);
int do_CMAC_Gen(uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen);
int do_CBCMAC_Gen(uint8_t * Key_p, uint32_t KeySize, uint8_t * Msg_p, uint32_t MsgLen, uint8_t * Mac_p, uint32_t MacLen, uint8_t * Iv_p);

