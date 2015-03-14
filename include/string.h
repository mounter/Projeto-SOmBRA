#ifndef STRING_H
#define STRING_H

#include <_sizet.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);
size_t strlen(const char *str);
int   strcmp(char *s1, char *s2);
char* strcpy(char *dest, char *src);
char* strncpy(char *dest, char *src, unsigned int n);
char *strcat(char *dest, const char *src);
void  strtoupper(char *src);
int fchar(const char *s, char c);
int strncmp(const char *s1, const char *s2, unsigned int n);
int memcmp(const void *memptr1, const void *memptr2, unsigned int count);
void itoa(int d, char *buf, int b);

#endif
