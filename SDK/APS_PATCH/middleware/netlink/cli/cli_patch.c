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
#include <string.h>
#include "cli_patch.h"


/**
 * @brief User CLI command example.
 *        OPL1000 DbgUart command parser is after this function
 * @param pbuf [in] The input buffer pointer.
 * @param len [in] The length of input buffer.
 * @return Command searching status.
 * @retval CMD_FINISHED - CLI command finished, it won't run OPL1000 CLI command parser.
 *                        For the case user command found.
 * @retval CMD_CONTINUE - After this function, it will run OPL1000 CLI command parser.
 *                        For the case to run original OPL1000 commands.
 */
E_CLI_CMD_PROC Cli_UserCmdExample(char *pbuf, int len)
{
    /* Command parser example */
    if (!strcmp(pbuf, "usrcmd"))
    {
        printf("\r\nUser command example\r\n");
        return CMD_FINISHED;
    }
        
    /* User command not matched, to search OPL1000 commands */
    return CMD_CONTINUE;
}


/**
 * @brief To register user command process function to diag_task, 
 *        the CLI command handler in DbgUart.
 *        If need to enable user command, put this function to initialization
 */
void Cli_UserCmdProcReg(void)
{
    Diag_UserCmdSet(Cli_UserCmdExample);
}
