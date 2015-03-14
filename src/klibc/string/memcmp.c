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

// Compara dois espaços de memória.
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
