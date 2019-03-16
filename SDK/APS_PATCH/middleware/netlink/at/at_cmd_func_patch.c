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

#ifdef AT_CMD_EXT_TBL_LST
extern T_CmdTblLst g_tAtCmdExtTblLst;
T_CmdTblLst g_tAtCmdAppTbl = {0};
#else
extern _at_command_t *gp_at_cmd_ext_table;
#endif

/*
 * @brief AT Module API Interface Function Register
 *
 */
#ifdef AT_CMD_EXT_TBL_LST
int at_cmd_process_func_register(void *ptr)
{
    int iRet = 1;
    T_CmdTblLst *ptLst = &g_tAtCmdExtTblLst;

    if(ptLst->taCmdTbl)
    {
        #if 1
        // only two elements in table list: one for gAtCmdTbl_ext and the other for app cmd
        if(ptLst->ptNext)
        {
            iRet = 0;
            goto done;
        }

        ptLst->ptNext = &g_tAtCmdAppTbl;
        #else
        // more elements in table list: but malloc will occupy more memory (64 to 2048)
        while(ptLst->ptNext)
        {
            ptLst = ptLst->ptNext;
        }
    
        ptLst->ptNext = (T_CmdTblLst *)malloc(sizeof(T_CmdTblLst));
    
        if(ptLst->ptNext == NULL)
        {
            iRet = 0;
            goto done;
        }
        #endif

        ptLst = ptLst->ptNext;
    }

    ptLst->taCmdTbl = (_at_command_t *)ptr;
    ptLst->ptNext = NULL;

done:
    return iRet;
}
#else
int at_cmd_process_func_register(void *ptr)
{
    if (ptr == NULL) {
        return false;
    }
    
    gp_at_cmd_ext_table = ptr;
    
    return true;
}
#endif

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

    
    #if defined(__AT_CMD_SUPPORT__)
    /** AT Msg Extend */
    at_msg_ext_init_patch();
    
    at_cmd_app_func_preinit_patch();
    #endif
    
    at_task_func_init_patch();
    
    /** Extend Table register */
    at_cmd_process_func_register(&gAtCmdTbl_ext[0]);
}
