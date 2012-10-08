#include <stdio.h>

// Função printf
int sprintf(char *buffer, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	int i = vsprintf(buffer, fmt, args);
	va_end(args);

	return i;
}
