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

// Copia os dados de src para dest.
char* strcpy(char *dest, char *src){
  char *c1, *c2;
  for (c1=dest, c2=src; (*c2)!='\0';c1++,c2++){
    (*c1) = (*c2);
  }
  (*c1) = '\0';
  return dest;
}
