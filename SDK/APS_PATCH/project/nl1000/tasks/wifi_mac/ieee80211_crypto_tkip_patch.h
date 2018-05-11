#ifndef _IEEE80211_CRYPTO_TKIP_PATCH_H_
#define _IEEE80211_CRYPTO_TKIP_PATCH_H_

#include <stdint.h>

#include "common/wifi_ieee802_11_defs.h"


/* Rom patch interface */
typedef int (*tkip_mic_check_fp_t)(const u8 *tk, const struct ieee80211_hdr *hdr, u8 *plain, size_t plain_len);

/* Export interface funtion pointer */
extern tkip_mic_check_fp_t tkip_mic_check;

void wifi_mac_tkip_patch_init(void);

#endif /* _IEEE80211_CRYPTO_TKIP_PATCH_H_ */

