#include <stdio.h>

void panic(const char *fmt, ...)
{
	set_con(get_con());
	va_list args;
	va_start(args, fmt);

	disable();
	set_color(CRED, CBLACK);
	puts("\nPANIC: ");

	set_color(CBRIGHTWHITE, CBLACK);

	(void)do_printf(fmt, args, kprintf_help, NULL, 0);

	va_end(args);

	while(1){
		asm("cli\n"
            "hlt\n");
	}
}
