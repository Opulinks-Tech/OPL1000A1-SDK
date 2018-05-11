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

#if 0

#include "scrt.h"
#include "scrt_patch.h"

/*
 * scrt_patch_init - SCRT Patch Initialization
 *
 */
void scrt_patch_init(void)
{
    nl_hmac_sha_1 = nl_hmac_sha_1_patch;
    nl_scrt_aes_ecb = nl_scrt_aes_ecb_patch;

    nl_scrt_Init = nl_scrt_init_patch;
    nl_aes_ccm = nl_aes_ccm_patch;
    nl_scrt_aes_ccm = nl_scrt_aes_ccm_patch;
}

#endif
