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

// Compara duas string.
int strcmp(char *s1, char *s2){
  int   i;
  char *c1, *c2;
  for (c1=s1, c2=s2;(*c1)!='\0'; c1++,c2++){
    i = (*c1)-(*c2);
    if (i!=0){
      return i;
    }
  }
  if ((*c2)!=0){
    return -(*c2);
  }
  return 0;
}
