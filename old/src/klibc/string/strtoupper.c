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
#include <ctype.h>

// Converte a string para tudo maiusculo.
void strtoupper(char *src){
  char *c;

  if (!src){
    return;
  }
  c = src;
  while((*c)!='\0'){
    (*c) = toupper(*c);
    c++;
  }
}
