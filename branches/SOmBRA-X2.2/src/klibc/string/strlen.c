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

size_t strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}
