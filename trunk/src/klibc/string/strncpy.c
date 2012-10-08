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

/* Copia até n bytes de uma string para outra. */
char* strncpy(char *dest, char *src, unsigned int n){
	char *c1, *c2;
	int i = 0;
  	for (c1=dest, c2=src; (*c2)!='\0';c1++,c2++){
    	(*c1) = (*c2);
    	i++;
    	if (i>=n){
	    	break;
    	}
  	}
  	(*c1) = '\0';
  	return dest;
}
