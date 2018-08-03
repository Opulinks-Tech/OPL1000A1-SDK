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
#include "at_cmd_func_patch.h"
#include "at_cmd_patch.h"
#include "at_cmd_common_patch.h"
#include "at_cmd_app_patch.h"
#include "at_cmd_msg_ext_patch.h"
#include "at_cmd_task_patch.h"
#include "at_cmd_table_ext.h"
#include "at_cmd_rf_patch.h"

extern _at_command_t *gp_at_cmd_ext_table;

/*
 * @brief AT Module API Interface Function Register
 *
 */
int at_cmd_process_func_register(void *ptr)
{
    if (ptr == NULL) {
        return false;
    }
    
    gp_at_cmd_ext_table = ptr;
    
    return true;
}

/*
 * @brief AT Module API Interface Initialization
 *
 */
void at_func_init_patch(void)
{
    /** AT Cmd Architecture */
    _at_cmd_func_init_patch();

    /** AT Cmd Common */
    at_cmd_common_func_init_patch();

    /** AT Cmd RF */
    _at_cmd_rf_func_init_patch();

    /** AT Msg Extend */
    at_msg_ext_init_patch();

    at_cmd_app_func_preinit_patch();

    at_task_func_init_patch();
    
    /** Extend Table register */
    at_cmd_process_func_register(&gAtCmdTbl_ext[0]);
}
