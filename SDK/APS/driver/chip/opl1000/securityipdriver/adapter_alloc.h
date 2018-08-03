/* adapter_alloc.h
 *
 *
 */

/*****************************************************************************
* Copyright (c) 2011-2016 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef ADAPTER_ALLOC_H_
#define ADAPTER_ALLOC_H_

void * Adapter_Alloc(int size);
void Adapter_Free(void *p);

#endif /* ADAPTER_ALLOC_H_ */


/* end of file adapter_alloc.h */
