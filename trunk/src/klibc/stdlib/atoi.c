#include <string.h>

typedef enum{
  FALSE = 0,
  TRUE = 1 
} bool;


int atoi(const char *str){

  int slen, nret = 0, cnum = 0;
  bool ingt = FALSE;
  char cchar;


/*Sanity check for NULL valus*/
  if(str == NULL ){
    return -1;
  }
  
  slen = strlen(str); /* Size check*/
  
  if(slen <= 0){
    return -1;
   }

  if(*str == '-'){
    ingt = TRUE;    /*Is a Negatige number*/
    slen -= 2;
    str++;
  }
  else{
    ingt = FALSE;   /* Isn't a Negative number*/
    slen--;
  }

  while(*str){
    cchar = *str;
    if(cchar >= '0' && cchar <= '9'){
      cnum = *str - '0';
      nret = nret*10 + cnum;
    }
  str++;
  }

  if(ingt){
    return nret*(-1);
  }

  return nret;
}
