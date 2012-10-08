/*****************************************************************************
**                                                                          **
**	Arquivo: video.h                                                        **
**	Descrição: Driver de video                                              **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#ifndef VIDEO_H
#define VIDEO_H

#include <kernel/types.h>
#include <system.h>
#include <kernel.h>


enum ECOLOR{CBLACK,       CBLUE,        CGREEN, CCYAN,         CRED,
            CMAGENTA,     CBROWN,       CWHITE, CDARKGRAY,     CBRIGHTBLUE,
            CBRIGHTGREEN, CBRIGHTCYAN,  CPINK,  CBRIGHTMAGENTA,
            CYELLOW,      CBRIGHTWHITE};

typedef struct{
	uint16_t	col, row;
	uint16_t	color;
	uint16_t	*src;

	queue_t keys;

	wait_queue_t wait;
} console_t;

bool init_video();
void blink();
void clscr();
bool alt_con(uint16_t idcon);
bool gotoxy(uint32_t x, uint32_t y);
void putch(char c);
void puts(char *cp);
void nputs(char *cp, int len);
void set_color(uint8_t text, uint8_t background);
int get_con();
int8_t set_con(uint16_t idcon);
uint32_t getxy();
console_t *getConsole();

#endif
