/******************************************************************************
*  Copyright 2017, Netlink Communication Corp.
*  ---------------------------------------------------------------------------
*  Statement:
*  ----------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of Netlink Communication Corp. (C) 2017
******************************************************************************/

//#include "le_host.h"
#include "ble_host_patch_init.h"
//#include "ble_host_patch_init_fn.h"

extern void LeHostPatchVarPatch_Init(void);
extern void LeUtilPatch_Init(void);
extern void LeRtosTaskPatch_Init(void);
extern void LeCmHandlerPatch_Init(void);
extern void LeAttCommonPatch_Init(void);
extern void LeAttPatch_Init(void);
#ifdef __LE_STORE_BOND__ 
extern void LeSmpStorePatch_Init(void);
extern void LeSmpMsgHandlerPatch_Init(void);
#endif
extern void LeSmpUtilPatch_Init(void);
extern void LeGattCommonPatch_Init(void);
extern void LeGattServerPatch_Init(void);
extern void LeGapIfPatch_Init(void);
#ifdef __LE_STORE_BOND__ 
extern void LeHostTestCmdPatch_Init(void);
#endif
#ifdef __AT_CMD_SUPPORT__
extern void LePtsAppCmdPatch_Init(void);
extern void LeCmdAppPatch_Init(void);
extern void LeCmdAppCmdPatch_Init(void);
extern void LeCmdAppGattPatch_Init(void);
extern void LeCmdAppDbPatch_Init(void);
extern void LeHostSetVarFn(void *fn);
#endif


void LeHostPatchAssign(void)
{
	LeHostPatchVarPatch_Init();

	// Util
	LeUtilPatch_Init();
    
    // Task
    LeRtosTaskPatch_Init();

	// CM
	LeCmHandlerPatch_Init();

	// ATT
	LeAttCommonPatch_Init();
	LeAttPatch_Init();
    
    // SMP
#ifdef __LE_STORE_BOND__ 
	LeSmpStorePatch_Init();
    LeSmpMsgHandlerPatch_Init();
#endif
	LeSmpUtilPatch_Init();
    
	// GATT
	LeGattCommonPatch_Init();
    LeGattServerPatch_Init();
    
	// GAP
	LeGapIfPatch_Init();
    
	// APP Util
#ifdef __LE_STORE_BOND__ 
	LeHostTestCmdPatch_Init();
#endif

	// PTS APP
#ifdef __AT_CMD_SUPPORT__
	LePtsAppCmdPatch_Init();
#endif

	// CMD APP
#ifdef __AT_CMD_SUPPORT__
	LeCmdAppPatch_Init();
    LeCmdAppCmdPatch_Init();
	LeCmdAppGattPatch_Init();
	LeCmdAppDbPatch_Init();

    LeHostSetVarFn(0);
#endif
}
