/******************************************************************************
*  Copyright 2018, Netlink Communication Corp.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2018
******************************************************************************/

#ifndef _WIFI_MAC_TX_DATA_PATCH_H_
#define _WIFI_MAC_TX_DATA_PATCH_H_

/* Interface initial function */
int wifi_mac_get_encrypt_eapol_frame_impl(u8 *en);
int wifi_mac_set_encrypt_eapol_frame_impl(u8 en);
void wifi_mac_txdata_func_init_patch(void);
int wifi_mac_xmit_eapol_frame_patch(u8 *buf, size_t len, const u8 *eth_dest, u16 proto);
u32 wifi_mac_xmit_frame_patch(u8 *tx_data, u32 tx_data_len);
s32 ieee80211_xmit_frame_patch(u8 *tx_data, int tx_data_len);

/* Rom patch interface */
typedef int (*wifi_mac_get_encrypt_eapol_frame_fp_t)(u8 *en);
typedef int (*wifi_mac_set_encrypt_eapol_frame_fp_t)(u8 en);

/* Export interface function pointer */
extern wifi_mac_get_encrypt_eapol_frame_fp_t wifi_mac_get_encrypt_eapol_frame;
extern wifi_mac_set_encrypt_eapol_frame_fp_t wifi_mac_set_encrypt_eapol_frame;

#endif /* _WIFI_MAC_TX_DATA_PATCH_H_ */
