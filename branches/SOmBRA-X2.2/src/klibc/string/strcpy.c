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

// Copia os dados de src para dest.
char* strcpy(char *dest, char *src){
  char *c1, *c2;
  for (c1=dest, c2=src; (*c2)!='\0';c1++,c2++){
    (*c1) = (*c2);
  }
  (*c1) = '\0';
  return dest;
}
