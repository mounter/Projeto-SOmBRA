#include <stdio.h>

// Função printf
void printf(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	(void)do_printf(fmt, args, kprintf_help, NULL, 0);
	va_end(args);
}
