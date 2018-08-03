/**
 * @file api_val_buffers.h
 *
 * @brief VAL API - Data Buffer definitions
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

#ifndef INCLUDE_GUARD_API_VAL_BUFFERS_H
#define INCLUDE_GUARD_API_VAL_BUFFERS_H

/**----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "clib.h"                   // size_t
#include "basic_defs.h"             // uint8_t


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
/** ValOctets*_t macros\n
 *  The types can be used in pointer-parameter declarations to clarify the
 *  direction of the parameter (input-only, output-only or input-output).
 *  Use the _Optional variant when the pointer parameter may be NULL. */
typedef const uint8_t ValOctetsIn_t;
typedef uint8_t ValOctetsOut_t;
typedef uint8_t ValOctetsInOut_t;

/** ValOctets*_Optional_t macros\n
 *  Refers to a pointer-parameter declarations or has the value NULL */
typedef ValOctetsIn_t    ValOctetsIn_Optional_t;
typedef ValOctetsOut_t   ValOctetsOut_Optional_t;
typedef ValOctetsInOut_t ValOctetsInOut_Optional_t;


/** ValSize_t\n
 *  This type is large enough to hold the size of a buffer, or the length of
 *  the data in the buffer. The maximum size for the entire API is defined by
 *  VAL_SIZE_MAX and when data is DMAed it is restricted to VAL_SIZE_MAX_DMA */
typedef size_t ValSize_t;

/** The maximum size value */
#define VAL_SIZE_MAX         0x7FFFFFFF
/** The maximum DMA-able size value */
#define VAL_SIZE_MAX_DMA     0x001FFFFF


#endif /* Include Guard */

/* end of file api_val_buffers.h */
