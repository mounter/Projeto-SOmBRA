/*****************************************************************************
**                                                                          **
**	Arquivo: info.c                                                         **
**	Descrição: Informações sobre o SOmBRA                                   **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <info.h>

#include <kernel/types.h>
#include <version.h>

#define C1 set_color(CGREEN,CBLACK)
#define C2 set_color(CYELLOW,CBLACK)
#define C3 set_color(CBLUE,CBLACK)
#define C4 set_color(CBRIGHTWHITE, CBLACK)

void bem_vindo()
{
	clscr();
	C1;puts("         XXXXX       XXXXX       \n");
	C1;puts("      XXXX");C3; puts("      XXXX    ");C1;puts("XXXX      "); C4; printf(" %s[i386] - PM\n", VERSION_OSNAME);
	C1;puts("     XXXX"); C3; puts("    XXXXXXXX     ");C1;puts("XXXX    "); C4; printf(" Versao do kernel: %d.%d.%d%c\n", VERSION_MAJOR, VERSION_MIDDLE, VERSION_MINOR, VERSION_STATE);
	C1;puts("    XXXX"); C3; puts("     XXXXXXXXXX    "); C1; puts("XXXX   "); C4; printf(" Autor : %s \n", VERSION_AUTHOR);
	C1;puts("   XXXX"); C3; puts("        XXXXXXX      ");C1; puts("XXXX  "); C4; printf(" E-mail: %s\n", VERSION_EMAIL);
	C1;puts("   XXXXXX"); C3; puts("     XXXXXXX     "); C1; puts("XXXXXX  "); C4; printf(" (C) Copyright %s.\n", VERSION_COPY);
	C1;puts("   XXXXX"); C2; puts("XXXXXXXXXXXXXXXXXXX"); C1; puts("XXXXX  \n");
	C1;puts("    XXXXX"); C2; puts("XXXXXXXXXXXXXXXXX"); C1; puts("XXX     \n");
	C1;puts("      XXXX"); C2; puts("XXXXXXXXXXXXXXX"); C1; puts("XXX      \n");
	C1;puts("        XXX"); C2; puts("XXXXXXXXXXXXX"); C1; puts("XX        \n");
	C1;puts("          XXXXXXXXXXXXXX          \n");
	set_color(CWHITE, CBLACK);
}
