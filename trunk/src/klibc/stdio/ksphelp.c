#include <stdio.h>

int ksprintf_help(unsigned c, void **ptr)
{
	char **s_ptr = (char **)ptr;

	**s_ptr = c;
	*(++(*s_ptr)) = 0;
	return 0;
}
