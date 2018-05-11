/* adapter_bufmanager.c
 *
 * Buffer Manager intended for VaultIP tokens.
 */

/*****************************************************************************
* Copyright (c) 2014-2016 INSIDE Secure B.V. All Rights Reserved.
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

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */
#include "adapter_bufmanager.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "dmares_types.h"           // DMAResource_Handle_t
                                    // DMAResource_Properties_t
                                    // DMAResource_AddrPair_t
#include "dmares_buf.h"             // DMAResource_Alloc, DMAResource_Release
                                    // DMAResource_CheckAndRegister
#include "dmares_addr.h"            // DMAResource_Translate
//#include "dmares_rw.h"              // DMAResource_PreDMA, DMAResource_PostDMA


/*----------------------------------------------------------------------------
 * Definitions and macros
 */
#ifndef BUFMANAGER_ADMIN_ENTRIES
#define BUFMANAGER_ADMIN_ENTRIES    12
#endif

/*----------------------------------------------------------------------------
 * BufManager_AdminEntry_t
 * BufManager adminstration entry
 */
typedef struct
{
    DMAResource_Handle_t BufferHandle;  // Buffer handle
    uint64_t BufferAddress;             // Physic buffer address
    void * Buffer_p;                    // Host pointer to buffer
    size_t BufferSize;                  // Actual buffer size
    bool fBounced;                      // Bounce buffer indication
    bool fFromUserSpace;                // Data_p is from User Space indication
    BufManager_BufferType_t Type;       // Buffer type: NOT_USED,IN, OUT, INOUT
    union
    {
        const void * c_p;
        void * n_p;
    } Data;                             // Caller data buffer pointer
    size_t DataSize;                    // Caller data buffer size
    void * UserData_p;                  // Caller user data
} BufManager_AdminEntry_t;


#if 0
/*----------------------------------------------------------------------------
 * Local variables
 */
static bool gl_BufManager_InitDone = false;
static BufManager_AdminEntry_t gl_BufManager_Admin[BUFMANAGER_ADMIN_ENTRIES];
static BufManager_CB_SizeAlignment_t gl_BufManager_SizeAlignment = NULL;
static BufManager_CB_CheckClear_t gl_BufManager_CheckClear = NULL;
static BufManager_CB_CheckReady_t gl_BufManager_CheckReady = NULL;


/*----------------------------------------------------------------------------
 * BufManagerLocal_GetEntry
 */
static BufManager_AdminEntry_t *
BufManagerLocal_GetEntry(
        uint64_t BufferAddress)
{
    int i;

    if (!gl_BufManager_InitDone)
    {
        // Initial the buffer manager administration
        memset(&gl_BufManager_Admin[0],
               0,
               (BUFMANAGER_ADMIN_ENTRIES * sizeof(BufManager_AdminEntry_t)));
        gl_BufManager_InitDone = true;
    }

    // Search for the requested buffer
    for (i = 0; i < BUFMANAGER_ADMIN_ENTRIES; i++)
    {
        if (gl_BufManager_Admin[i].BufferAddress == BufferAddress)
        {
            return &gl_BufManager_Admin[i];
        }
    }
    return (BufManager_AdminEntry_t *)NULL;
}


/*----------------------------------------------------------------------------
 * BufManager_Alloc
 */
static uint64_t
BufManagerLocal_AllocBuffer(
        bool fFromUserSpace,
        bool fBounce,
        BufManager_BufferType_t Type,
        size_t BufferSize,
        const void * const Data_p,
        size_t DataSize,
        void * UserData_p)
{
    uint64_t BufferAddress = 0;
    BufManager_AdminEntry_t *Entry_p = NULL;

    switch (Type)
    {
    case BUFMANAGER_BUFFERTYPE_IN:
    case BUFMANAGER_BUFFERTYPE_OUT:
    case BUFMANAGER_BUFFERTYPE_INOUT:
        Entry_p = BufManagerLocal_GetEntry(0);
        break;
    default:
        break;
    }
    if (Entry_p != NULL)
    {
        // Found a free entry
        DMAResource_Properties_t Props = { 0, 0, 0, 0 };
        DMAResource_AddrPair_t AddrPair;

        Entry_p->Type = Type;
        Entry_p->fFromUserSpace = fFromUserSpace;
        Entry_p->Data.c_p = Data_p;
        Entry_p->DataSize = DataSize;
        Entry_p->UserData_p = UserData_p;

        // Determine buffer size
        Props.Alignment = 4;
        Props.Size = ((BufferSize + 3) & ~3);
        if (gl_BufManager_SizeAlignment &&
            ((Type == BUFMANAGER_BUFFERTYPE_OUT) ||
             (Type == BUFMANAGER_BUFFERTYPE_INOUT)))
        {
            Props.Size = gl_BufManager_SizeAlignment(Props.Size);
        }
        Entry_p->BufferSize = Props.Size;

        AddrPair.Address_p = Entry_p->Data.n_p;
        AddrPair.Domain = DMARES_DOMAIN_HOST;

        if (!fFromUserSpace && !fBounce &&
            (Entry_p->BufferSize == ((DataSize + 3) & ~3)))
        {
            // Check if current buffer is usable
            if (DMAResource_CheckAndRegister(Props,
                                             AddrPair,
                                             'k',
                                             &Entry_p->BufferHandle) == 0)
            {
                Entry_p->Buffer_p = AddrPair.Address_p;
            }
        }

        if (Entry_p->Buffer_p == NULL)
        {
            if (DMAResource_Alloc(Props, &AddrPair, &Entry_p->BufferHandle) < 0)
            {
                // Internal error
                goto error_func_exit;
            }
            Entry_p->Buffer_p = AddrPair.Address_p;
            Entry_p->fBounced = true;
        }

        // Translate address to get the physical address of the buffer
        if (DMAResource_Translate(Entry_p->BufferHandle,
                                  DMARES_DOMAIN_BUS,
                                  &AddrPair) != 0)
        {
            // Internal error
            goto error_func_exit;
        }
        Entry_p->BufferAddress = (uint64_t)(uintptr_t)AddrPair.Address_p;

        // If needed, clear/reset output ready check
        if (gl_BufManager_CheckClear &&
            ((Type == BUFMANAGER_BUFFERTYPE_OUT) ||
             (Type == BUFMANAGER_BUFFERTYPE_INOUT)))
        {
            if(gl_BufManager_CheckClear(Entry_p->Buffer_p,
                                        Entry_p->BufferSize,
                                        Entry_p->UserData_p) < 0)
            {
                // Internal error
                goto error_func_exit;
            }
        }

        // Copy the input data
        if (Entry_p->fBounced &&
            ((Type == BUFMANAGER_BUFFERTYPE_IN) ||
             (Type == BUFMANAGER_BUFFERTYPE_INOUT)))
        {
#ifdef MODULE
            if (fFromUserSpace)
            {
                if (copy_from_user(Entry_p->Buffer_p, Data_p, DataSize) != 0)
                {
                    // Internal error
                    goto error_func_exit;
                }
            }
            else
#endif
            {
                (void)memcpy(Entry_p->Buffer_p, Data_p, DataSize);
            }
        }
        BufferAddress = Entry_p->BufferAddress;

error_func_exit:
        if (BufferAddress == 0)
        {
            if (Entry_p->BufferHandle)
            {
                // Release the buffer
                DMAResource_Release(Entry_p->BufferHandle);
            }

            // Clear the administration entry
            memset(Entry_p, 0, sizeof(BufManager_AdminEntry_t));
        }
    }
    return BufferAddress;
}


/*----------------------------------------------------------------------------
 * BufManager_GetInAddress
 */
int
BufManager_Register(
        BufManager_CB_SizeAlignment_t SizeAlignment,
        BufManager_CB_CheckClear_t CheckClear,
        BufManager_CB_CheckReady_t CheckReady)
{
    gl_BufManager_SizeAlignment = SizeAlignment;
    gl_BufManager_CheckClear = CheckClear;
    gl_BufManager_CheckReady = CheckReady;
    return 0;
}


/*----------------------------------------------------------------------------
 * BufManager_GetInAddress
 */
uint64_t
BufManager_Map(
        bool fFromUserSpace,
        BufManager_BufferType_t Type,
        const void * const Data_p,
        size_t DataSize,
        void * UserData_p)
{
    uint64_t BufferAddress = 0;

    if (Data_p != NULL)
    {
      BufferAddress = BufManagerLocal_AllocBuffer(fFromUserSpace,
#ifdef ADAPTER_REMOVE_BOUNCEBUFFERS
                                                false,
#else
                                                true,
#endif
                                                Type,
                                                DataSize,
                                                Data_p,
                                                DataSize,
                                                UserData_p);
      if (BufferAddress != 0)
      {
        BufManager_PreDmaAddress(BufferAddress);
      }
    }
    return BufferAddress;
}


/*----------------------------------------------------------------------------
 * BufManager_Alloc
 */
uint64_t
BufManager_Alloc(
        bool fFromUserSpace,
        BufManager_BufferType_t Type,
        size_t BufferSize,
        const void * const Data_p,
        size_t DataSize,
        void * UserData_p)
{
    return BufManagerLocal_AllocBuffer(fFromUserSpace,
                                       true,
                                       Type,
                                       BufferSize,
                                       Data_p,
                                       DataSize,
                                       UserData_p);
}


/*----------------------------------------------------------------------------
 * BufManager_GetInAddress
 */
int
BufManager_Unmap(
        uint64_t BufferAddress,
        bool fCopy)
{
    int rc = -1;

    if (BufferAddress != 0)
    {
        BufManager_AdminEntry_t * Entry_p;

        Entry_p = BufManagerLocal_GetEntry(BufferAddress);
        if (Entry_p != NULL)
        {
            rc = 0;                    // Looks OK

            if (fCopy  &&
                ((Entry_p->Type == BUFMANAGER_BUFFERTYPE_OUT) ||
                 (Entry_p->Type == BUFMANAGER_BUFFERTYPE_INOUT)))
            {
                DMAResource_PostDMA(Entry_p->BufferHandle,
                                    0,
                                    Entry_p->BufferSize);

                if (gl_BufManager_CheckReady != NULL)
                {
                    rc = gl_BufManager_CheckReady(Entry_p->Buffer_p,
                                                  Entry_p->BufferSize,
                                                  Entry_p->UserData_p);
                    if (rc != 0)
                    {
                        // Report internal error
                        rc = -3;
                    }
                }

                if (rc == 0)
                {
                    // Copy output data
#ifdef MODULE
                    if (Entry_p->fFromUserSpace)
                    {
                        if (copy_to_user(Entry_p->Data.n_p,
                                         Entry_p->Buffer_p,
                                         Entry_p->DataSize) != 0)
                        {
                            // Report internal error
                            rc = -2;
                        }
                    }
                    else if (Entry_p->fBounced)
#else
                    if (Entry_p->fBounced)
#endif
                    {
                        (void)memcpy(Entry_p->Data.n_p,
                                     Entry_p->Buffer_p,
                                     Entry_p->DataSize);
                    }
                }
            }

            // Release buffer and clear administration entry
            DMAResource_Release(Entry_p->BufferHandle);
            memset(Entry_p, 0, sizeof(BufManager_AdminEntry_t));
        }
    }

    return rc;
}


/*----------------------------------------------------------------------------
 * BufManager_GetSize
 */
size_t
BufManager_GetSize(
        uint64_t BufferAddress)
{
    if (BufferAddress != 0)
    {
        BufManager_AdminEntry_t *Entry_p;

        Entry_p = BufManagerLocal_GetEntry(BufferAddress);
        if (Entry_p != NULL)
        {
            return Entry_p->BufferSize;
        }
    }
    return 0;
}



/*----------------------------------------------------------------------------
 * BufManager_GetHostAddress
 */
void *
BufManager_GetHostAddress(
        uint64_t BufferAddress)
{
    if (BufferAddress != 0)
    {
        BufManager_AdminEntry_t *Entry_p;

        Entry_p = BufManagerLocal_GetEntry(BufferAddress);
        if (Entry_p != NULL)
        {
            return Entry_p->Buffer_p;
        }
    }
    return NULL;
}


/*----------------------------------------------------------------------------
 * BufManager_PreDmaAddress
 */
int
BufManager_PreDmaAddress(
        uint64_t BufferAddress)
{
    if (BufferAddress != 0)
    {
        BufManager_AdminEntry_t *Entry_p;

        Entry_p = BufManagerLocal_GetEntry(BufferAddress);
        if (Entry_p != NULL)
        {
            DMAResource_PreDMA(Entry_p->BufferHandle, 0, Entry_p->BufferSize);
            return 0;
        }
    }
    return -1;
}

#endif


/* end of file adapter_bufmanager.c */


