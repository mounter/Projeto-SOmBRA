/*****************************************************************************
**                                                                          **
**	Arquivo: video.c                                                        **
**	Descrição: Driver de video                                              **
**	Autor: Mauro Joel Schütz                                                **
**	Data: 17/01/2006                                                        **
**                                                                          **
*****************************************************************************/
#include <x86-common/drivers/video.h>
//#include <kernel/queue.h>

console_t	consoles[MAX_CONSOLE];
console_t  	*console;
static uint8_t		dcon, acon;
static uint16_t		nrows;
static uint16_t		ncols;
static uint16_t		*video_mem;
static uint16_t		video_io;

// Protótipos
static void move_scr(console_t *con);

// Inicia e coleta informações sobre o controlador de video.
bool init_video()
{
	uint16_t 	i;
	uintptr_t 	addr;

	// Verifica se o monitor é de 16 cores ou monocromico
	if((inportb(0x3CC) & 0x01) != 0){
		addr      = 0xB8000L;
		video_io  = 0x3D4;
	}
	else{
		addr      = 0xB0000L;
		video_io  = 0x3B4;
	}

	video_mem = (uint16_t *)(addr);

	// Lê da BIOS o número o número de colunas e linhas.
	ncols = *(uint16_t *)(0x044A);
	nrows = *(uint8_t *)(0x0484) + 1;

	// Inicializa todos os consoles virtuais
	dcon = acon = 0;
	for (i=0;i<MAX_CONSOLE;i++){
		consoles[i].col			= consoles[i].row = 0;
		consoles[i].color		= (CWHITE<<8) + (CBLACK<<12);
		consoles[i].src			= video_mem + ((nrows * ncols) * i);
	}

	// Limpa a tela
	console = &consoles[0];
	clscr();

	return true;
}

void blink()
{
	(*(unsigned char *)console->src)++;
}

// Rola a tela para cima.
static void scroll(console_t *con)
{
	uint32_t t;

	// Rola a tela para cima
	for(t=0; t < ncols*(nrows-1); t++)
		con->src[t]=con->src[t+ncols];

	// Gera uma linha em branco
	for(; t < ncols*nrows; t++)
		con->src[t]=' ' | (con->color<<8);

  	con->row = nrows - 1;
	con->col = 0;
}

// Atualiza o cursor do console.
static void move_scr(console_t *con)
{
	uint32_t i;

	// calcula a posição do cursor
	i = (con->row * ncols + con->col) + (con->src - video_mem);

	if ((int)i > 0xFFFF)
        (*(unsigned char *)consoles[0].src)++;

    // Envia o comando para atualizar a posição do cursor
    outportb(video_io, 0x0E);
    outportb(video_io + 1, (uint8_t)((i >> 8) & 0xFF));
    outportb(video_io, 0x0F);
    outportb(video_io + 1, (uint8_t)(i & 0xFF));
}

// Atribui como será o cursor
void display_cursor(uint16_t c)
{
    outportb(video_io, 0x0A);
    outportb(video_io+1, ((1 << 5) | c) & 0x3F);

    outportb(video_io, 0x0B);
    outportb(video_io+1, (c >> 0x05) & 0x1F);
}

// Limpa a tela de um console virtual.
void clscr()
{
	uint16_t i;

	// Limpa a tela
	for (i=0;i<(ncols*nrows);i++){
		console->src[i] = (uint16_t)' '|console->color;
	}

	// Y = X = 0
	console->row = console->col = 0;

	// Atualiza o cursor
	move_scr(console);
}

// Altera o console virtual.
bool alt_con(uint16_t idcon)
{
	uint32_t	src;

	// verifica se é um console válido
	if (idcon>=MAX_CONSOLE){
		return false;
	}

	// src == endereço do console escolhido
	src = consoles[idcon].src - video_mem;

	// envia o comando para alterar o console
	outportb(video_io + 0, 12);
	outportb(video_io + 1, src >> 8);
	outportb(video_io + 0, 13);
	outportb(video_io + 1, src);

	// Atualiza o cursor
	move_scr(&consoles[idcon]);

	dcon=idcon;
	set_con(idcon);

	return true;
}

// Altera a posição do cursor
bool gotoxy(uint32_t x, uint32_t y)
{
	// checa se x e y são validos
	if ((x<=ncols)&&(y<=nrows)){
		console->row = y;
		console->col = x;
		// atualiza a posição
		move_scr(console);

		return true;
	}
	else{
		return false;
	}
}

// Envia um caracter para o console
void putch(char c)
{
	// Backspace
	if (c == '\b')
	{
        if (console->col == 0)
        {
            console->col = ncols - 1;
            console->row--;
        }
        else
            console->col--;
		console->src[(console->row*ncols)+console->col] = (unsigned char) ' ' | console->color;
	}
	// TAB
	else if (c == '\t')
	{
        console->col = (console->col + 8) & ~(8 - 1);
	}
	// Retorno de carro
	else if (c == '\r')
	{
        console->col = 0;
	}
	// Enter
	else if (c == '\n')
	{
        console->col = 0;
        console->row++;
	}
	// Texto
	else
	{
        console->src[(console->row*ncols)+console->col] = (unsigned char) c | console->color;
        console->col++;
	}

	// Checa o cursor
	if (console->col >= ncols)
	{
    		console->col = 0;
    		console->row++;
	}

    if (console->row >= nrows)
    {
        scroll(console);
    }

	// Move para nova posição
	if (console == &consoles[dcon])
	{
        move_scr(console);
	}
}

// Escreve um string na tela
void puts(char *cp)
{
	char *str;

	for (str = cp; *str; str++)
		putch(*str);
}

void nputs(char *cp, int len)
{
	int i = len;
	char *str;

	for (str = cp; i; str++, i--)
        putch(*str);
}

// Altera as cores dos caracteres e de fundo da tela
void set_color(uint8_t text, uint8_t background)
{
    console->color = (text<<8) + (background<<12);
}

// Pega o console atual.
int get_con()
{
	return dcon;
}

// Pega um console virtual pelo seu identificador
int8_t set_con(uint16_t idcon)
{
	int i = acon;

	if (idcon >= MAX_CONSOLE){
		return -1;
	}

	console = &consoles[idcon];
	acon = idcon;

    move_scr(console);

	return i;
}

/* Pega a posicão do cursor. */
uint32_t getxy()
{
	return (console->row*ncols)+console->col;
}
