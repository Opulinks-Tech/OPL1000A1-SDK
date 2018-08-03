#include "sfzutf.h"


uint32_t
sfzutf_strlen(
    const char * str)
{
    uint32_t len = 0;
    const char *string_it = str;
    while(*string_it)
    {
        len++;
        string_it++;
    }
    return len;
}


/* Discard const qualifier from pointer */
void *
sfzutf_discard_const(
        const void * Ptr_p)
{
    union
    {
        const void * c_p;
        void * n_p;
    } Conversion;

    Conversion.c_p = Ptr_p;
    return Conversion.n_p;
}


#if 0
/* Switch ordering of array.
   This implementation works byte at a time. */
void
sfzutf_endian_flip(
        void *dst,
        uint32_t len)
{
    unsigned char *dst_c = dst;
    unsigned char t;
    uint32_t i;

    for(i=0; i < len/2; i++)
    {
        t = dst_c[i];
        dst_c[i] = dst_c[len-i-1];
        dst_c[len-i-1] = t;
    }
}
#endif

