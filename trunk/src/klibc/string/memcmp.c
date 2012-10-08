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

// Compara dois espa�os de mem�ria.
int memcmp(const void *memptr1, const void *memptr2, unsigned int count){
  const unsigned char *mem1 = (const unsigned char *)memptr1;
  const unsigned char *mem2 = (const unsigned char *)memptr2;

  for(; count != 0; count--){
    if(*mem1 != *mem2){
      return *mem1 -  *mem2;
    }
    mem1++;
    mem2++;
  }
  return 0;
}
