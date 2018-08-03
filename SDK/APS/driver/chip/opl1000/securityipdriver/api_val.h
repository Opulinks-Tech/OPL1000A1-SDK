/**
 * @mainpage VAL API
 * @section Introduction
 * This documentation covers the API for the VaultIP that each application
 * should use.
 *
 * @file api_val.h
 *
 * @brief VaultIP Abstraction Layer (VAL) API
 *
 * This API covers all functionality that is provided by VaultIP. It implements
 * an Abstraction Layer that runs on top of the layer that performs the actual
 * communication (exchange) with VaultIP.
 */

/*****************************************************************************
* Copyright (c) 2014-2017 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_API_VAL_H
#define INCLUDE_GUARD_API_VAL_H

#define VAL_API_VERSION "2.1"

/** VAL API specifics per category */
#include "api_val_result.h"
#include "api_val_buffers.h"
#include "api_val_asset.h"
#include "api_val_securetimer.h"
#include "api_val_asym.h"
#include "api_val_aunlock.h"
#include "api_val_nop.h"
#include "api_val_random.h"
#include "api_val_sym.h"
#include "api_val_symkeywrap.h"
#include "api_val_service.h"
#include "api_val_system.h"
#include "api_val_emmc.h"
#include "api_val_claim.h"

#endif /* Include Guard */

/* end of file api_val.h */
