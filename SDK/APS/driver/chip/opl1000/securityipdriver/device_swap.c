#include "basic_defs.h"
#include "device_swap.h"


/*----------------------------------------------------------------------------
 * Device_SwapEndian32
 *
 * This function can be used to swap the byte order of a 32bit integer. The
 * implementation could use custom CPU instructions, if available.
 */
uint32_t
Device_SwapEndian32(
        const uint32_t Value)
{
/*#ifdef DEVICE_SWAP_SAFE
    return (((Value & 0x000000FFU) << 24) |
            ((Value & 0x0000FF00U) <<  8) |
            ((Value & 0x00FF0000U) >>  8) |
            ((Value & 0xFF000000U) >> 24));
#else*/
    // reduces typically unneeded AND operations
    return ((Value << 24) |
            ((Value & 0x0000FF00U) <<  8) |
            ((Value & 0x00FF0000U) >>  8) |
            (Value >> 24));
//#endif
}



