/************************************************
**Este código é uma biblioteca do
**Protótipo de kernel do SOmBRA.
**
**
** SEM GARANTIAS
**
**Esté codigo não tem garantias, assim não
**me responsabilizo pelo que ele possa causar...
**
**Bom Proveito!
************************************************/
#include <string.h>

void *memset(void *dest, char val, unsigned count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}
