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

int fchar(const char *s, char c)
{
	int i=0;
	for (;*s != c;s++, i++);
	return i;
}
