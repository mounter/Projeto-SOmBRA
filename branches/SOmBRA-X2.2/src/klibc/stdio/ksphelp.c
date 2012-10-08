#include <stdio.h>

int ksprintf_help(unsigned c, void **ptr, size_t *counter, size_t maxcounter)
{
	char **s_ptr = (char **)ptr;

	**s_ptr = c;
	*(++(*s_ptr)) = 0;
	return 0;
}
