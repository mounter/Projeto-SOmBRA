#include <drivers/mouse.h>
#include <drivers/ps2.h>
#include <system.h>
#include <stdio.h>
#include <kernel/types.h>
#include <x86/isr.h>
#include <x86/interrupts.h>

uint8_t mouse_cycle=0;     //unsigned char
int8_t mouse_byte[5] = {0, 0, 0, 0, 0};    //signed char
int8_t mouse_x=0;         //signed char
int8_t mouse_y=0;         //signed char
int8_t mouse_z=0;         //signed char

// find wheel
unsigned char ps2_init_string[] = { 0xF3, 0xC8, 0xF3, 0x64, 0xF3, 0x50, 0x00 };
// find buttons
unsigned char ps2_init_string2[] = { 0xF3, 0xC8, 0xF3, 0xC8, 0xF3, 0x50, 0x00 };
// real init
unsigned char ps2_init_string3[] = { 0xF6, 0xE6, 0xF4, 0xF3, 0x64, 0xE8, 0x03, 0x00 };
// mouse type
unsigned char extwheel = 0xff, extbuttons = 0xff;

#define MOUSE_CHAR '+' // character displayed as the mouse pointer
#define MOUSE_FORC YELLOW // its (foreground) color
#define MOUSE_BACC BLACK // the background color
#define MOUSE_STEP 21

//Mouse functions
int mouse_handler(uregs_t *a_r) //struct regs *a_r (not used but just there)
{
   static unsigned char buf[4] = {0, 0, 0, 0};
	int dx, dy, dz;

 ps2_write(PS2_CTRL, 0xAD); // disable keyb

 buf[mouse_cycle++] = ps2_read();

 if ((mouse_cycle == 3) && ((extwheel == 0x03) || (extbuttons == 0x04))) {
	buf[mouse_cycle++] = ps2_read();
	mouse_cycle = 0;
 }

 mouse_byte[0] = buf[0] & 1;
 mouse_byte[1] = (buf[0] & 2) >> 1;
 mouse_byte[2] = (buf[0] & 4) >> 2;
 mouse_byte[3] = (buf[3] & 0x10) >> 4;
 mouse_byte[4] = (buf[3] & 0x20) >> 5;
 dx = (buf[0] & 0x10) ? buf[1] - 256 : buf[1];
 dy = (buf[0] & 0x20) ? -(buf[2] - 256) : -buf[2];
 dz = (buf[3] & 0x08) ? (buf[3]&7) - 8 : buf[3]&7;

 if (dx > 5 || dx < -5)
	dx *= 4;
 if (dy > 5 || dy < -5)
	dy *= 4;

 mouse_x += dx;
 mouse_y += dy;
 mouse_z += dz;

 if (mouse_x > 80) mouse_x = 80;
 if (mouse_x < 1) mouse_x = 1;
 if (mouse_y > 24) mouse_y = 24;
 if (mouse_y < 1) mouse_y = 1;

 ps2_write(PS2_CTRL, 0xAE); // enable keyb

 outportb(0xA0, 0x20); // eoi slave
 outportb(0x20, 0x20); // eoi master

 return 0;
}

inline int mouse_reset(void)
{
	ps2_write(PS2_CTRL, 0xD4);
	ps2_writeack(PS2_PORT, 0xFF);

	int status = ps2_read();
	status += ps2_read();
	
	return status == 0xAA;
}

inline void mouse_command(unsigned char *str_com)
{
	unsigned char *ch;

	for (ch=str_com; *ch; ch++)
	{
		ps2_write(PS2_CTRL, 0xD4);
		ps2_writeack(PS2_PORT, *ch);
	}
}

inline int mouse_getid(void)
{
	ps2_write(PS2_CTRL, 0xD4);
	ps2_writeack(PS2_PORT, 0xF2);
	return ps2_read();
}

inline void mouse_setrate(uint8_t rate)
{
	ps2_write(PS2_CTRL, 0xD4);
	ps2_writeack(PS2_PORT, 0xF3);
	ps2_write(PS2_CTRL, 0xD4);
	ps2_writeack(PS2_PORT, rate);
}

inline void mouse_enable(void)
{
	ps2_write(PS2_CTRL, 0xD4);
	ps2_writeack(PS2_PORT, 0xF4);

	ps2_write(PS2_CTRL, 0x20);
	unsigned char stat = ps2_read();
	stat |= 0x02; // turn on IRQ 12 generation
	stat &= 0xdf; // enable mouse serial clock line

	outportb(PS2_CTRL, 0x60); // write command byte
	outportb(PS2_PORT, stat);

	isr_vector_t mouse_vec = { 0x8E, mouse_handler };

	isr_set_handler(0x2C, &mouse_vec);
	irq_enable(12);
}

void mouse_install()
{
	unsigned char cmm[] = { 0xE8, 0x03, 0xE6, 0xF3, 0x28, 0};

	// Ativando a porta auxiliar
	ps2_write(PS2_CTRL, 0xA8);

	printf("Resetando mouse... ");
	if (mouse_reset())
		printf("OK\n");

	mouse_command(ps2_init_string);

	printf("Mouse ID: %i\n", mouse_getid());

	mouse_setrate(10);

	mouse_command(cmm);

	mouse_enable();
}
