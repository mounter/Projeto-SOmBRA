/************************************************
**Este c�digo � uma biblioteca do
**Prot�tipo de kernel do SOmBRA.
**
**
** SEM GARANTIAS
**
**Est� codigo n�o tem garantias, assim n�o
**me responsabilizo pelo que ele possa causar...
**
**Bom Proveito!
************************************************/
#include <string.h>

void *memcpy(void *dest, const void *src, size_t count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}
