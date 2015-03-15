#include <stdio.h>

int ksprintf_help(unsigned c, void **ptr, size_t *counter, size_t maxcounter);

// Função printf
int vsnprintf(char *buffer, size_t count, const char *fmt, va_list arg_ptr)
{
	return do_printf(fmt, arg_ptr, ksprintf_help, buffer, count);
}
