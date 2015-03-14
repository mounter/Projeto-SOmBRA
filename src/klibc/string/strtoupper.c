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
