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

#include <stdio.h>
#include "blewifi_common.h"


void BleWifi_HexDump(const char *title, const uint8_t *buf, size_t len)
{
	size_t i;

	printf("\n%s - hexdump(len=%lu):", title, (unsigned long)len);
	if (buf == NULL)
	{
		printf(" [NULL]");
	}
	else
	{
		for (i = 0; i < len; i++)
		{
            if (i%16 == 0)
                printf("\n");
			printf(" %02x", buf[i]);
		}
	}
	printf("\n");

}
