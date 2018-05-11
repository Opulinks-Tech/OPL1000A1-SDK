#include "adapter_alloc.h"
#include "basic_defs.h"
#include "stdlib.h"

/*----------------------------------------------------------------------------
  Adapter_Alloc
*/
void *
Adapter_Alloc(int size)
{
    if (size > 0)
        return malloc(size);
    else
        return NULL; // ... but only extreme optimists do not check the result!
}

/*----------------------------------------------------------------------------
  Adapter_Free
*/
void
Adapter_Free(void *p)
{
    if (p != NULL)
        free(p);
}


