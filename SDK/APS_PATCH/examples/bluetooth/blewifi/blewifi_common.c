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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "blewifi_common.h"

int blewifi_debug_level = BLEWIFI_MSG_DEBUG;

void blewifi_hexdump(int level, const char *title, const uint8_t *buf, size_t len, int show)
{
	size_t i;

	if (level < blewifi_debug_level)
		return;

	printf("\n%s - hexdump(len=%lu):", title, (unsigned long) len);
	if (buf == NULL) {
		printf(" [NULL]");
	} else if (show) {
		for (i = 0; i < len; i++)
		{
            if (i%16 == 0)
                printf("\r\n");
			printf(" %02x", buf[i]);
		}
	} else {
		printf(" [REMOVED]");
	}
	printf("\n");

}

