#include <stdio.h>

int ksnprintf_help(unsigned c, void **ptr, size_t *counter, size_t maxcounter)
{
	if (*counter == maxcounter)
		return 0;
		
	(*counter)++;	
	char **s_ptr = (char **)ptr;

	**s_ptr = c;
	*(++(*s_ptr)) = 0;
	return 0;
}
