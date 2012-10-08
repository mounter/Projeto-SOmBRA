/*****************************************************************************
**                                                                          **
**	Arquivo: ctype.c                                                        **
**	Descri��o: Macros de convers�o e classifica��o de caracteres            **
**	Autor: Mauro Joel Sch�tz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <ctype.h>

char _ctype[] =
{
	0x00,
/* 0 */	CT_CTL, CT_CTL, CT_CTL, CT_CTL,
	CT_CTL, CT_CTL, CT_CTL, CT_CTL,
/* 8 */	CT_CTL, CT_CTL | CT_WHT, CT_CTL | CT_WHT, CT_CTL | CT_WHT,
	CT_CTL | CT_WHT, CT_CTL | CT_WHT, CT_CTL, CT_CTL,
/* 16 */CT_CTL, CT_CTL, CT_CTL, CT_CTL,
	CT_CTL, CT_CTL, CT_CTL, CT_CTL,
/* 24 */CT_CTL, CT_CTL, CT_CTL, CT_CTL,
	CT_CTL, CT_CTL, CT_CTL, CT_CTL,
/* ' ' */CT_WHT | CT_SP, CT_PUN, CT_PUN, CT_PUN,
	CT_PUN, CT_PUN, CT_PUN, CT_PUN,
/* '(' */CT_PUN, CT_PUN, CT_PUN, CT_PUN,
	CT_PUN, CT_PUN, CT_PUN, CT_PUN,
/* '0' */CT_DIG, CT_DIG, CT_DIG, CT_DIG,
	CT_DIG, CT_DIG, CT_DIG, CT_DIG,
/* '8' */CT_DIG, CT_DIG, CT_PUN, CT_PUN,
	CT_PUN, CT_PUN, CT_PUN, CT_PUN,
/* '@' */CT_PUN, CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX,
	CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP | CT_HEX, CT_UP,
/* 'H' */CT_UP, CT_UP, CT_UP, CT_UP,
	CT_UP, CT_UP, CT_UP, CT_UP,
/* 'P' */CT_UP, CT_UP, CT_UP, CT_UP,
	CT_UP, CT_UP, CT_UP, CT_UP,
/* 'X' */CT_UP, CT_UP, CT_UP, CT_PUN,
	CT_PUN, CT_PUN, CT_PUN, CT_PUN,
/* '`' */CT_PUN, CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX,
	CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW | CT_HEX, CT_LOW,
/* h' */CT_LOW, CT_LOW, CT_LOW, CT_LOW,
	CT_LOW, CT_LOW, CT_LOW, CT_LOW,
/* 'p' */CT_LOW, CT_LOW, CT_LOW, CT_LOW,
	CT_LOW, CT_LOW, CT_LOW, CT_LOW,
/* 'x' */CT_LOW, CT_LOW, CT_LOW, CT_PUN,
	CT_PUN, CT_PUN, CT_PUN, CT_CTL,
/* 128 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 144 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 160 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 176 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 192 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 208 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 224 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 240 */0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
