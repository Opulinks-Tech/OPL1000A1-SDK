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

#ifndef _AT_CMD_APP_PATCH_H
#define _AT_CMD_APP_PATCH_H

void at_cmd_wifi_hook_patch(void);
void at_user_wifi_app_entry(void *args);

#if defined(__AT_CMD_SUPPORT__)
void at_cmd_app_func_preinit_patch(void);
#endif

#endif /* _AT_CMD_APP_PATCH_H */
