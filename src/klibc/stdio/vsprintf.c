#include <stdio.h>

int ksprintf_help(unsigned c, void **ptr, size_t *counter, size_t maxcounter);

// Fun��o printf
int vsprintf(char *buffer, const char *fmt, va_list arg_ptr)
{
	return do_printf(fmt, arg_ptr, ksprintf_help, buffer, 0);
}
