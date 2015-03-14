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

int fchar(const char *s, char c)
{
	int i=0;
	for (;*s != c;s++, i++);
	return i;
}
