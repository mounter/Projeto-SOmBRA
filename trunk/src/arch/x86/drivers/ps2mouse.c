#include <drivers/ps2mouse.h>
#include <system.h>
#include <stdio.h>
#include <x86/x86.h>
#include ISR_H
#include INTERRUPTS_H

// I/O addresses
#define PS2_PORT_DATA            0x60
#define PS2_PORT_CTRL            0x64

// control words
#define PS2_CTRL_WRITE_CMD       0x60
#define PS2_CTRL_WRITE_AUX       0xD4

// data words
#define PS2_CMD_DEV_INIT         0xA8
#define PS2_CMD_ENABLE_MOUSE     0x20
#define PS2_CMD_DISABLE_MOUSE    0xF5
#define PS2_CMD_RESET_MOUSE      0xFF

#define PS2_RES_ACK              0xFA
#define PS2_RES_RESEND           0xFE

// interrupts
#define INT_PS2_MOUSE            0x0C
#define INT_CASCADE              0x02

#define PACKET_SIZE              3

typedef struct {
   char status;
	char delta_x;
	char delta_y;
} ps2m_data; // mouse_data

static ps2m_data md_int;
static ps2m_data md_read;
static int in_read;

int ps2m_irq(uregs_t *regs)
{
	char c;
	static int next_input = 0;

	// read port
	c = inportb(PS2_PORT_DATA);

	// put port contents in the appropriate data member, according to
	// current cycle
	switch(next_input) {
		// status byte
		case 0:
			md_int.status = c;
		break;

		// x-axis change
		case 1:
			md_int.delta_x += c;
		break;

		// y-axis change
		case 2:
			md_int.delta_y += c;

			// check if someone is waiting to read data
			if (in_read) {
				// copy data to read structure, and release waiting process
				memcpy(&md_read, &md_int, sizeof(ps2m_data));
				memset(&md_int, 0, sizeof(ps2m_data));
				in_read = 0;
			} // if
		break;
	} // switch
	(*(unsigned char*)0xB8000)++;

	next_input = (next_input + 1) % 3;

	outportb(0xA0, 0x20); // Reinicia os dois controladores.
	outportb(0x20, 0x20);

	return 0;
}

inline void ps2m_wait(unsigned char  a_type)
{
	switch (a_type)
	{
		case 0:
			while (inportb(PS2_PORT_CTRL) & 0x02);
		return;

		case 1:
			while((inportb(PS2_PORT_CTRL) & 0x01) == 0);
		return;

		case 2:
			while(inportb(PS2_PORT_CTRL) & 0x3);
		return;
	}
}

inline void ps2m_write_cmd(unsigned char a_write)
{
	ps2m_wait(2);
	outportb(PS2_PORT_CTRL, PS2_CTRL_WRITE_CMD);
	ps2m_wait(0);
	outportb(PS2_PORT_DATA, a_write);
} // write_command_byte

inline void ps2m_write(unsigned char a_write)
{
	ps2m_wait(2);
	outportb(PS2_PORT_CTRL, PS2_CTRL_WRITE_AUX);
	ps2m_wait(0);
	outportb(PS2_PORT_DATA, a_write);
}

unsigned char ps2m_read()
{
	//Get's response from mouse
	ps2m_wait(1);
	return inportb(PS2_PORT_DATA);
}

void ps2m_init(void)
{
	/* This enables the mouse, called in main() */
	unsigned char _status;

	// init device driver
	memset(&md_int, 0, sizeof(ps2m_data));

	// empty Output buffer by reading from it
	if ( inportb(PS2_PORT_CTRL) & 0x1 )
		inportb(PS2_PORT_DATA);

	//Enable the auxiliary mouse device
	// enable auxilary device, IRQs and PS/2 mouse
	ps2m_write_cmd(PS2_CMD_DEV_INIT);
	ps2m_write_cmd(PS2_CMD_ENABLE_MOUSE);

	// controller should send ACK if mouse was detected
	if(ps2m_read() != PS2_RES_ACK) {
		printf("No PS/2 mouse found\n");
		return;
	} // if

	ps2m_write_cmd(PS2_CTRL_WRITE_CMD);

	printf("A PS/2 mouse has been successfully detected\n");

	// Depends on your OS!
	isr_vector_t ps2m_vec = { 0x8E, ps2m_irq };
	isr_set_handler(0x2C, &ps2m_vec);
	irq_enable(0x0C);

	printf("Mouse successfully initialized.\n");
}

